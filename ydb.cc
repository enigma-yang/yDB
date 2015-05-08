#include <iostream>

#include "ydb.h"

void YDb::put(long k, Record* v) {
	// TODO
}

Record* YDb::get(long k) {
	// TODO
}

void YDb::remove(long k) {
	// TODO
}

Txn* YDb::newTxn() {
	return new Txn(this);
}
