/* Copyright (C) 2001-2015 Free Software Foundation, Inc.
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

#ifndef _LINUX_VE_SYSDEP_H
#define _LINUX_VE_SYSDEP_H 1

/* There is some commonality.  */
#include <sysdeps/unix/ve/sysdep.h>
#include <tls.h>
#include <tcb-offsets.h>

#if IS_IN (rtld)
# include <dl-sysdep.h>		/* Defines RTLD_PRIVATE_ERRNO.  */
#endif

/* For Linux we can use the system call table in the header file
	/usr/include/asm/unistd.h
   of the kernel.  But these symbols do not follow the SYS_* syntax
   so we have to redefine the `SYS_ify' macro here.  */
#undef SYS_ify
#define SYS_ify(syscall_name)	__NR_##syscall_name

/* This is a kludge to make syscalls.list find these under the names
   pread and pwrite, since some kernel headers define those names
   and some define the *64 names for the same system calls.  */
#if !defined __NR_pread && defined __NR_pread64
# define __NR_pread __NR_pread64
#endif
#if !defined __NR_pwrite && defined __NR_pwrite64
# define __NR_pwrite __NR_pwrite64
#endif

/* This is to help the old kernel headers where __NR_semtimedop is not
   available */
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
# define PSEUDO(name, syscall_name, args)			\
  .section .data;                                               \
        sys_##syscall_name: .int SYS_ify(syscall_name);         \
	.set err,       -4095;					\
 .text;								\
  ENTRY (name)							\
    DO_CALL (syscall_name, args);				\
    lea %s62, err;						\
    cmpu.l  %s47, %s62, %s0;                                    \
    brge.l.t 0, %s47, SYSCALL_ERROR_LABEL;			\


# undef	PSEUDO_END
# define PSEUDO_END(name)						      \
  SYSCALL_ERROR_HANDLER							      \
  END (name)

# undef	PSEUDO_NOERRNO
# define PSEUDO_NOERRNO(name, syscall_name, args) 			      \
  .section .data;                                                              \
        sys_##syscall_name: .int SYS_ify(syscall_name);                        \
  .text;                                                                      \
   ENTRY (name);								      \
    DO_CALL (syscall_name, args)

# undef	PSEUDO_END_NOERRNO
# define PSEUDO_END_NOERRNO(name) 					      \
  END (name)

# define ret_NOERRNO DO_RET

# undef	PSEUDO_ERRVAL
# define PSEUDO_ERRVAL(name, syscall_name, args) \
  .section .data                                                              \
        sys_##syscall_name: .int SYS_ify(syscall_name)                        \
  .text;                                                                      \
  ENTRY (name)                                                                \
    DO_CALL (syscall_name, args);					      \
    mulu.l %s0, -1, %s0

# undef	PSEUDO_END_ERRVAL
# define PSEUDO_END_ERRVAL(name) \
  END (name)


# define ret_ERRVAL ret
/* ld recognizes rtld_errno so access it directly
 * SYSCALL_SET_ERRNO sets the errno by multiplying the kernal returned errno by -1
 * In case of libc module or modules other than rtld, errno is a TLS variable */
# if defined PIC && defined RTLD_PRIVATE_ERRNO && IS_IN (rtld)
#  define SYSCALL_SET_ERRNO \
  lea	  %s59, rtld_errno@GOTOFF_LO;\
  and	  %s59, %s59, (32)0;		\
  lea.sl  %s59, rtld_errno@GOTOFF_HI(%got, %s59);	\
  mulu.l  %s0, -1, %s0;				\
  st	  %s0, (,%s59);
#else
#  if IS_IN (libc)
#  define SYSCALL_ERROR_ERRNO __libc_errno
#  else
#  define SYSCALL_ERROR_ERRNO errno
#  endif
/* Calculate errno for TLS variable errno or __libc_errno */
#  define SYSCALL_SET_ERRNO \
  lea	%sp, -176-50(,%sp); \
  st	%got, 184(,%sp);\
  st	%plt, 192(,%sp);\
  st	%lr, 200(,%sp);\
  st	%s18, 208(,%sp);\
  lea	%got,_GLOBAL_OFFSET_TABLE_@PC_LO(-24);\
  and	%got,%got,(32)0;\
  sic	%plt;\
  lea.sl  %got,_GLOBAL_OFFSET_TABLE_@PC_HI(%got,%plt);\
  or	  %s18, 0, %s0;   \
  lea	  %s0, SYSCALL_ERROR_ERRNO@TLS_GD_LO(-24); \
  and     %s0,%s0,(32)0;  \
  sic     %lr;    \
  lea.sl  %s0, SYSCALL_ERROR_ERRNO@TLS_GD_HI(%s0,%lr);\
  lea     %s12,__tls_get_addr@PLT_LO(8);        \
  and	  %s12, %s12,(32)0;       \
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

