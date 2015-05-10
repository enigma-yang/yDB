###yDB: high-performance and scalable in-memory key-value store based on RTM
Team member: Zhiyuan Yang, Zhizhou Yang

###SUMMARY
We implemented an in-memory key-value store called yDB based on Intel RTM (Restricted Transactional Memory) and optimistic concurrency control[2]. yDB is high-performance and scalable on multicore machine. It can achieve the same performance of a state-of-art in-memory database called Silo (SOSP'13)[3] on a benchmark derived from YCSB. Specifically, it achieves 7.2 million tps using 4 cores with 8 hardware threads. Also, it scales well from 1 threads to 8 threads on 4-core CPU with hyperthreading.

###BACKGROUND
Traditionally, databases use fine-grained locks and atomic operations to do synchronization and transaction. While it can provide good performance and reasonable scalability, it's hard to make sure the correctness and the resulting code is very complex.  

Recently, Intel introduces restricted transactional memory (RTM) support in Haswell processors. Using RTM instructions, one can transactionally execute a piece of code, which is a much easier way to do synchronization. RTM doesn't guarantee progress but let user do it using fallback handler. Typically, in fallback handler, user may either retry the transaction or grab a coarse grained lock and then perform transaction.
```
RETRY:
	_xbegin(ABORT)
	do_transaction()
	_xend()
	return

ABORT:
	if numRetry < threshold
		goto RETRY
	else
		lock(l)
		do_transaction()
		unlock(l)
	return
```

###THE CHALLENGE
Although RTM is promising in synchronizaiton problem, it is challenging to use it to implement an in-memory database with high performance and good scalability. 

Research results[1] show that RTM is limited by the working set because processors use cache implement it. Any transaction with working set larger than cache(L1) size will be aborted, so it's critical to reduce the working set size. Also, coarse-grained lock in fallback handlers is a performance-bottleneck, so it's critical to make sure to reduce the possibility of RTM abort and use fallback handler less frequently.

###DESIGN
DBX[1] has a simple design and achieves great performance, so we followed its design. We separate the database into two layers: storage layer and transaction layer. The following graph illustrates the architecture(graph is from [1]):  

<a href="url"><img src="https://raw.githubusercontent.com/Zhiyuan-Yang/yDB/occ/design1.png?token=AHtqN2wYvNuJuL_wYqxUoi6uigNyUm_Mks5VVo9ywA%3D%3D" align="right" height="340"></a>

Storage layer is responsible to provide simply put/get interface, and it's implemented using B+ tree. It's hard to apply fine-grained lock on B+ tree correctly, but with RTM, things become much easier. We can simply enclose B+ tree operations with _xbegin() and _xend() and provide a fallback handler which graps coarse-grain lock and then performs the operations. Actually, the data structure of storage layer doesn't matter as long as put/get interface is offered. For example, we can also use hashmap as storage layer(but make it hard to implement scan operation).  
Transaction layer is reponsible to provide transaction ability, and it's implemented using optimistic concurrency control. If using pessimistic concurrency control, both transaction execution and transaction commit need to be syncrhonized which causes larger working set and longer critical section, which make RTM more likely to abort. With optimisitic concurrency control, transaction execution and transaction commit are separated, and only transaction commit need to be synchronized.  


In transaction execution, we track the read set and write set of transactions. To read a data entry, we first get the data from write set if we already write this entry, otherwise we read the data from storage layer with its version number. Notice we use RTM to ensure we read both data and version number atomically. To write a data entry, we just buffer it in write set and then apply all writes in transaction commit. See the following graph[1] for algorithm of read interface offered by transaction layer. 

<a href="url"><img src="https://raw.githubusercontent.com/Zhiyuan-Yang/yDB/occ/d2.png?token=AHtqN_69vtOKhbnZaAYZqR2aBzrX6XfHks5VVo-awA%3D%3D" align="right" width="420"></a>

In transaction commit, we first check whether data in read set has been updated via recorded version number, and then check whether data in write set has been deleted. If anything gos wrong, the transaction will be aborted. Otherwise, we apply all writes of this transaction to storage layer and update version numbers of data. Then whole commit phase need to be guarded by RTM. See the following graph[1] for algorithm of transaction commit.  

<a href="url"><img src="https://raw.githubusercontent.com/Zhiyuan-Yang/yDB/occ/d3.png?token=AHtqN5pOLhWpt10bxNEsF5xK2l4O0_hBks5VVo-owA%3D%3D" align="left" width="420"></a>


###EVALUATION
We implemented yDB using C++. As in Silo and DBX, we use fixed-length key(8 bytes). Storage layer manages <key, pointer> pair, in which pointer points to a recordthat contains version number and pointer to real data. This implementation adds another layer of indirection, but make storage layer fairly simple and decoupled from data type. B+ tree node is 256 bytes with fanout of 15. Transactions are supported via store procedure. Insert/read/update operations are supported, and delete are partially supported because we simply mark the deleted data but lack the garbage collector now. Scan can be supported but not supported now.

We tested yDB on a machine with Intel(R) Core(TM) i7-4770 CPU @ 3.40GHz and 16 GB RAM. The benchmark we used is derived from DBX's and Silo's benchmark which are derived from YCSB benchmark. The modifications are:  
(1) Transactions are issued via function call to eliminate the network bottleneck  
(2) Data are bulk-loaded before testing, and only read and write transactions are used in testing 
(3) Read-write ratio is 80:20  
(4) The number of records is 10M. This number is used because Silo is tested using 160M data on a machine with 256 GB RAM. Since we only have machine with 16 GB RAM, we reduce the data size by a factor of 256/16=16 which is 10M.
(5) The size of real data is 8bytes. The size of real data doesn't really matters because what yDB really handles is pointer not real data. Using larger data size only causes more memory consumption and unnecessary extra stress the memory allocator. Also, larger data size may make memory copy between test worker and yDB become a potential bottleneck.
(6) Latency metrics don't make sense here. Since there is no durability support currently and all operations are in memory, so the latency is always very very low (less than 1 micro-seconds).

The result is in following table and chart. Chart from DBX is also attached for comparison.(The test of DBX is performed on a similar machine with 32 GB RAM[1])

Number of Threads | Throughput (ops)
------------ | -------------
1	| 1.5E+06
2	| 2.9E+06
4	| 5.5E+06
6	| 6.4E+06
8	| 7.2E+06

![chart2](https://raw.githubusercontent.com/Zhiyuan-Yang/yDB/occ/chart2.png?token=AHtqN77Ok8P7OSXOdewGzm4Wf7Q1Vq42ks5VVoQLwA%3D%3D)

<a href="url"><img src="https://raw.githubusercontent.com/Zhiyuan-Yang/yDB/occ/chart1.png?token=AHtqN19LmOKlJB_kHZFtf-f_PU2MjjUWks5VVoPwwA%3D%3D" height="331" width="480" ></a>

For performance, it achieves about same performance of Silo(B+ tree in Silo), but it's not as good as DBX(RTM B+ tree). There are two reasons. First, in derived benchmark, 80% of transactions are read transactions, and read transactions are read-only. DBX uses snapshot of data to support fast read-only transactions which requires no commit phase, while yDB use normal read-operation and commit phase to perform read transactions. This optimization is reported to improve performance dramatically. Actually we've implemented DBX's read-only transactions but never got the same improvement, which means the performance is very implementation-dependent. Second, DBX has better tuning on RTM retry threshold. the paper says DBX use different threshold for different kinds of RTM abort and for different height of tree, while yDB use the same threshold for all RTM transactions. Also, the tuning of this threshold is very important. Initially we used 3 as threshold, but got very poor scalability on 5-8 threads. Then we tuned it to 100 and got good scalability on 1-8 threads.

For scalability, we can see yDB scales well from 1-8 threads. Scalability test can only be performed using 1-8 threads due to hardware limit. Currently Intel processors that support RTM can have at most 4 cores with hyperthreading. So the scalability result here is constraint in this scope and may not be true with more than 4 cores.

###REFERENCES

[1] Wang, Zhaoguo, et al. "Using restricted transactional memory to build a scalable in-memory database." Proceedings of the Ninth European Conference on Computer Systems. ACM, 2014.

[2] Kung, Hsiang-Tsung, and John T. Robinson. "On optimistic methods for concurrency control." ACM Transactions on Database Systems (TODS) 6.2 (1981): 213-226.

[3] Tu, Stephen, et al. "Speedy transactions in multicore in-memory databases." Proceedings of the Twenty-Fourth ACM Symposium on Operating Systems Principles. ACM, 2013.
