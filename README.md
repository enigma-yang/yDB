###yDB: high-performance and scalable in-memory kv store based on RTM
Team memeber: Zhiyuan Yang, Zhizhou Yang

###SUMMARY
We implemented an in-memory kv store called yDB based on Intel RTM(Restricted Transactional Memory) and optimistic concurrency control[2]. yDB is high-performance and scalable on multicore machine, and it can achieve the same performance of Silo(SOSP'13)[3] on a benchmark derived from YCSB. Specificaly, it achieves 4.8 million transaction per second using 4 cores.


###BACKGROUND
Traditionally, databases use fine-grained locks and atomic operations to do synchronization and transaction. While it can provide good performance and reasonable scalability, it's hard to make sure the correctness and the resulting code is very complex.  

Recently, Intel introduces restricted transactional memory (RTM) support in Haswell processors. Using RTM instructions, one can transactionally execute a part of code and explicitly abort in the middle of transactional execution, which provides a much easier way to do synchronization.

###THE CHALLENGE
Although RTM is a promising in synchronizaiton problem, it is challenging to use it to implement a in-memory database with high performance and good scalability. 

Research results show that RTM is limited by the working set because processors use cache implement it. Any transaction with working set larger than cache size will be aborted, so it's critical to reduce the working set size. Also, RTM doesn't guarantee progress but let user do it using fallback handler. Fallback handlers typically use coarse grained lock which is always performance-bottleneck, so it's critical to make sure to reduce RTM abort and use fallback handler less frequently.

###DESIGN
DBX[1] has a simple design and achieves great performance, so we followed its design. We separate the database into two layers: storage layer and transaction layer.  
Storage layer is responsible to provide simply get and put interface, and it's implemented using B+ tree. It's hard to apply fine-grained lock on B+ tree, but with RTM, things become much easier. We can simply enclose B+ tree operations with _xbegin() and _xend() and provide a fallback handler which graps coarse-grain lock and then performs the operations.
Transaction layer is reponsible to provide transaction ability, and it's implemented using optimistic concurrency control. If using pessimistic concurrency control, both transaction execution and transaction commit need to be syncrhonized which causes larger working set and longer critical section. With optimisitic concurrency control, only transaction commit need to be synchronized.  

###EVALUATION
The benchmark we used is derived from YCSB bechmark. The modifications are:
(1) transactions are supported using store procedure
(2) transactions are issued via function call to eliminate the network bottleneck
(3) only read and write transactions are used 
(4) read-write ratio is 80:20  
(5) the number of records is 10M and the size of real data is 8bytes. The size of real data doesn't really matters because what yDB really handles is pointer not real data.

The result is in following table and chart. Chart from DBX is also attached. 

We can see yDB scales well from 1-4 cores. It achieves same performance of Silo, but it's not as good as DBX.

###LIMITATION and FUTURE WORKS
1. Currently yDB doesn't scales well from 5 to 8 threads, but DBX[1] scales well consistently from 1 to 8 threads, we are still working on this problem. 
2. Current implementation only support read and write operations because that's enough to demonstrate its potential, but we are still going to implement other operations such as insert, delete and scan so that we can test it against full version of YCSB. 

###REFERENCES

[1] Wang, Zhaoguo, et al. "Using restricted transactional memory to build a scalable in-memory database." Proceedings of the Ninth European Conference on Computer Systems. ACM, 2014.

[2] Kung, Hsiang-Tsung, and John T. Robinson. "On optimistic methods for concurrency control." ACM Transactions on Database Systems (TODS) 6.2 (1981): 213-226.

[3] Tu, Stephen, et al. "Speedy transactions in multicore in-memory databases." Proceedings of the Twenty-Fourth ACM Symposium on Operating Systems Principles. ACM, 2013.
