
#ifndef _INSTANCE_HPP_
#define _INSTANCE_HPP_

#include <string>
#include <vector>
#include <memory>

#include "../Sys/Process.hpp"
#include "Template.hpp"

using std::string;
using std::shared_ptr;
using std::to_string;
using std::make_shared;

namespace Vm {

class Controller;

class Instance {

	friend class Controller;

private:

	int id;
	int memory;
	int cpus;
	int sshPort;

	shared_ptr<Sys::Process> process;
	shared_ptr<Template> templat;

	Instance(
		shared_ptr<Template> _templat,
		string kernelPath,
		string rootfsPath,
		int _memory,
		int _cpus,
		int _sshPort)
		: templat(_templat)
		, memory(_memory)
		, cpus(_cpus)
		, sshPort(_sshPort) {

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

		process = make_shared<Sys::Process>("/bin/bash", vmArgs);
	}

public:

	int getId() { return id; }

	int getMemory() { return memory; }

	int getCpus() { return cpus; }

	int getSshPort() { return sshPort; }

	shared_ptr<Template> getTemplate() { return templat; }

	bool run() {
		if (isRunning()) return false;
		process->run(true);
		return isRunning();
	}

	bool isRunning() {
		return process->isRunning();
	}

	void kill() {
		process->kill();
	}
};

}

#endif // _INSTANCE_HPP_
