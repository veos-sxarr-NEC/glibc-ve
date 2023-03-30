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
#define SS_ONSTACK 1
	.section .rodata.str1.1,"aMS",@progbits,1
	.type	longjmp_msg,@object
	.global longjmp_msg
longjmp_msg:
	.string "longjmp causes uninitialized stack frame"
	.size	longjmp_msg, .-longjmp_msg


#ifdef PIC
#define CALL_FAIL	addi %s59,0,longjmp_msg@got_lo; 		\
			lea.sl	%s59, longjmp_msg@got_hi(%s59, %s15);	\
			ld	%s0, (,%s59);	/*		\
			HIDDEN_JUMPTARGET(__fortify_fail)*/
#else
#define CALL_FAIL	lea	%s0,	longjmp_msg;	/*	\
			HIDDEN_JUMPTARGET(__fortify_fail)*/
#endif


	.text
ENTRY (____longjmp_chk)

/* Check the stack pointer held in the jumpbuf.  Make sure it's in either the
   current stack (r12) or in the signal stack.  */
	ld	%fp, 0x0(0, %s0)	/* Fetch Frame Pointer. */
	ld	%lr, 0x8(0, %s0)	/* Fetch Link Register. */
	ld	%s63, 0x10(0, %s0)	/* Fetch Stack Pointer. */
	ld	%s15, 0x20(0, %s0)	/* Fetch PLT Register. */
	ld	%s16, 0x28(0, %s0)	/* Fetch GOT Register. */
	brge.l.t	%s63, %sp, 3f

	/* Save function parameters. */
	or	%s62, %s0, (0)1
	or	%s61, %s1, (0)1

	/* call sigaltstack system call */
	xor	%s1, %s1, %s1
	lea	%sp, -24(,%sp)
	or	%s2, %sp, (0)1
	lea	%s38, __NR_sigaltstack
	or	%s0, %s38, (0)1
	ldl.sx	%s60, SHM_OFFSET(,%tp)
	shm.l	%s0, 0x0(%s60)
	shm.l	%s1, 0x8(%s60)
	shm.l	%s2, 0x10(%s60)
	or %s34,10,(0)1 ;          /* VLFA DELAY */
	brgt.w  0,%s34,32;
	fencei ;
	subs.w.sx  %s34,%s34,(63)0;
	br.l -24;
	nop;
	monc

	/* check return val */
	brne.l.t 0, %s0, 2f

	/* check if SS_ONSTACK set*/
	ldl.sx	%s0, 0x8(,%sp)
	lea	%s38, SS_ONSTACK
	or	%s1, %s38, (0)1
	breq.l.t	%s0, %s1, 1f

	ld	%s0, 0x0(,%sp)
	ld	%s1, 0x10(,%sp)
	addu.l	%s0, %s0, %s1
	subu.l	%s0, %s0, %s63
	brge.l.t	%s0, %s1, 2f

	/* call fail */
1:	CALL_FAIL

	/* Fetch the Other Callee Saved Registers. */
2:	lea	%sp, 24(,%sp)
	or	%s0, %s62, (0)1
	or	%s1, %s61, (0)1

        # Fetch the Callee Saved Registers
3:      ld      %s14, 0x28(0, %s0)
        ld      %s15, 0x30(0, %s0)
        ld      %s16, 0x38(0, %s0)
        ld      %s17, 0x40(0, %s0)
        ld      %s18, 0x48(0, %s0)
        ld      %s19, 0x50(0, %s0)
        ld      %s20, 0x58(0, %s0)
        ld      %s21, 0x60(0, %s0)
        ld      %s22, 0x68(0, %s0)
        ld      %s23, 0x70(0, %s0)
        ld      %s24, 0x78(0, %s0)
        ld      %s25, 0x80(0, %s0)
        ld      %s26, 0x88(0, %s0)
        ld      %s27, 0x90(0, %s0)
        ld      %s28, 0x98(0, %s0)
        ld      %s29, 0xA0(0, %s0)
        ld      %s30, 0xA8(0, %s0)
        ld      %s31, 0xB0(0, %s0)
        ld      %s32, 0xB8(0, %s0)
        ld      %s33, 0xC0(0, %s0)
	/* Store %s1 to %s0. */
	or	%sp, %s63, (0)1
	or	%s0, %s1, (0)1
	beq.l.t	0, 0x0(,%lr)
END (____longjmp_chk)
