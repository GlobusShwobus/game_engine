#pragma once

#include "Rectangle.h"
#include "Vec2M.h"

namespace badEngine {

	class Camera2D {

	public:

		Camera2D(int screenWidth, int screenHeight)noexcept :mScreenSize(screenWidth, screenHeight) {}
		Camera2D(vec2i screenSize)noexcept :mScreenSize(std::move(screenSize)) {}

		//SCALE AND ZOOM
		float get_scale()const noexcept {
			return mScale;
		}

		void set_scale(float factor)noexcept {
			if (factor > 0.0f) {
				mScale = factor;
			}
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
				(worldRect.get_pos() - mOffset) * mScale,
				worldRect.get_size() * mScale
			);
		}
		vec2f screen_to_world_point(const vec2f& screenPoint)const noexcept {
			return vec2f(
				(screenPoint / mScale) + mOffset);
		}
		
		rectF get_view_rect()const noexcept {
			return rectF(
				mOffset, mScreenSize / mScale
			);
		}

		void focus_on(const vec2f& point)noexcept {
			mOffset = point - (mScreenSize * 0.5f) / mScale;
		}
		
		void pan(vec2f point)noexcept {
			mOffset += point / mScale;
		}
		void pan(float x, float y)noexcept {
			pan(vec2f(x, y));
		}

	private:
		vec2f mOffset;

		float mScale = 1.f;

		vec2i mScreenSize;
	};
}