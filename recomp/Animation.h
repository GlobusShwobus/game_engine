#pragma once

#include "Sprite.h"
#include "Vec2M.h"
#include "SequenceM.h"

namespace badEngine {

	class Animation: public Sprite {

		void advance()noexcept {
			++mCurrentFrame;
			if (mCurrentFrame >= mFrameCount)
				mCurrentFrame = 0;
		}
	public:

		Animation(Sprite sprite, const vec2i& readBegin, const vec2i& frameSize, uint16_t frameCount, float holdTime)
			:Sprite(std::move(sprite))
		{
			this->set_source_size(frameSize);
			mFrameCount = frameCount;
			mHoldTime = holdTime;

			const std::size_t neededWidth = readBegin.x + frameCount * frameSize.x;
			const std::size_t neededHeight = readBegin.y + frameSize.y;


			if (neededWidth > this->texture_width() || neededHeight > this->texture_height()) {
				throw std::runtime_error("Mismatch between widths or heights");
			}

			for (uint16_t i = 0; i < frameCount; i++) {
				mFrames.element_create(readBegin.x + (i * frameSize.x), readBegin.y);
			}
		}

		void update(float dt)noexcept {
			mCurrentFrameTime += dt;
			while (mCurrentFrameTime >= mHoldTime) {
				advance();
				mCurrentFrameTime -= mHoldTime;
			}
		}
		void draw_animation(SDL_Renderer& renderer, const vec2i& destinationPosition) {
			this->set_source_position(mFrames[mCurrentFrame]);
			this->draw(renderer, destinationPosition);
		}

	private:
		SequenceM<vec2i> mFrames;

		uint16_t mFrameCount = 0;
		uint16_t mCurrentFrame = 0;

		float mHoldTime = 0.0f;
		float mCurrentFrameTime = 0.0f;
	};

}