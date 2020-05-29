/* clock_gettime -- Get the current time from a POSIX clockid_t.  Unix version.
   Copyright (C) 1999-2015 Free Software Foundation, Inc.
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

#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <libc-internal.h>
#include <ldsodefs.h>

/* For all clock ids */
#define  MAX_CLOCKS  16

/* Declare locks for all clockids */
static volatile int lock[MAX_CLOCKS] = {LLL_LOCK_INITIALIZER};
static volatile int lock_freq = LLL_LOCK_INITIALIZER;

static struct timespec base_tspec[MAX_CLOCKS] = {0};
static struct timespec prev_tspec[MAX_CLOCKS] = {0};
static uint64_t base_stm[MAX_CLOCKS] = {0ULL};

#if HP_TIMING_AVAIL
/* Clock frequency of the processor.  We make it a 64-bit variable
   because some jokers are already playing with processors with more
   than 4GHz.  */
static hp_timing_t freq;


/* This function is defined in the thread library.  */
extern int __pthread_clock_gettime (clockid_t clock_id, hp_timing_t freq,
                                   struct timespec *tp)
     __attribute__ ((__weak__));

static int
hp_timing_gettime (clockid_t clock_id, struct timespec *tp)
{
  hp_timing_t tsc;
  hp_timing_t base_clock;

  lll_lock (lock_freq, LLL_PRIVATE);
  if (__glibc_unlikely (freq == 0))
    {
      /* This can only happen if we haven't initialized the `freq'
        variable yet.  Do this now. We don't have to protect this
        code against multiple execution since all of them should
        lead to the same result.  */
      freq = __get_clockfreq ();
      if (__glibc_unlikely (freq == 0))
       /* Something went wrong.  */
       return -1;
    }
    base_clock = freq* UINT64_C(1000000); /* Convert frequency from MHzto Hz */

  lll_unlock (lock_freq, LLL_PRIVATE);

  if (clock_id != CLOCK_PROCESS_CPUTIME_ID
      && __pthread_clock_gettime != NULL)
    return __pthread_clock_gettime (clock_id, base_clock, tp);

  /* Get the current counter.  */
  HP_TIMING_NOW (tsc);
  /* Compute the offset since the start time of the process.  */
  tsc -= GL(dl_cpuclock_offset);

  /* Compute the seconds.  */
  tp->tv_sec = tsc / base_clock;

  /* And the nanoseconds.  This computation should be stable until
     we get machines with about 16GHz frequency.  */
  tp->tv_nsec = ((tsc % base_clock) * UINT64_C (1000000000)) / base_clock;

  return 0;
}
#endif


static inline int
realtime_gettime (struct timespec *tp)
{
  struct timeval tv;
  int retval = gettimeofday (&tv, NULL);
  if (retval == 0)
    /* Convert into `timespec'.  */
    TIMEVAL_TO_TIMESPEC (&tv, tp);
  return retval;
}

