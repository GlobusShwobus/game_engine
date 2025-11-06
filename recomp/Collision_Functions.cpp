#include "Collision_Functions.h"
#include <limits>

namespace badEngine {

	float SweptAABB_collision_exe::sweptAABB(const rectF& objA, const rectF& objB, const vec2f& velocity, vec2f& normal)const noexcept {
		/*
		InvEntry and yInvEntry both specify how far away the closest edges of the objects are from each other.
		xInvExit and yInvExit is the distance to the far side of the object.
		*/
		const vec2f invEntry = inv_entry(objA, objB, velocity);
		const vec2f invExit = inv_exit(objA, objB, velocity);

		/*
		dividing the xEntry, yEntry, xExit and yExit by the object's velocity.
		These new variables will give us our value between 0 and 1 of when each collision occurred on each axis.
		*/
		const vec2f entry = entry_point(invEntry, velocity);
		const vec2f exit = exit_point(invExit, velocity);

		// find the earliest/latest times of collisionfloat 
		float entryTime = mValue_max(entry.x, entry.y);
		float exitTime = mValue_min(exit.x, exit.y);

		// if there was no collision
		if (entryTime > exitTime || entry.x < 0.0f && entry.y < 0.0f || entry.x > 1.0f || entry.y > 1.0f) {
			normal = vec2f(0, 0);
			return 1.0f;//1.0f means the object can travel for it's full lenght
		}
		//normal
		if (entry.x > entry.y) {
			normal = (invEntry.x < 0.0f) ? vec2f{ 1.0f, 0.0f } : vec2f{ -1.0f, 0.0f };
		}
		else {
			normal = (invEntry.y < 0.0f) ? vec2f{ 0.0f, 1.0f } : vec2f{ 0.0f, -1.0f };
		}

		// return the time of collisionreturn entryTime; 
		return entryTime;
	}
	bool SweptAABB_collision_exe::sweptAABB_dynamic_vs_dynamic(SweptResult& pair)const noexcept {

		const auto rectA = pair.refA->get_rectangle();
		const auto rectB = pair.refB->get_rectangle();

		//since both objects are moving, of if B isn't doesn't matter, the logic needs to run on relative velocity
		vec2f relativeVel = pair.refA->get_currVel() - pair.refB->get_currVel();
		//expanded rectangle must also be with the consideration of relative velocity
		rectF expandedA = sweptAABB_expand_rect(rectA, relativeVel);
		//early return
		if (!expandedA.intersects_rect(rectB))
			return false;
		//get contact time of collision, default should be 1.0f which means no collision in the span of the current frame
		pair.collisionTime = sweptAABB(rectA, rectB, relativeVel, pair.collisionNormal);

		return (pair.collisionTime >= 0.f && pair.collisionTime < 1.f);
	}
	rectF SweptAABB_collision_exe::sweptAABB_expand_rect(const rectF& rect, const vec2f& velocity)const noexcept {
		return rectF(
			(velocity.x > 0) ? rect.x : rect.x + velocity.x,
			(velocity.y > 0) ? rect.y : rect.y + velocity.y,
			rect.w + std::abs(velocity.x),
			rect.h + std::abs(velocity.y)
		);
	}
	SequenceM<SweptAABB_collision_exe::SweptResult> SweptAABB_collision_exe::get_colliders(SequenceM<TransformF>& objects) {
		const int entityCount = objects.size();

		SequenceM<SweptResult> collisions;

		for (int i = 0; i < entityCount; ++i) {
			for (int j = i + 1; j < entityCount; ++j) {//j=i+1 becasue A vs B is same as B vs A

				SweptResult pair(&objects[i], &objects[j], 1.0f, vec2f(0, 0));

				if (sweptAABB_dynamic_vs_dynamic(pair))
					collisions.emplace_back(pair);

			}
		}
		std::sort(collisions.begin(), collisions.end(), [](const auto& a, const auto& b) {
			return a.collisionTime < b.collisionTime;
			});

		return collisions;
	}

	void objects_vs_container_resolved(SequenceM<TransformF>& objects, const rectI& container)noexcept {
		for (auto& obj : objects) {
		
			auto result = obj.force_contained_in(container);

			if (result.first) {
				obj.flip_X_mainVel();
			}

			if (result.second) {
				obj.flip_Y_mainVel();
			}

		}
	}
}