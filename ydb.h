/**
 * ydb storage layer 
 */
#ifndef YDB_H
#define YDB_H

#include "common.h"
#include "bptree.h"
#include <map>


class Txn;
class YDb;

class YDb {
	friend class YCSBLoader;
private:
	BPlusTree bptree;
public:
	void put(long k, Record* v);
	Record* get(long k, Stat *stat);
	void remove(long k);
	Txn* newTxn();
};

/* transaction class */
class Txn {
public:
	static Lock *lock;

	Txn(YDb *db);

	/* in-transaction operation */
	void read(long k, char *buf, int size, Stat* stat);
	void write(long k, char *buf, int size, Stat* stat);

	/* commit a transaction, return whether it succeeds */
	bool commit(Stat* stat);
	
	/* restart a transaction */
	void reuse();

private:
	// <pointer to record, version number of record at the time of read>
	std::map<Record*, int> readSet;
	// <pinter to record, new value to write>
	std::map<long, Record*> writeSet;
	// FIXME free value buffers if transaction aborts
	std::map<Record*, char*> writeValueSet;
	YDb *db;
};

#endif /* YDB_H */
