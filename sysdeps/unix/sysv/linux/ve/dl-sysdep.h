/* System-specific settings for dynamic linker code.  VE version.
   Copyright (C) 2005-2020 Free Software Foundation, Inc.
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

#include_next <dl-sysdep.h>

/* On many architectures the kernel provides a virtual DSO and gives
   AT_SYSINFO_EHDR to point us to it.  As this is introduced for new
   machines, we should look at it for unwind information even if
   we aren't making direct use of it.  So enable this across the board.  */

#undef NEED_DL_SYSINFO_DSO


#ifndef __ASSEMBLER__
/* Get version of the OS.  */
extern int _dl_discover_osversion (void) attribute_hidden;
# define HAVE_DL_DISCOVER_OSVERSION	1
#endif
