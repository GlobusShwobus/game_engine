#pragma once

#include "badUtility.h"
#include <cmath>

namespace badEngine {

	template <typename T>
		requires IS_MATHMATICAL_VECTOR_T<T>
	class Vec2M {

	public:
		//CONSTRUCTORS
		constexpr Vec2M()noexcept = default;
		constexpr Vec2M(T X, T Y)noexcept :x(X), y(Y) {}
		//CONVERSION CONSTRUCTOR
		template <typename S>
		constexpr Vec2M(const Vec2M<S>& rhs)noexcept :x(T(rhs.x)), y(T(rhs.y)) {}
		//CONVERSION ASSIGNMENT
		template <typename S>
		constexpr Vec2M& operator=(const Vec2M<S>& rhs)noexcept {
			x = T(rhs.x);
			y = T(rhs.y);
			return *this;
		}

		//OPERATORS
		template <typename S>
		constexpr Vec2M operator+(const Vec2M<S>& rhs)const noexcept {
			return Vec2M(x + rhs.x, y + rhs.y);
		}

		template<typename S>
		constexpr Vec2M& operator+=(const Vec2M<S>& rhs)noexcept {
			x += rhs.x;
			y += rhs.y;
			return *this;
		}

		template <typename S>
		constexpr Vec2M operator-(const Vec2M<S>& rhs)const noexcept {
			return Vec2M(x - rhs.x, y - rhs.y);
		}
		constexpr Vec2M operator-()const noexcept {
			return Vec2M(-x, -y);
		}

		template<typename S>
		constexpr Vec2M& operator-=(const Vec2M<S>& rhs)noexcept {
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}

		template<typename S>
			requires IS_MATHMATICAL_T<S>
		constexpr Vec2M<float> operator*(const S scalar)const noexcept {
			return Vec2M<float>(float(x) * scalar, float(y) * scalar);
		}
		template<typename S>
		constexpr Vec2M<float> operator*(const Vec2M<S>& scalar)const noexcept {
			return Vec2M<float>(float(x) * scalar.x, float(y) * scalar.y);
		}

		template<typename S>
			requires IS_MATHMATICAL_T<S>
		constexpr Vec2M& operator*=(const S scalar)noexcept {
			x *= scalar;
			y *= scalar;
			return *this;
		}
		template <typename S>
		constexpr Vec2M& operator*=(const Vec2M<S>& scalar)noexcept {
			x *= scalar.x;
			y *= scalar.y;
			return *this;
		}

		template<typename S>
			requires IS_MATHMATICAL_T<S>
		constexpr Vec2M<float> operator/(const S scalar)const noexcept {
			return Vec2M<float>(float(x) / scalar, float(y) / scalar);
		}
		template<typename S>
		constexpr Vec2M<float> operator/(const Vec2M<S>& scalar)const noexcept {
			return Vec2M<float>(float(x) / scalar.x, float(y) / scalar.y);
		}

		template<typename S>
			requires IS_MATHMATICAL_T<S>
		constexpr Vec2M& operator/=(const S scalar)noexcept {
			x /= scalar;
			y /= scalar;
			return *this;
		}
		template<typename S>
		constexpr Vec2M& operator/=(const Vec2M<S>& scalar)noexcept {
			x /= scalar.x;
			y /= scalar.y;
			return *this;
		}

		template<typename S>
		constexpr bool operator ==(const Vec2M<S>& rhs)const noexcept {
			return x == rhs.x && y == rhs.y;
		}

		template <typename S>
		constexpr bool operator!=(const Vec2M<S>& rhs)const noexcept {
			return !(*this == rhs);
		}

		constexpr void nullify()noexcept {
			x = 0;
			y = 0;
		}

	public:

		T x = 0;
		T y = 0;
	};

	using vec2s  = Vec2M<short>;
	using vec2i  = Vec2M<int>;
	using vec2l  = Vec2M<long>;
	using vec2ll = Vec2M<long long>;
	using vec2f  = Vec2M<float>;
	using vec2d  = Vec2M<double>;
	using vec2ld = Vec2M<long double>;


	template<typename T, typename U>
		requires IS_MATHMATICAL_T<T>
	Vec2M<float> operator*(T scalar, const Vec2M<U>& v)noexcept {
		return Vec2M<float>(float(v.x) * scalar, float(v.y) * scalar);
	}

	template<typename T, typename U> 
		requires IS_MATHMATICAL_T<T>
	Vec2M<float> operator/(T scalar, const Vec2M<U>& v)noexcept {
		return Vec2M<float>(float(v.x) / scalar, float(v.y) / scalar);
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
	constexpr vec2d reciprocal_vector(const Vec2M<T>& v)noexcept {
		return vec2d(1.0f / v.x, 1.0f / v.y);
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