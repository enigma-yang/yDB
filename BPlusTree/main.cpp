#include <iostream>
#include <mutex>
#include <thread>
#include <pthread.h>
#include <semaphore.h>

#include "rtm.h"
#include "BPlusTree.hpp"

/* macro */
#define NUM_THREAD 4
#define WORK_PER_THD 1000000


/* global variable */
int num;
BPlusTree bt;

void worker() {
    for (int i = 0; i < WORK_PER_THD; i++) {
        cout << *(long*)bt.get(26) << endl;
    }
}

int main() {

    long a = 123, b = 642;

    cout << &a << endl;

    bt.put(26, &a);
    bt.put(29, &b);

    std::thread* thd[NUM_THREAD];
    for (int i = 0; i < NUM_THREAD; i++)
        thd[i] = new std::thread(worker);
    for (int i = 0; i < NUM_THREAD; i++)
        thd[i]->join();
    //std::cout << "Final value: " << num << std::endl;
}


