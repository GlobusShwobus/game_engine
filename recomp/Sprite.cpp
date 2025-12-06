#include "Sprite.h"

namespace badEngine {


	//####################################################################################

	Animation::Animation(Texture* texture, const vec2i& start, uint16_t fWidth, uint16_t fHeight, uint16_t fCount)
		:Sprite(texture), mFrameCount(fCount)
	{
		assert(start.x >= 0 && start.y >= 0 && "Out of bounds texture access");

		const rectF requiredArea = rectF(
			start.x,
			start.y,
			start.x + (fWidth * fCount),
			start.y + (fHeight * fCount)
		);

		//check if the entire demand is within the control block
		assert(is_within_bounds(requiredArea) && "demanded size too large for this texture");

		for (uint16_t i = 0; i < fCount; ++i)
			mFrames.emplace_back(start.x + (i * fWidth), start.y);
		set_source_pos(mFrames.front());
		set_source_size(vec2f(fWidth, fHeight));

		set_dest_size(vec2f(fWidth, fHeight));//default inital
	}
	//#########################################################################################

	Font::Font(Texture* texture, uint32_t columnsCount, uint32_t rowsCount)
		:Sprite(texture),
		mColumnsCount(columnsCount),
		mRowsCount(rowsCount)
	{
		rectF textureBounds = get_bounds();

		mGlyphWidth = static_cast<unsigned int>(textureBounds.w / columnsCount);
		mGlyphHeight = static_cast<unsigned int>(textureBounds.h / rowsCount);
		//becasue int vs float
		assert(mGlyphWidth * columnsCount == textureBounds.w && "texture image likely off size or invalid counts");
		assert(mGlyphHeight * rowsCount == textureBounds.h && "texture image likely off size or invalid counts");
		//set_source_size(vec2f(GylphWidth, GylphHeight));
		//set_dest_size(vec2f(GylphWidth, GylphHeight));
	}
	
	void Font::draw(SDL_Renderer* renderer, const vec2f& pos) {
		for (const auto& letter : mLetterPos) {
			set_source_pos(letter.mSourcePos);
			set_dest_pos(letter.mDestPos);
			Sprite::draw(renderer);
		}
	}

	void Font::set_text(std::string_view string, const vec2f& pos)noexcept {
		clear_text();
		const float scaledW = mGlyphWidth * mScale;
		const float scaledH = mGlyphHeight * mScale;
		vec2f destP = pos;

		for (char c : string) {

			if (c == '\n') {
				//if new line start in the same position on x axis but below offset by 1 amount of height
				destP.x = pos.x;
				destP.y += scaledH;
				continue;
			}
			//spacebar
			if (c == first_ASCII_character) {
				destP.x += scaledW;
				continue;
			}

			// if char is the empty space key (c == first_ASCII_character), then skip this part as in anycase
			// position is incremented for the next iteration in the loop
			if (c >= first_ASCII_character + 1 && c <= last_ASCII_character) {
				const int gylphIndex = c - first_ASCII_character;
				const int glyphY = gylphIndex / mColumnsCount;//ASCII math
				const int glyphX = gylphIndex % mColumnsCount;//ASCII math

				mLetterPos.emplace_back(
					rectF(
						glyphX * mGlyphWidth, //source x
						glyphY * mGlyphHeight,//source y
						mGlyphWidth,          //source w
						mGlyphHeight          //source h
					),
					rectF(
						destP.x, //dest x
						destP.y, //dest y
						scaledW, //dest w
						scaledH  //dest h
					)
				);
				//move cursor to next 
				destP.x += scaledW;
			}
		}
	}
	void Font::clear_text()noexcept {
		mLetterPos.clear();
	}
}