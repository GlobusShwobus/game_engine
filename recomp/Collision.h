#pragma once
#include <iterator>
#include "Rectangle.h"
#include "Transform.h"

namespace badEngine {
	
	//SWEPT IS AN EXPENSIVE CONTINUOUS COLLISION CHECK
	//USE ONLY FOR FAST MOVING OBJECTS OR THE PLAYER CHARACTER FOR BEST DETECTION
	//ASSUMES ONE OBJECT IS DYNAMIC, OTHER STATIONARY

	//######################################################################################################################
	bool sweptAABB_static(const vec2f& ray_origin, const vec2f& ray_dir, const rectF& target, float& t_hit_near, vec2i* contact_normal = nullptr, vec2f* contact_point = nullptr) noexcept;
	bool sweptAABB_dynamic(const Transform& dynamic, const Transform& stationary, float& collisionTime, vec2i* contact_normal = nullptr, vec2f* contact_point = nullptr) noexcept;
	
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
	//######################################################################################################################

	
	bool AABB_intersect_get_overlap(const rectF& A, const rectF& B, vec2f& overlap)noexcept;
	bool AABB_intersect_get_displace(const rectF& A, const rectF& B, const vec2f& Avel, vec2f& displace)noexcept;

}