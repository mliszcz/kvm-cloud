
#ifndef _CONTROLLERPROXY_HPP_
#define _CONTROLLERPROXY_HPP_

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

class ControllerProxy {
	
private:
	shared_ptr<Net::Socket> socket;
};

}

#endif // _CONTROLLERPROXY_HPP_
