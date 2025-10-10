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




	struct Rect3D {
		float xmin, ymin, zmin;
		float xmax, ymax, zmax;
	};
	Rect3D make_swept_box(const TransformF& box) {
		return{
			std::min(box.mBox.x, box.mBox.x + box.mVelocity.x),
			std::min(box.mBox.y, box.mBox.y + box.mVelocity.y),
			0.0f,
			std::max(box.mBox.x + box.mBox.w, box.mBox.x + box.mVelocity.x + box.mBox.w),
			std::max(box.mBox.y + box.mBox.h, box.mBox.y + box.mVelocity.y + box.mBox.h),
			1.0f
		};
	}
	bool rect3D_vs_rect3D(const Rect3D&a, const Rect3D& b) {
		return (
			a.xmin < b.xmax && a.xmax > b.xmin &&
			a.ymin < b.ymax && a.ymax > b.ymin &&
			a.zmin < b.zmax && a.zmax > b.zmin
			);
	}
	bool swept_rect3D(TransformF& box1, TransformF& box2) {

		Rect3D b1 = make_swept_box(box1);
		Rect3D b2 = make_swept_box(box2);


		if (rect3D_vs_rect3D(b1, b2)) {
			return true;
			//collision?
		}
		return false;
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
	rectF make_broad_phase_box(const TransformF& box) {
		return rectF(
			(box.mVelocity.x > 0) ? box.mBox.x : box.mBox.x + box.mVelocity.x,
			(box.mVelocity.y > 0) ? box.mBox.y : box.mBox.y + box.mVelocity.y,
			(box.mVelocity.x > 0) ? box.mVelocity.x + box.mBox.w : box.mBox.w - box.mVelocity.x,
			(box.mVelocity.y > 0) ? box.mVelocity.y + box.mBox.h : box.mBox.h - box.mVelocity.y
		);
	}
	bool ray_vs_rect(const vec2f ray_origin, const vec2f ray_dir, const rectF& target, vec2f& contactNormal, float& contactTime) {
		// Cache division
		vec2f invdir = 1.0f / ray_dir;

		// Calculate intersections with rectangle bounding axes
		vec2f t_near = vec2f(
			(target.x - ray_origin.x) * invdir.x,
			(target.y - ray_origin.y) * invdir.y
		);
		vec2f t_far = vec2f(
			(target.x + target.w - ray_origin.x) * invdir.x,
			(target.y + target.h - ray_origin.y) * invdir.y
		);

		if (std::isnan(t_far.y) || std::isnan(t_far.x)) return false;
		if (std::isnan(t_near.y) || std::isnan(t_near.x)) return false;

		// Sort distances
		if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
		if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

		// Early rejection		
		if (t_near.x > t_far.y || t_near.y > t_far.x) return false;

		// Closest 'time' will be the first contact
		contactTime = std::max(t_near.x, t_near.y);

		// Furthest 'time' is contact on opposite side of target
		float t_hit_far = std::min(t_far.x, t_far.y);

		// Reject if ray direction is pointing away from object
		if (t_hit_far < 0)
			return false;

		if (t_near.x > t_near.y)
			if (invdir.x < 0)
				contactNormal = { 1, 0 };
			else
				contactNormal = { -1, 0 };
		else if (t_near.x < t_near.y)
			if (invdir.y < 0)
				contactNormal = { 0, 1 };
			else
				contactNormal = { 0, -1 };

		// Note if t_near == t_far, collision is principly in a diagonal
		// so pointless to resolve. By returning a CN={0,0} even though its
		// considered a hit, the resolver wont change anything.
		return true;
	}
	float another_swept_check(TransformF& box1, TransformF& box2, vec2f& normal) {
		float xInvEntry, yInvEntry;
		float xInvExit, yInvExit;

		auto& b1 = box1.mBox;
		auto& b2 = box2.mBox;
		// find the distance between the objects on the near and far sides for both x and y 
		if (box1.mVelocity.x > 0.0f)
		{
			xInvEntry = b2.x - (b1.x + b1.w);
			xInvExit = (b2.x + b2.w) - b1.x;
		}
		else
		{
			xInvEntry = (b2.x + b2.w) - b1.x;
			xInvExit = b2.x - (b1.x + b1.w);
		}

		if (box1.mVelocity.y > 0.0f)
		{
			yInvEntry = b2.y - (b1.y + b1.h);
			yInvExit = (b2.y + b2.h) - b1.y;
		}
		else
		{
			yInvEntry = (b2.y + b2.h) - b1.y;
			yInvExit = b2.y - (b1.y + b1.h);
		}
		// find time of collision and time of leaving for each axis (if statement is to prevent divide by zero) 
		float xEntry, yEntry;
		float xExit, yExit;

		if (box1.mVelocity.x == 0.0f)
		{
			xEntry = -std::numeric_limits<float>::infinity();
			xExit = std::numeric_limits<float>::infinity();
		}
		else
		{
			xEntry = xInvEntry / box1.mVelocity.x;
			xExit = xInvExit / box1.mVelocity.x;
		}

		if (box1.mVelocity.y == 0.0f)
		{
			yEntry = -std::numeric_limits<float>::infinity();
			yExit = std::numeric_limits<float>::infinity();
		}
		else
		{
			yEntry = yInvEntry / box1.mVelocity.y;
			yExit = yInvExit / box1.mVelocity.y;
		}
		// find the earliest/latest times of collisionfloat 
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
	bool do_swept_collision(TransformF& box1, TransformF& box2, vec2f& contactNormal, float& contactTime) {

		rectF broadPhaseBox = make_broad_phase_box(box2);

		if (rect_vs_rect(box1.mBox, broadPhaseBox)) {

			vec2f relativeVelocity = box1.mVelocity - box2.mVelocity;

			if (ray_vs_rect(box1.mBox.get_center_point(), relativeVelocity, box2.mBox, contactNormal, contactTime)) {

				return (contactTime >= 0.0f && contactTime < 1.0f);
			}
		}
		return false;
	}
}
