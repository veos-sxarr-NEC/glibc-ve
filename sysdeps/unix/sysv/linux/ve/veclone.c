/*
   Copyright (C) 2017-2019 NEC Corporation
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it
   and/or modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either version
   2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.
*/

#include <sys/types.h>
#include <errno.h>
#include <stdarg.h>

/* Defining glibc wrapper function for  clone system call */
int clone(int (*func)(void *), void *stack, int flags, void *arg, ...)
{
	va_list ap;
	pid_t *ptid = NULL, *ctid = NULL;
	void  *tls = NULL;
	int ret = 0;

	va_start(ap, arg);
	ptid = va_arg(ap, pid_t *);
	tls  = va_arg(ap, void *);
	ctid = va_arg(ap, pid_t *);
	va_end(ap);

	if((ret =__glibc_unlikely (__clone(func, stack, flags, arg, ptid, tls, ctid)))
			< 0) {
		errno = -ret;
		return -1;
	}
	return ret;
}
