/* Set floating-point environment exception handling.
   Copyright (C) 2001-2015 Free Software Foundation, Inc.
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
/* Changes by NEC Corporation for the VE port, 2017-2019 */

#include <fenv.h>
#include <math.h>

int
fesetexceptflag (const fexcept_t *flagp, int excepts)
{
  fenv_t temp = *flagp & excepts & FE_ALL_EXCEPT;

  /* Get the current FPU environment.  We have to do this since we
     cannot separately set the status word. And store the new status
     word (along with the rest of the environment. Possibly new
     exceptions are set but they won't get executed unless the next
     floating-point instruction.*/

  __asm__ ("sfr %%s1\n\t"
	"or %%s1, %0, %%s1\n\t"
	"lfr %%s1":: "r" (temp): "%s1");

  /* Success.  */
  return 0;
}
