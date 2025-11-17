#pragma once

#include "Rectangle.h"
#include "Transform.h"

namespace badEngine {
	
	//SWEPT IS AN EXPENSIVE CONTINUOUS COLLISION CHECK
	//USE ONLY FOR FAST MOVING OBJECTS OR THE PLAYER CHARACTER FOR BEST DETECTION
	//ASSUMES ONE OBJECT IS DYNAMIC, OTHER STATIONARY

	//######################################################################################################################
	bool sweptAABB_static(const vec2f& ray_origin, const vec2f& ray_dir, const rectF& target, float& t_hit_near, vec2i* contact_normal = nullptr);

	template <typename T>
	bool sweptAABB_dynamic(const Transform<T>& dynamic, const Transform<T>& stationary, float& collisionTime, vec2i* contact_normal = nullptr) noexcept {

		//if relative velocity is 0, then no collision
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
	//######################################################################################################################
}