#pragma once
#include "Vec2M.h"
#include "Rectangle.h"

namespace badEngine {
	struct Ray {
		float2 origin;
		float2 dir;
	};
	struct Hit {
		float2 pos;
		float t = INFINITY;
		int materialID = -1;

		constexpr bool is_hit(float len)const noexcept {
			return t >= 0.0f && t <= len;
		}
	};

	//maybe update if literture does it differently
	inline void sweep(Ray& ray, const float4& target, Hit& hit)noexcept
	{
		//inv dir
		float2 invdir(
			(ray.dir.x == 0.0f) ? INFINITY : 1.0f / ray.dir.x,
			(ray.dir.y == 0.0f) ? INFINITY : 1.0f / ray.dir.y
		);
		// Calculate intersections with rectangle bounding axes
		float2 t_near(
			(target.x - ray.origin.x) * invdir.x,
			(target.y - ray.origin.y) * invdir.y
		);

		float2 t_far(
			(target.x + target.w - ray.origin.x) * invdir.x,
			(target.y + target.h - ray.origin.y) * invdir.y
		);

		// Sort distances
		if (t_near.x > t_far.x) swap_numerical(t_near.x, t_far.x);
		if (t_near.y > t_far.y) swap_numerical(t_near.y, t_far.y);

		//times
		float t_hit_near = bad_maxV(t_near.x, t_near.y);
		float t_hit_far = bad_minV(t_far.x, t_far.y);

		//if no intersect or rectangle behind ray
		if (t_hit_near > t_hit_far || t_hit_far < 0.0f) return;

		// Closest 'time' will be the first contact
		hit.t = (t_hit_near < 0.0f) ? t_hit_far : t_hit_near;

		// point of impact
		hit.pos = ray.origin + hit.t * ray.dir;
	}
	//maybe depricate if literture does it differently
	Hit sweep_dynamic(const float4& dynamicBox, const float2& dynamicDir, const float4& staticBox) noexcept;

	/*
	DEPRICATED AND BORKEN ANYWAY

	template<std::input_iterator InputIt>
		requires std::same_as<Transform, std::iter_reference_t<InputIt>>//remove const cv_ref?
	bool sweptAABB_brute_force(InputIt first, InputIt last, const Transform& against) {
		for (; first != last; ++first) {
			
			if (*first.intersects(against) == false) {
				continue;
			}
			
			float time = 0;
			bool hit = sweptAABB_static(*first, against, time);
			if (hit) {
				return true;
			}
		}
		return false;
	}
	*/
}