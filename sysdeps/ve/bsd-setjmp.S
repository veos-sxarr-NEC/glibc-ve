/* Copyright (C) 2017-2019 NEC Corporation
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it
   and/or modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either version
   2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.
*/

/* BSD `setjmp' entry point to `sigsetjmp (..., 1)'.  VE version.

/* This just does a tail-call to `__sigsetjmp (ARG, 1)'.
   We cannot do it in C because it must be a tail-call, so frame-unwinding
   in setjmp doesn't clobber the state restored by longjmp.  */

#include <sysdep.h>
#define _ASM
#define _SETJMP_H
#include <bits/setjmp.h>

ENTRY (setjmp)
	/* Set up arguments, we only need to set the 2nd arg.  */
	or	%s1, 1, %s1
	lea %got,_GLOBAL_OFFSET_TABLE_@PC_LO(-24)
	and %got,%got,(32)0
	sic %plt
	lea.sl %got,_GLOBAL_OFFSET_TABLE_@PC_HI(%plt,%got)
	JUMPTARGET (__sigsetjmp)
END (setjmp)
