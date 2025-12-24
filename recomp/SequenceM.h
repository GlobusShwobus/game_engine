#pragma once
/*
Does not guarantee safety against invariance on reallocation failure beyond conceptual requirements of
std::is_nothrow_move_constructible_v and std::is_nothrow_move_assignable_v.
Concepts should protect against invariance up front, but if in any case something was to happen mid way an operation
then invariance WILL happen.
*/

#include <memory>
#include "badUtility.h"
#include "BadExceptions.h"
namespace badEngine {

	template<typename T>
		requires IS_SEQUENCE_COMPATIBLE<T>
	class SequenceM {

		using type = SequenceM<T>;
		using value_type = T;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		class const_iterator;

		class iterator {
		public:
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using reference = T&;
			using pointer = T*;
			using iterator_category = std::random_access_iterator_tag;
			using self_type = iterator;

			constexpr iterator() = default;
			constexpr iterator(T* p) :ptr(p) {}

			constexpr reference operator*()noexcept {
				return *ptr;
			}
			constexpr pointer   operator->()noexcept {
				return ptr;
			}
			constexpr reference operator[](difference_type n)noexcept {
				return ptr[n];
			}

			constexpr const reference operator*() const noexcept {
				return *ptr;
			}
			constexpr const pointer   operator->() const noexcept {
				return ptr;
			}
			constexpr const reference operator[](difference_type n) const noexcept {
				return ptr[n];
			}

			constexpr self_type& operator++()noexcept {
				++ptr;
				return *this;
			}
			constexpr self_type  operator++(int)noexcept {
				self_type temp = *this;
				++ptr;
				return temp;
			}
			constexpr self_type& operator--()noexcept {
				--ptr;
				return *this;
			}
			constexpr self_type  operator--(int)noexcept {
				self_type temp = *this;
				--ptr;
				return temp;
			}

			constexpr self_type& operator+=(difference_type n)noexcept {
				ptr += n;
				return *this;
			}
			constexpr self_type& operator-=(difference_type n)noexcept {
				ptr -= n;
				return *this;
			}
			constexpr self_type  operator+(difference_type n)const noexcept {
				return ptr + n;
			}
			constexpr self_type  operator-(difference_type n)const noexcept {
				return ptr - n;
			}
			constexpr difference_type operator-(const self_type& rhs)const noexcept {
				return ptr - rhs.ptr;
			}

			constexpr bool operator==(const self_type& rhs)const noexcept {
				return ptr == rhs.ptr;
			}
			constexpr std::strong_ordering operator<=>(const self_type& rhs)const noexcept {
				return ptr <=> rhs.ptr;
			}

		private:
			friend class const_iterator;
			friend class SequenceM;
			T* ptr = nullptr;
		};

		class const_iterator {
		public:
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using reference = const T&;
			using pointer = const T*;
			using iterator_category = std::random_access_iterator_tag;
			using self_type = const_iterator;

			constexpr const_iterator() = default;
			constexpr const_iterator(T* p) :ptr(p) {}
			constexpr const_iterator(const iterator& rp) : ptr(rp.ptr) {}

			constexpr reference operator*()const noexcept {
				return *ptr;
			}
			constexpr pointer   operator->()const noexcept {
				return ptr;
			}
			constexpr reference operator[](difference_type n)const noexcept {
				return ptr[n];
			}

			constexpr self_type& operator++() noexcept {
				++ptr;
				return *this;
			}
			constexpr self_type  operator++(int) noexcept {
				self_type temp = *this;
				++ptr;
				return temp;
			}
			constexpr self_type& operator--() noexcept {
				--ptr;
				return *this;
			}
			constexpr self_type  operator--(int) noexcept {
				self_type temp = *this;
				--ptr;
				return temp;
			}

			constexpr self_type& operator+=(difference_type n) noexcept {
				ptr += n;
				return *this;
			}
			constexpr self_type& operator-=(difference_type n) noexcept {
				ptr -= n;
				return *this;
			}
			constexpr self_type  operator+(difference_type n)const noexcept {
				return ptr + n;
			}
			constexpr self_type  operator-(difference_type n)const noexcept {
				return ptr - n;
			}
			constexpr difference_type operator-(const self_type& rhs)const noexcept {
				return ptr - rhs.ptr;
			}

			constexpr bool operator==(const self_type& rhs)const noexcept {
				return ptr == rhs.ptr;
			}
			constexpr std::strong_ordering operator<=>(const self_type& rhs)const noexcept {
				return ptr <=> rhs.ptr;
			}

