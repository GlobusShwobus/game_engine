#pragma once

#include <random>
#include <memory>

namespace badEngine {

	class NumberGenerator {

		std::unique_ptr<std::mt19937> rng;

	public:
		NumberGenerator()
			:rng(std::make_unique<std::mt19937>(std::random_device{}())) {
		}


		int random_int(int min, int max)const noexcept {
			return std::uniform_int_distribution<int>(min, max)(*rng);
		}

		float random_float(float min, float max)const noexcept {
			return std::uniform_real_distribution<float>(min, max)(*rng);
		}

		/*
		//DEPRICATED (over templetization)
		//also requries #include badUtility
		template <typename T>
			requires IS_INTEGER_TYPE_T<T> || IS_FLOATING_TYPE_T<T>
		T get_random(T min, T max)const {

			if constexpr (IS_INTEGER_TYPE_T<T>) {
				return std::uniform_int_distribution<T>(min, max)(*rng);
			}
			else {
				return std::uniform_real_distribution<T>(min, max)(*rng);
			}
		}
		*/
	};

}