/* Copyright (C) 2011-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@gmail.com>, 2011.

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
/* Changes by NEC Corporation for the VE port, 2022 */

#include <math.h>
#include <math_private.h>
#include <math-svid-compat.h>
#include <libm-alias-finite.h>

static const _Float128 zero = 0;

static _Float128
__ieee754_remainderl_ve(_Float128 x, _Float128 p)
{
        int64_t hx,hp;
        uint64_t sx,lx,lp;
        _Float128 p_half;

        GET_LDOUBLE_WORDS64(hx,lx,x);
        GET_LDOUBLE_WORDS64(hp,lp,p);
        sx = hx&0x8000000000000000ULL;
        hp &= 0x7fffffffffffffffLL;
        hx &= 0x7fffffffffffffffLL;

    /* purge off exception values */
        if((hp|lp)==0) return (x*p)/(x*p);      /* p = 0 */
        if((hx>=0x7fff000000000000LL)||                 /* x not finite */
          ((hp>=0x7fff000000000000LL)&&                 /* p is NaN */
          (((hp-0x7fff000000000000LL)|lp)!=0)))
            return (x*p)/(x*p);


        if (hp<=0x7ffdffffffffffffLL) x = __ieee754_fmodl(x,p+p);       /* now x < 2p */
        if (((hx-hp)|(lx-lp))==0) return zero*x;
        x  = fabsl(x);
        p  = fabsl(p);
        if (hp<0x0002000000000000LL) {
            if(x+x>p) {
                x-=p;
                if(x+x>=p) x -= p;
            }
        } else {
            p_half = L(0.5)*p;
            if(x>p_half) {
                x-=p;
                if(x>=p_half) x -= p;
            }
        }
        GET_LDOUBLE_MSW64(hx,x);
        SET_LDOUBLE_MSW64(x,hx^sx);
        return x;
}

long double
__ieee754_remainderl (long double x, long double y)
{
  if (((__builtin_expect (y == 0.0L, 0) && ! isnan (x))
       || (__builtin_expect (isinf (x), 0) && ! isnan (y)))
      && _LIB_VERSION != _IEEE_)
    return __kernel_standard_l (x, y, 228); /* remainder domain */

  return __ieee754_remainderl_ve (x, y);
}
libm_alias_finite (__ieee754_remainderl, __remainderl)
