#pragma once

#include "Sprite.h"
#include "Vec2M.h"
#include "SequenceM.h"

namespace badEngine {



	class Animation {
	
		void advance()noexcept {
			++mCurrentFrame;
			if (mCurrentFrame >= mFrameCount)
				mCurrentFrame = 0;
		}
	public:

		void update(float dt)noexcept {
			mCurrentFrameTime += dt;
			while (mCurrentFrameTime >= mHoldTime) {
				advance();
				mCurrentFrameTime -= mHoldTime;
			}
		}

		void draw(SDL_Renderer* renderer) {

		}

	private:
		SequenceM<vec2i> mFrames;
		Sprite& mSprite;
	
		uint16_t mFrameCount = 0;
		uint16_t mFrameWidth = 0;
		uint16_t mFrameHeight = 0;
		
		uint16_t mCurrentFrame = 0;
		float mHoldTime = 0.0f;
		float mCurrentFrameTime = 0.0f;
	};

}