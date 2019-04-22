/* Copyright (C) 2017-2019 NEC Corporation
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

#include <atomic.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define weak_variable weak_function

extern ssize_t __libc_read (int, void *, size_t);
extern ssize_t __libc_pread (int, void *, size_t, off_t);
extern ssize_t __readv (int, const struct iovec *, int);
extern ssize_t __libc_write (int, const void *, size_t);
extern ssize_t __libc_pwrite (int, const void *, size_t, off_t);
extern ssize_t __writev (int, const struct iovec *, int);

ssize_t weak_variable (* __read_hook)
  (int, void *, size_t) = NULL;
ssize_t weak_variable (* __pread_hook)
  (int, void *, size_t, off_t) = NULL;
ssize_t weak_variable (* __readv_hook)
  (int, const struct iovec *, int) = NULL;
ssize_t weak_variable (* __write_hook)
  (int, const void *, size_t) = NULL;
ssize_t weak_variable (* __pwrite_hook)
  (int, const void *, size_t, off_t) = NULL;
ssize_t weak_variable (* __writev_hook)
  (int, const struct iovec *, int) = NULL;

ssize_t
__io_hook_read (int fd, void *buf, size_t count)
{
  ssize_t (*hook) (int, void *, size_t)
    = atomic_forced_read(__read_hook);
  if (hook)
    return hook(fd, buf, count);
  else
    return __libc_read(fd, buf, count);
}

weak_alias (__io_hook_read, __read)
libc_hidden_weak (__read) /* To avoid undefined symbol */
weak_alias (__read, read)

ssize_t
__io_hook_pread (int fd, void *buf, size_t count, const off_t offset)
{
  ssize_t (*hook) (int, void *, size_t, off_t)
    = atomic_forced_read(__pread_hook);
  if (hook)
    return hook(fd, buf, count, offset);
  else
    return __libc_pread(fd, buf, count, offset);
}

weak_alias (__io_hook_pread, __pread)
weak_alias (__pread, pread)
weak_alias (__io_hook_pread, __pread64)
weak_alias (__pread64, pread64)

ssize_t
__io_hook_readv (int fd, const struct iovec *iov, int iovcnt)
{
  ssize_t (*hook) (int, const struct iovec *, int)
    = atomic_forced_read(__readv_hook);
  if (hook)
    return hook(fd, iov, iovcnt);
  else
    return __readv(fd, iov, iovcnt);
}
weak_alias (__io_hook_readv, readv)

ssize_t
__io_hook_write (int fd, const void *buf, size_t count)
{
  ssize_t (*hook) (int, const void *, size_t)
    = atomic_forced_read(__write_hook);
  if (hook)
    return hook(fd, buf, count);
  else
    return __libc_write(fd, buf, count);
}

weak_alias (__io_hook_write, __write)
libc_hidden_weak (__write) /* To avoid undefined symbol */
weak_alias (__write, write)

ssize_t
__io_hook_pwrite (int fd, const void *buf, size_t count, const off_t offset)
{
  ssize_t (*hook) (int, const void *, size_t, off_t)
    = atomic_forced_read(__pwrite_hook);
  if (hook)
    return hook(fd, buf, count, offset);
  else
    return __libc_pwrite(fd, buf, count, offset);
}

weak_alias (__io_hook_pwrite, __pwrite)
weak_alias (__pwrite, pwrite)

weak_alias (__io_hook_pwrite, __pwrite64)
libc_hidden_weak (__pwrite64) /* To avoid undefined symbol */
weak_alias (__pwrite64, pwrite64)

ssize_t
__io_hook_writev (int fd, const struct iovec *iov, int iovcnt)
{
  ssize_t (*hook) (int, const struct iovec *, int)
    = atomic_forced_read(__writev_hook);
  if (hook)
    return hook(fd, iov, iovcnt);
  else
    return __writev(fd, iov, iovcnt);
}
weak_alias (__io_hook_writev, writev)
