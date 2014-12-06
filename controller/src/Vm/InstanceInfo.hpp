
#ifndef _INSTANCEINFO_HPP_
#define _INSTANCEINFO_HPP_

#include <map>
#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;
using std::to_string;
using std::shared_ptr;
using std::make_shared;
using std::stoi;

namespace Vm {

class InstanceInfo {
	
private:

	string _id;
	string _templ;
	int _memory;
	int _cpus;
	int _sshPort;
	bool _running;

public:

	InstanceInfo(
		const string& id,
		const string& templ,
		int memory,
		int cpus,
		int sshPort,
		bool running)
		: _id(id)
		, _templ(templ)
		, _memory(memory)
		, _cpus(cpus)
		, _sshPort(sshPort)
		, _running(running) { }

	string getId() { return _id; }
	string getTemplate() { return _templ; }
	int getMemory() { return _memory; }
	int getCpus() { return _cpus; }
	int getSshPort() { return _sshPort; }
	bool isRunning() { return _running; }


	/* serialization */

	static constexpr int SERIAL_SIZE = 6;

	vector<string> serialize() {
		return vector<string> {
			_id,
			_templ,
			to_string(_memory),
			to_string(_cpus),
			to_string(_sshPort),
			to_string((int)_running)
		};
	}

	static shared_ptr<InstanceInfo> deserialize(vector<string> data) {
		return shared_ptr<InstanceInfo>(new InstanceInfo(
			data[0],
			data[1],
			stoi(data[2]),
			stoi(data[3]),
			stoi(data[4]),
			(bool)stoi(data[5])
		));
	}
};

}

#endif // _INSTANCEINFO_HPP_
