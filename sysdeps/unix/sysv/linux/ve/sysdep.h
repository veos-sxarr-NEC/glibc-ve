/* Copyright (C) 2001-2020 Free Software Foundation, Inc.
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

#ifndef _LINUX_VE_SYSDEP_H
#define _LINUX_VE_SYSDEP_H 1

/* There is some commonality.  */
#include <sysdeps/unix/sysv/linux/sysdep.h>
#include <sysdeps/unix/ve/sysdep.h>
#include <tls.h>
#include <tcb-offsets.h>

/* Defines RTLD_PRIVATE_ERRNO.  */
#include <dl-sysdep.h>

/* For Linux we can use the system call table in the header file
	/usr/include/asm/unistd.h
   of the kernel.  But these symbols do not follow the SYS_* syntax
   so we have to redefine the `SYS_ify' macro here.  */
#undef SYS_ify
#define SYS_ify(syscall_name)	__NR_##syscall_name

#undef DELAY_FOR_VLFA_EXCEPTION
#define DELAY_FOR_VLFA_EXCEPTION \
"or %s63,10,(0)1\n\t" \
"brgt.w  0,%s63,32\n\t" \
"fencei\n\t" \
"subs.w.sx  %s63,%s63,(63)0\n\t" \
"br.l -24\n\t" \
"nop\n\t"

/* This is to help the old kernel headers where __NR_semtimedop is not
   available.  */
#ifndef __NR_semtimedop
# define __NR_semtimedop 220
#endif


#ifdef __ASSEMBLER__

/* Linux uses a negative return value to indicate syscall errors,
   unlike most Unices, which use the condition codes' carry flag.

   Since version 2.1 the return value of a system call might be
   negative even if the call succeeded.	 E.g., the `lseek' system call
   might return a large offset.	 Therefore we must not anymore test
   for < 0, but test for a real error by making sure the value in %eax
   is a real error number.  Linus said he will make sure the no syscall
   returns a value in -1 .. -4095 as a valid result so we can savely
   test with -4095.  */

/* We don't want the label for the error handle to be global when we define
   it here.  */
# ifdef PIC
#  define SYSCALL_ERROR_LABEL 0f
# else
#  define SYSCALL_ERROR_LABEL syscall_error
# endif

# undef	PSEUDO
# define PSEUDO(name, syscall_name, args)				      \
  .section .data;                                               \
        sys_##syscall_name: .int SYS_ify(syscall_name);         \
        .set err,       -4095;                                                  \
  .text;                                                                      \
  ENTRY (name)                                                                \
    DO_CALL (syscall_name, args);                                             \
    lea %s62, err;                                              \
    cmpu.l  %s47, %s62, %s0;                                    \
    brge.l.t 0, %s47, SYSCALL_ERROR_LABEL;                      \


# undef	PSEUDO_END
# define PSEUDO_END(name)						      \
  SYSCALL_ERROR_HANDLER							      \
  END (name)

# undef	PSEUDO_NOERRNO
# define PSEUDO_NOERRNO(name, syscall_name, args) \
 .section .data;                                                              \
        sys_##syscall_name: .int SYS_ify(syscall_name);                        \
  .text;								      \
  ENTRY (name)								      \
    DO_CALL (syscall_name, args)

# undef	PSEUDO_END_NOERRNO
# define PSEUDO_END_NOERRNO(name) \
  END (name)

# define ret_NOERRNO DO_RET

# undef	PSEUDO_ERRVAL
# define PSEUDO_ERRVAL(name, syscall_name, args) \
  .section .data                                                              \
        sys_##syscall_name: .int SYS_ify(syscall_name)                        \
  .text;								      \
  ENTRY (name)								      \
   DO_CALL (syscall_name, args);					      \
   mulu.l %s0, -1, %s0

# undef	PSEUDO_END_ERRVAL
# define PSEUDO_END_ERRVAL(name) \
  END (name)

# define ret_ERRVAL ret

