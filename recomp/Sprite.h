#pragma once

#include "SequenceM.h"
#include "Rectangle.h"
#include <assert.h>
#include "Texture.h"

namespace badEngine {
	
	class Sprite {
	
	protected:

		Sprite(const Texture& texture) :mTexture(texture) {
			float w, h;
			SDL_GetTextureSize(mTexture.get(), &w, &h);
			mSource = rectF(0, 0, w, h);
			mDest = rectF(0, 0, w, h);
		}

	public:

		virtual ~Sprite() = default;
		
		const rectF& get_source()const noexcept {
			return mSource;
		}
		const rectF& get_dest()const noexcept {
			return mDest;
		}
		SDL_Texture* const get_texture()const noexcept {
			return mTexture.get();
		}
		bool isNullptr()const noexcept {
			return mTexture.isNullPtr();
		}

	protected:
		const Texture& mTexture;
		rectF mSource;
		rectF mDest;
	};

	class Animation :public Sprite {

	public:

		Animation(const Texture& texture, uint16_t frameWidth, uint16_t frameHeight, uint16_t* nColumns = nullptr, uint16_t* nRows = nullptr);

		void update(float dt, vec2f* pos = nullptr)noexcept;
		void set_frame_hold_time(float time)noexcept;
		uint16_t get_lines_count()const noexcept {
			return mRows;
		}
		void set_line(uint16_t line)noexcept {
			assert(line < mRows && "input line can not exceed the count of rows");
			mCurrentRow = line;
		}

	private:
		SequenceM<vec2i> mFrames;

		float mHoldTime = 0.08f;
		float mCurrentFrameTime = 0.0f;

		uint16_t mColumns = 0;
		uint16_t mRows = 0;

		uint16_t mCurrentRow = 0;
		uint16_t mCurrentFrame = 0;
	};

	class Font : public Sprite {

		static constexpr char first_ASCII_character = ' ';
		static constexpr char last_ASCII_character = '~';

	public:

		Font(const Texture& texture, uint32_t columnsCount, uint32_t rowsCount);
		
		void set_text(std::string_view string, const vec2f& pos)noexcept;
		void clear_text()noexcept;
		void set_scale(float scale)noexcept;

		const SequenceM<std::pair<rectF, rectF>>& get_letter_positions()const {
			return mLetterPos;
		}
	private:
		SequenceM<std::pair<rectF, rectF>> mLetterPos;

		uint32_t mColumnsCount = 0;
		uint32_t mRowsCount = 0;
		uint32_t mGlyphWidth = 0;
		uint32_t mGlyphHeight = 0;
		float mScale = 1.0f;
	};

}
