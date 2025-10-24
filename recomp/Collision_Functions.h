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


	void objects_vs_container_resolved(SequenceM<TransformF>& objects, const rectI& container)noexcept;


	struct CollisionResult {
		int indexA, indexB;
		float collisionTime;
		vec2f collisionNormal;
	};

	float sweptAABB(const rectF& objA, const rectF& objB, const vec2f& velocity, vec2f& normal);
	
	template <typename T>
	bool sweptAABB_dynamic_vs_dynamic(Transform<T>& objA, Transform<T>& objB, vec2f& normal, float& contactTime) {

		vec2f relativeVel = objA.mCurrVelocity - objB.mCurrVelocity;
		rectF expandedA = objA.get_expanded_rect(relativeVel);

		if (!expandedA.rect_vs_rect(objB.mBox)) {
			return false;
		}

		contactTime = sweptAABB(objA.mBox, objB.mBox, relativeVel, normal);

		return (contactTime >= 0.f && contactTime < 1.f);
	}

	SequenceM<CollisionResult> determine_colliders(SequenceM<TransformF>& objects);

	template<typename Policy>
	concept CollisionResponsePolicy = std::invocable<Policy, TransformF&, TransformF&, const CollisionResult&>;

	template <CollisionResponsePolicy ExecutionPolicy>
	void collision_algorithm(SequenceM<TransformF>& objects, ExecutionPolicy&& policy) {
		//determine who collides
		auto collisionResults = determine_colliders(objects);
		//sort order of priority
		std::sort(collisionResults.begin(), collisionResults.end(), [](const auto& a, const auto& b) {
			return a.collisionTime < b.collisionTime;
			});

		//resolve collision (may be wrong tho and require a second layer of detection, if bug == here)
		for (auto& collision : collisionResults) {

			auto& objA = objects[collision.indexA];
			auto& objB = objects[collision.indexB];

			const float collisionTime = collision.collisionTime;

			//move upto the point of touching
			objA.mBox.move_by(objA.mCurrVelocity * collisionTime);
			objB.mBox.move_by(objB.mCurrVelocity * collisionTime);

			//zero the current velocity
			objA.mCurrVelocity = vec2f(0, 0);
			objB.mCurrVelocity = vec2f(0, 0);

			//response behavior for velocity in next frame (only mods velocity)

			std::forward<ExecutionPolicy>(policy)(objA, objB, collision);//can take both named and unnamed functors
		}
	}
	template <CollisionResponsePolicy ExecutionPolicy>
	void collision_algorithm_executable(SequenceM<TransformF>& objects, rectI globalBorders, ExecutionPolicy&& policy) {
		const size_t entityCount = objects.size_in_use();
		//BEFORE ANYTHING, UPDATE VELOCITY 
		for (auto& each : objects)
			each.reset_velocity();

		//do collision checks
		collision_algorithm(objects, std::forward<ExecutionPolicy>(policy));

		//AFTER COLLISION, MOVE BLINDLY (if there was collision and resolution applied, mCurrentVelocity should be zeroed out meaning no movement)
		for (auto& each : objects)
			each.update_position();

		//do border checks
		objects_vs_container_resolved(objects, globalBorders);
	}

	static constexpr auto COLLISION_POLICY_REFLECT_BOTH = [](TransformF& A, TransformF& B, const CollisionResult& collisionData)noexcept {
		/*NOTE: if velocity is zero in object then reversing it is meaning less, 0 times anything is 0*/
		if (collisionData.collisionNormal.x != 0) {
			A.mVelocity.x *= -1;
			B.mVelocity.x *= -1;
		}
		if (collisionData.collisionNormal.y != 0) {
			A.mVelocity.y *= -1;
			B.mVelocity.y *= -1;
		}
		};

}