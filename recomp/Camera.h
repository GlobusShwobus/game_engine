#pragma once

#include "Rectangle.h"
#include "Vec2M.h"
#include <cassert>
namespace badEngine {

	class Camera2D {

	public:

		Camera2D(int screenWidth, int screenHeight)noexcept :mScreenSize(screenWidth, screenHeight) {}
		Camera2D(int2 screenSize)noexcept :mScreenSize(std::move(screenSize)) {}

		//SCALE AND ZOOM
		constexpr float2 get_scale()const noexcept {
			return mScale;
		}

		constexpr void zoom_towards(float factor, const float2& focus_point)noexcept {
			//before zoom coordiante
			float2 worldBefore = screen_to_world_point(focus_point);
			//apply zoom
			mScale *= factor;
			//after zoom coordinate
			float2 worldAfter = screen_to_world_point(focus_point);
			//diffrence to keep offset fixed
			mOffset += (worldBefore - worldAfter);
		}

		//SCREEN
		constexpr int2 get_screen()const noexcept {
			return mScreenSize;
		}
		constexpr void set_screen_update(int screenWidth, int screenHeight)noexcept {
			
			auto old = mScreenSize;
			mScreenSize = int2(screenWidth, screenHeight);

			float proportionalX = static_cast<float>(mScreenSize.x) / old.x;
			float proportionalY = static_cast<float>(mScreenSize.y) / old.y;

			mScale.x *= proportionalX;
			mScale.y *= proportionalY;
		}

		//TRANSLATE A BEWEEN WORLD AND SCREEN SPACE
		template<typename S>
		constexpr AABB world_to_screen(const Rectangle<S>& worldRect)const noexcept {
			return rectF(
				(worldRect.x - mOffset.x) * mScale.x,
				(worldRect.y - mOffset.y) * mScale.y,
				worldRect.w * mScale.x,
				worldRect.h * mScale.y
			);
		}
		constexpr float2 screen_to_world_point(const float2& screenPoint)const noexcept {
			return float2(
				(screenPoint.x / mScale.x) + mOffset.x,
				(screenPoint.y / mScale.y) + mOffset.y
			);
		}
		
		constexpr AABB get_view_rect()const noexcept {
			return AABB(
				mOffset, float2(mScreenSize.x / mScale.x, mScreenSize.y / mScale.y)
			);
		}

		void focus_on(const float2& point)noexcept {
			mOffset.x = point.x - (mScreenSize.x * 0.5f) / mScale.x;
			mOffset.y = point.y - (mScreenSize.y * 0.5f) / mScale.y;
		}
		
		void pan(float2 point)noexcept {
			mOffset.x += point.x / mScale.x;
			mOffset.y += point.y / mScale.y;
		}
		void pan(float x, float y)noexcept {
			pan(float2(x, y));
		}

	private:
		float2 mOffset;
		float2 mScale = float2(1.0f, 1.0f);
		int2 mScreenSize;
	};
}