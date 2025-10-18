#pragma once

#include <cmath>
#include <limits>
#include "Vec2M.h"
#include "Rectangle.h"
#include "Transform.h"

namespace badEngine {

	template <typename T, typename U> requires IS_LESS_THAN_COMPARABLE<T, U>
	constexpr auto mValue_max(const T& x, const U& y)noexcept {
		return (x < y) ? y : x;
	}
	template<typename T, typename U> requires IS_LESS_THAN_COMPARABLE<T, U>
	constexpr auto mValue_min(const T& x, const U& y)noexcept {
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
	constexpr bool rect_vs_rect(const Rectangle<T>& b1, const Rectangle<U>& b2)noexcept {
		return
			b1.x < b2.x + b2.w &&
			b1.x + b1.w > b2.x &&
			b1.y < b2.y + b2.h &&
			b1.y + b1.h > b2.y;
	}
	float AABB_SWEPT(const rectF& objA, const rectF& objB, const vec2f& relativeVel, vec2f& normal) {
		float xInvEntry, yInvEntry;
		float xInvExit, yInvExit;

		// find the distance between the objects on the near and far sides for both x and y 

		if (relativeVel.x > 0.0f)
		{
			xInvEntry = objB.x - (objA.x + objA.w);
			xInvExit = (objB.x + objB.w) - objA.x;
		}
		else
		{
			xInvEntry = (objB.x + objB.w) - objA.x;
			xInvExit = objB.x - (objA.x + objA.w);
		}

		if (relativeVel.y > 0.0f)
		{
			yInvEntry = objB.y - (objA.y + objA.h);
			yInvExit = (objB.y + objB.h) - objA.y;
		}
		else
		{
			yInvEntry = (objB.y + objB.h) - objA.y;
			yInvExit = objB.y - (objA.y + objA.h);
		}

		float xEntry, yEntry;
		float xExit, yExit;

		if (relativeVel.x == 0.0f)
		{
			xEntry = -std::numeric_limits<float>::infinity();
			xExit = std::numeric_limits<float>::infinity();
		}
		else
		{
			xEntry = xInvEntry / relativeVel.x;
			xExit = xInvExit / relativeVel.x;
		}

		if (relativeVel.y == 0.0f)
		{
			yEntry = -std::numeric_limits<float>::infinity();
			yExit = std::numeric_limits<float>::infinity();
		}
		else
		{
			yEntry = yInvEntry / relativeVel.y;
			yExit = yInvExit / relativeVel.y;
		}


		float entryTime = std::max(xEntry, yEntry);
		float exitTime = std::min(xExit, yExit);

		if (entryTime > exitTime || xEntry < 0.0f && yEntry < 0.0f || xEntry > 1.0f || yEntry > 1.0f)
		{
			normal = vec2f(0, 0);
			return 1.0f;
		}
		else // if there was a collision 
		{
			// calculate normal of collided surface
			if (xEntry > yEntry)
			{
				if (xInvEntry < 0.0f)
				{
					normal = vec2f(1, 0);
				}
				else
				{
					normal = vec2f(-1, 0);
				}
			}
			else
			{
				if (yInvEntry < 0.0f)
				{
					normal = vec2f(0, 1);
				}
				else
				{
					normal = vec2f(0, -1);
				}
			} // return the time of collisionreturn entryTime; 
		}
		return entryTime;
	}

	bool do_collision(TransformF& objA, TransformF& objB, vec2f& normal, float& contactTime) {

		vec2f relativeVel = objA.mCurrVelocity - objB.mCurrVelocity;
		rectF expandedA = objA.get_expanded_rect(relativeVel);

		if (!rect_vs_rect(expandedA, objB.mBox)) {
			return false;
		}

		contactTime = AABB_SWEPT(objA.mBox, objB.mBox, relativeVel, normal);

		return (contactTime >= 0.f && contactTime < 1.f);
	}

}
/*
* 
* 
* 	vec2f get_swept_result_normal(const vec2f& entryTime, const vec2f& reciprocal) {
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

		
		//set the point where contact was made, idk what to do with it, can remove later tho
		if (contactPoint)
			*contactPoint = (rayVector * hitFar) + rayOrigin;
		

		//get normal
contactNormal = get_swept_result_normal(tNear, reciprocal);

return true;
	}

	template <typename T, typename U>
	constexpr bool rect_vs_rect(const Rectangle<T>& a, const Rectangle<U>& b, vec2f& output)noexcept {

		auto distances = (a.get_center_point() - b.get_center_point());
		auto overlap   = (a.get_half_size() + b.get_half_size()) - abs_vector(distances);

		if (overlap.x < 0.0f || overlap.y < 0.0f)
			return false;

		if (overlap.x < overlap.y) {
			output = vec2f(
				(distances.x > 0) ? overlap.x : -overlap.x,
				0.0f
			);
		}
		else {
			output = vec2f(
				0.0f,
				(distances.y > 0) ? overlap.y : -overlap.y
			);
		}

		return true;
	}

	template <typename T, typename U>
	constexpr bool container_vs_rect(const Rectangle<T>& bigBox, const Rectangle<U>& smallBox, vec2f& displacement)noexcept {

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
*/
