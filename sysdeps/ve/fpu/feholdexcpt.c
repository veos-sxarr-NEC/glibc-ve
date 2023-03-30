/* Store current floating-point environment and clear exceptions.
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
   <http://www.gnu.org/licenses/>.  */
/* Changes by NEC Corporation for the VE port, 2020 */

#include <fenv.h>

int
__feholdexcept (fenv_t *envp)
{
  unsigned int new=0;

    __asm__ __volatile__ (
    DELAY_FOR_VLFA_EXCEPTION
    );
  /* Store the environment. And then clear all exceptions.  */
  __asm__ __volatile("smir %%s1, %%psw\n\t"
	   "st2b %%s1, %0 " :: "m" (*&new): "%s1");
  *envp = new & 0x3fff;
 new &= ~0x3fff;
  __asm__ __volatile("lpm %0\n\t"
		     "lfr %0 " :: "r" (new));

  /* Success.  */
  return 0;
}
libm_hidden_def (__feholdexcept)
weak_alias (__feholdexcept, feholdexcept)
libm_hidden_weak (feholdexcept)
