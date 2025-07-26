#pragma once

#include <cstddef>
#include <iterator>

namespace badEngine {

	template <typename T>
	class ConstSeqIterator;

	template<typename T>
	class SeqIterator {
	public:
		using self_type = SeqIterator;
		using value_type = T;
		using reference = T&;
		using pointer = T*;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::random_access_iterator_tag;

		SeqIterator() = default;
		SeqIterator(pointer value) :value(value) {}

		const reference operator*()const {
			return *value;
		}
		reference operator*() {
			return *value;
		}
		const pointer operator->()const {
			return value;
		}
		pointer operator->() {
			return value;
		}

		self_type& operator++() {
			++value;
			return *this;
		}
		self_type operator++(int) {
			self_type temp = *this;
			++value;
			return temp;
		}

		self_type& operator--() {
			--value;
			return *this;
		}
		self_type operator--(int) {
			self_type temp = *this;
			--value;
			return temp;
		}

		self_type& operator+=(difference_type n) {
			value += n;
			return *this;
		}
		self_type& operator-=(difference_type n) {
			value -= n;
			return *this;
		}

		self_type operator+(difference_type n)const {
			return self_type(value + n);
		}
		self_type operator-(difference_type n)const {
			return self_type(value - n);
		}
		difference_type operator-(const self_type& rhs) {
			return value - rhs.value;
		}

		const reference operator[](difference_type n)const {
			return value[n];
		}
		reference operator[](difference_type n ) {
			return value[n];
		}

		bool operator==(const self_type& rhs)const {
			return value == rhs.value;
		}
		bool operator!=(const self_type& rhs)const {
			return value != rhs.value;
		}
		bool operator<(const self_type& rhs)const {
			return value < rhs.value;
		}
		bool operator>(const self_type& rhs)const {
			return value > rhs.value;
		}
		bool operator<=(const self_type& rhs)const {
			return value <= rhs.value;
		}
		bool operator>=(const self_type& rhs)const {
			return value >= rhs.value;
		}
	private:
		friend class ConstSeqIterator<T>;
		pointer value = nullptr;
	};
	template <typename T>
	class ConstSeqIterator {
	public:
		using self_type = ConstSeqIterator;
		using value_type = T;
		using reference = const T&;
		using pointer = const T*;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::random_access_iterator_tag;

		ConstSeqIterator() = default;
		ConstSeqIterator(pointer value) :value(value) {}
		ConstSeqIterator(const SeqIterator<T>& iter) :value(&*iter) {}

		reference operator*()const {
			return *value;
		}
		pointer operator->()const {
			return value;
		}

		self_type& operator++() {
			++value;
			return *this;
		}
		self_type operator++(int) {
			self_type temp = *this;
			++value;
			return temp;
		}
		self_type& operator--() {
			--value;
			return *this;
		}
		self_type operator--(int) {
			self_type temp = *this;
			--value;
			return temp;
		}

		self_type& operator+=(difference_type n) {
			value += n;
			return *this;
		}
		self_type& operator-=(difference_type n) {
			value -= n;
			return *this;
		}

		self_type operator+(difference_type n)const {
			return self_type(value + n);
		}
		self_type operator-(difference_type n)const {
			return self_type(value - n);
		}
		difference_type operator-(const self_type& rhs)const {
			return value - rhs.value;
		}

		reference operator[](difference_type n)const {
			return value[n];
		}

		bool operator==(const self_type& rhs)const {
			return value == rhs.value;
		}
		bool operator!=(const self_type& rhs)const {
			return value != rhs.value;
		}
		bool operator<(const self_type& rhs)const {
			return value < rhs.value;
		}
		bool operator>(const self_type& rhs)const {
			return value > rhs.value;
		}
		bool operator<=(const self_type& rhs)const {
			return value <= rhs.value;
		}
		bool operator>=(const self_type& rhs)const {
			return value >= rhs.value;
		}

	private:
		const pointer value = nullptr;
	};

	template <typename T>
	SeqIterator<T> operator+(typename SeqIterator<T>::difference_type n, const SeqIterator<T>& it) {
		return it + n;
	}
	template<typename T>
	ConstSeqIterator<T> operator+(typename ConstSeqIterator<T>::difference_type n, const ConstSeqIterator<T>& it) {
		return it + n;
	}
}