
#ifndef _PROCESS_HPP_
#define _PROCESS_HPP_

#include <map>
#include <string>
#include <vector>
#include <cstring>

#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../Util/Exception.hpp"


using std::map;;
using std::string;
using std::vector;

namespace Sys {

class Process {
private:

	string _path;
	vector<string> _opts;

	pid_t _pid;

public:
	Process(string path, vector<string> opts = vector<string>(), int pid = 0)
		: _path(path), _opts(opts), _pid(pid) { }

	// Process(int pid)
	// : _pid(pid) {
	// 	if (!isRunning())
	// 		throw Util::Exception("Process::Process(int)", "attempt to attach to non-existent process");
	// }

	~Process() = default;

	pid_t run(bool silent = true) {

		if (isRunning())
			throw Util::Exception("Process::run(bool)", "attempt to run running process");

		_pid = fork();

		if (_pid != 0) return _pid;

		if (silent) {
			int fd = open("/dev/null", O_RDWR);
			dup2(fd, 1);
			dup2(fd, 2);
			dup2(fd, 0);
			close(fd);
		}

		_opts.insert(_opts.begin(), "");

		char** args = new char*[_opts.size()+1];
		int idx = 0;
		for (auto& kv: _opts)
			for (auto a: vector<string>{kv}) {
				args[idx] = new char[a.size()+1];
				strcpy(args[idx++], a.c_str());
			}
		args[idx] = nullptr;

		int code = execvp(_path.c_str(), args);
		fprintf(stderr, "an error occurred in execvp: %d\n", code);
		abort();
	}

	bool isRunning() {
		if (_pid == 0) return false;
		return ::kill(_pid, 0) == 0;
	}

	void kill() {
		// ::kill(_pid, 9);
		::kill(_pid, 2);
	}

	int getPid() {
		return static_cast<int>(_pid);
	}

	void wait() {
		int status;
		::waitpid(_pid, &status, 0);
	}

	void runAndWait() {
		run();
		wait();
	}

};

}

#endif // _PROCESS_HPP_
