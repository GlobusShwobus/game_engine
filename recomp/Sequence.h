#pragma once

#include <cassert>
#include <algorithm>
#include <memory>
#include <concepts>
#include <type_traits>

namespace badEngine {
	template <typename T>
	concept SequenceTypeTrait = std::semiregular<T> && !std::is_const_v<T>;

	template <typename T> requires SequenceTypeTrait<T>
	class Sequence {
	private:
		//forward declares
		class Iterator;
		class Const_Iterator;
	public:
		//type names
		using type = Sequence<T>;
		using value_type = T;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		using iterator = Iterator;
		using const_iterator = Const_Iterator;
	private:
		//private members
		pointer memAlloc(size_type amount) {
			void* bytes = ::operator new(amount * sizeof(value_type));
			return static_cast<pointer>(bytes);
		}
		void memFree() {
			if (array) {
				::operator delete(array, cap * sizeof(value_type));
				array = nullptr;
				cap = 0;
			}
		}
		constexpr void reset()noexcept {
			array = nullptr;
			cap = 0;
			mSize = 0;
		}
		constexpr pointer raw_begin()const {
			return array;
		}
		constexpr pointer raw_end()const {
			return array + mSize;
		}
		constexpr size_type growthFactor(size_type input)const {
			return input + (input / 2) + 1;
		}
		void ifGrow() {
			if (mSize == cap)
				reAlloc(growthFactor(cap));
		}
		void reAlloc(size_type newCap) {
			pointer newArray = memAlloc(newCap);
			std::uninitialized_move(raw_begin(), raw_end(), newArray);
			std::destroy(raw_begin(), raw_end());
			memFree();
			array = newArray;
			cap = newCap;
		}
		pointer eraseImpl(const_iterator pos) {
			assert(pos >= begin() && pos < end());
			pointer ppos = const_cast<pointer>(pos.base());
			std::move(ppos + 1, raw_end(), ppos);
			array[mSize - 1].~T();
			--mSize;

			return (ppos == raw_end()) ? raw_end() : ppos;
		}
		pointer eraseRangeImpl(const_iterator first, const_iterator last) {
			assert(first >= begin() && first <= last && last <= end());

			pointer pfirst = const_cast<pointer>(first.base());
			pointer plast = const_cast<pointer>(last.base());

			if (pfirst == plast)//if first and last is same element, then no op
				return pfirst;

			pointer end = raw_end();

			if (plast == end) {//if last is one off the end, then erase start to end
				std::destroy(pfirst, end);
				mSize = pfirst - raw_begin();
				return raw_end();
			}
			//else if last is not one off the end
			std::move(plast, end, pfirst);//move from plast, until one of the end, into first++

			//math: size 25: destroy elem 5 to 10:: 5+(25-10) = 20, destroy 20 to 25. then mSize-=(25-20), reduce by 5
			pointer destroyBegin = pfirst + (end - plast);
			std::destroy(destroyBegin, end);
			mSize -= (end - destroyBegin);

			return pfirst;
		}
		pointer removeImpl(const_iterator pos) {
			assert(pos >= begin() && pos < end());

			pointer dest = const_cast<pointer>(pos.base());
			pointer last = raw_end() - 1;

			*dest = std::move(*last);
			std::destroy_at(last);

			--mSize;
			return dest;
		}
		void uninitCopy(const_pointer  fromBegin, const_pointer  fromEnd) {
			size_type count = static_cast<size_type>(fromEnd - fromBegin);
			array = memAlloc(count);
			mSize = count;
			cap = count;

			std::uninitialized_copy(fromBegin, fromEnd, array);
		}
	public:
		//CONSTRUCTORS
		Sequence()noexcept :array(nullptr), mSize(0), cap(0) {}
		Sequence(std::initializer_list<value_type> init) {
			if (init.size() > 0) {
				uninitCopy(init.begin(), init.end());
			}
		}
		explicit Sequence(size_type count) {
			if (count > 0) {
				array = memAlloc(count);
				cap = count;
				mSize = count;

				std::uninitialized_value_construct_n(array, count);//envokes T()
			}
		}
		Sequence(size_type count, const_reference value) {
			if (count > 0) {
				array = memAlloc(count);
				cap = count;
				mSize = count;

				std::uninitialized_fill_n(array, count, value);
			}
		}
		Sequence(const Sequence& rhs) {
			if (rhs.array) {
				uninitCopy(rhs.raw_begin(), rhs.raw_end());
			}
		}
		constexpr Sequence(Sequence&& rhs)noexcept :array(rhs.array), cap(rhs.cap), mSize(rhs.mSize) {
			rhs.reset();
		}
		Sequence& operator=(const Sequence& rhs) {
			if (this != &rhs) {
				clear();
				memFree();

				if (rhs.array) {
					uninitCopy(rhs.raw_begin(), rhs.raw_end());
				}
				else {
					reset();
				}
			}
			return *this;
		}
		Sequence& operator=(Sequence&& rhs)noexcept {
			if (this != &rhs) {
				clear();
				memFree();

				array = rhs.array;
				mSize = rhs.mSize;
				cap = rhs.cap;

				rhs.reset();
			}
			return *this;
		}
		Sequence& operator=(std::initializer_list<value_type> ilist) {
			clear();
			memFree();

			if (ilist.size() > 0) {
				uninitCopy(ilist.begin(), ilist.end());
			}
			else {
				reset();
			}
			return *this;
		}

