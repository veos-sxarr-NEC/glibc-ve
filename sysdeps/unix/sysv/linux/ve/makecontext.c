/* Create new context.
   Copyright (C) 2002-2015 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Andreas Jaeger <aj@suse.de>, 2002.

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
   <http://www.gnu.org/licenses/>.  */
/* Changes by NEC Corporation for the VE port, 2017-2019 */

#include <sysdep.h>
#include <stdarg.h>
#include <stdint.h>
#include <ucontext.h>

#include "ucontext_i.h"

/* This implementation can handle any ARGC value but only
   normal integer parameters.
   makecontext sets up a stack and the registers for the
   user context. The stack looks like this:
	%fp ->	+-----------------------+
		| trampoline address    |
		+-----------------------+
		| parameter 7-n         |
		+-----------------------+
		| Register Save Address |
	%sp ->	+-----------------------+

The registers are set up like this:
	%s0,%s1,%s2,%s3,%s4,%s5,%s6,%s7 parameter 1 to 8
	%fp   : address of trampoline address
	%sp   : stack pointer.
*/

/* XXX: This implementation currently only handles integer arguments.
   To handle long int and pointer arguments the va_arg arguments needs
   to be changed to long and also the stdlib/tst-setcontext.c file needs
   to be changed to pass long arguments to makecontext.  */
#define REG_SAVE_AREA	(176)

void
__makecontext (ucontext_t *ucp, void (*func) (void), int argc, ...)
{
  extern void __start_context (void);
  uint64_t *sp = NULL;
  uint64_t *fp = NULL;
  va_list ap;
  int i = 0;

  /* Generate room on stack for parameter if needed and uc_link.  */
  sp = (uint64_t *) ((uintptr_t) ucp->uc_stack.ss_sp
		   + ucp->uc_stack.ss_size - REG_SAVE_AREA - (argc*8));
  /* sp -= (argc > 6 ? argc - 6 : 0) + 1;*/
  /* Align stack and make space for trampoline address.  */
  sp = (uint64_t *) ((((uintptr_t) sp) & -8L) - 16);

  fp = (uint64_t *) (((uintptr_t) sp + REG_SAVE_AREA + (argc*8)) & -8L);
 /* idx_uc_link = (argci > 6 ? argc - 6 : 0) + 1; */

  /* Setup context ucp.  */
  /* Address to jump to.  */
  ucp->uc_mcontext.SR[12] = (uintptr_t) func;

  /* Execution starts from here. */
  ucp->uc_mcontext.IC = (uintptr_t) func;
  /* Setup rbx.*/
  ucp->uc_mcontext.SR[9] = ((uintptr_t) &fp[2]);

  /* Setup stack limit.*/
  ucp->uc_mcontext.SR[8] =(uintptr_t) ucp->uc_stack.ss_sp;

  /* Setup stack.  */
  ucp->uc_mcontext.SR[11] = (uintptr_t) sp;
  /* Setup return address.  */
  ucp->uc_mcontext.SR[10] = (uintptr_t) &__start_context;

/* *((uint64_t *)ucp->uc_mcontext.SR[9] + 1) = (uintptr_t) ucp->uc_link; */

  sp = (uint64_t *)((uintptr_t) sp + REG_SAVE_AREA);
  fp[1] = (uintptr_t) ucp->uc_link;

  va_start (ap, argc);
  /* Handle arguments.

     The standard says the parameters must all be int values.  This is
     an historic accident and would be done differently today.  For
     VE all integer values are passed as 64-bit values and
     therefore extending the API to copy 64-bit values instead of
     32-bit ints makes sense.  It does not break existing
     functionality and it does not violate the standard which says
     that passing non-int values means undefined behavior.  */
  for (i = 0; i < argc; ++i){
    switch (i)
      {
      case 0:
	ucp->uc_mcontext.SR[0] = va_arg (ap, uint64_t);
	sp[0] = ucp->uc_mcontext.SR[0];
	break;
      case 1:
	ucp->uc_mcontext.SR[1] = va_arg (ap, uint64_t);
	sp[1] = ucp->uc_mcontext.SR[1];
	break;
      case 2:
	ucp->uc_mcontext.SR[2] = va_arg (ap, uint64_t);
	sp[2] = ucp->uc_mcontext.SR[2];
	break;
      case 3:
	ucp->uc_mcontext.SR[3] = va_arg (ap, uint64_t);
	sp[3] = ucp->uc_mcontext.SR[3];
	break;
      case 4:
	ucp->uc_mcontext.SR[4] = va_arg (ap, uint64_t);
	sp[4] = ucp->uc_mcontext.SR[4];
	break;
      case 5:
	ucp->uc_mcontext.SR[5] = va_arg (ap, uint64_t);
	sp[5] = ucp->uc_mcontext.SR[5];
	break;
      case 6:
	ucp->uc_mcontext.SR[6] = va_arg (ap, uint64_t);
	sp[6] = ucp->uc_mcontext.SR[6];
	break;
      case 7:
	ucp->uc_mcontext.SR[7] = va_arg (ap, uint64_t);
	sp[7] = ucp->uc_mcontext.SR[7];
	break;
      default:
	/* Put value on stack.  */
	sp[i] = va_arg (ap, uint64_t);
	break;
      }
   }
  va_end (ap);
}
weak_alias (__makecontext, makecontext)
