###yDB: high-performance and scalable in-memory kv store based on RTM
Team memeber: Zhiyuan Yang, Zhizhou Yang

###SUMMARY
We implemented an in-memory kv store called yDB based on Intel RTM (Restricted Transactional Memory) and optimistic concurrency control[2]. yDB is high-performance and scalable on multicore machine, and it can achieve the same performance of Silo (SOSP'13)[3] on a benchmark derived from YCSB. Specificaly, it achieves 4.8 million transactions per second using 4 cores.


###BACKGROUND
Traditionally, databases use fine-grained locks and atomic operations to do synchronization and transaction. While it can provide good performance and reasonable scalability, it's hard to make sure the correctness and the resulting code is very complex.  

Recently, Intel introduces restricted transactional memory (RTM) support in Haswell processors. Using RTM instructions, one can transactionally execute a piece of code, which is a much easier way to do synchronization.

###THE CHALLENGE
Although RTM is promising in synchronizaiton problem, it is challenging to use it to implement an in-memory database with high performance and good scalability. 

Research results[2] show that RTM is limited by the working set because processors use cache implement it. Any transaction with working set larger than cache size will be aborted, so it's critical to reduce the working set size. Also, RTM doesn't guarantee progress but let user do it using fallback handler. Fallback handlers typically use coarse grained lock which is always performance-bottleneck, so it's critical to make sure to reduce RTM abort and use fallback handler less frequently.

###DESIGN
DBX[1] has a simple design and achieves great performance, so we followed its design. We separate the database into two layers: storage layer and transaction layer.  
Storage layer is responsible to provide simply get and put interface, and it's implemented using B+ tree. It's hard to apply fine-grained lock on B+ tree, but with RTM, things become much easier. We can simply enclose B+ tree operations with _xbegin() and _xend() and provide a fallback handler which graps coarse-grain lock and then performs the operations.
Transaction layer is reponsible to provide transaction ability, and it's implemented using optimistic concurrency control. If using pessimistic concurrency control, both transaction execution and transaction commit need to be syncrhonized which causes larger working set and longer critical section. With optimisitic concurrency control, only transaction commit need to be synchronized.  

<a href="url"><img src="https://raw.githubusercontent.com/Zhiyuan-Yang/yDB/occ/d3.png?token=AHtqN5pOLhWpt10bxNEsF5xK2l4O0_hBks5VVo-owA%3D%3D" align="left" width="410"></a>

<a href="url"><img src="https://raw.githubusercontent.com/Zhiyuan-Yang/yDB/occ/d2.png?token=AHtqN_69vtOKhbnZaAYZqR2aBzrX6XfHks5VVo-awA%3D%3D" align="right" width="410"></a>

<a href="url"><img src="https://raw.githubusercontent.com/Zhiyuan-Yang/yDB/occ/design1.png?token=AHtqN2wYvNuJuL_wYqxUoi6uigNyUm_Mks5VVo9ywA%3D%3D" align="right" height="340"></a>


(The above graphs are from [1])  

###EVALUATION
We tested yDB on machine with Intel(R) Core(TM) i7-4770 CPU @ 3.40GHz. The benchmark we used is derived from YCSB benchmark. The modifications are:  
(1) transactions are supported using store procedure  
(2) transactions are issued via function call to eliminate the network bottleneck  
(3) only read and write transactions are used  
(4) read-write ratio is 80:20  
(5) the number of records is 10M and the size of real data is 8bytes. The size of real data doesn't really matters because what yDB really handles is pointer not real data.  

The result is in following table and chart. Chart from DBX is also attached. 

![chart2](https://raw.githubusercontent.com/Zhiyuan-Yang/yDB/occ/chart2.png?token=AHtqN77Ok8P7OSXOdewGzm4Wf7Q1Vq42ks5VVoQLwA%3D%3D)

<a href="url"><img src="https://raw.githubusercontent.com/Zhiyuan-Yang/yDB/occ/chart1.png?token=AHtqN19LmOKlJB_kHZFtf-f_PU2MjjUWks5VVoPwwA%3D%3D" height="331" width="480" ></a>

Number of Threads | Throughput (ops/sec)
------------ | -------------
1	| 1.43E+06
2	| 2.76E+06
3	| 3.88E+06
4	| 4.85E+06

We can see yDB scales well from 1-4 cores. It achieves same performance of Silo, but it's not as good as DBX.


###LIMITATION and FUTURE WORKS
1. Currently yDB doesn't scale well from 5 to 8 threads, but DBX[1] scales well consistently from 1 to 8 threads, we are still working on this problem. 
2. Current implementation only support read and write operations because that's enough to demonstrate its potential, but we are still going to implement other operations such as insert, delete and scan so that we can test it against full version of YCSB. 

###REFERENCES

[1] Wang, Zhaoguo, et al. "Using restricted transactional memory to build a scalable in-memory database." Proceedings of the Ninth European Conference on Computer Systems. ACM, 2014.

[2] Kung, Hsiang-Tsung, and John T. Robinson. "On optimistic methods for concurrency control." ACM Transactions on Database Systems (TODS) 6.2 (1981): 213-226.

[3] Tu, Stephen, et al. "Speedy transactions in multicore in-memory databases." Proceedings of the Twenty-Fourth ACM Symposium on Operating Systems Principles. ACM, 2013.