/* ld recognizes rtld_errno so access it directly
 * SYSCALL_SET_ERRNO sets the errno by multiplying the kernal returned errno by -1
 * In case of libc module or modules other than rtld, errno is a TLS variable */
# if defined PIC && RTLD_PRIVATE_ERRNO && IS_IN (rtld)
#  define SYSCALL_SET_ERRNO			\
  addi %s59,0,rtld_errno@GOTOFF_LO;		\
  lea.sl  %s59, rtld_errno@GOTOFF_HI(%got, %s59);	\
  mulu.l  %s0, -1, %s0;				\
  st	  %s0, (,%s59);
# else
#  if IS_IN (libc)
#   define SYSCALL_ERROR_ERRNO __libc_errno
#  else
#   define SYSCALL_ERROR_ERRNO errno
#  endif
/* Calculate errno for TLS variable errno or __libc_errno */
#  define SYSCALL_SET_ERRNO			\
  lea	%sp, -176-50(,%sp); \
  st	%got, 184(,%sp);\
  st	%plt, 192(,%sp);\
  st	%lr, 200(,%sp);\
  st	%s18, 208(,%sp);\
  addi %got,-16,_GLOBAL_OFFSET_TABLE_@PC_LO;\
  sic	%plt;\
  lea.sl  %got,_GLOBAL_OFFSET_TABLE_@PC_HI(%got,%plt);\
  or	  %s18, 0, %s0;   \
  addi %s0,-16,SYSCALL_ERROR_ERRNO@TLS_GD_LO;  \
  sic     %lr;    \
  lea.sl  %s0, SYSCALL_ERROR_ERRNO@TLS_GD_HI(%s0,%lr);\
  addi %s12,8,__tls_get_addr@PLT_LO;       \
  lea.sl  %s12,__tls_get_addr@PLT_HI(%s12,%lr);   \
  bsic    %lr,(,%s12);    \
  mulu.l  %s18, -1, %s18; \
  st      %s18, (,%s0);	\
  ld	  %got,	184(,%sp);\
  ld	  %plt,	192(,%sp);\
  ld	  %lr,	200(,%sp);\
  ld	  %s18,	208(,%sp);\
  lea	  %sp, 176+50(,%sp);
# endif

# ifndef PIC
#  define SYSCALL_ERROR_HANDLER	/* Nothing here; code in sysdep.S is used.  */
# else
#  define SYSCALL_ERROR_HANDLER			\
0:						\
  SYSCALL_SET_ERRNO;				\
  lea %s59, -1;					\
  xor %s0, %s0, %s0;				\
  or %s0, %s59, (0)1;				\
  beq.l.t 0, 0x0(,%lr);
# endif	/* PIC */

/* The VEOS expects the system call parameters in
   registers according to the following table:

    syscall number	S0	
    arg 1		S1
    arg 2		S2
    arg 3		S3
    arg 4		S4
    arg 5		S5
    arg 6		S6

    The Linux kernel uses and destroys internally these registers:
    return address from
    syscall		rcx
    eflags from syscall	r11

    Normal function call, including calls to the system call stub
    functions in the libc, get the first six parameters passed in
    registers and the seventh parameter and later on the stack.  The
    register use is as follows:

     system call number	in the DO_CALL macro
     arg 1		S1
     arg 2		S2
     arg 3		S3
     arg 4		S4
     arg 5		S5
     arg 6		S6

    We have to take care that the stack is aligned to 16 bytes.  When
    called the stack is not aligned since the return address has just
    been pushed.


    Syscalls of more than 6 arguments are not supported.  */

# undef	DO_CALL
# define DO_CALL(syscall_name, args)		\
    ld	%s46, 0x18(,%tp);		\
    DOARGS_##args				\
    lea 	%s0, __NR_##syscall_name;       \
    shm.l 	%s0, 0x0(%s46);	         	\
    or %s63,10,(0)1; \
    brgt.w  0,%s63,32; \
    fencei ; \
    subs.w.sx  %s63,%s63,(63)0; \
    br.l -24; \
    nop; \
    monc;

