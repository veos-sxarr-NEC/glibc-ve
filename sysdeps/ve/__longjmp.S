/* Copyright (C) 2020 NEC Corporation
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
   <https://www.gnu.org/licenses/>.
*/


#include <sysdep.h>
#include <jmpbuf-offsets.h>
#include <asm-syntax.h>
#include <stap-probe.h>

/* Jump to the position specified by ENV, causing the
   setjmp call there to return VAL, or 1 if VAL is 0.
   void __longjmp (__jmp_buf env, int val).  */
	.text
ENTRY(__longjmp)
	/* Restore registers. */
	/* if (arg2) jump else	increment arg2. */
	brne.w	0, %s1, 1f
	addu.w	%s1, 1, %s1

1:	ld      %sl, 0x0(0, %s0)       /* Fetch Stack Limit */
	ld	%fp, 0x8(, %s0)		/* Fetch Frame Pointer. */
	ld	%lr, 0x10(, %s0)		/* Fetch Link Register. */
	ld	%sp, 0x18(, %s0)	/* Fetch Stack Pointer. */
	ld	%s12, 0x20(, %s0)	/* Fetch PLT Register. */
	ld	%s13, 0x28(, %s0)	/* Fetch GOT Register. */
	/* Fetch the Other Callee Saved Registers. */
	ld	%s15, 0x30(, %s0)
	ld	%s16, 0x38(, %s0)
	ld	%s17, 0x40(, %s0)
	ld	%s18, 0x48(, %s0)
	ld	%s19, 0x50(, %s0)
	ld	%s20, 0x58(, %s0)
	ld	%s21, 0x60(, %s0)
	ld	%s22, 0x68(, %s0)
	ld	%s23, 0x70(, %s0)
	ld	%s24, 0x78(, %s0)
	ld	%s25, 0x80(, %s0)
	ld	%s26, 0x88(, %s0)
	ld	%s27, 0x90(, %s0)
	ld	%s28, 0x98(, %s0)
	ld	%s29, 0xa0(, %s0)
	ld	%s30, 0xa8(, %s0)
	ld	%s31, 0xb0(, %s0)
	ld	%s32, 0xb8(, %s0)
	ld	%s33, 0xc0(, %s0)
	/* Store %s1 to %s0. */
	or	%s0, %s1, (0)1
	beq.l.t 0,0x0(,%lr)

END (__longjmp)
