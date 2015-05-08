/**
 * ycsb benchmark
 */
#include <iostream>
#include <pthread.h>
#include <time.h>
#include <thread>
#include <vector>

#include <unistd.h>

#include "ydb.h"
#include "ycsb.h"
#include "spinbarrier.h"


#define MAXKEY 10000000

bool running;

YCSBWorker::YCSBWorker(SpinBarrier* startBarrier, YDb* db, int id) {
	this->startBarrier= startBarrier;
	this->db = db;
	this->numCommit = 0;
	this->numAbort = 0;
	this->numRTM = 0;
	this->numRTMAbort = 0;
	this->id = id;
	this->thread = new std::thread(&YCSBWorker::worker, this);

	// bind core
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(id, &cpuset);
	pthread_setaffinity_np(thread->native_handle(), sizeof(cpuset), &cpuset);
	std::cout << "on core " << id << std::endl;
}


// TODO to be implemented
void YCSBWorker::txnCreate(long k, long v) {
}

void YCSBWorker::txnRead(long k) {
	// FIXME will this leak memory?
	Txn* txn = db->newTxn();

	long v;
	txn->read(k, (char *)&v, sizeof(v));

	if (txn->commit() == true) {
		numCommit++;
	} else {
		numAbort++;
	}

	numRTM += 2;
	numRTMAbort += txn->abortCnt;
	delete txn;
}

void YCSBWorker::txnUpdate(long k, long v) {
	Txn* txn = db->newTxn();

	txn->write(k, (char *)&v, sizeof(v));

	if (txn->commit() == true) {
		numCommit++;
	} else {
		numAbort++;
	}

	numRTM += 1;
	numRTMAbort += txn->abortCnt;
	delete txn;
}

// TODO to be implemented
int YCSBWorker::txnRemove(long k) {
}


void YCSBWorker::worker() {
	startBarrier->countDown();
	startBarrier->waitFor();
	int op;
	long k, v;
	while (running) {
		op = rand() % 5;
		switch (op) {
		case 0:
		case 1:
		case 2:
		case 3:
			// read txn
			k = rand() % MAXKEY;
			txnRead(k);
			break;
		case 4:
			k = rand() % MAXKEY;
			v = rand() % MAXKEY;
			txnUpdate(k, v);
			break;
		}
	}
}

void YCSBLoader::load() {
	// FIXME experiment simple workload
	for (int i = 0; i < MAXKEY; i++) {
		Record *r = new Record();
		r->value = new char[sizeof(long)];
		r->ver = 0;
		db->put(i, r);
	}
}


int main(int argc, char **argv) {
	YDb ydb;
	int nthreads = 4;
	int numSecs = 10;
	int id = 0;
	
	/* load data */
	YCSBLoader loader(&ydb);
	loader.load();

	/* create workers, run workload */
	running = true;
	SpinBarrier startBarrier(nthreads+1);
	std::vector<YCSBWorker*> workers;
	for (int i = 0; i < nthreads; i++) {
		YCSBWorker* w = new YCSBWorker(&startBarrier, &ydb, id++);
		workers.push_back(w);
	}
	// let's go!
	startBarrier.countDown();
	sleep(numSecs);
	running = false;

	/* collect stats */
	int numCommit = 0;
	int numAbort = 0;
	int numRTM = 0;
	int numRTMAbort = 0;
	for(std::vector<YCSBWorker*>::iterator it = workers.begin(); it != workers.end(); ++it) {
		(*it)->thread->join();
		numCommit += (*it)->numCommit;
		numAbort += (*it)->numAbort;
		numRTM += (*it)->numRTM;
		numRTMAbort += (*it)->numRTMAbort;
	}

	std::cout << "ops :" << numCommit * 1.0 / numSecs << std::endl;
	std::cout << "aps :" << numAbort * 1.0 / numSecs << std::endl;
	std::cout << "rtm :" << numRTM << std::endl;
	std::cout << "rtmAbort :" << numRTMAbort << std::endl;

	return 0;
}
