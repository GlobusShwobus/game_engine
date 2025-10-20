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
	concept IS_RULE_OF_FIVE_CLASS_T = std::default_initializable<T> &&
		std::copyable<T> &&
		std::is_nothrow_move_constructible_v<T> &&
		std::is_nothrow_move_assignable_v<T> &&
		std::destructible<T> &&
		!std::is_const_v<T>;
}