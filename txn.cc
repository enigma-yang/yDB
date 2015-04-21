#include "txn.h"

Txn::Txn(std::mutex *db_lock) {
	this->db_lock = db_lock;
}

void Txn::txn_begin() {
	db_lock->lock();
}

void Txn::txn_commit() {
	db_lock->unlock();
}
