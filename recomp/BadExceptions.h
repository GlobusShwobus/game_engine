#pragma once
#include <stdexcept>
#include <cassert>
#include <string>

namespace badEngine {

	class BadException : public std::exception
	{
	private:
		void build_location(const char* file, unsigned int line)noexcept {
			mLocation = "Line [" + std::to_string(line) + "] in " + file;
		}
	public:

		BadException(const char* file, unsigned int line, const char* type, const std::string& description)
			:mType(type), mDescription(description)
		{
			build_location(file, line);
		}

		const std::string& get_location()const noexcept {
			return mLocation;
		}
		const std::string& get_type()const noexcept {
			return mType;
		}
		const char* what() const noexcept override {
			return mDescription.c_str();
		}
		virtual std::string full_message()const noexcept {
			return "TYPE: " + mType + " at " + mLocation + " IS >> " + mDescription;
		}

		virtual ~BadException() = default;
	protected:
		std::string mDescription;
		std::string mType;
		std::string mLocation;
	};


}