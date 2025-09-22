#pragma once

#include <chrono>

namespace badEngine {

	class Stopwatch {

		using Floatsec = std::chrono::duration<float>;
		using Millisec = std::chrono::milliseconds;
		using Microsec = std::chrono::microseconds;

		template<typename DurationType = Millisec>
		DurationType delta_time()noexcept {
			const auto old = mWatch;
			mWatch = std::chrono::steady_clock::now();
			return std::chrono::duration_cast<DurationType>(mWatch - old);
		}

	public:
		Stopwatch()noexcept {
			mWatch = std::chrono::steady_clock::now();
		}

		float dt_float()noexcept {
			return delta_time<Floatsec>().count();
		}
		std::size_t dt_millisec()noexcept {
			return delta_time<Millisec>().count();
		}
		std::size_t dt_microsec()noexcept {
			return delta_time<Microsec>().count();
		}

		void reset()noexcept {
			mWatch = std::chrono::steady_clock::now();
		}

	private:
		std::chrono::steady_clock::time_point mWatch;
	};
}