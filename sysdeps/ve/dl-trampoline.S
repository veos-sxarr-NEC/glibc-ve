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
	/* Reserve the space on stack(%s11) for 7 register */
        lea    %s11, -72(,%s11)
	st     %lr, 0x0(0,%sp)
	st     %s0, 0x8(0,%sp)
	st     %s1, 0x10(0,%sp)
	st     %s2, 0x18(0,%sp)
	st     %s3, 0x20(0,%sp)
	st     %s4, 0x28(0,%sp)
	st     %s5, 0x30(0,%sp)
	st     %s6, 0x38(0,%sp)
	st     %s7, 0x40(0,%sp)

	/* Stack reserved for %s0 to %s7, %s8-%s11, plt(%s16),
	got(%s15) and %s18-%s33  and some extra buffer */

	lea     %s11, -176-80(,%s11)
        /* #1346 ABI change */
	ld	%s0, 0x10(, %s62)	/* addr of link_map object, got[2]*/
	or	%s1, 0, %s13		/* Index of PLT in bin/lib, the symbol which we are trying to resolve */

	lea     %s12, _dl_fixup@PC_LO(-24)
        and     %s12, %s12,(32)0
        sic     %s62
        lea.sl  %s12, _dl_fixup@PC_HI(%s12,%s62)
        bsic    %lr, (%s12)		/* Fetch the address of dl_fixup() and invoke. */
	
	or	%s63, %s0, (0)1		# Save the function addr in %s63 */
	lea     %s11, 176+80(,%s11)
	# Get the saved register content back.
	ld     %lr, 0x0(0,%sp)
	ld     %s0, 0x8(0,%sp)
        ld     %s1, 0x10(0,%sp)
        ld     %s2, 0x18(0,%sp)
        ld     %s3, 0x20(0,%sp)
        ld     %s4, 0x28(0,%sp)
	ld     %s5, 0x30(0,%sp)
	ld     %s6, 0x38(0,%sp)
	ld     %s7, 0x40(0,%sp)
        lea    %s11, 72(,%s11)

	b.l     (,%s63)  /* Finally jumping to actual function */
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

