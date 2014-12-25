
#ifndef _HELPERS_HPP_
#define _HELPERS_HPP_

#include <string>
#include <utility>

#include "Exception.hpp"

using namespace std;

namespace Util {

class Helpers {

public:

	static pair<string, int> explodeAddress(const string& addr) {
		return make_pair(
			addr.substr(0, addr.find(":")),
			stoi(addr.substr(addr.find(":")+1))
		);
	}

	template <typename ... Args>
	static std::string printf(const std::string& format, Args ... args) {
		char buffer[1024];
		sprintf(buffer, format.c_str(), std::forward<Args>(args) ...);
		return std::string(buffer);
	}
};

}

#endif // _HELPERS_HPP_
