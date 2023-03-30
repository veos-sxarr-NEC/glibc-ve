/* Linux read syscall implementation.
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
#include <sysdep-cancel.h>
#include <io_hook.h>

#if IS_IN (libc) || IS_IN(rtld)
#define weak_variable weak_function
ssize_t weak_variable (* __read_hook)
  (int, void *, size_t) = NULL;
#endif

/* Read NBYTES into BUF from FD.  Return the number read or -1.  */
ssize_t
__libc_read (int fd, void *buf, size_t nbytes)
{
  ssize_t (*hook) (int, void *, size_t)
    = atomic_forced_read(__read_hook);
  if (hook)
    return hook(fd, buf, nbytes);
  else
    return SYSCALL_CANCEL (read, fd, buf, nbytes);
}
libc_hidden_def (__libc_read)

libc_hidden_def (__read)
weak_alias (__libc_read, __read)
libc_hidden_def (read)
weak_alias (__libc_read, read)
