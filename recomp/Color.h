#pragma once

namespace badEngine {
	class Color {
		unsigned int dword;

	public:
		constexpr Color()                 noexcept :dword(0) {}
		constexpr Color(unsigned int dw)  noexcept :dword(dw) {}

		constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)noexcept
			: dword((a << 24u) | (b << 16u) | (g << 8u) | r) {
		}
		constexpr Color(uint8_t r, uint8_t g, uint8_t b)noexcept
			: dword((255u << 24u) | (b << 16u) | (g << 8u) | r) {
		}
		constexpr Color& operator=(unsigned int color) noexcept {
			dword = color;
			return *this;
		}
		constexpr bool operator==(const Color& color)const noexcept {
			return dword == color.dword;
		}
		constexpr bool operator!=(const Color& color)const noexcept {
			return !(*this == color);
		}

		constexpr void set_alpha(uint8_t alpha)noexcept {
			dword = (dword & 0xFFFFFFu) | (alpha << 24u);
		}
		constexpr void set_blue(uint8_t blue)noexcept {
			dword = (dword & 0xFF00FFFFu) | (blue << 16u);
		}
		constexpr void set_green(uint8_t green)noexcept {
			dword = (dword & 0xFFFF00FFu) | (green << 8u);
		}
		constexpr void set_red(uint8_t red)noexcept {
			dword = (dword & 0xFFFFFF00u) | red;
		}

		constexpr uint8_t get_alpha()const noexcept {
			return (dword >> 24u) & 0xFFu;
		}
		constexpr uint8_t get_blue()const noexcept {
			return (dword >> 16u) & 0xFFu;
		}
		constexpr uint8_t get_green()const noexcept {
			return (dword >> 8u) & 0xFFu;
		}
		constexpr uint8_t get_red()const noexcept {
			return dword & 0xFFu;
		}

	};
	namespace Colors {
		static constexpr Color makeRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
			return (a << 24u) | (b << 16u) | (g << 8u) | r;
		}

		static constexpr Color White   = makeRGBA(255u, 255u, 255u, 255u);
		static constexpr Color Black   = makeRGBA(0u, 0u, 0u, 255u);
		static constexpr Color Gray    = makeRGBA(0x80u, 0x80u, 0x80u, 255u);
		static constexpr Color Red     = makeRGBA(255u, 0u, 0u, 255u);
		static constexpr Color Green   = makeRGBA(0u, 255u, 0u, 255u);
		static constexpr Color Blue    = makeRGBA(0u, 0u, 255u, 255u);
		static constexpr Color Yellow  = makeRGBA(255u, 255u, 0u, 255u);
		static constexpr Color Cyan    = makeRGBA(0u, 255u, 255u, 255u);
		static constexpr Color Magenta = makeRGBA(255u, 0u, 255u, 255u);
	}
}