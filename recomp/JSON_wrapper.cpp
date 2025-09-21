#include "JSON_wrapper.h"

namespace badEngine {

	JSONWrapper::JSONWrapper(std::string_view path) {
		init(path);
	}

	void JSONWrapper::init(std::string_view path) {
		if (data) {
			data.release();
		}

		std::ifstream in(path.data());
		if (in.good()) {
			data = std::make_unique<nlohmann::json>();
			in >> *data;
		}
		in.close();
	}

	const nlohmann::json& JSONWrapper::get()const {
		return *data;
	}
}