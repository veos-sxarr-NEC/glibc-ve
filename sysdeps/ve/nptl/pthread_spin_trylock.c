/* Copyright (C) 2003-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Paul Mackerras <paulus@au.ibm.com>, 2003.

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

#include <errno.h>
#include "pthreadP.h"

int
pthread_spin_trylock (pthread_spinlock_t *lock)
{
  unsigned int old;
  int err = EBUSY;

    asm volatile ("cas.w %0, %1, %2"
		  : "=&r" (old)
		  : "r" (lock), "i"(0)
		  : "memory");

   if (!old)
    	err = 0;

  return err;
}
