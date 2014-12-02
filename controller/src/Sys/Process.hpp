
#ifndef _PROCESS_HPP_
#define _PROCESS_HPP_

#include <map>
#include <string>
#include <vector>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


using std::map;;
using std::string;
using std::vector;

namespace Sys {

class Process {
private:
	pid_t _pid = 0;

	string _path;
	vector<string> _opts;

public:
	Process(string path, vector<string> opts = vector<string>())
		: _path(path), _opts(opts) { }

	~Process() = default;

	pid_t run(bool silent = true) {

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
		::kill(_pid, 9);
	}

};

}

#endif // _PROCESS_HPP_