		private:
			friend class SequenceM;
			T* ptr = nullptr;
		};

		//internal ptr usage
		constexpr pointer impl_begin()const noexcept {
			return mArray;
		}
		constexpr pointer impl_end()const noexcept {
			return mArray + mSize;
		}

		//allocator/deallocator functions
		pointer alloc_memory(size_type count)const noexcept {
			assert(count < max_size() && "Memory limit reached");
			return static_cast<pointer>(::operator new(count * sizeof(value_type)));
		}
		constexpr void free_memory(pointer& mem)noexcept {
			::operator delete(mem);
			mem = nullptr;
		}
		constexpr void destroy(pointer obj)noexcept {
			obj->~T();
		}
		constexpr void destroy_objects(pointer begin, pointer end)noexcept {
			while (begin != end)
				begin++->~T();
		}
		/*
		DOES NOT SET SIZE!!!!!!!!!!
		must return type pointer and take arguments pointer and size_type
		later check out  std::function<pointer(pointer, size_type)> constructor...
		*/
		template<typename LambdaFunc>
		void reConstructAllocate(size_type newSize, LambdaFunc constructor) {
			//allocate new chunck of memory of size newSize and move [from -> to] data into it (always move) 
			pointer thisBegin = impl_begin();
			pointer thisEnd = impl_end();

			pointer destination = nullptr;
			pointer initialized = nullptr;

			try {
				destination = alloc_memory(newSize);
				initialized = constructor(destination, newSize);
			}
			catch (...) {
				destroy_objects(destination, initialized);
				free_memory(destination);
				throw;
			}

			//if curr, cleanup
			if (mArray) {
				destroy_objects(thisBegin, thisEnd);
				free_memory(mArray);
			}
			mArray = destination;
			mCapacity = newSize;
		}

		//growth math
		constexpr size_type growthFactor(size_type seed)const noexcept {
			return size_type(seed + (seed / mGrowthResistor) + 1);
		}

	public:

		using iterator = SequenceM::iterator;
		using const_iterator = SequenceM::const_iterator;

		//constructors
		constexpr SequenceM()noexcept { set_capacity(10); };
		SequenceM(size_type count)
			requires std::default_initializable<value_type>
		{
			if (count > IS_ZERO) {
				reConstructAllocate(count, [](pointer dest, size_type n) {
					return std::uninitialized_value_construct_n(dest, n);
					});
				mSize = count;
			}
		}
		SequenceM(size_type count, const_reference value)
			requires std::constructible_from<value_type, const_reference>
		{
			if (count > IS_ZERO) {
				reConstructAllocate(count, [&value](pointer dest, size_type n) {
					return std::uninitialized_fill_n(dest, n, value);
					});
				mSize = count;
			}
		}
		SequenceM(std::initializer_list<value_type> init)
			requires std::constructible_from<value_type, const_reference>//initializer_list members are const, can't move
		{
			const size_type size = init.size();
			if (size > IS_ZERO) {
				reConstructAllocate(size, [init](pointer dest, size_type n) {
					return std::uninitialized_copy(init.begin(), init.end(), dest);
					});
				mSize = size;
			}
		}
		SequenceM(const SequenceM& rhs)
			requires std::constructible_from<value_type, const_reference>
		{
			size_type size = rhs.size();
			if (size > IS_ZERO) {
				reConstructAllocate(size, [&rhs](pointer dest, size_type n) {
					return std::uninitialized_copy(rhs.begin(), rhs.end(), dest);
					});
				mSize = size;
			}
		}
		constexpr SequenceM(SequenceM&& rhs)noexcept {
			mArray = std::move(rhs.mArray);
			rhs.mArray = nullptr;

			mSize = std::move(rhs.mSize);
			rhs.mSize = 0;

			mCapacity = std::move(rhs.mCapacity);
			rhs.mCapacity = 0;

			mGrowthResistor = std::move(rhs.mGrowthResistor);
			rhs.mGrowthResistor = GROWTH_MEDIUM_RESIST;
		}
		SequenceM& operator=(SequenceM rhs)noexcept {
			//using swap idiom
			rhs.swap(*this);
			return *this;
		}
		SequenceM& operator=(std::initializer_list<value_type> ilist) {
			//using swap idiom
			SequenceM temp(ilist);
			temp.swap(*this);
			return *this;
		}
		~SequenceM()noexcept {//compiler didn't implicitly add noexcept (?) it should
			if (mArray) {
				destroy_objects(impl_begin(), impl_end());
				free_memory(mArray);
				mSize = 0;
				mCapacity = 0;
			}
		}
		//swap
		constexpr void swap(SequenceM& rhs)noexcept {
			std::swap(mArray, rhs.mArray);
			std::swap(mSize, rhs.mSize);
			std::swap(mCapacity, rhs.mCapacity);
			std::swap(mGrowthResistor, rhs.mGrowthResistor);
		}

