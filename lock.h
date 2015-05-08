#ifndef LOCK_H
#define LOCK_H


#include <semaphore.h>

/* wrapper class of sem_t */
class Lock {
public:
	Lock();
	void lock();
	void unlock();
	bool isLocked();
private:
	sem_t sem;
};


#endif /* LOCK_H */
