#pragma once

#include "Rectangle.h"

namespace badEngine {
	/*
	CONSIDER INIT mCurrVelocity ON OBJECT CREATION
	*/

	template <typename T>
		requires IS_MATHMATICAL_VECTOR_T<T>
	class Transform {

	public:

		constexpr Transform()noexcept = default;

		template<typename S>
		constexpr Transform(Rectangle<S> rectangle, Vec2M<S> velocity)noexcept :mBox(std::move(rectangle)), mVelocity(std::move(velocity)){}

		template<typename S>
		constexpr Transform(Rectangle<S> rectangle)noexcept :mBox(std::move(rectangle)) {}

		template <typename S>
		constexpr Transform(Vec2M<S> velocity)noexcept :mVelocity(std::move(velocity)){}

		template<typename S>
		constexpr Transform(const Transform<S>& rhs)noexcept :mBox(rhs.mBox), mVelocity(rhs.mVelocity){}

		template <typename S>
		constexpr Transform& operator=(const Transform<S>& rhs)noexcept {
			mBox = rhs.mBox;
			mVelocity = rhs.mVelocity;
			mCurrVelocity = rhs.mCurrVelocity;
			return *this;
		}

		constexpr void update_position()noexcept {
			mBox.x += mCurrVelocity.x;
			mBox.y += mCurrVelocity.y;
		}
		constexpr void reset_velocity()noexcept {
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