/* Copyright (C) 2003-2015 Free Software Foundation, Inc.
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
   <http://www.gnu.org/licenses/>.  */
/* Changes by NEC Corporation for the VE port, 2017-2019 */

#include "pthreadP.h"
int pthread_spin_lock (lock)
     pthread_spinlock_t *lock;
{
  unsigned int val = 1;

    asm volatile ("1: cas.w %0, %1, %2\n\t"
		  "fencei \n\t"
		  "brne.w 0, %0, 1b"
		  : "=&r" (val)
		  : "r" (lock), "i"(0)
		  : "memory");
  return 0;
}
