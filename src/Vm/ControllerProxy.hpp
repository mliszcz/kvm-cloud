
#ifndef _CONTROLLERPROXY_HPP_
#define _CONTROLLERPROXY_HPP_

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "Controller.hpp"
#include "../Net/Socket.hpp"
#include "Template.hpp"
#include "InstanceInfo.hpp"

using std::map;
using std::string;
using std::vector;
using std::to_string;
using std::shared_ptr;
using std::make_shared;
using std::stoi;

namespace Vm {

class ControllerProxy {

private:
	shared_ptr<Net::Socket> socket;

public:

	ControllerProxy(string address, int port) {
		socket = make_shared<Net::Socket>(address, port);
	}

	~ControllerProxy() {
		socket->close();
	}

	map<string, shared_ptr<Template>> getTemplates() {

		socket->write("getTemplates").send();
		auto data = socket->read();

		map<string, shared_ptr<Template>> result;

		for (int i=0; i<data.size(); i += Template::SERIAL_SIZE) {
			auto tmpl = Template::deserialize(vector<string>(data.begin()+i, data.begin()+i+Template::SERIAL_SIZE));
			result[tmpl->getName()] = tmpl;
			return result;
		}
	}

	map<string, shared_ptr<InstanceInfo>> getInstances() {

		socket->write("getInstances").send();
		auto data = socket->read();

		map<string, shared_ptr<InstanceInfo>> result;

		for (int i=0; i<data.size(); i += InstanceInfo::SERIAL_SIZE) {
			auto inst = InstanceInfo::deserialize(vector<string>(data.begin()+i, data.begin()+i+InstanceInfo::SERIAL_SIZE));
			result[inst->getId()] = inst;
		}

		return result;
	}

	shared_ptr<InstanceInfo> instantiate(shared_ptr<Template> templ, int memory, int cpus) {

		socket-> write("instantiate")
				.write(InstanceInfo("0", templ->getName(), memory, cpus, 0, 0).serialize())
				.send();

		return InstanceInfo::deserialize(socket->read());
	}

	bool run(string fullId) {

		socket-> write("run")
				.write(fullId)
				.send();

		return (bool)stoi(socket->read()[0]);
	}
};

}

#endif // _CONTROLLERPROXY_HPP_
