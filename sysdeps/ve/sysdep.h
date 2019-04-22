/* Assembler macros for x86-64.
   Copyright (C) 2001-2015 Free Software Foundation, Inc.
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

#ifndef _VE_SYSDEP_H
#define _VE_SYSDEP_H 1

#include <sysdeps/generic/sysdep.h>

#ifdef	__ASSEMBLER__

/* Syntactic details of assembler.  */
#define DO_RET beq.l.t 0, 0x0(,%s10)

/* ELF uses byte-counts for .align, most others use log2 of count of bytes.  */
#define ALIGNARG(log2) 1<<log2
#define ASM_SIZE_DIRECTIVE(name) .size name,.-name;


/* Define an entry point visible from C.  */
#define	ENTRY(name)							      \
  .global C_SYMBOL_NAME(name);						      \
  .type C_SYMBOL_NAME(name),@function;					      \
  .balign ALIGNARG(8);							      \
  C_LABEL(name)								      /*\
  cfi_startproc;                                                               \
  CALL_MCOUNT*/

#undef	END
#define END(name)							      \
  /*cfi_endproc;                                                                \*/\
  ASM_SIZE_DIRECTIVE(name)

#define ENTRY_CHK(name) ENTRY (name)
#define END_CHK(name) END (name)

/* If compiled for profiling, call `mcount' at the start of each function.  */
#ifdef	PROF
/* The mcount code relies on a normal frame pointer being on the stack
   to locate our caller, so push one just for its benefit.  */
#define CALL_MCOUNT                                                          \
  JUMPTARGET(mcount);                                                        \
#else
#define CALL_MCOUNT		/* Do nothing.  */
#endif

/* Since C identifiers are not normally prefixed with an underscore
   on this system, the asm identifier `syscall_error' intrudes on the
   C name space.  Make sure we use an innocuous name.  */
#define	syscall_error	__syscall_error
#define mcount		_mcount

#define	PSEUDO(name, syscall_name, args)				      \
lose:									      \
  JUMPTARGET(syscall_error)						      \
  .globl syscall_error;							      \
  ENTRY (name)								      \
  DO_CALL (syscall_name, args);						      \
  blt.l.t 0, %s0, lose;

#undef	PSEUDO_END
#define	PSEUDO_END(name)						      \
  END (name)

#undef JUMPTARGET
#ifdef PIC
#define JUMPTARGET(name)						      \
	lea     %s12, name@plt_lo(-24);					      \
        and     %s12, %s12, (32)0;					      \
        sic     %s35;							      \
        lea.sl  %s12, name@plt_hi(%s12,%s35);			 	      \
        xor     %s35, %s35, %s35;					      \
        beq.l.t 0x0, 0x0(,%s12);
#else
#define JUMPTARGET(name)						      \
	lea     %s12, name@CALL_LO;						      \
        and     %s12, %s12,(32)0;					      \
	lea.sl  %s12, name@CALL_HI(,%s12);					      \
	beq.l.t 0x0, 0x0(,%s12);
#endif

/* Local label name for asm code. */
#ifndef L
/* ELF-like local names start with `.L'.  */
# define L(name)	.L##name
#endif

#define atom_text_section .section ".text.atom", "ax"

/* Long and pointer size in bytes.  */
//#define LP_SIZE	8

/* Instruction to operate on long and pointer.  */
#define LP_OP(insn) insn##q

/* Assembler address directive. */
#define ASM_ADDR .quad

/* Registers to hold long and pointer.  */
#define S0_LP	s0
#define S1_LP	s1
#define S2_LP	s2
#define S3_LP	s3
#define S4_LP	s4
#define S5_LP	s5
#define S6_LP	s6
#define S7_LP	s7
#define S8_LP	s45
#define S9_LP	s46
#define S10_LP	s47
#define S11_LP	s48
#define S12_LP	s49
#define S13_LP	s50
#define S14_LP	s51
#define S15_LP	s51

#else	/* __ASSEMBLER__ */

/* Long and pointer size in bytes.  */
//#define LP_SIZE "8"

/* Instruction to operate on long and pointer.  */
#define LP_OP(insn) #insn "q"

/* Assembler address directive. */
#define ASM_ADDR ".quad"

/* Registers to hold long and pointer.  */
#define S0_LP	"s0"
#define S1_LP	"s1"
#define S2_LP	"s2"
#define S3_LP	"s3"
#define S4_LP	"s4"
#define S5_LP	"s5"
#define S6_LP	"s6"
#define S7_LP	"s7"
#define S8_LP	"s45"
#define S9_LP	"s46"
#define S10_LP	"s47"
#define S11_LP	"s48"
#define S12_LP	"s49"
#define S13_LP	"s50"
#define S14_LP	"s51"
#define S15_LP	"s52"

#endif	/* __ASSEMBLER__ */

#endif	/* _VE_SYSDEP_H */
