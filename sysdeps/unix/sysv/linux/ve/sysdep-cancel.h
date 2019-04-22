/* Copyright (C) 2002-2015 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Jakub Jelinek <jakub@redhat.com>, 2002.

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

/* __TBD_PORT_HCLT: ASSEMBLER LIMITATIONS
 * 1. functional symbol (calling by . or not)
 * 2. FUNCTION RETURN
 * 3. calling flow use of registers.
 * MAIN $$$$$$$$ ... built-in return 1 and cmp return 0 on true.
 * 	so either of the case need to be update
 **/

#include <sysdep.h>
#include <tls.h>
#ifndef __ASSEMBLER__
# include <nptl/pthreadP.h>
#endif

#if IS_IN (libc) || IS_IN (libpthread) || IS_IN (librt)

/* The code to disable cancellation depends on the fact that the called
   functions are special.  They don't modify registers other than %s0
   if they return.  Therefore we don't have to preserve other registers
   around these calls.  */
# undef PSEUDO
# define PSEUDO(name, syscall_name, args)				      \
.section        .data;                                                        \
.set val,       -4095;							\
  .text;								      \
  ENTRY (name)								      \
    SINGLE_THREAD_P;							      \
  .type __##syscall_name##_nocancel,@function;				      \
  .globl __##syscall_name##_nocancel;					      \
  __##syscall_name##_nocancel:						      \
    DO_CALL (syscall_name, args);					      \
   lea %s62, val;					\
     cmpu.l  %s47, %s62, %s0;                                    \
    brge.l.t 0, %s47, SYSCALL_ERROR_LABEL;                      \
    ret;								\
  .size __##syscall_name##_nocancel,.-__##syscall_name##_nocancel;	      \
  L(pseudo_cancel):							      \
    /* We always have to align the stack before calling a function.  */	      \
    lea     %sp,  -176-24(,%sp);		\
    st	    %s0, 176+8(,%sp);							      \
    st	    %lr, 176+16(,%sp);							      \
    CENABLE								      \
    /* The return value from CENABLE is argument for CDISABLE.  */	      \
    st      %s0, 176(,%sp);	/* @@@@ not feasible but tricky*/		      \
    ld      %s0, 176+8(,%sp); 	\
    DO_CALL (syscall_name, args);					      \
    /* Save %s0 since it's the error code from the syscall.  */		      \
    or      %s1, %s0, (0)1;							      \
    st	    %s1, 176+8(,%sp);							      \
    ld      %s0, 176(,%sp);						      \
    CDISABLE								      \
    ld      %s0, 176+8(,%sp);						      \
    lea     %s50, val;                                                         \
    cmpu.l  %s47, %s50, %s0;                                    \
    ld      %lr, 176+16(,%sp); 	\
    brge.l.t 0, %s47, SYSCALL_ERROR_LABEL;                      \
    lea     %sp,  176+24(,%sp);\

# if IS_IN (libpthread)
#ifdef SHARED
#  define CENABLE	lea     %s12,__pthread_enable_asynccancel@PC_LO(-24);  \
			and     %s12,%s12,(32)0;			      \
			sic     %s62; \
			lea.sl  %s12,__pthread_enable_asynccancel@PC_HI(%s12,%s62);  \
			bsic    %lr,(,%s12);

#  define CDISABLE	lea     %s12,__pthread_disable_asynccancel@PC_LO(-24); \
			and     %s12,%s12,(32)0;			      \
			sic     %s62; \
			lea.sl  %s12,__pthread_disable_asynccancel@PC_HI(%s12,%s62); \
			bsic    %lr,(,%s12);
#else
#  define CENABLE       lea     %s12,__pthread_enable_asynccancel@CALL_LO;  \
                        and     %s12,%s12,(32)0;                              \
                        lea.sl  %s12,__pthread_enable_asynccancel@CALL_HI(, %s12);  \
                        bsic    %lr,(,%s12);

#  define CDISABLE      lea     %s12,__pthread_disable_asynccancel@CALL_LO; \
                        and     %s12,%s12,(32)0;                              \
                        lea.sl  %s12,__pthread_disable_asynccancel@CALL_HI(, %s12); \
                        bsic    %lr,(,%s12);
#endif
#  define __local_multiple_threads __pthread_multiple_threads
# elif IS_IN (libc)
#ifdef SHARED
#  define CENABLE	lea     %s12,__libc_enable_asynccancel@PC_LO(-24);    \
			and     %s12,%s12,(32)0;			      \
			sic     %s62; \
			lea.sl  %s12,__libc_enable_asynccancel@PC_HI(%s12,%s62);\
			bsic    %lr,(,%s12);

