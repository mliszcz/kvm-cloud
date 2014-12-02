
#ifndef _COOKIE_HPP_
#define _COOKIE_HPP_

#include <string>
#include <fstream>

using std::string;
using std::ifstream;
using std::ofstream;

namespace Util {

class Cookie {

	template <typename T>
	static void write(string cookie, T data) {
		ofstream(cookie) << data;
	}

	template <typename T>
	static T read(string cookie) {
		T data;
		try { ifstream(cookie) >> data; }
		catch(...) { }
		return data;
	}
};

}

#endif // _COOKIE_HPP_
