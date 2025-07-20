#pragma once
#include <string_view>
#include "Colors.h"

namespace badEngine {

	class Surface {

		Color* pPixels = nullptr;
		int width;
		int height;

	public:
		Surface(std::string_view path);
		Surface(int width, int height);
		Surface(const Surface&);
		~Surface();
		Surface& operator=(const Surface&);

		void putPixel(int x, int y, Color c);
		Color getPixel(int x, int y)const;
		const Color* const data()const;

		int getWidth()const;
		int getHeight()const;
	};

}