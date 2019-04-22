/* Copyright (C) 2017-2019 NEC Corporation
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


#include <sysdep.h>
#define _ERRNO_H
#include <bits/errno.h>

#if IS_IN (rtld)
# include <dl-sysdep.h>		/* Defines RTLD_PRIVATE_ERRNO.  */
#endif

.globl C_SYMBOL_NAME(errno)
.globl syscall_error

__syscall_error:
	nop
END (__syscall_error)
