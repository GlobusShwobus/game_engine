#pragma once

#include "SequenceM.h"
#include "Rectangle.h"
#include <assert.h>
#include "Texture.h"

namespace badEngine {
	
	class Sprite {
	
	protected:

		Sprite(Texture* texture) :mTexture(texture) {
			assert(mTexture != nullptr && "texture can not be init with nullptr");
			float w, h;
			SDL_GetTextureSize(mTexture->get(), &w, &h);
			mSource = rectF(0, 0, w, h);
			mDest = rectF(0, 0, w, h);
		}

		//source managment should be restricted to inheritence because the assumption is
		//designing future derived classes the user knows to check bounds using is_within_bounds themselves at setup

		inline void set_source_pos(const vec2f& pos)noexcept {
			mSource.set_pos(pos);
		}
		inline void set_source_size(const vec2f& size)noexcept {
			mSource.set_size(size);
		}

	public:

		virtual ~Sprite() = default;
		
		bool is_within_bounds(const rectF& rect)const noexcept {
			return mTexture->get_control_block().contains(rect);
		}

		inline const rectF& get_source()const noexcept {
			return mSource;
		}
		inline const rectF& get_dest()const noexcept {
			return mDest;
		}
		inline const rectF get_bounds()const noexcept {
			return mTexture->get_control_block();
		}

		inline void set_dest_pos(const vec2f& pos)noexcept {
			mDest.set_pos(pos);
		}		
		inline void set_dest_size(const vec2f& size)noexcept {
			mDest.set_size(size);
		}
	private:

		Texture* mTexture = nullptr;
		rectF mSource;
		rectF mDest;
	};

	class Animation :public Sprite {

	public:

		Animation(Texture* texture, const vec2i& start, uint16_t fWidth, uint16_t fHeight, uint16_t fCount);

		void update(float dt, vec2f* pos = nullptr)noexcept {
			//add to the time counter
			mCurrentFrameTime += dt;
			//while if counter is more than hold time
			while (mCurrentFrameTime >= mHoldTime) {
				++mCurrentFrame;					  //next frame
				if (mCurrentFrame >= mFrameCount)	  //if frame reached the end
					mCurrentFrame = 0;				  //reset
				
				mCurrentFrameTime -= mHoldTime;       //subtract 1 update cycle worth of time
			}

			set_source_pos(mFrames[mCurrentFrame]);   //set source position

			if (pos)                                  //if pos set dest position
				set_dest_pos(*pos);
		}
		inline void set_frame_hold_time(float time)noexcept {
			assert(time >= 0 && "negative time");
			mHoldTime = time;
		}

	private:
		SequenceM<vec2i> mFrames;

		uint16_t mFrameCount = 0;
		uint16_t mCurrentFrame = 0;

		float mHoldTime = 0.08f;
		float mCurrentFrameTime = 0.0f;
	};

	class Font : public Sprite {

		static constexpr char first_ASCII_character = ' ';
		static constexpr char last_ASCII_character = '~';

	public:

		Font(Texture* texture, uint32_t columnsCount, uint32_t rowsCount);
		void draw(SDL_Renderer* renderer, const vec2f& pos);
		
		void set_text(std::string_view string, const vec2f& pos)noexcept;
		void clear_text()noexcept;
		void set_scale(float scale)noexcept {
			assert(scale > 0.0f && "scale can not be zero or negative");
			mScale = scale;
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
