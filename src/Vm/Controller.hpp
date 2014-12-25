
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
#include <dirent.h>

#include "Template.hpp"
#include "Instance.hpp"

#include "../Net/Socket.hpp"
#include "../Thread/Mutex.hpp"
#include "../Thread/ScopedLock.hpp"
#include "../Util/Logger.hpp"
#include "../Util/Exception.hpp"
#include "../Util/Cookie.hpp"
#include "../Sys/Process.hpp"


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

	string workingDir;

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

	int findNextId(int baseId) {
		while(dirExists(mkInstDir(baseId)) || Util::Helpers::mapExists(instances, baseId)) ++baseId;
		return baseId;
	}

	int findNextPort(int basePort) {
		while(!isPortFree(++basePort));
		return basePort;
	}

	string mkInstDir(int id) {
		return workingDir + "/" + to_string(id);
	}


public:

	Controller(const std::string& _workingDir, int _initialId, int _initialPort,
		shared_ptr<Util::Logger> _logger = make_shared<Util::Logger>(Util::Logger::Level::INFO, std::cout))
		: workingDir(_workingDir), nextId(_initialId), nextPort(_initialPort), logger(_logger) {

		// initialize templates

		templates["cirros"] = shared_ptr<Template>(new Template(
			"cirros",
			"CirrOS is a Tiny OS that specializes in running on a cloud.",
			"templates/cirros/cirros-0.3.3-x86_64-kernel",
			"templates/cirros/cirros-0.3.3-x86_64-rootfs-non_ec2.img"
		));

		// load existing VMs

		struct dirent *de = nullptr;
		DIR *d = nullptr;

		if (!(d = opendir(workingDir.c_str())))
			throw Util::Exception("Controller::Controller(...)", "failed to open working directory " + workingDir);

		while (de = readdir(d)) {
			try {
				try {
					restore(std::stoi(string(de->d_name)));
				}
				catch (std::invalid_argument& e) { }
			}
			catch (Util::Exception& ex) {
				logger->error(ex.what());
			}
		}

		closedir(d);
	}

	map<string, shared_ptr<Template>> getTemplates() {
		Thread::ScopedLock lock(mutex);
		return templates;
	}

	map<int, shared_ptr<Instance>> getInstances() {
		Thread::ScopedLock lock(mutex);
		return instances;
	}

	bool run(int id) {

		if (!Util::Helpers::mapExists(instances, id)) {
			logger->error("vm with id " + to_string(id) + " not found");
			return false;
		}

		auto inst = instances[id];

		if (inst->getProcess()->isRunning()) {
			logger->error("vm with id " + to_string(id) + " is already running");
			return false;
		}


		// port may be in use
		int sshPort = inst->getSshPort();
		if (!isPortFree(sshPort)) {
			logger->warn("port " + to_string(sshPort) + " is already in use. searching for other port");
			nextPort = findNextPort(nextPort);
			Util::Cookie::write(mkInstDir(id) + "/.ssh", nextPort);
			inst->setSshPort(nextPort);
			logger->info("assigned new port " + to_string(nextPort));
		}

		inst->getProcess()->run();
		Util::Cookie::write(mkInstDir(id) + "/.pid", inst->getProcess()->getPid());
		return true;
	}

	bool kill(int id) {

		if (!Util::Helpers::mapExists(instances, id)) {
			logger->error("vm with id " + to_string(id) + " not found");
			return false;
		}

		auto inst = instances[id];

		auto proc = inst->getProcess();

		if (!proc->isRunning()) return true;
		proc->kill();
		
		return true;
	}

	shared_ptr<Instance> instantiate(shared_ptr<Template> templ, int memory, int cpus) {
		Thread::ScopedLock lock(mutex);

		nextPort = findNextPort(nextPort);
		nextId = findNextId(nextId);

		string instanceDir = mkInstDir(nextId); 
		Sys::Process("/bin/mkdir", std::vector<string>{instanceDir}).runAndWait();

		string kernelPath = instanceDir + "/kernel";
		string rootfsPath = instanceDir + "/rootfs.img";

		copyFile(templ->_kernelPath, kernelPath);
		copyFile(templ->_rootfsPath, rootfsPath);

		Util::Cookie::write(instanceDir + "/.template", templ->getName());
		Util::Cookie::write(instanceDir + "/.memory", memory);
		Util::Cookie::write(instanceDir + "/.cpus", cpus);
		Util::Cookie::write(instanceDir + "/.ssh", nextPort);
		Util::Cookie::write(instanceDir + "/.pid", 0);

		return restore(nextId);
	}

	shared_ptr<Instance> restore(int id) {
		// Thread::ScopedLock lock(mutex);

		string instanceDir = mkInstDir(id);

		if (!dirExists(instanceDir))
			throw Util::Exception("Controller::restore(int)", "instnace with given id does not exist");

		if (Util::Helpers::mapExists(instances, id)) {
			return instances[id];
		}

		string kernelPath = instanceDir + "/kernel";
		string rootfsPath = instanceDir + "/rootfs.img";

		string tempName;
		int memory;
		int cpus;
		int sshPort;
		int pid;

		try {
			tempName 	= Util::Cookie::read<string>(instanceDir + "/.template");
			memory 		= Util::Cookie::read<int>(instanceDir + "/.memory");
			cpus 		= Util::Cookie::read<int>(instanceDir + "/.cpus");
			sshPort 	= Util::Cookie::read<int>(instanceDir + "/.ssh");
			pid 		= Util::Cookie::read<int>(instanceDir + "/.pid");
		} catch(...) {
			throw Util::Exception("Controller::restore(int)", "vm metadata is corrupted");
		}

		// port may be taken already

		if (!isPortFree(sshPort)) {
			nextPort = findNextPort(nextPort);
			Util::Cookie::write(instanceDir + "/.ssh", nextPort);
			sshPort = nextPort;
		}


		// prepare process instance

		vector<string> vmArgs {
			"-c",
			"''/usr/bin/kvm -m " + to_string(memory)
			+" -smp " + to_string(cpus)
			+" -redir tcp:" + to_string(sshPort) + "::22"
			+" -kernel " + kernelPath
			+" -hda " + rootfsPath
			+" -boot c"
			+" -append \"root=/dev/sda console=ttyS0\""
			+" -nographic"
			+" -enable-kvm''"
		};

		auto process = make_shared<Sys::Process>("/bin/bash", vmArgs);

		if (!Util::Helpers::mapExists(templates, tempName)) {
			throw Util::Exception("Controller::restore(int)", "template " + tempName + " is missing");
		}

		auto inst = shared_ptr<Instance>(new Instance(
			id,
			memory,
			cpus,
			sshPort,
			templates[tempName],
			process
		));

		instances[id] = inst;

		logger->info("instantiating " + templates[tempName]->getName() +
			" (id " + to_string(id) + ") at port " + to_string(inst->getSshPort()));

		return inst;
	}
};

}

#endif // _CONTROLLER_HPP_
