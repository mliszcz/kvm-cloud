
#ifndef _COOKIE_HPP_
#define _COOKIE_HPP_

#include <string>
#include <iostream>
#include <fstream>

#include "Exception.hpp"

using std::string;
using std::ifstream;
using std::ofstream;

namespace Util {

class Cookie {

public:

	template <typename T>
	static void write(string cookie, T data) {
		ofstream(cookie) << data;
	}

	template <typename T>
	static T read(string cookie) {
		T data;
		ifstream fin(cookie);
		if (!fin) throw Exception("Cookie::read(string)", "file does not exist");
		fin >> data;
		return data;
	}
};

template <>
string Cookie::read(string cookie) {
	string data;
	ifstream fin(cookie);
	if (!fin) throw Exception("Cookie::read(string)", "file does not exist");
	std::getline(fin, data);
	return data;
}

}

#endif // _COOKIE_HPP_
