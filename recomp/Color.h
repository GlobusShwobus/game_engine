#pragma once

namespace badEngine {
	class Color {
		unsigned int dword;

	public:
		constexpr Color()                 noexcept :dword(0) {}
		constexpr Color(unsigned int dw)  noexcept :dword(dw) {}

		constexpr Color(unsigned char a, unsigned char r, unsigned char g, unsigned char b)noexcept
			: dword((a << 24u) | (r << 16u) | (g << 8u) | b) {
		}
		constexpr Color(unsigned char r, unsigned char g, unsigned char b)noexcept
			: dword((r << 16u) | (g << 8u) | b) {
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

		constexpr void set_alpha(unsigned char alpha)noexcept {
			dword = (dword & 0xFFFFFFu) | (alpha << 24u);  //mask first then write, not dword |= alpha << 24u
		}
		constexpr void set_red(unsigned char red)noexcept {
			dword = (dword & 0xFF00FFFFu) | (red << 16u);
		}
		constexpr void set_green(unsigned char green)noexcept {
			dword = (dword & 0xFFFF00FFu) | (green << 8u);
		}
		constexpr void set_blue(unsigned char blue)noexcept {
			dword = (dword & 0xFFFFFF00u) | blue;
		}

		constexpr unsigned char get_alpha()const noexcept {
			return (dword >> 24u) & 0xFFu;
		}
		constexpr unsigned char get_red()const noexcept {
			return (dword >> 16u) & 0xFFu;
		}
		constexpr unsigned char get_green()const noexcept {
			return (dword >> 8u) & 0xFFu;
		}
		constexpr unsigned char get_blue()const noexcept {
			return dword & 0xFFu;
		}

	};
	namespace Colors {
		static constexpr Color makeRGB(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
			return (a << 24) | (r << 16u) | (g << 8u) | b;
		}
		static constexpr Color White = makeRGB(255u, 255u, 255u, 255u);
		static constexpr Color Black = makeRGB(0u, 0u, 0u, 255u);
		static constexpr Color Gray = makeRGB(0x80u, 0x80u, 0x80u, 255u);
		static constexpr Color Red = makeRGB(255u, 0u, 0u, 255u);
		static constexpr Color Green = makeRGB(0u, 255u, 0u, 255u);
		static constexpr Color Blue = makeRGB(0u, 0u, 255u, 255u);
		static constexpr Color Yellow = makeRGB(255u, 255u, 0u, 255u);
		static constexpr Color Cyan = makeRGB(0u, 255u, 255u, 255u);
		static constexpr Color Magenta = makeRGB(255u, 0u, 255u, 255u);
	}
}