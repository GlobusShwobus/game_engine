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

		class Iterator {
		public:
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using reference = T&;
			using pointer = T*;
			using iterator_category = std::random_access_iterator_tag;
			using self_type = Iterator;
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

			constexpr Iterator() = default;
			constexpr Iterator(pointer p) :ptr(p) {}
			constexpr pointer base()const noexcept {
				return ptr;
			}
		private:
			pointer ptr = nullptr;
		};
		class Const_Iterator {
		public:
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using reference = const T&;
			using pointer = const T*;
			using iterator_category = std::random_access_iterator_tag;
			using self_type = Const_Iterator;
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

			constexpr Const_Iterator() = default;
			constexpr Const_Iterator(pointer p) :ptr(p) {}
			constexpr Const_Iterator(const Iterator& rp) : ptr(rp.base()) {}
			constexpr pointer base()const noexcept {
				return ptr;
			}
		private:
			pointer ptr = nullptr;
		};

	public:

		using iterator = Iterator;
		using const_iterator = Const_Iterator;

		//iterator access
		constexpr iterator begin()noexcept {
			return  mArray;
		}
		constexpr iterator end()noexcept {
			return  mArray + mUsableSize;
		}
		constexpr const_iterator begin()const noexcept {
			return  mArray;
		}
		constexpr const_iterator end()const noexcept {
			return  mArray + mUsableSize;
		}
		constexpr const_iterator cbegin()const noexcept {
			return  mArray; 
		}
		constexpr const_iterator cend()const noexcept {
			return  mArray + mUsableSize;
		}

		//meta access
		constexpr pointer data() noexcept {
			return  mArray; 
		}
		constexpr const_pointer data() const noexcept {
			return  mArray; 
		}

		//basic access
		constexpr reference front() {
			assert(mUsableSize > 0 && "front() called on empty container");
			return mArray[0];
		}
		constexpr const_reference front()const {
			assert(mUsableSize > 0 && "front() called on empty container");
			return mArray[0];
		}
		constexpr reference back() {
			assert(mUsableSize > 0 && "back() called on empty container");
			return mArray[mUsableSize - 1];
		}
		constexpr const_reference back()const {
			assert(mUsableSize > 0 && "back() called on empty container");
			return mArray[mUsableSize - 1];
		}
		constexpr reference operator[](size_type index) {
			assert(index < mUsableSize && "operator[] access with out of range index");
			return mArray[index];
		}
		constexpr const_reference operator[](size_type index)const { 
			assert(index < mUsableSize && "operator[] access with out of range index");
			return mArray[index]; 
		}
		reference at(size_type index) {
			if (index >= mUsableSize)
				throw BadException(__FILE__, __LINE__, "at() access with out of range index");
			return mArray[index];
		}
		const_reference at(size_type index)const {
			if (index >= mUsableSize)
				throw BadException(__FILE__, __LINE__, "at() access with out of range index");
			return mArray[index];
		}

		//meta data
		constexpr size_type size()const noexcept { 
			return mUsableSize;
		}
		constexpr size_type capacity() const noexcept {
			return mCapacity;
		}
		constexpr size_type storage_left() const noexcept {
			return mCapacity - mUsableSize;
		}
		constexpr bool isEmpty()const noexcept { 
			return mUsableSize == EMPTY_GUARD;
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

	private:
		//implementation critical functions

		//memory handlers
		constexpr size_type max_size()const noexcept {
			return std::numeric_limits<size_type>::max() / sizeof(value_type);
		}
		pointer alloc_memory(size_type count) {
			if (count > max_size())
				throw BadException(__FILE__, __LINE__, "max size overflow error");
			return static_cast<pointer>(::operator new(count * sizeof(value_type)));
		}
		pointer free_memory(pointer mem)noexcept {
			::operator delete(mem);
			return nullptr;
		}
		void deconstruct_objects(pointer begin, pointer end)noexcept {
			std::destroy(begin, end);
		}

		//internal ptr usage
		pointer pBegin_mem()noexcept {
			return mArray; 
		}
		pointer pEnd_usable()noexcept {
			return mArray + mUsableSize; 
		}
		pointer pEnd_constructed()noexcept {
			return mArray + mConstructedSize;
		}

		//growth math
		constexpr size_type growthFactor(size_type seed)const noexcept {
			return size_type(seed + (seed / mGrowthResistor) + 1);
		}
		//allocator method, on failure guaranteed clean up and nullptr
		template<PtrPredicateFunc<pointer, size_type> ConstructorPred>
		pointer alloc_and_construct(ConstructorPred constructor, size_type size) {
			pointer destination = nullptr;
			pointer initialized = nullptr;

			try {
				destination = alloc_memory(size);
				initialized = constructor(destination, size);
			}
			catch (...) {
				deconstruct_objects(destination, initialized);
				destination = free_memory(destination);
				throw;
			}

			return destination;
		}
		//reallocator method
		void reallocate(size_type newSize) {
			//allocate new chunck of memory of size newSize and move [from -> to] data into it (always move) 
			pointer from = pBegin_mem();
			pointer to = pEnd_usable();
			pointer constructedMem = alloc_and_construct([from, to](pointer dest, size_type n) {
				return std::uninitialized_move(from, to, dest);
				}, newSize);
			//destroy current
			deconstruct_objects(pBegin_mem(), pEnd_constructed());
			mArray = free_memory(mArray);
			mArray = constructedMem;
			mCapacity = newSize;

			//reallocate should be called until usable end not constructed end, 
			//because if reallocate, might aswell clean up and dangling shit
			//OTHERWISE USABLE SIZE DOES NOT CHANGE
			mConstructedSize = mUsableSize;
		}
		//basic setter
		constexpr void basic_construction_setter(pointer data, size_type size) noexcept {
			mArray = data;
			mUsableSize = size;
			mConstructedSize = size;
			mCapacity = size;
			mGrowthResistor = GROWTH_MEDIUM_RESIST;
		}
	public:
		//constructors
		constexpr SequenceM()noexcept = default;
		SequenceM(size_type count)
			requires std::default_initializable<value_type>
		{
			if (count > EMPTY_GUARD) {
				pointer constructedMemory = alloc_and_construct([](pointer dest, size_type n) {
					return std::uninitialized_value_construct_n(dest, n);
					}, count);

				basic_construction_setter(constructedMemory, count);
			}
		}
		SequenceM(size_type count, const_reference value)
			requires std::copyable<value_type>	
		{
			if (count > EMPTY_GUARD) {
				pointer constructedMemory = alloc_and_construct([&value](pointer dest, size_type n) {
					return std::uninitialized_fill_n(dest, n, value);
					}, count);

				basic_construction_setter(constructedMemory, count);
			}
		}
		SequenceM(std::initializer_list<value_type> init)
			requires std::copyable<value_type>//initializer_list members are const, can't move
		{
			const size_type size = init.size();
			if (size > EMPTY_GUARD) {
				pointer constructedMemory = alloc_and_construct([init](pointer dest, size_type n) {
					return std::uninitialized_copy(init.begin(), init.end(), dest);
					}, size);

				basic_construction_setter(constructedMemory, size);
			}
		}
		SequenceM(const SequenceM& rhs)
			requires std::copyable<value_type>
		{
			size_type size = rhs.size();
			if (size > EMPTY_GUARD) {
				pointer constructedMemory = alloc_and_construct([&rhs](pointer dest, size_type n) {
					return std::uninitialized_copy(rhs.begin(), rhs.end(), dest);
					}, size);

				basic_construction_setter(constructedMemory, size);
			}
		}
		constexpr SequenceM(SequenceM&& rhs)noexcept {
			mArray = rhs.mArray;
			rhs.mArray = nullptr;

			mUsableSize = rhs.mUsableSize;
			rhs.mUsableSize = 0;

			mConstructedSize = rhs.mConstructedSize;
			rhs.mConstructedSize = 0;

			mCapacity = rhs.mCapacity;
			rhs.mCapacity = 0;

			mGrowthResistor = rhs.mGrowthResistor;
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
			reset();
		}
		//swap
		constexpr void swap(SequenceM& rhs)noexcept {
			std::swap(mArray, rhs.mArray);
			std::swap(mUsableSize, rhs.mUsableSize);
			std::swap(mConstructedSize, rhs.mConstructedSize);
			std::swap(mCapacity, rhs.mCapacity);
			std::swap(mGrowthResistor, rhs.mGrowthResistor);
		}
		//clear does not actually clear, it moves internal counter
		void clear()noexcept {
			mUsableSize = 0;
		}
		//actually clears but nukes everything
		void reset()noexcept {
			if (mArray) {
				//since we save time elsewhere on deconstructing, resetting everything needs to remember to clear everything
				deconstruct_objects(pBegin_mem(), pEnd_constructed());
				//free mem
				mArray = free_memory(pBegin_mem());
				mUsableSize = 0;
				mConstructedSize = 0;
				mCapacity = 0;
				mGrowthResistor = GROWTH_MEDIUM_RESIST;
			}
		}
		//copies elements
		void push_back(const value_type& value)
			requires std::copyable<value_type>
		{
			//if at capacity, reallocate with extra memory
			if (mConstructedSize == mCapacity)
				reallocate(growthFactor(mCapacity));
			//current end point
			pointer slot = pEnd_usable();
			//if there is a constructed but depricated slot, copy assign into it
			if ((mConstructedSize - mUsableSize) > EMPTY_GUARD) {
				*slot = value;
			}
			//if there aren't any slots remaining, need official constructor
			else {
				std::construct_at(slot, value);
				//if we constructed a new thing, incr constructed counter
				++mConstructedSize;
			}
			//in any case after adding incr usable size
			++mUsableSize;
		}
		//creates elements and or accepts moving as well
		template<typename... Args>
		void emplace_back(Args&&... args)
			requires std::constructible_from<value_type, Args&&...>
		{
			//if at capacity, reallocate with extra memory
			if (mConstructedSize == mCapacity)
				reallocate(growthFactor(mCapacity));
			//current end point
			pointer slot = pEnd_usable();
			//if there is a constructed but depricated slot must destroy it first
			if ((mConstructedSize - mUsableSize) > EMPTY_GUARD) {
				if constexpr (!std::is_trivially_destructible_v<value_type>) {
					std::destroy_at(slot);
				}
			}
			else { //otherwise need to increment mConstructedSize bookkeeping
				++mConstructedSize;
			}
			//construct the element
			std::construct_at(slot, std::forward<Args>(args)...);
			//in any case after adding incr usable size
			++mUsableSize;
		}
		//does nothing besides decrementing counter
		constexpr void pop_back()noexcept {
			if (!isEmpty())
				--mUsableSize;
		}
		//basically erase
		void remove_preserved_order(iterator pos)
			requires std::is_nothrow_move_assignable_v<value_type>
		{
			pointer target = pos.base();
			pointer begin = pBegin_mem();
			pointer end = pEnd_usable();

			assert(target >= begin && target < end && "position out of range");
			//from, till, into... can theoretically throw std::bad_alloc but concept requirements should protect against most faults up front
			std::move(target + 1, end, target);
			//reminder: constructed object counter DOES NOT CHANGE
			--mUsableSize;
		}
		//basically erase
		void remove_preserved_order(iterator first, iterator last) 
			requires std::is_nothrow_move_assignable_v<value_type>
		{
			pointer targetBegin = first.base();
			pointer targetEnd = last.base();
			pointer arrayBegin = pBegin_mem();
			pointer arrayEnd = pEnd_usable();
			assert(arrayBegin <= targetBegin && targetBegin <= targetEnd && targetEnd <= arrayEnd && "position invalidation");

			//if range is 0 then there is nothing to remove (MAY BE FLAWED LOGIC)
			if (targetBegin == targetEnd) return;

			//if target end is not arrays end then move the elements from targets end until arry end into target begin (cutting of the middle chunck)	
			//otherwise, since we skip deconstruction, just skip the step
			if (targetEnd != arrayEnd)
				std::move(targetEnd, arrayEnd, targetBegin);
			//depricate usable size by the amount difference of target end and begin
			//reminder: constructed object counter DOES NOT CHANGE
			mUsableSize -= (targetEnd - targetBegin);
		}
		//swaps last element with pos element, meaning it does not preserve order
		void remove_unpreserved_order(iterator pos)
			requires std::is_nothrow_move_assignable_v<value_type>
		{
			pointer target = pos.base();
			pointer begin = pBegin_mem();
			pointer end = pEnd_usable();
			
			assert(begin <= target && target < end && "position invalidation");

			//end is one off the end so first update the end
			--end;
			//move the end into target
			*target = std::move(*end);
			//reminder: constructed object counter DOES NOT CHANGE
			--mUsableSize;
		}
		//reserves more memory if condition is met
		void reserve(size_type newCapacity) {
			//note: when reserving, any objects between usable and constructed get destroyed, which is good
			if (newCapacity > mCapacity)
				reallocate(newCapacity);
		}
		//shrinks to current size
		void shrink_to_fit() {
			if (mCapacity > mUsableSize)
				reallocate(mUsableSize);
		}
	private:
		//member variables
		pointer                  mArray = nullptr;
		size_type                mUsableSize = 0;
		size_type                mConstructedSize = 0;
		size_type                mCapacity = 0;

		float                    mGrowthResistor = GROWTH_MEDIUM_RESIST;

		//universal variables
		static constexpr size_type EMPTY_GUARD = 0;
		static constexpr float     GROWTH_HIGH_RESIST = 4.0f;
		static constexpr float     GROWTH_MEDIUM_RESIST = 2.0f;
		static constexpr float     GROWTH_LOW_RESIST = 1.0f;
		static constexpr float     GROWTH_NEGATIVE_RESIST = 0.80f;
	};
}