#ifndef TXN_H
#define TXN_H

#include <mutex>
#include <map>

/* transaction class */
class Txn {
public:
	Txn(std::mutex *db_lock);
	/* start a transaction */
	void txn_begin();

	/* commit a transaction */
	// FIXME return void because txns are serialized, may change it
	void txn_commit();
private:
	std::mutex *db_lock;
};

#endif /* TXN_H */
