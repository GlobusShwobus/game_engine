#pragma once

#include <cmath>
#include <limits>
#include "Vec2M.h"
#include "Rectangle.h"
#include "Transform.h"

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

	template <typename T>
	constexpr bool rect_vs_point(const Rectangle<T>& rect, T X, T Y)noexcept {
		return (
			X >= rect.x &&
			Y >= rect.y &&
			X < rect.x + rect.w &&
			Y < rect.y + rect.h);
	}

	template <typename T>
	constexpr bool rect_vs_point(const Rectangle<T>& rect, Vec2M<T>& pos)noexcept {
		return rect_vs_point(rect, pos.x, pos.y);
	}

	template <typename T>
	constexpr bool rect_vs_rect(const Rectangle<T>& b1, const Rectangle<T>& b2)noexcept {
		return
			b1.x < b2.x + b2.w &&
			b1.x + b1.w > b2.x &&
			b1.y < b2.y + b2.h &&
			b1.y + b1.h > b2.y;
	}

	float rect_vs_ray(const rectF& objA, const rectF& objB, const vec2f& velocity, vec2f& normal);


	template <typename T>
	bool dynamic_vs_dynamic_collision(Transform<T>& objA, Transform<T>& objB, vec2f& normal, float& contactTime) {

		vec2f relativeVel = objA.mCurrVelocity - objB.mCurrVelocity;
		rectF expandedA = objA.get_expanded_rect(relativeVel);

		if (!rect_vs_rect(expandedA, objB.mBox)) {
			return false;
		}

		contactTime = rect_vs_ray(objA.mBox, objB.mBox, relativeVel, normal);

		return (contactTime >= 0.f && contactTime < 1.f);
	}

	template <typename T, typename ExecutionPolicy>//I WANT EXPRESSION BUT NOT FORCE USING SEQUENCEM, FUGG
	requires std::invocable<ExecutionPolicy>
	void collision_algorithm_one(SequenceM<Transform<T>>& objects, ExecutionPolicy&& policy) {
		
		struct CollisionResult {
			int i, j;
			float t;
			vec2f normal;
		};

	}

}
