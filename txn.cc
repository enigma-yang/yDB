#include "ydb.h"


Lock* Txn::lock = new Lock();

Txn::Txn(YDb *db) {
	this->db = db;
}

bool Txn::commit(Stat *stat) {
	bool lockedByMe = false;
	int numAbort = 0;
RTM_EXEC2(lock, lockedByMe, numAbort,
	// validate phase
	for (std::unordered_map<Record*, int>::iterator it = readSet.begin(); it != readSet.end(); it++) {
		if (it->first->ver != it->second) {
			if (lockedByMe)
				lock->unlock();
			return false;
		}
	}

	for (std::unordered_map<long, Record*>::iterator it = writeSet.begin(); it != writeSet.end(); it++) {
		if (it->second->ver < 0) {
			if (lockedByMe)
				lock->unlock();
			return false;
		}
	}

	// write phase
	for (std::unordered_map<Record*, char*>::iterator it = writeValueSet.begin(); it != writeValueSet.end(); it++) {
		// FIXME memory leak, but delete would be inefficient
		//delete it->first->value;
		it->first->value = it->second;
		it->first->ver++;
	}
)
	stat->numRTMTxn += 1;
	stat->numRTMAbortTxn += numAbort;

	return true;
}

/* FIXME assuem key exists */
void Txn::read(long k, char *buf, int size, Stat* stat) {
	void *vp;
	if (writeSet.find(k) != writeSet.end()) {
		vp = writeValueSet[writeSet[k]];
	} else {
		int ver;
		Record *rp = db->get(k, stat);
		int numAbort = 0;

		
		RTM_EXEC3(lock, numAbort,
				ver = rp->ver;
				char *value = rp->value;
				for (int i = 0; i < size; i++)
					buf[i] = value[i];
		);
		readSet[rp] = ver;
	}
}

void Txn::write(long k, char *buf, int size, Stat* stat) {
	char *value = new char[size];
	for (int i = 0; i < size; i++)
		value[i] = buf[i];

	Record *rp;
	if (writeSet.find(k) != writeSet.end())
		rp = writeSet[k];
	else
		rp = db->get(k, stat);
	writeValueSet[rp] = value;
}

void Txn::reuse() {
	readSet.clear();
	writeSet.clear();
	writeValueSet.clear();
}
