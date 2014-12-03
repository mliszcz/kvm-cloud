
#ifndef _TEMPLATE_HPP_
#define _TEMPLATE_HPP_

#include <string>

using std::string;

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

};

}

#endif // _TEMPLATE_HPP_
