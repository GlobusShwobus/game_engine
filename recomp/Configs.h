#pragma once

#include "json.hpp"
#include <string_view>
#include "BadExceptions.h"
#include <fstream>

/*
TODO: can do something interesting with JSON custom parsing using parser_callback_t and parse_event_t combination, more on wiki
*/
namespace badEngine {

#define BAD_CONFIG_EXCEPTION(type, note, code) Configs::ConfigException(__FILE__, __LINE__, type, note, code)

	class Configs {
	public:
		class ConfigException :public BadException {
		public:
			ConfigException(const char* file, unsigned int line, const char* type, const std::string& description, int error_code)
				:BadException(file, line, type, description), error_code(error_code) {
			}
			std::string full_message()const noexcept override {
				return "TYPE: " + mType + " at " + mLocation + " IS >> " + mDescription + " CODE " + std::to_string(error_code);
			}
			int get_code() const noexcept {
				return error_code;
			}
		private:
			int error_code;
		};

	public:
		Configs() = default;
		Configs(std::string_view path) {
			init_from_file(path);
		}

		const nlohmann::json& get()const noexcept {
			return data;
		}

		//it can throw anything json::parse can throw like parse_error.101/102/103
		void init_from_file(std::string_view path)
		{
			assert(!path.empty() && "string_view can not be empty");
			assert(path.data() != nullptr && "string_view can not be nullptr");

			try {
				std::ifstream file;
				file.exceptions(std::ifstream::failbit | std::ifstream::badbit);//enables exceptions
				file.open(path.data());//if open fails, it sets failbit

				file.seekg(0, std::ios::end);
				if (file.tellg() == 0)
					throw BAD_CONFIG_EXCEPTION("File IO error", "Config file is empty [" + std::string(path)+"]", -1);
				file.seekg(0, std::ios::beg);
				data = nlohmann::json::parse(file, nullptr, true, false);//no custom parser, allow exceptions, don't ignore trailing commas
			}
			catch (const std::ios_base::failure& e) {
				throw BAD_CONFIG_EXCEPTION("File IO error", "[" + std::string(path) + "] " + std::string(e.what()), e.code().value());
			}
			catch (const nlohmann::json::parse_error& e) {
				throw BAD_CONFIG_EXCEPTION("JSON parse_error", "[" + std::string(path) + "] at byte{" + std::to_string(e.byte) + "} " + std::string(e.what()), e.id);
			}
			catch (const ConfigException& e) {
				throw;
			}
		}

	private:
		nlohmann::json data;
	};
}