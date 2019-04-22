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

/* BSD `_setjmp' entry point to `sigsetjmp (..., 0)'.  VE version.

/* This just does a tail-call to `__sigsetjmp (ARG, 0)'.
   We cannot do it in C because it must be a tail-call, so frame-unwinding
   in setjmp doesn't clobber the state restored by longjmp.  */

#include <sysdep.h>
#define _ASM
#define _SETJMP_H
#include <bits/setjmp.h>

ENTRY (_setjmp)
	/* Set up arguments, we only need to set the second arg.  */
	xor	%s1, %s1, %s1
	st      %sl, 0x0(0, %s0)        #Save Stack Limit
	st      %fp, 0x8(0, %s0)        #Save Frame Pointer
        st      %lr, 0x10(0, %s0)        #Save Link Register
        st      %sp, 0x18(0, %s0)       #Save Stack Pointer
        st      %s12, 0x20(0, %s0)      #Save Outer Register
        st      %s13, 0x28(0, %s0)      #Save Link area Register
        # Saving Callee Saved Registers
        st      %s15, 0x30(0, %s0)
        st      %s16, 0x38(0, %s0)
        st      %s17, 0x40(0, %s0)
        st      %s18, 0x48(0, %s0)
        st      %s19, 0x50(0, %s0)
        st      %s20, 0x58(0, %s0)
        st      %s21, 0x60(0, %s0)
        st      %s22, 0x68(0, %s0)
        st      %s23, 0x70(0, %s0)
        st      %s24, 0x78(0, %s0)
        st      %s25, 0x80(0, %s0)
        st      %s26, 0x88(0, %s0)
        st      %s27, 0x90(0, %s0)
        st      %s28, 0x98(0, %s0)
        st      %s29, 0xA0(0, %s0)
        st      %s30, 0xA8(0, %s0)
        st      %s31, 0xB0(0, %s0)
        st      %s32, 0xB8(0, %s0)
        st      %s33, 0xC0(0, %s0)
#if IS_IN (rtld)
	/* In ld.so we never save the signal mask.  */
	xor	%s0, %s0, %s0
	beq.l.t 0x0, 0x0(,%lr)
#else
	/* Make a tail call to __sigjmp_save; it takes the same args.  */

#ifndef SHARED
	lea	%s12, __sigjmp_save@CALL_LO
	and	%s12,%s12,(32)0
	lea.sl	%s12, __sigjmp_save@CALL_HI(,%s12)
#else
        lea     %s12, __sigjmp_save@PC_LO(-24)
        and     %s12,%s12,(32)0
        sic     %s62
        lea.sl  %s12, __sigjmp_save@PC_HI(%s12,%s62)
#endif
	beq.l.t 0, 0x0(,%s12)
#endif
END (_setjmp)
libc_hidden_def (_setjmp)
