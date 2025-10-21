#pragma once

#include <cmath>
#include "Vec2M.h"


namespace badEngine {

	template <typename T> requires IS_LESS_THAN_COMPARABLE<T>
	constexpr auto mValue_max(const T& x, const T& y)noexcept {
		return (x < y) ? y : x;
	}
	template<typename T> requires IS_LESS_THAN_COMPARABLE<T>
	constexpr auto mValue_min(const T& x, const T& y)noexcept {
		return (x < y) ? x : y;
	}

	template<typename T>
	constexpr Vec2M<T> abs_vector(const Vec2M<T>& vec)noexcept {
		return Vec2M<T>(std::abs(vec.x), std::abs(vec.y));
	}

	template <typename T>
	constexpr auto dot_vector(const Vec2M<T>& v1, const Vec2M<T>& v2)noexcept {
		return (v1.x * v2.x) + (v1.y * v2.y);
	}

	template <typename T>
	inline float length_vector(const Vec2M<T>& v)noexcept {
		return std::sqrt(static_cast<float>((v.x * v.x) + (v.y * v.y)));
	}
	template <typename T>
	inline float distance_between(const Vec2M<T>& a, const Vec2M<T>& b) noexcept {
		return length_vector(b - a);
	}

	template <typename T>
	constexpr vec2d reciprocal_vector(const Vec2M<T>& v)noexcept {
		return vec2d(1.0f / v.x, 1.0f / v.y);
	}

	template <typename T>
	constexpr auto opposite_vector(const Vec2M<T>& v)noexcept {
		return -v;
	}

	template <typename T>
	inline vec2d unit_vector(const Vec2M<T>& v) noexcept {
		const float length = length_vector(v);
		return vec2d(v.x / length, v.y / length);
	}

	template <typename T>
	constexpr auto normal_vector(Vec2M<T> v)noexcept {
		if (v.x > 0) v.x = 1;
		else if (v.x < 0) v.x = -1;

		if (v.y > 0) v.y = 1;
		else if (v.y < 0) v.y = -1;

		return v;
	}
}
