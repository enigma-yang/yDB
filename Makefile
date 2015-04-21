all:
	g++ -std=c++11  ydb.cc txn.cc ycsb.cc -o ycsb -lpthread
clean:
	rm ycsb
