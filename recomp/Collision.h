#pragma once

#include "Rectangle.h"
#include "Transform.h"

namespace badEngine {
	
	//SWEPT IS AN EXPENSIVE CONTINUOUS COLLISION CHECK
	//USE ONLY FOR FAST MOVING OBJECTS OR THE PLAYER CHARACTER FOR BEST DETECTION
	//ASSUMES ONE OBJECT IS DYNAMIC, OTHER STATIONARY

	//######################################################################################################################
	bool sweptAABB_static(const vec2f& ray_origin, const vec2f& ray_dir, const rectF& target, float& t_hit_near, vec2i* contact_normal = nullptr) noexcept;
	bool sweptAABB_dynamic(const Transform& dynamic, const Transform& stationary, float& collisionTime, vec2i* contact_normal = nullptr) noexcept;
	//######################################################################################################################

	
	bool AABB_intersect_get_overlap(const rectF& A, const rectF& B, vec2f& overlap)noexcept;
	bool AABB_intersect_get_displace(const rectF& A, const rectF& B, const vec2f& Avel, vec2f& displace)noexcept;

}