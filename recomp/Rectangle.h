#pragma once

#include "Vec2M.h"

namespace badEngine {

	template<typename T> requires IS_MATHMATICAL_VECTOR_T<T>
	class Rectangle {
	public:
		constexpr Rectangle()noexcept = default;
		constexpr Rectangle(T x, T y, T w, T h) noexcept :x(x),y(y),w(w),h(h) {}

		template <typename S, typename U> requires IS_MATHMATICAL_VECTOR_T<U>
		constexpr Rectangle(const Vec2M<S>& pos, U w, U h) noexcept : x(pos.x), y(pos.y), w(w), h(h) {}

		template <typename S, typename U> requires IS_MATHMATICAL_VECTOR_T<U>
		constexpr Rectangle(U x, U y, const Vec2M<S>& dimensions) noexcept : x(x),y(y), w(dimensions.x), h(dimensions.y) {}

		template <typename S, typename U>
		constexpr Rectangle(const Vec2M<S>& pos, const Vec2M<U>& dimensions)noexcept :x(pos.x), y(pos.y), w(dimensions.x), y(dimensions.y) {}

		template<typename S>
		constexpr Rectangle(const Rectangle<S>& rhs)noexcept :x(rhs.x), y(rhs.y), w(rhs.w), y(rhs.y) {}

		template<typename S>
		constexpr Rectangle& operator=(const Rectangle<S>& rhs)noexcept {
			x = rhs.x;
			y = rhs.y;
			w = rhs.w;
			h = rhs.h;
			return *this;
		}
		template<typename S>
		constexpr bool operator==(const Rectangle<S>& rhs)const noexcept {
			return (x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h);
		}
		template<typename S>
		constexpr bool operator!=(const Rectangle<S>& rhs) const noexcept {
			return!(*this == rhs);
		}

		constexpr Vec2M<T> get_center_point()const noexcept {
			return Vec2M<T>(x + (w * 0.5f), y + (h * 0.5f));
		}
		Vec2M<T> get_half_size()const noexcept {
			return Vec2M<T>(w * 0.5f, h * 0.5f);
		}

		constexpr bool contains_point(T X, T Y)const noexcept {
			return (
				X >= x &&
				Y >= y &&
				X < x + w &&
				Y < y + h
				);
		}
		constexpr bool contains_point(const Vec2M<T>& pos)const noexcept {
			return contains_point(pos.x, pos.y);
		}

		template <typename S>
		bool contains_rect(const Rectangle<S>& contained)const noexcept {
			return (
				contained.x >= x &&
				contained.y >= y &&
				contained.x + contained.w <= x + w &&
				contained.y + contained.h <= y + h
				);
		}

		template <typename S>
		constexpr bool intersects_rect(const Rectangle<S>& rhs)const noexcept {
			return(
				x < rhs.x + rhs.w &&
				x + w > rhs.x &&
				y < rhs.y + rhs.h &&
				y + h > rhs.y
				);
		}

	public:
		T x = 0;
		T y = 0;
		T w = 0;
		T h = 0;
	};

	using rectS  = Rectangle<short>;
	using rectI  = Rectangle<int>;
	using rectL  = Rectangle<long>;
	using rectLL = Rectangle<long long>;
	using rectF  = Rectangle<float>;
	using rectD  = Rectangle<double>;
	using rectLD = Rectangle<long double>;
}