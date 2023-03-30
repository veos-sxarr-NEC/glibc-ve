/* Set current rounding direction.
   Copyright (C) 2001-2020 Free Software Foundation, Inc.
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
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */
/* Changes by NEC Corporation for the VE port, 2020 */

#include <fenv.h>

int
__fesetround (int round)
{
  unsigned short int cw = 0;

  if ((round & ~0x3000) != 0)
    /* ROUND is no valid rounding mode.  */
    return 1;

  /* First get the Control Register Status.  */
  __asm__ __volatile("spm %%s1\n\t"
	     "st2b %%s1, %0" :: "m" (*&cw): "%s1");

  cw = (cw & ~0x3000) | round;

 __asm__ __volatile("lpm %0" :: "r" (cw));

  /* Success.  */
  return 0;
}
libm_hidden_def (__fesetround)
weak_alias (__fesetround, fesetround)
libm_hidden_weak (fesetround)
