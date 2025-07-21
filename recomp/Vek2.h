#pragma once
#include <cmath>
namespace badEngine {

	template <typename T> 
	class Vec2 {

	public:
		T x;
		T y;

		Vec2() :x(), y(){}
		Vec2(T x, T y) :x(x), y(y) {}

		template <typename S>
		explicit Vec2(const Vec2<S>& v) :x(T(v.x)), y(T(v.y)) {}

		inline Vec2 operator+(const Vec2& p)const
		{
			return{ x + p.x, y + p.y };
		}
		inline Vec2& operator+=(const Vec2& p)
		{
			x += p.x; y += p.y;
			return *this;
		}
		inline Vec2 operator-(const Vec2& p)const
		{
			return{ x - p.x, y - p.y };
		}
		inline Vec2& operator-=(const Vec2& p)
		{
			x -= p.x; y -= p.y;
			return *this;
		}
		inline Vec2 operator*(const T scalar)const
		{
			return{ x * scalar, y * scalar };
		}
		inline Vec2& operator*=(const T scalar)
		{
			x *= scalar; y *= scalar;
			return *this;
		}
		inline Vec2 operator/(const T scalar)const
		{
			return{ x / scalar, y / scalar };
		}
		inline Vec2& operator/=(const T scalar)
		{
			x /= scalar; y /= scalar;
			return *this;
		}
		inline bool operator==(const Vec2& p)const
		{
			return x == p.x && y == p.y;
		}
		inline bool operator!=(const Vec2& p)const
		{
			return x != p.x || y != p.y;
		}

		inline T dot()const
		{
			return Vec2::dot(*this);
		}
		static inline T dot(const Vec2& p)
		{
			return(p.x * p.x) + (p.y * p.y);
		}

		inline T lenght()const
		{
			return Vec2::lenght(*this);
		}
		static inline T lenght(const Vec2& p)
		{
			return (T)std::sqrt(dot(p));
		}

		inline Vec2 inverse()const
		{
			return Vec2::inverse(*this);
		}
		static inline Vec2 inverse(const Vec2& p) {
			return{ (T)1 / p.x, (T)1 / p.y };
		}

		inline Vec2 unitVector()const
		{
			return Vec2::unitVector(*this);
		}
		static inline Vec2 unitVector(const Vec2& p)
		{
			T len = Vec2::lenght(p);
			if (len == (T)0)
				return Vec2();
			else
				return { p.x / len, p.y / len };
		}

		/*
		* DEPRICATED FOR NOW
		inline Vec2 signVector()const
		{
			return Vec2::signVector(*this);
		}
		static inline Vec2 signVector(const Vec2& p)
		{
			Vec2 z{ 0.0f,0.0f };
			if (p.x > 0.0f)
				z.x = 1.0f;
			else if (p.x < 0.0f)
				z.x = -1.0f;
			if (p.y > 0.0f)
				z.y = 1.0f;
			else if (p.y < 0.0f)
				z.y = -1.0f;
			return z;
		}
		*/
	};
	using Vec2f = Vec2<float>;
	using Vec2i = Vec2<int>;
	using Vec2d = Vec2<double>;
}

