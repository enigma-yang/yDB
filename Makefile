all:
	g++ -g -std=c++11  bptree.cc lock.cc txn.cc ydb.cc ycsb.cc -o ycsb -lpthread
test:
	g++ -g -std=c++11  bptree.cc lock.cc txn.cc ydb.cc ycsb2.cc -o ycsb -lpthread
clean:
	rm ycsb
perf:
	g++ -g -std=c++11  bptree.cc lock.cc txn.cc ydb.cc ycsb.cc -o ycsb -lpthread
	perf stat -B -e cache-references,cache-misses,cycles,instructions,branches,faults,migrations ./ycsb
