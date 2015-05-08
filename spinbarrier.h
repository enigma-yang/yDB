#ifndef _SPINBARRIER_H_
#define _SPINBARRIER_H_

#include "asm.h"

/**
 * Barrier implemented by spinning
 */

class SpinBarrier {
public:
  SpinBarrier(size_t n)
    : n(n)
  {
  }

  SpinBarrier(const SpinBarrier &) = delete;
  SpinBarrier(SpinBarrier &&) = delete;
  SpinBarrier &operator=(const SpinBarrier &) = delete;

  ~SpinBarrier()
  {
  }

  void
  countDown()
  {
    // written like this (instead of using __sync_fetch_and_add())
    // so we can have assertions
    for (;;) {
      size_t copy = n;
      if (__sync_bool_compare_and_swap(&n, copy, copy - 1))
        return;
    }
  }

  void
  waitFor()
  {
    while (n > 0)
      nop_pause();
  }

private:
  volatile size_t n;
};

#endif /* _SPINBARRIER_H_ */
