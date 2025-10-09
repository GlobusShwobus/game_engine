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
	template <typename T, typename U>
	constexpr auto dot_vector_perpendicular(const Vec2M<T>& v1, const Vec2M<U>& v2)noexcept {
		return (v1.x * v2.y) + (v1.y * v2.x);
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

	template <typename T, typename U, typename S>
	constexpr bool AABB_contains_clamp(const Rectangle<T>& a, const Rectangle<U>& b, rectF& output)noexcept {
		auto distances = (a.get_center_point() - b.get_center_point());
		auto overlap = (a.get_half_size() + b.get_half_size()) - abs_vector(distances);

		if (overlap.x < 0.0f || overlap.y < 0.0f) return false;

		output.set_XY(distances);
		output.set_WH(overlap);
		return true;
	}

	constexpr bool AABB_swept(
		const vec2f& rayOrigin,
		const vec2f& rayVector,
		const rectF& target,
		float& contactTime,
		vec2f& contactNormal) noexcept
	{
		auto invdir = reciprocal_vector(rayVector);

		vec2f nearHit = vec2f(
			(target.x - rayOrigin.x) * invdir.x,
			(target.y - rayOrigin.y) * invdir.y
		);
		vec2f farHit = vec2f(
			(target.x + target.w - rayOrigin.x) * invdir.x,
			(target.y + target.h - rayOrigin.y) * invdir.y
		);

		//broken float value (division by 0 probably)
		if (std::isnan(farHit.y) || std::isnan(farHit.x))
			return false;
		if (std::isnan(nearHit.y) || std::isnan(nearHit.x))
			return false;
		//order
		if (nearHit.x > farHit.x) std::swap(nearHit.x, farHit.x);
		if (nearHit.y > farHit.y) std::swap(nearHit.y, farHit.y);
		//if no hit == false
		if (nearHit.x > farHit.y || nearHit.y > farHit.x)
			return false;

		//determine contacts
		contactTime = std::max(nearHit.x, nearHit.y);
		float contactExit = std::min(farHit.x, farHit.y);

		//if hit but opposite direction, then no actual hit, just on same line
		if (contactExit < 0)
			return false;
		
		if (nearHit.x > nearHit.y)
			if (invdir.x < 0)
				contactNormal = { 1, 0 };
			else
				contactNormal = { -1, 0 };
		else if (nearHit.x < nearHit.y)
			if (invdir.y < 0)
				contactNormal = { 0, 1 };
			else
				contactNormal = { 0, -1 };
		
		/*
		    //DEPRICATED FOR NOW
			//if (contactPoint)
			//	*contactPoint = (rayVector * hitFar) + rayOrigin;
		*/

		return true;
	}
	template<typename T, typename U>
	constexpr bool AABB_swept_expanded(
		const Transform<T>& a,
		const Transform<U>& b,
		float& contactTime,
		vec2f& contactNormal)noexcept
	{
		auto relativeVelocity = a.mVelocity - b.mVelocity;//because both objects can move, if not however its something - zero so doesnt matter
		//no relative movement
		if (relativeVelocity.x == 0 && relativeVelocity.y == 0)
			return false;

		vec2f halfOfA = a.mBox.get_half_size();
		rectF expandedTarget = rectF(
			b.mBox.x - halfOfA.x,
			b.mBox.y - halfOfA.y,
			b.mBox.w + a.mBox.w,
			b.mBox.h + a.mBox.h
		);

		if (intersects_projection(a.mBox.get_center_point(), relativeVelocity, expandedTarget, contactTime, contactNormal)) {
			return (contactTime >= 0.0f && contactTime < 1.0f);
		}
		return false;
	}

}
