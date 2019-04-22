/* Copyright (C) 1995-2015 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@gnu.ai.mit.edu>, August 1995.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */
/* Changes by NEC Corporation for the VE port, 2017-2019 */

#include <errno.h>
#include <stdarg.h>
#include <sys/sem.h>
#include <ipc_priv.h>
#include <sysdep.h>
#include <sys/syscall.h>


/* Define a `union semun' suitable for Linux here.  */
union semun
{
  int val;			/* value for SETVAL */
  struct semid_ds *buf;		/* buffer for IPC_STAT & IPC_SET */
  unsigned short int *array;	/* array for GETALL & SETALL */
  struct seminfo *__buf;	/* buffer for IPC_INFO */
};

int __new_semctl (int semid, int semnum, int cmd, ...);
int
__new_semctl (int semid, int semnum, int cmd, ...)
{
	union semun arg = {0};
	va_list ap;

	switch(cmd) {
		case SETVAL: case GETALL: case SETALL: case IPC_STAT: case IPC_SET:
		case IPC_INFO: case SEM_INFO: case SEM_STAT:
			va_start (ap, cmd);
			/* Get the argument.  */
			arg = va_arg (ap, union semun);
			va_end (ap);
			break;
	}
	return INLINE_SYSCALL (semctl, 4, semid, semnum, cmd ,
			arg.buf);
}

#include <shlib-compat.h>
versioned_symbol (libc, __new_semctl, semctl, GLIBC_2_2);
