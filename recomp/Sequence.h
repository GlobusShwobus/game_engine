#pragma once

#include <utility>

template <typename T>
class Sequence {

	T* array = nullptr;
	size_t size;
	size_t cap;

	void grow() {
		size_t newCap = (cap == 0) ? 1 : cap * 2;//assign new size
		void* bytes = ::operator new(newCap * sizeof(T));//allocate raw bytes
		T* newArray = static_cast<T*> bytes;//cast it to the actual type

		for (size_t i = 0; i < size; i++) {
			new (newArray + i) T(std::move(array[i]));//beginning of the sequence + sizeof(T) implicit, ptr arithmetic in ( )
		}

		for (size_t i = 0; i < size; i++) {
			array[i].~T();
		}

		::operator delete(array);//deletes the whole memory sequence bytes thing, does not care about the data there, meaning does call their destructors
		array = newArray;
		cap = newCap;
	}

public:
	Sequence() :array(nullptr), size(0), cap(0) {}
	~Sequence() {
		for (size_t i = 0; i < size; ++i) {
			array[i].~T();//first delete the element
		}
		::operator delete(array);//then delete the bytes
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

};