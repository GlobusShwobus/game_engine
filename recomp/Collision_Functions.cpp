#include "Collision_Functions.h"
#include <limits>

namespace badEngine {
	float sweptAABB(const rectF& objA, const rectF& objB, const vec2f& velocity, vec2f& normal) {
		/*
		InvEntry and yInvEntry both specify how far away the closest edges of the objects are from each other.
		xInvExit and yInvExit is the distance to the far side of the object.
		*/
		float xInvEntry, yInvEntry;
		float xInvExit, yInvExit;

		if (velocity.x > 0.0f) {
			xInvEntry = objB.x - (objA.x + objA.w);
			xInvExit = (objB.x + objB.w) - objA.x;
		}
		else {
			xInvEntry = (objB.x + objB.w) - objA.x;
			xInvExit = objB.x - (objA.x + objA.w);
		}

		if (velocity.y > 0.0f) {
			yInvEntry = objB.y - (objA.y + objA.h);
			yInvExit = (objB.y + objB.h) - objA.y;
		}
		else {
			yInvEntry = (objB.y + objB.h) - objA.y;
			yInvExit = objB.y - (objA.y + objA.h);
		}


		/*
		dividing the xEntry, yEntry, xExit and yExit by the object's velocity.
		These new variables will give us our value between 0 and 1 of when each collision occurred on each axis.
		*/
		float xEntry, yEntry;
		float xExit, yExit;

		if (velocity.x == 0.0f) {
			xEntry = -std::numeric_limits<float>::infinity();
			xExit = std::numeric_limits<float>::infinity();
		}
		else {
			xEntry = xInvEntry / velocity.x;
			xExit = xInvExit / velocity.x;
		}

		if (velocity.y == 0.0f) {
			yEntry = -std::numeric_limits<float>::infinity();
			yExit = std::numeric_limits<float>::infinity();
		}
		else {
			yEntry = yInvEntry / velocity.y;
			yExit = yInvExit / velocity.y;
		}

		// find the earliest/latest times of collisionfloat 
		float entryTime = mValue_max(xEntry, yEntry);
		float exitTime = mValue_min(xExit, yExit);

		// if there was no collision
		if (entryTime > exitTime || xEntry < 0.0f && yEntry < 0.0f || xEntry > 1.0f || yEntry > 1.0f)
		{
			normal = vec2f(0, 0);
			return 1.0f;//1.0f means the object can travel for it's full lenght
		}
		else // if there was a collision 
		{
			// calculate normal of collided surface
			if (xEntry > yEntry)
			{
				if (xInvEntry < 0.0f) {
					normal = vec2f(1, 0);
				}
				else {
					normal = vec2f(-1, 0);
				}
			}
			else
			{
				if (yInvEntry < 0.0f) {
					normal = vec2f(0, 1);
				}
				else {
					normal = vec2f(0, -1);
				}
			}
		}
		// return the time of collisionreturn entryTime; 
		return entryTime;
	}

	SequenceM<SweptAABB_Data> determine_colliders(SequenceM<TransformF>& objects) {
		const int entityCount = objects.size_in_use();

		SequenceM<SweptAABB_Data> collisions;

		for (int i = 0; i < entityCount; ++i) {
			for (int j = i + 1; j < entityCount; ++j) {//j=i+1 becasue A vs B is same as B vs A

				float collisionTime = 1.0f;
				vec2f collisionNormal;

				if (sweptAABB_dynamic_vs_dynamic(objects[i], objects[j], collisionNormal, collisionTime)) {
					collisions.element_create(
						SweptAABB_Data(&objects[i], &objects[j], collisionTime, collisionNormal)
					);
				}

			}
		}
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