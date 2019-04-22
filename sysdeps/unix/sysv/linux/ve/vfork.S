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

 /* __TODO_PORT_HCLT :
   1. SAVE program counter is required or not ?
   2. callee saved register for storing PID */

#include <sysdep.h>
#include <tcb-offsets.h>
#include <tls.h>

.section	.data
		.align 8
.set pid_clean,	 0x80000000
.set verr,	 -4095

ENTRY(__vfork)
	/* Save return PC value into any GP Register.  We need a register that
	   is preserved by the syscall and that we're allowed to destroy. */

	/* Save the TCB-cached PID away in GP Register, and then negate the TCB
           field.  But if it's zero, set it to 0x80000000 instead.  See
           raise.c for the logic that relies on this value.  */
	ldl.sx	%s63, PID_OFFSET(, %tp)
	subs.w	%s0, 0, %s63
	lea	%s1, pid_clean
	cmov.w.eq	%s0, %s1, %s0
	stl	%s0, PID_OFFSET(, %tp)

	xor	%s46, %s46, %s46
	ldl	%s46, SHM_OFFSET(, %tp) /*! fetch shared SHM offset */
	or      %s0, SYS_ify(vfork), (0)1 /*Calling  vfork  directly,instead of using clone with arguments to do vfork system call*/
	xor	%s2, %s2, %s2
	shm.l	%s0, 0x0(%s46)
	monc
	breq.l.t 0, %s0, 1f
	/* Restore PID value, if we're the parent */
	stl	%s63, PID_OFFSET(, %tp)
1:
	/* Restore PC */
	lea %s62, verr
	cmpu.l  %s47, %s62, %s0
	brge.l.t 0, %s47, SYSCALL_ERROR_LABEL /* If true, then set errno */
	beq.l.t 0,0x0(, %lr)

PSEUDO_END (__vfork)
libc_hidden_def (__vfork)

weak_alias (__vfork, vfork)
strong_alias (__vfork, __libc_vfork)
