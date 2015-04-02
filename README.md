###YDB: high performance in-memory database based on RTM
Team memeber: Zhiyuan Yang, Zhizhou Yang

###SUMMARY
We are going to use Intel RTM to implement a prototype of in-memory database that is high performance and scalable on multicore machine.


###BACKGROUND
Traditionally, databases use fine-grained locks and atomic operations to do synchronization. While it can provide good performance and reasonable scalability, the resulting code is very complex to reason and it is difficult to make sure the correctness on different memory models of processors. 

Recently, Intel introduces restricted transactional memory (RTM) support in Haswell processors. Using RTM instructions, one can transactionally execute a part of code or explicitly abort in the middle of transactional execution. If a transaction conflicts with other concurrent memory operations, the processor hardware will discard all its writes and roll back the system to the begining of the execution. All the transaction properties are guaranteed by hardware, so this provides a much easier way to do synchronizations comparing.

###THE CHALLENGE
Although RTM is a promising solution in synchronizaiton problem, it's challenging to use it to implement a in-memory database with high performance and good scalability. First, even it sounds straightforward to use RTM for synchronization, its programming model is different from other synchronization mechanism and currently we have no experience in it. Second, it's not clear how to adopt RTM programming model in db's architecture and also it's difficult to debug RTM related bug becaues within transactional execution many debugging related instructions are forbidded. Third, RTM's abstraction is very simple, but we are not sure about the real performance characteristics of Intel's implementation and it needs efforts to achieve high performance and good scalability.

###RESOURCES
We are going to start our project from scratch.
The computer we use should have CPUs of Haswell architecture and support RTM.
There is one paper proposed by Z.Wang et al. in 2014 that realized a scalable in-memory database using RTM [1]. We learnt the idea in this paper and are trying to build our own database system on RTM.


###GOALS AND DELIVERABLES
Describe the deliverables or goals of your project.

This is by far the most important section of the proposal:

Separate your goals into what you PLAN TO ACHIEVE (what you believe you must get done to have a successful project and get the grade you expect) and an extra goal or two that you HOPE TO ACHIEVE if the project goes really well and you get ahead of schedule. It may not be possible to state precise performance goals at this time, but we encourage you be as precise as possible. If you do state a goal, give some justification of why you think you can achieve it. (e.g., I hope to speed up my starter code 10x, because if I did it would run in real-time.)
If applicable, describe the demo you plan to show at the parallelism computation (will it be an interactive demo? will you show an output of the program that is really neat? will you show speedup graphs?). Specifically, what will you show us that will demonstrate you did a good job?
If your project is an analysis project, what are you hoping to learn about the workload or system being studied? What question(s) do you plan to answer in your analysis?
Systems project proposals should describe what the system will be capable of and what performance is hoped to be achieved.
PLATFORM CHOICE. Describe why the platform (computer and/or language) you have chosen is a good one for your needs. Why does it make sense to use this parallel system for the workload you have chosen?

###SCHEDULE
| Date        | Milestone  |
| ------------- |-------------------------|
| 4.9      | right-aligned |
| 4.16      | centered      |
| 4.23 | are neat      |
| 4.30 | are neat      |
| 5.7 | are neat      |
| 5.11 | Parallelism Competition and Final Report      |
Produce a schedule for your project. Your schedule should have at least one item to do per week. List what you plan to get done each week from now until the parallelism competition in order to meet your project goals. Keep in mind that due to other classes, you'll have more time to work some weeks than others (work that into the schedule). You will need to re-evaluate your progress at the end of each week and update this schedule accordingly. Note the intermediate checkpoint deadline is April 16th. In your schedule we encourage you to be precise as precise as possible. It's often helpful to work backward in time from your deliverables and goals, writing down all the little things you'll need to do (establish the dependencies!).

###REFERENCES

[1] Wang, Zhaoguo, et al. "Using restricted transactional memory to build a scalable in-memory database." Proceedings of the Ninth European Conference on Computer Systems. ACM, 2014.