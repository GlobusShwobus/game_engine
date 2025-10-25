#pragma once

#include "badUtility.h"
#include "Rectangle.h"
#include "Transform.h"
#include "SequenceM.h"


#include <algorithm>//for sort
/*
#include <limits>
*/

namespace badEngine {

	struct SweptAABB_Data {
		/*
		non owning pointers, can't be smart because not cool to call delete
		*/
		TransformF* refA = nullptr;
		TransformF* refB = nullptr;
		float collisionTime = 1.0f;
		vec2f collisionNormal;
	};

	template<typename T>
	rectF sweptAABB_expand_rect(const Rectangle<T>& rect, const Vec2M<T>& velocity)noexcept {
		return Rectangle<T>(
			(velocity.x > 0) ? rect.x : rect.x + velocity.x,
			(velocity.y > 0) ? rect.y : rect.y + velocity.y,
			rect.w + std::abs(velocity.x),
			rect.h + std::abs(velocity.y)
		);
	}

	float sweptAABB(const rectF& objA, const rectF& objB, const vec2f& velocity, vec2f& normal);
	
	template <typename T>
	bool sweptAABB_dynamic_vs_dynamic(Transform<T>& objA, Transform<T>& objB, vec2f& normal, float& contactTime) {

		vec2f relativeVel = objA.get_currVel() - objB.get_currVel();
		rectF expandedA = sweptAABB_expand_rect(objA.get_rectangle(), relativeVel);

		if (!expandedA.intersects_rect(objB.get_rectangle())) {
			return false;
		}

		contactTime = sweptAABB(objA.get_rectangle(), objB.get_rectangle(), relativeVel, normal);

		return (contactTime >= 0.f && contactTime < 1.f);
	}

	SequenceM<SweptAABB_Data> determine_colliders(SequenceM<TransformF>& objects);


	struct PhysicsData {
		float collisionTime = 1.0f;
		vec2f collisionNormal;
	};

	template<typename Policy>
	concept CollisionResponsePolicy = std::invocable<Policy, TransformF&, TransformF&, PhysicsData>;

	template <CollisionResponsePolicy ExecutionPolicy>
	void collision_algorithm(SequenceM<TransformF>& objects, ExecutionPolicy&& policy) {
		//determine who collides
		auto collisionResults = determine_colliders(objects);
		//sort order of priority
		std::sort(collisionResults.begin(), collisionResults.end(), [](const auto& a, const auto& b) {
			return a.collisionTime < b.collisionTime;
			});

		//resolve collision immediately, then determine behavior for the next frame in via policy
		for (auto& colliders : collisionResults) {

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

			std::forward<ExecutionPolicy>(policy)(
				*colliders.refA,
				*colliders.refB, 
				PhysicsData(colliders.collisionTime, colliders.collisionNormal)
				);//can take both named and unnamed functors
		}
	}




	void objects_vs_container_resolved(SequenceM<TransformF>& objects, const rectI& container)noexcept;

	template<CollisionResponsePolicy ExecutionPolicy>
	void collision_algorithm_executable(SequenceM<TransformF>& objects, rectI globalBorders, ExecutionPolicy&& policy) {
		const size_t entityCount = objects.size_in_use();
		//BEFORE ANYTHING, UPDATE VELOCITY 
		for (auto& each : objects)
			each.set_currVel_to_mainVel();

		//do collision checks
		collision_algorithm(objects, std::forward<ExecutionPolicy>(policy));

		//AFTER COLLISION, MOVE BLINDLY (if there was collision and resolution applied, mCurrentVelocity should be zeroed out meaning no movement)
		for (auto& each : objects)
			each.update_position_default();

		//do border checks
		objects_vs_container_resolved(objects, globalBorders);
	}

	//TEMPORARY BS, MOVE TO TRANSFORM
	static constexpr auto COLLISION_POLICY_REFLECT_BOTH = [](TransformF&A, TransformF&B, PhysicsData pd)noexcept {
		/*NOTE: if velocity is zero in object then reversing it is meaning less, 0 times anything is 0*/
		if (pd.collisionNormal.x != 0) {
			A.flip_X_mainVel();
			B.flip_X_mainVel();
		}
		if (pd.collisionNormal.y != 0) {
			A.flip_Y_mainVel();
			B.flip_Y_mainVel();
		}
		};


}