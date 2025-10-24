#pragma once

#include "Sprite.h"
#include "Vec2M.h"
#include "SequenceM.h"

namespace badEngine {

	class Animation {

		inline void advance()noexcept {
			++mCurrentFrame;
			if (mCurrentFrame >= mFrameCount)
				mCurrentFrame = 0;
		}

	public:

		Animation(Sprite& sprite, const vec2i& readBegin, const vec2i& frameSize, uint16_t frameCount, float holdTime = 0.16f);

		void update(float dt)noexcept;
		void draw_animation(SDL_Renderer* renderer, const vec2i& destinationPosition);

	private:
		SequenceM<vec2i> mFrames;
		Sprite& mSprite;

		uint16_t mFrameCount = 0;
		uint16_t mCurrentFrame = 0;

		float mHoldTime = 0.0f;
		float mCurrentFrameTime = 0.0f;
	};

}