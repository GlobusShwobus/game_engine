#pragma once
#include <stdint.h>
namespace badEngine {
	class FrameTimer {
		static constexpr double ONE_SECOND = 1.0;
		static constexpr uint32_t DEFAULT_FPS = 60;

	public:
		FrameTimer(uint32_t fps = DEFAULT_FPS) :mFPS(fps), mFrameDurationLimit(ONE_SECOND / fps) {}

		constexpr bool is_within_limit(float deltaTime)const noexcept {
			return mFrameDurationLimit > deltaTime;
		}
		constexpr double get_limit()const noexcept {
			return mFrameDurationLimit;
		}
		constexpr uint32_t get_fps()const noexcept {
			return mFPS;
		}
		constexpr void set_FPS(uint32_t fps)noexcept {
			mFPS = fps;
			mFrameDurationLimit = ONE_SECOND / mFPS;
		}

	private:
		double mFrameDurationLimit = 0;
		uint32_t mFPS = 0;
	};
}
