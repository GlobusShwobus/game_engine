#pragma once
#include "badUtility.h"
#include "Vec2M.h"

namespace badEngine {
	template<typename T> 
		requires VECTOR_TYPE<T>
	class Rectangle {
	public:
		//CONSTRUCTORS
		constexpr Rectangle()noexcept = default;
		constexpr Rectangle(T x, T y, T w, T h) noexcept :x(x), y(y), w(w), h(h) {}
		constexpr Rectangle(const vec2<T>& pos, const vec2<T>& size) noexcept : x(pos.x), y(pos.y), w(size.x), h(size.y) {}

		//CONVERSION CONSTRUCTOR
		template<typename S>
		constexpr Rectangle(const Rectangle<S>& rhs)noexcept :x(static_cast<T>(rhs.x)), y(static_cast<T>(rhs.y)), w(static_cast<T>(rhs.w)), h(static_cast<T>(rhs.h)) {}
		//CONVERSION ASSIGNMENT
		template<typename S>
		constexpr Rectangle& operator=(const Rectangle<S>& rhs)noexcept {
			x = static_cast<T>(rhs.x); 
			y = static_cast<T>(rhs.y);
			w = static_cast<T>(rhs.w);
			h = static_cast<T>(rhs.h);
			return *this;
		}
		template<typename U>
		constexpr Rectangle<U> union_with(const Rectangle<U>& other)const noexcept
		{
			const U minx = bad_minV(x,     static_cast<T>(other.x));
			const U miny = bad_minV(y,     static_cast<T>(other.y));
			const U maxx = bad_maxV(x + w, static_cast<T>(other.x + other.w));
			const U maxy = bad_maxV(y + h, static_cast<T>(other.y + other.h));

			return Rectangle<U>(minx, miny, maxx - minx, maxy - miny);
		}

		constexpr float perimeter()const noexcept {
			return 2.0f * (w + h);
		}

		template <typename S>
		constexpr bool contains(const vec2<S>& pos)const noexcept {
			return
				pos.x >= x &&
				pos.y >= y &&
				pos.x <= x + w &&
				pos.y <= y + h;
		}

		template <typename S>
		constexpr bool contains(const Rectangle<S>& rect)const noexcept {
			return 
				rect.x >= x &&
				rect.y >= y &&
				rect.x + rect.w <= x + w &&
				rect.y + rect.h <= y + h;
		}

		template <typename S>
		constexpr bool intersects(const Rectangle<S>& rhs)const noexcept {
			return
				x < rhs.x + rhs.w &&
				x + w > rhs.x &&
				y < rhs.y + rhs.h &&
				y + h > rhs.y;
		}

		template<typename S>
		constexpr bool is_same_size(const Rectangle<S>& rhs)const noexcept {
			return w == rhs.w && h == rhs.h;
		}
		constexpr float2 get_center_point()const noexcept {
			return float2(
				x + (w * 0.5f),
				y + (h * 0.5f)
			);
		}
	public:
		T x = 0;
		T y = 0;
		T w = 0;
		T h = 0;
	};

	using int4 = Rectangle<int>;
	using float4 = Rectangle<float>;

	template<typename U>
	constexpr  Rectangle<U> union_rect(const Rectangle<U>& A, const Rectangle<U>& B)noexcept {
		return A.union_with(B);
	}

	template<typename U>
	bool AABB_overlap(const Rectangle<U>& A, const Rectangle<U>& B, const float2& Avel, float2& displace)noexcept
	{
		if (displace.x < displace.y) {
			displace.y = 0;
			displace.x = (isPlus(Avel.x)) ? -displace.x : displace.x;
		}
		else {
			displace.x = 0;
			displace.y = (isPlus(Avel.y)) ? -displace.y : displace.y;
		}

		return true;
	}

	struct AABB_overap {
		float2 displaceA;
		float2 displaceB;
	};
	template<typename U>
	constexpr bool AABB_overlap_result(const Rectangle<U>& lBox, const vec2<U>& lVec, const Rectangle<U>& rBox, const vec2<U>& rVec, AABB_overap& result) noexcept {
		const float2 centerPointDistance = abs_vector(lBox.get_center_point() - rBox.get_center_point());

		float2 overlap(
			(lBox.w * 0.5f) + (rBox.w * 0.5f) - centerPointDistance.x,
			(lBox.h * 0.5f) + (rBox.h * 0.5f) - centerPointDistance.y
		);

		if (overlap.x <= 0 || overlap.y <= 0) {
			return false;
		}
		const float2 relativeVel = lVec - rVec;

		if (overlap.x < overlap.y) {//resolve on X axis
			
			result.displaceA.x = (relativeVel.x > 0) ? overlap.x : -overlap.x;
			result.displaceA.y = 0;
			//B moves in opposite dir
			result.displaceB.x = -result.displaceA.x;
			result.displaceB.y = 0;
		}
		else {//resolve on Y axis
			result.displaceA.x = 0;
			result.displaceA.y = (relativeVel.y > 0) ? overlap.y : -overlap.y;

			// B should move in opposite direction
			result.displaceB.x = 0;
			result.displaceB.y = -result.displaceA.y;
		}
		return true;
	}

}