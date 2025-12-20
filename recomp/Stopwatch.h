#pragma once

#include <chrono>

namespace badEngine {

	class Stopwatch {

		using Valuesec = std::chrono::duration<double>;
		using Millisec = std::chrono::milliseconds;
		using Microsec = std::chrono::microseconds;
		using Nanosec  = std::chrono::nanoseconds;

		template<typename DurationType = Millisec>
		DurationType delta_time()noexcept {
			const auto old = mWatch;
			mWatch = std::chrono::steady_clock::now();
			return std::chrono::duration_cast<DurationType>(mWatch - old);
		}

	public:
		Stopwatch()noexcept 
		:mWatch(std::chrono::steady_clock::now())
		{
		}

		double dt_float()noexcept {
			return delta_time<Valuesec>().count();
		}
		std::size_t dt_millisec()noexcept {
			return delta_time<Millisec>().count();
		}
		std::size_t dt_microsec()noexcept {
			return delta_time<Microsec>().count();
		}
		std::size_t dt_nanosec()noexcept {
			return delta_time<Nanosec>().count();
		}

		void reset()noexcept {
			mWatch = std::chrono::steady_clock::now();
		}

	private:
		std::chrono::steady_clock::time_point mWatch;
	};


	/*
	* DEPRICATED, FUNCTION IS NOT COMPLETE
	auto timeMyFunction = [](auto&& func, auto&&... args)->float {
		Stopwatch timer;

		std::forward<decltype(func)>(func)(
			std::forward<decltype(args)>(args)...
			);
		return timer.dt_float();
		};
		*/
}