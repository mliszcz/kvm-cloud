
#ifndef _CONTROLLER_HPP_
#define _CONTROLLER_HPP_

#include <map>
#include <string>
#include <memory>
#include <vector>
#include <fstream>

#include <unistd.h>

#include "Template.hpp"
#include "Instance.hpp"

#include "../Thread/Mutex.hpp"

using std::map;
using std::string;
using std::shared_ptr;
using std::to_string;

namespace Vm {

class Controller {

private:
	int nextId = 1001;
	int nextPort = 2221;

	map<string, 	shared_ptr<Template>> 	templates;
	map<InstanceId, shared_ptr<Instance>> 	instances;

	string workingDir = "/tmp";

	void copyFile(string from, string to) {
		std::ifstream  src(from.c_str(), std::ios::binary);
		std::ofstream  dst(to.c_str(), 	 std::ios::binary);
		dst << src.rdbuf();
	}

public:

	Thread::Mutex mutex;

	Controller(int _initialId, int _initialPort)
		nextId(_initialId), nextPort(_initialPort) {

		// initialize templates
		templates["cirros"] = shared_ptr<Template>(new Template(
			"cirros",
			"templates/cirros/cirros-0.3.3-x86_64-kernel",
			"templates/cirros/cirros-0.3.3-x86_64-rootfs-non_ec2.img"
		));
	}

	map<string, shared_ptr<Template>> getTemplates() {
		return templates;
	}

	map<InstanceId, shared_ptr<Instance>> getInstances() {
		return instances;
	}

	int instantiate(shared_ptr<Template> templ, int memory, int cpus) {

		string instanceDir = workingDir + "/" + to_string(nextId); 
		string kernelPath = instanceDir + "/kernel";
		string rootfsPath = instanceDir + "/rootfs.img";

		Sys::Process("/bin/mkdir", std::vector<string>{instanceDir}).run();
		usleep(500000);
		
		copyFile(templ->_kernelPath, kernelPath);
		copyFile(templ->_rootfsPath, rootfsPath);

		instances[nextId] = shared_ptr<Instance>(new Instance(
			templ,
			kernelPath,
			rootfsPath,
			memory,
			cpus,
			nextPort
			));

		nextPort++;
		return nextId++;
	}
};

}

#endif // _CONTROLLER_HPP_
