#pragma once

#include <random>
#include <memory>
#include "BadExceptions.h"

namespace badEngine {

	class NumberGenerator {

		std::unique_ptr<std::mt19937> rng;

	public:
		NumberGenerator() {
			try {
				rng = std::make_unique<std::mt19937>(std::random_device{}());
			}
			catch (const std::runtime_error& e) {
				throw BadException(__FILE__, __LINE__, "Number Generator random_device error: " + std::string(e.what()));
			}
		}


		int random_int(int min, int max)const noexcept {
			assert(min <= max && "Invalid logic: min is more than max");
			return std::uniform_int_distribution<int>(min, max)(*rng);
		}

		float random_float(float min, float max)const noexcept {
			assert(min <= max && "Invalid logic: min is more than max");
			return std::uniform_real_distribution<float>(min, max)(*rng);
		}
	};

}