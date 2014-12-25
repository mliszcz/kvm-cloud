
#ifndef _TEMPLATE_HPP_
#define _TEMPLATE_HPP_

#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;
using std::shared_ptr;

namespace Vm {

// class Controller;

class Template {

friend class Controller;

private:
	string _name;
	string _description;
	string _kernelPath;
	string _rootfsPath;

	Template(
		const string& name,
		const string& description,
		const string& kernelPath,
		const string& rootfsPath)
		: _name(name)
		, _description(description)	
		, _kernelPath(kernelPath)
		, _rootfsPath(rootfsPath) { }

public:

	string getName() { return _name; }
	string getDescription() { return _description; }
	string getKernelPath() { return _kernelPath; }
	string getRootfsPath() { return _rootfsPath; }


	/* serialization */

	static constexpr int SERIAL_SIZE = 4;

	vector<string> serialize() {
		return vector<string> {
			_name,
			_description,
			_kernelPath,
			_rootfsPath
		};
	}

	static shared_ptr<Template> deserialize(vector<string> data) {
		return shared_ptr<Template>(new Template(
			data[0],
			data[1],
			data[2],
			data[3]
		));
	}

};

}

#endif // _TEMPLATE_HPP_