# define DOARGS_0 /* nothing */
# define DOARGS_1 \
	or	%s1, %s0, %s0;			\
	shm.l	%s1, 0x8(%s46);			\
	DOARGS_0
# define DOARGS_2 \
        or      %s2, %s1, %s1;		        \
        shm.l   %s2, 0x10(%s46);                \
        DOARGS_1
# define DOARGS_3 \
        or      %s3, %s2, %s2;                  \
        shm.l   %s3, 0x18(%s46);                \
        DOARGS_2
# define DOARGS_4 \
        or      %s4, %s3, %s3;                  \
        shm.l   %s4, 0x20(%s46);                \
        DOARGS_3
# define DOARGS_5 \
        or      %s5, %s4, %s4;                  \
        shm.l   %s5, 0x28(%s46);                \
        DOARGS_4
# define DOARGS_6 \
        or      %s6, %s5, %s5;                  \
        shm.l   %s6, 0x30(%s46);                \
        DOARGS_5

#else	/* !__ASSEMBLER__ */
#define LP_SIZE 8
#define LP_SIZE1 offsetof (tcbhead_t, pointer_guard)

/*Function gets clock frequency*/
extern ssize_t _ve_get_ve_info(const char *name, char *buffer, size_t size);

#undef GET_STM
#define GET_STM(ret_stm, vehva) \
asm volatile("lhm.l %0,0(%1)":"=r"(ret_stm):"r"(vehva));
/* Define a macro which expands inline into the wrapper code for a system
   call.  */
# undef INLINE_SYSCALL
# define INLINE_SYSCALL(name, nr, args...) \
  ({									      \
    unsigned long int resultvar = INTERNAL_SYSCALL (name, , nr, args);	      \
    if (__glibc_unlikely (INTERNAL_SYSCALL_ERROR_P (resultvar, )))	      \
      {									      \
	__set_errno (INTERNAL_SYSCALL_ERRNO (resultvar, ));		      \
	resultvar = (unsigned long int) -1;				      \
      }									      \
    (long int) resultvar; })

/* Define a macro with explicit types for arguments, which expands inline
   into the wrapper code for a system call.  It should be used when size
   of any argument > size of long int.  */
# undef INLINE_SYSCALL_TYPES
# define INLINE_SYSCALL_TYPES(name, nr, args...) \
  ({									      \
    unsigned long int resultvar = INTERNAL_SYSCALL_TYPES (name, , nr, args);  \
    if (__glibc_unlikely (INTERNAL_SYSCALL_ERROR_P (resultvar, )))	      \
      {									      \
	__set_errno (INTERNAL_SYSCALL_ERRNO (resultvar, ));		      \
	resultvar = (unsigned long int) -1;				      \
      }									      \
    (long int) resultvar; })

# undef INTERNAL_SYSCALL_DECL
# define INTERNAL_SYSCALL_DECL(err) do { } while (0)

/* Registers clobbered by syscall.  */
# define REGISTERS_CLOBBERED_BY_SYSCALL "s0"

/* Create a variable 'name' based on type 'X' to avoid explicit types.
   This is mainly used set use 64-bits arguments in x32.   */
#define TYPEFY(X, name) __typeof__ ((X) - (X)) name
/* Explicit cast the argument to avoid integer from pointer warning on
   x32.  */
#define ARGIFY(X) ((__typeof__ ((X) - (X))) (X))

#undef INTERNAL_SYSCALL
#define INTERNAL_SYSCALL(name, err, nr, args...)			\
	internal_syscall##nr (SYS_ify (name), err, args)

#undef INTERNAL_SYSCALL_NCS
#define INTERNAL_SYSCALL_NCS(number, err, nr, args...)			\
	internal_syscall##nr (number, err, args)

# define LOAD_SYS_NAME(name)                                               \
  long int register __arg0 asm("%s0") = (long int) name;                                           \
  asm volatile("shm.l %0, 0x0(%%s46)\n\t"::"r"(__arg0));

