/* Copyright (C) 2016 NEC Corporation

   Divide long double (ldbl-128) values, narrowing the result to float.
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


        .macro FUNC_START name
        .text
	.balign	16
        .globl	\name
        .type	\name,@function
#
        .quad   0x0
        .quad   0x0
#
\name:
        .endm

        .macro RETURN
	b.l.t	(,%lr)
        .endm

FUNC_START __fdivl
	lea.sl	%s12,0x7fff0000
	and	%s4,%s12,%s0
	and	%s5,%s0,(16)0
	or	%s5,%s5,%s1
	and	%s6,%s12,%s2
	and	%s7,%s2,(16)0
	or	%s7,%s7,%s3
	subu.l	%s56,%s4,%s6
	brne.l	%s6,%s12,.L13
	brne.l	%s7,0,.L10
.L13:
	brne.l	%s4,%s12,.L14
	brne.l	%s5,0,.L10
.L14:
	brne.l	%s6,%s12,.L15
	breq.l	%s4,%s12,.L10
	br.l.t	.L12
.L15:
	breq.l	%s4,%s12,.L11
#
	brne.l	%s6,0,.L16
	breq.l	%s4,0,.L10
	lea.sl	%s4,0x3ff00000
	fdiv.d	%s6,%s4,%s6
	br.l.t	.L11	
.L16:
#
	or	%s4,%s0,(16)1
	and	%s4,%s4,(2)0
	cmov.l.eq	%s4,%s0,%s0
	or	%s5,0,%s1
	or	%s6,%s2,(16)1
	and	%s6,%s6,(2)0
	or	%s7,0,%s3
#
	cvt.d.q	%s12,%s6
	lea.sl	%s45,0x3ff00000
	fdiv.d	%s45,%s45,%s12
	cvt.q.d	%s46,%s45
#
	fmul.q	%s50,%s6,%s46
	lea.sl	%s54,0x40000000
	or	%s55,0,(0)1
	fsub.q	%s52,%s54,%s50
	fmul.q	%s46,%s46,%s52
	fmul.q	%s48,%s4,%s46
#
	or	%s50,0,%s48
	and	%s51,%s49,(7)1
	or	%s52,0,%s6
	and	%s53,%s7,(7)1
	fmul.q	%s54,%s50,%s52
	fsub.q	%s46,%s4,%s54
	fsub.q	%s54,%s6,%s52
	fmul.q	%s4,%s50,%s54
	fsub.q	%s46,%s46,%s4
	fsub.q	%s50,%s48,%s50
	fmul.q	%s4,%s50,%s52
	fsub.q	%s46,%s46,%s4
	fmul.q	%s4,%s50,%s54
	fsub.q	%s46,%s46,%s4
#
	cvt.d.q	%s45,%s46
	fdiv.d	%s45,%s45,%s12
	cvt.q.d	%s46,%s45
	fadd.q	%s48,%s48,%s46
	brne.l	%s48,0,.L17
	or	%s0,0,%s48
	or	%s1,0,%s49
	RETURN
#
.L17:
	addu.l	%s48,%s48,%s56
	lea.sl	%s12,0x7fff0000
	brge.l	%s48,%s12,.L18
	lea.sl	%s12,0x00010000
	brlt.l	%s48,%s12,.L18
#
	xor	%s0,%s0,%s2
	mrg	%s0,%s48,(1)0
	or	%s1,0,%s49
	RETURN
.L18:
	brlt.l	%s56,0,.L12
	lea.sl	%s12,0x7fefffff
	fadd.d	%s12,%s12,%s12
	br.l.t	.L11
.L10:
	lea.sl	%s0,0xffff8000
	or	%s1,0,(0)1
	RETURN
.L11:
	xor	%s0,%s0,%s2
	lea.sl	%s12,0x7fff0000
	mrg	%s0,%s12,(1)0
	or	%s1,0,(0)1
	RETURN
.L12:
	xor	%s0,%s0,%s2
	mrg	%s0,0,(1)0
	or	%s1,0,(0)1
	RETURN
libc_hidden_def (__fdivl)
weak_alias (__fdivl, fdivl)
weak_alias (__fdivl, f32divf64x)
weak_alias (__fdivl, f32divf128)
