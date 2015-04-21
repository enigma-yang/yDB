/**
 * ydb storage layer 
 */
#ifndef YDB_H
#define YDB_H

#include <map>
#include <mutex>
#include "common.h"
#include "txn.h"

class YDb {
private:
	std::mutex mtx;
public:
	std::map<KTYPE, VTYPE> store;
	void put(KTYPE k, VTYPE v);
	VTYPE get(KTYPE k);
	void remove(KTYPE k);
	Txn new_txn();
};

#endif /* YDB_H */
