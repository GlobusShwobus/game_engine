#pragma once

#include "json.hpp"
#include <memory>
#include <string_view>
#include <fstream>

namespace badEngine {

	class JSONWrapper {
	public:
		JSONWrapper() = default;
		JSONWrapper(std::string_view path);

		void init(std::string_view path);

		const nlohmann::json& get()const;

	private:
		std::unique_ptr<nlohmann::json> data;
	};

}