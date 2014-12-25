
#ifndef _HELPERS_HPP_
#define _HELPERS_HPP_

#include <string>
#include <utility>
#include <map>

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

	template <typename K, typename V>
	static bool mapExists(const std::map<K,V>& map, const K& key) {
		return map.find(key) != map.end();
	}
};

}

#endif // _HELPERS_HPP_
