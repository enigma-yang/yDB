/**
 * ycsb benchmark
 */
#include <iostream>
#include <pthread.h>
#include <time.h>
#include <thread>
#include <vector>

#include <unistd.h>

#include "txn.h"
#include "common.h"
#include "ydb.h"
#include "spinbarrier.h"


#define MAXKEY 100

volatile bool running;

class ycsb_worker {
private:
	int num_commit;
	int num_abort;
	spin_barrier *start_barrier;
	YDb *db;
	std::thread *thread;
	
public:
	ycsb_worker(spin_barrier *start_barrier, YDb *db) {
		this->start_barrier = start_barrier;
		this->db = db;
		this->thread = new std::thread(&ycsb_worker::worker, this);
	}

	void count_commit() {
		num_commit++;
	}

	void txn_create(KTYPE k, VTYPE v) {
		Txn txn = db->new_txn();
		txn.txn_begin();

		db->put(k, v); 
		txn.txn_commit();
		count_commit();
	}

	VTYPE txn_read(KTYPE k) {
		Txn txn = db->new_txn();
		txn.txn_begin();

		VTYPE retval = db->get(k);

		txn.txn_commit();
		count_commit();

		return retval;
	}

	void txn_update(KTYPE k, VTYPE v) {
		Txn txn = db->new_txn();
		txn.txn_begin();

		db->put(k, v);

		txn.txn_commit();
		count_commit();
	}

	int txn_remove(KTYPE k) {
		Txn txn = db->new_txn();
		txn.txn_begin();

		db->remove(k);

		txn.txn_commit();
		count_commit();
	}

	int get_num_commit() {
		return num_commit;
	}

	void worker() {
		start_barrier->count_down();
		start_barrier->wait_for();
		while (running) {
			int op = rand() % 2;
			switch (op) {
				case 0:
				{
					// read txn
					int k = rand() % MAXKEY;
					txn_read(k);
					break;
				}
				case 1:
				{
					int k = rand() % MAXKEY;
					int v = rand() % MAXKEY;
					txn_update(k, v);
					break;
				}
			}
		}
	}
};


int main(int argc, char **argv) {
	YDb ydb;
	// FIXME magic number
	int nthreads = 4;
	// FIXME magic number
	int num_secs = 5;
	
	/* load data */
	// FIXME experiment simple workload
	for (int i = 0; i < MAXKEY; i++)
		ydb.store[i] = i;

	/* create workers, run workload */
	running = true;
	spin_barrier start_barrier(nthreads+1);
	std::vector<ycsb_worker *> workers;
	for (int i = 0; i < nthreads; i++)
		workers.push_back(new ycsb_worker(&start_barrier, &ydb));
	// let's go!
	start_barrier.count_down();
	sleep(num_secs);
	running = false;

	/* collect stats */
	int num_txn = 0;
	for(std::vector<ycsb_worker *>::iterator it = workers.begin(); it != workers.end(); ++it) {
		num_txn += (*it)->get_num_commit();
	}

	std::cout << "tps :" << num_txn * 1.0 / num_secs;

	return 0;
}
