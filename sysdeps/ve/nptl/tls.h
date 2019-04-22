/* Definition for thread-local data handling.  NPTL/Alpha version.
   Copyright (C) 2003-2015 Free Software Foundation, Inc.
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
   <http://www.gnu.org/licenses/>.  */
/* Changes by NEC Corporation for the VE port, 2017-2019 */

/* TODO_PORT_HCLT:
 * 1. TBD: uintptr_t stack_guard; and uintptr_t pointer_guard; position
 *    need to decide. Currently we are following memory layout as below,
 *    size == [sizeof(struct pthread)]
 *    		+[pointer_guard]
 *    		+[stack_guard]
 *    		+[sizeof(struct tcbhead_t)]
 *    		8-byte aligned.
 */

#ifndef _TLS_H
#define _TLS_H	1

# include <dl-sysdep.h>
#ifndef __ASSEMBLER__
# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>

/* Type for the dtv.  */
typedef union dtv
{
  size_t counter;
  struct
  {
    void *val;
    bool is_static;
  } pointer;
} dtv_t;

///* Get system call information.  */
//# include <sysdep.h>

/* The TP points to the start of the thread blocks.  */
# define TLS_DTV_AT_TP	1
# define TLS_TCB_AT_TP	0

/* Get the thread descriptor definition.  */
# include <nptl/descr.h>

typedef struct
{
	dtv_t *dtv;
	uintptr_t pointer_guard;
	uintptr_t stack_guard;
	int shm_offset;
	void *__private;
	char dummy_arr[8]; /*Adding dummy char array of 8 bytes to make tcb size equal to 0x30h*/
} tcbhead_t;

/*  used to initialize the offset before tls setup for VE */
extern tcbhead_t tcbobject;

/* Get system call information.  */
# include <sysdep.h>

/* This is the size of the initial TCB.  */
# define TLS_INIT_TCB_SIZE	sizeof (tcbhead_t)

/* Alignment requirements for the initial TCB.  */
# define TLS_INIT_TCB_ALIGN	__alignof__ (tcbhead_t)

/* This is the size of the TCB.  */
# define TLS_TCB_SIZE		sizeof (tcbhead_t)

/* This is the size we need before TCB. */
# define TLS_PRE_TCB_SIZE 	sizeof (struct pthread)

/* Alignment requirements for the TCB.  */
# define TLS_TCB_ALIGN		__alignof__ (struct pthread)

/* Install the dtv pointer.  The pointer passed is to the element with
   index -1 which contain the length.  */
# define INSTALL_DTV(tcbp, dtvp) \
  (((tcbhead_t *) (tcbp))->dtv = (dtvp) + 1)

/* Install new dtv for current thread.  */
# define INSTALL_NEW_DTV(dtv) \
  ({ tcbhead_t *__tcbp;							      \
	 __asm __volatile ("lea %0, (%%tp)" : "=r" (__tcbp));		      \
	 __tcbp->dtv = (dtv);})

/* Return dtv of given thread descriptor.  */
# define GET_DTV(tcbp) \
  (((tcbhead_t *) (tcbp))->dtv)

/* Code to initially initialize the thread pointer.  This might need
   special attention since 'errno' is not yet available and if the
   operation can cause a failure 'errno' must not be touched.  */
# define TLS_INIT_TP(tcbp) \
  ({ __asm __volatile ("or %%tp, 0, %0": :"r" (tcbp)); NULL; })

/* Value passed to 'clone' for initialization of the thread register.  */
# define TLS_DEFINE_INIT_TP(tp, pd) \
   void *tp = (char *) (pd) + TLS_PRE_TCB_SIZE

/* Return the address of the dtv for the current thread.  */
# define THREAD_DTV() \
  ({ tcbhead_t *__tcbp;							      \
	 __asm __volatile ("lea %0, (%%tp)" : "=r" (__tcbp));		      \
	 __tcbp->dtv;})

/* Return the thread descriptor for the current thread.  */
# define THREAD_SELF \
 ({ tcbhead_t *__tcbp = NULL;						      \
     __asm __volatile("lea %0, (%%tp)":"+r"(__tcbp));			      \
    (struct pthread *) ((char *) (__tcbp) - TLS_PRE_TCB_SIZE);		      \
    })

