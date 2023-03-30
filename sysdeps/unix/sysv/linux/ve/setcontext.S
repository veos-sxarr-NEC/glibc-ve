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

/* Install given context. */

#include <sysdep.h>

#include "ucontext_i.h"


/*  int __setcontext (const ucontext_t *ucp)

  Restores the machine context in UCP and thereby resumes execution
  in that context.

  This implementation is intended to be used for *synchronous* context
  switches only.  Therefore, it does not have to restore anything
  other than the PRESERVED state.  */

.section        .data
.set err,       -4095

ENTRY(__setcontext)
	/* Save argument since syscall will destroy it.  */
	st	%s0, -8(, %sp)
	lea     %sp, -8(,%sp)
	/* Set the signal mask with
	   rt_sigprocmask (SIG_SETMASK, mask, NULL, _NSIG/8).  */
	lea	%s2, oSIGMASK(, %s0)
	xor	%s3, %s3, %s3
	or	%s1, SIG_SETMASK, (0)1
	or	%s4, _NSIG8, (0)1
	or	%s0, __NR_rt_sigprocmask, (0)1
	ldl.sx	%s60, SHM_OFFSET(, %tp)
	shm.l	%s0, 0x0(%s60)
	shm.l	%s1, 0x8(%s60)
	shm.l	%s2, 0x10(%s60)
	shm.l	%s3, 0x18(%s60)
	shm.l	%s4, 0x20(%s60)
	or %s34,10,(0)1 ;          /* VLFA DELAY */
	brgt.w  0,%s34,32;
	fencei ;
	subs.w.sx  %s34,%s34,(63)0;
	br.l -24;
	monc

	/* check return val */
	lea %s59, err
	cmpu.l  %s47, %s59, %s0
	brge.l.t 0, %s47, SYSCALL_ERROR_LABEL /* Branch to error handler if error. */

	lea    %sp, 8(,%sp)
	ld     %s0, -8(, %sp)

	/* Restore the floating-point context.  Not the registers, only the
	   rest.  */
	ld	%s1, oPSW(, %s0)
	and	%s2, %s1, (50)0
	and	%s2, %s2, (58)1
	lpm	%s2
	and	%s2, %s1, (58)0
	lfr	%s2

	/* Save the vector regs environment.  */
	or	%s2, 8, (0)1
	lea	%s1, oVRG0(, %s0)
	vld	%v0, %s2, %s1
	lea	%s1, oVRG1(, %s0)
	vld	%v1, %s2, %s1
	lea	%s1, oVRG2(, %s0)
	vld	%v2, %s2, %s1
	lea	%s1, oVRG3(, %s0)
	vld	%v3, %s2, %s1
	lea	%s1, oVRG4(, %s0)
	vld	%v4, %s2, %s1
	lea	%s1, oVRG5(, %s0)
	vld	%v5, %s2, %s1
	lea	%s1, oVRG6(, %s0)
	vld	%v6, %s2, %s1
	lea	%s1, oVRG7(, %s0)
	vld	%v7, %s2, %s1
	lea	%s1, oVRG8(, %s0)
	vld	%v8, %s2, %s1
	lea	%s1, oVRG9(, %s0)
	vld	%v9, %s2, %s1
	lea	%s1, oVRG10(, %s0)
	vld	%v10, %s2, %s1
	lea	%s1, oVRG11(, %s0)
	vld	%v11, %s2, %s1
	lea	%s1, oVRG12(, %s0)
	vld	%v12, %s2, %s1
	lea	%s1, oVRG13(, %s0)
	vld	%v13, %s2, %s1
	lea	%s1, oVRG14(, %s0)
	vld	%v14, %s2, %s1
	lea	%s1, oVRG15(, %s0)
	vld	%v15, %s2, %s1
	lea	%s1, oVRG16(, %s0)
	vld	%v16, %s2, %s1
	lea	%s1, oVRG17(, %s0)
	vld	%v17, %s2, %s1
	lea	%s1, oVRG18(, %s0)
	vld	%v18, %s2, %s1
	lea	%s1, oVRG19(, %s0)
	vld	%v19, %s2, %s1
	lea	%s1, oVRG20(, %s0)
	vld	%v20, %s2, %s1
	lea	%s1, oVRG21(, %s0)
	vld	%v21, %s2, %s1
	lea	%s1, oVRG22(, %s0)
	vld	%v22, %s2, %s1
	lea	%s1, oVRG23(, %s0)
	vld	%v23, %s2, %s1
	lea	%s1, oVRG24(, %s0)
	vld	%v24, %s2, %s1
	lea	%s1, oVRG25(, %s0)
	vld	%v25, %s2, %s1
	lea	%s1, oVRG26(, %s0)
	vld	%v26, %s2, %s1
	lea	%s1, oVRG27(, %s0)
	vld	%v27, %s2, %s1
	lea	%s1, oVRG28(, %s0)
	vld	%v28, %s2, %s1
	lea	%s1, oVRG29(, %s0)
	vld	%v29, %s2, %s1
	lea	%s1, oVRG30(, %s0)
	vld	%v30, %s2, %s1
	lea	%s1, oVRG31(, %s0)
	vld	%v31, %s2, %s1
	lea	%s1, oVRG32(, %s0)
	vld	%v32, %s2, %s1
	lea	%s1, oVRG33(, %s0)
	vld	%v33, %s2, %s1
	lea	%s1, oVRG34(, %s0)
	vld	%v34, %s2, %s1
	lea	%s1, oVRG35(, %s0)
	vld	%v35, %s2, %s1
	lea	%s1, oVRG36(, %s0)
	vld	%v36, %s2, %s1
	lea	%s1, oVRG37(, %s0)
	vld	%v37, %s2, %s1
	lea	%s1, oVRG38(, %s0)
	vld	%v38, %s2, %s1
	lea	%s1, oVRG39(, %s0)
	vld	%v39, %s2, %s1
	lea	%s1, oVRG40(, %s0)
	vld	%v40, %s2, %s1
	lea	%s1, oVRG41(, %s0)
	vld	%v41, %s2, %s1
	lea	%s1, oVRG42(, %s0)
	vld	%v42, %s2, %s1
	lea	%s1, oVRG43(, %s0)
	vld	%v43, %s2, %s1
	lea	%s1, oVRG44(, %s0)
	vld	%v44, %s2, %s1
	lea	%s1, oVRG45(, %s0)
	vld	%v45, %s2, %s1
	lea	%s1, oVRG46(, %s0)
	vld	%v46, %s2, %s1
	lea	%s1, oVRG47(, %s0)
	vld	%v47, %s2, %s1
	lea	%s1, oVRG48(, %s0)
	vld	%v48, %s2, %s1
	lea	%s1, oVRG49(, %s0)
	vld	%v49, %s2, %s1
	lea	%s1, oVRG50(, %s0)
	vld	%v50, %s2, %s1
	lea	%s1, oVRG51(, %s0)
	vld	%v51, %s2, %s1
	lea	%s1, oVRG52(, %s0)
	vld	%v52, %s2, %s1
	lea	%s1, oVRG53(, %s0)
	vld	%v53, %s2, %s1
	lea	%s1, oVRG54(, %s0)
	vld	%v54, %s2, %s1
	lea	%s1, oVRG55(, %s0)
	vld	%v55, %s2, %s1
	lea	%s1, oVRG56(, %s0)
	vld	%v56, %s2, %s1
	lea	%s1, oVRG57(, %s0)
	vld	%v57, %s2, %s1
	lea	%s1, oVRG58(, %s0)
	vld	%v58, %s2, %s1
	lea	%s1, oVRG59(, %s0)
	vld	%v59, %s2, %s1
	lea	%s1, oVRG60(, %s0)
	vld	%v60, %s2, %s1
	lea	%s1, oVRG61(, %s0)
	vld	%v61, %s2, %s1
	lea	%s1, oVRG62(, %s0)
	vld	%v62, %s2, %s1
	lea	%s1, oVRG63(, %s0)
	vld	%v63, %s2, %s1

	/* Save the current vector mask */
	xor	%s3, %s3, %s3
	lea	%s1, oVMRG0(, %s0)
	ld	%s2, 0x0(, %s1)
	lvm	%vm0, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x8(, %s1)
	lvm	%vm0, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x10(, %s1)
	lvm	%vm0, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x18(, %s1)
	lvm	%vm0, %s3, %s2

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG1(, %s0)
	ld	%s2, 0x0(, %s1)
	lvm	%vm1, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x8(, %s1)
	lvm	%vm1, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x10(, %s1)
	lvm	%vm1, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x18(, %s1)
	lvm	%vm1, %s3, %s2

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG2(, %s0)
	ld	%s2, 0x0(, %s1)
	lvm	%vm2, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x8(, %s1)
	lvm	%vm2, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x10(, %s1)
	lvm	%vm2, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x18(, %s1)
	lvm	%vm2, %s3, %s2

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG3(, %s0)
	ld	%s2, 0x0(, %s1)
	lvm	%vm3, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x8(, %s1)
	lvm	%vm3, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x10(, %s1)
	lvm	%vm3, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x18(, %s1)
	lvm	%vm3, %s3, %s2

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG4(, %s0)
	ld	%s2, 0x0(, %s1)
	lvm	%vm4, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x8(, %s1)
	lvm	%vm4, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x10(, %s1)
	lvm	%vm4, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x18(, %s1)
	lvm	%vm4, %s3, %s2

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG5(, %s0)
	ld	%s2, 0x0(, %s1)
	lvm	%vm5, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x8(, %s1)
	lvm	%vm5, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x10(, %s1)
	lvm	%vm5, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x18(, %s1)
	lvm	%vm5, %s3, %s2

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG6(, %s0)
	ld	%s2, 0x0(, %s1)
	lvm	%vm6, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x8(, %s1)
	lvm	%vm6, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x10(, %s1)
	lvm	%vm6, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x18(, %s1)
	lvm	%vm6, %s3, %s2

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG7(, %s0)
	ld	%s2, 0x0(, %s1)
	lvm	%vm7, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x8(, %s1)
	lvm	%vm7, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x10(, %s1)
	lvm	%vm7, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x18(, %s1)
	lvm	%vm7, %s3, %s2

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG8(, %s0)
	ld	%s2, 0x0(, %s1)
	lvm	%vm8, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x8(, %s1)
	lvm	%vm8, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x10(, %s1)
	lvm	%vm8, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x18(, %s1)
	lvm	%vm8, %s3, %s2

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG9(, %s0)
	ld	%s2, 0x0(, %s1)
	lvm	%vm9, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x8(, %s1)
	lvm	%vm9, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x10(, %s1)
	lvm	%vm9, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x18(, %s1)
	lvm	%vm9, %s3, %s2

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG10(, %s0)
	ld	%s2, 0x0(, %s1)
	lvm	%vm10, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x8(, %s1)
	lvm	%vm10, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x10(, %s1)
	lvm	%vm10, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x18(, %s1)
	lvm	%vm10, %s3, %s2

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG11(, %s0)
	ld	%s2, 0x0(, %s1)
	lvm	%vm11, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x8(, %s1)
	lvm	%vm11, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x10(, %s1)
	lvm	%vm11, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x18(, %s1)
	lvm	%vm11, %s3, %s2

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG12(, %s0)
	ld	%s2, 0x0(, %s1)
	lvm	%vm12, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x8(, %s1)
	lvm	%vm12, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x10(, %s1)
	lvm	%vm12, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x18(, %s1)
	lvm	%vm12, %s3, %s2

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG13(, %s0)
	ld	%s2, 0x0(, %s1)
	lvm	%vm13, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x8(, %s1)
	lvm	%vm13, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x10(, %s1)
	lvm	%vm13, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x18(, %s1)
	lvm	%vm13, %s3, %s2

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG14(, %s0)
	ld	%s2, 0x0(, %s1)
	lvm	%vm14, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x8(, %s1)
	lvm	%vm14, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x10(, %s1)
	lvm	%vm14, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x18(, %s1)
	lvm	%vm14, %s3, %s2

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG15(, %s0)
	ld	%s2, 0x0(, %s1)
	lvm	%vm15, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x8(, %s1)
	lvm	%vm15, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x10(, %s1)
	lvm	%vm15, %s3, %s2
	addu.l	%s3, 1, %s3
	ld	%s2, 0x18(, %s1)
	lvm	%vm15, %s3, %s2

	/* Load the new stack pointer, the preserved registers and
	   registers used for passing args.  */

	ld	%s1, oSRG1(, %s0)
	ld	%s2, oSRG2(, %s0)
	ld	%s3, oSRG3(, %s0)
	ld	%s4, oSRG4(, %s0)
	ld	%s5, oSRG5(, %s0)
	ld	%s6, oSRG6(, %s0)
	ld	%s7, oSRG7(, %s0)
	ld	%s8, oSRG8(, %s0)
	ld	%s9, oSRG9(, %s0)
	ld	%s10, oSRG10(, %s0)
	ld	%s11, oSRG11(, %s0)
	ld	%s12, oSRG12(, %s0)
	ld	%s13, oSRG13(, %s0)
	ld	%s14, oSRG14(, %s0)
	ld	%s15, oSRG15(, %s0)
	ld	%s16, oSRG16(, %s0)
	ld	%s17, oSRG17(, %s0)
	ld	%s18, oSRG18(, %s0)
	ld	%s19, oSRG19(, %s0)
	ld	%s20, oSRG20(, %s0)
	ld	%s21, oSRG21(, %s0)
	ld	%s22, oSRG22(, %s0)
	ld	%s23, oSRG23(, %s0)
	ld	%s24, oSRG24(, %s0)
	ld	%s25, oSRG25(, %s0)
	ld	%s26, oSRG26(, %s0)
	ld	%s27, oSRG27(, %s0)
	ld	%s28, oSRG28(, %s0)
	ld	%s29, oSRG29(, %s0)
	ld	%s30, oSRG30(, %s0)
	ld	%s31, oSRG31(, %s0)
	ld	%s32, oSRG32(, %s0)
	ld	%s33, oSRG33(, %s0)

	/* Clear rax to indicate success.
	xor	%s0, %s0, %s0  */

	/* Lastly load %s0.*/
	ld	%s62, oSRG0(, %s0)

	/* Load IC to start execution */
	ld	%s63, oIC(, %s0)
	or     %s0,0x0,%s62
	beq.l.t	0x0, (, %s63)

PSEUDO_END(__setcontext)

weak_alias (__setcontext, setcontext)
