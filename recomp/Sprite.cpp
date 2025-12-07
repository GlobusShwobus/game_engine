#include "Sprite.h"

namespace badEngine {


	//####################################################################################

	Animation::Animation(const Texture& texture, const vec2i& start, uint16_t fWidth, uint16_t fHeight, uint16_t fCount)
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
		assert(mTexture.get_control_block().contains(requiredArea) && "demanded size too large for this texture");

		for (uint16_t i = 0; i < fCount; ++i)
			mFrames.emplace_back(start.x + (i * fWidth), start.y);
		
		mSource.set_pos(mFrames.front());
		vec2f size(vec2f(static_cast<float>(fWidth), static_cast<float>(fHeight)));
		mSource.set_size(size);//size of the frame
		mDest.set_size(size);//default initial draw size
	}
	void Animation::update(float dt, vec2f* pos = nullptr)noexcept {
		//add to the time counter
		mCurrentFrameTime += dt;
		//while if counter is more than hold time
		while (mCurrentFrameTime >= mHoldTime) {
			++mCurrentFrame;					  //next frame
			if (mCurrentFrame >= mFrameCount)	  //if frame reached the end
				mCurrentFrame = 0;				  //reset

			mCurrentFrameTime -= mHoldTime;       //subtract 1 update cycle worth of time
		}

		mSource.set_pos(mFrames[mCurrentFrame]);  //set source position

		if (pos)                                  //if pos set dest position
			mDest.set_pos(*pos);
	}
	void Animation::set_frame_hold_time(float time)noexcept {
		assert(time >= 0 && "negative time");
		mHoldTime = time;
	}
	//#########################################################################################

	Font::Font(const Texture& texture, uint32_t columnsCount, uint32_t rowsCount)
		:Sprite(texture),
		mColumnsCount(columnsCount),
		mRowsCount(rowsCount)
	{
		rectF textureBounds = mTexture.get_control_block();

		mGlyphWidth = static_cast<unsigned int>(textureBounds.w / columnsCount);
		mGlyphHeight = static_cast<unsigned int>(textureBounds.h / rowsCount);
		//becasue int vs float
		assert(mGlyphWidth * columnsCount == textureBounds.w && "texture image likely off size or invalid counts");
		assert(mGlyphHeight * rowsCount == textureBounds.h && "texture image likely off size or invalid counts");
		//no initial default sizes nor pos is set, set_text does that
	}
	void Font::set_text(std::string_view string, const vec2f& pos)noexcept {
		clear_text();                                    //clear text, memory buffer stays
		const float scaledW = mGlyphWidth * mScale;      //used for dest, not source
		const float scaledH = mGlyphHeight * mScale;     //used for dest, not source
		vec2f destP = pos;                               //modifiable pos

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
					rectF(
						static_cast<float>(glyphX * mGlyphWidth), //source x
						static_cast<float>(glyphY * mGlyphHeight),//source y
						static_cast<float>(mGlyphWidth),          //source w
						static_cast<float>(mGlyphHeight)          //source h
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
	void Font::set_scale(float scale)noexcept {
		assert(scale > 0.0f && "scale can not be zero or negative");
		mScale = scale;
	}
}