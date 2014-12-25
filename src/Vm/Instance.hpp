
#ifndef _INSTANCE_HPP_
#define _INSTANCE_HPP_

#include <string>
#include <vector>
#include <memory>

#include "../Sys/Process.hpp"
#include "Template.hpp"
#include "InstanceInfo.hpp"

using std::string;
using std::shared_ptr;
using std::make_shared;
using std::to_string;
using std::make_shared;

using Sys::Process;

namespace Vm {

class Controller;

class Instance {

	friend class Controller;

private:

	int id;
	int memory;
	int cpus;
	int sshPort;

	shared_ptr<Process>  process;
	shared_ptr<Template> templat;

	Instance(
		int _id,
		// string kernelPath,
		// string rootfsPath,
		int _memory,
		int _cpus,
		int _sshPort,
		shared_ptr<Template> _templat,
		shared_ptr<Process>  _process
		)
		: id(_id)
		, memory(_memory)
		, cpus(_cpus)
		, sshPort(_sshPort)
		, templat(_templat)
		, process(_process) { }

public:

	bool run() {
		if (process->isRunning()) return false;
		process->run(true);
		return process->isRunning();
	}

	int getId() 		{ return id; 		}
	int getMemory() 	{ return memory; 	}
	int getCpus() 		{ return cpus; 		}
	int getSshPort() 	{ return sshPort; 	}

	shared_ptr<Template> 		getTemplate() 	{ return templat; }
	shared_ptr<Sys::Process> 	getProcess() 	{ return process; }

	shared_ptr<InstanceInfo> getInstanceInfo() {
		return make_shared<InstanceInfo>(
			std::to_string(id),
			templat->getName(),
			memory,
			cpus,
			sshPort,
			(int)(process->isRunning())
		);
	}
};

}

#endif // _INSTANCE_HPP_
