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

/* Thread-local storage handling in the ELF dynamic linker. VE version. */

#include <sysdep.h>
#include <tls.h>
#include "tlsdesc.h"

	.text

     /* This function is used to compute the TP offset for symbols in
	Static TLS, i.e., whose TP offset is the same for all
	threads.

	The incoming %s0 points to the TLS descriptor, such that
	0(%s0) points to _dl_tlsdesc_return itself, and 8(%s0) holds
	the TP offset of the symbol corresponding to the object
	denoted by the argument.  */

	.hidden _dl_tlsdesc_return
	.global	_dl_tlsdesc_return
	.type	_dl_tlsdesc_return,@function
#	cfi_startproc
	.balign 16
_dl_tlsdesc_return:
	ld	%s0, 8(,%s0)
	beq.l.t 0, 0x0(,%s10)
#	cfi_endproc
	.size	_dl_tlsdesc_return, .-_dl_tlsdesc_return

     /* This function is used for undefined weak TLS symbols, for
	which the base address (i.e., disregarding any addend) should
	resolve to NULL.

	%s0 points to the TLS descriptor, such that 0(%s0) points to
	_dl_tlsdesc_undefweak itself, and 8(%s0) holds the addend.
	We return the addend minus the TP, such that, when the caller
	adds TP, it gets the addend back.  If that's zero, as usual,
	that's most likely a NULL pointer.  */

	.hidden _dl_tlsdesc_undefweak
	.global	_dl_tlsdesc_undefweak
	.type	_dl_tlsdesc_undefweak,@function
#	cfi_startproc
	.balign 16
_dl_tlsdesc_undefweak:
	ld	%s0, 8(,%s0)
	ld	%s61, 0(,%tp) 
	subu.l  %s0, %s61, %s0
	beq.l.t 0, 0x0(,%s10)
#	cfi_endproc
	.size	_dl_tlsdesc_undefweak, .-_dl_tlsdesc_undefweak

#ifdef SHARED

     /* %s0 points to the TLS descriptor, such that 0(%s0) points to
	_dl_tlsdesc_dynamic itself, and 8(%s0) points to a struct
	tlsdesc_dynamic_arg object.  It must return in %s0 the offset
	between the thread pointer and the object denoted by the
	argument, without clobbering any registers.

	The assembly code that follows is a rendition of the following
	C code, hand-optimized a little bit.

ptrdiff_t
_dl_tlsdesc_dynamic (register struct tlsdesc *tdp asm ("%s0"))
{
  struct tlsdesc_dynamic_arg *td = tdp->arg;
  dtv_t *dtv = *(dtv_t **)((char *)__thread_pointer + DTV_OFFSET);
  if (__builtin_expect (td->gen_count <= dtv[0].counter
			&& (dtv[td->tlsinfo.ti_module].pointer.val
			    != TLS_DTV_UNALLOCATED),
			1))
    return dtv[td->tlsinfo.ti_module].pointer.val + td->tlsinfo.ti_offset
      - __thread_pointer;

  return __tls_get_addr_internal (&td->tlsinfo) - __thread_pointer;
}
*/

	.hidden _dl_tlsdesc_dynamic
	.global	_dl_tlsdesc_dynamic
	.type	_dl_tlsdesc_dynamic,@function
#	cfi_startproc
	.balign 16
_dl_tlsdesc_dynamic:
	/* Preserve call-clobbered registers that we modify.
	   We need two scratch regs anyway.  */
	st	%s2, -16(,%sp)	
	lea 	%s60, (,%tp)
	ld      %s2, DTV_OFFSET(0,%s60)
	st	%s1, -8(,%sp)
	ld	%s1, TLSDESC_ARG(,%s0)	
	ld	%s0, (,%s2)
	ld 	%s61, TLSDESC_GEN_COUNT(, %s1)
	cmpu.l  %s63, %s0, %s61
	brgt.w  0, %s63, .L1.1
	
	ld	%s0, TLSDESC_MODID(,%s1)
	sla.l	%s0, %s0, 4
	ld	%s61, (,%s0)
	ld	%s62, (,%s2)
	addu.l  %s0, %s61, %s62
	breq.w  %s0, -1, .L1.1
	ld	%s63, TLSDESC_MODOFF(%s1)
	addu.l	%s0, %s0, %s63
.L2.3:
	ld	%s2, -16(,%sp) 
	ld	%s63, 0(,%tp)
	subu.l	%s0, %s0, %s63
	ld 	%s1, -8(%sp)
	beq.l.t 0, 0x0(,%s10)
.L1.1:
	st	%s3, 176(,%sp)
	st      %s4, 184(,%sp)
	st      %s5, 192(,%sp)
	st      %s6, 200(,%sp)
	st      %s7, 208(,%sp)

	#call	HIDDEN_JUMPTARGET (__tls_get_addr)
	/* __tls_get_addr will be called at run time so expanding 
	as JUMPTARGET which will further expand into name##@PLT
	defined in sysdep.h */

	lea     %s35, __tls_get_addr@plt_lo(-24)
        and     %s35, %s35, (32)0
        sic     %s36
        lea.sl  %s35, __tls_get_addr@plt_hi(%s35,%s36)
        xor     %s36, %s36, %s36
        bsic    %s10, (,%s35)

        ld      %s3, 176(,%sp)
        ld      %s4, 184(,%sp)
        ld      %s5, 192(,%sp)
        ld      %s6, 200(,%sp)
        ld      %s7, 208(,%sp)
	br.l    .L2.3
