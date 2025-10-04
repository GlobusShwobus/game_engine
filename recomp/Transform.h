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

		constexpr void set_position(T x, T y)noexcept {
			mBox.mPosition.set(x, y);
		}
		constexpr void set_velocity(T x, T y)noexcept {
			mVelocity.set(x, y);
		}
		constexpr void move_on_velocity()noexcept {
			mBox.mPosition += mVelocity;
		}

	public:
		Rectangle<T> mBox;
		Vec2M<T> mVelocity;
	};

	using TransformI = Transform<int>;
	using TransformF = Transform<float>;
}