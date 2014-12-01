/*
 * ServerSocket.hpp
 *
 *  Created on: May 16, 2013
 *      Author: michal
 */

#ifndef SERVERSOCKET_HPP_
#define SERVERSOCKET_HPP_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstring>

#include "NetException.hpp"
#include "Socket.hpp"

namespace Net
{

class ServerSocket
{
private:

	sockaddr_in sockAddr;
	int sockFD;

public:

	ServerSocket(int port)
	{
		sockFD = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (sockFD == -1)
			throw NetException("ServerSocket::ServerSocket()", "socket() failed");

		int optReuseAddress = 1;
		int setsockoptResult = ::setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR,
				&optReuseAddress, sizeof(int));
		if (setsockoptResult == -1)
			throw NetException("ServerSocket::ServerSocket()", "setsockopt() failed");

		memset(&sockAddr, 0, sizeof(sockAddr));
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(port);
		sockAddr.sin_addr.s_addr = ::htonl(INADDR_ANY );

		int bindResult = ::bind(sockFD, (const sockaddr*) &sockAddr,
				sizeof(sockAddr));

		if (bindResult == -1)
			throw NetException("ServerSocket::ServerSocket()", "bind() failed");

		// max 10 pending
		int listenResult = ::listen(sockFD, 10);

		if (listenResult == -1)
			throw NetException("ServerSocket::ServerSocket()", "listen() failed");
	}

	void close()
	{
		::close(sockFD);
	}

	virtual ~ServerSocket()
	{
		close();
	}

	std::shared_ptr<Socket> accept()
	{
		int connFD = ::accept(sockFD, nullptr, nullptr);

		if (connFD < 0)
			throw NetException("ServerSocket::accept()", "accept() failed");

		return std::shared_ptr<Socket>(new Socket(connFD));
	}
};

}

#endif /* SERVERSOCKET_HPP_ */
