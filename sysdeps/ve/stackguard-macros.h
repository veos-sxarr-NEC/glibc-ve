/* Changes by NEC Corporation for the VE port, 2020 */

#include <stdint.h>

#define STACK_CHK_GUARD \
  ({ uintptr_t x;						\
     asm ("ld %0, %1(,%%tp)" : "=r" (x)				\
	  : "i" (offsetof (tcbhead_t, stack_guard))); x; })

#define POINTER_CHK_GUARD \
  ({ uintptr_t x;						\
     asm ("ld %0, %1(,%%tp)" : "=r" (x)				\
	  : "i" (offsetof (tcbhead_t, pointer_guard))); x; })