/* Get current value of CLOCK and store it in TP.  */
int
__clock_gettime (clockid_t clock_id, struct timespec *tp)
{
  int retval = -1;
  /* Quick call CLOCK MONOTONIC variables */
  struct timeval tvm = {0};
  struct timeval tmp_base_tspec = {0};
  struct timeval prev_tspec_tmp = {0};
  struct timeval diff;
  uint64_t e_time = 0, e_time_tmp = 0, cur_stm = 0;
  void *vehva = (void *)0x000000001000;
  uint64_t base_clock = 0ULL; /* MHz */
  pid_t pid = -1;

  pid = CPUCLOCK_PID(clock_id);

  if(pid < -1 || pid == 1)
  {
      retval = -1;
	__set_errno (EINVAL);
  }
  else if( pid > 1)
  {
    if(CPUCLOCK_WHICH(clock_id) >= CPUCLOCK_MAX)
    {
      retval = -1;
	__set_errno (EINVAL);
    }
    else
    {
    retval = SYSCALL_GETTIME (clock_id, tp);
    }
  }
  else
  {
    switch (clock_id)
    {
      /* process here for the following clockids */
      case CLOCK_MONOTONIC:
      case CLOCK_MONOTONIC_COARSE:
      case CLOCK_MONOTONIC_RAW:
      case CLOCK_REALTIME:
      case CLOCK_REALTIME_COARSE:
      case CLOCK_BOOTTIME:
      {
	retval = 0;
	lll_lock (lock_freq, LLL_PRIVATE);
	/* check if frequency is already calculated */
	if (__glibc_unlikely (freq == 0))
	{
	  /* find frequency which will be common for all clockids */
	  freq = __get_clockfreq ();
	  if (__glibc_unlikely (freq == 0))
	  /* something went wrong.  */
	  return -1;
	}
	base_clock = freq;
	lll_unlock (lock_freq, LLL_PRIVATE);
	if (!base_tspec[clock_id].tv_sec)
	{
	  lll_lock (lock[clock_id], LLL_PRIVATE);
	  if (!base_tspec[clock_id].tv_sec)
	  {
	    /* Invoke a fresh system call */
	    errno = 0;
	    retval = SYSCALL_GETTIME (clock_id, tp);
	    if(retval < 0)
	    {
	      goto set_return_status;
	    }
	    base_tspec[clock_id].tv_sec = tp->tv_sec;
	    base_tspec[clock_id].tv_nsec = tp->tv_nsec;
	    GET_STM(base_stm[clock_id], vehva);
	    prev_tspec[clock_id].tv_sec = tp->tv_sec;
	    prev_tspec[clock_id].tv_nsec = tp->tv_nsec;
	    goto set_return_status;
	  }

	  lll_unlock (lock[clock_id], LLL_PRIVATE);
	}

	/* Calculate for quick call */
	GET_STM(cur_stm, vehva);
	lll_lock (lock[clock_id], LLL_PRIVATE);
	e_time = ((cur_stm - base_stm[clock_id]) & ((1ULL << 56) - 1 ));
	e_time_tmp = e_time / base_clock;
	tvm.tv_sec = e_time_tmp / 1000000;
	tvm.tv_usec = e_time_tmp % 1000000;

	/*Storing base data from nano to micro sec structure*/
	tmp_base_tspec.tv_sec = base_tspec[clock_id].tv_sec;
	tmp_base_tspec.tv_usec = (int)base_tspec[clock_id].tv_nsec/1000;
	lll_unlock (lock[clock_id], LLL_PRIVATE);

	timeradd(&tmp_base_tspec, &tvm, &tvm);
	/*Storing current time data from micro to nano*/
	tp->tv_sec = tvm.tv_sec;
	tp->tv_nsec = (int)tvm.tv_usec*1000;

	lll_lock (lock[clock_id], LLL_PRIVATE);

	/* If system call was invoked 3600 seconds ago then invoke a fresh system call */
	if (tp->tv_sec - base_tspec[clock_id].tv_sec > 3600)
	{
	  retval = SYSCALL_GETTIME (clock_id, tp);
	  if(retval < 0)
	  {
	    goto set_return_status;
	  }

	  base_tspec[clock_id].tv_sec = tp->tv_sec;
	  base_tspec[clock_id].tv_nsec = tp->tv_nsec;
	  /* Get fresh STM at this point */
	  GET_STM(base_stm[clock_id], vehva);
	}

	/*Preparing structure for timercmp in micro sec format*/
	prev_tspec_tmp.tv_sec = prev_tspec[clock_id].tv_sec;
	prev_tspec_tmp.tv_usec = (int)prev_tspec[clock_id].tv_nsec/1000;

	if (timercmp(&tvm, &prev_tspec_tmp, <))
	{
	  timersub(&prev_tspec_tmp, &tvm, &diff);
	  if (diff.tv_sec < 60)
	  {
	    tp->tv_sec = prev_tspec[clock_id].tv_sec;
	    tp->tv_nsec = prev_tspec[clock_id].tv_nsec;
	    goto set_return_status;
	  }
	}

	prev_tspec[clock_id].tv_sec = tp->tv_sec;
	prev_tspec[clock_id].tv_nsec = tp->tv_nsec;

set_return_status:
	lll_unlock (lock[clock_id], LLL_PRIVATE);
      }
      break;

      default:
      /* Calculate here for CLOCK_THREAD_CPUTIME_ID */
#if HP_TIMING_AVAIL
      if ((clock_id & ((1 << CLOCK_IDFIELD_SIZE) - 1))
	    == CLOCK_THREAD_CPUTIME_ID)
      {
	retval = hp_timing_gettime (clock_id, tp);
	break;
      }
#endif
			  /* Process here for remianing clockids if any */
#ifdef SYSDEP_GETTIME_CPU
	SYSDEP_GETTIME_CPU (clock_id, tp);
#endif
	__set_errno (EINVAL);
	break;

#if HP_TIMING_AVAIL && !defined HANDLED_CPUTIME
	case CLOCK_PROCESS_CPUTIME_ID:
	  retval = hp_timing_gettime (clock_id, tp);
	  break;
#endif
    }

  }

  return retval;
}
weak_alias (__clock_gettime, clock_gettime)
libc_hidden_def (__clock_gettime)
