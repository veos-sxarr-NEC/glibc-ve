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

/* This is the helper code which gets called if a function which is
   registered with 'makecontext' returns.  In this case we have to
   install the context listed in the uc_link element of the context
   'makecontext' manipulated at the time of the 'makecontext' call.
   If the pointer is NULL the process must terminate.  */


ENTRY(__start_context)
	/* This removes the parameters passed to the function given to
	   'makecontext' from the stack.  RBX contains the address
	   on the stack pointer for the next context.  */
	ld	%s0, -0x8(, %fp)		/* This is the next context.  */
	breq.l.nt 0, %s0, 1f			/* If it is zero exit.  */

	JUMPTARGET(__setcontext)
	/* If this returns (which can happen if the syscall fails) we'll
	   exit the program with the return error value (-1).  */
	or	%s0, %s0, (0)0
1:
	addi %got,-16,_GLOBAL_OFFSET_TABLE_@PC_LO
	sic %plt
	lea.sl %got,_GLOBAL_OFFSET_TABLE_@PC_HI(%plt,%got)
	JUMPTARGET(exit)
	/* The 'exit' call should never return.  In case it does cause
	   the process to terminate.  */
2:	nop
	/* Here we need to stop the core immediately. */
	or %s34,10,(0)1 ;          /* VLFA DELAY */
	brgt.w  0,%s34,32;
	fencei ;
	subs.w.sx  %s34,%s34,(63)0;
	br.l -24;
	monc
	breq.l.t 0, 0x0, 2b

END(__start_context)
