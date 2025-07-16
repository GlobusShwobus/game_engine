#include "Stopwatch.h"
namespace badEngine {

	std::chrono::duration<float> Stopwatch::Mark() {
		const auto old = last;
		last = std::chrono::steady_clock::now();
		const std::chrono::duration<float> frametime = last - old;
		return frametime;
	}
	Stopwatch::Stopwatch() {
		last = std::chrono::steady_clock::now();
	}

	float Stopwatch::MarkFloat() {
		auto time = Mark();
		return time.count();
	}
	std::chrono::milliseconds Stopwatch::MarkMilliSec() {
		auto time = Mark();
		return std::chrono::duration_cast<std::chrono::milliseconds>(time);
	}
	std::chrono::microseconds Stopwatch::MarkMicroSec() {
		auto time = Mark();
		return std::chrono::duration_cast<std::chrono::microseconds>(time);
	}

	uint32_t FrameTimer::getFPS()const
	{
		return fps;
	}
	void FrameTimer::setFPS(uint32_t FPS)
	{
		fps = FPS;
		limit = std::chrono::duration<float>(ONE_SECOND / fps);
	}
	const float FrameTimer::getLimit()const {
		return limit.count();
	}

}