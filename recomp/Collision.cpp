#include "Collision.h"

namespace badEngine {
	bool sweptAABB_static(const vec2f& ray_origin, const vec2f& ray_dir, const rectF& target, float& t_hit_near, vec2i* contact_normal)noexcept
	{
		// Cache vals
		auto targetPos = target.get_pos();
		auto targetSize = target.get_size();

		// Cache division
		vec2f invdir = vec2f(
			(ray_dir.x == 0.0f) ? INFINITY : 1.0f / ray_dir.x,
			(ray_dir.y == 0.0f) ? INFINITY : 1.0f / ray_dir.y
		);
		// Calculate intersections with rectangle bounding axes
		vec2f t_near(
			(targetPos.x - ray_origin.x) * invdir.x,
			(targetPos.y - ray_origin.y) * invdir.y
		);

		vec2f t_far(
			(targetPos.x + targetSize.x - ray_origin.x) * invdir.x,
			(targetPos.y + targetSize.y - ray_origin.y) * invdir.y
		);
		// Is a number test
		if (bad_isNaN(t_far.y) ||  bad_isNaN(t_far.x)) return false;
		if (bad_isNaN(t_near.y) || bad_isNaN(t_near.x)) return false;

		// Sort distances
		if (t_near.x > t_far.x) swap_numerical(t_near.x, t_far.x);
		if (t_near.y > t_far.y) swap_numerical(t_near.y, t_far.y);

		// Early rejection		
		if (t_near.x > t_far.y || t_near.y > t_far.x) return false;

		// Closest 'time' will be the first contact
		t_hit_near = bad_maxV(t_near.x, t_near.y);

		// Furthest 'time' is contact on opposite side of target
		float t_hit_far = bad_minV(t_far.x, t_far.y);

		// Reject if ray direction is pointing away from object
		if (t_hit_far < 0)
			return false;

		if (contact_normal) {
			if (t_near.x > t_near.y)
				if (invdir.x < 0)
					*contact_normal = vec2i(1, 0);
				else
					*contact_normal = vec2i(-1, 0);
			else if (t_near.x < t_near.y)
				if (invdir.y < 0)
					*contact_normal = vec2i(0, 1);
				else
					*contact_normal = vec2i(0, -1);
		}
		// Note if t_near == t_far, collision is principly in a diagonal
		// so pointless to resolve. By returning a CN={0,0} even though its
		// considered a hit, the resolver wont change anything.
		return true;
	}
	bool sweptAABB_dynamic(const Transform& dynamic, const Transform& stationary, float& collisionTime, vec2i* contact_normal) noexcept 
	{
		//if stationary (maybe should omit it, maybe not, depending on scripting)
		if (dynamic.mCurrVelocity.x == 0 && dynamic.mCurrVelocity.y == 0)
			return false;
		//expanded rectangle must also be with the consideration of relative velocity
		const rectF expandedA = rectF(
			stationary.mBox.get_pos() - dynamic.mBox.get_half_size(),
			stationary.mBox.get_size() + dynamic.mBox.get_size()
		);

		//get contact time of collision, default should be 1.0f which means no collision in the span of the current frame
		if (sweptAABB_static(dynamic.mBox.get_center_point(), dynamic.mCurrVelocity, expandedA, collisionTime, contact_normal))
			return (collisionTime >= 0.0f && collisionTime < 1.0f);

		else
			return false;
	}


	bool AABB_intersect_get_overlap(const rectF& A, const rectF& B, vec2f& overlap)noexcept
	{
		const vec2f centerPointDistance = A.get_center_point() - B.get_center_point();

		overlap = A.get_half_size() + B.get_half_size() - abs_vector(centerPointDistance);

		if (overlap.x <= 0 || overlap.y <= 0)
			return false;

		return true;
	}
	bool AABB_intersect_get_displace(const rectF& A, const rectF& B, const vec2f& Avel, vec2f& displace)noexcept
	{
		if (!AABB_intersect_get_overlap(A, B, displace))
			return false;

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
}