#pragma once

#include "Vec2M.h"

namespace badEngine {

	template<typename T> 
		requires IS_MATHMATICAL_VECTOR_T<T>
	class Rectangle {
	public:
		//CONSTRUCTORS
		constexpr Rectangle()noexcept = default;
		constexpr Rectangle(T x, T y, T w, T h) noexcept :x(x), y(y), w(w), h(h) {}
		constexpr Rectangle(const Vec2M<T>& pos, const Vec2M<T>& size) noexcept : x(pos.x), y(pos.y), w(size.x), h(size.y) {}

		//CONVERSION CONSTRUCTOR
		template<typename S>
		constexpr Rectangle(const Rectangle<S>& rhs)noexcept :x(static_cast<T>(rhs.x)), y(static_cast<T>(rhs.y)), w(static_cast<T>(rhs.w)), h(static_cast<T>(rhs.h)) {}
		//CONVERSION ASSIGNMENT
		template<typename S>
		constexpr Rectangle& operator=(const Rectangle<S>& rhs)noexcept {
			x = static_cast<T>(rhs.x);
			y = static_cast<T>(rhs.y);
			w = static_cast<T>(rhs.w);
			h = static_cast<T>(rhs.h);
			return *this;
		}


		template<typename S>
		constexpr bool is_same_size(const Rectangle<S>& rhs)const noexcept {
			return w == rhs.w && h == rhs.h;
		}

		template <typename S>
		constexpr bool contains(const Vec2M<S>& pos)const noexcept {
			return
				pos.x >= x &&
				pos.y >= y &&
				pos.x <= x + w &&
				pos.y <= y + h;
		}

		template <typename S>
		constexpr bool contains(const Rectangle<S>& rect)const noexcept {
			return 
				rect.x >= x &&
				rect.y >= y &&
				rect.x + rect.w <= x + w &&
				rect.y + rect.h <= y + h;
		}

		template <typename S>
		constexpr bool intersects(const Rectangle<S>& rhs)const noexcept {
			return
				x < rhs.x + rhs.w &&
				x + w > rhs.x &&
				y < rhs.y + rhs.h &&
				y + h > rhs.y;
		}

		constexpr void set_pos(const Vec2M<T>& pos)noexcept {
			x = pos.x;
			y = pos.y;
		}
		constexpr void increment_pos(const Vec2M<T>& incr)noexcept {
			x += incr.x;
			y += incr.y;
		}
		constexpr void set_size(const Vec2M<T>& size)noexcept {
			w = size.x;
			h = size.y;
		}

		constexpr Vec2M<T> get_pos()const noexcept {
			return Vec2M<T>(x, y);
		}
		constexpr Vec2M<T> get_size()const noexcept {
			return Vec2M<T>(w, h);
		}
		constexpr vec2f get_center_point()const noexcept {
			return vec2f(
				x + (w * 0.5f),
				y + (h * 0.5f)
			);
		}
		constexpr vec2f get_half_size()const noexcept {
			return vec2f(w * 0.5f, h * 0.5f);
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