#pragma once

#include "Sprite.h"
#include "Vec2M.h"

#include <cassert>
#include <string_view>

namespace badEngine {

	class Font: protected Sprite {
	
	public:

		Font(Sprite sprite, uint32_t columnsCount, uint32_t rowsCount)
			:Sprite(std::move(sprite)),
			mColumnsCount(columnsCount),
			mRowsCount(rowsCount),
			mGylphWidth(this->texture_width() / columnsCount),
			mGylphHeight(this->texture_height() / rowsCount)
		{
			assert(mGylphWidth * mColumnsCount == this->texture_width());
			assert(mGylphHeight * mRowsCount == this->texture_height());
			this->set_source_size(vec2i(mGylphWidth, mGylphHeight));
		}

		void draw_text(std::string_view string, SDL_Renderer& renderer, const vec2i& pos) {

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

					this->set_source_position(vec2i(xGylph * mGylphWidth, yGylph * mGylphHeight));

					this->draw(renderer, iteratedPosition);
				}
				iteratedPosition.x += mGylphWidth;//if char is the empty space key, this by default skips over it and adds padding as well
			}
		}

	public:
		
		const uint32_t mColumnsCount = 0;
		const uint32_t mRowsCount = 0;

		const uint32_t mGylphWidth = 0;
		const uint32_t mGylphHeight = 0;

		static constexpr char first_ASCII_character = ' ';
		static constexpr char last_ASCII_character = '~';
	};

}

