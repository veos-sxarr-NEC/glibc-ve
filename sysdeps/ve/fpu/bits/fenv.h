/* Copyright (C) 2004-2020 Free Software Foundation, Inc.
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
   <https://www.gnu.org/licenses/>.  */
/* Changes by NEC Corporation for the VE port, 2020 */

#ifndef _FENV_H
# error "Never use <bits/fenv.h> directly; include <fenv.h> instead."
#endif

/* Define bits representing exceptions in the FPU status word.  */
enum
  {
    FE_INEXACT =
#define FE_INEXACT	0x1
      FE_INEXACT,
    FE_INVALID =
#define FE_INVALID	0x2
      FE_INVALID,
    FE_UNDERFLOW =
#define FE_UNDERFLOW	0x8
      FE_UNDERFLOW,
    FE_OVERFLOW =
#define FE_OVERFLOW	0x10
      FE_OVERFLOW,
    FE_DIVBYZERO =
#define FE_DIVBYZERO	0x20
      FE_DIVBYZERO
  };

/* All supported exceptions.  */
#define FE_ALL_EXCEPT	\
	(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW | FE_INEXACT)

/* Amount to shift by to convert an exception to a mask bit.  */
#define FE_EXCEPT_SHIFT	6

/* VFP supports all of the four defined rounding modes.  */
enum
  {
    FE_TOWARDZERO =
#define FE_TOWARDZERO	0x0
      FE_TOWARDZERO,
    FE_UPWARD =
#define FE_UPWARD	0x1000
      FE_UPWARD,
   FE_DOWNWARD =
#define FE_DOWNWARD	0x2000
      FE_DOWNWARD,
    FE_TONEAREST =
#define FE_TONEAREST	0x3000
      FE_TONEAREST
  };

typedef unsigned short int fexcept_t;

typedef unsigned short int fenv_t;

/* If the default argument is used we use this value.  TODO 3*/
#define FE_DFL_ENV	((const fenv_t *) -1l)

#undef DELAY_FOR_VLFA_EXCEPTION
#define DELAY_FOR_VLFA_EXCEPTION \
"or %s34,14,(0)1\n\t" \
"brgt.w  0,%s34,32\n\t" \
"fencei\n\t" \
"subs.w.sx  %s34,%s34,(63)0\n\t" \
"br.l -24\n\t" \
"nop\n\t"

#ifdef __USE_GNU
# define FE_NOMASK_ENV  ((const fenv_t *) -2)
#endif

#if __GLIBC_USE (IEC_60559_BFP_EXT_C2X)
/* Type representing floating-point control modes.  */
typedef unsigned short int femode_t;

/* Default floating-point control modes.  */
# define FE_DFL_MODE    ((const femode_t *) -1L)
#endif
