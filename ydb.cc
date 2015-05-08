#include <iostream>

#include "ydb.h"

void YDb::put(long k, Record* v) {
	bptree.put(k, v);
}

Record* YDb::get(long k) {
	void *v = bptree.get(k);
	return (Record *)v;
}

void YDb::remove(long k) {
	// TODO
}

Txn* YDb::newTxn() {
	return new Txn(this);
}
