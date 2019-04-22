/* Copyright (C) 2002-2015 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 2002.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* Changes by NEC Corporation for the VE port, 2017-2019 */

/* TODO_PORT_HCLT: atomic_and() */
#include <stdint.h>
#include <tls.h>	/* For tcbhead_t.  */
#include <libc-internal.h>


typedef int8_t atomic8_t;
typedef uint8_t uatomic8_t;
typedef int_fast8_t atomic_fast8_t;
typedef uint_fast8_t uatomic_fast8_t;

typedef int16_t atomic16_t;
typedef uint16_t uatomic16_t;
typedef int_fast16_t atomic_fast16_t;
typedef uint_fast16_t uatomic_fast16_t;

typedef int32_t atomic32_t;
typedef uint32_t uatomic32_t;
typedef int_fast32_t atomic_fast32_t;
typedef uint_fast32_t uatomic_fast32_t;

typedef int64_t atomic64_t;
typedef uint64_t uatomic64_t;
typedef int_fast64_t atomic_fast64_t;
typedef uint_fast64_t uatomic_fast64_t;

typedef intptr_t atomicptr_t;
typedef uintptr_t uatomicptr_t;
typedef intmax_t atomic_max_t;
typedef uintmax_t uatomic_max_t;

/* This is equal to 1 iff the architecture supports 64b atomic operations.  */
#define __HAVE_64B_ATOMICS 1

#define USE_ATOMIC_COMPILER_BUILTINS 1

#define __arch_compare_and_exchange_val_32_acq(mem, newval, oldval)	      \
  ({									      \
      __typeof (*(mem)) __tmp;						      \
      __typeof (newval)  nval = newval;						      \
      __typeof (oldval)  oval = oldval;						      \
      __asm __volatile ("or %0, 0, %3\n\t"				      \
			"fencem %4\n\t"							\
			"cas.w %0, %1, %2\n\t"			      \
			"fencem %5\n\t"					\
		        : "=&r" (__tmp)					      \
		        : "r" (mem), "r" (oval), "r" (nval), "i"(1), "i"(2)  \
			: "memory");					      \
	__tmp;								      \
  })

#  define __arch_compare_and_exchange_val_64_acq(mem, newval, oldval) \
  ({ __typeof (*mem) __tmp;						      \
      __typeof (newval) nval = newval;                                                     \
      __typeof (oldval) oval = oldval;                                                     \
      __asm __volatile ("or %0, 0, %3\n\t"				      \
			 "fencem %4\n\t"			\
			"cas.l %0, %1, %2\n\t"				      \
			"fencem %5\n\t"						\
		        : "=&r" (__tmp)					      \
		        : "r" (mem), "r" (oval), "r" (nval), "i"(1), "i"(2)   \
			: "memory");					      \
	__tmp;								      \
   })

#  define __arch_c_compare_and_exchange_val_64_acq(mem, newval, oldval) \
  ({ __typeof (*mem) __tmp;						      \
      __typeof (newval) nval = newval;                                                     \
      __typeof (oldval) oval = oldval;                                                     \
      __asm __volatile ("or %0, 0, %3\n\t"				      \
			"fencem %4\n\t"						\
			"cas.l %0, %1, %2\n\t"				      \
			"fencem %5\n\t"						\
		        : "=&r" (__tmp)					      \
		        : "r" (mem), "r" (oval), "r" (nval), "i"(1), "i"(2)   \
		        : "memory");					      \
     __tmp;								      \
   })

#define atomic_compare_and_exchange_val_acq(mem, newval, oldval) \
  ({									      \
    __typeof (*(mem)) __result;						      \
      __typeof (newval) __newval = newval;                                                     \
      __typeof (oldval) __oldval = oldval;                                                     \
    if (sizeof (*mem) == 4)						      \
      __result = __arch_compare_and_exchange_val_32_acq(mem, __newval, __oldval); \
    else if (sizeof (*mem) == 8)					      \
      __result = __arch_compare_and_exchange_val_64_acq(mem, __newval, __oldval); \
    else 								      \
       abort ();							      \
    __result;								      \
  })

