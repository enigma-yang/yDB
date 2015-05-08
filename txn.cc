#include "ydb.h"


Lock* Txn::lock = new Lock();

Txn::Txn(YDb *db) {
	this->db = db;
}

bool Txn::commit() {
RTM_EXEC(lock,
	// validate phase
	for (std::map<Record*, int>::iterator it = readSet.begin(); it != readSet.end(); it++)
		if (it->first->ver != it->second)
			_xabort(1);

	for (std::map<long, Record*>::iterator it = writeSet.begin(); it != writeSet.end(); it++)
		if (it->second->ver < 0)
			_xabort(1);

	// write phase
	for (std::map<Record*, char*>::iterator it = writeValueSet.begin(); it != writeValueSet.end(); it++) {
		delete it->first->value;
		it->first->value = it->second;
		it->first->ver++;
	}
)

	return true;
}

/* FIXME assuem key exists */
void Txn::read(long k, char *buf, int size) {
	void *vp;
	if (writeSet.find(k) != writeSet.end()) {
		vp = writeValueSet[writeSet[k]];
	} else {
		int ver;
		void *value;
		Record *rp = db->get(k);

		RTM_EXEC(lock,
				ver = rp->ver;
				char *value = rp->value;
				for (int i = 0; i < size; i++)
					buf[i] = value[i];
		);
		readSet[rp] = ver;
	}
}

void Txn::write(long k, char *buf, int size) {
	char *value = new char[size];
	for (int i = 0; i < size; i++)
		value[i] = buf[i];

	Record *rp;
	if (writeSet.find(k) != writeSet.end())
		rp = writeSet[k];
	else
		rp = db->get(k);
	writeValueSet[rp] = value;
}

void Txn::restart() {
	readSet.clear();
	writeSet.clear();
}
