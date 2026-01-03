#pragma once

#include "badUtility.h"
#include <cmath>
/*
NOTE: no operator overlaod checks for NAN or INFINITY. functions provide some checks
*/
namespace badEngine {
	template <typename T>
		requires VECTOR_TYPE<T>
	class vec2 {

	public:
		//CONSTRUCTORS
		constexpr vec2()noexcept = default;
		constexpr vec2(T X, T Y)noexcept :x(X), y(Y) {}
		//CONVERSION CONSTRUCTOR
		template <typename S>
		constexpr vec2(const vec2<S>& rhs)noexcept :x(static_cast<T>(rhs.x)), y(static_cast<T>(rhs.y)) {}
		//CONVERSION ASSIGNMENT
		template <typename S>
		constexpr vec2& operator=(const vec2<S>& rhs)noexcept {
			x = static_cast<T>(rhs.x);
			y = static_cast<T>(rhs.y);
			return *this;
		}

		//OPERATORS
		template <typename S>
		constexpr vec2 operator+(const vec2<S>& rhs)const noexcept {
			return vec2(x + rhs.x, y + rhs.y);
		}

		template<typename S>
		constexpr vec2& operator+=(const vec2<S>& rhs)noexcept {
			x += rhs.x;
			y += rhs.y;
			return *this;
		}

		template <typename S>
		constexpr vec2 operator-(const vec2<S>& rhs)const noexcept {
			return vec2(x - rhs.x, y - rhs.y);
		}
		constexpr vec2 operator-()const noexcept {
			return vec2(-x, -y);
		}

		template<typename S>
		constexpr vec2& operator-=(const vec2<S>& rhs)noexcept {
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}

		template<typename S>
			requires MATHEMATICAL_PRIMITIVE<S>
		constexpr vec2<float> operator*(S scalar)const noexcept {
			return vec2<float>(static_cast<float>(x) * scalar, static_cast<float>(y) * scalar);
		}
		template<typename S>
			requires MATHEMATICAL_PRIMITIVE<S>
		constexpr vec2& operator*=(S scalar)noexcept {
			x *= scalar;
			y *= scalar;
			return *this;
		}


		template<typename S>
			requires MATHEMATICAL_PRIMITIVE<S>
		constexpr vec2<float> operator/(S scalar)const noexcept {
			return vec2<float>(static_cast<float>(x) / scalar, static_cast<float>(y) / scalar);
		}
		template<typename S>
			requires MATHEMATICAL_PRIMITIVE<S>
		constexpr vec2& operator/=(S scalar)noexcept {
			x /= scalar;
			y /= scalar;
			return *this;
		}


		template<typename S>
		constexpr bool operator ==(const vec2<S>& rhs)const noexcept {
			return x == rhs.x && y == rhs.y;
		}

		template <typename S>
		constexpr bool operator!=(const vec2<S>& rhs)const noexcept {
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


	using int2  = vec2<int>;
	using float2 = vec2<float>;
	using double2 = vec2<double>;


	template<typename T, typename U>
		requires MATHEMATICAL_PRIMITIVE<T>
	vec2<float> operator*(T scalar, const vec2<U>& v)noexcept {
		return vec2<float>(static_cast<float>(v.x) * scalar, static_cast<float>(v.y) * scalar);
	}

	template<typename T, typename U> 
		requires MATHEMATICAL_PRIMITIVE<T>
	vec2<float> operator/(T scalar, const vec2<U>& v)noexcept {
		return vec2<float>(static_cast<float>(v.x) / scalar, static_cast<float>(v.y) / scalar);
	}


	template<typename T>
	constexpr vec2<T> abs_vector(const vec2<T>& vec)noexcept {
		return vec2<T>(std::abs(vec.x), std::abs(vec.y));
	}
	template<typename T>
	constexpr float dot_vector(const vec2<T>& vec)noexcept {
		return (vec.x * vec.x) + (vec.y * vec.y);
	}

	/*
	DEPRICATED FOR NOW, this is a real math version, not graphics. and should be abs i think
	template <typename T>
	constexpr auto dot_vector(const vec2<T>& v1, const vec2<T>& v2)noexcept {
		return (v1.x * v2.x) + (v1.y * v2.y);
	}
	*/


	template <typename T>
	inline float length_vector(const vec2<T>& v)noexcept {
		return std::sqrt(static_cast<float>((v.x * v.x) + (v.y * v.y)));
	}
	//if dot of vectors is 0, returns NAN
	//if product of lenghts is 0, returns NAN
	template <typename T>
	float angle_vector(const vec2<T>& v1, const vec2<T>& v2)noexcept {

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
	inline float2 unit_vector(const vec2<T>& v) noexcept {

		const float len = length_vector(v);

		if (len == 0.0f)
			return float2(NAN, NAN);
		
		const float invlen = 1.0f / len;
		return float2(v.x * invlen, v.y * invlen);
	}

	template <typename T>
	constexpr auto normal_vector(vec2<T> v)noexcept {
		if (v.x > 0) v.x = 1;
		else if (v.x < 0) v.x = -1;
		else v.x = 0;

		if (v.y > 0) v.y = 1;
		else if (v.y < 0) v.y = -1;
		else v.y = 0;

		return v;
	}
}