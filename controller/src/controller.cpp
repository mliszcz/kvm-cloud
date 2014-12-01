
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

int main(int argc, char** argv) {

	auto controller = std::shared_ptr<vm::Controller>(new vm::Controller());

	auto cirros = controller->getTemplates()["cirros"];

	auto id = controller->instantiate(cirros, 128, 2);
	auto inst = controller->getInstances()[id];
	inst->run();

	std::cin.get();

	std::cout << inst->isRunning() << "\n";

	// execlp("/usr/bin/kvm", "-m 128",
	// 		"-smp 2",
	// 		("-redir tcp:" + std::string("2221") + "::22").c_str(),
	// 		"-kernel ./1001/kernel",
	// 		"-hda ./1001/rootfs.img",
	// 		"-boot c",
	// 		"-append \"root=/dev/sda console=ttyS0\"",
	// 		"-nographic",
	// 		"-enable-kvm", NULL);

	// execlp("/usr/bin/kvm", "-m 128"
	// 		" -smp 2"
	// 		" -redir tcp:2221::22"
	// 		" -kernel ./1001/kernel"
	// 		" -hda ./1001/rootfs.img"
	// 		" -boot c"
	// 		" -append \"root=/dev/sda console=ttyS0\""
	// 		" -nographic"
	// 		" -enable-kvm", NULL);

		// execlp("/usr/bin/kvm", "/usr/bin/kvm", "-m", "128", "-smp", "2", "-redir", "tcp:2221::22", "-kernel", "./1001/kernel",
			// "-hda", "./1001/rootfs.img", "-boot", "c", "-append", "\"root=/dev/sda console=ttyS0\"", "-nographic", "-enable-kvm", NULL);

	// execlp("/bin/bash", "-c \"/usr/bin/kvm -m 128 -smp 2 -redir tcp:2221::22 -kernel ./1001/kernel -hda ./1001/rootfs.img -boot c -append \"root=/dev/sda console=ttyS0\" -nographic -enable-kvm\"", NULL);

	// auto pid = fork();

	// auto pid = 0;

	// if (pid == 0) {
		// char* arg[] = {"./run_vm.sh", "128", "2", "2221", "./1001/kernel", "./1001/rootfs.img", NULL};
		// char* arg[] = {
		// 	"/usr/bin/kvm", "-m", "128", "-smp", "2", "-redir", "tcp:2221::22", "-kernel", "./1001/kernel",
		// 	"-hda", "./1001/rootfs.img", "-boot", "c", "-append", "\"root=/dev/sda console=ttyS0\"", "-nographic", "-enable-kvm", NULL

		// };

		// char* arg[] = {
		// 	"/bin/bash", "-c", "''/usr/bin/kvm -m 128 -smp 2 -redir tcp:2221::22 -kernel ./1001/kernel -hda ./1001/rootfs.img -boot c -append \"root=/dev/sda console=ttyS0\" -nographic -enable-kvm''", NULL

		// };

		// char* arg[] = {"/bin/bash", "-c", "ps", NULL};
		// execvp(arg[0],arg);
		// execlp("./run_vm.sh", "empty", "128", "2", "2221", "./1001/kernel", "./1001/rootfs.img", NULL);
	// }



	return 0;
}