#define atomic_compare_and_exchange_bool_acq(mem, newval, oldval) \
  ({									      \
    __typeof (*(mem)) __result = 0;						      \
      __typeof (newval) nval = newval;                                    \
      __typeof (oldval) oval = oldval;                                    \
    if (sizeof (*mem) == 4)						      \
       __asm __volatile ("or %0, 0, %3\n\t"				      \
			  "fencem %4\n\t"			\
			 "cas.w %0, %1, %2\n\t"				      \
			  "fencem %5\n\t"					\
			 : "=&r" (__result) 				      \
			 : "r" (mem), "r" (oval), "r" (nval), "i"(1), "i"(2)	      \
			 : "memory");					      \
    else if (sizeof (*mem) == 8)					      \
       __asm __volatile ("or %0, 0, %3\n\t"				      \
			   "fencem %4\n\t"					\
			 "cas.l %0, %1, %2\n\t"				      \
			   "fencem %5\n\t"					\
			 : "=&r" (__result) 				      \
			 : "r" (mem), "r" (oval), "r" (nval), "i"(1), "i"(2)	      \
			 : "memory");					      \
    else 								      \
       abort ();							      \
	((__result) == (oval)) ? 0 : 1; 		     	  	      \
  })

#ifndef atomic_compare_and_exchange_bool_rel
#define atomic_compare_and_exchange_bool_rel(mem, newval, oldval)             \
	atomic_compare_and_exchange_bool_acq (mem, newval, oldval)
#endif

/*
#define __arch_c_compare_and_exchange_val_8_acq(mem, newval, oldval)

#define __arch_c_compare_and_exchange_val_16_acq(mem, newval, oldval)
*/

/* Store NEWVALUE in *MEM and return the old value.  */
#define atomic_exchange_acq(mem, newvalue) \
  ({ __typeof (*(mem)) __result = (newvalue);				      \
     if (sizeof (*mem) == 4) {     					      \
       __asm __volatile ("fencem %3\n\t"					\
			"ts1am.w %0, %1, %2\n\t"			      \
			"fencem %4\n\t"						\
			 : "+&r" (__result)				      \
			 : "r" (mem), "i" (15), "i"(1), "i"(2));			      \
    } else if (sizeof (*mem) == 8) {					      \
       __asm __volatile ("fencem %3\n\t"						\
			"ts1am.l %0, %1, %2\n\t"				      \
			"fencem %4\n\t"						\
			 : "+&r" (__result)				      \
			 : "r" (mem), "r" (255), "i"(1), "i"(2));			      \
    }  else 								      \
	       abort ();						      \
     __result;								      \
  })

/* Add NEWVALUE in *MEM and return the old value of *MEM.  */
#define atomic_exchange_and_add(mem, value) \
  ({ __typeof (*(mem)) __result = 0;		                              \
     __typeof (*(mem)) __temp = 2;					      \
     __typeof (value) val = value;					      \
            if (sizeof (*(mem)) == 4)					      \
       __asm __volatile ("1:\tldl.sx %1, 0(0,%2)\n\t"			      \
			 "addu.w %0, %1, %3\n\t"			      \
			"fencem %4\n\t"						\
			 "cas.w %0, %2, %1\n\t"				      \
			"fencem %5\n\t"						\
			 "brne.w %0, %1, 1b"				      \
			 : "+&r" (__result), "+r" (__temp)		      \
			 : "r" (mem), "r" (val), "i"(1), "i"(2));	      \
    else if (sizeof (*mem) == 8)					      \
       __asm __volatile ("or %0, 0, %3\n\t"				      \
			"fencem %4\n\t"						\
			 "atmam %0, %2, %1\n\t"				      \
			"fencem %5\n\t"						\
			 : "+&r" (__result)				      \
			 : "r" (__temp), "r" (mem), "r" (val), "i"(1), "i"(2));	      \
    else 								      \
       abort ();							      \
    __result;								      \
  })
#define catomic_exchange_and_add(mem, value) \
   atomic_exchange_and_add(mem, value)

#define atomic_max(mem, value) \
  ({ __typeof (*(mem)) __result = 0;					      \
     __typeof (*(mem)) __temp = 0;						      \
     __typeof (value) val = value;					      \
     if (sizeof (*(mem)) == 4)						      \
	 __asm __volatile ("1:\tldl.sx %1, (,%2)\n\t"			      \
		 "maxs.w %0, %1, %3\n\t"				      \
		"fencem %4\n\t"						\
		 "cas.w %0, %2, %1\n\t"					      \
		"fencem %5\n\t"						\
		 "brne.w %0, %1, 1b\n\t"				      \
		 : "+&r" (__result), "+r" (__temp)			      \
		 : "r" (mem), "r" (val), "i"(1), "i"(2));				      \
     else if (sizeof (*mem) == 8)					      \
	 __asm __volatile ("1:\tld %1, 0(0,%2)\n\t"			      \
		 "maxs.l %0, %1, %3\n\t"				      \
		"fencem %4\n\t"						\
		 "cas.l %0, %2, %1\n\t"					      \
		"fencem %5\n\t"						\
		 "brne.l %0, %1, 1b\n\t"				      \
		 : "+&r" (__result), "+r" (__temp)			      \
		 : "r" (mem), "r" (val), "i"(1), "i"(2));			\
  })

