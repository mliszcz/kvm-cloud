
// #include <linux/kvm.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>

#include <sys/types.h>
#include <unistd.h>

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "Controller.hpp"

extern "C" {
	#include "libkvm.h"
}

using std::string;



int main(int argc, char** argv) {

	auto controller = std::shared_ptr<vm::Controller>(new vm::Controller());

	auto cirros = controller->getTemplates()["cirros"];

	auto id = controller->instantiate(cirros, 128, 2);
	auto inst = controller->getInstances()[id];
	inst->run();

	std::cin.get();

	std::cout << inst->isRunning() << "\n";

	return 0;
}
