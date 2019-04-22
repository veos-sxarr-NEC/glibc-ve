/* Set flags signalling availability of kernel features based on given
   kernel version number.  VE version.
   Copyright (C) 1999-2015 Free Software Foundation, Inc.
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
/* __TODO_PORT_HCLT : Only observed changes are added.
 * Rest other macros  are needs to be verify before any modification.
 * changes include :
 *	1. Remove __ASSUME_SOCKETCALL
 *	2. undef __ASSUME_FUTEX_LOCK_PI
 *	3. undef __ASSUME_REQUEUE_PI
 *	4. Support __ASSUME_SET_ROBUST_LIST : available in generic kernel-features.h
 *	5. Support __ASSUME_PRIVATE_FUTEX : available in generic kernel-features.h
 *	6. TBD . __ASSUME_FUTEX_CLOCK_REALTIME
 */
/* The recvmmsg syscall was added for i386 in 2.6.33.  */
#if __LINUX_KERNEL_VERSION >= 0x020621
# define __ASSUME_RECVMMSG_SYSCALL	1
#endif

/* The sendmmsg syscall was added for i386 in 3.0.  */
#if __LINUX_KERNEL_VERSION >= 0x030000
# define __ASSUME_SENDMMSG_SYSCALL	1
#endif

# undef __ASSUME_FUTEX_LOCK_PI
# undef __ASSUME_REQUEUE_PI

#include_next <kernel-features.h>
