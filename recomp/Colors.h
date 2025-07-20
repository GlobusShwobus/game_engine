#pragma once

namespace badEngine {
	class Color {
		unsigned int dword;

	public:
		constexpr Color() :dword() {}
		constexpr Color(const Color& col) :dword(col.dword) {}
		constexpr Color(unsigned int dw) :dword(dw) {}
		constexpr Color(unsigned char a, unsigned char r, unsigned char g, unsigned char b)
			:dword((a << 24u) | (r << 16u) | (g << 8u) | b){}
		constexpr Color(unsigned char r, unsigned char g, unsigned char b)
			:dword((r << 16u) | (g << 8u) | b) {}
		

		Color& operator =(unsigned int color) {
			dword = color;
			return *this;
		}
		Color& operator =(Color color){
			dword = color.dword;
			return *this;
		}
		bool operator==(const Color& color)const {
			return dword == color.dword;
		}
		bool operator!=(const Color& color)const {
			return !(*this == color);
		}

		void setAlpha(unsigned char alpha) {
			dword = (dword & 0xFFFFFFu) | (alpha << 24u);//mask first then write, not dword |= alpha << 24u
		}
		void setRed(unsigned char red) {
			dword = (dword & 0xFF00FFFFu) | (red << 16u);
		}
		void setGreen(unsigned char green) {
			dword = (dword & 0xFFFF00FFu) | (green << 8u);
		}
		void setBlue(unsigned char blue) {
			dword = (dword & 0xFFFFFF00u) | blue;
		}

		unsigned char getAlpha() {
			return (dword >> 24u) & 0xFFu;
		}
		unsigned char getRed() {
			return (dword >> 16u) & 0xFFu;
		}
		unsigned char getGreen() {
			return (dword >> 8u) & 0xFFu;
		}
		unsigned char getBlue() {
			return dword & 0xFFu;
		}

	};
	namespace Colors {
		static constexpr Color makeRGB(unsigned char r, unsigned char g, unsigned char b) {
			return (r << 16u) | (g << 8u) | b;
		}
		static constexpr Color White = makeRGB(255u, 255u, 255u);
		static constexpr Color Black = makeRGB(0u, 0u, 0u);
		static constexpr Color Gray = makeRGB(0x80u, 0x80u, 0x80u);
		static constexpr Color Red = makeRGB(255u, 0u, 0u);
		static constexpr Color Green = makeRGB(0u, 255u, 0u);
		static constexpr Color Blue = makeRGB(0u, 0u, 255u);
		static constexpr Color Yellow = makeRGB(255u, 255u, 0u);
		static constexpr Color Cyan = makeRGB(0u, 255u, 255u);
		static constexpr Color Magenta = makeRGB(255u, 0u, 255u);
	}
}