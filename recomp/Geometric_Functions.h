#pragma once

#include <cmath>
#include <limits>
#include "Vec2M.h"
#include "Rectangle.h"
#include "Transform.h"

namespace badEngine {

	template<typename T>
	constexpr Vec2M<T> abs_vector(const Vec2M<T>& vec)noexcept {
		return Vec2M<T>(std::abs(vec.x), std::abs(vec.y));
	}

	template <typename T, typename U>
	constexpr auto dot_vector(const Vec2M<T>& v1, const Vec2M<U>& v2)noexcept {
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

	template <typename T, typename U> requires IS_MATHMATICAL_T<U>
	constexpr bool rect_contains_pos(const Rectangle<T>& rect, U X, U Y)noexcept {
		return (
			X >= rect.x &&
			Y >= rect.y &&
			X < rect.x + rect.w &&
			Y < rect.y + rect.h);
	}

	template <typename T, typename U>
	constexpr bool rect_contains_pos(const Rectangle<T>& rect, Vec2M<U>& pos)noexcept {
		return rect_contains_pos(rect, pos.x, pos.y);
	}

	template <typename T, typename U>
	constexpr bool AABB_contains(const Rectangle<T>& b1, const Rectangle<U>& b2)noexcept {
		return !(b1.x + b1.w < b2.x || b1.x > b2.x + b2.w || b1.y + b1.h < b2.y || b1.y > b2.y + b2.h);
	}

	template <typename T, typename U>
	constexpr bool AABB_contains(const Rectangle<T>& a, const Rectangle<U>& b, rectF& output)noexcept {
		auto distances = (a.get_center_point() - b.get_center_point());
		auto overlap = (a.get_half_size() + b.get_half_size()) - abs_vector(distances);

		if (overlap.x < 0.0f || overlap.y < 0.0f) return false;

		output.set_XY(distances);
		output.set_WH(overlap);
		return true;
	}

	constexpr rectF expanded_AABB_box(const TransformF& box)noexcept {
		return rectF(
			box.mBox.x + box.mVelocity.x,
			box.mBox.y + box.mVelocity.y,
			box.mBox.w + std::abs(box.mVelocity.x),
			box.mBox.h + std::abs(box.mVelocity.y)
		);
	}
	bool AABB_collision(const TransformF& box1, const TransformF& box2) {

		rectF expanded = expanded_AABB_box(box1);
		rectF output;

		if (AABB_contains(expanded, box2.mBox, output)) {

			/*
			*FIRST MAKE SURE AABB_contains IS NOT BUGGED. REFER TO THE BALD YOUTUBE GUY
			
			*OUTPUT SHOULD GIVE A NORMAL AND A SIZE, NOT A RECTANGLE!!!

			TWO OPTIONS: EITHER RESOLVE THE CLAMP HERE AND RETURN A LEFTOVER VELOCITY OR RETURN BOTH NORMAL AND 
			OUTPUT AND RESOLVE IN A DEDICATED MANNER

			*/

			return true;
		}

		return false;
	}
	
}
