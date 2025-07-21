#pragma once
#include <cmath>
#include "Vek2.h"

namespace badEngine {

	template<typename T>
	class Rectangle {
	public:
		T x;
		T y;
		T w;
		T h;

		Rectangle() :x(), y(), w(), h() {}
		Rectangle(T x, T y, T w, T h) :x(x), y(y), w(w), h(h) {}
		
		template <typename S>
		explicit Rectangle(const Rectangle<S>& rect) :x(T(rect.x)), y(T(rect.y)), w(T(rect.w)), h(T(rect.h)) {}
		
		Rectangle(const Vec2<T>& position, T w, T h) :x(position.x), y(position.y), w(w), h(h) {}

		static inline bool containsPoint(const Rectangle& rect, T cordX, T cordY)
		{
			return (
				cordX >= rect.x &&
				cordY >= rect.y &&
				cordX < rect.x + rect.w &&
				cordY < rect.y + rect.h);
		}
		static inline bool containsPoint(const Rectangle& rect, const Vec2<T>& position)
		{
			return Rectangle::containsPoint(rect, position.x, position.y);
		}
		inline bool        containsPoint(T cordX, T cordY)const
		{
			return Rectangle::containsPoint(*this, cordX, cordY);
		}
		inline bool        containsPoint(const Vec2<T>& position)const
		{
			return Rectangle::containsPoint(*this, position.x, position.y);
		}

		static inline bool containsRect(const Rectangle& larger, const Rectangle& smaller)
		{
			return (
				smaller.x >= larger.x &&
				smaller.y >= larger.y &&
				smaller.x + smaller.w <= larger.x + larger.w &&
				smaller.y + smaller.h <= larger.y + larger.h);
		}
		inline bool        containsRect(const Rectangle& other)const
		{
			return Rectangle::containsRect(*this, other);
		}

		static inline bool intersects(const Rectangle& a, const Rectangle& b)
		{
			return (
				a.x < b.x + b.w &&
				a.x + a.w > b.x &&
				a.y < b.y + b.h &&
				a.y + a.h > b.y
				);
		}
		inline bool        intersects(const Rectangle& other)const
		{
			return Rectangle::intersects(*this, other);
		}

		static bool        intersectsEnhanced(const Rectangle& a, const Rectangle& b, Rectangle* output = nullptr)
		{
			float dx = center(a.x, a.w) - center(b.x, b.w);
			float dy = center(a.y, a.h) - center(b.y, b.h);

			float overlapX = overlap(a.w, b.w, dx);
			float overlapY = overlap(a.h, b.h, dy);

			if (overlapX < 0.0f && overlapY < 0.0f)
				return false;

			if (output)
				*output = { dx,dy,overlapX, overlapY };

			return true;
		}
		bool               intersectsEnhanced(const Rectangle& other, Rectangle* output = nullptr)const
		{
			return Rectangle::intersectsEnhanced(*this, other, output);
		}

		inline float centerX()const
		{
			return Rectangle::center(x, w);
		}
		inline float centerY()const
		{
			return Rectangle::center(y, h);
		}
		inline float halfWidth()const
		{
			return Rectangle::half(w);
		}
		inline float halfHeight()const
		{
			return Rectangle::half(h);
		}

	private:

		static inline float half(T lenght)
		{
			return lenght * 0.5f;
		}
		static inline float center(T pos, T lenght)
		{
			return pos + Rectangle::half(lenght);
		}
		static inline float overlap(T dim1, T dim2, float dx)
		{
			return Rectangle::half(dim1) + Rectangle::half(dim2) - std::fabs(dx);
		}

	};
	using RectI = Rectangle<int>;
	using RectF = Rectangle<float>;
}