
// #include <linux/kvm.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>

#include <sys/types.h>
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

extern "C" {
	#include "libkvm.h"
}

using std::string;
using std::vector;
using std::to_string;
using std::shared_ptr;
using std::make_shared;
using std::stoi;


Thread::Mutex ctrlMutex;
shared_ptr<Vm::Controller> controller;
shared_ptr<Net::ServerSocket> ssocket;
shared_ptr<Util::Logger> 	   logger;


class CommandHandler: public Thread::Thread<Thread::DetachState::Detached, Thread::CancelType::Deferred> {

private:
	std::shared_ptr<Net::Socket> socket;

public:
	CommandHandler(std::shared_ptr<Net::Socket> _socket)
		: socket(_socket) { }

	virtual void run() {

		for(;;) {

		auto commands = socket->read();
		auto cmd = commands.at(0);

		logger->log("processing request: " + cmd);

		if (cmd == "templates") {
			for (auto& kv : controller->getTemplates())
				socket->write(vector<string>{kv.second->getName()});
		}

		else if (cmd == "instances") {
			if (controller->getInstances().size() == 0) {
				socket->write(vector<string>{"no running instances"});
			} else {
				for (auto& kv : controller->getInstances())
					socket->write(vector<string>{
						"id: " + to_string(kv.first) + ", " +
						"mem: " + to_string(kv.second->getMemory()) + ", " +
						"cpus: " + to_string(kv.second->getCpus()) + ", " +
						"ssh: " + to_string(kv.second->getSshPort()) + ", " +
						(kv.second->isRunning() ? "ONLINE" : "OFFLINE")
					});
			}
		}

		else if (cmd == "new") {
			try {
				auto name = commands.at(1);
				int mem = stoi(commands.at(2));
				int cpus = stoi(commands.at(3));

				auto tmpls = controller->getTemplates();

				if (tmpls.find(name) == tmpls.end()) {
					socket->write(vector<string>{"template not found"});
				} else {
					auto tmpl = tmpls[name];
					auto inst = controller->instantiate(tmpl, mem, cpus);
					inst->run();
					socket->write(vector<string>{"instantiated and listening at " + to_string(inst->getSshPort())});
				}

			} catch(...) {
				socket->write(vector<string>{"invalid arguments for 'new'"});
			}
		}

		else {
			socket->write(vector<string>{"unknown command"});
		}

		socket->write(vector<string>{""});

		// socket->close();
		}
	}
};


int main(int argc, char** argv) {

	logger 		= make_shared<Util::Logger>(std::cout);
	

	if (argc < 4) {
		logger->log("usage: controller <control-port> <initial-id> <initial-ssh-port>");
		return -1;
	}

	ssocket 	= make_shared<Net::ServerSocket>(std::stoi(argv[1]));
	controller 	= make_shared<Vm::Controller>(stoi(argv[2]), stoi(argv[3]), logger);

	logger->log("VM Controller started!");

	for (;;) {
		try {
			auto socket = ssocket->accept();
			auto handler = new CommandHandler(socket);
			handler->start();
		}
		catch(Util::Exception& ex) {
			logger->log(ex.what());
		}
	}

	ssocket->close();

	return 0;
}
