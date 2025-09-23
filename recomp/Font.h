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

		}

	public:
		
		Sprite& mSprite;

		const uint32_t mColumnsCount = 0;
		const uint32_t mRowsCount = 0;
		const uint32_t mGylphWidth = 0;
		const uint32_t mGylphHeight = 0;

		static constexpr char firstChar = ' ';
		static constexpr char lastChar = '~';
	};

}
/*
	SequenceM<vec2i> Font::get_font_source_list(std::string_view text) {
		SequenceM<vec2i> frames;
		frames.set_reserve_size(text.size());
		for (char c : text) {
			if (c == '\n') {
				continue;
			}
			else if (c >= firstChar + 1 && c <= lastChar) {
				const int gylphIndex = c - firstChar;
				const int yGylph = gylphIndex / mColumnsCount;
				const int xGylph = gylphIndex % mColumnsCount;

				frames.element_create(xGylph * mCharWidth, yGylph * mCharHeight);
			}
		}

		return frames;
	}
*/