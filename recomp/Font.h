#pragma once

#include "Sprite.h"
#include "SequenceM.h"
#include "Vec2M.h"
#include <cassert>
#include <string_view>

namespace badEngine {

	class Font {
	
	public:

		Font(Sprite& sprite, uint32_t columnsCount, uint32_t rowsCount)
			:mSprite(sprite),
			mColumnsCount(columnsCount),
			mRowsCount(rowsCount),
			mGylphWidth(sprite.get_width() / columnsCount),
			mGylphHeight(sprite.get_height() / rowsCount)
		{
			assert(mGylphWidth * mColumnsCount == sprite.get_width());
			assert(mGylphHeight * mRowsCount == sprite.get_height());
		}

		void draw(std::string_view string, SDL_Renderer& renderer, const vec2i& pos) {

			vec2i iteratedPosition = pos;

			for (char c : string) {
				if (c == '\n') {
					iteratedPosition = vec2i(pos.x, iteratedPosition.y += mGylphHeight);
					continue;
				}
				else if (c >= first_ASCII_character + 1 && c <= last_ASCII_character) {
					const int gylphIndex = c - first_ASCII_character;
					const int yGylph = gylphIndex / mColumnsCount;
					const int xGylph = gylphIndex % mColumnsCount;

					sprite.setSourceX(float(xGylph * gylphWidth));
					sprite.setSourceY(float(yGylph * gylphHeight));

					sprite.DrawTexture(renderer, curX, curY);
				}
				curX += gylphWidth;//if char is the empty space key, this by default skips over it and adds padding as well
			}
		}

	public:
		
		Sprite& mSprite;

		const uint32_t mColumnsCount = 0;
		const uint32_t mRowsCount = 0;
		const uint32_t mGylphWidth = 0;
		const uint32_t mGylphHeight = 0;

		static constexpr char first_ASCII_character = ' ';
		static constexpr char last_ASCII_character = '~';
	};

}

