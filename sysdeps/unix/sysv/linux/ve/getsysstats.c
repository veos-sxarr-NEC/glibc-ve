/* Determine various system internal values, Linux version.
   Copyright (C) 1996-2015 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1996.

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

#include <alloca.h>
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <mntent.h>
#include <paths.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ldsodefs.h>
#include <sys/sysinfo.h>

#include <atomic.h>
#include <not-cancel.h>

#define SYS_sysve 316
#define VE_SYSVE_GET_VE_INFO 51

static ssize_t __ve_get_ve_info(const char *name, char *buffer, size_t size)
{
	int res = -1;

	INTERNAL_SYSCALL_DECL(err);
	res = INTERNAL_SYSCALL_NCS(SYS_sysve, err, 4, VE_SYSVE_GET_VE_INFO, (uint64_t)name,
			(uint64_t)buffer, size);
	if (__glibc_unlikely (INTERNAL_SYSCALL_ERROR_P (res, err))) {
		__set_errno (INTERNAL_SYSCALL_ERRNO (res, err));
		res = -1;
	}

	return res;
}

int
__get_nprocs (void)
{
  const size_t buffer_size = __libc_use_alloca (8192) ? 8192 : 512;
  char *core = alloca (buffer_size);
  int res = -1;

  res = __ve_get_ve_info("num_of_core", core, buffer_size);
  if (res == -1)
	  goto err_out;

  errno = 0;
  res = strtol(core, NULL, 10);
  if (errno) {
	  res = -1;
	  goto err_out;
  }

err_out:
  return res;
}
weak_alias (__get_nprocs, get_nprocs)


/* On some architectures it is possible to distinguish between configured
   and active cpus.  */
int
__get_nprocs_conf (void)
{
  int result = -1;

  result = __get_nprocs ();

  return result;
}
weak_alias (__get_nprocs_conf, get_nprocs_conf)

int __lsysinfo(struct sysinfo *info)
{
	INTERNAL_SYSCALL_DECL(err);
	int res = INTERNAL_SYSCALL(sysinfo, err, 1, info);
	if (__glibc_unlikely (INTERNAL_SYSCALL_ERROR_P (res, err))) {
		__set_errno (INTERNAL_SYSCALL_ERRNO (res, err));
		res = -1;
	}
	return res;
}

/* General function to get information about memory status from proc
   filesystem.  */
static long int
internal_function
phys_pages_info (const char *func)
{
	unsigned long long mem = -1;
	int __lsysinfo(struct sysinfo *);
	const uintptr_t pagesz = GLRO(dl_pagesize);
	struct sysinfo si = {0};

	__lsysinfo(&si);
	if (!si.mem_unit) si.mem_unit = 1;

	/* condition, to provide Total pages in RAM or available pages in RAM */
	if (!strcmp(func, "__get_phys_pages")) mem = si.totalram;
	else mem = si.freeram + si.bufferram;
	mem *= si.mem_unit;
	mem /= pagesz;
	return (mem > LONG_MAX) ? LONG_MAX : mem;
}


/* Return the number of pages of physical memory in the system.  There
   is a system call which uses a structure sysinfo to populate the number
   of pages in RAM. */
long int
__get_phys_pages (void)
{
  /* XXX Here will come a test for the new system call.  */

  return phys_pages_info (__func__);
}
weak_alias (__get_phys_pages, get_phys_pages)


/* Return the number of pages of physical memory in the system.  There
   is a system call which uses a structure sysinfo to populate the number
   of available pages in RAM. */
long int
__get_avphys_pages (void)
{
  /* XXX Here will come a test for the new system call.  */

  return phys_pages_info (__func__);
}
weak_alias (__get_avphys_pages, get_avphys_pages)
