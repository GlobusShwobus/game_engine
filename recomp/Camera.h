#pragma once

#include "Rectangle.h"
#include "Vek2.h"

namespace badEngine {
	class Camera2D {

		float offsetX = 0.0f;
		float offsetY = 0.0f;
		float scaleX = 1.0f;
		float scaleY = 1.0f;

		int screenW = 0;
		int screenH = 0;

	public:

		Camera2D(int screenWidth, int screenHeight) :screenW(screenWidth), screenH(screenHeight) {}


		inline void updateScreenSize(int width, int height)
		{
			screenW = width;
			screenH = height;
		}

		inline void focusPoint(float x, float y)
		{
			offsetX = x - (screenW * 0.5f) / scaleX;	
			offsetY = y - (screenH * 0.5f) / scaleY;	
		}
		template <typename S>
		inline void focusPoint(const Vec2<S>& position)
		{
			focusPoint((float)position.x, (float)position.y);
		}
		inline void focusPoint(float x, float y, float w, float h)
		{
			focusPoint(x + (w * 0.5f), y + (h * 0.5f));
		}
		template <typename S>
		inline void focusPoint(const Rectangle<S>& rect)
		{
			focusPoint(float(rect.x + rect.halfWidth()), float(rect.y + rect.halfHeight()));
		}

		inline void move(float deltaX, float deltaY)
		{
			offsetX += deltaX / scaleX;
			offsetY += deltaY / scaleY;
		}
		template <typename S>
		inline void move(const Vec2<S>& delta)
		{
			move(delta.x, delta.y);
		}

		inline void zoom(float factor)
		{
			scaleX *= factor;
			scaleY *= factor;
		}

		inline void worldToScreen(int worldX, int worldY, float& screenX, float& screenY) const
		{
			screenX = (worldX - offsetX) * scaleX;
			screenY = (worldY - offsetY) * scaleY;
		}
		template <typename S>
		Vec2f worldToScreen(const Vec2<S>& position)const
		{
			Vec2f p;
			worldToScreen((int)position.x, (int)position.y, p.x, p.y);
			return p;
		}
		Vec2f worldToScreen(int x, int y)const
		{
			Vec2f p;
			worldToScreen(x, y, p.x, p.y);
			return p;
		}
		template <typename S>
		RectF worldToScreen(const Rectangle<S>& rect)const
		{
			Vec2f p = worldToScreen((int)rect.x, (int)rect.y);
			float w = (float)rect.w * scaleX;
			float h = (float)rect.h * scaleY;

			return { p.x, p.y, w,h };
		}

		inline void screenToWorld(float screenX, float screenY, int& worldX, int& worldY) const
		{
			worldX = screenX / scaleX + offsetX;
			worldY = screenY / scaleY + offsetY;
		}
		template <typename S> 
		Vec2i screenToWorld(const Vec2<S>& position) const 
		{
			Vec2i p;
			screenToWorld((float)position.x, (float)position.y, p.x, p.y);
			return p;
		}
		Vec2i screenToWorld(float x, float y) const
		{
			Vec2i p;
			screenToWorld(x, y, p.x, p.y);
			return p;
		}
		template <typename S>
		RectF screenToWorld(const Rectangle<S>& rect)const
		{
			Vec2i p = screenToWorld((float)rect.x, (float)rect.y);
			float w = rect.w / scaleX;
			float h = rect.h / scaleY;

			return { (float)p.x, (float)p.y, w,h };
		}

		inline float getScaleX() const { return scaleX; }
		inline float getScaleY() const { return scaleY; }
	};
}
