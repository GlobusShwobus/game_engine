#pragma once

#include "Sprite.h"
#include "Vec2M.h"

#include <string_view>

namespace badEngine {

	class Font {
	public:

		Font(Sprite& sprite, uint32_t columnsCount, uint32_t rowsCount);

		void draw_text(std::string_view string, SDL_Renderer* rendererRef, const vec2i& pos);

	private:

		Sprite& mSprite;

		const uint32_t mColumnsCount = 0;
		const uint32_t mRowsCount = 0;

		const uint32_t mGylphWidth = 0;
		const uint32_t mGylphHeight = 0;

		static constexpr char first_ASCII_character = ' ';
		static constexpr char last_ASCII_character = '~';
	};

}

