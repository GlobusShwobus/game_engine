#pragma once

#include "Rectangle.h"

namespace badEngine {

	class Transform {

	public:

		//CONSTRUCTORS
		constexpr Transform()noexcept = default;
		constexpr Transform(const rectF& rectangle, const vec2f& velocity)noexcept
			:mBox(rectangle), mVelocity(velocity), mCurrVelocity(mVelocity) {}
		constexpr Transform(const rectF& rectangle)noexcept
			:mBox(rectangle) {}

		void update_position()noexcept {
			mBox.increment_pos(mCurrVelocity);
		}
		void reset_velocity()noexcept {
			mCurrVelocity = mVelocity;
		}

	public:

		rectF mBox;
		vec2f mVelocity;
		vec2f mCurrVelocity;
	};
}