		~Sequence() {
			clear();
			memFree();
		}
		//#################################################

		//ACCESS
		constexpr reference operator[](size_type index) {
			return at(index);
		}
		constexpr const_reference operator[](size_type index)const {
			return at(index);
		}
		constexpr reference at(size_type pos) {
			assert(pos < mSize);
			return array[pos];
		}
		constexpr const_reference at(size_type pos)const {
			assert(pos < mSize);
			return array[pos];
		}
		constexpr pointer data()noexcept {
			return array;
		}
		constexpr const_pointer data()const noexcept {
			return array;
		}
		constexpr iterator       begin()noexcept { return { array }; }
		constexpr iterator       end()noexcept { return { array + mSize }; }
		constexpr const_iterator begin()const noexcept { return { array }; }
		constexpr const_iterator end()const noexcept { return { array + mSize }; }
		constexpr const_iterator cbegin()const noexcept { return { array }; }
		constexpr const_iterator cend()const noexcept { return { array + mSize }; }
		//#################################################

		//MODIFICATION
		void emplace_back(const_reference value) {
			ifGrow();
			std::construct_at(raw_end(), value);
			++mSize;
		}
		void emplace_back(T&& value) {
			ifGrow();
			std::construct_at(raw_end(), std::move(value));
			++mSize;
		}
		template<typename... Args>
		void emplace_back(Args&&... args) {
			ifGrow();
			std::construct_at(raw_end(), std::forward<Args>(args)...);
			++mSize;
		}
		void pop_back() {
			assert(mSize > 0);
			array[mSize - 1].~T();
			--mSize;
		}
		void clear()noexcept {
			if (mSize == 0)
				return;
			std::destroy(raw_begin(), raw_end());
			mSize = 0;
		}
		void resize(size_type count) {
			if (count == mSize) {
				return;
			}
			//if count is less than current capacity, then no need to reallocate
			if (count <= cap) {
				//if count is less than size, cut off the end, else add default constructed elements
				if (count < mSize) {
					std::destroy(raw_begin() + count, raw_end());
				}
				else {
					std::uninitialized_default_construct(raw_end(), raw_begin() + count);//same as vector, if resize sequence<int>, extra ints are there but not initalized (not zeroed)
				}
				mSize = count;
				return;
			}
			//need reallocation and since count > cap, its always greater than mSize
			size_type newCap = growthFactor(count);
			pointer newArray = memAlloc(newCap);

			std::uninitialized_move(raw_begin(), raw_end(), newArray);
			std::uninitialized_default_construct(newArray + mSize, newArray + count);

			std::destroy(raw_begin(), raw_end());
			memFree();
			array = newArray;
			cap = newCap;
			mSize = count;
		}
		template <typename UnaryPred>
		iterator remove(UnaryPred predicate) {

			pointer last = raw_end() - 1;
			pointer current = raw_begin();

			//if predicate is true, move last element to current then move the last pointer
			while (current <= last) {

				if (predicate(*current)) {
					*current = std::move(*last);
					--last;
				}
				else {
					++current;
				}
			}
			size_type newSize = last - raw_begin() + 1;

			//bulk delete everything from last to end then apply new size
			if (newSize < mSize) {
				std::destroy(last + 1, raw_end());
				mSize = newSize;
			}

			return end();
		}
		const_iterator remove(const_iterator pos) { return { removeImpl(pos) }; }
		iterator       remove(iterator pos) { return { removeImpl(pos) }; }
		const_iterator erase(const_iterator pos) { return { eraseImpl(pos) }; }
		iterator       erase(iterator pos) { return { eraseImpl(pos) }; }
		const_iterator erase(const_iterator first, const_iterator last) { return { eraseRangeImpl(first, last) }; }
		iterator       erase(iterator first, iterator last) { return { eraseRangeImpl(first, last) }; }

		constexpr void swap(Sequence& rhs)noexcept {
			pointer tempArr = array;
			array = rhs.array;
			rhs.array = tempArr;

			size_type tempSize = mSize;
			mSize = rhs.mSize;
			rhs.mSize = tempSize;

			size_type tempCap = cap;
			cap = rhs.cap;
			rhs.cap = tempCap;
		}

		//#################################################


		//CAPACITY
		constexpr bool      isEmpty()const noexcept { return mSize == 0; }
		constexpr size_type size()const noexcept { return mSize; }
		constexpr size_type capacity()const noexcept { return cap; }

		void reserve(size_type newCap) {
			if (newCap > cap) {
				reAlloc(newCap);
			}
		}
		void shrinkToFit() {
			if (cap > mSize) {
				reAlloc(mSize);
			}
		}
		//#################################################
	private:
		//members
		pointer array = nullptr;
		size_type mSize = 0;
		size_type cap = 0;
	};

