
#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <chrono>
#include <ctime>
#include <iomanip>
#include <string>
#include <iostream>
#include <memory>

#include "../Thread/Mutex.hpp"
#include "../Thread/ScopedLock.hpp"
#include "Helpers.hpp"

namespace Util
{

class Logger {

public:
	enum class Level {
		DEBUG 	= 0,
		INFO 	= 1,
		WARN 	= 2,
		ERROR 	= 3,
		NONE 	= 4
	};

private:
	std::ostream& os;
	std::shared_ptr<Thread::Mutex> mutex = std::make_shared<Thread::Mutex>();
	Level loggerLevel;

	std::string timeString() {
		auto now = std::chrono::system_clock::now();
		auto in_time_t = std::chrono::system_clock::to_time_t(now);

		char buffer[32];
		std::strftime(buffer, sizeof(buffer), "%T", std::localtime(&in_time_t));
		return std::string(buffer);
	}

	std::string levelString(const Level& level) {
		switch (level) {
			case Level::DEBUG: 	return "DEBUG";
			case Level::INFO: 	return "INFO";
			case Level::WARN: 	return "WARN";
			case Level::ERROR: 	return "ERROR";
			case Level::NONE: 	return "NONE";	
			default: 			return "";
		}
	}

	void output(const Level& messageLevel, const std::string& message) {
		if (static_cast<int>(messageLevel) >= static_cast<int>(loggerLevel)) {
			Thread::ScopedLock lock(mutex);
			os << Helpers::printf("[%s] %s: %s",
				levelString(messageLevel).c_str(),
				timeString().c_str(),
				message.c_str()
			) << std::endl;
		}
	}

public:

	Logger(const Level& level, std::ostream& os)
		: loggerLevel(level), os(os) { }

	void debug(const std::string& message) { output(Level::DEBUG, message); }
	void  info(const std::string& message) { output(Level::INFO,  message); }
	void  warn(const std::string& message) { output(Level::WARN,  message); }
	void error(const std::string& message) { output(Level::ERROR, message); }
};

}

#endif /* LOGGER_HPP_ */