#undef internal_syscall0
#define internal_syscall0(number, err, dummy...)			\
({									\
    unsigned long int resultvar;					\
    unsigned long long int s46var;\
    asm volatile ("st %%s46, %0\n\t" :: "m"(*&s46var) : "%s46");\
    asm volatile (                                                  \
    "\nxor %%s46, %%s46, %%s46; ld %%s46, %0(,%%tp)\n\t" : : "i"(SHM_OFFSET));\
    LOAD_SYS_NAME (number)						\
    asm volatile (							\
    DELAY_FOR_VLFA_EXCEPTION						\
    );									\
    asm volatile (							\
    "monc\n\t"                                                      \
    "or %0, 0, %%s0\n\t"                                                \
    : "=r" (resultvar)							\
    :							\
    : "%s0");			\
    asm volatile ("ld %%s46, %0\n\t" : "=m"(*&s46var):: "%s46");\
    (long int) resultvar;						\
})

#undef internal_syscall1
#define internal_syscall1(number, err, arg1)				\
({									\
    unsigned long int resultvar;					\
    unsigned long long int s46var;\
    asm volatile ("st %%s46, %0\n\t" :: "m"(*&s46var) : "%s46");\
    asm volatile (                                                      \
    "\nxor %%s46, %%s46, %%s46; ld %%s46, %0(,%%tp)\n\t" : : "i"(SHM_OFFSET));\
    LOAD_SYS_NAME (number)						\
    TYPEFY (arg1, __arg1) = ARGIFY (arg1);			 	\
    register TYPEFY (arg1, _a1) asm ("%s1");                            \
    asm volatile("or %0,0,%1": "=r"(_a1): "r"(__arg1));		        \
    LOAD_SHM_1								\
    asm volatile (							\
    DELAY_FOR_VLFA_EXCEPTION						\
    );									\
    asm volatile (							\
    "monc\n\t"                                                      \
    "or %0, 0, %%s0\n\t"                                                \
    : "=r" (resultvar)							\
    :             							\
    : "%s0");			\
    asm volatile ("ld %%s46, %0\n\t" : "=m"(*&s46var):: "%s46");\
    (long int) resultvar;						\
})

#undef internal_syscall2
#define internal_syscall2(number, err, arg1, arg2)			\
({									\
    unsigned long int resultvar;					\
    unsigned long long int s46var;\
    asm volatile ("st %%s46, %0\n\t" :: "m"(*&s46var) : "%s46");\
    asm volatile (                                                      \
    "\nxor %%s46, %%s46, %%s46; ld %%s46, %0(,%%tp)\n\t" : : "i"(SHM_OFFSET));\
    LOAD_SYS_NAME (number)						\
    TYPEFY (arg2, __arg2) = ARGIFY (arg2);			 	\
    TYPEFY (arg1, __arg1) = ARGIFY (arg1);			 	\
    register TYPEFY (arg2, _a2) asm ("%s2");    			\
    register TYPEFY (arg1, _a1) asm ("%s1");       			\
    asm volatile("or %0,0,%1": "=r"(_a1): "r"(__arg1));		        \
    asm volatile("or %0,0,%1": "=r"(_a2): "r"(__arg2));		        \
    LOAD_SHM_2								\
    asm volatile (							\
    DELAY_FOR_VLFA_EXCEPTION						\
    );									\
    asm volatile (							\
    "monc\n\t"                                                      \
    "or %0, 0, %%s0\n\t"                                                \
    : "=r" (resultvar)							\
    : 				\
    : "%s0");			\
    asm volatile ("ld %%s46, %0\n\t" : "=m"(*&s46var):: "%s46");\
    (long int) resultvar;						\
})

