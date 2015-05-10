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
	std::thread* thread;
	Stat stat;

	YCSBWorker(SpinBarrier *startBarrier, YDb *db, int id);
	void txnCreate(long k, long v);
	void txnRead(long k);
	void txnUpdate(long k, long v);
	int txnRemove(long k);
	void worker();
	inline int fastrand();

private:
	SpinBarrier *startBarrier;
	YDb *db;
	int id;
	int g_seed;
	Txn *readTxn;
	Txn *updateTxn;;
};

#endif /* YCSB_H */
