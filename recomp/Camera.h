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
		inline void updateScreenSize(const Vec2i& dimensions)
		{
			updateScreenSize(dimensions.x, dimensions.y);
		}

		inline void focusPoint(float x, float y)
		{
			offsetX = x - (screenW * 0.5f) / scaleX;	
			offsetY = y - (screenH * 0.5f) / scaleY;	
		}
		inline void focusPoint(const Vec2f& position)
		{
			focusPoint(position.x, position.y);
		}
		inline void focusPoint(float x, float y, float w, float h)
		{
			focusPoint(x + (w * 0.5f), y + (h * 0.5f));
		}
		inline void focusPoint(const RectF& rect)
		{
			focusPoint(rect.x + rect.halfWidth(), rect.y + rect.halfHeight());
		}
		inline void focusPoint(const RectI& rect)
		{
			focusPoint(float(rect.x + rect.halfWidth()), float(rect.y + rect.halfHeight()));
		}

		inline void move(float deltaX, float deltaY)
		{
			offsetX += deltaX / scaleX;
			offsetY += deltaY / scaleY;
		}
		inline void move(const Vec2f& delta)
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
		Vec2f worldToScreen(const Vec2i& position)const;
		Vec2f worldToScreen(int x, int y)const;
		RectF worldToScreen(const RectF& rect)const;

		inline void screenToWorld(float screenX, float screenY, int& worldX, int& worldY) const
		{
			worldX = screenX / scaleX + offsetX;
			worldY = screenY / scaleY + offsetY;
		}
		Vec2i screenToWorld(const Vec2f& position) const;
		Vec2i screenToWorld(float x, float y) const;
		RectF screenToWorld(const RectF& rect)const;

		inline float getScaleX() const { return scaleX; }
		inline float getScaleY() const { return scaleY; }
	};
}
