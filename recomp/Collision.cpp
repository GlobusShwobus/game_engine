#include "Collision.h"

namespace badEngine {
	bool sweptAABB_static(const vec2f& ray_origin, const vec2f& ray_dir, const rectF& target, float& t_hit_near, vec2i* contact_normal)
	{
		// Cache division
		vec2f invdir = 1.0f / ray_dir;

		// Calculate intersections with rectangle bounding axes
		vec2f t_near = (target.get_pos() - ray_origin) * invdir;
		vec2f t_far = (target.get_pos() + target.get_size() - ray_origin) * invdir;

		if (std::isnan(t_far.y) || std::isnan(t_far.x)) return false;
		if (std::isnan(t_near.y) || std::isnan(t_near.x)) return false;

		// Sort distances
		if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
		if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

		// Early rejection		
		if (t_near.x > t_far.y || t_near.y > t_far.x) return false;

		// Closest 'time' will be the first contact
		t_hit_near = mValue_max(t_near.x, t_near.y);

		// Furthest 'time' is contact on opposite side of target
		float t_hit_far = mValue_min(t_far.x, t_far.y);

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


	bool AABB_intersect_get_overlap(const rectF& A, const rectF& B, vec2f& overlap)noexcept {

		const vec2f centerPointDistance = A.get_center_point() - B.get_center_point();

		overlap = A.get_half_size() + B.get_half_size() - abs_vector(centerPointDistance);

		if (overlap.x <= 0 || overlap.y <= 0)
			return false;

		return true;
	}
	bool AABB_intersect_get_displace(const rectF& A, const rectF& B, const rectF& Avel, vec2f& displace)noexcept {


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