/* SYSCALL_ERROR_HANDLER sets the return value of syscall which is -1 generally */
# ifndef PIC
#  define SYSCALL_ERROR_HANDLER	/* Nothing here; code in sysdep.S is used.  */
# else
#  define SYSCALL_ERROR_HANDLER		\
0:						\
  SYSCALL_SET_ERRNO				\
  lea %s59, -1;					\
  xor %s0, %s0, %s0;				\
  or %s0, %s59, (0)1;				\
  beq.l.t 0, 0x0(,%lr);
# endif	/* PIC */

/* The veos expects the system call parameters in
   registers according to the following table:

    syscall number	s0
    arg 1		s1
    arg 2		s2
    arg 3		s3
    arg 4		s4
    arg 5		s5
    arg 6		s6

    The Linux kernel uses and destroys internally these registers:
    return address from
    syscall		rcx
    eflags from syscall	r11

    Normal function call, including calls to the system call stub
    functions in the libc, get the first six parameters passed in
    registers and the seventh parameter and later on the stack.  The
    register use is as follows:

     system call number	in the DO_CALL macro
     arg 1		s0
     arg 2		s1
     arg 3		s2
     arg 4		s3
     arg 5		s4
     arg 6		s5

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
	monc;

# define DOARGS_0 /* nothing */
# define DOARGS_1 				\
	or	%s1, %s0, %s0;			\
	shm.l	%s1, 0x8(%s46);			\
	DOARGS_0
# define DOARGS_2				\
        or      %s2, %s1, %s1;		        \
        shm.l   %s2, 0x10(%s46);                \
        DOARGS_1
# define DOARGS_3				\
        or      %s3, %s2, %s2;                  \
        shm.l   %s3, 0x18(%s46);                \
        DOARGS_2
# define DOARGS_4				\
        or      %s4, %s3, %s3;                  \
        shm.l   %s4, 0x20(%s46);                \
        DOARGS_3
# define DOARGS_5				\
        or      %s5, %s4, %s4;                  \
        shm.l   %s5, 0x28(%s46);                \
        DOARGS_4
# define DOARGS_6				\
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

# define INTERNAL_SYSCALL_NCS(name, err, nr, args...) \
  ({									      \
    unsigned long int resultvar;					      \
    asm volatile (							      \
    "\nxor %%s46, %%s46, %%s46; ld %%s46, %0(,%%tp)\n\t" : : "i"(SHM_OFFSET));\
    LOAD_SYS_NAME (name)						      \
    LOAD_ARGS_##nr (args)						      \
    LOAD_SHM_##nr							      \
    asm volatile (							      \
    "monc\n\t"								      \
    "or %0, 0, %%s0\n\t"					              \
    : "=r" (resultvar)                                                        \
    :                                                                         \
    :"%s0");						                      \
    (long int) resultvar; })

