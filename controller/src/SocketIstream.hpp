/*
 * SocketIstream.hpp
 *
 *  Created on: May 19, 2013
 *      Author: michal
 */

#ifndef SOCKETISTREAM_HPP_
#define SOCKETISTREAM_HPP_

/*
 * http://artofcode.wordpress.com/2010/12/12/deriving-from-stdstreambuf/
 */

#include <iostream>
#include <streambuf>
#include <string>
#include <unistd.h>

namespace Net
{

class SocketStreambuf: public std::streambuf
{
	friend class SocketIstream;

private:

	int streamFD;

	char currChar;
	char nextChar;

	bool isEOS;

	SocketStreambuf(int unixFileDescriptor)
	{
		streamFD = unixFileDescriptor;
		isEOS = false;

		nextChar = peek();
		next();
	}

	char peek()
	{
		char c;

		// line separator: \r\n, EOS = \r\n\r\n

		// discard all \r
		do { ::read(streamFD, &c, 1); }
		while(c == '\r');

		// now: \n\n = EOS

		return c;
	}

	void next()
	{
		currChar = nextChar;
		nextChar = peek();

		if(currChar == '\n' && nextChar == '\n') isEOS = true;
	}

	virtual int_type underflow()
	{
		if (isEOS) return traits_type::eof();
		return traits_type::to_int_type(currChar);
	}

	virtual int_type uflow()
	{
		if (isEOS) return traits_type::eof();

		char tmp = currChar;
		next();

		if (isEOS) return traits_type::eof();
		return traits_type::to_int_type(tmp);
	}

	virtual int_type pbackfail(int_type)
	{
		return traits_type::eof();
	}

	virtual std::streamsize showmanyc()
	{
		return 0;
	}
};

class SocketIstream: public std::istream
{

private:

	SocketStreambuf sb;

public:

	SocketIstream(int unixFileDescriptor) :
	std::istream(&sb), sb(unixFileDescriptor) { }
};

}

#endif /* SOCKETISTREAM_HPP_ */
