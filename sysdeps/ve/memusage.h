/* Copyright (C) 2001-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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
   <https://www.gnu.org/licenses/>.  */
/* Changes by NEC Corporation for the VE port, 2020 */

#define GETSP() ({ 						\
	uintptr_t stack_ptr;					\
	asm volatile("lea %0, (%%sp)":"=r"(stack_ptr));		\
	stack_ptr; })

/* #define GETTIME(low,high) asm ("rdtsc" : "=a" (low), "=d" (high)) */

#ifndef DELAY_FOR_VLFA_EXCEPTION
#define DELAY_FOR_VLFA_EXCEPTION \
"or %s34,14,(0)1\n\t" \
"brgt.w  0,%s34,32\n\t" \
"fencei\n\t" \
"subs.w.sx  %s34,%s34,(63)0\n\t" \
"br.l -24\n\t" \
"nop\n\t"
#endif

#define GETTIME(low,high) ({					\
	uint64_t var;					\
        __asm__ __volatile__ ( \
        DELAY_FOR_VLFA_EXCEPTION \
        ); \
	__asm__ __volatile__ ("smir %0, %%usrcc" : "=r" (var));	\
	low = 0xffffffff & var;					\
	high = var >> 32; 					\
	high; })

#include <sysdeps/generic/memusage.h>
