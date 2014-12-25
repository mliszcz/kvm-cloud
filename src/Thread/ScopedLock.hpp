
#ifndef _SCOPEDLOCK_HPP_
#define _SCOPEDLOCK_HPP_

#include <memory>

#include "Mutex.hpp"

using std::shared_ptr;

namespace Thread {

	class ScopedLock {

	private:
		shared_ptr<Mutex> mutex;

	public:
		ScopedLock(shared_ptr<Mutex> _mutex)
			: mutex(_mutex) { mutex->lock(); }

		virtual ~ScopedLock() {
			mutex->unlock();
		}
	};
}

#endif // _SCOPEDLOCK_HPP_
