#include "Sprite.h"

namespace badEngine {

	//####################################################################################

	Animation::Animation(const StaticTexture& texture, uint16_t frameWidth, uint16_t frameHeight, uint16_t* nColumns, uint16_t* nRows)
		:Sprite(texture)
	{
		float textureW, textureH;
		SDL_GetTextureSize(mTexture, &textureW, &textureH);
		//set values for iteration, internally frames are stored as 2D array
		uint16_t columnCount = (nColumns!=nullptr) ? *nColumns : static_cast<uint16_t>(textureW) / frameWidth;
		uint16_t rowCount = (nRows!=nullptr) ? *nRows : static_cast<uint16_t>(textureH) / frameHeight;

		const int2 requiredSize(
			columnCount * frameWidth,
			rowCount * frameHeight
		);

		//check if the entire demand is within the control block
		assert(textureW >= requiredSize.x && textureH >= requiredSize.y && "demanded size too large for this texture");

		//setup a classical 2D array
		for (uint16_t row = 0; row < rowCount; ++row) {
			for (uint16_t col = 0; col < columnCount; ++col) {
				mFrames.emplace_back(col * frameWidth, row * frameHeight);
			}
		}
			
		mColumnsN = columnCount;
		mRowsN = rowCount;

		const auto& firstFrame = mFrames.front();
		mSource.x = firstFrame.x;
		mSource.y = firstFrame.y;
		mSource.w = static_cast<float>(frameWidth);
		mSource.h = static_cast<float>(frameHeight);

		//default (set scale will mod it tho)
		mDest.w = static_cast<float>(frameWidth);
		mDest.h = static_cast<float>(frameHeight);
	}
	void Animation::anim_update(float dt, float2* pos)noexcept {
		//add to the time counter
		mCurrentFrameTime += dt;

		if (mCurrentFrameTime >= mHoldTime) {

			//while if counter is more than hold time
			while (mCurrentFrameTime >= mHoldTime) {
				++mCurrentColumn;					   //next frame
				if (mCurrentColumn >= mColumnsN)        //if frame reached the end
					mCurrentColumn = 0;				   //reset

				mCurrentFrameTime -= mHoldTime;        //subtract 1 update cycle worth of time
			}

			//set source position [y*width+x]
			const auto& frame = mFrames[mCurrentRow * mColumnsN + mCurrentColumn];
			mSource.x = frame.x;
			mSource.y = frame.y;
		}

		//if pos set dest position
		if (pos) {
			mDest.x = pos->x;
			mDest.y = pos->y;
		}
	}
	void Animation::anim_set_hold_time(float time)noexcept {
		assert(time >= 0 && "negative time");
		mHoldTime = time;
	}
	uint16_t Animation::anim_get_lines_count()const noexcept {
		return mRowsN;
	}
	void Animation::anim_set_line(uint16_t line)noexcept {
		assert(line < mRowsN && "input line can not exceed the count of rows");
		mCurrentRow = line;
	}
	float Animation::anim_get_scale()const noexcept {
		return mScale;
	}
	void Animation::anim_set_scale(float scale)noexcept {
		assert(scale > 0.0f && "scalar can not be 0 or less");
		mScale = scale;
		//use base size of texture not already scaled dest
		mDest.w = mSource.w * mScale;
		mDest.h = mSource.h * mScale;
	}
	//#########################################################################################

	Font::Font(const StaticTexture& texture, uint32_t columnsCount, uint32_t rowsCount)
		:Sprite(texture),
		mColumnsCount(columnsCount),
		mRowsCount(rowsCount)
	{
		float textureW, textureH;
		SDL_GetTextureSize(mTexture, &textureW, &textureH);

		mGlyphWidth = static_cast<unsigned int>(textureW / columnsCount);
		mGlyphHeight = static_cast<unsigned int>(textureH / rowsCount);
		//becasue int vs float
		assert(mGlyphWidth * columnsCount == textureW && "texture image likely off size or invalid counts");
		assert(mGlyphHeight * rowsCount == textureH && "texture image likely off size or invalid counts");
		//no initial default sizes nor pos is set, set_text does that
	}
	void Font::font_set_text(std::string_view string, const float2& pos)noexcept {
		font_clear_text();                                    //clear text, memory buffer stays
		const float scaledW = mGlyphWidth * mScale;      //used for dest, not source
		const float scaledH = mGlyphHeight * mScale;     //used for dest, not source
		float2 destP = pos;                               //modifiable pos

		for (char c : string){
			//if new line character then set cursor to the beginning which is pos.x and go down by height (scaled) then continue to next char
			if (c == '\n') {
				destP.x = pos.x;
				destP.y += scaledH;
				continue;
			}
			//if spacebar (' ') character, then move by width (scaled) then continue to next char
			if (c == first_ASCII_character) {
				destP.x += scaledW;
				continue;
			}
			//for any printable character
			if (c >= first_ASCII_character + 1 && c <= last_ASCII_character) {
				const uint32_t glyphIndex = c - first_ASCII_character;  //convert char to its index in the texture atlas (0 based)
				const uint32_t glyphY = glyphIndex / mColumnsCount;     //row index: for example char = 'a' and columns = 32. 97-32 = 65; 65/32 = 2 (rounded down because of float vs integer)
				const uint32_t glyphX = glyphIndex % mColumnsCount;     //column index: using above, 65%32 = (32+32 = 64, remainder is 1)
				                                                        //^ if font is set up not in order of ASCII then its all fucked
				mLetterPos.emplace_back(
					AABB(
						static_cast<float>(glyphX * mGlyphWidth), //source x
						static_cast<float>(glyphY * mGlyphHeight),//source y
						static_cast<float>(mGlyphWidth),          //source w
						static_cast<float>(mGlyphHeight)          //source h
					),
					AABB(
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
	void Font::font_clear_text()noexcept {
		mLetterPos.clear();
	}
	void Font::font_set_scale(float scale)noexcept {
		assert(scale > 0.0f && "scale can not be zero or negative");
		mScale = scale;
	}
}