# define catomic_max(mem, value) \
	atomic_max(mem, value)

#define atomic_min(mem, value) \
  ({ __typeof (*(mem)) __result;					      \
     __typeof (*(mem)) __temp;						      \
     __typeof (value) val = value;                                        \
     if (sizeof (*(mem)) == 4)						      \
	 __asm __volatile ("1:\tldl.sx %1, %2\n\t"			      \
		 "mins.w %0, %1, %3\n\t"					      \
		"fencem %4\n\t"						\
		 "cas.w %0, %2, %1\n\t"					      \
		"fencem %5\n\t"						\
		 "brne.w %0, %1, 1b\n\t"					      \
		 : "+&r" (__result), "+r" (__temp)			      \
		 : "r" (mem), "r" (val), "i"(1), "i"(2));				      \
    else if (sizeof (*mem) == 8)					      \
	 __asm __volatile ("1:\tld %1, %2\n\t"				      \
		 "mins.l %0, %1, %3\n\t"					      \
		"fencem %4\n\t"						\
		 "cas.l %0, %2, %1\n\t"					      \
		"fencem %5\n\t"						\
		 "brne.l %0, %1, 1b\n\t"					      \
		 : "+&r" (__result), "+r" (__temp)			      \
		 : "r" (mem), "r" (val), "i"(1), "i"(2));				      \
  })

#define atomic_add(mem, value) (void) atomic_exchange_and_add ((mem), (value))

/* Atomically adds value to *MEM and returns true if the result is negative,
 * or false when result is greater than or equal to zero */
#define atomic_add_negative(mem, value) \
  ({ __typeof (*(mem)) __result;					      \
     __typeof (*(mem)) __temp = 2;					      \
     __typeof (value) val = value;                                        \
     if (sizeof (*mem) == 4)						      \
	 __asm __volatile ("1:\tldl.sx %1, %2\n\t"			      \
		 "adds.w %0, %1, %3\n\t"				      \
		 "or  %s1, 0, %0\n\t"					      \
		 "fencem %4\n\t"						\
		 "cas.w %0, %2, %1\n\t"				 	      \
		 "fencem %5\n\t"						\
		 "brne.w %0, %1, 1b\n\t"				      \
		 "or %1, 0, %s1"					      \
		 : "+&r" (__result), "=r" (__temp)			      \
		 : "r" (mem), "r" (val), "i"(1), "i"(2)			      \
		 : "%s1");						      \
     else if (sizeof (*mem) == 8)					      \
	 __asm __volatile ("or %0, 0, %3\n\t"				      \
		 "fencem %4\n\t"						\
		 "atmam %0, %2, %1\n\t"					      \
		 "fencem %5\n\t"						\
		 "ldl.sx %1, %2"						      \
		 : "+&r" (__result), "+r" (__temp)			      \
		 : "r" (mem), "r" (val), "i"(1), "i"(2));				      \
     else 								      \
	 abort ();							      \
     ((__temp) < 0) ? 1 : 0;						      \
  })

#define atomic_add_zero(mem, value) \
  ({ __typeof (*(mem)) __result = 0;					      \
     __typeof (*(mem)) __temp = 2;					      \
     __typeof (value) val = value;					      \
     if (sizeof (*mem) == 4)						      \
	 __asm __volatile ("1:\tldl.sx %1, %2\n\t"			      \
		 "lea %%s42, %2\n\t"					      \
		 "adds.w %0, %1, %3\n\t"				      \
		 "or  %%s41, 0, %0\n\t"					      \
		 "fencem %4\n\t"						\
		 "cas.w %0, %%s42, %1\n\t"				      \
		 "fencem %5\n\t"						\
		 "or %1, 0, %%s41"					      \
		 : "+r" (__result), "+r" (__temp)			      \
		 : "m" (*mem), "r" (val), "i"(1), "i"(2));				      \
     else if (sizeof (*mem) == 8)					      \
	 __asm __volatile ("or %0, 0, %3\n\t"				      \
		 "fencem %4\n\t"						\
		 "atmam %0, %2, %1\n\t"					      \
		 "fencem %5\n\t"						\
		 "ldl.sx %1, 0(0,%2)"						      \
		 : "+&r" (__result), "+r" (__temp)			      \
		 : "r" (mem), "r" (val), "i"(1), "i"(2));				      \
     else 								      \
	 abort ();							      \
     ((__temp) == 0) ? 1 : 0;						      \
  })

