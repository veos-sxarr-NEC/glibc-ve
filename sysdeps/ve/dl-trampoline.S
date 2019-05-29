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

/* PLT trampolines.  VE version. */

#include <config.h>
#include <sysdep.h>
#include <link-defines.h>

/* Area on stack to save and restore registers used for parameter
   passing when calling _dl_fixup.  */

	.text
	.globl _dl_runtime_resolve
	.type _dl_runtime_resolve, @function
	.balign 16
_dl_runtime_resolve:
	/* Increment the space on stack for all scaler register */
	/* 176: Callee save register
	   80: Caller argument with buffer
	   72: %s0 - %s7, %s10
	   240: %s34 - %s63 (30 registers) */

	st     %lr, 0x8(0,%sp)
        lea    %s11, -176-80-72-248(,%s11)
	brge.l.t	%s11, %s8, 0x48
	ld      %s10, 0x18(0,%s14)
	lea     %s12, 0x13b
	shm.l   %s12, 0x0(%s10)
	shm.l   %s8, 0x8(%s10)
	shm.l   %s11, 0x10(%s10)
	or      %s12, 0, %s0
	monc
	or      %s0, 0, %s12
	/* Stack reserved for %s0 to %s7, %s10-%s11, plt(%s16),
           got(%s15), %s18-%s33  , %s34-%s63 and some extra buffer */
	st     %s0, 0x108(0,%sp)
	st     %s1, 0x110(0,%sp)
	st     %s2, 0x118(0,%sp)
	st     %s3, 0x120(0,%sp)
	st     %s4, 0x128(0,%sp)
	st     %s5, 0x130(0,%sp)
	st     %s6, 0x138(0,%sp)
	st     %s7, 0x140(0,%sp)

	/* Saving temporary register %s34-%s63 */
	st     %s34, 0x148(0,%sp)
	st     %s35, 0x150(0,%sp)
	st     %s36, 0x158(0,%sp)
	st     %s37, 0x160(0,%sp)
	st     %s38, 0x168(0,%sp)
	st     %s39, 0x170(0,%sp)
	st     %s40, 0x178(0,%sp)
	st     %s41, 0x180(0,%sp)
	st     %s42, 0x188(0,%sp)
	st     %s43, 0x190(0,%sp)
	st     %s44, 0x198(0,%sp)
	st     %s45, 0x1a0(0,%sp)
	st     %s46, 0x1a8(0,%sp)
	st     %s47, 0x1b0(0,%sp)
	st     %s48, 0x1b8(0,%sp)
	st     %s49, 0x1c0(0,%sp)
	st     %s50, 0x1c8(0,%sp)
	st     %s51, 0x1d0(0,%sp)
	st     %s52, 0x1d8(0,%sp)
	st     %s53, 0x1e0(0,%sp)
	st     %s54, 0x1e8(0,%sp)
	st     %s55, 0x1f0(0,%sp)
	st     %s56, 0x1f8(0,%sp)
	st     %s57, 0x200(0,%sp)
	st     %s58, 0x208(0,%sp)
	st     %s59, 0x210(0,%sp)
	st     %s60, 0x218(0,%sp)
	st     %s61, 0x220(0,%sp)
	st     %s62, 0x228(0,%sp)
	st     %s63, 0x230(0,%sp)
	/* #1583 trac */
	spm    %s63
	sfr    %s61
	st     %s63, 0x238(0,%sp)      /* Store the content in stack */
	st     %s61, 0x240(0,%sp)      /* Store the content in stack */
	lfr    0		       /* load the psw[58..63] with 0 */
	lea    %s61, 0x800
	lpm    %s61		       /* load the psw[50..57] with 0 */

        /* #1346 ABI change */
	ld	%s0, 0x10(, %s62)	/* addr of link_map object, got[2]*/
	or	%s1, 0, %s13		/* Index of PLT in bin/lib, the symbol which we are trying to resolve */

	lea     %s12, _dl_fixup@PC_LO(-24)
        and     %s12, %s12,(32)0
        sic     %s62
        lea.sl  %s12, _dl_fixup@PC_HI(%s12,%s62)
        bsic    %lr, (%s12)		/* Fetch the address of dl_fixup() and invoke. */
	
	or	%s12, %s0, (0)1		# Save the function addr in %s12 */

	/* #1583 trac */
	ld     %s63, 0x238(0,%sp)
	ld     %s61, 0x240(0,%sp)
        lpm    %s63                    #load the psw[50..57] with s63[50..57]
        lfr    %s61                    #load the psw[58..63] with s61[58..63]

	# Get the saved register content back.
	ld	%s0, 0x108(0,%sp)
        ld	%s1, 0x110(0,%sp)
        ld	%s2, 0x118(0,%sp)
        ld	%s3, 0x120(0,%sp)
        ld	%s4, 0x128(0,%sp)
	ld	%s5, 0x130(0,%sp)
	ld	%s6, 0x138(0,%sp)
	ld	%s7, 0x140(0,%sp)

	ld     %s34, 0x148(0,%sp)
        ld     %s35, 0x150(0,%sp)
        ld     %s36, 0x158(0,%sp)
        ld     %s37, 0x160(0,%sp)
        ld     %s38, 0x168(0,%sp)
        ld     %s39, 0x170(0,%sp)
        ld     %s40, 0x178(0,%sp)
        ld     %s41, 0x180(0,%sp)
        ld     %s42, 0x188(0,%sp)
        ld     %s43, 0x190(0,%sp)
        ld     %s44, 0x198(0,%sp)
        ld     %s45, 0x1a0(0,%sp)
        ld     %s46, 0x1a8(0,%sp)
        ld     %s47, 0x1b0(0,%sp)
        ld     %s48, 0x1b8(0,%sp)
        ld     %s49, 0x1c0(0,%sp)
        ld     %s50, 0x1c8(0,%sp)
        ld     %s51, 0x1d0(0,%sp)
        ld     %s52, 0x1d8(0,%sp)
        ld     %s53, 0x1e0(0,%sp)
        ld     %s54, 0x1e8(0,%sp)
        ld     %s55, 0x1f0(0,%sp)
        ld     %s56, 0x1f8(0,%sp)
        ld     %s57, 0x200(0,%sp)
        ld     %s58, 0x208(0,%sp)
        ld     %s59, 0x210(0,%sp)
        ld     %s60, 0x218(0,%sp)
        ld     %s61, 0x220(0,%sp)
        ld     %s62, 0x228(0,%sp)
        ld     %s63, 0x230(0,%sp)

	/* Do stack unwinding */
        lea	%s11, 176+80+72+248(,%s11)
	ld      %lr, 0x8(0,%sp)

	b.l     (,%s12)  /* Finally jumping to actual function */
	/* We are done with the function calling, lets wind up */
	.size _dl_runtime_resolve, .-_dl_runtime_resolve /* End of function */



	.globl _dl_runtime_profile
	.type _dl_runtime_profile, @function
	.align 16

