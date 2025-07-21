#pragma once

#include <vector>
#include "Rectangle.h"
#include "Vek2.h"

namespace badEngine {

	class Animation {
		std::vector<Vek2> frames;
		int curFrame = 0;
		float holdTime;
		float curFrameTime = 0.0f;
	private:
		void advance();
	public:
		Animation(const Vek2& pos, const Vek2& imageSize, int frameWidth, int frameHeight, int count, float holdTime = 0.16f);
		void update(float dt);
		Vek2 getFrame();
	};
}