	template <typename SequenceType>
	constexpr bool operator==(const Sequence<SequenceType>& lhs, const Sequence<SequenceType>& rhs) {
		if (lhs.size != rhs.size) {
			return false;
		}
		return std::equal(lhs.begin(), lhs.end(), rhs.begin());
	}
	template <typename SequenceType>
	constexpr bool operator!=(const Sequence<SequenceType>& lhs, const Sequence<SequenceType>& rhs) {
		return !(lhs == rhs);
	}
	template <typename SequenceType>
	constexpr bool operator<(const Sequence<SequenceType>& lhs, const Sequence<SequenceType>& rhs) {
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}
	template <typename SequenceType>
	constexpr bool operator>(const Sequence<SequenceType>& lhs, const Sequence<SequenceType>& rhs) {
		return rhs < lhs;
	}
	template <typename SequenceType>
	constexpr bool operator<=(const Sequence<SequenceType>& lhs, const Sequence<SequenceType>& rhs) {
		return !(rhs < lhs);
	}
	template <typename SequenceType>
	constexpr bool operator>=(const Sequence<SequenceType>& lhs, const Sequence<SequenceType>& rhs) {
		return !(lhs < rhs);
	}

	template<typename SequenceType>
	constexpr void swap(Sequence<SequenceType>& lhs, Sequence<SequenceType>& rhs)noexcept {
		lhs.swap(rhs);
	}

	template<typename T> requires SequenceTypeTrait<T>
	class Sequence<T>::Iterator {
	public:
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::random_access_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using self_type = Iterator;

		constexpr reference operator*()noexcept { return *ptr; }
		constexpr pointer operator->()noexcept { return ptr; }
		constexpr reference operator[](difference_type n)noexcept { return ptr[n]; }
		constexpr const reference operator*() const noexcept { return *ptr; }
		constexpr const pointer operator->() const noexcept { return ptr; }
		constexpr const reference operator[](difference_type n) const noexcept { return ptr[n]; }

		constexpr self_type& operator++()noexcept { ++ptr; return *this; }
		constexpr self_type operator++(int)noexcept { self_type temp = *this; ++ptr; return temp; }
		constexpr self_type& operator--()noexcept { --ptr; return *this; }
		constexpr self_type operator--(int)noexcept { self_type temp = *this; --ptr; return temp; }

		constexpr self_type& operator+=(difference_type n)noexcept { ptr += n; return *this; }
		constexpr self_type& operator-=(difference_type n)noexcept { ptr -= n; return *this; }
		constexpr self_type operator+(difference_type n)const noexcept { return self_type(ptr + n); }
		constexpr self_type operator-(difference_type n)const noexcept { return self_type(ptr - n); }
		constexpr difference_type operator-(const self_type& rhs)const noexcept { return ptr - rhs.ptr; }

		constexpr bool operator==(const self_type& rhs)const noexcept { return ptr == rhs.ptr; }
		constexpr std::strong_ordering operator<=>(const self_type& rhs)const noexcept { return ptr <=> rhs.ptr; }

		constexpr Iterator(pointer p) noexcept :ptr(p) {}
		constexpr pointer base()const noexcept { return ptr; }
	private:
		pointer ptr = nullptr;
	};

	template<typename T> requires SequenceTypeTrait<T>
	class Sequence<T>::Const_Iterator {
	public:
		using value_type = const T;
		using pointer = const T*;
		using reference = const T&;
		using iterator_category = std::random_access_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using self_type = Const_Iterator;

		constexpr reference operator*()const noexcept { return *ptr; }
		constexpr pointer operator->()const noexcept { return ptr; }
		constexpr reference operator[](difference_type n)const noexcept { return ptr[n]; }

		constexpr self_type& operator++() noexcept { ++ptr; return *this; }
		constexpr self_type operator++(int) noexcept { self_type temp = *this; ++ptr; return temp; }
		constexpr self_type& operator--() noexcept { --ptr; return *this; }
		constexpr self_type operator--(int) noexcept { self_type temp = *this; --ptr; return temp; }

		constexpr self_type& operator+=(difference_type n) noexcept { ptr += n; return *this; }
		constexpr self_type& operator-=(difference_type n) noexcept { ptr -= n; return *this; }
		constexpr self_type operator+(difference_type n)const noexcept { return self_type(ptr + n); }
		constexpr self_type operator-(difference_type n)const noexcept { return self_type(ptr - n); }
		constexpr difference_type operator-(const self_type& rhs)const noexcept { return ptr - rhs.ptr; }

		constexpr bool operator==(const self_type& rhs)const noexcept { return ptr == rhs.ptr; }
		constexpr std::strong_ordering operator<=>(const self_type& rhs)const noexcept { return ptr <=> rhs.ptr; }

		constexpr Const_Iterator(pointer p)noexcept :ptr(p) {}
		constexpr Const_Iterator(const Iterator& rp) noexcept :ptr(rp.base()) {}
		constexpr pointer base()const noexcept { return ptr; }
	private:
		pointer ptr = nullptr;
	};
}
