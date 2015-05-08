all:
	g++ -g -std=c++11  bptree.cc lock.cc txn.cc ydb.cc ycsb.cc -o ycsb -lpthread
test:
	g++ -g -std=c++11  bptree.cc lock.cc txn.cc ydb.cc ycsb2.cc -o ycsb -lpthread
clean:
	rm ycsb
