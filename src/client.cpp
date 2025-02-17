#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "Thread/Thread.hpp"
#include "Thread/Mutex.hpp"
#include "Vm/Controller.hpp"
#include "Vm/ControllerProxy.hpp"
#include "Util/Logger.hpp"
#include "Util/Helpers.hpp"

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
using std::cout;


shared_ptr<Util::Logger> logger;
shared_ptr<Vm::ControllerProxy> controller;

int main(int argc, char** argv) {

	logger = make_shared<Util::Logger>(Util::Logger::Level::INFO, std::cout);

	if (argc < 2) {
		std::cout << "usage: client <controller-address:controller-port>\n";
		return -1;
	}

	try {
		auto addr = Util::Helpers::explodeAddress(argv[1]);
		controller = make_shared<Vm::ControllerProxy>(addr.first, addr.second);
	} catch(...) {
		logger->error("cannot create controller proxy");
		return -1;
	}

	string line;

	cout << "KVM-cloud CLI\n";
	cout << "type 'help' for list of all available commands\n\n";
	cout << "> ";

	while(std::getline(std::cin, line)) {

		if (line == "exit") break;

		std::istringstream iss(line);
		string token;
		vector<string> msg;

		while(iss >> token) {
			msg.push_back(token);
		}

		if (msg.size() == 0) {
			std::cout << "\n> ";
			continue;
		}

		if (msg[0] == "help") {

			cout << " - templates\n";
			cout << "       list of all available templates\n";
			cout << " - instances\n";
			cout << "       list of all running VM instances\n";
			cout << " - new <template> <memory> <cpus>\n";
			cout << "       create new VM using given parameters\n";
			cout << " - run <vm-id>\n";
			cout << "       run VM with given id\n";
			cout << " - kill <vm-id>\n";
			cout << "       kill VM with given id\n";
			cout << " - exit\n";
			cout << "       exit CLI\n";
		}

		else if (msg[0] == "templates") {

			for (auto kv : controller->getTemplates()) {
				auto tmpl = kv.second;
				cout << " - " + tmpl->getName() + " (" + tmpl->getDescription() + ")\n";
			}
		}

		else if (msg[0] == "instances") {

			cout << "   ID\t\ttemplate\tRAM\t\tCPUs\t\tSSH port\tstatus\n";

			for (auto kv : controller->getInstances()) {
				auto inst = kv.second;
				cout << " - " << inst->getId() << "\t"
					 << inst->getTemplate() << "\t\t"
					 << inst->getMemory() << "\t\t"
					 << inst->getCpus() << "\t\t"
					 << (inst->isRunning() ? to_string(inst->getSshPort()) : "  -  ") << "\t\t"
					 << (inst->isRunning() ? "ONLINE" : "OFFLINE") << "\t\n";
			}
		}

		else if (msg[0] == "new") {

			try {
				auto templ = controller->getTemplates();
				auto inst = controller->instantiate(
					templ.at(msg.at(1)), stoi(msg.at(2)), stoi(msg.at(3)));

				cout << "   result: ID: " << inst->getId() << ", SSH port: " << inst->getSshPort() << "\n";
			}
			catch (...) {
				cout << "invalid arguments\n";
			}
		}

		else if (msg[0] == "run") {

			try {
				cout << (controller->run(msg.at(1)) ? "OK\n" : "ERROR\n");
			}
			catch (...) {
				cout << "invalid arguments\n";
			}
		}

		else if (msg[0] == "kill") {

			try {
				cout << (controller->kill(msg.at(1)) ? "OK\n" : "ERROR\n");
			}
			catch (...) {
				cout << "invalid arguments\n";
			}
		}

		else {

			cout << "unknown command\n";
		}

		std::cout << "\n> ";
	}

	return 0;
}