#  define CDISABLE	lea     %s12,__libc_disable_asynccancel@PC_LO(-24);   \
			and     %s12,%s12,(32)0;			      \
			sic     %s62; \
			lea.sl  %s12,__libc_disable_asynccancel@PC_HI(%s12,%s62);    \
			bsic    %lr,(,%s12);
#else
#  define CENABLE	lea     %s12,__libc_enable_asynccancel@CALL_LO;    \
			and     %s12,%s12,(32)0;			      \
			lea.sl  %s12,__libc_enable_asynccancel@CALL_HI(, %s12);\
			bsic    %lr,(,%s12);

#  define CDISABLE	lea     %s12,__libc_disable_asynccancel@CALL_LO;   \
			and     %s12,%s12,(32)0;			      \
			lea.sl  %s12,__libc_disable_asynccancel@CALL_HI(, %s12);    \
			bsic    %lr,(,%s12);

#endif
#  define __local_multiple_threads __libc_multiple_threads
# elif IS_IN (librt)
#ifdef SHARED
#  define CENABLE	lea     %s12,__librt_enable_asynccancel@PC_LO(-24);   \
			and     %s12,%s12,(32)0;			      \
			sic     %s62; \
			lea.sl  %s12,__librt_enable_asynccancel@PC_HI(%s12,%s62); \
			bsic    %lr,(,%s12);

#  define CDISABLE	lea     %s12,__librt_disable_asynccancel@PC_LO(-24);   \
			and     %s12,%s12,(32)0;			      \
			sic     %s62; \
			lea.sl  %s12,__librt_disable_asynccancel@PC_HI(%s12,%s62);   \
			bsic    %lr,(,%s12);
#else
#  define CENABLE       lea     %s12,__librt_enable_asynccancel@CALL_LO;   \
                        and     %s12,%s12,(32)0;                              \
                        lea.sl  %s12,__librt_enable_asynccancel@CALL_HI(, %s12); \
                        bsic    %lr,(,%s12);

#  define CDISABLE      lea     %s12,__librt_disable_asynccancel@CALL_LO;   \
                        and     %s12,%s12,(32)0;                              \
                        lea.sl  %s12,__librt_disable_asynccancel@CALL_HI(, %s12);   \
                        bsic    %lr,(,%s12);
#endif
# else
#  error Unsupported library
# endif

# if IS_IN (libpthread) || IS_IN (libc)
#  ifndef __ASSEMBLER__
extern int __local_multiple_threads attribute_hidden;
#   define SINGLE_THREAD_P \
  __builtin_expect (__local_multiple_threads == 0, 1)
#  else
#ifdef SHARED
#   define SINGLE_THREAD_P	 \
				        lea     %s12,  __local_multiple_threads@PC_LO(-24); \
				        and     %s12,%s12,(32)0; \
				        sic     %s62; \
				        lea.sl  %s12,  __local_multiple_threads@PC_HI(%s12,%s62); \
					cmps.w  %s46, 0, %s12;	\
					brne.l 0, %s46, L(pseudo_cancel);
#else
#   define SINGLE_THREAD_P	\
					lea     %s12,  __local_multiple_threads@CALL_LO; \
                                        and     %s12, %s12, (32)0; \
                                        lea.sl  %s12,  __local_multiple_threads@CALL_HI(, %s12); \
					cmps.w  %s46, 0, %s12; \
					brne.l 0, %s46, L(pseudo_cancel);
#endif
#endif

# else

#  ifndef __ASSEMBLER__
#   define SINGLE_THREAD_P \
  __builtin_expect (THREAD_GETMEM (THREAD_SELF, \
				   header.multiple_threads) == 0, 1)
#  else
#   define SINGLE_THREAD_P   ldl.sx %s46, MULTIPLE_THREADS_OFFSET(0,%tp);	\
			     cmps.w %s46, 0, %s46;	\
			     brne.l 0, %s46, L(pseudo_cancel);
#  endif

# endif

#elif !defined __ASSEMBLER__

# define SINGLE_THREAD_P (1)
# define NO_CANCELLATION 1

#endif

#ifndef __ASSEMBLER__
# define RTLD_SINGLE_THREAD_P \
  __builtin_expect (THREAD_GETMEM (THREAD_SELF, \
				   header.multiple_threads) == 0, 1)
#endif
