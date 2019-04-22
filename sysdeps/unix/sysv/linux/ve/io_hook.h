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

#ifndef _IO_HOOK_H_

#include <sys/types.h>
#include <stdint.h>

#ifdef _LIBC
# define __IO_HOOK_VOLATILE
#else
# define __IO_HOOK_VOLATILE volatile
#endif

__BEGIN_DECLS

extern ssize_t (*__IO_HOOK_VOLATILE __read_hook) (int, void *, size_t);
extern ssize_t (*__IO_HOOK_VOLATILE __pread_hook) (int, void *, size_t, off_t);
extern ssize_t (*__IO_HOOK_VOLATILE __readv_hook) (int, const struct iovec *,
                                                   int);
extern ssize_t (*__IO_HOOK_VOLATILE __preadv_hook) (int, const struct iovec *,
                                                    int, off_t);
extern ssize_t (*__IO_HOOK_VOLATILE __write_hook) (int, const void *, size_t);
extern ssize_t (*__IO_HOOK_VOLATILE __pwrite_hook) (int, const void *,
                                                    size_t, off_t);
extern ssize_t (*__IO_HOOK_VOLATILE __writev_hook) (int, const struct iovec *,
                                                    int);
extern ssize_t (*__IO_HOOK_VOLATILE __pwritev_hook) (int, const
                                                     struct iovec *,
                                                     int, off_t);

__END_DECLS

#endif /* _IO_HOOK_H_ */
