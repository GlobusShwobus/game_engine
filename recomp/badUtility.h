#pragma once

#include <concepts>

namespace badEngine {

	template<typename T>
	concept IS_INTEGER_TYPE_T = std::_Is_any_of_v<std::remove_cv_t<T>, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long>;

	template <typename T>
	concept IS_FLOATING_TYPE_T = std::_Is_any_of_v<std::remove_cv_t<T>, float, double, long double>;

	template <typename T>
	concept IS_MATHMATICAL_VECTOR_T = std::_Is_any_of_v<std::remove_cv_t<T>, short, int, long, long long, float, double, long double>;

	template <typename T>
	concept IS_MATHMATICAL_T = std::_Is_any_of_v<std::remove_cv_t<T>, short, unsigned short, int, unsigned int, long int, unsigned long, long long, unsigned long long, float, double, long double>;

	template <typename T>
	concept IS_LESS_THAN_COMPARABLE = requires (T a, T b) {
		{ a < b }->std::convertible_to<bool>;
	};
	template <typename T>
	concept IS_SEQUENCE_COMPATIBLE =
		std::destructible<T> &&
		std::is_nothrow_move_constructible_v<T> &&
		!std::is_const_v<T>;

	template <typename T> requires IS_LESS_THAN_COMPARABLE<T>
	constexpr auto bad_maxV(const T& x, const T& y)noexcept {
		return (x < y) ? y : x;
	}
	template<typename T> requires IS_LESS_THAN_COMPARABLE<T>
	constexpr auto bad_minV(const T& x, const T& y)noexcept {
		return (x < y) ? x : y;
	}

	template<typename T>requires IS_MATHMATICAL_T<T>
	constexpr bool isMinus(T x)noexcept {
		return x < 0;
	}
	template<typename T>requires IS_MATHMATICAL_T<T>
	constexpr bool isPlus(T x)noexcept {
		return x > 0;
	}
	template<typename T> requires IS_MATHMATICAL_T<T>
	constexpr void swap_numerical(T& n1, T& n2)noexcept {
		T temp = n1;
		n1 = n2;
		n2 = temp;
	}
	template<typename T> requires IS_FLOATING_TYPE_T<T>
	constexpr bool bad_isNaN(T x)noexcept {
		return x != x;
	}
}