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

		bool sweptAABB_dynamic(const Transform<T>& rhs, float& collisionTime, vec2i* contact_normal = nullptr)const noexcept {

			//since both objects are moving, of if B isn't doesn't matter, the logic needs to run on relative velocity
			const vec2f relativeVel = mCurrVelocity - rhs.mCurrVelocity;
			//if relative velocity is 0, then no collision
			if (relativeVel.x == 0 && relativeVel.y == 0)
				return false;
			//expanded rectangle must also be with the consideration of relative velocity
			const rectF expandedA = rectF(
				rhs.mBox.get_pos() - mBox.get_half_size(),
				rhs.mBox.get_size() + mBox.get_size()
			);

			//get contact time of collision, default should be 1.0f which means no collision in the span of the current frame
			if (RayVsRect(mBox.get_center_point(), relativeVel, expandedA, collisionTime, contact_normal))
				return (collisionTime >= 0.0f && collisionTime < 1.0f);
			
			else
				return false;
		}

		static bool RayVsRect(const vec2f& ray_origin, const vec2f& ray_dir, const rectF& target, float& t_hit_near, vec2i* contact_normal = nullptr)
		{
			// Cache division
			vec2f invdir = 1.0f / ray_dir;

			// Calculate intersections with rectangle bounding axes
			vec2f t_near = (target.get_pos() - ray_origin) * invdir;
			vec2f t_far = (target.get_pos() + target.get_size() - ray_origin) * invdir;

			if (std::isnan(t_far.y) || std::isnan(t_far.x)) return false;
			if (std::isnan(t_near.y) || std::isnan(t_near.x)) return false;

			// Sort distances
			if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
			if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

			// Early rejection		
			if (t_near.x > t_far.y || t_near.y > t_far.x) return false;

			// Closest 'time' will be the first contact
			t_hit_near = mValue_max(t_near.x, t_near.y);

			// Furthest 'time' is contact on opposite side of target
			float t_hit_far = mValue_min(t_far.x, t_far.y);

			// Reject if ray direction is pointing away from object
			if (t_hit_far < 0)
				return false;

			if (contact_normal) {
				if (t_near.x > t_near.y)
					if (invdir.x < 0)
						*contact_normal = vec2i( 1, 0 );
					else
						*contact_normal = vec2i(- 1, 0 );
				else if (t_near.x < t_near.y)
					if (invdir.y < 0)
						*contact_normal = vec2i( 0, 1 );
					else
						*contact_normal = vec2i( 0, -1 );
			}
			// Note if t_near == t_far, collision is principly in a diagonal
			// so pointless to resolve. By returning a CN={0,0} even though its
			// considered a hit, the resolver wont change anything.
			return true;
		}

	public:

		Rectangle<T> mBox;
		Vec2M<T> mVelocity;
		Vec2M<T> mCurrVelocity;
	
		Color col;
	};

	using TransformI = Transform<int>;
	using TransformF = Transform<float>;
}