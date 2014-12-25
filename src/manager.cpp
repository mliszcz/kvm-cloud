
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
#include <utility>

#include "Thread/Thread.hpp"
#include "Thread/Mutex.hpp"
#include "Thread/ScopedLock.hpp"
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
using std::pair;


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
shared_ptr<Thread::Mutex> 		mutex;

vector<shared_ptr<ControllerRef>> controllers;
int nextController = 0;


class CommandHandler:
	public Thread::Thread<Thread::DetachState::Detached, Thread::CancelType::Deferred> {

private:
	std::shared_ptr<Net::Socket> socket;

	string makeFullId(string serverName, string numericId) {
		return serverName + "/" + numericId;
	}

	std::pair<string, string> extractIdParts(string fullId) {
		auto id = fullId.substr(fullId.find("/")+1);
		auto srv = fullId.substr(0, fullId.find("/"));
		return std::make_pair(srv, id);
	}

	int advanceModulo(int base) {
		return ++base % controllers.size();
	}

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
				::Thread::ScopedLock lock(mutex);

				for (auto& controller : controllers) {
					for (auto& kv : controller->proxy->getTemplates()) {
						socket->write(kv.second->serialize());
					}
				}
			}

			else if (cmd == "getInstances") {
				::Thread::ScopedLock lock(mutex);

				for (auto& controller : controllers) {
					for (auto& kv : controller->proxy->getInstances()) {
						auto instance = kv.second;
						auto fullId = makeFullId(controller->id, instance->getId());
						instance->setId(fullId);
						socket->write(instance->serialize());
					}
				}
			}

			else if (cmd == "instantiate") {
				::Thread::ScopedLock lock(mutex);

				auto request = Vm::InstanceInfo::deserialize(vector<string>(commands.begin()+1, commands.end()));
				auto templName = request->getTemplate();

				int tries = controllers.size();

				while(tries --> 0) {

					nextController = advanceModulo(nextController);
					auto controller = controllers[nextController];
					auto templates = controller->proxy->getTemplates();

					for (auto& kv : templates) {

						if (kv.first == templName) {

							// ok, next controller offers this template
							auto inst = controller->proxy->instantiate(
								kv.second, request->getMemory(), request->getCpus()
							);

							inst->setId(makeFullId(controller->id, inst->getId()));

							socket->write(inst->serialize());
							tries = -100;
							break;
						}
					}	
				}

				if (tries > -10) {
					logger->warn("unable to find controller with template " + templName);
					// echo-response
					socket->write(request->serialize());
				}
			}

			else if (cmd == "run") {
				::Thread::ScopedLock lock(mutex);

				bool result = false;

				if (commands.size() < 2)
					logger->error("id not provided");

				else {
					auto idParts = extractIdParts(commands[1]);
					bool found = false;
					for (auto& controller : controllers) {
						if (controller->id == idParts.first) {
							result = controller->proxy->run(idParts.second);
							found = true;
							break;
						}
					}

					logger->warn("controller with id " + idParts.first + " not found");
				}

				socket->write(to_string((int)result));

			}

			else if (cmd == "kill") {
				::Thread::ScopedLock lock(mutex);

				bool result = false;

				if (commands.size() < 2)
					logger->error("id not provided");

				else {
					auto idParts = extractIdParts(commands[1]);
					bool found = false;
					for (auto& controller : controllers) {
						if (controller->id == idParts.first) {
							result = controller->proxy->kill(idParts.second);
							found = true;
							break;
						}
					}

					logger->warn("controller with id " + idParts.first + " not found");
				}

				socket->write(to_string((int)result));

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
	mutex = make_shared<Thread::Mutex>();

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
			controllers.push_back(
				make_shared<ControllerRef>(
					s1, addr.first, addr.second,
					make_shared<Vm::ControllerProxy>(addr.first, addr.second)
				)
			);
		} catch (...) {
			logger->error("failed to connect to " + s1 + " controller instance");
		}
	}
	fControl.close();

	logger->info("supervised controllers:");
	for (auto ctrl : controllers)
		logger->info(
			ctrl->id + ": " + ctrl->address + ":" + to_string(ctrl->port)
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
