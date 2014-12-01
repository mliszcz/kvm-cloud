#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "Thread/Thread.hpp"
#include "Thread/Mutex.hpp"
#include "Vm/Controller.hpp"
#include "Net/Socket.hpp"
#include "Net/ServerSocket.hpp"
#include "Util/Logger.hpp"

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <sstream>

using std::string;
using std::vector;
using std::to_string;
using std::shared_ptr;
using std::make_shared;


int main(int argc, char** argv) {

	shared_ptr<Net::Socket> socket;
	shared_ptr<Util::Logger> logger;

	logger = make_shared<Util::Logger>(std::cout);

	if (argc < 3) {
		logger->log("usage: client <controller-address> <controller-port>");
		return -1;
	}

	try {
		socket = make_shared<Net::Socket>(argv[1], std::stoi(argv[2]));
	} catch(...) {
		logger->log("cannot create socket");
		return -1;
	}

	string line;

	std::cout << "> ";
	while(std::getline(std::cin, line)) {

		if (line == "exit") break;

		std::istringstream iss(line);
		string token;
		vector<string> msg;

		while(iss >> token) {
			msg.push_back(token);
		}

		socket->write(msg);
		socket->write(vector<string>{""});

		auto res = socket->read();

		for (auto& r : res)
			std::cout << "- " << r << "\n";

		std::cout << "\n> ";
	}

	socket->close();

	return 0;
}
