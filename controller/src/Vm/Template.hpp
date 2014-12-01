
#ifndef _TEMPLATE_HPP_
#define _TEMPLATE_HPP_

#include <string>

using std::string;

namespace Vm {

class Controller;

class Template {

	friend class Controller;

private:
	string _name;
	string _kernelPath;
	string _rootfsPath;

	Template(
		const string& name,
		const string& kernelPath,
		const string& rootfsPath)
		: _name(name),
		_kernelPath(kernelPath),
		_rootfsPath(rootfsPath) { }

public:
	string getName() {
		return _name;
	}

};

}

#endif // _TEMPLATE_HPP_