#undef internal_syscall3
#define internal_syscall3(number, err, arg1, arg2, arg3)		\
({									\
    unsigned long int resultvar;					\
    unsigned long long int s46var;\
    asm volatile ("st %%s46, %0\n\t" :: "m"(*&s46var): "%s46");\
    asm volatile (                                                      \
    "\nxor %%s46, %%s46, %%s46; ld %%s46, %0(,%%tp)\n\t" : : "i"(SHM_OFFSET));\
    LOAD_SYS_NAME (number)						\
    TYPEFY (arg3, __arg3) = ARGIFY (arg3);			 	\
    TYPEFY (arg2, __arg2) = ARGIFY (arg2);			 	\
    TYPEFY (arg1, __arg1) = ARGIFY (arg1);			 	\
    register TYPEFY (arg3, _a3) asm ("%s3");				\
    register TYPEFY (arg2, _a2) asm ("%s2");				\
    register TYPEFY (arg1, _a1) asm ("%s1");				\
    asm volatile("or %0,0,%1": "=r"(_a1): "r"(__arg1));		        \
    asm volatile("or %0,0,%1": "=r"(_a2): "r"(__arg2));		        \
    asm volatile("or %0,0,%1": "=r"(_a3): "r"(__arg3));		        \
    LOAD_SHM_3								\
    asm volatile (							\
    DELAY_FOR_VLFA_EXCEPTION						\
    );									\
    asm volatile (							\
    "monc\n\t"  	                                                    \
    "or %0, 0, %%s0\n\t"                                                \
    : "=r" (resultvar)							\
    : 									\
    : "%s0");			\
    asm volatile ("ld %%s46, %0\n\t" : "=m"(*&s46var):: "%s46");\
    (long int) resultvar;						\
})

#undef internal_syscall4
#define internal_syscall4(number, err, arg1, arg2, arg3, arg4)		\
({									\
    unsigned long int resultvar;					\
    unsigned long long int s46var;\
    asm volatile ("st %%s46, %0\n\t" :: "m"(*&s46var) : "%s46");\
    asm volatile (                                                      \
    "\nxor %%s46, %%s46, %%s46; ld %%s46, %0(,%%tp)\n\t" : : "i"(SHM_OFFSET));\
    LOAD_SYS_NAME (number)						\
    TYPEFY (arg4, __arg4) = ARGIFY (arg4);			 	\
    TYPEFY (arg3, __arg3) = ARGIFY (arg3);			 	\
    TYPEFY (arg2, __arg2) = ARGIFY (arg2);			 	\
    TYPEFY (arg1, __arg1) = ARGIFY (arg1);			 	\
    register TYPEFY (arg4, _a4) asm ("%s4");			\
    register TYPEFY (arg3, _a3) asm ("%s3");			\
    register TYPEFY (arg2, _a2) asm ("%s2");			\
    register TYPEFY (arg1, _a1) asm ("%s1");			\
    asm volatile("or %0,0,%1": "=r"(_a1): "r"(__arg1));		        \
    asm volatile("or %0,0,%1": "=r"(_a2): "r"(__arg2));		        \
    asm volatile("or %0,0,%1": "=r"(_a3): "r"(__arg3));		        \
    asm volatile("or %0,0,%1": "=r"(_a4): "r"(__arg4));		        \
    LOAD_SHM_4							\
    asm volatile (							\
    DELAY_FOR_VLFA_EXCEPTION						\
    );									\
    asm volatile (							\
    "monc\n\t"                                                      \
    "or %0, 0, %%s0\n\t"                                                \
    : "=r" (resultvar)							\
    : 									\
    : "%s0");			\
    asm volatile ("ld %%s46, %0\n\t" : "=m"(*&s46var):: "%s46");\
    (long int) resultvar;						\
})

