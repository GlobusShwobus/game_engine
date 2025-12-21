#pragma once

#include "badUtility.h"
#include <cmath>
/*
NOTE: no operator overlaod checks for NAN or INFINITY. functions provide some checks
*/
namespace badEngine {

	template <typename T>
		requires VECTOR_TYPE<T>
	class Vec2M {

	public:
		//CONSTRUCTORS
		constexpr Vec2M()noexcept = default;
		constexpr Vec2M(T X, T Y)noexcept :x(X), y(Y) {}
		//CONVERSION CONSTRUCTOR
		template <typename S>
		constexpr Vec2M(const Vec2M<S>& rhs)noexcept :x(static_cast<T>(rhs.x)), y(static_cast<T>(rhs.y)) {}
		//CONVERSION ASSIGNMENT
		template <typename S>
		constexpr Vec2M& operator=(const Vec2M<S>& rhs)noexcept {
			x = static_cast<T>(rhs.x);
			y = static_cast<T>(rhs.y);
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
			requires MATHEMATICAL_PRIMITIVE<S>
		constexpr Vec2M<float> operator*(S scalar)const noexcept {
			return Vec2M<float>(static_cast<float>(x) * scalar, static_cast<float>(y) * scalar);
		}
		template<typename S>
			requires MATHEMATICAL_PRIMITIVE<S>
		constexpr Vec2M& operator*=(S scalar)noexcept {
			x *= scalar;
			y *= scalar;
			return *this;
		}


		template<typename S>
			requires MATHEMATICAL_PRIMITIVE<S>
		constexpr Vec2M<float> operator/(S scalar)const noexcept {
			return Vec2M<float>(static_cast<float>(x) / scalar, static_cast<float>(y) / scalar);
		}
		template<typename S>
			requires MATHEMATICAL_PRIMITIVE<S>
		constexpr Vec2M& operator/=(S scalar)noexcept {
			x /= scalar;
			y /= scalar;
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
		requires MATHEMATICAL_PRIMITIVE<T>
	Vec2M<float> operator*(T scalar, const Vec2M<U>& v)noexcept {
		return Vec2M<float>(static_cast<float>(v.x) * scalar, static_cast<float>(v.y) * scalar);
	}

	template<typename T, typename U> 
		requires MATHEMATICAL_PRIMITIVE<T>
	Vec2M<float> operator/(T scalar, const Vec2M<U>& v)noexcept {
		return Vec2M<float>(static_cast<float>(v.x) / scalar, static_cast<float>(v.y) / scalar);
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
	//if dot of vectors is 0, returns NAN
	//if product of lenghts is 0, returns NAN
	template <typename T>
	float angle_vector(const Vec2M<T>& v1, const Vec2M<T>& v2)noexcept {

		const float dot = dot_vector(v1, v2);
		if (dot == 0)
			return NAN;

		const float len1 = length_vector(v1);
		const float len2 = length_vector(v2);

		if (len1 == 0.0f || len2 == 0.0f)
			return NAN;

		const float cosAlpha = dot / (len1 * len2);

		return std::acosf(cosAlpha);
	}
	//if length of the vector is 0, then returns a vec2d(NAN,NAN)
	template <typename T>
	inline vec2d unit_vector(const Vec2M<T>& v) noexcept {

		const float len = length_vector(v);

		if (len == 0.0f)
			return vec2d(NAN, NAN);
		
		const float invlen = 1.0f / len;
		return vec2d(v.x * invlen, v.y * invlen);
	}

	template <typename T>
	constexpr auto normal_vector(Vec2M<T> v)noexcept {
		if (v.x > 0) v.x = 1;
		else if (v.x < 0) v.x = -1;
		else v.x = 0;

		if (v.y > 0) v.y = 1;
		else if (v.y < 0) v.y = -1;
		else v.y = 0;

		return v;
	}
}