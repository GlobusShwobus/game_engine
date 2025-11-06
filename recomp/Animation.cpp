#include "Animation.h"
#include <stdexcept>
namespace badEngine {

	Animation::Animation(Sprite& sprite, const vec2i& readBegin, const vec2i& frameSize, uint16_t frameCount, float holdTime)
		:mSprite(sprite)
	{
		if (!mSprite.set_destination_size(frameSize))
			throw std::runtime_error("Failure on setting destination size (frame size negative value?)");
		if (!mSprite.set_source_size(frameSize))
			throw std::runtime_error("Failure on setting source size (frame larger than image?)");

		mFrameCount = frameCount;
		mHoldTime = holdTime;

		const std::size_t neededWidth = std::size_t(readBegin.x + frameCount * frameSize.x);
		const std::size_t neededHeight = std::size_t(readBegin.y + frameSize.y);


		if (neededWidth > mSprite.get_texture_width() || neededHeight > mSprite.get_texture_height())
			throw std::runtime_error("Mismatch between widths or heights");

		for (uint16_t i = 0; i < frameCount; i++)
			mFrames.emplace_back(readBegin.x + (i * frameSize.x), readBegin.y);
	}

	void Animation::update(float dt)noexcept {
		mCurrentFrameTime += dt;
		while (mCurrentFrameTime >= mHoldTime) {
			advance();
			mCurrentFrameTime -= mHoldTime;
		}
	}
	void Animation::draw_animation(SDL_Renderer* renderer, const vec2i& destinationPosition) {
		//constructor assures these operations are valid
		mSprite.set_source_position(mFrames[mCurrentFrame]);
		mSprite.set_destination_position(destinationPosition);
		mSprite.draw(renderer);
	}
}