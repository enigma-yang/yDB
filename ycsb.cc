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


#define MAXKEY 100

volatile bool running;

YCSBWorker::YCSBWorker(SpinBarrier* startBarrier, YDb* db) {
	this->startBarrier= startBarrier;
	this->db = db;
	this->thread = new std::thread(&YCSBWorker::worker, this);
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
		assert(false);
		numAbort++;
	}

	delete txn;
}

void YCSBWorker::txnUpdate(long k, long v) {
	Txn* txn = db->newTxn();

	txn->write(k, (char *)&v, sizeof(v));

	if (txn->commit() == true) {
		numCommit++;
	} else {
		numAbort++;
		assert(false);
	}

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
		op = rand() % 2;
		switch (op) {
		case 0:
			// read txn
			k = rand() % MAXKEY;
			txnRead(k);
			break;
		case 1:
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
	int numSecs = 5;
	
	/* load data */
	YCSBLoader loader(&ydb);
	loader.load();

	/* create workers, run workload */
	running = true;
	SpinBarrier startBarrier(nthreads+1);
	std::vector<YCSBWorker*> workers;
	for (int i = 0; i < nthreads; i++) {
		YCSBWorker* w = new YCSBWorker(&startBarrier, &ydb);
		workers.push_back(w);
	}
	// let's go!
	startBarrier.countDown();
	sleep(numSecs);
	running = false;

	/* collect stats */
	int numTxn = 0;
	for(std::vector<YCSBWorker*>::iterator it = workers.begin(); it != workers.end(); ++it) {
		numTxn += (*it)->numCommit;
	}

	std::cout << "tps :" << numTxn * 1.0 / numSecs;

	return 0;
}
