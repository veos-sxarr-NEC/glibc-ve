/* Copyright (C) 2003-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Jakub Jelinek <jakub@redhat.com>, 2003.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */
/* Changes by NEC Corporation for the VE port, 2020 */

#ifndef _VE_LOWLEVELLOCK_H
#define _VE_LOWLEVELLOCK_H 1

#include <sysdeps/nptl/lowlevellock.h>

#ifdef lll_futex_wake
#undef lll_futex_wake
#define lll_futex_wake(ftx, nr, private)                                \
        lll_futex_syscall (3, ftx,                                      \
                        __lll_private_flag (FUTEX_WAKE, private), nr)
#endif
#endif  /* lowlevellock.h */

