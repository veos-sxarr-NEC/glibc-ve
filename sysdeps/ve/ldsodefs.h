/* Run-time dynamic linker data structures for loaded ELF shared objects.
   Copyright (C) 2012-2020 Free Software Foundation, Inc.
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
   License along with the GNU C Library.  If not, see
   <https://www.gnu.org/licenses/>.  */
/* Changes by NEC Corporation for the VE port, 2020*/

#ifndef __LDSODEFS_H

#include <elf.h>

#define LOOKUP_VALUE_ADDRESS(map, set) ((set) || (map) ? (map)->l_addr : 0)
#define SYMBOL_ADDRESS(map, ref, map_set)                               \
  ((ref) == NULL ? 0                                                    \
   : (__glibc_unlikely ((ref)->st_shndx == SHN_ABS) ? 0                 \
      : LOOKUP_VALUE_ADDRESS (map, map_set)) + (ref)->st_value)

struct La_ve_regs;
struct La_ve_retval;

#define ARCH_PLTENTER_MEMBERS						\
    Elf64_Addr (*ve_gnu_pltenter) (Elf64_Sym *, unsigned int,	\
				     uintptr_t *, uintptr_t *,		\
				     struct La_ve_regs *,		\
				     unsigned int *, const char *name,  \
				     long int *framesizep);

#define ARCH_PLTEXIT_MEMBERS						\
    unsigned int (*ve_gnu_pltexit) (Elf64_Sym *, unsigned int,	\
				      uintptr_t *, uintptr_t *,		\
				      const struct La_ve_regs *,	\
				      struct La_ve_retval *,		\
				      const char *);

#include_next <ldsodefs.h>

#endif
