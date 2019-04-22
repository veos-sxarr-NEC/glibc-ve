/* gettimeofday - get the time.  Linux/x86 version.
   Copyright (C) 2015 Free Software Foundation, Inc.
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

#include <sys/time.h>
# include <sysdep.h>
# include <errno.h>

#define SYS_sysve 316
#define VE_SYSVE_GET_VE_INFO 51

static uint64_t base_clock; /* MHz */
static struct timeval base_tv = {0};
static struct timeval prev_tv = {0};
static uint64_t base_stm = 0ULL;
static volatile int lock = LLL_LOCK_INITIALIZER;

ssize_t _ve_get_ve_info(const char *name, char *buffer, size_t size)
{
  int res = -1;
  INTERNAL_SYSCALL_DECL(err);
  res = INTERNAL_SYSCALL_NCS(SYS_sysve, err, 4, VE_SYSVE_GET_VE_INFO, (uint64_t)name,
			(uint64_t)buffer, size);
  if (__glibc_unlikely (INTERNAL_SYSCALL_ERROR_P (res, err)))
    {
      __set_errno (INTERNAL_SYSCALL_ERRNO (res, err));
      res = -1;
    }
  return res;
}


int
__gettimeofday (struct timeval *tv, struct timezone *tz)
{
  int ret = 0;
  struct timespec ts;
  uint64_t e_time = 0, e_time_tmp = 0, cur_stm = 0;
  void *vehva = (void *)0x000000001000;
  char base_clk[10] = {0};

  /*
   * If tz is not NULL, Ve-lib handles to give users the precise time.
  */

  if (tz != NULL)
    {
      ret = INLINE_SYSCALL (gettimeofday, 2, tv, tz);
      return ret;
    }

  if (!base_tv.tv_sec)
    {
      lll_lock (lock, LLL_PRIVATE);

      if (!base_tv.tv_sec)
        {
          errno = 0;

          ret = _ve_get_ve_info("clock_base", base_clk, 10);
          if(ret < 0) goto set_return_status;
          base_clock = strtol(base_clk, NULL, 10);
          if (errno)
            {
              ret = -1;
              goto set_return_status;
            }

          ret = INLINE_SYSCALL (gettimeofday, 2, tv, tz);
          if(ret < 0)
            {
              goto set_return_status;
            }
          base_tv.tv_sec = tv->tv_sec;
          base_tv.tv_usec = tv->tv_usec;
          GET_STM(base_stm, vehva);
          prev_tv.tv_sec = tv->tv_sec;
          prev_tv.tv_usec = tv->tv_usec;
          goto set_return_status;
        }
      lll_unlock (lock, LLL_PRIVATE);
    }
  GET_STM(cur_stm, vehva);
  e_time = (cur_stm - base_stm) & ((1ULL << 56) - 1 );
  e_time_tmp = e_time / base_clock;
  tv->tv_sec = e_time_tmp / 1000000;
  tv->tv_usec = e_time_tmp % 1000000;

  timeradd(&base_tv, tv, tv);

  lll_lock (lock, LLL_PRIVATE);

  if (tv->tv_sec - prev_tv.tv_sec > 3600)
    {
      ret = INLINE_SYSCALL (gettimeofday, 2, tv, tz);
      if(ret < 0)
        {
          goto set_return_status;
        }
      base_tv.tv_sec = tv->tv_sec = ts.tv_sec;
      base_tv.tv_usec = tv->tv_usec = (int)ts.tv_nsec / 1000;
      base_stm = cur_stm;
    }

  if (timercmp(tv, &prev_tv, <))
    {
      struct timeval diff;
      timersub(&prev_tv, tv, &diff);
      if (diff.tv_sec < 60)
        {
          /* If the difference is less than 60 seconds,
             return the previous value, in order to
             avoid rewinding the clock at user's view point. */
          tv->tv_sec = prev_tv.tv_sec;
          tv->tv_usec = prev_tv.tv_usec;
          goto set_return_status;
        }
    }
  prev_tv.tv_sec = tv->tv_sec;
  prev_tv.tv_usec = tv->tv_usec;

  set_return_status:
    lll_unlock (lock, LLL_PRIVATE);
    return ret;
}


libc_hidden_def (__gettimeofday);

weak_alias (__gettimeofday, gettimeofday);
libc_hidden_weak (gettimeofday);