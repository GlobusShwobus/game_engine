#pragma once

#include "badConcepts.h"

namespace badEngine {

	template <typename T>
		requires IS_MATHMATICAL_VECTOR_T<T>
	class Vec2M {

	public:
		//CONSTRUCTORS
		constexpr Vec2M()noexcept = default;
		constexpr Vec2M(T X, T Y)noexcept :x(X), y(Y) {}

		template <typename S>
		constexpr Vec2M(const Vec2M<S>& rhs)noexcept :x(rhs.x), y(rhs.y) {}

		template <typename S>
		constexpr Vec2M& operator=(const Vec2M<S>& rhs)noexcept {
			x = rhs.x;
			y = rhs.y;
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
		constexpr Vec2M operator*(const S scalar)const noexcept {
			return Vec2M(x * scalar, y * scalar);
		}
		template<typename S>
		constexpr Vec2M operator*(const Vec2M<S>& scalar)const noexcept {
			return Vec2M(x * scalar.x, y * scalar.y);
		}

		template<typename S>
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
		constexpr Vec2M operator/(const S scalar)const noexcept {
			return Vec2M(x / scalar, y / scalar);
		}
		template<typename S>
		constexpr Vec2M operator/(const Vec2M<S>& scalar)const noexcept {
			return Vec2M(x / scalar.x, y / scalar.y);
		}

		template<typename S>
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

		constexpr void set(T X, T Y)noexcept {
			x = X;
			y = Y;
		}

		constexpr void set_x(T X)noexcept {
			x = X;
		}

		constexpr void set_y(T Y)noexcept {
			y = Y;
		}

	public:

		T x = 0;
		T y = 0;
	};

	template<typename T, typename U> requires IS_MATHMATICAL_T<T>
	Vec2M<U> operator*(T scalar, const Vec2M<U>& v)noexcept {
		return Vec2M<U>(v.x * scalar, v.y * scalar);
	}

	template<typename T, typename U> requires IS_MATHMATICAL_T<T>
	Vec2M<U> operator/(T scalar, const Vec2M<U>& v)noexcept {
		return Vec2M<U>(v.x / scalar, v.y / scalar);
	}
	using vec2s = Vec2M<short>;
	using vec2i = Vec2M<int>;
	using vec2l = Vec2M<long>;
	using vec2ll = Vec2M<long long>;
	using vec2f = Vec2M<float>;
	using vec2d = Vec2M<double>;
	using vec2ld = Vec2M<long double>;
}