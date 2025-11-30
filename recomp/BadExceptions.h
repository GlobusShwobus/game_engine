#pragma once
#include <stdexcept>
#include <cassert>
#include <string>

namespace badEngine {

#define BAD_BASIC_EXCEPTION(note) BadException(__FILE__, __LINE__, note)

	class BadException : public std::exception {
	
	public:

		BadException(const char* file, unsigned int line, const std::string& note = "")
			:mNote(note), mFile(file), mLine(line)
		{
			build_msg();
		}

		const std::string& get_note()const noexcept{
			return mNote;
		}
		const std::string& get_file()const noexcept{
			return mFile;
		}
		unsigned int get_line()const noexcept{
			return mLine;
		}


		std::string get_location()const noexcept {
			return std::string("Line [") + std::to_string(mLine) + "] in " + mFile;
		}

		const char* what() const noexcept override {
			return mMsg.c_str();
		}

		virtual void build_msg() {
			mMsg = "Exception at " + get_location();
			if (!mNote.empty())
				mMsg += " : " + mNote;
		}

		virtual ~BadException() = default;
	protected:
		std::string mNote;
		std::string mFile;
		std::string mMsg;
		unsigned int mLine = 0;
	};


}