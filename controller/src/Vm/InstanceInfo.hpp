
#ifndef _INSTANCEINFO_HPP_
#define _INSTANCEINFO_HPP_

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "Thread/Thread.hpp"
#include "Thread/Mutex.hpp"
#include "Vm/Controller.hpp"
#include "Net/Socket.hpp"
#include "Net/ServerSocket.hpp"
#include "Util/Logger.hpp"
#include "Util/Helpers.hpp"

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

public:

	InstanceInfo(
		const string& id,
		const string& templ,
		int memory,
		int cpus,
		int sshPort)
		: _id(id)
		, _templ(templ)
		, _memory(momory)
		, _cpus(cpus)
		, _sshPort(sshPort) { }

	string getId() { return _id; }
	string getTemplate() { return _templ; }
	int getMemory() { return _memory; }
	int getCpus() { return _cpus; }
	int getSshPort() { return _sshPort; }


	/* serialization */

	static constexpr int SERIAL_SIZE = 5;

	vector<string> serialize() {
		return vector<string> {
			id,
			_templ,
			to_string(_memory),
			to_string(_cpus),
			to_string(_sshPort)
		};
	}

	static shared_ptr<InstanceInfo> deserialize(vector<string> data) {
		return shared_ptr<InstanceInfo>(new InstanceInfo(
			data[0],
			data[1],
			stoi(data[2]),
			stoi(data[3]),
			stoi(data[4])
		));
	}
};

}

#endif // _INSTANCEINFO_HPP_
