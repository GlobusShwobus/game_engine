#pragma once

#include "Simple_Geometric_Functions.h"
#include "Rectangle.h"
#include "Transform.h"
#include "SequenceM.h"


#include <algorithm>//for sort
/*
#include <limits>
*/

namespace badEngine {

	template <typename T>
	constexpr bool rect_vs_point(const Rectangle<T>& rect, T X, T Y)noexcept {
		return (
			X >= rect.x &&
			Y >= rect.y &&
			X < rect.x + rect.w &&
			Y < rect.y + rect.h);
	}
	template <typename T>
	constexpr bool rect_vs_point(const Rectangle<T>& rect, Vec2M<T>& pos)noexcept {
		return rect_vs_point(rect, pos.x, pos.y);
	}



	template <typename T>
	constexpr bool rect_vs_rect(const Rectangle<T>& b1, const Rectangle<T>& b2)noexcept {
		return
			b1.x < b2.x + b2.w &&
			b1.x + b1.w > b2.x &&
			b1.y < b2.y + b2.h &&
			b1.y + b1.h > b2.y;
	}



	float rect_vs_ray(const rectF& objA, const rectF& objB, const vec2f& velocity, vec2f& normal);



	template <typename T>
	bool dynamic_vs_dynamic_rectangle(Transform<T>& objA, Transform<T>& objB, vec2f& normal, float& contactTime) {

		vec2f relativeVel = objA.mCurrVelocity - objB.mCurrVelocity;
		rectF expandedA = objA.get_expanded_rect(relativeVel);

		if (!rect_vs_rect(expandedA, objB.mBox)) {
			return false;
		}

		contactTime = rect_vs_ray(objA.mBox, objB.mBox, relativeVel, normal);

		return (contactTime >= 0.f && contactTime < 1.f);
	}



	struct CollisionResult {
		int indexA, indexB;
		float collisionTime;
		vec2f collisionNormal;
	};

	SequenceM<CollisionResult> determine_colliders(SequenceM<TransformF>& objects);

	template <typename ExecutionPolicy>
		requires std::invocable<ExecutionPolicy, TransformF&, TransformF&, const CollisionResult&>
	void collision_algorithm_executable(SequenceM<TransformF>& objects, ExecutionPolicy&& policy) {
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

			policy(objA, objB, collision);
		}


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

	void objects_vs_container_resolved(SequenceM<TransformF>& objects, const rectI& container)noexcept;

}