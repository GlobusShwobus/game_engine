#pragma once

#include "Rectangle.h"

namespace badEngine {

	class Transform {

	public:

		//CONSTRUCTORS
		constexpr Transform()noexcept = default;
		constexpr Transform(const AABB& rectangle, const float2& velocity)noexcept
			:mBox(rectangle), mVelocity(velocity), mCurrVelocity(mVelocity) {}
		constexpr Transform(const AABB& rectangle)noexcept
			:mBox(rectangle) {}

		void update_position()noexcept {
			mBox.x += mCurrVelocity.x;
			mBox.y += mCurrVelocity.y;
		}
		void reset_velocity()noexcept {
			mCurrVelocity = mVelocity;
		}

	public:

		AABB mBox;
		float2 mVelocity;
		float2 mCurrVelocity;
	};
}