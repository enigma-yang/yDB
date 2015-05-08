/**
 * ycsb benchmark: debug correctness
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


volatile bool running;

YCSBWorker::YCSBWorker(SpinBarrier* startBarrier, YDb* db) {
	this->startBarrier= startBarrier;
	this->db = db;
	this->thread = new std::thread(&YCSBWorker::worker, this);
	this->numCommit = 0;
	this->numAbort = 0;
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
		assert(v == k);
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
	long k=1, v;
	//while (running) {
	for (int i = 0; i < 100000; ) {
		Txn *txn;
		txn = db->newTxn();
		txn->read(k, (char *)&v, sizeof(v));
		v++;
		txn->write(k, (char *)&v, sizeof(v));
		if (txn->commit())
			i++;
		delete txn;
	}
//	}
}

void YCSBLoader::load() {
	// FIXME experiment simple workload
	Record *r = new Record();
	r->value = new char[sizeof(long)];
	*(long *)(r->value) = (long)1;
	r->ver = 1;
	db->put(1, r);
}


int main(int argc, char **argv) {
	YDb ydb;
	int nthreads = 4;
	int numSecs = 60;
	
	/* load data */
	YCSBLoader loader(&ydb);
	loader.load();

	/* create workers, run workload */
	//running = true;
	SpinBarrier startBarrier(nthreads+1);
	std::vector<YCSBWorker*> workers;
	for (int i = 0; i < nthreads; i++) {
		YCSBWorker* w = new YCSBWorker(&startBarrier, &ydb);
		workers.push_back(w);
	}
	// let's go!
	startBarrier.countDown();
	//sleep(numSecs);
	//running = false;

	for(std::vector<YCSBWorker*>::iterator it = workers.begin(); it != workers.end(); ++it)
		(*it)->thread->join();
		
	Txn* txn = ydb.newTxn();
	long v;
	txn->read(1, (char *)&v, sizeof(v));
	txn->commit();
	std::cout << v << std::endl;

	return 0;
}
