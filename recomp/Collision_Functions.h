#pragma once

#include "badUtility.h"
#include "Rectangle.h"
#include "Transform.h"
#include "SequenceM.h"


#include <algorithm>//for sort

namespace badEngine {


	class SweptAABB_collision_exe {

		struct SweptResult {
			TransformF* refA = nullptr;
			TransformF* refB = nullptr;
			float collisionTime = 1.0f;
			vec2f collisionNormal;
		};

		inline vec2f inv_entry(const rectF& objA, const rectF& objB, const vec2f& velocity)const noexcept {
			return vec2f(
				(velocity.x > 0.0f) ? objB.x - (objA.x + objA.w) : (objB.x + objB.w) - objA.x,
				(velocity.y > 0.0f) ? objB.y - (objA.y + objA.h) : (objB.y + objB.h) - objA.y
			);
		}
		inline vec2f inv_exit(const rectF& objA, const rectF& objB, const vec2f& velocity)const noexcept {
			return vec2f(
				(velocity.x > 0.0f) ? (objB.x + objB.w) - objA.x : objB.x - (objA.x + objA.w),
				(velocity.y > 0.0f) ? (objB.y + objB.h) - objA.y : objB.y - (objA.y + objA.h)
			);
		}
		inline vec2f entry_point(const vec2f& invEntry, const vec2f& velocity)const noexcept {
			return vec2f(
				(velocity.x == 0.0f) ? -std::numeric_limits<float>::infinity() : invEntry.x / velocity.x,
				(velocity.y == 0.0f) ? -std::numeric_limits<float>::infinity() : invEntry.y / velocity.y
			);
		}
		inline vec2f exit_point(const vec2f& invExit, const vec2f& velocity)const noexcept {
			return vec2f(
				(velocity.x == 0.0f) ? std::numeric_limits<float>::infinity() : invExit.x / velocity.x,
				(velocity.y == 0.0f) ? std::numeric_limits<float>::infinity() : invExit.y / velocity.y
			);
		}
		float sweptAABB(const rectF& objA, const rectF& objB, const vec2f& velocity, vec2f& normal)const noexcept;
		bool sweptAABB_dynamic_vs_dynamic(SweptResult& pair)const noexcept;
		rectF sweptAABB_expand_rect(const rectF& rect, const vec2f& velocity)const noexcept;
	public:

		SweptAABB_collision_exe() = default;

		SequenceM<SweptResult> get_colliders(SequenceM<TransformF>& objects);

	public:

		SweptAABB_collision_exe(const SweptAABB_collision_exe&) = delete;
		SweptAABB_collision_exe(SweptAABB_collision_exe&&)noexcept = delete;
		SweptAABB_collision_exe& operator=(const SweptAABB_collision_exe&) = delete;
		SweptAABB_collision_exe& operator=(SweptAABB_collision_exe&&)noexcept = delete;
	};

	//TEMPORARY: BECAUSE LATER WITH DIFFERENT ENTITY TYPES DIFFERENT RESOLUTION MAY BE DESIRED BUT SWEPT BEING ISOLATED IS FINE
	template<typename Policy>
	concept SweptCollisionResolvePolicy = std::invocable<Policy, TransformF&, float, const vec2f&>;

	template<SweptCollisionResolvePolicy Policy>
	void sweptAABB_algorithm(SequenceM<TransformF>& objects, Policy policy) {

		SweptAABB_collision_exe collisionExe;

		auto collisions = collisionExe.get_colliders(objects);

		//resolve collision immediately, then determine behavior for the next frame in via policy
		for (auto& colliders : collisions) {

			const float collisionTime = colliders.collisionTime;

			//move upto the point of touching
			const auto ACurrentVel = colliders.refA->get_currVel();
			const auto BCurrentVel = colliders.refB->get_currVel();

			colliders.refA->update_position_expression(ACurrentVel * collisionTime);
			colliders.refB->update_position_expression(BCurrentVel * collisionTime);
			//zero the current velocity
			colliders.refA->set_currVel({ 0,0 });
			colliders.refB->set_currVel({ 0,0 });
			//response behavior for velocity in next frame (only mods velocity)

			policy(*colliders.refA, colliders.collisionTime, colliders.collisionNormal);
			policy(*colliders.refB, colliders.collisionTime, colliders.collisionNormal);
		}
	}

	void objects_vs_container_resolved(SequenceM<TransformF>& objects, const rectI& container)noexcept;
}