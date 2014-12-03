
#ifndef _CONTROLLER_HPP_
#define _CONTROLLER_HPP_

#include <map>
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <iostream>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Template.hpp"
#include "Instance.hpp"

#include "../Net/Socket.hpp"
#include "../Thread/Mutex.hpp"
#include "../Thread/ScopedLock.hpp"
#include "../Util/Logger.hpp"
#include "../Util/Exception.hpp"
#include "../Util/Cookie.hpp"

using std::map;
using std::string;
using std::shared_ptr;
using std::make_shared;
using std::to_string;

namespace Vm {

class Controller {

private:

	shared_ptr<Thread::Mutex> mutex = std::make_shared<Thread::Mutex>();
	shared_ptr<Util::Logger> logger;

	int nextId;
	int nextPort;

	map<string, shared_ptr<Template>> 	templates;
	map<int, 	shared_ptr<Instance>> 	instances;

	string workingDir = "/tmp";

	void copyFile(string from, string to) {
		std::ifstream  src(from.c_str(), std::ios::binary);
		std::ofstream  dst(to.c_str(), 	 std::ios::binary);
		dst << src.rdbuf();
	}

	bool isPortFree(int port) {
		try {
			Net::Socket("127.0.0.1", port);
			return false;
		} catch(...) {
			return true;
		}
	}

	bool dirExists(string path) {
		struct stat myStat;
		return (stat(path.c_str(), &myStat) == 0) &&
				(((myStat.st_mode) & S_IFMT) == S_IFDIR);
	}

	string mkInstDir(int id) {
		return workingDir + "/" + to_string(id);
	}


	shared_ptr<Instance> createInstance(shared_ptr<Template> templ, int memory, int cpus, int id, int port,
		string kernelPath, string rootfsPath) {

		auto inst = shared_ptr<Instance>(new Instance(
			id,
			templ,
			kernelPath,
			rootfsPath,
			memory,
			cpus,
			port
		));

		instances[id] = inst;

		string instanceDir = workingDir + "/" + to_string(id);

		Util::Cookie::write(instanceDir + "/.template", templ->getName());
		Util::Cookie::write(instanceDir + "/.memory", inst->getMemory());
		Util::Cookie::write(instanceDir + "/.cpus", inst->getCpus());
		Util::Cookie::write(instanceDir + "/.ssh", inst->getSshPort());

		logger->log("instantiating " + templ->getName() +
			" (id " + to_string(id) + ") at port " + to_string(inst->getSshPort()));

		return inst;
	}

public:

	Controller(int _initialId, int _initialPort,
		shared_ptr<Util::Logger> _logger = make_shared<Util::Logger>(std::cout))
		: nextId(_initialId), nextPort(_initialPort), logger(_logger) {

		// initialize templates

		templates["cirros"] = shared_ptr<Template>(new Template(
			"cirros",
			"CirrOS is a Tiny OS that specializes in running on a cloud.",
			"templates/cirros/cirros-0.3.3-x86_64-kernel",
			"templates/cirros/cirros-0.3.3-x86_64-rootfs-non_ec2.img"
		));

		// load existing VMs

		// TODO
	}

	map<string, shared_ptr<Template>> getTemplates() {
		Thread::ScopedLock lock(mutex);
		return templates;
	}

	map<int, shared_ptr<Instance>> getInstances() {
		Thread::ScopedLock lock(mutex);
		return instances;
	}

	shared_ptr<Instance> instantiate(shared_ptr<Template> templ, int memory, int cpus) {
		Thread::ScopedLock lock(mutex);

		while(!isPortFree(++nextPort));
		while(dirExists(mkInstDir(++nextId)));

		string instanceDir = mkInstDir(nextId); 
		string kernelPath = instanceDir + "/kernel";
		string rootfsPath = instanceDir + "/rootfs.img";

		Sys::Process("/bin/mkdir", std::vector<string>{instanceDir}).runAndWait();
		// usleep(500000);
		
		copyFile(templ->_kernelPath, kernelPath);
		copyFile(templ->_rootfsPath, rootfsPath);

		return createInstance(templ, memory, cpus, nextId, nextPort, kernelPath, rootfsPath);
	}

	shared_ptr<Instance> restore(int id) {
		Thread::ScopedLock lock(mutex);

		string instanceDir = mkInstDir(id);

		if (!dirExists(instanceDir))
			throw Util::Exception("Controller::restore(int)", "instnace with given id does not exist");

		if (instances.find(id) != instances.end()) {
			logger->log("id collision on restoring; assigning new id");
			while(dirExists(mkInstDir(++nextId)));
			id = nextId;
			Sys::Process("/bin/mkdir", std::vector<string>{instanceDir}).run();
			usleep(500000);

			Sys::Process("/bin/mv", std::vector<string>{instanceDir, mkInstDir(id)}).runAndWait();
			instanceDir = mkInstDir(id);
		}

		string kernelPath = instanceDir + "/kernel";
		string rootfsPath = instanceDir + "/rootfs.img";

		string tempName;
		int memory;
		int cpus;
		int sshPort;

		try {
			tempName 	= Util::Cookie::read<string>(instanceDir + "/.template");
			memory 		= Util::Cookie::read<int>(instanceDir + "/.memory");
			cpus 		= Util::Cookie::read<int>(instanceDir + "/.cpus");
			sshPort 	= Util::Cookie::read<int>(instanceDir + "/.ssh");
		} catch(...) {
			throw Util::Exception("Controller::restore(int)", "vm metadata is corrupted");
		}

		if (!isPortFree(sshPort))
			while(!isPortFree(++nextPort));

		return createInstance(templates[tempName], memory, cpus, nextId, nextPort, kernelPath, rootfsPath);
	}
};

}

#endif // _CONTROLLER_HPP_
