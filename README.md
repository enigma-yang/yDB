###yDB: high-performance and scalable in-memory database based on RTM
Team memeber: Zhiyuan Yang, Zhizhou Yang

###SUMMARY
We are going to use Intel RTM to implement a prototype of in-memory database that is high-performance and scalable on multicore machine.


###BACKGROUND
Traditionally, databases use fine-grained locks and atomic operations to do synchronization. While it can provide good performance and reasonable scalability, the resulting code is very complex to reason and it is difficult to make sure the correctness on different memory models of processors. 

Recently, Intel introduces restricted transactional memory (RTM) support in Haswell processors. Using RTM instructions, one can transactionally execute a part of code or explicitly abort in the middle of transactional execution. If a transaction conflicts with other concurrent memory operations, the processor hardware will discard all its writes and roll back the system to the begining of the execution. All the transaction properties are guaranteed by hardware, so this provides a much easier way to do synchronizations comparing.

###THE CHALLENGE
Although RTM is a promising solution in synchronizaiton problem, it is challenging to use it to implement a in-memory database with high performance and good scalability. 

Firstly, even it sounds straightforward to use RTM for synchronization, its programming model is different from other synchronization mechanism and currently we have no experience in it. 

Secondly, it is not clear how to adopt RTM programming model in database architecture and also it is difficult to debug RTM related bugs because within transactional execution many debugging related instructions are forbidded. 

Thirdly, RTM's abstraction is very simple, but we are not sure about the real performance characteristics of Intel's implementation and it needs efforts to achieve high performance and good scalability.

###RESOURCES
We are going to start our project from scratch, but in case of the time pressure, we may use existing library such as B+ tree library if it can fit in our purpose well. 

For the database design, currently we are reading recent conference papers and investigating existing efforts on this topic. We found these papers [1][2][3] are very useful in high-performance in-memory database design and may adopt some ideas of them. 

To use RTM techinque, we need a machine with CPU that supports RTM, but we could not find such one in CMU.


###GOALS AND DELIVERABLES (Proposal)
Our goal is to implement a high-performance and scalable in-memory database using RTM technique. 

For the functionality, we would like to make it able to run TPC-C benchmark or YCSB benchmark. We are going to use B-tree based store and not going to implement hashtable based store. Due to the time pressure, we are not going to support the durability.

For the performance, we would like to achieve the tps(transaction per second) no worse than the start-of-art in-memory database using fine-grained locks such as Silo in [2]. 

For the scalability, we would like to show it can scale linearly as the number of threads. Since current Intel RTM can only support at most 4 cores, we may only test the scalability from 1 threads to 8 threads. Also, the hyperthreading feature may affect the RTM performance, we expect the scalability may decrease after threads number exceeds 4.

After this project, we expect we can demo the common database functionality and the testing result(data and graphs) about performance and scalability using TPC-C or YCSB benchmark.


###SCHEDULE (Proposal)
| Date        | Milestone  |
| ------------- |-------------------------|
| 4.9      | Design arthitecture of the database, learn to use RTM. |
| 4.16      | Realize our naive database architecture using RTM. |
| 4.23 |  Evaluate the performance of yDB, figure out bottlenecks of the database |
| 4.30 | Optimize the implementation of yDB and try to achieve high performance in RTM. |
| 5.7 | Compare yDB with other transactional databases and make evaluations.  |
| 5.11 | Parallelism competition and final report. |


###CHECKPOINT SUMMARY
We have done the following things:  
1. Implemented coarse-grained hashtable based key-value store.  
2. Realized coarse-grained B+ Tree based key-value store.  
3. Implemented basic performance testing framwork.  
4. Learned the design of Silo and DBX from papers.  
5. Learned RTM programming and performance characteristics.  
6. Learned Optimistic Concurrency Control.


We have no preliminary result yet because we've not applied RTM technique on our key-value store and we didn't implement the YCSB workload.


Currently we don't have serious issues. The RTM machine has been found(thanks to Prof. Kayvon and Prof. Dave), and other things are just time matter of time and hard working.

###UPDATED GOALS AND DELIVERABLES
Our goal is still to implement a high-performance and scalable in-memory database using RTM technique.

For the functionality,we will only support as many features as needed to run YCSB benchmark, especially transaction feature on key-value store,  because we want to focus on improving performance. So we won't support SQL interface but use stored procedure instead. Also, durability will not be supported. We will still keep using B+ tree store but hashtable store may also be supported as a sideproduct of our development because we always start implementing features with a simplified hashtable store and then apply on B+ tree store.

For the performance, we still keep the goal of achieving the tps(transaction per second) no worse than the start-of-art in-memory database such as Silo in [2]. As to the benchmark, since we don't have to support SQL interface, we have to implement derived version of existing benchmarks. So we decide to only use YCSB and not use TPC-C because implementing a derived version of TPC-C would take too much of time.

For the scalability, we would like to show it can scale linearly as the number of threads. Since current Intel RTM can only support at most 4 cores, we may only test the scalability from 1 threads to 8 threads. Also, the hyperthreading feature may affect the RTM performance, we expect the scalability may decrease after threads number exceeds 4.

After this project, we expect we can demo the performance and scalability (in data and graph) of our key-value store against our derived version of YCSB benchmark.


###UPDATED SCHEDULE
| Date        | Milestone  |
| ----------- | ------------------------- |
| 4.20 - 4.22 | Implement OCC on hashtable based kv store (YZY); Implement ycsb workloads (YZZ) |
| 4.23 - 4.26 | Implement OCC on hashtable based kv store (YZY); Implement OCC on B+ Tree (YZZ) |
| 4.27 - 4.29 | Apply RTM on hashtable based kv store (YZY); Implemment OCC on B+ Tree (YZZ) |
| 4.30 - 5.3 | Apply RTM on hashtable based kv store (YZY); Apply RTM on B+ Tree based kv store (YZZ) |
| 5.4 - 5.6 | Apply RTM on B+ Tree based kv store(YZZ); Performance tuning and evaluations (YZY)  |
| 5.7 - 5.10 | More performance tuning and final report. |

###REFERENCES

[1] Wang, Zhaoguo, et al. "Using restricted transactional memory to build a scalable in-memory database." Proceedings of the Ninth European Conference on Computer Systems. ACM, 2014.

[2] Kung, Hsiang-Tsung, and John T. Robinson. "On optimistic methods for concurrency control." ACM Transactions on Database Systems (TODS) 6.2 (1981): 213-226.

[3] Tu, Stephen, et al. "Speedy transactions in multicore in-memory databases." Proceedings of the Twenty-Fourth ACM Symposium on Operating Systems Principles. ACM, 2013.
