#pragma once

#include <vector>
#include "Rectangle.h"
#include "Vek2.h"

namespace badEngine {

	class Animation {
		std::vector<Vec2i> frames;
		int curFrame = 0;
		float holdTime;
		float curFrameTime = 0.0f;
	private:
		void advance();
	public:
		Animation(const Vec2i& pos, const Vec2i& imageSize, int frameWidth, int frameHeight, int count, float holdTime = 0.16f);
		void update(float dt);
		Vec2i getFrame();
	};
}