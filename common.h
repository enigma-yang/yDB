#ifndef COMMON_H
#define COMMON_H

#include <assert.h>

#include "stat.h"
#include "lock.h"
#include "rtm.h"
#include "asm.h"

/* FIXME int type is only used for experiment */

struct Version {
	char *value;
	struct Version *next;
	int lsn;
};

struct Record {
	char *value;
	int ver;
	int lsn;
	Version *oldVersions;
};


#endif /* COMMON_H */
