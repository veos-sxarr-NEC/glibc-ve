/* Copyright (C) 1991-2020 Free Software Foundation, Inc.
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

#include <sysdeps/unix/sysdep.h>
#include <sysdeps/ve/sysdep.h>

#ifdef	__ASSEMBLER__

/* This is defined as a separate macro so that other sysdep.h files
   can include this one and then redefine DO_CALL.  */

#define DO_CALL(syscall_name, args)					      \
  lea %s0,SYS_ify (syscall_name);					      \
  or %s34,10,(0)1 ;          /* VLFA DELAY */
  brgt.w  0,%s34,32;
  fencei ;
  subs.w.sx  %s34,%s34,(63)0;
  br.l -24;
  monc;

#define	r0		%s0	/* Normal return-value register.  */
#define	r1		%s1	/* Secondary return-value register.  */
#define ret             DO_RET

#endif	/* __ASSEMBLER__ */
