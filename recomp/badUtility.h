#pragma once

#include <concepts>

namespace badEngine {

	template <typename T, typename... Ts>
	concept ONE_OF = (std::same_as<T, Ts> || ...);

	template <typename T>
	concept BY_VALUE_TYPE = std::same_as<T, std::remove_cvref_t<T>>;

	template<typename T>
	concept PRIMITIVE_TYPE = ONE_OF<std::remove_cvref_t<T>, bool, char, unsigned char, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long, float, double, long double>;

	template <typename T>
	concept MATHEMATICAL_PRIMITIVE = ONE_OF<std::remove_cvref_t<T>, char, short, int, long, long long, float, double, long double>;

	template <typename T>
	concept INTEGER_TYPE= ONE_OF<std::remove_cvref_t<T>, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long>;

	template <typename T>
	concept FLOAT_TYPE = ONE_OF<std::remove_cvref_t<T>, float, double, long double>;

	template <typename T>
	concept VECTOR_TYPE = ONE_OF<std::remove_cvref_t<T>, short, int, long, long long, float, double, long double> && BY_VALUE_TYPE<T>;


	template<typename T>
	concept LESS_THAN_COMPARE = requires (const T & x, const T & y) {
		{ x < y }->std::convertible_to<bool>;
	};

	template <typename T>
	concept IS_SEQUENCE_COMPATIBLE = std::destructible<T> && std::is_nothrow_move_constructible_v<T> && BY_VALUE_TYPE<T>;

	template<typename T>
	concept IS_SLLIST_COMPATIBLE = std::destructible<T> && BY_VALUE_TYPE<T>;

	template <typename T, typename U> requires LESS_THAN_COMPARE<std::remove_cvref_t<T>>
	constexpr auto bad_maxV(const T& x, const U& y)noexcept {
		return (x < y) ? y : x;
	}
	template<typename T, typename U> requires LESS_THAN_COMPARE<std::remove_cvref_t<T>>
	constexpr auto bad_minV(const T& x, const U& y)noexcept {
		return (x < y) ? x : y;
	}

	//undefined behavior if low is higher than high
	template<typename T>
	requires LESS_THAN_COMPARE<std::remove_cvref_t<T>>
	constexpr const T& bad_clamp(const T& val, const T& low, const T& high) {
		if (val < low)return low;
		if (high < val)return high;
		return val;
	}

	template<typename T>requires MATHEMATICAL_PRIMITIVE<T>
	constexpr bool isMinus(T x)noexcept {
		return x < 0;
	}
	template<typename T>requires MATHEMATICAL_PRIMITIVE<T>
	constexpr bool isPlus(T x)noexcept {
		return x > 0;
	}
	template<typename T> requires PRIMITIVE_TYPE<T>
	constexpr void swap_numerical(T& n1, T& n2)noexcept {
		static_assert(!std::is_const_v<T>, "swap_numerical requires non const lvalue T");
		T temp = n1;
		n1 = n2;
		n2 = temp;
	}
	template<typename T> requires FLOAT_TYPE<T>
	constexpr bool bad_isNaN(T x)noexcept {
		return x != x;
	}
}