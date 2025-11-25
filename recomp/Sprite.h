#pragma once

#include "SequenceM.h"
#include "Rectangle.h"
#include <assert.h>
#include "Texture.h"

namespace badEngine {
	
	class Sprite {
	
	protected:

		Sprite(Texture* texture);

		void draw(SDL_Renderer* renderer)noexcept;
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
		
		bool is_within_bounds(const rectF& rect)const noexcept;

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

		void draw(SDL_Renderer* renderer, const vec2f& pos)noexcept;

		void progress(float dt)noexcept;
		inline void set_frame_hold_time(float time)noexcept {
			assert(time >= 0 && "negative time");
			mHoldTime = time;
		}

	private:

		inline void next_frame()noexcept {
			++mCurrentFrame;
			if (mCurrentFrame >= mFrameCount)
				mCurrentFrame = 0;
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

		struct LetterPositions {
			vec2f mSourcePos;
			vec2f mDestPos;
		};

	public:

		Font(Texture* texture, uint32_t columnsCount, uint32_t rowsCount);
		void draw(SDL_Renderer* renderer, const vec2f& pos);
		
		void set_text(std::string_view string)noexcept;
		void clear_text()noexcept;

	private:
		SequenceM<LetterPositions> mLetterPos;

		const uint32_t mColumnsCount = 0;
		const uint32_t mRowsCount = 0;
	};

}
