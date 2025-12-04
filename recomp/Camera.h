#pragma once

#include "Rectangle.h"
#include "Vec2M.h"
#include <cassert>
namespace badEngine {

	class Camera2D {

	public:

		Camera2D(int screenWidth, int screenHeight)noexcept :mScreenSize(screenWidth, screenHeight) {}
		Camera2D(vec2i screenSize)noexcept :mScreenSize(std::move(screenSize)) {}

		//SCALE AND ZOOM
		vec2f get_scale()const noexcept {
			return mScale;
		}

		void zoom_towards(float factor, const vec2f& focus)noexcept {
			//before zoom coordiante
			vec2f worldBefore = screen_to_world_point(focus);
			//apply zoom
			mScale *= factor;
			//after zoom coordinate
			vec2f worldAfter = screen_to_world_point(focus);
			//diffrence to keep offset fixed
			mOffset += (worldBefore - worldAfter);
		}

		//SCREEN
		vec2i get_screen()const noexcept {
			return mScreenSize;
		}
		void set_screen_update(int screenWidth, int screenHeight)noexcept {
			
			auto old = mScreenSize;
			mScreenSize = vec2i(screenWidth, screenHeight);

			float proportionalX = static_cast<float>(mScreenSize.x) / old.x;
			float proportionalY = static_cast<float>(mScreenSize.y) / old.y;

			mScale.x *= proportionalX;
			mScale.y *= proportionalY;
		}

		//TRANSLATE A BEWEEN WORLD AND SCREEN SPACE
		template<typename S>
		rectF world_to_screen(const Rectangle<S>& worldRect)const noexcept {
			return rectF(
				(worldRect.x - mOffset.x) * mScale.x,
				(worldRect.y - mOffset.y) * mScale.y,
				worldRect.w * mScale.x,
				worldRect.h * mScale.y
			);
		}
		vec2f screen_to_world_point(const vec2f& screenPoint)const noexcept {
			return vec2f(
				(screenPoint.x / mScale.x) + mOffset.x,
				(screenPoint.y / mScale.y) + mOffset.y
			);
		}
		
		rectF get_view_rect()const noexcept {
			return rectF(
				mOffset, vec2f(mScreenSize.x / mScale.x, mScreenSize.y / mScale.y)
			);
		}

		void focus_on(const vec2f& point)noexcept {
			mOffset.x = point.x - (mScreenSize.x * 0.5f) / mScale.x;
			mOffset.y = point.y - (mScreenSize.y * 0.5f) / mScale.y;
		}
		
		void pan(vec2f point)noexcept {
			mOffset.x += point.x / mScale.x;
			mOffset.y += point.y / mScale.y;
		}
		void pan(float x, float y)noexcept {
			pan(vec2f(x, y));
		}

	private:
		vec2f mOffset;

		vec2f mScale = vec2f(1.0f, 1.0f);

		vec2i mScreenSize;
	};
}