/*
 * NetException.hpp
 *
 *  Created on: May 16, 2013
 *      Author: michal
 */

#ifndef NETEXCEPTION_HPP_
#define NETEXCEPTION_HPP_

#include <string>

#include "../Util/Exception.hpp"

namespace Net
{

class NetException : public Util::Exception
{
public:

	NetException(const std::string& message) :
		Util::Exception(message) { }

	NetException(const std::string& where, const std::string& what) :
		Util::Exception(where, what) { }
};

}

#endif /* NETEXCEPTION_HPP_ */
