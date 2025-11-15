#pragma once

#include "Rectangle.h"
#include <utility>

namespace badEngine {

	template <typename T>
		requires IS_MATHMATICAL_VECTOR_T<T>
	class Transform {

	public:

		//CONSTRUCTORS
		constexpr Transform()noexcept = default;
		constexpr Transform(Rectangle<T> rectangle, Vec2M<T> velocity)noexcept
			:mBox(std::move(rectangle)), mVelocity(std::move(velocity)), mCurrVelocity(mVelocity) {
		}
		constexpr Transform(Rectangle<T> rectangle)noexcept
			:mBox(std::move(rectangle)) {
		}

		void update_position()noexcept {
			mBox.increment_pos(mCurrVelocity);
		}
		void reset_velocity()noexcept {
			mCurrVelocity = mVelocity;
		}

		bool sweptAABB_dynamic(const Transform<T>& rhs, float& collisionTime, vec2i& collisionSignVector)const noexcept {

			const auto& rectA = mBox;
			const auto& rectB = rhs.mBox;

			//since both objects are moving, of if B isn't doesn't matter, the logic needs to run on relative velocity
			const vec2f relativeVel = mCurrVelocity - rhs.mCurrVelocity;
			//expanded rectangle must also be with the consideration of relative velocity
			const rectF expandedA = mBox.get_expanded(relativeVel);
			//early return
			if (!expandedA.intersects_rect(rectB))
				return false;
			//get contact time of collision, default should be 1.0f which means no collision in the span of the current frame
			collisionTime = sweptAABB_static(rectA, rectB, relativeVel, collisionSignVector);

			return (collisionTime >= 0.f && collisionTime < 1.f);
		}

		static float sweptAABB_static(const rectF& objA, const rectF& objB, const vec2f& velocity, vec2i& normal) noexcept {
			/*
			InvEntry and yInvEntry both specify how far away the closest edges of the objects are from each other.
			xInvExit and yInvExit is the distance to the far side of the object.
			*/
			const vec2f invEntry =  vec2f(
				(velocity.x > 0.0f) ? objB.x - (objA.x + objA.w) : (objB.x + objB.w) - objA.x,
				(velocity.y > 0.0f) ? objB.y - (objA.y + objA.h) : (objB.y + objB.h) - objA.y
			);

			const vec2f invExit = vec2f(
				(velocity.x > 0.0f) ? (objB.x + objB.w) - objA.x : objB.x - (objA.x + objA.w),
				(velocity.y > 0.0f) ? (objB.y + objB.h) - objA.y : objB.y - (objA.y + objA.h)
			);
			/*
			dividing the xEntry, yEntry, xExit and yExit by the object's velocity.
			These new variables will give us our value between 0 and 1 of when each collision occurred on each axis.
			*/
			const vec2f entry = vec2f(
				(velocity.x == 0.0f) ? -std::numeric_limits<float>::infinity() : invEntry.x / velocity.x,
				(velocity.y == 0.0f) ? -std::numeric_limits<float>::infinity() : invEntry.y / velocity.y
			);
			const vec2f exit = vec2f(
				(velocity.x == 0.0f) ? std::numeric_limits<float>::infinity() : invExit.x / velocity.x,
				(velocity.y == 0.0f) ? std::numeric_limits<float>::infinity() : invExit.y / velocity.y
			);
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

	public:

		Rectangle<T> mBox;
		Vec2M<T> mVelocity;
		Vec2M<T> mCurrVelocity;
	
	};

	using TransformI = Transform<int>;
	using TransformF = Transform<float>;

	/*
	// TRANSFORM WILL BE UPDATED TO ALSO INCLUDE WEIGHT THEN THESE NEED CHANGING
	static constexpr auto COLLISION_POLICY_REFLECT = [](TransformF& entity, float time, const vec2f& normal)noexcept {
		NOTE: if velocity is zero in object then reversing it is meaning less, 0 times anything is 0
		if (normal.x != 0) {
			entity.flip_X_mainVel();
		}
		if (normal.y != 0) {
			entity.flip_Y_mainVel();
		}
		};

	static constexpr auto COLLISION_POLICY_PUSH = [](TransformF& entity, float time, const vec2f& normal)noexcept {
		const float remainingTime = 1.0f - time;
		vec2f vel = entity.get_mainVel();

		const float vn = dot_vector(vel, normal);
		const vec2f vNormal = normal * vn;
		const vec2f vTangent = vel - vNormal;

		const vec2f newVel = vTangent * remainingTime;

		entity.set_mainVel(newVel);
		};
	*/

	/*
			
		# 1) clamps the rectangle to the axis of container if there is an this isn't fully contained by container
		#    if this is larger than the container, it will be buggy
		#
		# 2) returns a pair of bools telling what axis penetration occured in (X or Y)
		
	std::pair<bool, bool> force_contained_in(const rectI& container)noexcept {

		std::pair<bool, bool> axis(false, false);
		if (mBox.x < container.x) {
			mBox.x = container.x;
			axis.first = true;
		}
		if (mBox.y < container.y) {
			mBox.y = container.y;
			axis.second = true;
		}
		if (mBox.x + mBox.w > container.x + container.w) {
			mBox.x = (container.x + container.w) - mBox.w;
			axis.first = true;
		}
		if (mBox.y + mBox.h > container.y + container.h) {
			mBox.y = (container.y + container.h) - mBox.h;
			axis.second = true;
		}

		return axis;
	}
	
	*/
}