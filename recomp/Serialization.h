#pragma once

#include <iostream>
#include <fstream>

namespace badEngine {

	struct testObj {
		int field1;
		short field2;
		bool field3;

		testObj(int f1, short f2, bool f3) :field1(f1), field2(f2), field3(f3) {}


		void serialize(std::ostream& os)const {
			os.write(reinterpret_cast<const char*>(&field1), sizeof(field1));
			os.write(reinterpret_cast<const char*>(&field2), sizeof(field2));
			os.write(reinterpret_cast<const char*>(&field3), sizeof(field3));
		}
	};
}