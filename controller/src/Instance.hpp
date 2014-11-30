
#ifndef _INSTANCE_HPP_
#define _INSTANCE_HPP_

#include <string>
#include <vector>
#include <memory>

#include "Process.hpp"

using std::string;
using std::shared_ptr;
using std::to_string;
using std::make_shared;

namespace vm {

class Controller;

class Instance {

	friend class Controller;

private:

	shared_ptr<sys::Process> process;

	Instance(
		string kernelPath,
		string rootfsPath,
		int memory,
		int cpus,
		int sshPort) {

		vector<string> vmArgs {
			"-m", to_string(memory),
			"-smp", to_string(cpus),
			"-redir", "tcp:" + to_string(sshPort) + "::22",
			"-kernel", kernelPath,
			"-hda", rootfsPath,
			"-boot", "c",
			"-append", "\"root=/dev/sda console=ttyS0\"",
			"-nographic",
			"-enable-kvm"
		};

		process = make_shared<sys::Process>("/usr/bin/kvm", vmArgs);
	}

public:

	void run() {
		process->run();
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
