#pragma once
#include "Vec2M.h"
#include "Rectangle.h"

namespace badEngine {
	//assumes dir is normalized to unit lenght
	struct Ray {
		float2 origin;
		float2 dir;
	};
	struct Hit {
		float2 pos;
		float t = INFINITY;
		int materialID = -1;// invalid index = -1

		//assumes dot lenght is passed, recommended
		constexpr bool is_hit_dot(float dot_lenght)const noexcept {
			return t >= 0.0f && (t * t) < dot_lenght;
		}
		//assumes sqrt lenght is passed
		constexpr bool is_hit_sqrt(float sqrt_lenght)const noexcept {
			return t >= 0.0f && t < sqrt_lenght;
		}
	};

	//assumes rays dir is already set in unit vector scale
	//rays should be constructed in bulk up front
	inline void sweep(const Ray& ray, const float4& target, Hit& hit)noexcept
	{
		//inv dir
		float2 invdir(
			(ray.dir.x == 0.0f) ? INFINITY : 1.0f / ray.dir.x,
			(ray.dir.y == 0.0f) ? INFINITY : 1.0f / ray.dir.y
		);
		// Calculate intersections with rectangle bounding axes
		float t_near_x = (target.x - ray.origin.x) * invdir.x;
		float t_far_x  = (target.x + target.w - ray.origin.x) * invdir.x;
		float t_near_y = (target.y - ray.origin.y) * invdir.y;
		float t_far_y  = (target.y + target.h - ray.origin.y) * invdir.y;

		float t_hit_near = bad_maxV(bad_minV(t_near_x, t_far_x), bad_minV(t_near_y, t_far_y));
		float t_hit_far = bad_minV(bad_maxV(t_near_x, t_far_x), bad_maxV(t_near_y, t_far_y));

		//if no intersect or rectangle behind ray
		if (t_hit_near > t_hit_far || t_hit_far < 0.0f) return;
		
		// Closest 'time' will be the first contact
		hit.t = (t_hit_near < 0.0f) ? t_hit_far : t_hit_near;
		
		// point of impact
		hit.pos = ray.origin + hit.t * ray.dir;
	}

	//assumes rays dir is already set in unit vector scale
	//rays should be constructed in bulk up front
	inline bool sweep_fast(const Ray& ray, const float4& target)noexcept
	{
		//inv dir
		float2 invdir(
			(ray.dir.x == 0.0f) ? INFINITY : 1.0f / ray.dir.x,
			(ray.dir.y == 0.0f) ? INFINITY : 1.0f / ray.dir.y
		);
		float t_near_x = (target.x - ray.origin.x) * invdir.x;
		float t_far_x = (target.x + target.w - ray.origin.x) * invdir.x;
		float t_near_y = (target.y - ray.origin.y) * invdir.y;
		float t_far_y = (target.y + target.h - ray.origin.y) * invdir.y;

		float t_hit_near = bad_maxV(bad_minV(t_near_x, t_far_x), bad_minV(t_near_y, t_far_y));
		float t_hit_far = bad_minV(bad_maxV(t_near_x, t_far_x), bad_maxV(t_near_y, t_far_y));

		return (t_hit_near <= t_hit_far && t_hit_far >= 0.0f);
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