#ifndef COMMON_H
#define COMMON_H

#include <assert.h>

#include "rtm.h"
#include "rtm-goto.h"
#include "stat.h"

/* FIXME int type is only used for experiment */

struct Record {
	char *value;
	int ver;
};

#define CACHELINE_SIZE 64


#endif /* COMMON_H */
