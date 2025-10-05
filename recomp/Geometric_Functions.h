#pragma once

#include <cmath>
#include "Vec2M.h"
#include "Rectangle.h"
#include "Transform.h"

namespace badEngine {

	//template <typename T, typename U> requires IS_LESS_THAN_COMPARABLE<T, U>
	//constexpr auto larger_value(const T& x, const U& y)noexcept {
	//	return (x < y) ? y : x;
	//}
	//template<typename T, typename U> requires IS_LESS_THAN_COMPARABLE<T, U>
	//constexpr auto smaller_value(const T& x, const U& y)noexcept {
	//	return (x < y) ? x : y;
	//}
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
	constexpr auto normal_vector(Vec2M<T> v)noexcept {
		if (v.x > 0) v.x = 1;
		else if (v.x < 0) v.x = -1;

		if (v.y > 0) v.y = 1;
		else if (v.y < 0) v.y = -1;

		return v;
	}
	//template<typename T, typename U>
	//constexpr vec2i normal_vector_on_entry(const Vec2M<T>& entry, const Vec2M<U>& direction)noexcept {
	//	if (entry.x > entry.y)
	//		if (direction.x < 0)
	//			return { 1, 0 };
	//		else
	//			return { -1, 0 };
	//	else if (entry.x < entry.y)
	//		if (direction.y < 0)
	//			return { 0, 1 };
	//		else
	//			return { 0, -1 };
	//	return {0,0};
	//}

	template <typename T, typename U> requires IS_MATHMATICAL_T<U>
	constexpr bool intersects_rectangle(const Rectangle<T>& rect, U X, U Y)noexcept {
		return (
			X >= rect.mPosition.x &&
			Y >= rect.mPosition.y &&
			X < rect.mPosition.x + rect.mDimensions.x &&
			Y < rect.mPosition.y + rect.mDimensions.y);
	}

	template <typename T, typename U>
	constexpr bool intersects_rectangle(const Rectangle<T>& rect, Vec2M<U>& pos)noexcept {
		return intersects_rectangle(rect, pos.x, pos.y);
	}

