#include "Transform.h"

namespace badEngine {

	bool Transform::rayCollision(const Vec2f& rayOrigin, const Vec2f& rayVector, const RectF& target, float& hitTime, Vec2f* contantPoint, Vec2i* contactNormal)
	{
		// Inverse direction, idfk why, but it is OK to divide floats by 0, creates an infiniti or NaN
		Vec2f inverse = rayVector.inverse();

		// Calculate intersections with rectangle bounding axes
		Vec2f tNear = {
			(target.x - rayOrigin.x) * inverse.x,
			(target.y - rayOrigin.y) * inverse.y
		};
		Vec2f tFar = {
			(target.x + target.w - rayOrigin.x) * inverse.x,
			(target.y + target.h - rayOrigin.y) * inverse.y
		};

		// Check for bullshit floating value
		if (std::isnan(tNear.x) || std::isnan(tNear.y) || std::isnan(tFar.x) || std::isnan(tFar.y))
			return false;
		// Swap near and far
		if (tNear.x > tFar.x) Transform::swap(tNear.x, tFar.x);
		if (tNear.y > tFar.y) Transform::swap(tNear.y, tFar.y);

		// if no hit
		if (tNear.x > tFar.y || tNear.y > tFar.x)
			return false;

		// get hit times (not actually time but coordinate, idfk)
		hitTime = BADENGINE_MAX(tNear.x, tNear.y);
		float tHitFar = BADENGINE_MIN(tFar.x, tFar.y);

		//if hit but opposite direction (infinite plane)
		if (tHitFar < 0.0f)
			return false;

		//OPTIONAL: set the point where contact was made, idk what to do with it, can remove later tho
		if (contantPoint) {
			*contantPoint = {
				(rayVector.x * tHitFar) + rayOrigin.x,
				(rayVector.y * tHitFar) + rayOrigin.y
			};
		}
		//OPTIONAL: get normalized Sign value
		if (contactNormal) {
			if (tNear.x > tNear.y)
				if (rayVector.x < 0.0f)
					*contactNormal = { 1, 0 };
				else
					*contactNormal = { -1, 0 };
			else if (tNear.x < tNear.y)
				if (rayVector.y < 0.0f)
					*contactNormal = { 0, 1 };
				else
					*contactNormal = { 0, -1 };
		}

		return true;
	}
	bool Transform::rayCollisionEnhanced(const RectF& bRect, const Vec2f& bVel, float& hitTime, Vec2f* contactPoint, Vec2i* contactNormal)const
	{
		return Transform::rayCollisionEnhanced(
			this->rectangle,
			this->velocity,
			bRect,
			bVel,
			hitTime,
			contactPoint,
			contactNormal
		);
	}
	bool Transform::rayCollisionEnhanced(const Transform& target, float& hitTime, Vec2f* contactPoint, Vec2i* contactNormal)const
	{
		return Transform::rayCollisionEnhanced(
			this->rectangle,
			this->velocity,
			target.rectangle,
			target.velocity,
			hitTime,
			contactPoint,
			contactNormal
		);
	}
	bool Transform::rayCollisionEnhanced(const Transform& a, const Transform& b, float& hitTime, Vec2f* contactPoint, Vec2i* contactNormal)
	{
		return Transform::rayCollisionEnhanced(
			a.rectangle,
			a.velocity,
			b.rectangle,
			b.velocity,
			hitTime,
			contactPoint,
			contactNormal
		);
	}
	bool Transform::rayCollisionEnhanced(const RectF& aRect, const Vec2f& aVel, const RectF& bRect, const Vec2f& bVel, float& hitTime, Vec2f* contactPoint, Vec2i* contactNormal)
	{
		// relative motion
		Vec2f relativeVelocity = aVel - bVel;

		// Early out if no motion
		if (relativeVelocity.x == 0.0f && relativeVelocity.y == 0.0f)
			return false;

		//adjust target rectangle since ray origin is from the center not top left
		RectF expandedTarget = {
			bRect.x - aRect.halfWidth(),
			bRect.y - aRect.halfHeight(),
			bRect.w + aRect.w,
			bRect.h + aRect.h
		};

		// Ray origin is the center of this object
		Vec2f rayOrigin = { aRect.centerX(), aRect.centerY() };

		if (Transform::rayCollision(rayOrigin, relativeVelocity, expandedTarget, hitTime, contactPoint, contactNormal)) {
			return (hitTime >= 0.0f && hitTime < 1.0f);
		}
		return false;
	}

}