_dl_runtime_profile:
	
	stl %s1, LR_S1_OFFSET(%sp)
	stl %s2, LR_S2_OFFSET(%sp)
	stl %s3, LR_S3_OFFSET(%sp)
	stl %s4, LR_S4_OFFSET(%sp)
	stl %s5, LR_S5_OFFSET(%sp)
	stl %s6, LR_S6_OFFSET(%sp)
	stl %fp, LR_fp_OFFSET(%sp)

	or %s33, %lr, (0)1  /* Saving return address */
	or %s62, %s0, (0)1
	
	stl %s0, LR_SP_OFFSET(%sp)

        ld %s0, LR_S1_OFFSET(,%sp)
        ld %s1, LR_S2_OFFSET(,%sp)
	or %s2, %s62, (0)1
	ld %s3, LR_S4_OFFSET(,%sp)
	ld %s4, LR_fp_OFFSET(,%sp)
	ld      %sp, -176(%sp)
        /* Call resolver */
	lea     %s12, _dl_profile_fixup@PC_LO(-24)
        and     %s12, %s12,(32)0
        sic     %s62
        lea.sl  %s12, _dl_profile_fixup@PC_HI(%s12,%s62)
        bsic    %lr, (%s12)
        or 	%s63, %s0, (0)1	    /* Save return value */
	ld      %sp, 176(%sp)
        ld  %s4, LR_S4_OFFSET(%sp)
        ld  %s5, LR_S5_OFFSET(%sp)
        ld  %s6, LR_S6_OFFSET(%sp)
	
	ld	%s61, -176(%sp)
	and     %s61, %s61, %s61        /* Check address is zero or not */

	brgt.w  0, %s61, .L4.3          /* If address is positive then do nothing */
        br.l    .L4.1
.L4.1:	
        /* There's nothing in the frame size, so there
           will be no call to the _dl_call_pltexit. */

        /* Get back registers content.  */
	ld  %s1, LR_S1_OFFSET(%sp)
        ld  %s2, LR_S2_OFFSET(%sp)
        ld  %s3, LR_S3_OFFSET(%sp)

	b.l     (,%s33)		/* Jump to function address */
.L4.3:
        /* At this point we need to prepare new stack for the function
           which has to be called.  We copy the original stack to a
           temporary buffer of the size specified by the 'framesize'
           returned from _dl_profile_fixup */
	ld %s2, LR_S3_OFFSET(%sp)
        ld %s1, LR_S1_OFFSET(%sp)
        ld %s0, LR_S2_OFFSET(%sp)

	ld      %sp, -176(%sp)
	lea     %s12, _dl_call_pltexit@PC_LO(-24)
        and     %s12, %s12,(32)0
	sic     %s62
        lea.sl  %s12, _dl_call_pltexit@PC_HI(%s12,%s62)
        bsic    %lr, (%s12)
	ld      %sp, 176(%sp)

        b.l     (,%s33)
	.size _dl_runtime_profile, .-_dl_runtime_profile

