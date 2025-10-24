#include "Font.h"
#include <stdexcept>
namespace badEngine {

	Font::Font(Sprite& sprite, uint32_t columnsCount, uint32_t rowsCount)
		:mSprite(sprite),
		mColumnsCount(columnsCount),
		mRowsCount(rowsCount),
		mGylphWidth(mSprite.get_texture_width() / columnsCount),
		mGylphHeight(mSprite.get_texture_height() / rowsCount)
	{
		if ((mGylphWidth * mColumnsCount) != mSprite.get_texture_width())
			throw std::runtime_error("Missmatch between width calculations (texture image likely off size or invalid counts)");
		if ((mGylphHeight * mRowsCount) != mSprite.get_texture_height())
			throw std::runtime_error("Missmatch between height calculations (texture image likely off size or invalid counts)");

		//checks above should take care of it, check here would be overkill
		mSprite.set_source_size(vec2i(mGylphWidth, mGylphHeight));
		mSprite.set_destination_size(vec2i(mGylphWidth, mGylphHeight));
	}

	void Font::draw_text(std::string_view string, SDL_Renderer* rendererRef, const vec2i& pos) {

		vec2i iteratedPosition = pos;

		for (char c : string) {
			if (c == '\n') {
				//if new line start in the same position on x axis but below offset by 1 amount of height
				iteratedPosition = vec2i(pos.x, iteratedPosition.y += mGylphHeight);
				continue;
			}
			// if char is the empty space key (c == first_ASCII_character), then skip this part as in anycase
			// position is incremented for the next iteration in the loop
			else if (c >= first_ASCII_character + 1 && c <= last_ASCII_character) {
				const int gylphIndex = c - first_ASCII_character;
				const int yGylph = gylphIndex / mColumnsCount;//ASCII math
				const int xGylph = gylphIndex % mColumnsCount;//ASCII math

				mSprite.set_source_position(vec2i(xGylph * mGylphWidth, yGylph * mGylphHeight));//this is fine, as constructor should assure it
				mSprite.set_destination_position(iteratedPosition);
				mSprite.draw(rendererRef);
			}
			iteratedPosition.x += mGylphWidth;
		}
	}
}