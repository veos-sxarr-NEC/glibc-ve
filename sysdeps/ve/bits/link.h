/* Copyright (C) 2005-2020 Free Software Foundation, Inc.
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
/* Changes by NEC Corporation for the VE port, 2020 */

#ifndef	_LINK_H
# error "Never include <bits/link.h> directly; use <link.h> instead."
#endif


/* Registers for entry into PLT on Alpha.  */
typedef struct La_ve_regs
{
  uint64_t lr_s0;
  uint64_t lr_s1;
  uint64_t lr_s2;
  uint64_t lr_s3;
  uint64_t lr_s4;
  uint64_t lr_s5;
  uint64_t lr_fp;
  uint64_t lr_sp;
} La_ve_regs;

/* Return values for calls from PLT on VE architecture.  */
typedef struct La_ve_retval
{
  uint64_t lrv_s0;
  uint64_t lrv_s1;
  long double lrv_st0;
  long double lrv_st1;
} La_ve_retval;


__BEGIN_DECLS

extern Elf64_Addr la_ve_gnu_pltenter (Elf64_Sym *__sym,
					 unsigned int __ndx,
				         uintptr_t *__refcook,
				         uintptr_t *__defcook,
				         La_ve_regs *__regs,
				         unsigned int *__flags,
				         const char *__symname,
				         long int *__framesizep);
extern unsigned int la_ve_gnu_pltexit (Elf64_Sym *__sym,
					  unsigned int __ndx,
					  uintptr_t *__refcook,
					  uintptr_t *__defcook,
					  const La_ve_regs *__inregs,
					  La_ve_retval *__outregs,
					  const char *__symname);

__END_DECLS
