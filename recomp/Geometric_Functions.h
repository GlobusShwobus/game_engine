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

	float sweptAABB_basic(const TransformF& a, const TransformF& b, vec2f& normal) noexcept {
		constexpr float EPSILON = 1e-6f;

		// Compute inverse entry/exit distances
		float xInvEntry, xInvExit;
		if (a.mVelocity.x > 0.0f) {
			xInvEntry = b.mBox.x - (a.mBox.x + a.mBox.w);
			xInvExit = (b.mBox.x + b.mBox.w) - a.mBox.x;
		}
		else {
			xInvEntry = (b.mBox.x + b.mBox.w) - a.mBox.x;
			xInvExit = b.mBox.x - (a.mBox.x + a.mBox.w);
		}

		float yInvEntry, yInvExit;
		if (a.mVelocity.y > 0.0f) {
			yInvEntry = b.mBox.y - (a.mBox.y + a.mBox.h);
			yInvExit = (b.mBox.y + b.mBox.h) - a.mBox.y;
		}
		else {
			yInvEntry = (b.mBox.y + b.mBox.h) - a.mBox.y;
			yInvExit = b.mBox.y - (a.mBox.y + a.mBox.h);
		}

		// Convert to entry/exit times
		float xEntry = (a.mVelocity.x == 0.0f) ? -INFINITY : xInvEntry / a.mVelocity.x;
		float xExit = (a.mVelocity.x == 0.0f) ? INFINITY : xInvExit / a.mVelocity.x;

		float yEntry = (a.mVelocity.y == 0.0f) ? -INFINITY : yInvEntry / a.mVelocity.y;
		float yExit = (a.mVelocity.y == 0.0f) ? INFINITY : yInvExit / a.mVelocity.y;

		// Find earliest/latest collision times
		float entryTime = std::max(xEntry, yEntry);
		float exitTime = std::min(xExit, yExit);

		// No collision this frame?
		if (entryTime > exitTime || (xEntry < 0.0f && yEntry < 0.0f) || entryTime > 1.0f) {
			normal = { 0.0f, 0.0f };
			return 1.0f;
		}

		// Determine surface normal
		if (xEntry > yEntry) {
			normal = (a.mVelocity.x < 0.0f) ? vec2f{ 1, 0 } : vec2f{ -1, 0 };
		}
		else {
			normal = (a.mVelocity.y < 0.0f) ? vec2f{ 0, 1 } : vec2f{ 0, -1 };
		}

		return entryTime;
	}
	void collision_response_deflect(TransformF& box, float remainingTime, const vec2f& normal) {
		box.mVelocity *= remainingTime;

		float dot = dot_vector(box.mVelocity, normal);
		box.mVelocity -= 2.0f * dot * normal;
	}
	void collision_response_push(TransformF& box, float remainingTime, const vec2f& normal) {
	
		vec2f tangent = vec2f(normal.y, normal.x);
		float speed = length_vector(box.mVelocity) * remainingTime;
		float dot = dot_vector(box.mVelocity, tangent);

		tangent = normal_vector(tangent);

		box.mVelocity = tangent * (dot >= 0 ? speed : -speed);
	}
	void collision_response_slide(TransformF& box, float remainingTime, const vec2f& normal) {
		
		vec2f tangent = vec2f(normal.y, normal.x);
		tangent = normal_vector(tangent);

		float dot = dot_vector(box.mVelocity, tangent);

		box.mVelocity = tangent * dot * remainingTime;
	}
	rectF get_broadbox(const TransformF& box)noexcept {
		rectF broadbox;
		broadbox.x = box.mVelocity.x > 0 ? box.mBox.x : box.mBox.x + box.mVelocity.x;
		broadbox.y = box.mVelocity.y > 0 ? box.mBox.y : box.mBox.y + box.mVelocity.y;
		broadbox.w = box.mVelocity.x > 0 ? box.mVelocity.x + box.mBox.w : box.mBox.w - box.mVelocity.x;
		broadbox.h = box.mVelocity.y > 0 ? box.mVelocity.y + box.mBox.h : box.mBox.h - box.mVelocity.y;

		return broadbox;
	}

	template <typename T>
	concept AABB_PREDICATE_T = std::invocable<T,TransformF&, float, const vec2f&>;

	template<typename Condition>
		requires AABB_PREDICATE_T<Condition>
	void AABB_collision_algorithm(TransformF& moving, TransformF& stationary, Condition predicate)noexcept {

		rectF broadbox = get_broadbox(moving);

		if (!AABB_contains(broadbox, stationary.mBox))
			return;
	
		vec2f normal;
		float collisionTime = sweptAABB_basic(moving, stationary, normal);
		moving.mBox.x += moving.mVelocity.x * collisionTime;
		moving.mBox.y += moving.mVelocity.y * collisionTime;

		if (collisionTime < 1.0f) {
			float remainingTime = 1.0f - collisionTime;
			predicate(moving, remainingTime, normal);
		}

	}
	/*
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
	*/
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
