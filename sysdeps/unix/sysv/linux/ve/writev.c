/* Linux writev syscall implementation.
   Copyright (C) 2017-2020 Free Software Foundation, Inc.
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

#include <unistd.h>
#include <sys/uio.h>
#include <sysdep-cancel.h>

#include <io_hook.h>

#if IS_IN (libc)
#define weak_variable weak_function

ssize_t weak_variable (* __writev_hook)
  (int, const struct iovec *, int) = NULL;
#endif

ssize_t
__writev (int fd, const struct iovec *iov, int iovcnt)
{
  ssize_t (*hook) (int, const struct iovec *, int)
    = atomic_forced_read(__writev_hook);
  if (hook)
    return hook(fd, iov, iovcnt);
  else
    return SYSCALL_CANCEL (writev, fd, iov, iovcnt);
}
libc_hidden_def (__writev)
weak_alias (__writev, writev)
