#pragma once

#include <cassert>
#include <utility>
#include "SeqIterator.h"

namespace badEngine {
	template <typename T>
	class Sequence {

		T* array = nullptr;
		size_t mSize;
		size_t cap;

		void grow() {
			size_t newCap = (cap == 0) ? 1 : cap * 2;//assign new size
			void* bytes = ::operator new(newCap * sizeof(T));//allocate raw bytes
			T* newArray = static_cast<T*>(bytes);//cast it to the actual type

			for (size_t i = 0; i < mSize; i++) {
				new (newArray + i) T(std::move(array[i]));//beginning of the sequence + sizeof(T) implicit, ptr arithmetic in ( )
			}

			for (size_t i = 0; i < mSize; i++) {
				array[i].~T();
			}

			::operator delete(array, cap * sizeof(T));//deletes the whole memory sequence bytes thing, does not care about the data there, meaning does call their destructors
			array = newArray;
			cap = newCap;
		}
	public:
		Sequence() :array(nullptr), mSize(0), cap(0) {}
		~Sequence() {
			for (size_t i = 0; i < mSize; ++i) {
				array[i].~T();//first destroy the element
			}
			::operator delete(array, cap * sizeof(T));//then delete the bytes
		}

		bool isEmpty()const {
			return mSize == 0;
		}
		size_t size()const {
			return mSize;
		}
		size_t capacity()const {
			return cap;
		}
		void add(const T& value) {
			if (mSize == cap)
				grow();
			new(array + mSize) T(value);//beginning of the sequence+ptr arithmetic index, place the value there
			++mSize;
		}
		void add(T&& value) {
			if (mSize == cap)
				grow();
			new(array + mSize) T(std::move(value));//beginning of the sequence+ptr arithmetic index, place the value there
			++mSize;
		}

		void pop_back() {
			assert(mSize != 0);
			--mSize;
			array[mSize].~T();
		}

		void clear() {
			if (mSize == 0)
				return;
			for (size_t i = mSize; i-- > 0;) {
				array[i].~T();
			}
			mSize = 0;
		}

		T& operator[](size_t index) {
			assert(index < mSize);
			return array[index];
		}
		const T& operator[](size_t index)const {
			assert(index < mSize);
			return array[index];
		}


		using iterator = SeqIterator<T>;
		using const_iterator = ConstSeqIterator<T>;

		iterator begin() {
			return iterator(array);
		}
		iterator end() {
			return iterator(array + mSize);
		}
		const_iterator begin()const {
			return const_iterator(array);
		}
		const_iterator end()const {
			return const_iterator(array + mSize);
		}
		const_iterator cbegin()const {
			return const_iterator(array);
		}
		const_iterator cend()const {
			return const_iterator(array + mSize);
		}

		void eraseUnordered(size_t index) {
			assert(index < mSize);
			array[index].~T();

			if (index != mSize - 1) {//if last element, ignore
				new(array + index) T(std::move(array[mSize - 1]));//move last element to the other slot
				array[mSize - 1].~T();//remove the jast junk element after move
			}
			--mSize;
		}
		void eraseUnordered(iterator pos) {
			typename iterator::difference_type index = pos - begin();
			eraseUnordered(index);
		}
		template <typename Predicate>
		void remove_erase_if(Predicate predicate) {
        	auto dest = begin();
        
        	for (auto src = begin(); src != end(); ++src) {
        		if (!predicate(*src)) {
        			*dest++ = std::move(*src);
        		}
        	}
			eraseUnordered(dest);
        }


			/*
			TODO
			erase range

			resize
			reserve
			emplace_back - i have no idea how to even but need to know ( && ? )
			append range (add multiple in one go, initalizer list?)
			shrink to fit


			rule of 5 constructors
			initalizer list constructor

			IMPORTNAT:
				static asserts to make sure object has default, copy and move constructors, otherwise == no bueno
				*/
	};
}