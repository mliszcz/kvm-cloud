
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
#include "NetException.hpp"


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

		sockFD = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (sockFD == -1)
			throw NetException("Socket::Socket(string, int)", "socket() failed");

		struct sockaddr_in serv_addr;
		bzero((char*)&serv_addr, sizeof(serv_addr));
		serv_addr.sin_family  = AF_INET;
		serv_addr.sin_addr.s_addr  = inet_addr(address.c_str());
		serv_addr.sin_port  = htons(port);

		if (-1 == ::connect(sockFD, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) )
			throw NetException("Socket::Socket(string, int)", "connect() failed");
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

	Socket& write(const std::vector<std::string>& data)
	{
		for (auto& d : data) write(d);
		return *this;
	}

	Socket& write(const std::string& d)
	{
		::write(sockFD, (d+"\r\n").c_str(), d.size()+2);
		return *this;
	}

	void send()
	{
		write("");
	}
};

}

#endif /* SOCKET_HPP_ */
