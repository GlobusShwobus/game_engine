#pragma once

#include <cmath>
#include "Vec2M.h"
#include "Rectangle.h"
#include "Transform.h"

namespace badEngine {

	template <typename T, typename U> requires IS_LESS_THAN_COMPARABLE<T, U>
	constexpr auto larger_value(const T& x, const U& y)noexcept {
		return (x < y) ? y : x;
	}
	template<typename T, typename U> requires IS_LESS_THAN_COMPARABLE<T, U>
	constexpr auto smaller_value(const T& x, const U& y)noexcept {
		return (x < y) ? x : y;
	}
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
	constexpr auto sign_vector(Vec2M<T> v)noexcept {
		if (v.x > 0) v.x = 1;
		else if (v.x < 0) v.x = -1;

		if (v.y > 0) v.y = 1;
		else if (v.y < 0) v.y = -1;

		return v;
	}

	template <typename T, typename U> requires IS_MATHMATICAL_T<U>
	constexpr bool intersects_rectangle(const Rectangle<T>& rect, U X, U Y)noexcept {
		return (
			X >= rect.x &&
			Y >= rect.y &&
			X < rect.x + rect.w &&
			Y < rect.y + rect.h);
	}

	template <typename T, typename U>
	constexpr bool intersects_rectangle(const Rectangle<T>& rect, Vec2M<U>& pos)noexcept {
		return intersects_rectangle(rect, pos.x, pos.y);
	}

	template <typename T, typename U>
	constexpr bool intersects_rectangle(const Rectangle<T>& a, const Rectangle<U>& b)noexcept {
		return (
			a.x < b.x + b.w &&
			a.x + a.w > b.x &&
			a.y < b.y + b.h &&
			a.y + a.h > b.y
			);
	}

	template <typename T, typename U, typename S>
	constexpr bool intersects_rectangle(const Rectangle<T>& a, const Rectangle<U>& b, rectF& output)noexcept {
		auto distances = (a.mPosition + a.get_half_dimensions()) - (b.mPosition + b.get_half_dimensions());
		auto overlap = (a.get_half_dimensions() + b.get_half_dimensions()) - abs_vector(distances);

		if (overlap.x < 0.0f || overlap.y < 0.0f) return false;

		output.mPosition = distances;
		output.mDimensions = overlap;

		return true;
	}

	constexpr bool intersects_projection(
		const vec2f& rayOrigin,
		const vec2f& rayVector,
		const rectF& target,
		float& dt,
		vec2f* contactPoint = nullptr,
		vec2f* contactNormal = nullptr) noexcept
	{
		auto reciprocal = reciprocal_vector(rayVector);
		auto tNear = (target.mPosition - rayOrigin) * reciprocal;
		auto tFar = (target.mPosition + target.mDimensions - rayOrigin) * reciprocal;
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
		dt = larger_value(tNear.x, tNear.y);
		float hitFar = smaller_value(tFar.x, tFar.y);

		//if hit but opposite direction, then no actual hit, just on same line
		if (hitFar < 0.0f) return false;
		//OPTIONAL: set the point where contact was made, idk what to do with it, can remove later tho
		if (contactPoint)
			*contactPoint = (rayVector * hitFar) + rayOrigin;
		//OPTIONAL: get normalized Sign value
		if (contactNormal)
			*contactNormal = sign_vector(rayVector);

		return true;
	}
	template<typename T, typename U>
	constexpr bool intersects_projection_adjusted(
		const Transform<T>& a,
		const Transform<U>& b,
		float& dt,
		vec2f* contactPoint = nullptr,
		vec2f* contactNormal = nullptr)noexcept
	{
		auto relativeVelocity = a.mVelocity - b.mVelocity;
		//no movement
		if (relativeVelocity.x == 0 && relativeVelocity.y == 0) return false;

		rectF expandedTarget = { b.mBox.mPosition - a.mBox.get_half_dimensions(), b.mBox.mDimensions + a.mBox.mDimensions };


		auto rayOrigin = a.mBox.get_center_point();

		if (intersects_projection(rayOrigin, relativeVelocity, expandedTarget, dt, contactPoint, contactNormal)) {
			return (dt >= 0.0f && dt < 1.0f);
		}
		return false;
	}


}