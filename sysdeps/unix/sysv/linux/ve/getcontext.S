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

/* Save current context. */

#include <sysdep.h>

#include "ucontext_i.h"

/*  int __getcontext (ucontext_t *ucp)

  Saves the machine context in UCP such that when it is activated,
  it appears as if __getcontext() returned again.

  This implementation is intended to be used for *synchronous* context
  switches only.  Therefore, it does not have to save anything
  other than the PRESERVED state.  */


.section        .data
.set err,       -4095

ENTRY(__getcontext)
	/* Save the preserved registers, the registers used for passing
	   args, and the return address.  */
	/* Save All Scalar Registers */
	xor 	%s60, %s60, %s60
	st      %s60, oSRG0(, %s0)
	st	%s1, oSRG1(, %s0)
	st	%s2, oSRG2(, %s0)
	st	%s3, oSRG3(, %s0)
	st	%s4, oSRG4(, %s0)
	st	%s5, oSRG5(, %s0)
	st	%s6, oSRG6(, %s0)
	st	%s7, oSRG7(, %s0)
	st	%s8, oSRG8(, %s0)
	st	%s9, oSRG9(, %s0)
	st	%s10, oSRG10(, %s0)
	st	%s11, oSRG11(, %s0)
	st	%s12, oSRG12(, %s0)
	st	%s13, oSRG13(, %s0)
	st	%s14, oSRG14(, %s0)
	st	%s15, oSRG15(, %s0)
	st	%s16, oSRG16(, %s0)
	st	%s17, oSRG17(, %s0)
	st	%s18, oSRG18(, %s0)
	st	%s19, oSRG19(, %s0)
	st	%s20, oSRG20(, %s0)
	st	%s21, oSRG21(, %s0)
	st	%s22, oSRG22(, %s0)
	st	%s23, oSRG23(, %s0)
	st	%s24, oSRG24(, %s0)
	st	%s25, oSRG25(, %s0)
	st	%s26, oSRG26(, %s0)
	st	%s27, oSRG27(, %s0)
	st	%s28, oSRG28(, %s0)
	st	%s29, oSRG29(, %s0)
	st	%s30, oSRG30(, %s0)
	st	%s31, oSRG31(, %s0)
	st	%s32, oSRG32(, %s0)
	st	%s33, oSRG33(, %s0)

	or %s34,14,(0)1 ;          /* VLFA DELAY */
	brgt.w  0,%s34,32;
	fencei ;
	subs.w.sx  %s34,%s34,(63)0;
	br.l -24;
	/* Save the floating-point environment.  */
	smir	%s3, %psw
