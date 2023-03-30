/* Types for registers for sys/procfs.h.  VE version.
   Copyright (C) 2001-2020 Free Software Foundation, Inc.
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

#ifndef _SYS_PROCFS_H
# error "Never include <bits/procfs.h> directly; use <sys/procfs.h> instead."
#endif

/* Type for a scaler register.  */
typedef unsigned long elf_greg_t;

/* And the whole bunch of them.  We could have used `struct
   user_regs_struct' directly in the typedef, but tradition says that
   the register set is an array, which does have some peculiar
   semantics, so leave it that way.  */
#define ELF_NGREG (sizeof (struct ve_user_regs) / sizeof (elf_greg_t))
typedef elf_greg_t elf_gregset_t[ELF_NGREG];

typedef struct ve_user_vregs elf_fpregset_t;

