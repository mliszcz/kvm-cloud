
#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <iostream>

#include "../Thread/Mutex.hpp"

namespace Util
{

class Logger
{
private:
	std::ostream& os;
	Thread::Mutex mutex;

public:

	Logger(std::ostream& os) :
		os(os) { }

	void log(const std::string& message)
	{
		mutex.lock();
		os << "logger: " << message << "\n";
		mutex.unlock();
	}
};

}

#endif /* LOGGER_HPP_ */
