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
#define CLK_PER_MS 3400000

bool running;

YCSBWorker::YCSBWorker(SpinBarrier* startBarrier, YDb* db, int id) {
	this->startBarrier= startBarrier;
	this->db = db;
	this->id = id;
	g_seed = id;
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

	unsigned long beginTsc = rdtsc();
	txn->read(k, (char *)&v, sizeof(v), &stat);

	if (txn->commit(&stat) == true) {
		stat.numCommit++;
	} else {
		stat.numAbort++;
	}
	unsigned long endTsc = rdtsc();
	stat.latency += (endTsc - beginTsc)*1.0 / CLK_PER_MS;
}

void YCSBWorker::txnUpdate(long k, long v) {
	Txn* txn = updateTxn;
	txn->reuse();

	unsigned long beginTsc = rdtsc();
	txn->write(k, (char *)&v, sizeof(v), &stat);

	if (txn->commit(&stat) == true) {
		stat.numCommit++;
	} else {
		stat.numAbort++;
	}
	unsigned long endTsc = rdtsc();
	stat.latency += (endTsc - beginTsc)*1.0 / CLK_PER_MS;
}

// TODO to be implemented
int YCSBWorker::txnRemove(long k) {
}

inline int YCSBWorker::fastrand() {
	g_seed = (214013 * g_seed + 2531011);
	return (g_seed>>16)&0x7FFF; 
}

void YCSBWorker::worker() {
	startBarrier->countDown();
	startBarrier->waitFor();
	long op;
	long k, v;
	while (running) {
		op = fastrand();
		switch ((op+id)%5) {
		case 0:
		case 1:
		case 2:
		case 3:
			// read txn
			k = fastrand();
			txnRead(k);
			break;
		case 4:
			k = fastrand();
			v = k;
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
	int nthreads = 3;
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
	Stat stat;
	for(std::vector<YCSBWorker*>::iterator it = workers.begin(); it != workers.end(); ++it) {
		(*it)->thread->join();
		stat.numCommit += (*it)->stat.numCommit;
		stat.numAbort += (*it)->stat.numAbort;
		stat.numRTMTxn += (*it)->stat.numRTMTxn;
		stat.numRTMAbortTxn += (*it)->stat.numRTMAbortTxn;
		stat.numRTMTree += (*it)->stat.numRTMTree;
		stat.numRTMAbortTree += (*it)->stat.numRTMAbortTree;
		stat.latency += (*it)->stat.latency;
	}

	std::cout << "ops :" << stat.numCommit * 1.0 / numSecs << std::endl;
	std::cout << "aps :" << stat.numAbort * 1.0 / numSecs << std::endl;
	std::cout << "rtmTxn:" << stat.numRTMTxn << std::endl;
	std::cout << "rtmAbortTxn :" << stat.numRTMAbortTxn << std::endl;
	std::cout << "rtmTree :" << stat.numRTMTree << std::endl;
	std::cout << "rtmAbortTree :" << stat.numRTMAbortTree << std::endl;
	std::cout << "latency:" << stat.latency / (stat.numCommit+stat.numAbort) << std::endl;

	return 0;
}
