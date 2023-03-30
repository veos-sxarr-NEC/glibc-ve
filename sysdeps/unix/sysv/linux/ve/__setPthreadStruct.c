/*
   Copyright (C) 2020 NEC Corporation
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

#define MAX_SIZE_TP 40

void __setPthreadStruct(void)
{
#ifndef SHARED
	static char buf[MAX_SIZE_TP] = {0};

	asm("or %%tp, %1, %0" : : "r" (buf), "i"(0));
	/* #1153, set "rtld_fini"(%s5) to zero when statically linked */
	asm("st  %0, 0xF0(, %%fp)": :"r"(0));
#endif
                return;
}
