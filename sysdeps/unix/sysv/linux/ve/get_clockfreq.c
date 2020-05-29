/* Get frequency of the system processor.
   Copyright (C) 2000-2015 Free Software Foundation, Inc.
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

#include <libc-internal.h>
#include <errno.h>

hp_timing_t
__get_clockfreq (void)
{
  char base_clk[10] = {0};
  static hp_timing_t retval;
  if (retval != 0)
  return retval;
  if( _ve_get_ve_info("clock_base", base_clk, 10) >= 0 )
  {
	errno = 0;
	retval = strtol(base_clk, NULL, 10);
	if(errno)
	{
		retval = 0;
	}
  }

  return retval;
}
