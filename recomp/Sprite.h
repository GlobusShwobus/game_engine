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
			mSource = float4(0, 0, w, h);
			mDest = float4(0, 0, w, h);
		}

		Sprite(const TargetTexture& texture) :mTexture(texture.get())
		{
			assert(mTexture != nullptr && "Texture is nullptr");
			float w, h;
			SDL_GetTextureSize(mTexture, &w, &h);
			mSource = float4(0, 0, w, h);
			mDest = float4(0, 0, w, h);
		}

	public:

		virtual ~Sprite() = default;
		
		const float4& get_source()const noexcept {
			return mSource;
		}
		const float4& get_dest()const noexcept {
			return mDest;
		}
		SDL_Texture* const get_texture()const noexcept {
			return mTexture;
		}

	protected:
		SDL_Texture* mTexture = nullptr;
		float4 mSource;
		float4 mDest;
	};

	class Animation :public Sprite {

	public:

		Animation(const StaticTexture& texture, uint16_t frameWidth, uint16_t frameHeight, uint16_t* nColumns = nullptr, uint16_t* nRows = nullptr);

		void anim_update(float dt, float2* pos = nullptr)noexcept;
		void anim_set_hold_time(float time)noexcept;
		uint16_t anim_get_lines_count()const noexcept;
		void anim_set_line(uint16_t line)noexcept;
		float anim_get_scale()const noexcept;
		void anim_set_scale(float scale)noexcept;

	private:
		SequenceM<float2> mFrames;

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

		void font_set_text(std::string_view string, const float2& pos)noexcept;
		void font_clear_text()noexcept;
		void font_set_scale(float scale)noexcept;

		const SequenceM<std::pair<float4, float4>>& get_letter_positions()const {
			return mLetterPos;
		}
	private:
		SequenceM<std::pair<float4, float4>> mLetterPos;

		uint32_t mColumnsCount = 0;
		uint32_t mRowsCount = 0;
		uint32_t mGlyphWidth = 0;
		uint32_t mGlyphHeight = 0;
		float mScale = 1.0f;
	};

	class Canvas :public Sprite {
	public:


		Canvas(const TargetTexture& texture)
			:Sprite(texture)
		{
		}
		//false == failure, call SDL_GetError
		bool start_drawing(const GraphicsSys& gfx)const noexcept{
			return gfx.set_render_target(mTexture);
		}
		//false == failure, call SDL_GetError
		bool end_drawing(const GraphicsSys& gfx)const noexcept {
			return gfx.set_render_target(nullptr);
		}
	};

}