#define atomic_increment(mem) atomic_add ((mem), 1)
#define atomic_decrement(mem) atomic_add ((mem), -1)

#define atomic_increment_and_test(mem) atomic_add_zero((mem), 1)
#define atomic_decrement_and_test(mem) atomic_add_zero((mem), -1)

#define atomic_delay() __asm __volatile ("nop")

#define __arch_or_body(mem, mask)					      \
 ({ __typeof (*(mem)) __temp = 0;						      \
    unsigned long __mask = mask;;					      \
    if (sizeof (*mem) == 1)						      \
      __asm __volatile ("ld1b %0, %1\n\t"				      \
			"or %0, %0, %2\n\t"				      \
			"st1b %0, %1"					      \
			: "+r"(__temp)					      \
			: "r" (mem), "r" (mask)				      \
			: "memory");					      \
    else if (sizeof (*mem) == 2)					      \
      __asm __volatile ("ld2b %0, %1\n\t"				      \
			"or %0, %0, %2\n\t"				      \
			"st2b %0, %1"					      \
			: "+r"(__temp)					      \
			: "r" (mem), "r" (mask)				      \
			: "memory");					      \
    else if (sizeof (*mem) == 4)					      \
      __asm __volatile ("1:\tldl.sx %0, %1\n\t"				      \
			"lea %%s2, %1\n\t"				      \
			"or %%s1, %0, %2\n\t"				      \
			"fencem %3\n\t"						\
			"cas.w %%s1, %%s2, %0\n\t"			      \
			"fencem %4\n\t"						\
			"brne.l %0, %%s1, 1b"				      \
			: "+r"(__temp)					      \
			: "m" (*mem), "r" (__mask), "i"(1), "i"(2)			      \
			: "%s1", "memory");				      \
    else								      \
      __asm __volatile ("or %0 , 0, %2\n\t"				      \
			"fencem %4\n\t"						\
			"atmam %0, %3, %1"				      \
			"fencem %5\n\t"						\
			: "+&r" (__temp)				      \
			: "r" (mem), "r" (mask), "i" (1), "i"(1), "i"(2)      \
			: "memory");					      \
 })

#define atomic_or(mem, mask) __arch_or_body (mem, mask)

#define catomic_or(mem, mask) __arch_or_body (mem, mask)

#define atomic_and(mem, mask) \
 ({ __typeof (*(mem)) __temp = 0;					      \
    __typeof (*(mem)) __temp1 = 0;                                            \
    __typeof (*(mem)) __mask = mask;			                      \
    if (sizeof (*mem) == 1)						      \
      __asm __volatile ("ld1b %0, (,%1)\n\t"				      \
			"and %0, %0, %2\n\t"				      \
			"st1b %0, %1"					      \
			: "+&r"(__temp)					      \
			: "r" (mem), "r" (__mask)			      \
			: "memory");					      \
    else if (sizeof (*mem) == 2)					      \
      __asm __volatile ("ld2b %0, (,%1)\n\t"				      \
			"and %0, %0, %2\n\t"				      \
			"st2b %0, %1"					      \
			: "+&r"(__temp)					      \
			: "r" (mem), "r" (__mask)			      \
			: "memory");					      \
    else if (sizeof (*mem) == 4)/*Changes made to resolve bus_error issue inside free function*/ \
      __asm __volatile ("1:\tldl.sx %3, (,%1)\n\t"			      \
			"and %0, %2, %3\n\t"				      \
			"stu  %0, (,%1) \n\t"				    \
			: "+&r"(__temp)					      \
			: "r" (mem), "r" (__mask),"r"(__temp1)			      \
			: "memory");				      \
    else								      \
      __asm __volatile ("or %0 , 0, (,%2)\n\t"				      \
			"fencem %4\n\t"						\
			"atmam %0, %3, %1"				      \
			"fencem %5\n\t"						\
			:"+&r" (__temp)					      \
			: "r" (mem), "r" (mask), "i" (0), "i"(1), "i"(2)      \
			);						      \
  })

#define catomic_and(mem, value) \
   atomic_and(mem, value)
