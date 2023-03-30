/* Copyright (C) 2009-2020 Free Software Foundation, Inc.
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

#include <sys/uio.h>
#include <sysdep-cancel.h>

#include <io_hook.h>

#if IS_IN (libc)
#define weak_variable weak_function

ssize_t weak_variable (* __pwritev_hook)
  (int, const struct iovec *, int, off_t) = NULL;
#endif

#ifndef __OFF_T_MATCHES_OFF64_T

# ifdef __ASSUME_PREADV

#  ifndef __NR_pwritev
#   define __NR_pwritev __NR_pwritev64
#  endif

ssize_t
pwritev (int fd, const struct iovec *vector, int count, off_t offset)
{
  ssize_t (*hook) (int, const struct iovec *, int, off_t)
    = atomic_forced_read(__pwritev_hook);
  if (hook)
    return hook(fd, vector, count, offset);
  return SYSCALL_CANCEL (pwritev, fd, vector, count, offset);
}
# else
static ssize_t __atomic_pwritev_replacement (int, const struct iovec *,
					     int, off_t);
ssize_t
pwritev (int fd, const struct iovec *vector, int count, off_t offset)
{
  ssize_t (*hook) (int, const struct iovec *, int, off_t)
    = atomic_forced_read(__pwritev_hook);
  if (hook)
    return hook(fd, vector, count, offset);
#  ifdef __NR_pwritev
  ssize_t result = SYSCALL_CANCEL (pwritev, fd, vector, count,
				   offset);
  if (result >= 0 || errno != ENOSYS)
    return result;
#  endif
  return __atomic_pwritev_replacement (fd, vector, count, offset);
}
#  define PWRITEV static __atomic_pwritev_replacement
#  define PWRITE __pwrite
#  define OFF_T off_t
#  include <sysdeps/posix/pwritev_common.c>
# endif /* __ASSUME_PREADV  */

libc_hidden_def (pwritev)
#endif
