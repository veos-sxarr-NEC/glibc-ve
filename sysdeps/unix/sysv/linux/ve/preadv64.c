/* Copyright (C) 2016-2020 Free Software Foundation, Inc.
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
/* Changes by NEC Corporation for the VE port, 2022 */

#include <sys/uio.h>
#include <sysdep-cancel.h>

#include <io_hook.h>

#if IS_IN (libc)
#define weak_variable weak_function

ssize_t weak_variable (* __preadv_hook)
  (int, const struct iovec *, int, off_t) = NULL;
#endif

#ifdef __ASSUME_PREADV

# ifndef __NR_preadv64
#  define __NR_preadv64 __NR_preadv
# endif

ssize_t
preadv64 (int fd, const struct iovec *vector, int count, off64_t offset)
{
  ssize_t (*hook) (int, const struct iovec *, int, off_t)
    = atomic_forced_read(__preadv_hook);
  if (hook)
    return hook(fd, vector, count, offset);
  return SYSCALL_CANCEL (preadv64, fd, vector, count, LO_HI_LONG (offset));
}
#else
static ssize_t __atomic_preadv64_replacement (int, const struct iovec *,
					      int, off64_t);
ssize_t
preadv64 (int fd, const struct iovec *vector, int count, off64_t offset)
{
  ssize_t (*hook) (int, const struct iovec *, int, off_t)
    = atomic_forced_read(__preadv_hook);
  if (hook)
    return hook(fd, vector, count, offset);
#ifdef __NR_preadv64
  ssize_t result = SYSCALL_CANCEL (preadv64, fd, vector, count,
				   LO_HI_LONG (offset));
  if (result >= 0 || errno != ENOSYS)
    return result;
#endif
  return __atomic_preadv64_replacement (fd, vector, count, offset);
}
# define PREADV static __atomic_preadv64_replacement
# define PREAD __pread64
# define OFF_T off64_t
# include <sysdeps/posix/preadv_common.c>
#endif
libc_hidden_def (preadv64)

#ifdef __OFF_T_MATCHES_OFF64_T
strong_alias (preadv64, preadv)
libc_hidden_def (preadv)
#endif
