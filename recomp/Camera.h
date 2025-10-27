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
		void zoom(float factor)noexcept {
			mScale *= factor;
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

		template<typename S>
		rectF screen_to_world(const Rectangle<S>& screenRect)const noexcept {
			return rectF(
				(screenRect.x / mScale.x) + mOffset.x,
				(screenRect.y / mScale.y) + mOffset.y,
				screenRect.w / mScale.x,
				screenRect.h / mScale.y
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

		void move(vec2f point)noexcept {
			mOffset += point / mScale;
		}
		void move(float x, float y)noexcept {
			move(vec2f(x, y));
		}

	private:
		vec2f mOffset;
		vec2f mScale = vec2f(1.0f, 1.0f);

		vec2i mScreenSize;
	};
}