# undef INTERNAL_SYSCALL
# define INTERNAL_SYSCALL(name, err, nr, args...) \
  INTERNAL_SYSCALL_NCS (__NR_##name, err, nr, ##args)

# define INTERNAL_SYSCALL_NCS_TYPES(name, err, nr, args...) \
  ({									      \
    unsigned long int resultvar;					      \
    asm volatile (                                                            \
    "\nxor %%s46, %%s46, %%s46; ld %%s46, %0(,%%tp)\n\t" : : "i"(SHM_OFFSET));\
    LOAD_SYS_NAME (name)						      \
    LOAD_ARGS_TYPES_##nr (args)						      \
    LOAD_SHM_TYPES_##nr							      \
    asm volatile (							      \
    "monc\n\t"								      \
    "or %0, 0, %%s0\n\t"					              \
    : "=r" (resultvar)             					      \
    :                                                                         \
    :"%s0" );						                      \
    (long int) resultvar; })

# undef INTERNAL_SYSCALL_TYPES
# define INTERNAL_SYSCALL_TYPES(name, err, nr, args...) \
  INTERNAL_SYSCALL_NCS_TYPES (__NR_##name, err, nr, ##args)

# undef INTERNAL_SYSCALL_ERROR_P
# define INTERNAL_SYSCALL_ERROR_P(val, err) \
  ((unsigned long int) (long int) (val) >= -4095L)

# undef INTERNAL_SYSCALL_ERRNO
# define INTERNAL_SYSCALL_ERRNO(val, err)	(-(val))

# ifdef SHARED
#  define INLINE_VSYSCALL(name, nr, args...) \
  ({									      \
    __label__ out;							      \
    __label__ iserr;							      \
    INTERNAL_SYSCALL_DECL (sc_err);					      \
    long int sc_ret;							      \
									      \
    __typeof (__vdso_##name) vdsop = __vdso_##name;			      \
    PTR_DEMANGLE (vdsop);						      \
    if (vdsop != NULL)							      \
      {									      \
	sc_ret = vdsop (args);						      \
	if (!INTERNAL_SYSCALL_ERROR_P (sc_ret, sc_err))			      \
	  goto out;							      \
	if (INTERNAL_SYSCALL_ERRNO (sc_ret, sc_err) != ENOSYS)		      \
	  goto iserr;							      \
      }									      \
									      \
    sc_ret = INTERNAL_SYSCALL (name, sc_err, nr, ##args);		      \
    if (INTERNAL_SYSCALL_ERROR_P (sc_ret, sc_err))			      \
      {									      \
      iserr:								      \
	__set_errno (INTERNAL_SYSCALL_ERRNO (sc_ret, sc_err));		      \
	sc_ret = -1L;							      \
      }									      \
  out:									      \
    sc_ret;								      \
  })
#  define INTERNAL_VSYSCALL(name, err, nr, args...) \
  ({									      \
    __label__ out;							      \
    long int v_ret;							      \
									      \
    __typeof (__vdso_##name) vdsop = __vdso_##name;			      \
    PTR_DEMANGLE (vdsop);						      \
    if (vdsop != NULL)							      \
      {									      \
	v_ret = vdsop (args);						      \
	if (!INTERNAL_SYSCALL_ERROR_P (v_ret, err)			      \
	    || INTERNAL_SYSCALL_ERRNO (v_ret, err) != ENOSYS)		      \
	  goto out;							      \
      }									      \
    v_ret = INTERNAL_SYSCALL (name, err, nr, ##args);			      \
  out:									      \
    v_ret;								      \
  })

# else
#  define INLINE_VSYSCALL(name, nr, args...) \
  INLINE_SYSCALL (name, nr, ##args)
#  define INTERNAL_VSYSCALL(name, err, nr, args...) \
  INTERNAL_SYSCALL (name, err, nr, ##args)
# endif

# define LOAD_SYS_NAME(name)						   \
  long int register __arg0 asm("%s0") = (long int) name;					   \
  asm volatile("shm.l %0, 0x0(%%s46)\n\t"::"r"(__arg0));

# define LOAD_ARGS_0()
# define LOAD_SHM_0

# define LOAD_ARGS_TYPES_1(t1, a1)					   \
  t1 register __arg1 asm("%s1");				   \
  asm volatile("or %0,0,%1": "=r"(__arg1): "r"((t1) (a1)));                \
  LOAD_ARGS_0 ()

# define LOAD_SHM_TYPES_1						   \
  asm volatile("shm.l %0, 0x8(%%s46)\n\t"::"r"(__arg1));	           \
  LOAD_SHM_0

# define LOAD_ARGS_1(a1)						   \
  LOAD_ARGS_TYPES_1 (long int, a1)

# define LOAD_SHM_1							   \
  LOAD_SHM_TYPES_1

# define LOAD_ARGS_TYPES_2(t1, a1, t2, a2)				   \
  t2 register __arg2 asm("%s2");			           	   \
  asm volatile("or %0,0,%1": "=r"(__arg2): "r"((t2) (a2)));                \
  LOAD_ARGS_TYPES_1 (t1, a1)

# define LOAD_SHM_TYPES_2						   \
  asm volatile("shm.l %0, 0x10(%%s46)\n\t"::"r"(__arg2)); 	           \
  LOAD_SHM_TYPES_1

# define LOAD_ARGS_2(a1, a2)						   \
  LOAD_ARGS_TYPES_2 (long int, a1, long int, a2)

# define LOAD_SHM_2							   \
  LOAD_SHM_TYPES_2

# define LOAD_ARGS_TYPES_3(t1, a1, t2, a2, t3, a3)			   \
  t3 register __arg3 asm("%s3");				   	   \
  asm volatile("or %0,0,%1": "=r"(__arg3): "r"((t3) (a3)));                \
  LOAD_ARGS_TYPES_2 (t1, a1, t2, a2)

# define LOAD_SHM_TYPES_3						   \
  asm volatile("shm.l %0, 0x18(%%s46)\n\t"::"r"(__arg3));                   \
  LOAD_SHM_TYPES_2

# define LOAD_ARGS_3(a1, a2, a3)					   \
  LOAD_ARGS_TYPES_3 (long int, a1, long int, a2, long int, a3)

# define LOAD_SHM_3							   \
  LOAD_SHM_TYPES_3

# define LOAD_ARGS_TYPES_4(t1, a1, t2, a2, t3, a3, t4, a4)		   \
  t4 register __arg4 asm("%s4");           				   \
  asm volatile("or %0,0,%1": "=r"(__arg4): "r"((t4) (a4)));                \
  LOAD_ARGS_TYPES_3 (t1, a1, t2, a2, t3, a3)

# define LOAD_SHM_TYPES_4						   \
  asm volatile("shm.l %0, 0x20(%%s46)\n\t"::"r"(__arg4));                   \
  LOAD_SHM_TYPES_3

# define LOAD_ARGS_4(a1, a2, a3, a4)					   \
  LOAD_ARGS_TYPES_4 (long int, a1, long int, a2, long int, a3,		   \
		     long int, a4)
# define LOAD_SHM_4							   \
  LOAD_SHM_TYPES_4

# define LOAD_ARGS_TYPES_5(t1, a1, t2, a2, t3, a3, t4, a4, t5, a5)	   \
  t5 register __arg5 asm("%s5");				   \
  asm volatile("or %0,0,%1": "=r"(__arg5): "r"((t5) (a5)));                \
  LOAD_ARGS_TYPES_4 (t1, a1, t2, a2, t3, a3, t4, a4)

# define LOAD_SHM_TYPES_5						   \
  asm volatile("shm.l %0, 0x28(%%s46)\n\t"::"r"(__arg5)); 	           \
  LOAD_SHM_TYPES_4

# define LOAD_ARGS_5(a1, a2, a3, a4, a5)				   \
  LOAD_ARGS_TYPES_5 (long int, a1, long int, a2, long int, a3,		   \
		     long int, a4, long int, a5)
# define LOAD_SHM_5							   \
  LOAD_SHM_TYPES_5

# define LOAD_ARGS_TYPES_6(t1, a1, t2, a2, t3, a3, t4, a4, t5, a5, t6, a6) \
  t6 register __arg6 asm("%s6");					   \
  asm volatile("or %0,0,%1": "=r"(__arg6): "r"((t6) (a6)));		   \
  LOAD_ARGS_TYPES_5 (t1, a1, t2, a2, t3, a3, t4, a4, t5, a5)

# define LOAD_SHM_TYPES_6						   \
  asm volatile("shm.l %0, 0x30(%%s46)\n\t"::"r"(__arg6));	           \
  LOAD_SHM_TYPES_5

# define LOAD_ARGS_6(a1, a2, a3, a4, a5, a6)				   \
  LOAD_ARGS_TYPES_6 (long int, a1, long int, a2, long int, a3,		   \
		     long int, a4, long int, a5, long int, a6)

# define LOAD_SHM_6							   \
  LOAD_SHM_TYPES_6

#endif	/* __ASSEMBLER__ */


/* Pointer mangling support.  */
#if IS_IN (rtld)
/* We cannot use the thread descriptor because in ld.so we use setjmp
   earlier than the descriptor is initialized.  */
# ifdef __ASSEMBLER__

#  define PTR_MANGLE(reg)	xor     %s63, %s63, %s63			\
		lea     %s63, __pointer_chk_guard_local@got_lo			\
		and     %s63, %s63, (32)0			       		\
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
                lea     %s61, __pointer_chk_guard_local@CALL_LO                 \
                and     %s61, %s61, (32)0                                       \
                lea.sl  %s61, __pointer_chk_guard_local@CALL_HI(%s61,%s15)      \
                ld      %s60, (,%s61)                                           \
                xor     %reg, %s60, reg;
# else

#  define PTR_MANGLE(reg)	asm ("xor     %%s63, %%s63, %%s63\n"		\
		"lea     %%s63, __pointer_chk_guard_local@got_lo\n"		\
		"and     %%s63, %%s63, (%2)0\n"					\
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
		"lea     %%s61, __pointer_chk_guard_local@CALL_LO\n"		\
		"and	 %%s61, %%s61, (32)0\n"					\
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

#endif /* linux/ve/sysdep.h */
