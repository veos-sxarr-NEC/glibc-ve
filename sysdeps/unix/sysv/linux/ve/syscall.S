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


#include <sysdep.h>
/* Usage: long syscall (syscall_number, arg1, arg2, arg3, arg4, arg5, arg6) */

.section        .data
.set err,       -4095
.text
 ENTRY (syscall)
	xor     %s46, %s46, %s46
	ld	%s46, SHM_OFFSET(, %tp)
        shm.l	%s0, 0x0(%s46)
        shm.l	%s1, 0x8(%s46)
        shm.l	%s2, 0x10(%s46)
        shm.l	%s3, 0x18(%s46)
        shm.l	%s4, 0x20(%s46)
	shm.l	%s5, 0x28(%s46)
	shm.l	%s6, 0x30(%s46)
	monc
	lea %s62, err
	cmpu.l  %s47, %s62, %s0
	brge.l.t 0, %s47, SYSCALL_ERROR_LABEL
	beq.l.t 0,0x0(, %lr)				 /* Return to caller.  */
PSEUDO_END (syscall)
