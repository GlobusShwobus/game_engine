#include "Geometric_Functions.h"

namespace badEngine {
	namespace swept {
		bool AABB_swept(
			const vec2f& rayOrigin,
			const vec2f& rayVector,
			const rectF& target,
			float& contactTime,
			vec2f& contactNormal) noexcept
		{
			auto reciprocal = reciprocal_vector(rayVector);

			vec2d tNear = vec2d(
				(target.x - rayOrigin.x) * reciprocal.x,
				(target.y - rayOrigin.y) * reciprocal.y
			);
			vec2d tFar = vec2d(
				(target.x + target.w - rayOrigin.x) * reciprocal.x,
				(target.y + target.h - rayOrigin.y) * reciprocal.y
			);

			//broken float value (division by 0 probably)
			if (
				std::isnan(tNear.x) ||
				std::isnan(tNear.y) ||
				std::isnan(tFar.x) ||
				std::isnan(tFar.y)) return false;
			//order
			if (tNear.x > tFar.x) std::swap(tNear.x, tFar.x);
			if (tNear.y > tFar.y) std::swap(tNear.y, tFar.y);
			//if no hit == false
			if (tNear.x > tFar.y || tNear.y > tFar.x)
				return false;
			contactTime  = std::max(tNear.x, tNear.y);
			float hitFar = std::min(tFar.x, tFar.y);

			//if hit but opposite direction, then no actual hit, just on same line
			if (hitFar < 0.0f)
				return false;

			/*
			* NOT REQUIRED FOR NOW
			//set the point where contact was made, idk what to do with it, can remove later tho
			if (contactPoint)
				*contactPoint = (rayVector * hitFar) + rayOrigin;
			*/
			//the fuggin normal

			contactNormal = get_swept_result_normal(tNear, reciprocal);

			return true;
		}

		bool AABB_swept_dynamic_collision(
			const TransformF& a,
			const TransformF& b,
			float& contactTime,
			vec2f& contactNormal)noexcept
		{
			auto relativeVelocity = a.mVelocity - b.mVelocity;
			//no movement
			if (relativeVelocity.x == 0 && relativeVelocity.y == 0) return false;

			rectF expandedTarget = get_swept_expanded_target(a.mBox, b.mBox);

			if (AABB_swept(a.mBox.get_center_point(), relativeVelocity, expandedTarget, contactTime, contactNormal)) {
				return (contactTime >= 0.0f && contactTime < 1.0f);
			}
			return false;
		}
	}
}