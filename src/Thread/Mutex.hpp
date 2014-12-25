
#ifndef MUTEX_HPP_
#define MUTEX_HPP_

#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>

namespace Thread
{

class Mutex
{
private:

	pthread_mutex_t mutex;

public:

	Mutex()
	{
		// mutex = PTHREAD_MUTEX_INITIALIZER;
		mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
	}

	virtual ~Mutex()
	{
		destroy();
	}

	void lock()
	{
		pthread_mutex_lock(&mutex);
	}

	void unlock()
	{
		pthread_mutex_unlock(&mutex);
	}

	void destroy()
	{
		pthread_mutex_destroy(&mutex);
	}
};

}

#endif /* MUTEX_HPP_ */
