#pragma once

#include "Rectangle.h"
#include <utility>

#include "Color.h"//TEMP FOR TESTING

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

	public:

		Rectangle<T> mBox;
		Vec2M<T> mVelocity;
		Vec2M<T> mCurrVelocity;
	};

	using TransformI = Transform<int>;
	using TransformF = Transform<float>;
}