
#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <vector>
#include <string>
#include <list>
#include <algorithm>
#include <memory>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

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

	Socket(std::string address, int port) {
		struct sockaddr_in serv_addr;
		int fdSocket;

		// fdSocket = socket(AF_INET, SOCK_STREAM, 0);
		fdSocket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (!fdSocket)
		{
			perror("socket failed");
			throw -1;
		}

		bzero((char*)&serv_addr, sizeof(serv_addr));
		serv_addr.sin_family  = AF_INET;
		serv_addr.sin_addr.s_addr  = inet_addr(address.c_str());
		serv_addr.sin_port  = htons(port);

		if (-1 == connect(fdSocket, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) )
		{
			perror("connect failed");
			throw -1;
		}

		sockFD = fdSocket;
	}

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
