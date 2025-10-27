#pragma once

#include "Rectangle.h"
#include "Vec2M.h"

namespace badEngine {

	class Camera2D {

	public:

		Camera2D(int screenWidth, int screenHeight)noexcept :mScreenSize(screenWidth, screenHeight) {}
		Camera2D(vec2i screenSize)noexcept :mScreenSize(std::move(screenSize)) {}

		//SCALE AND ZOOM
		vec2f get_scale()const noexcept {
			return mScale;
		}
		void set_scale(float x, float y)noexcept {
			mScale = vec2f(x, y);
		}
		void set_scale(float x)noexcept {
			mScale = vec2f(x, x);
		}
		void set_scale(vec2f scale)noexcept {
			mScale = std::move(scale);
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
		void set_screen(int screenWidth, int screenHeight)noexcept {
			mScreenSize = vec2i(screenWidth, screenHeight);
		}
		void set_screen(vec2i size)noexcept {
			mScreenSize = std::move(size);
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

		//UPDATE CAMERA
		void focus_on(vec2f point)noexcept {
			mOffset = point - (mScreenSize * 0.5f) / mScale;
		}
		template<typename S>
		void focus_on(const Rectangle<S>& rect)noexcept {
			focus_on(rect.get_center_point());
		}

		
		void pan(vec2f point)noexcept {
			mOffset += point / mScale;
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