		//iterator access
		constexpr iterator begin()noexcept {
			return  iterator(impl_begin());
		}
		constexpr iterator end()noexcept {
			return  iterator(impl_end());
		}
		constexpr const_iterator begin()const noexcept {
			return const_iterator(impl_begin());
		}
		constexpr const_iterator end()const noexcept {
			return const_iterator(impl_end());
		}
		constexpr const_iterator cbegin()const noexcept {
			return const_iterator(impl_begin());
		}
		constexpr const_iterator cend()const noexcept {
			return const_iterator(impl_end());
		}

		//basic access
		/*
		UNDEFINED BEHAVIOR CONDITIONS:
			if container is empty
		*/
		constexpr reference front() {
			assert(mSize > 0 && "front() called on empty container");
			return *begin();
		}
		/*
		UNDEFINED BEHAVIOR CONDITIONS:
			if container is empty
		*/
		constexpr const_reference front()const {
			assert(mSize > 0 && "front() called on empty container");
			return *begin();
		}
		/*
		UNDEFINED BEHAVIOR CONDITIONS:
			if container is empty
		*/
		constexpr reference back() {
			assert(mSize > 0 && "back() called on empty container");
			return mArray[mSize - 1];
		}
		/*
		UNDEFINED BEHAVIOR CONDITIONS:
			if container is empty
		*/
		constexpr const_reference back()const {
			assert(mSize > 0 && "back() called on empty container");
			return mArray[mSize - 1];
		}
		/*
		UNDEFINED BEHAVIOR CONDITIONS:
			if index is not in the range of [begin -> end]
		*/
		constexpr reference operator[](size_type index) {
			assert(index < mSize && "operator[] access with out of range index");
			return mArray[index];
		}
		/*
		UNDEFINED BEHAVIOR CONDITIONS:
			if index is not in the range of [begin -> end]
		*/
		constexpr const_reference operator[](size_type index)const {
			assert(index < mSize && "operator[] access with out of range index");
			return mArray[index];
		}
		/*
		THROWS EXCEPTION:
			if element is not in range of [begin -> end] throws type BadException
		*/
		reference at(size_type index) {
			if (index >= mSize)
				throw BadException(__FILE__, __LINE__, "Out of Range", "at() access with out of range index");
			return mArray[index];
		}
		/*
		THROWS EXCEPTION:
			if element is not in range of [begin -> end] throws type BadException
		*/
		const_reference at(size_type index)const {
			if (index >= mSize)
				throw BadException(__FILE__, __LINE__, "Out of Range", "at() access with out of range index");
			return mArray[index];
		}

		//number of initalized members
		constexpr size_type size()const noexcept {
			return mSize;
		}
		//number of maximum initalizable members for the system
		constexpr size_type max_size()const noexcept {
			return std::numeric_limits<size_type>::max() / sizeof(value_type);
		}
		//number of objects that can be initalized before reallocation
		constexpr size_type capacity() const noexcept {
			return mCapacity;
		}
		//returns true if size is zero
		constexpr bool isEmpty()const noexcept {
			return mSize == IS_ZERO;
		}

		//setters for vector growth resistors
		constexpr void set_growth_resist_high()noexcept {
			mGrowthResistor = GROWTH_HIGH_RESIST;
		}
		constexpr void set_growth_resist_medium()noexcept {
			mGrowthResistor = GROWTH_MEDIUM_RESIST;
		}
		constexpr void set_growth_resist_low()noexcept {
			mGrowthResistor = GROWTH_LOW_RESIST;
		}
		constexpr void set_growth_resist_negative() noexcept {
			mGrowthResistor = GROWTH_NEGATIVE_RESIST;
		}