/* Magic for libthread_db to know how to do THREAD_SELF.  */
/* tp register == s14 */
# define INT_REGISTER_SIZE	8
# define VEGP_SCALAR_OFFSET	(0x1400)
# define REG_TP_OFFSET	(VEGP_SCALAR_OFFSET + INT_REGISTER_SIZE * 14)
# define DB_THREAD_SELF REGISTER (64, 64, REG_TP_OFFSET, -TLS_PRE_TCB_SIZE)

/* Access to data in the thread descriptor is easy.  */
#define THREAD_GETMEM(descr, member) \
  descr->member
#define THREAD_GETMEM_NC(descr, member, idx) \
  descr->member[idx]
#define THREAD_SETMEM(descr, member, value) \
  descr->member = (value)
#define THREAD_SETMEM_NC(descr, member, idx, value) \
  descr->member[idx] = (value)

/* Set the stack guard field in TCB head.  */
#define THREAD_SET_STACK_GUARD(value) \
	({ tcbhead_t *__tcbp;						      \
	         __asm __volatile ("lea %0, (%%tp)" : "=r" (__tcbp));	      \
		 __tcbp->stack_guard = (value);				      \
	})
#define THREAD_COPY_STACK_GUARD(descr) \
	({ tcbhead_t *__tcbp = NULL;					      \
	   tcbhead_t *_tcbp;						      \
	 __asm __volatile("lea %0, (%%tp)":"+r"(__tcbp));		      \
	 _tcbp = (tcbhead_t *) ((char *) (descr) + TLS_PRE_TCB_SIZE);	      \
	 _tcbp->stack_guard = __tcbp->stack_guard;			      \
	 ((tcbhead_t *) ((char *) (descr) + TLS_PRE_TCB_SIZE))->stack_guard   \
			= __tcbp->stack_guard;				      \
	 })

/* Set the pointer guard field in TCB head.  */
#define THREAD_GET_POINTER_GUARD() \
	({ tcbhead_t *__tcbp = NULL;					      \
	 __asm __volatile("lea %0, (%%tp)":"+r"(__tcbp));		      \
	 __tcbp->pointer_guard;						      \
	 })
#define THREAD_SET_POINTER_GUARD(value) \
	({ tcbhead_t *__tcbp = NULL;					      \
	 __asm __volatile("lea %0, (%%tp)":"+r"(__tcbp));		      \
	 __tcbp->pointer_guard = (value);				      \
	})
#define THREAD_COPY_POINTER_GUARD(descr) \
  ({ tcbhead_t *_tcbp;							      \
	_tcbp = (tcbhead_t *) ((char *) (descr) + TLS_PRE_TCB_SIZE);	      \
	_tcbp->pointer_guard = THREAD_GET_POINTER_GUARD ();		      \
  })

/* Get and set the global scope generation counter in struct pthread.  */
#define THREAD_GSCOPE_FLAG_UNUSED 0
#define THREAD_GSCOPE_FLAG_USED   1
#define THREAD_GSCOPE_FLAG_WAIT   2
#define THREAD_GSCOPE_RESET_FLAG() \
  do									     \
    { int __res								     \
	= atomic_exchange_rel (&THREAD_SELF->header.gscope_flag,	     \
			       THREAD_GSCOPE_FLAG_UNUSED);		     \
      if (__res == THREAD_GSCOPE_FLAG_WAIT)				     \
	lll_futex_wake (&THREAD_SELF->header.gscope_flag, 1, LLL_PRIVATE);   \
    }									     \
  while (0)
#define THREAD_GSCOPE_SET_FLAG() \
  do									     \
    {									     \
      THREAD_SELF->header.gscope_flag = THREAD_GSCOPE_FLAG_USED;	     \
      atomic_write_barrier ();						     \
    }									     \
  while (0)
#define THREAD_GSCOPE_WAIT() \
  GL(dl_wait_lookup_done) ()

#else /* __ASSEMBLER__ */
# include <tcb-offsets.h>
#endif /* __ASSEMBLER__ */
//#undef __ASSEMBLER__

#endif	/* tls.h */