#undef internal_syscall5
#define internal_syscall5(number, err, arg1, arg2, arg3, arg4, arg5)	\
({									\
    unsigned long int resultvar;					\
    unsigned long long int s46var;\
    asm volatile ("st %%s46, %0\n\t" :: "m"(*&s46var) : "%s46");\
    asm volatile (                                                      \
    "\nxor %%s46, %%s46, %%s46; ld %%s46, %0(,%%tp)\n\t" : : "i"(SHM_OFFSET));\
    LOAD_SYS_NAME (number)						\
    TYPEFY (arg5, __arg5) = ARGIFY (arg5);			 	\
    TYPEFY (arg4, __arg4) = ARGIFY (arg4);			 	\
    TYPEFY (arg3, __arg3) = ARGIFY (arg3);			 	\
    TYPEFY (arg2, __arg2) = ARGIFY (arg2);			 	\
    TYPEFY (arg1, __arg1) = ARGIFY (arg1);			 	\
    register TYPEFY (arg5, _a5) asm ("%s5");			\
    register TYPEFY (arg4, _a4) asm ("%s4");			\
    register TYPEFY (arg3, _a3) asm ("%s3");			\
    register TYPEFY (arg2, _a2) asm ("%s2");			\
    register TYPEFY (arg1, _a1) asm ("%s1");			\
    asm volatile("or %0,0,%1": "=r"(_a1): "r"(__arg1));		        \
    asm volatile("or %0,0,%1": "=r"(_a2): "r"(__arg2));		        \
    asm volatile("or %0,0,%1": "=r"(_a3): "r"(__arg3));		        \
    asm volatile("or %0,0,%1": "=r"(_a4): "r"(__arg4));		        \
    asm volatile("or %0,0,%1": "=r"(_a5): "r"(__arg5));		        \
    LOAD_SHM_5							\
    asm volatile (							\
    DELAY_FOR_VLFA_EXCEPTION						\
    );									\
    asm volatile (							\
    "monc\n\t"                                                      \
    "or %0, 0, %%s0\n\t"                                                \
    : "=r" (resultvar)							\
    : 									\
    : "%s0");			\
    asm volatile ("ld %%s46, %0\n\t" : "=m"(*&s46var):: "%s46");\
    (long int) resultvar;						\
})


#undef internal_syscall6
#define internal_syscall6(number, err, arg1, arg2, arg3, arg4, arg5, arg6) \
({									\
    unsigned long int resultvar;					\
    unsigned long long int s46var;\
    asm volatile ("st %%s46, %0\n\t" :: "m"(*&s46var) : "%s46");\
    asm volatile (                                                      \
    "\nxor %%s46, %%s46, %%s46; ld %%s46, %0(,%%tp)\n\t" : : "i"(SHM_OFFSET));\
    LOAD_SYS_NAME (number)						\
    TYPEFY (arg6, __arg6) = ARGIFY (arg6);			 	\
    TYPEFY (arg5, __arg5) = ARGIFY (arg5);			 	\
    TYPEFY (arg4, __arg4) = ARGIFY (arg4);			 	\
    TYPEFY (arg3, __arg3) = ARGIFY (arg3);			 	\
    TYPEFY (arg2, __arg2) = ARGIFY (arg2);			 	\
    TYPEFY (arg1, __arg1) = ARGIFY (arg1);			 	\
    register TYPEFY (arg6, _a6) asm ("%s6");			\
    register TYPEFY (arg5, _a5) asm ("%s5");			\
    register TYPEFY (arg4, _a4) asm ("%s4");			\
    register TYPEFY (arg3, _a3) asm ("%s3");			\
    register TYPEFY (arg2, _a2) asm ("%s2");			\
    register TYPEFY (arg1, _a1) asm ("%s1");			\
    asm volatile("or %0,0,%1": "=r"(_a1): "r"(__arg1));                 \
    asm volatile("or %0,0,%1": "=r"(_a2): "r"(__arg2));                 \
    asm volatile("or %0,0,%1": "=r"(_a3): "r"(__arg3));                 \
    asm volatile("or %0,0,%1": "=r"(_a4): "r"(__arg4));                 \
    asm volatile("or %0,0,%1": "=r"(_a5): "r"(__arg5));                 \
    asm volatile("or %0,0,%1": "=r"(_a6): "r"(__arg6));			\
    LOAD_SHM_6							\
    asm volatile (							\
    DELAY_FOR_VLFA_EXCEPTION						\
    );									\
    asm volatile (							\
    "monc\n\t"                                                      \
    "or %0, 0, %%s0\n\t"                                                \
    : "=r" (resultvar)							\
    :									\
    : "%s0");			\
    asm volatile ("ld %%s46, %0\n\t" : "=m"(*&s46var):: "%s46");\
    (long int) resultvar;						\
})

