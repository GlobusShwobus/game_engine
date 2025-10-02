#pragma once

#include "json.hpp"
#include <string_view>
#include <fstream>

namespace badEngine {

	class Configs {
	public:
		Configs() = default;
		Configs(std::string_view path) {
			init_from_file(path);
		}

		const nlohmann::json& get()const {
			return data;
		}

		//it can throw anything json::parse can throw like parse_error.101/102/103
		void init_from_file(std::string_view path) {
			data = nlohmann::json::parse(std::ifstream(path.data()));
		}

	private:
		nlohmann::json data;
	};
}