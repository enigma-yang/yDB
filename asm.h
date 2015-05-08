#ifndef _AMD64_H_
#define _AMD64_H_

#include <stdint.h>

inline void
nop_pause()
{
  __asm volatile("pause" : :);
}

inline uint64_t
rdtsc(void)
{
  uint32_t hi, lo;
  __asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
  return ((uint64_t)lo)|(((uint64_t)hi)<<32);
}


inline unsigned long atomic_increment_and_return(volatile unsigned long * obj) {
	unsigned long i = 1;
	asm volatile("lock; xadd %0, %1"
			: "+r" (i), "+m" (*obj)
			: : "memory");
	return i;
}


#endif /* _AMD64_H_ */