# define LOAD_SHM_0

# define LOAD_SHM_1						   \
  asm volatile("shm.l %0, 0x8(%%s46)\n\t"::"r"(_a1));	           \
  LOAD_SHM_0

# define LOAD_SHM_2						   \
  asm volatile("shm.l %0, 0x10(%%s46)\n\t"::"r"(_a2)); 	           \
  LOAD_SHM_1

# define LOAD_SHM_3						   \
  asm volatile("shm.l %0, 0x18(%%s46)\n\t"::"r"(_a3));                   \
  LOAD_SHM_2

# define LOAD_SHM_4						   \
  asm volatile("shm.l %0, 0x20(%%s46)\n\t"::"r"(_a4));                   \
  LOAD_SHM_3

# define LOAD_SHM_5						   \
  asm volatile("shm.l %0, 0x28(%%s46)\n\t"::"r"(_a5)); 	           \
  LOAD_SHM_4

# define LOAD_SHM_6						   \
  asm volatile("shm.l %0, 0x30(%%s46)\n\t"::"r"(_a6));	           \
  LOAD_SHM_5

# undef INTERNAL_SYSCALL_ERROR_P
# define INTERNAL_SYSCALL_ERROR_P(val, err) \
  ((unsigned long int) (long int) (val) >= -4095L)

# undef INTERNAL_SYSCALL_ERRNO
# define INTERNAL_SYSCALL_ERRNO(val, err)	(-(val))

# define SINGLE_THREAD_BY_GLOBAL		1

#endif	/* __ASSEMBLER__ */


/* Pointer mangling support.  */
#if IS_IN (rtld)
/* We cannot use the thread descriptor because in ld.so we use setjmp
   earlier than the descriptor is initialized.  */
# ifdef __ASSEMBLER__
#  define PTR_MANGLE(reg)	xor     %s63, %s63, %s63			\
		addi %s63,0,__pointer_chk_guard_local@got_lo 	       		\
		lea.sl  %s63, __pointer_chk_guard_local@got_hi(%s63,%s15)	\
		ld      %s62, (,%s63)						\
		xor 	reg, %s62, reg;		 				\
		xor     %s61, %s61, %s61                        		\
		sll	%s61, reg, $2*LP_SIZE+1					\
		xor     %s60, %s60, %s60                        		\
		srl	%s60, reg, $2*LP_SIZE+1                         	\
		or	reg, %s60, %s61

#  define PTR_DEMANGLE(reg)	xor     %s63, %s63, %s63                        \
		srl     %s63, reg, $2*LP_SIZE+1                         	\
		xor     %s62, %s62, %s62                                	\
		sll     %s62, reg, $2*LP_SIZE+1		                        \
		or      reg, %s62, %s63						\
		xor     %s61, %s61, %s61   	                                \
                addi %s61,0,__pointer_chk_guard_local@CALL_LO                   \
                lea.sl  %s61, __pointer_chk_guard_local@CALL_HI(%s61,%s15)      \
                ld      %s60, (,%s61)                                           \
                xor     %reg, %s60, reg;
# else
#  define PTR_MANGLE(reg)	asm ("xor     %%s63, %%s63, %%s63\n"		\
		"addi %%s63,0,__pointer_chk_guard_local@got_lo\n"		\
		"lea.sl  %%s63, __pointer_chk_guard_local@got_hi(%%s63,%%s15)\n"\
		"ld      %%s62, (,%%s63)\n"					\
		"xor     %0, %%s62, %1;\n"					\
		"xor     %%s61, %%s61, %%s61\n"					\
		"sll     %%s61, %1, %3\n"					\
		"xor     %%s60, %%s60, %%s60\n"					\
		"srl     %%s60, %1, %3\n"					\
		"or      %0, %%s60, %%s61\n"					\
		: "=r" (reg) : "r" (reg), "i"(32), "i"(2*LP_SIZE+1))

