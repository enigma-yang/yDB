#include "lock.h"


Lock::Lock() {
	sem_init(&sem, 0, 1);
}

void Lock::lock() {
	sem_wait(&sem);
}

void Lock::unlock() {
	sem_post(&sem);
}

bool Lock::isLocked() {
	int n;
	sem_getvalue(&sem, &n);
	return n == 0;
}
