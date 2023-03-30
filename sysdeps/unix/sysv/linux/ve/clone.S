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


/* clone() is even more special than fork() as it mucks with stacks
   and invokes a function in the right context after its all over.  */

/* The argument passing order and corresponding registers is as follows:

   wrapper(clone):
   __clone(fn,	stack,	flags,	arg, ...
   &new->tid, TP_ADJ(new), &new->tid)
   VE convention: args need to fetch from stack as,
   fn =>			%s0 =>	1st_arg =>	176+8*0 (SP)
   stack =>			%s1 =>	2nd_arg =>	176+8*1 (SP)
   flags =>			%s2 =>	3rd arg =>	176+8*2 (SP)
   arg =>			%s3 =>	4th arg =>	176+8*3 (SP)
   ptid or &new->tid =>		%s4 =>	5th_arg =>	176+8*4 (SP)
   TLS or TP_ADJ(new) =>	%s5 =>	6th arg =>	176+8*5 (SP)
   ctid or &new->tid =>		%s6 =>	7th arg =>	176+8*6 (SP)
   Guard pointer        =>      %s7 =>  8th arg =>      176+8*7 (SP)
   syscall(clone):
   clone(	flags,	stack,	ptid,	ctid,	tls)
   VE convention:
   s0(nr)	s1,	s2,	s3,	s4,	s5
*/

 #include <sysdep.h>
 #define _ERRNO_H        1
 #include <bits/errno.h>

.section data
.set CLONE_THREAD, 0x00010000
.set CLONE_VM, 0x00000100
	.text
 ENTRY (__clone)

	/* prepare registers/arguments for clone system call */
	ld	%s0, 0xB0(0,%sp)	/* %s0	= 1st arg of function */
	ld	%s1, 0xB8(0,%sp)	/* %s1	= 2nd arg of function */

	/* return -EINVAL, if function or stack is NULL */
	breq.l   0, %s0, error          /* check for NULL function */
	breq.l   0, %s1, error          /* check for NULL stack */

	ldl.sx	%s2, 0xC0(0,%sp)	/* %s2	= 3rd arg of function */
	ld	%s3, 0xC8(0,%sp)	/* %s3	= 4th arg of function */
	ld	%s4, 0xD0(0,%sp)	/* %s4	= 5th arg of function */
	ld	%s5, 0xD8(0,%sp)	/* %s5	= 6th arg of function */
	ld	%s6, 0xE0(0,%sp)	/* %s6	= 7th arg of function */
	ld      %s7, 0xE8(0,%s11)       /* %s7 = 8th arg of function */
	
	or	%s62, %s0, (0)1		/* fn	= %s62	[save fn in s62]     */

	/* Get page size and set initial stack limit */
        lea     %sp, -176-80(,%sp)

	st	%s10, 176+40(,%sp)
	st	%s15, 176+48(,%sp)
	st	%s62, 176+56(,%sp)

	addi %got,-16,_GLOBAL_OFFSET_TABLE_@PC_LO
        sic %plt
        lea.sl %got,_GLOBAL_OFFSET_TABLE_@PC_HI(%plt,%got)

#ifndef SHARED
	addi %s12,0,__getpagesize@CALL_LO
        lea.sl  %s12, __getpagesize@CALL_HI(,%s12)
#else
	addi %s12,-16,__getpagesize@PLT_LO
        sic     %s55
        lea.sl  %s12, __getpagesize@PLT_HI(%s12,%s55)
#endif
        bsic    %lr, (%s12)

	ld	%s62, 176+56(,%sp)
	ld	%s15, 176+48(,%sp)
	ld	%s10, 176+40(,%sp)

        lea     %sp,  176+80(,%sp)

	or	%s52, %s0, (0)1

	or	%s0, SYS_ify(clone), (0)1
					/* %s0	= SYS_clone		     */
	or	%s63, %s1, (0)1		/* %s63 = temp. hold child SP	     */
	or	%s1, %s2, (0)1		/* %s1 = flags			     */
	or	%s47, %s63, (0)1	/* %s47 = SP, used as FP for child   */
	lea     %s2, -0xB0(,%s47)       /* %s2  = stack pointer
					also reserve memory for callee 
					saved register used in child context */
	or	%s61, %s3, (0)1		/* %s61 = args [save arg in 61]      */
	or	%s3, %s4, (0)1		/* %s3	= ptid			     */
	or	%s4, %s6, (0)1		/* %s4	= ctid			     */
					/* %s5 already has TLS		     */
	xor	%s6, %s6, %s6		/* %s6	= Copy of flags (for child)  */

	/* SHM and MONC of clone system call */
	xor	%s46, %s46, %s46
	ld	%s46, SHM_OFFSET(, %tp)	/* Fetch shared SHM offset	     */

	shm.l	%s0, 0x0(%s46)		/* %s0 = SYS_clone		     */
	shm.l	%s1, 0x8(%s46)		/* %s1 = flags 	(1st arg)	     */
	shm.l	%s2, 0x10(%s46)		/* %s2 = stack pointer (2nd arg)     */
	shm.l	%s3, 0x18(%s46)		/* %s3 = ptid		(3nd arg)    */
	shm.l	%s4, 0x20(%s46)		/* %s4 = ctid		(4th arg)    */
	shm.l	%s5, 0x28(%s46)		/* %s5 = TLS		(5th arg)    */
	shm.l   %s7, 0x30(%s46)         /* %s7 = Guard (Guard Pointer) */
	or	%s49, 0, %s1		/* Store value of s1 in temp register */

	or %s34,10,(0)1 ;          /* VLFA DELAY */
	brgt.w  0,%s34,32;
	fencei ;
	subs.w.sx  %s34,%s34,(63)0;
	br.l -24;
	monc				/* syscall()			     */
	brne.l.t 0, %s0, 3f		/* Return from parent context	     */

	/* Execute in child context & clear LR to zero		     */

	xor	%s9, %s9, %s9		/* Clear the frame pointer	     */
	or      %s9, %s47,(0)1          /* Set FP at initial SP 	     */

	subu.l  %s58, %s47, %s52
	lea     %s47, 0x180DC8          /* #1002: stack area reserved for signal */
	addu.l  %s58, %s47, %s58
        or      %s8, %s58,(0)1           /* set initial stack_limit */

	or	%s0, %s61, (0)1		/* %s0 = Functions Args pointer      */
	or	%s12, %s62, (0)1	/* %s12 = Child function pointer     */
	bsic	%s10, 0x0(, %s12)	/* Invoke child function	     */

	/* Prepare Arguments for exit system call */
	or	%s1, %s0, (0)1		/* %s1 = exit status		     */
	or	%s0, SYS_ify(exit), (0)1	/* %s0 = SYS_exit	     */

	/* SHM and MONC of exit system call */
	xor	%s46, %s46, %s46
	ld	%s46, SHM_OFFSET(, %tp)		/* Fetch SHM area offset    */
	shm.l	%s0, 0x0(%s46)
	shm.l	%s1, 0x8(%s46)
	or %s34,10,(0)1 ;          /* VLFA DELAY */
	brgt.w  0,%s34,32;
	fencei ;
	subs.w.sx  %s34,%s34,(63)0;
	br.l -24;
	monc
3:	beq.l.t 0,0x0(, %lr)
error:
	/* Setting EINVAL */
	or	%s0, -22, (0)1
	beq.l.t 0,0x0(, %lr)

PSEUDO_END (__clone)
libc_hidden_def (__clone)
