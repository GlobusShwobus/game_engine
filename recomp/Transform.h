#pragma once

#include "Vek2.h"
#include "Rectangle.h"

#define BADENGINE_MAX(x,y) (((x) > (y)) ? (x) : (y))
#define BADENGINE_MIN(x,y) (((x) < (y)) ? (x) : (y))

namespace badEngine {

	class Transform {

		RectF rectangle;
		Vec2f velocity;

	public:

		Transform() = default;
		Transform(const RectF& rectangle, Vec2f velocity) :rectangle(rectangle), velocity(velocity) {}

		inline const RectF& getRectangle()const
		{
			return rectangle;
		}
		inline const Vec2f& getVelocity()const
		{
			return velocity;
		}

		inline RectF* DataRectangle()
		{
			return &rectangle;
		}
		inline Vec2f* DataVelocity()
		{
			return &velocity;
		}

		inline void setRectangle(float x, float y, float w, float h)
		{
			rectangle = { x,y,w,h };
		}
		inline void setRectangle(const RectF& rectangle)
		{
			this->rectangle = rectangle;
		}
		inline void setVelocity(float x, float y)
		{
			velocity = { x,y };
		}
		inline void setVelocity(const Vec2f& velocity)
		{
			this->velocity = velocity;
		}

		inline void setPositionManual(float x, float y)
		{
			rectangle.x = x;
			rectangle.y = y;
		}
		inline void setPositionManual(const Vec2f& position)
		{
			rectangle.x = position.x;
			rectangle.y = position.y;
		}
		inline void setPositionAutomatic()
		{
			rectangle.x += velocity.x;
			rectangle.y += velocity.y;
		}

		static bool rayCollision(const Vec2f& rayOrigin, const Vec2f& rayVector, const RectF& target, float& hitTime, Vec2f* contantPoint = nullptr, Vec2i* contactNormal = nullptr);
		bool rayCollisionEnhanced(const RectF& bRect, const Vec2f& bVel, float& hitTime, Vec2f* contactPoint = nullptr, Vec2i* contactNormal = nullptr)const;
		bool rayCollisionEnhanced(const Transform& target, float& hitTime, Vec2f* contactPoint = nullptr, Vec2i* contactNormal = nullptr)const;
		static bool rayCollisionEnhanced(const Transform& a, const Transform& b, float& hitTime, Vec2f* contactPoint = nullptr, Vec2i* contactNormal = nullptr);
		static bool rayCollisionEnhanced(const RectF& aRect, const Vec2f& aVel, const RectF& bRect, const Vec2f& bVel, float& hitTime, Vec2f* contactPoint = nullptr, Vec2i* contactNormal = nullptr);

	private:

		static inline void swap(float& a, float& b)
		{
			float temp = a;
			a = b;
			b = temp;
		}

	};
}