#	cfi_endproc
	.size	_dl_tlsdesc_dynamic, .-_dl_tlsdesc_dynamic
#endif /* SHARED */

     /* This function is a wrapper for a lazy resolver for TLS_DESC
	RELA relocations.  The incoming 0(%sp) points to the caller's
	link map, pushed by the dynamic object's internal lazy TLS
	resolver front-end before tail-calling us.  We need to pop it
	ourselves.  %s0 points to a TLS descriptor, such that 0(%s0)
	holds the address of the internal resolver front-end (unless
	some other thread beat us to resolving it) and 8(%s0) holds a
	pointer to the relocation.

	When the actual resolver returns, it will have adjusted the
	TLS descriptor such that we can tail-call it for it to return
	the TP offset of the symbol.  */

	.hidden _dl_tlsdesc_resolve_rela
	.global	_dl_tlsdesc_resolve_rela
	.type	_dl_tlsdesc_resolve_rela,@function
	#cfi_startproc
	.balign 16
	/* The PLT entry will have pushed the link_map pointer.  */
_dl_tlsdesc_resolve_rela:
	/* Save all call-clobbered registers.  */
	st	%s0, 176(,%sp)
	st	%s1, 184(,%sp)
	or	%s1, 0, %s0
	st	%s2, 192(,%sp)
	lea	%s2, 176(,%sp)
	st	%s5, 200(,%sp)	
	st	%s6, 208(,%sp)
	st	%s7, 216(,%sp)
	st	%s3, 224(,%sp) 
	st	%s4, 232(,%sp) 

	lea     %sp, -176(0, %sp)
	lea     %s12, _dl_tlsdesc_resolve_rela_fixup@PC_LO(-24)
        and     %s12, %s12,(32)0
        sic     %s62
        lea.sl  %s12, _dl_tlsdesc_resolve_rela_fixup@PC_HI(%s12,%s62)
	bsic    %lr, (%s12)
	lea     %sp, 176(0, %sp)
	
	ld	%s0, 176(,%sp)
	ld	%s1, 184(,%sp)
	ld	%s2, 192(,%sp)
	ld	%s5, 200(,%sp)
	ld	%s6, 208(,%sp)
	ld	%s7, 216(,%sp)
	ld	%s3, 224(,%sp)
	ld	%s4, 232(,%sp)
#	cfi_endproc
	.size	_dl_tlsdesc_resolve_rela, .-_dl_tlsdesc_resolve_rela

     /* This function is a placeholder for lazy resolving of TLS
	relocations.  Once some thread starts resolving a TLS
	relocation, it sets up the TLS descriptor to use this
	resolver, such that other threads that would attempt to
	resolve it concurrently may skip the call to the original lazy
	resolver and go straight to a condition wait.

	When the actual resolver returns, it will have adjusted the
	TLS descriptor such that we can tail-call it for it to return
	the TP offset of the symbol.  */

	.hidden _dl_tlsdesc_resolve_hold
	.global	_dl_tlsdesc_resolve_hold
	.type	_dl_tlsdesc_resolve_hold,@function
#	cfi_startproc
	.balign 16
_dl_tlsdesc_resolve_hold:
0:
	/* Save all call-clobbered registers.  */
	st	%s0, 176(,%sp)
	st	%s1, 184(,%sp)
	
	or      %s1, 0, %s0 
	st	%s2, 192(%sp)

	/* Pass _dl_tlsdesc_resolve_hold's address in %s2.  */
	lea     %sp, -176(0, %sp)
	lea     %s2, _dl_tlsdesc_resolve_hold@PC_LO(-24)
        and     %s2, %s2, (32)0
        sic     %s35
        lea.sl  %s2, _dl_tlsdesc_resolve_hold@PC_LO(%s2, %s35)
        bsic    %lr, (%s12)
	lea     %sp, 176(0, %sp)
        xor     %s35, %s35, %s35

	st      %s5, 200(%sp)
	st      %s6, 208(%sp)
	st      %s7, 216(%sp)
	st      %s3, 224(%sp)
	st      %s4, 232(%sp)

	lea     %sp, -176(0, %sp)
	lea     %s12, _dl_tlsdesc_resolve_hold_fixup@PC_LO(-24)
        and     %s12, %s12,(32)0
        sic     %s62
        lea.sl  %s12, _dl_tlsdesc_resolve_hold_fixup@PC_HI(%s12,%s62)
        bsic    %lr, (%s12)
	lea     %sp, 176(0, %sp)
1:

	ld 	%s0, 176(,%sp)
	ld      %s1, 184(,%sp)
	ld      %s2, 192(,%sp)
	ld      %s5, 200(,%sp)
	ld      %s6, 208(,%sp)
	ld      %s7, 216(,%sp)
	ld      %s3, 224(,%sp)
	ld      %s4, 232(,%sp)
#	cfi_endproc
	.size	_dl_tlsdesc_resolve_hold, .-_dl_tlsdesc_resolve_hold
