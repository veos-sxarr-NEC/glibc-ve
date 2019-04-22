/* Test exception in current environment.
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

int
fetestexcept (int excepts)
{
  unsigned short int temp;

  /* Get current exceptions.  */
  __asm__ ("smir %%s1, %%psw \n\t"
	   "st2b %%s1, %0 " :: "m" (*&temp) : "%s1");

  return excepts & temp & FE_ALL_EXCEPT;
}
libm_hidden_def (fetestexcept)
