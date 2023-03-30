/* Install given floating-point environment.
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
#include <assert.h>


int
__fesetenv (const fenv_t *envp)
{
  fenv_t temp;

  if (envp == FE_DFL_ENV)
	temp =0x3000;
  else if (envp == FE_NOMASK_ENV)
	temp =0x3fc0;
  else
	temp = *envp;

  temp = temp & 0x3fff;
  __asm__ __volatile("lpm %0\n\t"
		     "lfr %0 " :: "r" (temp));
  /* Success.  */
  return 0;
}
libm_hidden_def (__fesetenv)
weak_alias (__fesetenv, fesetenv)
libm_hidden_weak (fesetenv)
