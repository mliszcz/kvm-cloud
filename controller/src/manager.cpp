
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "Thread/Thread.hpp"
#include "Thread/Mutex.hpp"
#include "Vm/Controller.hpp"
#include "Net/Socket.hpp"
#include "Net/ServerSocket.hpp"
#include "Util/Logger.hpp"

using std::string;
using std::vector;
using std::to_string;
using std::shared_ptr;
using std::make_shared;
using std::stoi;


shared_ptr<Net::ServerSocket> ssocket;
shared_ptr<Util::Logger> 	   logger;


int main(int argc, char** argv) {

	logger = make_shared<Util::Logger>(std::cout);

	if (argc < 3) {
		logger->log("usage: manager <control-port> <initial-id> <initial-ssh-port>");
		return -1;
	}

	return 0;
}
