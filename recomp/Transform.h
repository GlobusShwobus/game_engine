#pragma once

#include "Rectangle.h"

namespace badEngine {


	template <typename T>
		requires IS_MATHMATICAL_VECTOR_T<T>
	class Transform {

	public:

		constexpr Transform()noexcept = default;

		template<typename S>
		constexpr Transform(Rectangle<S> rectangle, Vec2M<S> velocity)noexcept :mBox(std::move(rectangle)), mVelocity(std::move(velocity)) {}

		template<typename S>
		constexpr Transform(Rectangle<S> rectangle)noexcept :mBox(std::move(rectangle)) {}

		template <typename S>
		constexpr Transform(Vec2M<S> velocity)noexcept :mVelocity(std::move(velocity)) {}

		template<typename S>
		constexpr Transform(const Transform<S>& rhs)noexcept :mBox(rhs.mBox), mVelocity(rhs.mVelocity) {}

		template <typename S>
		constexpr Transform& operator=(const Transform<S>& rhs)noexcept {
			mBox = rhs.mBox;
			mVelocity = rhs.mVelocity;
			return *this;
		}

		constexpr void set_velocity(T x, T y)noexcept {
			mVelocity = Vec2M<T>( x,y );
		}
		template <typename U>
		constexpr void set_velocity(const Vec2M<U>& vel)noexcept {
			set_velocity(vel.x, vel.y);
		}

		constexpr void set_position_transformative()noexcept {
			mBox.x += mVelocity.x;
			mBox.y += mVelocity.y;
		}

	public:
		Rectangle<T> mBox;
		Vec2M<T> mVelocity;
	};

	using TransformI = Transform<int>;
	using TransformF = Transform<float>;
}