
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
#include <fstream>

#include "Thread/Thread.hpp"
#include "Thread/Mutex.hpp"
#include "Vm/Controller.hpp"
#include "Vm/ControllerProxy.hpp"
#include "Net/Socket.hpp"
#include "Net/ServerSocket.hpp"
#include "Util/Logger.hpp"
#include "Util/Helpers.hpp"

using std::map;
using std::string;
using std::vector;
using std::to_string;
using std::shared_ptr;
using std::make_shared;
using std::stoi;


class ControllerRef {
public:
	string id;
	string address;
	int port;
	shared_ptr<Vm::ControllerProxy> proxy;

	ControllerRef(
		const string& _id,
		const string& _address,
		int _port,
		shared_ptr<Vm::ControllerProxy> _proxy)
		: id(_id)
		, address(_address)
		, port(_port)
		, proxy(_proxy) { }
};


shared_ptr<Net::ServerSocket> ssocket;
shared_ptr<Util::Logger> 	   logger;

map<string, shared_ptr<ControllerRef>> controllers;


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

		for (;;) {

			auto commands = socket->read();
			auto cmd = commands.at(0);

			logger->info("processing request: " + cmd);
	
			if (cmd == "getTemplates") {

				//TODO: collect templates from all controllers
				auto ctrl = controllers.begin()->second->proxy;

				for (auto& kv : ctrl->getTemplates())
					socket->write(kv.second->serialize());

			}

			else if (cmd == "getInstances") {

				//TODO: collect templates from all controllers
				auto ctrl = controllers.begin()->second->proxy;
				auto insts = ctrl->getInstances();

				if (insts.size() == 0)
					socket->write("");
				else
					for (auto& kv : insts) {
						socket->write(kv.second->serialize());
					}
			}

			else if (cmd == "instantiate") {

				auto ctrl = controllers.begin()->second->proxy;

				auto req = Vm::InstanceInfo::deserialize(vector<string>(commands.begin()+1, commands.end()));

				auto templ = ctrl->getTemplates();

				try {
					auto inst = ctrl->instantiate(templ.at(req->getTemplate()), req->getMemory(), req->getCpus());
					socket->write(inst->serialize());
				} catch (...) {
					socket->write(req->serialize());
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

	logger = make_shared<Util::Logger>(Util::Logger::Level::INFO, std::cout);

	if (argc < 3) {
		std::cout << "usage: manager <control-port> <controllers-file>\n";
		return -1;
	}

	ssocket = make_shared<Net::ServerSocket>(stoi(argv[1]));

	logger->info("manager started");

	string s1;
	string s2;
	std::ifstream fControl(argv[2]);
	while(fControl >> s1 && fControl >> s2) {
		auto addr = Util::Helpers::explodeAddress(s2);
		try {
			controllers[s1] = make_shared<ControllerRef>(
				s1, addr.first, addr.second,
			make_shared<Vm::ControllerProxy>(addr.first, addr.second));
		} catch (...) {
			logger->error("failed to connect to " + s1);
		}
	}
	fControl.close();

	logger->info("supervised controllers:");
	for (auto kv : controllers)
		logger->info(
			kv.second->id + ": " +
			kv.second->address + ":" + to_string(kv.second->port)
		);

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
