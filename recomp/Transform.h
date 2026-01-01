#pragma once

#include "Rectangle.h"

namespace badEngine {

	class Transform {

	public:

		//CONSTRUCTORS
		constexpr Transform()noexcept = default;
		constexpr Transform(const float4& rectangle, const float2& velocity)noexcept
			:mBox(rectangle), mVelocity(velocity), mCurrVelocity(mVelocity) {}
		constexpr Transform(const float4& rectangle)noexcept
			:mBox(rectangle) {}

		void update_position()noexcept {
			mBox.x += mCurrVelocity.x;
			mBox.y += mCurrVelocity.y;
		}
		void reset_velocity()noexcept {
			mCurrVelocity = mVelocity;
		}

	public:

		float4 mBox;
		float2 mVelocity;
		float2 mCurrVelocity;
	};
}