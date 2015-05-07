#include <iostream>
#include <mutex>
#include <thread>
#include <pthread.h>
#include <semaphore.h>

#include "rtm.h"


/* macro */
#define NUM_THREAD 4
#define WORK_PER_THD 1000000


/* global variable */
int num;
sem_t lock;


void worker() {
	for (int i = 0; i < WORK_PER_THD; i++) {
		if (_xbegin() == _XBEGIN_STARTED) {
			int lock_free;
			sem_getvalue(&lock, &lock_free);
			if (lock_free) {
				num++;
			} else {
				_xabort(1);
			}
			_xend();
		} else {
			sem_wait(&lock);
			num++;
			sem_post(&lock);
		}
	}
}

int main() {
	sem_init(&lock, 0, 1);
	std::thread* thd[NUM_THREAD];
	for (int i = 0; i < NUM_THREAD; i++)
		thd[i] = new std::thread(worker);
	for (int i = 0; i < NUM_THREAD; i++)
		thd[i]->join();
	std::cout << "Final value: " << num << std::endl;
}