#define PTR_DEMANGLE(reg)	asm ("xor     %%s63, %%s63, %%s63\n"		\
		"srl     %%s63, %1, %3\n"					\
		"xor     %%s62, %%s62, %%s62\n"					\
		"sll     %%s62, %1, %3\n"					\
		"or      %0, %%s63, %%s62\n"					\
		"xor     %%s61, %%s61, %%s61\n"					\
		"addi %%s61,0,__pointer_chk_guard_local@CALL_LO\n"		\
		"lea.sl  %%s61, __pointer_chk_guard_local@CALL_HI(,%%s61)\n"	\
		"ld      %%s60, (,%%s61)\n"					\
		"xor     %0, %%s60, %1;\n"					\
		: "=r" (reg) : "r" (reg), "i"(32), "i"(2*LP_SIZE+1))
# endif
#else
# ifdef __ASSEMBLER__
#  define PTR_MANGLE(reg)	ld %s61, POINTER_GUARD(,%tp)		\
			xor 	reg, %s61, reg;				\
			xor     %s63, %s63, %s63			\
                        sll     %s63, reg, 2*LP_SIZE+1			\
                        xor     %s62, %s62, %s62			\
                        srl     %s62, reg, 2*LP_SIZE+1			\
                        or      reg, %s62, %s63			\

#define PTR_DEMANGLE(reg)	xor     %s63, %s63, %s63		\
			srl     %s63, reg, 2*LP_SIZE+1			\
                        xor     %s62, %s62, %s62			\
                        sll     %s62, reg, 2*LP_SIZE+1			\
                        or      reg, %s63, %s62				\
			ld 	%s61, POINTER_GUARD(,%tp)          	\
			xor 	reg, %s61, reg
# else
#define PTR_MANGLE(var)		asm("ld %%s63, %1(,%%tp)\n"		\
			"xor 	 %0, %%s63, %0\n"			\
			"xor     %%s62, %%s62, %%s62\n"			\
			"sll     %%s62, %0, %2\n"			\
			"xor	 %%s61, %%s61, %%s61\n"			\
			"srl     %%s61, %0, %3\n"			\
			"or      %0, %%s62, %%s61"			\
			: "+r" (var)					\
			: "i" (LP_SIZE1),				\
			  "i" (2*LP_SIZE+1),				\
			  "i" (sizeof(var)*8 - (2*LP_SIZE+1)))

# define PTR_DEMANGLE(var)	asm ("xor     %%s63, %%s63, %%s63\n"	\
			"srl     %%s63, %0, %2\n"			\
			"xor     %%s62, %%s62, %%s62\n"			\
			"sll     %%s62, %0, %3\n"			\
			"or      %0, %%s63, %%s62\n"			\
			"ld 	 %%s61, %1(,%%tp)\n"           		\
			"xor 	 %0, %%s61, %0\n"			\
                        : "+r" (var)                                    \
			: "i" (LP_SIZE1),       			\
                          "i" (2*LP_SIZE+1),				\
			  "i" (sizeof(var)*8 - (2*LP_SIZE+1)))
# endif
#endif

/* How to pass the off{64}_t argument on p{readv,writev}{64}.  */
#undef LO_HI_LONG
#define LO_HI_LONG(val) (val), 0

/* Each shadow stack slot takes 8 bytes.  Assuming that each stack
   frame takes 256 bytes, this is used to compute shadow stack size
   from stack size.  */
//#define STACK_SIZE_TO_SHADOW_STACK_SIZE_SHIFT 5

#endif /* linux/ve/sysdep.h */
