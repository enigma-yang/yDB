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



YCSBWorker::YCSBWorker(SpinBarrier* startBarrier, YDb* db, int id) {
	this->startBarrier= startBarrier;
	this->db = db;
	this->id = id;
	this->thread = new std::thread(&YCSBWorker::worker, this);
	this->readTxn = db->newTxn();
	this->updateTxn = db->newTxn();

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
	Txn* txn = readTxn;
	txn->reuse();

	long v;
	txn->read(k, (char *)&v, sizeof(v), &stat);

	if (txn->commit(&stat) == true) {
		stat.numCommit++;
	} else {
		stat.numAbort++;
	}
}

void YCSBWorker::txnUpdate(long k, long v) {
	Txn* txn = updateTxn;
	txn->reuse();

	txn->write(k, (char *)&v, sizeof(v), &stat);

	if (txn->commit(&stat) == true) {
		stat.numCommit++;
	} else {
		stat.numAbort++;
	}
}

// TODO to be implemented
int YCSBWorker::txnRemove(long k) {
}


void YCSBWorker::worker() {
	startBarrier->countDown();
	startBarrier->waitFor();
	int op;
	long k=1, v;
	for (int i = 0; i < 1000000; ) {
		Txn *txn;
		txn = db->newTxn();
		txn->read(k, (char *)&v, sizeof(v), &stat);
		v++;
		txn->write(k, (char *)&v, sizeof(v), &stat);
		if (txn->commit(&stat))
			i++;
		delete txn;
	}
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
	int id = 0;
	
	/* load data */
	YCSBLoader loader(&ydb);
	loader.load();

	/* create workers, run workload */
	SpinBarrier startBarrier(nthreads+1);
	std::vector<YCSBWorker*> workers;
	for (int i = 0; i < nthreads; i++) {
		YCSBWorker* w = new YCSBWorker(&startBarrier, &ydb, id++);
		workers.push_back(w);
	}
	// let's go!
	startBarrier.countDown();

	for(std::vector<YCSBWorker*>::iterator it = workers.begin(); it != workers.end(); ++it)
		(*it)->thread->join();
		
	Txn* txn = ydb.newTxn();
	long v;
	Stat stat;
	txn->read(1, (char *)&v, sizeof(v), &stat);
	txn->commit(&stat);
	std::cout << v << std::endl;

	return 0;
}
