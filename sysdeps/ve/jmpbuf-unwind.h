/* Copyright (C) 2005-2015 Free Software Foundation, Inc.
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
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */
/* Changes by NEC Corporation for the VE port, 2017-2019 */

#include <setjmp.h>
#include <stdint.h>
#include <sysdep.h>
#include <unwind.h>
#include <jmpbuf-offsets.h>

/* Test if longjmp to JMPBUF would unwind the frame
   containing a local variable at ADDRESS.  */
#define _JMPBUF_UNWINDS(jmpbuf, address, demangle) \
  ((void *) (address) < (void *) demangle (jmpbuf[JB_SRSP]))

#define _JMPBUF_CFA_UNWINDS_ADJ(_jmpbuf, _context, _adj) \
  _JMPBUF_UNWINDS_ADJ (_jmpbuf, (void *) _Unwind_GetCFA (_context), _adj)

static inline uintptr_t __attribute__ ((unused))
_jmpbuf_sp (__jmp_buf regs)
{
  uintptr_t sp = regs[JB_SRSP];
/* TODO_HCLT: setjmp/longjmp never trust on demangle the SP.
 * use directly instead. */
/*
#ifdef PTR_DEMANGLE
  PTR_DEMANGLE (sp);
#endif
*/
  return sp;
}

#define _JMPBUF_UNWINDS_ADJ(_jmpbuf, _address, _adj) \
  ((uintptr_t) (_address) - (_adj) < _jmpbuf_sp (_jmpbuf) - (_adj))

/* We use the normal longjmp for unwinding.  */
#define __libc_unwind_longjmp(buf, val) __libc_longjmp (buf, val)
