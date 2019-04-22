/* Internal definitions for thread-friendly fork implementation.  Linux/i386.
   Copyright (C) 2002-2015 Free Software Foundation, Inc.
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
/* __TODO_PORT_HCLT : Total number of args for clone */
/* Changes by NEC Corporation for the VE port, 2017-2019 */

#include <sched.h>
#include <sysdep.h>
#include <tls.h>
/* Prototype for the raw system call :
   long clone(unsigned long flags, void *child_stack,  void *ptid, void *ctid,struct pt_regs *regs);

   According to  prototype, void *ctid is 4th argument to the clone system call .Hence &THREAD_SELF->tid
   must be passed as 4th argument to the clone .
 */
#define ARCH_FORK() \
  INLINE_SYSCALL (clone, 5,						      \
		  CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID | SIGCHLD,	      \
		  NULL, NULL, &THREAD_SELF->tid,NULL)
