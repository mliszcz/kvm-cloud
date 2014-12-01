/*
 * Thread.hpp
 *
 *  Created on: Jun 15, 2013
 *      Author: michal
 */

#ifndef THREAD_HPP_
#define THREAD_HPP_

#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>

namespace Thread
{

enum class DetachState { Detached, Joinable };
enum class CancelType { Async, Deferred };

template<DetachState detachState, CancelType cancelType>
class Thread
{
private:

	pthread_t threadID = 0;

	static void* threadStarter(void* callerThis)
	{
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);

		if(cancelType == CancelType::Async)
			pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);
		else
			pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, nullptr);


		pthread_cleanup_push(threadCleanup, callerThis);

		Thread* p = (Thread*) callerThis;
		p->run();

		pthread_cleanup_pop(1);
		return nullptr;
	}

	static void threadCleanup(void* callerThis)
	{
		Thread* p = (Thread*) callerThis;

		if(detachState == DetachState::Detached)
			delete p;
	}

public:

	pthread_t getTID() { return threadID; }

	virtual void run() = 0;

	Thread() = default;

	virtual ~Thread() { }

	void start()
	{

		pthread_attr_t attr;
		pthread_attr_init(&attr);

		if(detachState == DetachState::Detached)
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		else
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

		pthread_create(&threadID, &attr, Thread::threadStarter, this);
	}

	void cancel()
	{
		pthread_cancel(threadID);
	}

	void join()
	{
		pthread_join(threadID, nullptr);
	}
};

}

#endif /* THREAD_HPP_ */
