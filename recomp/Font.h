#pragma once

#include <string_view>
#include "GPUSprite.h"

namespace badEngine {

	class Font {

		Sprite sprite;

		int gylphWidth;
		int gylphHeight;

		int nColumns;
		int nRows;

		static constexpr char firstChar = ' ';
		static constexpr char lastChar = '~';

	public:
		Font(std::string_view path, SDL_Renderer* renderer, int columns, int rows);

		void DrawFont(std::string_view text, SDL_Renderer* renderer, int x, int y);
	};

}