/*	and	%s3, %s3, (50)0	*/
	st	%s3, oPSW (, %s0)

	/* Save last execute function IC. */
	st	%lr, oIC(, %s0)

	/* Save the vector regs environment.  */
	or	%s2, 8, (0)1
	lea	%s1, oVRG0(, %s0)
	vst	%v0, %s2, %s1
	lea	%s1, oVRG1(, %s0)
	vst	%v1, %s2, %s1
	lea	%s1, oVRG2(, %s0)
	vst	%v2, %s2, %s1
	lea	%s1, oVRG3(, %s0)
	vst	%v3, %s2, %s1
	lea	%s1, oVRG4(, %s0)
	vst	%v4, %s2, %s1
	lea	%s1, oVRG5(, %s0)
	vst	%v5, %s2, %s1
	lea	%s1, oVRG6(, %s0)
	vst	%v6, %s2, %s1
	lea	%s1, oVRG7(, %s0)
	vst	%v7, %s2, %s1
	lea	%s1, oVRG8(, %s0)
	vst	%v8, %s2, %s1
	lea	%s1, oVRG9(, %s0)
	vst	%v9, %s2, %s1
	lea	%s1, oVRG10(, %s0)
	vst	%v10, %s2, %s1
	lea	%s1, oVRG11(, %s0)
	vst	%v11, %s2, %s1
	lea	%s1, oVRG12(, %s0)
	vst	%v12, %s2, %s1
	lea	%s1, oVRG13(, %s0)
	vst	%v13, %s2, %s1
	lea	%s1, oVRG14(, %s0)
	vst	%v14, %s2, %s1
	lea	%s1, oVRG15(, %s0)
	vst	%v15, %s2, %s1
	lea	%s1, oVRG16(, %s0)
	vst	%v16, %s2, %s1
	lea	%s1, oVRG17(, %s0)
	vst	%v17, %s2, %s1
	lea	%s1, oVRG18(, %s0)
	vst	%v18, %s2, %s1
	lea	%s1, oVRG19(, %s0)
	vst	%v19, %s2, %s1
	lea	%s1, oVRG20(, %s0)
	vst	%v20, %s2, %s1
	lea	%s1, oVRG21(, %s0)
	vst	%v21, %s2, %s1
	lea	%s1, oVRG22(, %s0)
	vst	%v22, %s2, %s1
	lea	%s1, oVRG23(, %s0)
	vst	%v23, %s2, %s1
	lea	%s1, oVRG24(, %s0)
	vst	%v24, %s2, %s1
	lea	%s1, oVRG25(, %s0)
	vst	%v25, %s2, %s1
	lea	%s1, oVRG26(, %s0)
	vst	%v26, %s2, %s1
	lea	%s1, oVRG27(, %s0)
	vst	%v27, %s2, %s1
	lea	%s1, oVRG28(, %s0)
	vst	%v28, %s2, %s1
	lea	%s1, oVRG29(, %s0)
	vst	%v29, %s2, %s1
	lea	%s1, oVRG30(, %s0)
	vst	%v30, %s2, %s1
	lea	%s1, oVRG31(, %s0)
	vst	%v31, %s2, %s1
	lea	%s1, oVRG32(, %s0)
	vst	%v32, %s2, %s1
	lea	%s1, oVRG33(, %s0)
	vst	%v33, %s2, %s1
	lea	%s1, oVRG34(, %s0)
	vst	%v34, %s2, %s1
	lea	%s1, oVRG35(, %s0)
	vst	%v35, %s2, %s1
	lea	%s1, oVRG36(, %s0)
	vst	%v36, %s2, %s1
	lea	%s1, oVRG37(, %s0)
	vst	%v37, %s2, %s1
	lea	%s1, oVRG38(, %s0)
	vst	%v38, %s2, %s1
	lea	%s1, oVRG39(, %s0)
	vst	%v39, %s2, %s1
	lea	%s1, oVRG40(, %s0)
	vst	%v40, %s2, %s1
	lea	%s1, oVRG41(, %s0)
	vst	%v41, %s2, %s1
	lea	%s1, oVRG42(, %s0)
	vst	%v42, %s2, %s1
	lea	%s1, oVRG43(, %s0)
	vst	%v43, %s2, %s1
	lea	%s1, oVRG44(, %s0)
	vst	%v44, %s2, %s1
	lea	%s1, oVRG45(, %s0)
	vst	%v45, %s2, %s1
	lea	%s1, oVRG46(, %s0)
	vst	%v46, %s2, %s1
	lea	%s1, oVRG47(, %s0)
	vst	%v47, %s2, %s1
	lea	%s1, oVRG48(, %s0)
	vst	%v48, %s2, %s1
	lea	%s1, oVRG49(, %s0)
	vst	%v49, %s2, %s1
	lea	%s1, oVRG50(, %s0)
	vst	%v50, %s2, %s1
	lea	%s1, oVRG51(, %s0)
	vst	%v51, %s2, %s1
	lea	%s1, oVRG52(, %s0)
	vst	%v52, %s2, %s1
	lea	%s1, oVRG53(, %s0)
	vst	%v53, %s2, %s1
	lea	%s1, oVRG54(, %s0)
	vst	%v54, %s2, %s1
	lea	%s1, oVRG55(, %s0)
	vst	%v55, %s2, %s1
	lea	%s1, oVRG56(, %s0)
	vst	%v56, %s2, %s1
	lea	%s1, oVRG57(, %s0)
	vst	%v57, %s2, %s1
	lea	%s1, oVRG58(, %s0)
	vst	%v58, %s2, %s1
	lea	%s1, oVRG59(, %s0)
	vst	%v59, %s2, %s1
	lea	%s1, oVRG60(, %s0)
	vst	%v60, %s2, %s1
	lea	%s1, oVRG61(, %s0)
	vst	%v61, %s2, %s1
	lea	%s1, oVRG62(, %s0)
	vst	%v62, %s2, %s1
	lea	%s1, oVRG63(, %s0)
	vst	%v63, %s2, %s1

	/* Save the current vector mask */
	xor	%s3, %s3, %s3
	lea	%s1, oVMRG0(, %s0)
	svm	%s2, %vm0, %s3
	st	%s2, 0x0(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm0, %s3
	st	%s2, 0x8(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm0, %s3
	st	%s2, 0x10(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm0, %s3
	st	%s2, 0x18(, %s1)

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG1(, %s0)
	svm	%s2, %vm1, %s3
	st	%s2, 0x0(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm1, %s3
	st	%s2, 0x8(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm1, %s3
	st	%s2, 0x10(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm1, %s3
	st	%s2, 0x18(, %s1)

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG2(, %s0)
	svm	%s2, %vm2, %s3
	st	%s2, 0x0(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm2, %s3
	st	%s2, 0x8(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm2, %s3
	st	%s2, 0x10(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm2, %s3
	st	%s2, 0x18(, %s1)


	xor	%s3, %s3, %s3
	lea	%s1, oVMRG3(, %s0)
	svm	%s2, %vm3, %s3
	st	%s2, 0x0(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm3, %s3
	st	%s2, 0x8(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm3, %s3
	st	%s2, 0x10(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm3, %s3
	st	%s2, 0x18(, %s1)

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG4(, %s0)
	svm	%s2, %vm4, %s3
	st	%s2, 0x0(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm4, %s3
	st	%s2, 0x8(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm4, %s3
	st	%s2, 0x10(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm4, %s3
	st	%s2, 0x18(, %s1)

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG5(, %s0)
	svm	%s2, %vm5, %s3
	st	%s2, 0x0(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm5, %s3
	st	%s2, 0x8(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm5, %s3
	st	%s2, 0x10(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm5, %s3
	st	%s2, 0x18(, %s1)

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG6(, %s0)
	svm	%s2, %vm6, %s3
	st	%s2, 0x0(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm6, %s3
	st	%s2, 0x8(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm6, %s3
	st	%s2, 0x10(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm6, %s3
	st	%s2, 0x18(, %s1)

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG7(, %s0)
	svm	%s2, %vm7, %s3
	st	%s2, 0x0(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm7, %s3
	st	%s2, 0x8(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm7, %s3
	st	%s2, 0x10(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm7, %s3
	st	%s2, 0x18(, %s1)

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG8(, %s0)
	svm	%s2, %vm8, %s3
	st	%s2, 0x0(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm8, %s3
	st	%s2, 0x8(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm8, %s3
	st	%s2, 0x10(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm8, %s3
	st	%s2, 0x18(, %s1)

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG9(, %s0)
	svm	%s2, %vm9, %s3
	st	%s2, 0x0(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm9, %s3
	st	%s2, 0x8(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm9, %s3
	st	%s2, 0x10(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm9, %s3
	st	%s2, 0x18(, %s1)

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG10(, %s0)
	svm	%s2, %vm10, %s3
	st	%s2, 0x0(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm10, %s3
	st	%s2, 0x8(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm10, %s3
	st	%s2, 0x10(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm10, %s3
	st	%s2, 0x18(, %s1)

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG11(, %s0)
	svm	%s2, %vm11, %s3
	st	%s2, 0x0(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm11, %s3
	st	%s2, 0x8(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm11, %s3
	st	%s2, 0x10(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm11, %s3
	st	%s2, 0x18(, %s1)

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG12(, %s0)
	svm	%s2, %vm12, %s3
	st	%s2, 0x0(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm12, %s3
	st	%s2, 0x8(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm12, %s3
	st	%s2, 0x10(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm12, %s3
	st	%s2, 0x18(, %s1)

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG13(, %s0)
	svm	%s2, %vm13, %s3
	st	%s2, 0x0(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm13, %s3
	st	%s2, 0x8(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm13, %s3
	st	%s2, 0x10(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm13, %s3
	st	%s2, 0x18(, %s1)

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG14(, %s0)
	svm	%s2, %vm14, %s3
	st	%s2, 0x0(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm14, %s3
	st	%s2, 0x8(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm14, %s3
	st	%s2, 0x10(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm14, %s3
	st	%s2, 0x18(, %s1)

	xor	%s3, %s3, %s3
	lea	%s1, oVMRG15(, %s0)
	svm	%s2, %vm15, %s3
	st	%s2, 0x0(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm15, %s3
	st	%s2, 0x8(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm15, %s3
	st	%s2, 0x10(, %s1)
	addu.l	%s3, 1, %s3
	svm	%s2, %vm15, %s3
	st	%s2, 0x18(, %s1)

	/* Save the current signal mask with
	   rt_sigprocmask (SIG_BLOCK, NULL, set,_NSIG/8).  */
	lea	%s4, oSIGMASK(,%s0)
	xor	%s3, %s3, %s3
	or	%s2, SIG_BLOCK, (0)1
	or	%s5, _NSIG8, (0)1
	or	%s1, __NR_rt_sigprocmask, (0)1
	ldl.sx	%s60, SHM_OFFSET(,%tp)
	shm.l	%s1, 0x0(%s60)
	shm.l	%s2, 0x8(%s60)
	shm.l	%s3, 0x10(%s60)
	shm.l	%s4, 0x18(%s60)
	shm.l	%s5, 0x20(%s60)
	or %s34,10,(0)1 ;          /* VLFA DELAY */
	brgt.w  0,%s34,32;
	fencei ;
	subs.w.sx  %s34,%s34,(63)0;
	br.l -24;
	monc

	/* check return val */
	lea %s62, err
	cmpu.l  %s47, %s62, %s0
	brge.l.t 0, %s47, SYSCALL_ERROR_LABEL /* Branch to error handler if error. */

	/* All done, return 0 for success.  */
	xor	%s0, %s0, %s0
	beq.l.t	0x0, (, %lr)

PSEUDO_END(__getcontext)

weak_alias (__getcontext, getcontext)
