#include <iostream>

#include "common.h"
#include "ydb.h"
#include "txn.h"	

void YDb::put(KTYPE k, VTYPE v) {
	store[k] = v;
}

VTYPE YDb::get(KTYPE k) {
	// FIXME what if k not exist
	return store[k];
}

void YDb::remove(KTYPE k) {
	store.erase(k);
}

Txn YDb::new_txn() {
	return Txn(&mtx);
}
