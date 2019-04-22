/* Raise given exceptions.
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
#include <float.h>

int
__feraiseexcept (int excepts)
{
  /* Raise exceptions represented by EXPECTS.  But we must raise only
     one signal at a time.  It is important that if the overflow/underflow
     exception and the inexact exception are given at the same time,
     the overflow/underflow exception follows the inexact exception.  */

  /* First: invalid exception.  */
  if ((FE_INVALID & excepts) != 0)
    {
      float f = 0.0;
      /* One example of an invalid operation is 0.0 / 0.0.  */
      __asm__ __volatile__ ("fdiv.s %0, %0, %0 ": : "r" (f));
    }

  /* Next: division by zero.  */
  if ((FE_DIVBYZERO & excepts) != 0)
    {
      float f = FLT_MAX;
      float g = 0.0;
      __asm__ __volatile__ ("fdiv.s %%s1, %0, %1" :: "r" (f), "r" (g): "%s1");
    }

  /* Next: overflow.  */
  if ((FE_OVERFLOW & excepts) != 0)
    {
      float f = FLT_MAX;
      float g = FLT_MAX;
      __asm__ __volatile__ ("fmul.s %%s1, %0, %1" :: "r" (f), "r" (g): "%s1");
    }

  /* Next: underflow.  */
  if ((FE_UNDERFLOW & excepts) != 0)
    {
      float f = FLT_MIN;
      float g = 3.0;
      __asm__ __volatile__ ("fdiv.s %%s1, %0, %1" :: "r" (f), "r" (g): "%s1");
    }

  /* Last: inexact.  */
  if ((FE_INEXACT & excepts) != 0)
    {
      float f = 2.0;
      float g = 3.0;
      __asm__ __volatile__ ("fdiv.s %%s1, %0, %1" :: "r" (f), "r" (g): "%s1");
    }

  /* Success.  */
  return 0;
}
libm_hidden_def (__feraiseexcept)
weak_alias (__feraiseexcept, feraiseexcept)
libm_hidden_weak (feraiseexcept)
