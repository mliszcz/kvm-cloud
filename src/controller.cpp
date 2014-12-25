
// #include <linux/kvm.h>
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
#include "Vm/InstanceInfo.hpp"
#include "Net/Socket.hpp"
#include "Net/ServerSocket.hpp"
#include "Util/Logger.hpp"
#include "Util/Helpers.hpp"

extern "C" {
	#include "libkvm.h"
}

using std::string;
using std::vector;
using std::to_string;
using std::shared_ptr;
using std::make_shared;
using std::stoi;


shared_ptr<Vm::Controller> controller;
shared_ptr<Net::ServerSocket> ssocket;
shared_ptr<Util::Logger> 	   logger;


class CommandHandler:
	public Thread::Thread<Thread::DetachState::Detached, Thread::CancelType::Deferred> {

private:
	std::shared_ptr<Net::Socket> socket;

public:
	CommandHandler(std::shared_ptr<Net::Socket> _socket)
		: socket(_socket) { }

	~CommandHandler() {
		socket->close();
	}

	virtual void run() {

		for(;;) {

			auto commands = socket->read();
			auto cmd = commands.at(0);
	
			logger->info("processing request: " + cmd);
	
			if (cmd == "getTemplates") {
				for (auto& kv : controller->getTemplates())
					socket->write(kv.second->serialize());
			}
	
			else if (cmd == "getInstances") {
				if (controller->getInstances().size() == 0)
					socket->write("");
				else
					for (auto& kv : controller->getInstances())
						socket->write(kv.second->getInstanceInfo()->serialize());
			}
	
			else if (cmd == "instantiate") {

				auto inst = Vm::InstanceInfo::deserialize(vector<string>(commands.begin()+1, commands.end()));
				auto tmpls = controller->getTemplates();
	
				try {
	
					auto tmplName = inst->getTemplate();
	
					if (tmpls.find(tmplName) == tmpls.end()) {
						// template not found
						socket->write(inst->serialize());
					} else {
						auto tmpl = tmpls[tmplName];
						auto realInst = controller->instantiate(tmpl, inst->getMemory(), inst->getCpus());
						realInst->run();
						socket->write(realInst->getInstanceInfo()->serialize());
					}
	
				} catch(...) {
					// invalid arguments
					socket->write(inst->serialize());
				}
			}
	
			else {
				socket->write("unknown command");
			}
	
			socket->send();
		}
	}
};


int main(int argc, char** argv) {

	// http://stackoverflow.com/questions/17015830/how-can-i-prevent-zombie-child-processes
	struct sigaction sigchld_action;
	sigchld_action.sa_handler = SIG_DFL;
	sigchld_action.sa_flags = SA_NOCLDWAIT;
	sigaction(SIGCHLD, &sigchld_action, nullptr);


	logger 		= make_shared<Util::Logger>(Util::Logger::Level::INFO, std::cout);

	if (argc < 4) {
		std::cout << "usage: controller <control-port> <initial-id> <initial-ssh-port>\n";
		return -1;
	}

	ssocket 	= make_shared<Net::ServerSocket>(std::stoi(argv[1]));
	controller 	= make_shared<Vm::Controller>(stoi(argv[2]), stoi(argv[3]), logger);

	logger->info("VM Controller started!");

	for (;;) {
		try {
			auto socket = ssocket->accept();
			auto handler = new CommandHandler(socket);
			handler->start();
		}
		catch(Util::Exception& ex) {
			logger->error(ex.what());
		}
	}

	ssocket->close();

	return 0;
}
