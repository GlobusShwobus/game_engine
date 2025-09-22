#pragma once

#include "Rectangle.h"
#include "Vec2M.h"

namespace badEngine {

	class Camera2D {
	public:
		constexpr Camera2D(int screenWidth, int screenHeight)noexcept :mScreenDimensions(screenWidth, screenHeight) {}
		template<typename S>
		constexpr Camera2D(const Vec2M<S>& screenDimensions)noexcept :mScreenDimensions(screenDimensions) {}


		constexpr vec2i get_scale()const noexcept {
			return mScale;
		}
		constexpr void set_screen_dimensions(int screenWidth, int screenHeight)noexcept {
			set_screen_dimensions(vec2i(screenWidth, screenHeight));
		}
		template<typename S>
		constexpr void set_screen_dimensions(Vec2M<S> dimensions)noexcept {
			mScreenDimensions = dimensions;
		}
		constexpr void zoom(float factor)noexcept {
			mScale *= factor;
		}

		template<typename S>
		constexpr void focus_on(Vec2M<S> point)noexcept {
			mOffset = point - (mScreenDimensions * 0.5f) / mScale;
		}
		template<typename S>
		constexpr void focus_on(const Rectangle<S>& rect)noexcept {
			focus_on(rect.get_center_point());
		}
		constexpr void focus_on(float x, float y)noexcept {
			focus_on(vec2f(x, y));
		}
		template<typename S>
		constexpr void move(Vec2M<S> point)noexcept {
			mOffset += point / mScale;
		}
		constexpr void move(float x, float y)noexcept {
			move(vec2f(x, y));
		}

		template<typename S>
		constexpr vec2f world_to_screen_point(const Vec2M<S>& worldPoint)const noexcept {
			return vec2f((worldPoint - mOffset) * mScale);
		}
		template<typename S>
		constexpr rectF world_to_screen_rectangle(const Rectangle<S>& worldRect)const noexcept {
			auto position = world_to_screen_point(worldRect.mPosition);
			return rectF(position, worldRect.mDimensions * mScale);
		}

		template <typename S>
		constexpr vec2f screen_to_world_point(const Vec2M<S>& screenPoint)const noexcept {
			return vec2f((screenPoint / mScale) + mOffset);
		}
		template <typename S>
		constexpr rectF screen_to_world_rectangle(const Rectangle<S>& screenRect)const noexcept {
			auto position = screen_to_world_point(screenRect.mPosition);
			return rectF(position, screenRect.mDimensions / mScale);
		}

	private:
		vec2f mOffset;
		vec2f mScale;

		vec2i mScreenDimensions;
	};
}