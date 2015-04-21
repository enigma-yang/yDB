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

#endif /* _AMD64_H_ */
