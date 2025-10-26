#pragma once

#include "Rectangle.h"
#include <utility>

namespace badEngine {

	template <typename T>
		requires IS_MATHMATICAL_VECTOR_T<T>
	class Transform {

	public:

		constexpr Transform()noexcept = default;

		constexpr Transform(Rectangle<T> rectangle, Vec2M<T> velocity)noexcept :mBox(std::move(rectangle)), mVelocity(std::move(velocity)), mCurrVelocity(mVelocity) {}

		constexpr Transform(Rectangle<T> rectangle)noexcept :mBox(std::move(rectangle)) {}


		template<typename S>
		constexpr Transform(const Transform<S>& rhs)noexcept :mBox(rhs.mBox), mVelocity(rhs.mVelocity), mCurrVelocity(rhs.mVelocity) {}

		template <typename S>
		constexpr Transform& operator=(const Transform<S>& rhs)noexcept {
			mBox = rhs.mBox;
			mVelocity = rhs.mVelocity;
			mCurrVelocity = rhs.mCurrVelocity;
			return *this;
		}

		void update_position_default()noexcept {
			mBox.x += mCurrVelocity.x;
			mBox.y += mCurrVelocity.y;
		}
		void update_position_expression(const Vec2M<T>& expression)noexcept {
			mBox.x += expression.x;
			mBox.y += expression.y;
		}


		void set_currVel_to_mainVel()noexcept {
			mCurrVelocity = mVelocity;
		}


		Rectangle<T> get_rectangle()const noexcept {
			return mBox;
		}
		Vec2M<T> get_mainVel()const noexcept {
			return mVelocity;
		}
		Vec2M<T> get_currVel()const noexcept {
			return mCurrVelocity;
		}

		/*
		# 1) clamps the rectangle to the axis of container if there is an this isn't fully contained by container
		#    if this is larger than the container, it will be buggy
		#
		# 2) returns a pair of bools telling what axis penetration occured in (X or Y)
		*/
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

		void flip_X_mainVel()noexcept {
			mVelocity.x *= -1;
		}
		void flip_Y_mainVel()noexcept {
			mVelocity.y *= -1;
		}

		void set_mainVel(Vec2M<T> expression)noexcept {
			mVelocity = expression;
		}
		void set_currVel(Vec2M<T> expression)noexcept {
			mCurrVelocity = expression;
		}


	private:
		Rectangle<T> mBox;

		Vec2M<T> mVelocity;
		Vec2M<T> mCurrVelocity;
	};

	using TransformI = Transform<int>;
	using TransformF = Transform<float>;


	static constexpr auto COLLISION_POLICY_REFLECT = [](TransformF& entity, float time, const vec2f& normal)noexcept {
		/*NOTE: if velocity is zero in object then reversing it is meaning less, 0 times anything is 0*/
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

}