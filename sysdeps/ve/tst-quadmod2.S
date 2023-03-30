/* Copyright (C) 2020 NEC Corporation
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it
   and/or modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either version
   2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.
*/

.text
.balign 16

.global foo
.type   foo,@function

foo:
        st      %fp,0x0(,%sp)
        st      %lr,0x8(,%sp)
        or      %fp,0,%sp

        or      %sp,0,%fp
        ld      %lr,0x8(,%sp)
        ld      %fp,0x0(,%sp)
        b.l.t   (,%lr)
.global func
.type   func,@function

func:
        st      %fp,0x0(,%sp)
        st      %lr,0x8(,%sp)
        or      %fp,0,%sp

        or      %sp,0,%fp
        ld      %lr,0x8(,%sp)
        ld      %fp,0x0(,%sp)
        b.l.t   (,%lr)
