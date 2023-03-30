/* Defines for bits in AT_HWCAP.
   Copyright (C) 2012-2015 Free Software Foundation, Inc.
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
/* Changes by NEC Corporation for the VE port, 2020 */

#ifndef _SYS_AUXV_H
# error "Never include <bits/hwcap.h> directly; use <sys/auxv.h> instead."
#endif

/* Bits defined for VE architecture.  */
/* The following must match the kernel's <asm/hwcap.h>.  */
#define HWCAP_VE_VE1           (0x0)
#define HWCAP_VE_VE3           (0x1)
