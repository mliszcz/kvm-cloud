/*
 * Exception.hpp
 *
 *  Created on: Jun 14, 2013
 *      Author: michal
 */

#ifndef EXCEPTION_HPP_
#define EXCEPTION_HPP_

#include <stdexcept>
#include <cstdio>
#include <cerrno>

namespace Util
{

class Exception: public std::runtime_error
{
public:

	Exception(const std::string& message) :
			std::runtime_error(message.c_str())
	{
		if (errno != 0)
			::perror("perror");
	}

	Exception(const std::string& where, const std::string& what) :
		Exception("in " + where + ": " + what)
	{
	}

};

}

#endif /* EXCEPTION_HPP_ */
