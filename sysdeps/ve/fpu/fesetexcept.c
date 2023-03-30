/* Set given exception flags.  VE version.
   Copyright (C) 2016-2020 Free Software Foundation, Inc.
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
#include <fpu_control.h>

int
fesetexcept (int excepts)
{

/* Mask out unsupported bits/exceptions.  */
  excepts &= FE_ALL_EXCEPT;

  __asm__ ("sfr %%s1\n\t"
           "or %%s1, %0, %%s1\n\t"
           "lfr %%s1" :: "r" (excepts): "%s1");

  return 0;
}
