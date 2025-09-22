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

		Animation(Sprite& sprite, const vec2i& readBegin, uint16_t frameWidth, uint16_t frameHeight, uint16_t frameCount, float holdTime = 0.16f)
			:mSprite(sprite), mFrameWidth(frameWidth), mFrameHeight(frameHeight), mFrameCount(frameCount), mHoldTime(holdTime) {

			const std::size_t neededWidth = readBegin.x + frameCount * frameWidth;
			const std::size_t neededHeight = readBegin.y + frameHeight;

			if (neededWidth > sprite.data()->w || neededHeight > sprite.data()->h) {
				throw std::runtime_error("Mismatch between widths or heights");
			}

			for (uint16_t i = 0; i < frameCount; i++) {
				mFrames.element_create(readBegin.x + (i * frameWidth), readBegin.y);
			}
		}

		void update(float dt)noexcept {
			mCurrentFrameTime += dt;
			while (mCurrentFrameTime >= mHoldTime) {
				advance();
				mCurrentFrameTime -= mHoldTime;
			}
		}
		void draw(SDL_Renderer& renderer, const vec2i& destPos, vec2i* destScale = nullptr) {
			vec2i sourcePosition = mFrames[mCurrentFrame];
			SDL_FRect source = { sourcePosition.x ,sourcePosition.y, mFrameWidth,mFrameHeight };
			vec2i destinationScale = (destScale == nullptr) ? vec2i(mFrameWidth, mFrameHeight) : *destScale;
			SDL_FRect destination = { destPos.x, destPos.y, destinationScale.x, destinationScale.y };

			SDL_RenderTexture(&renderer, mSprite.data(), &source, &destination);
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