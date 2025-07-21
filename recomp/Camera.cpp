#include "Camera.h"

namespace badEngine {

	Vec2f Camera2D::worldToScreen(const Vec2i& position) const
	{
		Vec2f p;
		worldToScreen(position.x, position.y, p.x, p.y);
		return p;
	}
	Vec2f Camera2D::worldToScreen(int x, int y) const
	{
		Vec2f p;
		worldToScreen(x, y, p.x, p.y);
		return p;
	}
	RectF Camera2D::worldToScreen(const RectF& rect)const
	{
		Vec2f p = worldToScreen((int)rect.x, (int)rect.y);
		float w = rect.w * scaleX;
		float h = rect.h * scaleY;

		return { p.x, p.y, w,h };
	}
	Vec2i Camera2D::screenToWorld(const Vec2f& position) const
	{
		Vec2i p;
		screenToWorld(position.x, position.y, p.x, p.y);
		return p;
	}
	Vec2i Camera2D::screenToWorld(float x, float y) const
	{
		Vec2i p;
		screenToWorld(x, y, p.x, p.y);
		return p;
	}
	RectF Camera2D::screenToWorld(const RectF& rect)const
	{
		Vec2i p = screenToWorld(rect.x, rect.y);
		float w = rect.w / scaleX;
		float h = rect.h / scaleY;

		return { (float)p.x, (float)p.y, w,h };
	}
}