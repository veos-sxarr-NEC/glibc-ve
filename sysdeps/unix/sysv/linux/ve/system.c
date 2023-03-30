/* Copyright (C) 1991-2020 Free Software Foundation, Inc.
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

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sigsetops.h>
#include <spawn.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

#include <libc-lock.h>
#include <not-errno.h>
#include <not-cancel.h>
#include <internal-signals.h>

#define VE_SYSVE_SYSTEM 101
#define VE_SYSVE_SYSTEM2 102

/* This system implementation aims to be thread-safe, which requires to
   restore the signal dispositions for SIGINT and SIGQUIT correctly and to
   deal with cancellation by terminating the child process.

   The signal disposition restoration on the single-thread case is
   straighfoward.  For multithreaded case, a reference-counter with a lock
   is used, so the first thread will set the SIGINT/SIGQUIT dispositions and
   last thread will restore them.

   Cancellation handling is done with thread cancellation clean-up handlers
   on waitpid call.  */

#ifdef _LIBC_REENTRANT
static struct sigaction intr, quit;
static int sa_refcntr;
__libc_lock_define_initialized (static, lock);

# define DO_LOCK() __libc_lock_lock (lock)
# define DO_UNLOCK() __libc_lock_unlock (lock)
# define INIT_LOCK() ({ __libc_lock_init (lock); sa_refcntr = 0; })
# define ADD_REF() sa_refcntr++
# define SUB_REF() --sa_refcntr
#else
# define DO_LOCK()
# define DO_UNLOCK()
# define INIT_LOCK()
# define ADD_REF() 0
# define SUB_REF() 0
#endif


#if defined(_LIBC_REENTRANT) && defined(SIGCANCEL)
struct cancel_handler_args
{
  struct sigaction *quit;
  struct sigaction *intr;
  pid_t pid;
};

static void
cancel_handler (void *arg)
{
  struct cancel_handler_args *args = (struct cancel_handler_args *) (arg);

  __kill_noerrno (args->pid, SIGKILL);

  int state;
  __libc_ptf_call (__pthread_setcancelstate,
                   (PTHREAD_CANCEL_DISABLE, &state), 0);
  TEMP_FAILURE_RETRY (__waitpid (args->pid, NULL, 0));
  __libc_ptf_call (__pthread_setcancelstate, (state, NULL), 0);

  DO_LOCK ();
  if (SUB_REF () == 0)
    {
      __sigaction (SIGQUIT, args->quit, NULL);
      __sigaction (SIGINT, args->intr, NULL);
    }
  DO_UNLOCK ();
}
#endif

/* Execute LINE as a shell command, returning its status.  */
static int
do_system (const char *line)
{
  int status,len;
  pid_t pid;
  struct sigaction sa;
#ifndef _LIBC_REENTRANT
  struct sigaction intr = { { 0 } } , quit = { { 0 } };
#endif
  sigset_t omask;

  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  __sigemptyset (&sa.sa_mask);

  DO_LOCK ();
  if (ADD_REF () == 0)
    {
      /* sigaction can not fail with SIGINT/SIGQUIT used with SIG_IGN.  */
      __sigaction (SIGINT, &sa, &intr);
      __sigaction (SIGQUIT, &sa, &quit);
    }
  DO_UNLOCK ();

  __sigaddset (&sa.sa_mask, SIGCHLD);
  /* sigprocmask can not fail with SIG_BLOCK used with valid input
     arguments.  */
  __sigprocmask (SIG_BLOCK, &sa.sa_mask, &omask);


      /* Cancellation results in cleanup handlers running as exceptions in
	 the block where they were installed, so it is safe to reference
	 stack variable allocate in the broader scope.  */
#if defined(_LIBC_REENTRANT) && defined(SIGCANCEL)
      struct cancel_handler_args cancel_args =
      {
	.quit = &quit,
	.intr = &intr,
	.pid = pid
      };
      __libc_cleanup_region_start (1, cancel_handler, &cancel_args);
#endif
      len = strlen(line);
      INTERNAL_SYSCALL_DECL(err);
      status = INTERNAL_SYSCALL (sysve, err, 4, VE_SYSVE_SYSTEM2,
		      (uint64_t)line, len, *((uint64_t*)&omask));
      if (__glibc_unlikely(INTERNAL_SYSCALL_ERROR_P(status, err))) {
	      __set_errno(INTERNAL_SYSCALL_ERRNO(status, err));
	      status = -1;
      }
#if defined(_LIBC_REENTRANT) && defined(SIGCANCEL)
      __libc_cleanup_region_end (0);
#endif

  DO_LOCK ();
  if (SUB_REF () == 0)
    {
      /* sigaction can not fail with SIGINT/SIGQUIT used with old
	 disposition.  Same applies for sigprocmask.  */
      __sigaction (SIGINT, &intr, NULL);
      __sigaction (SIGQUIT, &quit, NULL);
      __sigprocmask (SIG_SETMASK, &omask, NULL);
    }
  DO_UNLOCK ();

  return status;
}

int
__libc_system (const char *line)
{
  if (line == NULL)
    /* Check that we have a command processor available.  It might
       not be available after a chroot(), for example.  */
    return do_system ("exit 0") == 0;

  return do_system (line);
}
weak_alias (__libc_system, system)
