#ifndef STAT_H
#define STAT_H


class Stat {
public:
	unsigned long numRTMAbortTxn;
	unsigned long numRTMTxn;
	unsigned long numRTMAbortTree;
	unsigned long numRTMTree;
	unsigned long numCommit;
	unsigned long numAbort;

	Stat() {
		numRTMAbortTxn = 0;
		numRTMTxn = 0;
		numRTMAbortTree = 0;
		numRTMTree = 0;
		numCommit = 0;
		numAbort = 0;
	}
};


#endif /* STAT_H */
