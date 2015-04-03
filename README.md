###YDB: high performance in-memory database based on RTM
Team memeber: Zhiyuan Yang, Zhizhou Yang

###SUMMARY
We are going to use Intel RTM to implement a prototype of in-memory database that is high performance and scalable on multicore machine.


###BACKGROUND
Traditionally, databases use fine-grained locks and atomic operations to do synchronization. While it can provide good performance and reasonable scalability, the resulting code is very complex to reason and it is difficult to make sure the correctness on different memory models of processors. 

Recently, Intel introduces restricted transactional memory (RTM) support in Haswell processors. Using RTM instructions, one can transactionally execute a part of code or explicitly abort in the middle of transactional execution. If a transaction conflicts with other concurrent memory operations, the processor hardware will discard all its writes and roll back the system to the begining of the execution. All the transaction properties are guaranteed by hardware, so this provides a much easier way to do synchronizations comparing.

###THE CHALLENGE
Although RTM is a promising solution in synchronizaiton problem, it is challenging to use it to implement a in-memory database with high performance and good scalability. 

Firstly, even it sounds straightforward to use RTM for synchronization, its programming model is different from other synchronization mechanism and currently we have no experience in it. 

Secondly, it is not clear how to adopt RTM programming model in db's architecture and also it is difficult to debug RTM related bugs because within transactional execution many debugging related instructions are forbidded. 

Thirdly, RTM's abstraction is very simple, but we are not sure about the real performance characteristics of Intel's implementation and it needs efforts to achieve high performance and good scalability.

###RESOURCES
We are going to start our project from scratch, but in case of the time pressure, we may use existing library such as B+ tree library if it can fit in our purpose well. 

For the database design, currently we are reading recent conference papers and investigating existing efforts on this topic. We found these papers [1][2][3] are very useful in high-performance in-memory database design and may adopt some ideas of them. 

To use RTM techinque, we need a machine with CPU that supports RTM, but we could not find such one in CMU.


###GOALS AND DELIVERABLES
Our goal is to implement a high performance and scalable in-memory database using RTM technique. 

For the functionality, we would like to make it able to run TPC-C benchmark or YCSB benchmark. We are going to use B-tree based index and not going to implement hashtable based index. 

For the performance, we would like to achieve the tps(transaction per second) no worse than the start-of-art in-memory database using fine-grained locks such as Silo in [2]. 

For the scalability, we would like to show it can scale linearly as the number of threads. Since current Intel RTM can only support at most 4 cores, we may only test the scalability from 1 threads to 8 threads. Also, the hyperthreading feature may affect the RTM performance, we expect the scalability may decrease after threads number exceeds 4. Due to the time pressure, we are not going to support the durability.

After this project, we expect we can demo the common database functionality and the testing result(data and graphs) about performance and scalability using TPC-C or YCSB benchmark.

###SCHEDULE
| Date        | Milestone  |
| ------------- |-------------------------|
| 4.9      | Design arthitecture of the database, determine the data structures to use. |
| 4.16      | Realize our dabatase structure and transactions using RTM. |
| 4.23 |  Evaluate the performance of YDB, figure out bottlenecks of the database |
| 4.30 | Optimize the implementation of YDB and try to achieve high performance in RTM. |
| 5.7 | Compare YDB with other transactional databases and make evaluations.  |
| 5.11 | Parallelism competition and final report. |

###REFERENCES

[1] Wang, Zhaoguo, et al. "Using restricted transactional memory to build a scalable in-memory database." Proceedings of the Ninth European Conference on Computer Systems. ACM, 2014.

[2] Kung, Hsiang-Tsung, and John T. Robinson. "On optimistic methods for concurrency control." ACM Transactions on Database Systems (TODS) 6.2 (1981): 213-226.

[3] Tu, Stephen, et al. "Speedy transactions in multicore in-memory databases." Proceedings of the Twenty-Fourth ACM Symposium on Operating Systems Principles. ACM, 2013.