	template <typename T, typename U>
	constexpr bool intersects_rectangle(const Rectangle<T>& a, const Rectangle<U>& b)noexcept {
		return (
			a.mPosition.x < b.mPosition.x + b.mDimensions.x &&
			a.mPosition.x + a.mDimensions.x > b.mPosition.x &&
			a.mPosition.y < b.mPosition.y + b.mDimensions.y &&
			a.mPosition.y + a.mDimensions.y > b.mPosition.y
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

	bool swept_AABB(const rectF& a, const rectF& b, float& penetration, vec2f& normal)noexcept {
		//X axis projection
		float aXmin = a.mPosition.x;
		float aXmax = a.mPosition.x + a.mDimensions.x;
		float bXmin = b.mPosition.x;
		float bXmax = b.mPosition.x + b.mDimensions.x;
		//Y axis projection
		float aYmin = a.mPosition.y;
		float aYmax = a.mPosition.y + a.mDimensions.y;
		float bYmin = b.mPosition.y;
		float bYmax = b.mPosition.y + b.mDimensions.y;
	
		//check for non-intersection
	
		float overlapX = std::min(aXmax, bXmax) - std::max(aXmin, bXmin);
		if (overlapX <= 0.0f) return false;
	
		float overlapY = std::min(aYmax, bYmax) - std::max(aYmin, bYmin);
		if (overlapY <= 0.0f) return false;
	
		//choose axis of least penetration (MTV)
		float aCenterX = aXmin + 0.5f * a.mDimensions.x;
		float bCenterX = bXmin + 0.5f * b.mDimensions.x;
		float aCenterY = aYmin + 0.5f * a.mDimensions.y;
		float bCenterY = bYmin + 0.5f * b.mDimensions.y;
	
		if (overlapX < overlapY) {
			penetration = overlapX;
			//if A is left of B, push A left (negative x) otherwise push right
			normal = (aCenterX < bCenterX) ? vec2f(-1.0f, 0.0f) : vec2f(1.0f, 0.0f);
		}
		else {
			penetration = overlapY;
			//if A is above B, push A up (negative y) otherwise push down
			normal = (aCenterY < bCenterY) ? vec2f(0.0f, -1.0f) : vec2f(0.0f, 1.0f);
		}
	
		return true;
	}
	bool swept_AABB_with_resolve(rectF& a, const rectF& b)noexcept {
		vec2f normal;
		float penetration = 0.0f;
		
		
		if (!swept_AABB(a, b, penetration, normal))
			return false;
	
		//move A out of collision by the MTV
		a.mPosition += normal * penetration;
	
		return true;
	}

	float sweptAABB_FINAL(TransformF& a, TransformF&b, vec2f& normal) {
		float xInvEntry, yInvEntry;
		float xInvExit, yInvExit;

		if (a.mVelocity.x > 0.0f) {
			xInvEntry = b.mBox.mPosition.x - (a.mBox.mPosition.x + a.mBox.mDimensions.x);
			xInvEntry = (b.mBox.mPosition.x + b.mBox.mDimensions.x) - a.mBox.mPosition.x;
		}
		else {
			xInvEntry = (b.mBox.mPosition.x + b.mBox.mDimensions.x) - a.mBox.mPosition.x;
			xInvExit = b.mBox.mPosition.x - (a.mBox.mPosition.x + a.mBox.mDimensions.x);
		}

		if (a.mVelocity.y > 0.0f) {
			yInvEntry = b.mBox.mPosition.y - (a.mBox.mPosition.y + a.mBox.mDimensions.y);
		}
		else {

		}
	}

	//bool intersects_ray_rect_basic(
	//	const vec2f& rayOrigin,
	//	const vec2f& rayDir,
	//	const rectF& target,
	//	float& contactTime,
	//	vec2f* contactPoint = nullptr,
	//	vec2f* contactNormal = nullptr) noexcept
	//{
	//	vec2f invdir = 1.0f / rayDir;//reciprocal?
	//	auto tNear = (target.mPosition - rayOrigin) * invdir;
	//	auto tFar = (target.mPosition + target.mDimensions - rayOrigin) * invdir;
	//	//broken float value (division by 0 probably)
	//	if (std::isnan(tFar.y) || std::isnan(tFar.x))
	//		return false;
	//	if (std::isnan(tNear.y) || std::isnan(tNear.x))
	//		return false;
	//	
	//	//order
	//	if (tNear.x > tFar.x) std::swap(tNear.x, tFar.x);
	//	if (tNear.y > tFar.y) std::swap(tNear.y, tFar.y);
	//	//if no hit == false
	//	if (tNear.x > tFar.y || tNear.y > tFar.x) 
	//		return false;
	//
	//	contactTime  = std::max(tNear.x, tNear.y);
	//	float hitFar = std::min(tFar.x, tFar.y);
	//
	//	//reject if ray direction is pointing away from object
	//	if (hitFar < 0)
	//		return false;
	//	
	//	//OPTIONAL: set the point where contact was made
	//	if (contactPoint)
	//		*contactPoint = rayOrigin + contactTime * rayDir;
	//	//OPTIONAL: get normalized Sign value
	//	if (contactNormal)
	//		*contactNormal = normal_vector_on_entry(tNear, invdir);
	//
	//	return true;
	//}
	//template<typename T, typename U>
	//bool intersects_ray_rect_adjusted(
	//	const Transform<T>& dynamic,
	//	const Transform<U>& stationary,
	//	float& contactTime,
	//	vec2f* contactPoint = nullptr,
	//	vec2f* contactNormal = nullptr)noexcept
	//{
	//	if (dynamic.mVelocity.x == 0 && dynamic.mVelocity.y == 0)
	//		return false;
	//
	//	rectF expandedTarget;
	//	expandedTarget.mPosition = stationary.mBox.mPosition - dynamic.mBox.get_half_dimensions();
	//	expandedTarget.mDimensions = stationary.mBox.mDimensions + dynamic.mBox.mDimensions;
	//
	//	if (intersects_ray_rect_basic(dynamic.mBox.get_center_point(), dynamic.mVelocity, expandedTarget, contactTime, contactPoint, contactNormal)) {
	//		return (contactTime >= 0.0f && contactTime < 1.0f);
	//	}
	//	else {
	//		return false;
	//	}
	//}
}
