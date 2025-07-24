#pragma once

#include <utility>
#include "SeqIterator.h"

namespace badEngine {
	template <typename T>
	class Sequence {

		T* array = nullptr;
		size_t size;
		size_t cap;

		void grow() {
			size_t newCap = (cap == 0) ? 1 : cap * 2;//assign new size
			void* bytes = ::operator new(newCap * sizeof(T));//allocate raw bytes
			T* newArray = static_cast<T*>(bytes);//cast it to the actual type

			for (size_t i = 0; i < size; i++) {
				new (newArray + i) T(std::move(array[i]));//beginning of the sequence + sizeof(T) implicit, ptr arithmetic in ( )
			}

			for (size_t i = 0; i < size; i++) {
				array[i].~T();
			}

			::operator delete(array, cap * sizeof(T));//deletes the whole memory sequence bytes thing, does not care about the data there, meaning does call their destructors
			array = newArray;
			cap = newCap;
		}

	public:
		Sequence() :array(nullptr), size(0), cap(0) {}
		~Sequence() {
			for (size_t i = 0; i < size; ++i) {
				array[i].~T();//first destroy the element
			}
			::operator delete(array, cap * sizeof(T));//then delete the bytes
		}

		void add(const T& value) {
			if (size == cap)
				grow();
			new(array + size) T(value);//beginning of the sequence+ptr arithmetic index, place the value there
			++size;
		}
		void add(T&& value) {
			if (size == cap)
				grow();
			new(array + size) T(std::move(value));//beginning of the sequence+ptr arithmetic index, place the value there
			++size;
		}

		using iterator = SeqIterator<T>;
		using const_iterator = ConstSeqIterator<T>;

		iterator begin() {
			return iterator(array);
		}
		iterator end() {
			return iterator(array + size);
		}
		const_iterator begin()const {
			return const_iterator(array);
		}
		const_iterator end()const {
			return const_iterator(array + size);
		}
		const_iterator cbegin()const {
			return const_iterator(array);
		}
		const_iterator cend()const {
			return const_iterator(array + size);
		}

			/*
			TODO
			pop back
			iterators
			erase (but not insert in middle, fuck it, never use it)
			erase range
			clear

			empty
			size
			cap
			operator []

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