		//clear, capacity unchanged
		constexpr void clear()noexcept {
			if (!isEmpty()) {
				destroy_objects(impl_begin(), impl_end());
				mSize = 0;
			}
		}
		//copies elements
		void push_back(const_reference value)
			requires std::constructible_from<value_type, const_reference>
		{
			emplace_back(value);
		}
		//moves elements
		void push_back(value_type&& value)
			requires std::constructible_from<value_type, value_type&&>
		{
			emplace_back(std::move(value));
		}
		//creates elements in place using any constructor
		template<typename... Args>
			requires std::constructible_from<value_type, Args...>
		void emplace_back(Args&&... args)
		{
			//if at capacity, reallocate with extra memory
			if (mSize == mCapacity) {
				set_capacity(growthFactor(mCapacity));
			}
			new(mArray + mSize)value_type(std::forward<Args>(args)...);
			++mSize;
		}
		//pop back, overload of erase(first, last)
		constexpr void pop_back()noexcept {
			if (!isEmpty()) {
				erase(end() - 1, end());
			}
		}
		//erase, overlaod of erase(first, last)
		void erase(const_iterator pos)
			requires std::is_nothrow_move_assignable_v<value_type>
		{
			erase(pos, pos + 1);
		}
		/*
		erases elements
		UNDEFIEND BEHAVIOR CONDITIONS:
			if [first -> last) is not in the range of [begin -> end]
		*/
		void erase(const_iterator first, const_iterator last)
			requires std::is_nothrow_move_assignable_v<value_type>
		{
			if (first == last)return;

			pointer targetBegin = first.ptr;
			pointer targetEnd = last.ptr;
			pointer thisBegin = impl_begin();
			pointer thisEnd = impl_end();

			assert(thisBegin <= targetBegin && thisEnd >= targetEnd && targetBegin <= targetEnd && "position invalidation");
			//get distance
			size_type destroy_size = static_cast<size_type>(targetEnd - targetBegin);

			//move objects
			for (; targetEnd != thisEnd; ++targetBegin, ++targetEnd) {
				*targetBegin = std::move(*targetEnd);
			}
			mSize -= destroy_size;
			destroy_objects(impl_end(), thisEnd);
		}
		//swaps elements and pops, overlaod of swap_with_last_erase(first, last)
		void swap_with_last_erase(const_iterator pos)
		{
			if (pos != end()) {
				swap_with_last_erase(pos, pos + 1);
			}
		}
		/*
		swaps last element with pos element then pops
		UNDEFIEND BEHAVIOR CONDITIONS:
			if [first -> last) is not in the range of [begin -> end]
		*/
		void swap_with_last_erase(const_iterator first, const_iterator last)
			requires std::is_nothrow_move_assignable_v<value_type>
		{
			if (first == last)return;
			pointer destination = first.ptr;
			pointer targetEnd = last.ptr;
			pointer thisEnd = impl_end();

			assert(impl_begin() <= destination && thisEnd >= targetEnd && destination <= targetEnd && "position invalidation");
			//get distance
			size_type destroy_size = static_cast<size_type>(targetEnd - destination);

			pointer src = thisEnd - 1;

			for (size_type i = 0; i < destroy_size; ++i) {
				*destination++ = std::move(*src--);
			}
			mSize -= destroy_size;
			destroy_objects(impl_end(), thisEnd);
		}
		/*
		reserves memory
		if however input parameter capacity is less than current capacity, it will destroy anything outside of mSize
		*/
		void set_capacity(size_type n)
			requires std::is_nothrow_move_assignable_v<value_type>
		{
			pointer oldBegin = impl_begin();
			size_type moveCount = bad_minV(mSize, n);
			reConstructAllocate(n, [oldBegin, moveCount](pointer dest, size_type size) {
				return std::uninitialized_move(oldBegin, oldBegin + moveCount, dest);
				}
			);
			mSize = moveCount;
		}
		//shrinks to current size
		void shrink_to_fit() {
			set_capacity(mSize);
		}
	private:
		//member variables
		pointer                  mArray = nullptr;
		size_type                mSize = 0;
		size_type                mCapacity = 0;

		float                    mGrowthResistor = GROWTH_MEDIUM_RESIST;

		//universal variables
		static constexpr size_type IS_ZERO = 0;
		static constexpr float     GROWTH_HIGH_RESIST = 4.0f;
		static constexpr float     GROWTH_MEDIUM_RESIST = 2.0f;
		static constexpr float     GROWTH_LOW_RESIST = 1.0f;
		static constexpr float     GROWTH_NEGATIVE_RESIST = 0.80f;
	};
}