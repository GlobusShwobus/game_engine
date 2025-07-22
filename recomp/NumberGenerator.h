#pragma once

#include <random>
#include <memory>

namespace badEngine {

	class NumberGenerator {
		std::unique_ptr<std::mt19937> rng;

	public:
		NumberGenerator() 
			:rng(std::make_unique<std::mt19937>(std::random_device{}())) 
		{}

		template <typename T>
		T getRand(T min, T max)const {
			return std::uniform_int_distribution<T>(min, max)(*rng);
		}
	};

}