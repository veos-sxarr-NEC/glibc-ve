/* FPU control word bits.  VE version.
   Copyright (C) 1993-2015 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Olaf Flebbe.

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

#ifndef _FPU_CONTROL_H
#define _FPU_CONTROL_H	1

/* Note that this file sets on VE FPU only.  */

/* Here is the dirty part. In VE both control word and status word
 * are managed in PSW register.
 * For control word,
 *	When the mask bit is set to â€œ0â€ (mask is not enabled), the interrupt
 *	due to the corresponding exception is not raised and following
 *	instructions are executed.
 *
 *	When the mask bit is set to â€œ1â€(mask is enabled) and an non-masked
 *	arithmetic exception is detected, ICE saves the address of the
 *	instruction to cause the exception and core stop execution.
 *
 * Note: When multiple arithmetic exceptions at detected the same time and
 * they are not masked (i.e., mask is enabled), only one of them is saved
 * in EXS register.
 *
 * For status word,
 *	Here is the dirty part. When exception is raised, Exception flag is
 *	set to 1, regardless of the existence of an interrupt.
 *
 * Layout of FPCR and FPSR
 * <--MSB		<------ %PSW ------>			   ---LSB.
 *
 * 	47	49	51	53	55	57	59	61	63
 *	|       |       |       |       |       |       |	|	|
 *	0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
 *		    c	c   c   c   c   c   c   c   s   s   s   s   s   s
 *	     		    |	     PEM	|   |       PEF		|
 *	|   RSV |   |IRM|   ZM  OM  UF  XM  VM  IM  ZF  OF  UF  XF  VF  IF
 *
 * IM: Invalid operation mask
 * XM: Fixed-point overflow mask
 * ZM: Zero-divide mask
 * OM: Floating-point Overflow mask
 * UM: Floating-point Underflow mask
 * PM: Inexact exception mask
 * RSV: Reserved Bits.
 *
 * Mask bit is 0 means no interrupt.
 *
 * IF: Invalid operation flag
 * XF: Fixed-point overflow flag
 * ZF: Zero-divide flag
 * OF: Floating-point Overflow flag
 * UF: Floating-point Underflow flag
 * PF: Inexact exception flag
 *
 * IRM: Rounding modes
 * 00: Round toward Zero (RZ)
 * 01: Round toward Plus infinity (RP)
 * 10: Round toward Minus infinity (RM)
 * 11: Round to Nearest even (RN)
 *
 * The hardware default is 0x3000 which we use.
 */

#include <features.h>

/* masking of interrupts */
#define _FPU_MASK_ZM  0x800
#define _FPU_MASK_OM  0x400
#define _FPU_MASK_UM  0x200
#define _FPU_MASK_XM  0x100
#define _FPU_MASK_PM  0x80
#define _FPU_MASK_IM  0x40

/* rounding control */
#define _FPU_RC_ZERO    0x0
#define _FPU_RC_UP      0x1000
#define _FPU_RC_DOWN    0x2000
#define _FPU_RC_NEAREST 0x3000    /* RECOMMENDED */

#define _FPU_RESERVED 0xC000  /* Reserved bits in AM of %psw */

#define _FPU_DEFAULT  0x3000

/* IEEE:  same as above.  */
#define _FPU_IEEE     0x3000

/* Type of the control word.  */
typedef unsigned short int fpu_control_t;

/* Macros for accessing the hardware control word. */
#define _FPU_GETCW(cw) \
	__asm__ __volatile__ ("spm %%s1 \n\t" \
			"st2b %%s1, %0" :: "m" (*&cw) : "%s1")

#define _FPU_SETCW(cw) __asm__ __volatile__ ("lpm %0" :: "r" (cw))

/* Default control word set at startup.  */
extern fpu_control_t __fpu_control;

#endif	/* fpu_control.h */
