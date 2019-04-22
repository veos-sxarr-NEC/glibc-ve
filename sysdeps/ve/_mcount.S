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

/* Machine-specific calling sequence for `mcount' profiling function.  x86-64 version.

/* Assembly stub to invoke _mcount().  Compiler generated code calls
   this stub after executing a function's prologue and without saving any
   registers.  It is therefore necessary to preserve %rcx, %rdx, %rsi, %rdi,
   %r8, %r9 as they may contain function arguments.  */

#include <sysdep.h>

	.globl C_SYMBOL_NAME(_mcount)
	.type C_SYMBOL_NAME(_mcount), @function
	.align ALIGNARG(4)
C_LABEL(_mcount)
	/* Allocate space for 10(%lr, %got, %s0-%s7) registers.  */
	lea    %sp, -80(,%sp)
	st     %lr, 0x0(0,%sp)
	st     %s0, 0x8(0,%sp)
	st     %s1, 0x10(0,%sp)
	st     %s2, 0x18(0,%sp)
	st     %s3, 0x20(0,%sp)
	st     %s4, 0x28(0,%sp)
	st     %s5, 0x30(0,%sp)
	st     %s6, 0x38(0,%sp)
	st     %s7, 0x40(0,%sp)
	st     %got,0x48(0,%sp)
	lea    %got,_GLOBAL_OFFSET_TABLE_@PC_LO(-24)
	and    %got,%got,(32)0
	sic    %plt
	lea.sl %got,_GLOBAL_OFFSET_TABLE_@PC_HI(%got,%plt)

	lea     %sp, -176-16(,%sp)
	/* Setup parameter for __mcount_internal. */
	/* %s0 and %s1 are filled and passed from binary itself*/
	/* frompc(%s0) is the return address from where the function is invoked */
	st	%s1, -176(,%sp) /* Saving the return address on stack */
	or 	%s0, 0, %s1
	/* Get selfpc(%s1) via the starting address of the function. */
	/* starting address should be return address of mcount */
	or	%s1, 0, %lr
	st	%s1, -176-8(,%sp) /* Saving function address on stack */
#ifndef PIC
        lea     %s12, __mcount_internal@CALL_LO
        and     %s12, %s12,(32)0
        lea.sl  %s12, __mcount_internal@CALL_HI(,%s12)
#else
        lea     %s12, __mcount_internal@PLT_LO(-24)
        and     %s12,%s12,(32)0
        sic     %s55
        lea.sl  %s12, __mcount_internal@PLT_HI(%s12,%s55)
#endif
        bsic    %lr, (%s12)
	lea     %sp, 176+16(,%sp)

	/* Pop the saved registers.  Please note that `mcount' has no
	   return value.  */

	ld     %lr, 0x0(0,%sp)
	ld     %s0, 0x8(0,%sp)
        ld     %s1, 0x10(0,%sp)
        ld     %s2, 0x18(0,%sp)
        ld     %s3, 0x20(0,%sp)
        ld     %s4, 0x28(0,%sp)
	ld     %s5, 0x30(0,%sp)
	ld     %s6, 0x38(0,%sp)
	ld     %s6, 0x40(0,%sp)
	ld     %got,0x48(0,%sp)
        lea    %s11, 80(,%s11)

	b.l     (,%lr)
	ASM_SIZE_DIRECTIVE(C_SYMBOL_NAME(_mcount))

#undef mcount
weak_alias (_mcount, mcount)
