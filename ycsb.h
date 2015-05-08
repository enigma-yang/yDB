#ifndef YCSB_H
#define YCSB_H


#include "spinbarrier.h"
#include "ydb.h"

class YCSBLoader {
public:
	YCSBLoader(YDb *db) : db(db) {}
	void load();
private:
	YDb *db;
};

class YCSBWorker {
public:
	unsigned long numCommit;
	unsigned long numAbort;
	unsigned long numRTM;
	unsigned long numRTMAbort;
	std::thread* thread;

	YCSBWorker(SpinBarrier *startBarrier, YDb *db, int id);
	void txnCreate(long k, long v);
	void txnRead(long k);
	void txnUpdate(long k, long v);
	int txnRemove(long k);
	void worker();

private:
	SpinBarrier *startBarrier;
	YDb *db;
	int id;
	Txn *readTxn;
	Txn *updateTxn;;
};

#endif /* YCSB_H */
