
#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <vector>
#include <string>
#include <list>
#include <algorithm>
#include <memory>
#include <cstdlib>

#include <sys/socket.h>

#include "SocketIstream.hpp"


namespace Net
{

// references
class HttpRequest;
//class HttpResponse;


class Socket
{
	friend class ServerSocket;

private:

	int sockFD;

	Socket(int sockFD)
	{
		this->sockFD = sockFD;
	}

public:

	void close()
	{
		int shutdownResult = ::shutdown(sockFD, SHUT_RDWR);

		//if (shutdownResult == -1)
			//throw NetException("Socket::close()", "shutdown() failed");
	}

	virtual ~Socket()
	{
		close();
	}

	std::vector<std::string> read()
	{
		std::vector<std::string> result;
		std::string line;
		SocketIstream socketStream(sockFD);
		while(std::getline(socketStream, line))
			result.push_back(line);

		return result;
	}

	void write(std::vector<std::string> data)
	{
		for (auto& d : data)
			::write(sockFD, (d+"\r\n").c_str(), d.size()+2);
	}
};

}

#endif /* SOCKET_HPP_ */
