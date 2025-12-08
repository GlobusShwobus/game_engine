#pragma once

#include "SequenceM.h"
#include "Rectangle.h"
#include <assert.h>
#include "Texture.h"

namespace badEngine {
	
	class Sprite {
	
	protected:

		Sprite(const StaticTexture& texture) :mTexture(texture.get())
		{
			assert(mTexture != nullptr && "Texture is nullptr");
			float w, h;
			SDL_GetTextureSize(mTexture, &w, &h);
			mSource = rectF(0, 0, w, h);
			mDest = rectF(0, 0, w, h);
		}

		Sprite(const TargetTexture& texture) :mTexture(texture.get())
		{
			assert(mTexture != nullptr && "Texture is nullptr");
			float w, h;
			SDL_GetTextureSize(mTexture, &w, &h);
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
			return mTexture;
		}
		bool isNullptr()const noexcept {
			return mTexture;
		}

	protected:
		SDL_Texture* mTexture = nullptr;
		rectF mSource;
		rectF mDest;
	};

	class Animation :public Sprite {

	public:

		Animation(const StaticTexture& texture, uint16_t frameWidth, uint16_t frameHeight, uint16_t* nColumns = nullptr, uint16_t* nRows = nullptr);

		void anim_update(float dt, vec2f* pos = nullptr)noexcept;
		void anim_set_hold_time(float time)noexcept;
		uint16_t anim_get_lines_count()const noexcept;
		void anim_set_line(uint16_t line)noexcept;
		float anim_get_scale()const noexcept;
		void anim_set_scale(float scale)noexcept;

	private:
		SequenceM<vec2i> mFrames;

		float mHoldTime = 0.08f;
		float mCurrentFrameTime = 0.0f;
		float mScale = 1.0f;

		uint16_t mColumnsN = 0;
		uint16_t mRowsN = 0;

		uint16_t mCurrentColumn = 0;
		uint16_t mCurrentRow = 0;
	};


	class Font : public Sprite {

		static constexpr char first_ASCII_character = ' ';
		static constexpr char last_ASCII_character = '~';

	public:

		Font(const StaticTexture& texture, uint32_t columnsCount, uint32_t rowsCount);

		void font_set_text(std::string_view string, const vec2f& pos)noexcept;
		void font_clear_text()noexcept;
		void font_set_scale(float scale)noexcept;

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

	class Canvas :public Sprite {
	public:


		Canvas(const TargetTexture& texture, const GraphicsSys& gfx, Uint32 w, Uint32 h)
			:Sprite(texture)
		{

		}
	};

}
