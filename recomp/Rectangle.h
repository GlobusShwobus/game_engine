#pragma once

#include "Vec2M.h"
#include "badConcepts.h"

namespace badEngine {

	template<typename T> requires IS_MATHMATICAL_VECTOR_T<T>
	class Rectangle {
	public:
		constexpr Rectangle()noexcept = default;
		constexpr Rectangle(T x, T y, T w, T h) noexcept :mPosition(x,y), mDimensions(w,h) {}
		
		template <typename S, typename U> requires IS_MATHMATICAL_VECTOR_T<U>
		constexpr Rectangle(const Vec2M<S>& pos, U w, U h) noexcept : mPosition(pos), mDimensions(w,h) {}

		template <typename S, typename U> requires IS_MATHMATICAL_VECTOR_T<U>
		constexpr Rectangle(U x, U y, const Vec2M<S>& dimensions) noexcept : mPosition(x,y), mDimensions(dimensions){}

		template <typename S, typename U>
		constexpr Rectangle(const Vec2M<S>& pos, const Vec2M<U>& dimensions)noexcept :mPosition(pos), mDimensions(dimensions) {}
		
		template<typename S>
		constexpr Rectangle(const Rectangle<S>& rhs)noexcept :mPosition(rhs.mPosition), mDimensions(rhs.mDimensions) {}

		template<typename S>
		constexpr Rectangle& operator=(const Rectangle<S>& rhs)noexcept {
			mPosition   = rhs.mPosition;
			mDimensions = rhs.mDimensions;
			return *this;
		}
		template<typename S>
		constexpr bool operator==(const Rectangle<S>& rhs)const noexcept {
			return mDimensions == rhs.mDimensions;
		}
		template<typename S>
		constexpr bool operator!=(const Rectangle<S>& rhs) const noexcept {
			return!(*this == rhs);
		}

		constexpr auto get_half_dimensions()const noexcept {
			return mDimensions * 0.5f;
		}
		constexpr auto get_center_point()const noexcept {
			return mPosition + get_half_dimensions();
		}

	public:
		Vec2M<T> mPosition;
		Vec2M<T> mDimensions;
	};

	using rectS  = Rectangle<short>;
	using rectI  = Rectangle<int>;
	using rectL  = Rectangle<long>;
	using rectLL = Rectangle<long long>;
	using rectF  = Rectangle<float>;
	using rectD  = Rectangle<double>;
	using rectLD = Rectangle<long double>;
}