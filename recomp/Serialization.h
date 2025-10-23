#pragma once

#include <iostream>
#include <fstream>
#include <type_traits>


#include <vector>

namespace badEngine {


	struct Test1 {
		int one;
		int two;
		int three, four;
		bool isbool = false;
		Test1() = default;
		Test1(int x) :one(x), two(x), three(x), four(x), isbool(true) {}
	};

	struct Test2 {
		int one;
		int two;
		int three, four;
		Test2() = default;
		Test2(int x) :one(x), two(x), three(x), four(x), isbool(false) {}
	private:
		bool isbool = true;
	};
	struct Test3 {
		std::vector<int>vec;
	};

	template<typename T>
	void POD_serialize(std::ofstream& out, const T& obj) {
		static_assert(std::is_trivially_copyable_v<T>, "must be trivially copyable");
		out.write(reinterpret_cast<const char*>(&obj), sizeof(T));
	}

	template<typename T>
	void POD_deserialize(std::ifstream& in, T& obj) {
		static_assert(std::is_trivially_copyable_v<T>, "must be trivially copyable");
		in.read(reinterpret_cast<char*>(&obj), sizeof(T));
	}

}