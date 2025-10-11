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
	constexpr bool rect_vs_dot(const Rectangle<T>& rect, U X, U Y)noexcept {
		return (
			X >= rect.x &&
			Y >= rect.y &&
			X < rect.x + rect.w &&
			Y < rect.y + rect.h);
	}

	template <typename T, typename U>
	constexpr bool rect_vs_dot(const Rectangle<T>& rect, Vec2M<U>& pos)noexcept {
		return rect_vs_dot(rect, pos.x, pos.y);
	}

	template <typename T, typename U>
	constexpr bool rect_vs_rect(const Rectangle<T>& a, const Rectangle<U>& b, rectF& output)noexcept {
		auto distances = (a.get_center_point() - b.get_center_point());
		auto overlap = (a.get_half_size() + b.get_half_size()) - abs_vector(distances);

		if (overlap.x < 0.0f || overlap.y < 0.0f) return false;

		output.set_XY(distances);
		output.set_WH(overlap);
		return true;
	}
	template <typename T, typename U>
	constexpr bool rect_vs_rect(const Rectangle<T>& b1, const Rectangle<U>& b2)noexcept {
		return(
			(b1.x < (b2.x + b2.w) &&
			(b1.x + b1.w) > b2.x) &&
			(b1.y < (b2.y + b2.h) &&
			(b1.y + b1.h) > b2.y)
			);
	}





	template<typename T, typename U>
	constexpr rectF get_swept_expanded_target(const Rectangle<T>& b1, const Rectangle<U>& b2)noexcept {
		return rectF(
			b2.x - (b1.w * 0.5f),
			b2.y - (b1.h * 0.5f),
			b2.w + b1.w,
			b2.h + b1.h
		);
	}
	vec2f get_swept_result_normal(const vec2f& entryTime, const vec2f& reciprocal) {
		vec2f normal;
		if (entryTime.x > entryTime.y)
			if (reciprocal.x < 0)
				normal = { 1, 0 };
			else
				normal = { -1, 0 };
		else if (entryTime.x < entryTime.y)
			if (reciprocal.y < 0)
				normal = { 0, 1 };
			else
				normal = { 0, -1 };
		
		return normal;
	}
	bool ray_vs_rect(
		const vec2f& rayOrigin,
		const vec2f& rayVector,
		const rectF& target,
		float& contactTime,
		vec2f& contactNormal) noexcept
	{
		auto reciprocal = reciprocal_vector(rayVector);
		
		vec2d tNear = vec2d(
			(target.x - rayOrigin.x) * reciprocal.x,
			(target.y - rayOrigin.y) * reciprocal.y
		);
		vec2d tFar = vec2d(
			(target.x + target.w - rayOrigin.x) * reciprocal.x,
			(target.y + target.h - rayOrigin.y) * reciprocal.y
		);

		//broken float value (division by 0 probably)
		if (
			std::isnan(tNear.x) ||
			std::isnan(tNear.y) ||
			std::isnan(tFar.x) ||
			std::isnan(tFar.y)) return false;
		//order
		if (tNear.x > tFar.x) std::swap(tNear.x, tFar.x);
		if (tNear.y > tFar.y) std::swap(tNear.y, tFar.y);
		//if no hit == false
		if (tNear.x > tFar.y || tNear.y > tFar.x)
			return false;
		contactTime = std::max(tNear.x, tNear.y);
		float hitFar = std::min(tFar.x, tFar.y);

		//if hit but opposite direction, then no actual hit, just on same line
		if (hitFar < 0.0f)
			return false;

		/*
		//set the point where contact was made, idk what to do with it, can remove later tho
		if (contactPoint)
			*contactPoint = (rayVector * hitFar) + rayOrigin;
		*/

		//get normal
		contactNormal = get_swept_result_normal(tNear, reciprocal);

		return true;
	}
	bool do_swept_collision(
		const TransformF& a,
		const TransformF& b,
		float& contactTime,
		vec2f& contactNormal)noexcept
	{
		auto relativeVelocity = a.mVelocity - b.mVelocity;
		//no movement
		if (relativeVelocity.x == 0 && relativeVelocity.y == 0) return false;

		rectF expandedTarget = get_swept_expanded_target(a.mBox, b.mBox);

		if (ray_vs_rect(a.mBox.get_center_point(), relativeVelocity, expandedTarget, contactTime, contactNormal)) {
			return (contactTime >= 0.0f && contactTime < 1.0f);
		}
		return false;
	}

	template <typename T, typename U>
	constexpr bool contaier_vs_rect(const Rectangle<T>& bigBox, const Rectangle<U>& smallBox, vec2f& displacement)noexcept {
		/*
		WILL BE BUGGY IF CALLED WITH A SMALLER BOX AS FIRST PARAMETER, SHOULD NOT BE USED THAT WAY
		*/

		bool isDisplacement = false;
		if (smallBox.x < bigBox.x) {
			displacement.x = bigBox.x - smallBox.x;
			isDisplacement = true;
		}
		else if (smallBox.x + smallBox.w > bigBox.x + bigBox.w) {
			displacement.x = (bigBox.x + bigBox.w) - (smallBox.x + smallBox.w);
			isDisplacement = true;
		}

		if (smallBox.y < bigBox.y) {
			displacement.y = bigBox.y - smallBox.y;
			isDisplacement = true;
		}
		else if (smallBox.y + smallBox.h > bigBox.y + bigBox.h) {
			displacement.y = (bigBox.y + bigBox.h) - (smallBox.y + smallBox.h);
			isDisplacement = true;
		}

		return isDisplacement;
	}
}
