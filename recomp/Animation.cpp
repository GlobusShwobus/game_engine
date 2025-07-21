#include "Animation.h"
#include <assert.h>

namespace badEngine {
	Animation::Animation(const Vec2i& pos, const Vec2i& imageSize, int frameWidth, int frameHeight, int count, float holdTime)
		:holdTime(holdTime)
	{
		assert((imageSize.x / frameWidth) <= count);
		assert(imageSize.y >= frameHeight);

		for (int i = 0; i < count; i++) {
			frames.emplace_back(Vec2i( pos.x + (frameWidth * i), pos.y ));
		}
	}
	void Animation::advance() {
		++curFrame;
		if (curFrame >= frames.size()) {
			curFrame = 0;
		}
	}
	void Animation::update(float dt) {
		curFrameTime += dt;
		while (curFrameTime >= holdTime) {
			advance();
			curFrameTime -= holdTime;
		}
	}
	Vec2i Animation::getFrame() {
		return frames[curFrame];
	}
}
