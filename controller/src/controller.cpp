
// #include <linux/kvm.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <cstdio>
#include <cstring>

extern "C" {
	#include "libkvm.h"
}

// https://www.kernel.org/doc/Documentation/virtual/kvm/api.txt

int main(int argc, char** argv) {

	kvm_callbacks callbacks;

	kvm_context_t kvm_ctx = kvm_init(&callbacks, nullptr);

	printf("ctx: %d\n", kvm_ctx);

	// int fd = open("/dev/kvm", O_RDWR);
	// if (fd == -1) {
	// 	perror("open /dev/kvm");
	// 	return 1;
	// }
	
	// int r = ioctl(fd, KVM_GET_API_VERSION, 0);
	// if (r == -1) {
	// 	fprintf(stderr, "kvm kernel version too old: KVM_GET_API_VERSION ioctl not supported\n");
	// 	return 1;
	// }

	// printf("api ver: %d\n", r);

	kvm_finalize(kvm_ctx);

	return 0;
}
