all:
	g++ -std=c++11  lock.cc txn.cc ydb.cc ycsb.cc -o ycsb -lpthread
clean:
	rm ycsb
