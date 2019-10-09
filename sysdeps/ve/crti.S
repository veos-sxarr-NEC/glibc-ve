/* Special .init and .fini section support for VE architecture.
   Copyright (C) 2012-2015 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   In addition to the permissions in the GNU Lesser General Public
   License, the Free Software Foundation gives you unlimited
   permission to link the compiled version of this file with other
   programs, and to distribute those programs without any restriction
   coming from the use of this file. (The GNU Lesser General Public
   License restrictions do apply in other respects; for example, they
   cover modification of the file, and distribution when not linked
   into another program.)

   Note that people who make modified versions of this file are not
   obligated to grant this special exception for their modified
   versions; it is their choice whether to do so. The GNU Lesser
   General Public License gives permission to release a modified
   version without this exception; this exception also makes it
   possible to release a modified version which carries forward this
   exception.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

/* crti.S puts a function prologue at the beginning of the .init and
   .fini sections and defines global symbols for those addresses, so
   they can be called as functions.  The symbols _init and _fini are
   magic and cause the linker to emit DT_INIT and DT_FINI.  */

#include <libc-symbols.h>
#include <sysdep.h>

#ifndef PREINIT_FUNCTION
# define PREINIT_FUNCTION __gmon_start__
#endif

#ifndef PREINIT_FUNCTION_WEAK
# define PREINIT_FUNCTION_WEAK 1
#endif

#if PREINIT_FUNCTION_WEAK
	weak_extern (PREINIT_FUNCTION)
#else
	.hidden PREINIT_FUNCTION	
#endif

/* The purpose of this function is to write prologue and call the pre-defined */
/* init function(PREINIT_FUNCTION). */
/* If PREINIT_FUNCTION is defined then invoke it. */
/* _init function is embedded in executable & all shared lib other than ld,libc */
	.section .init,"ax",@progbits
	.balign 1 		/* init section is 1 byte aligned in VE */
	.globl _init
	.type _init, @function
_init:
	/* Maintain 16-byte stack alignment for called functions.  */
	st      %fp,0x0(,%sp)
        st      %lr,0x8(,%sp)
        st      %got,0x18(,%sp)
        st      %plt,0x20(,%sp)
        lea 	%got,_GLOBAL_OFFSET_TABLE_@PC_LO(-24)
        and 	%got,%got,(32)0
        sic 	%plt
        lea.sl 	%got,_GLOBAL_OFFSET_TABLE_@PC_HI(%plt,%got)
        or      %fp,0,%sp
        lea     %sp, -0xE0(,%sp)
        brge.l.t        %sp,%sl,.L5.EoPL
        or      %s62,0,%s0
        ld      %s61,0x18(,%tp)
	lea     %s63,0x13b
        shm.w   %s63,0x0(%s61)
        shm.w   %sl,0x8(%s61)
        shm.w   %sp,0x10(%s61)
        monc
        or      %s0,0,%s62
.L5.EoPL:
#if PREINIT_FUNCTION_WEAK

	xor     %s61, %s61, %s61
	lea     %s61, PREINIT_FUNCTION@GOT_LO
	and     %s61, %s61, (32)0
	lea.sl  %s61, PREINIT_FUNCTION@GOT_HI(%s61, %got)
	ld	%s61, (,%s61)

	and 	%s61, %s61, %s61 	/* to check address is zero or not */

	breq.w  0, %s61, .L4.2		/* if address is zero then do nothing */
	br.l    .L4.1
.L4.1:
	lea     %s12, PREINIT_FUNCTION@plt_lo(-24)
	and     %s12, %s12, (32)0
	sic     %s60
	lea.sl  %s12, PREINIT_FUNCTION@plt_hi(%s12,%s60)
	bsic    %lr, (%s12)
	
.L4.2:
#else
        lea     %s12, PREINIT_FUNCTION@plt_lo(-24)
        and     %s12, %s12, (32)0
        sic     %s60
        lea.sl  %s12, PREINIT_FUNCTION@plt_hi(%s12,%s60)
	bsic    %lr, (%s12)
#endif


/* Workaround for HW bug about SPU coherency. #229 */
/* This workaround requires to insert padding region of 256B */
/*  at the tail of TEXT segment. */
/* "__ve_hw_specific01" is a label to identify this workaround */
/*  is applied to the executable/shared object */
	.section        .rodata
	.globl  __ve_hw_specific01
	.type   __ve_hw_specific01,@object
	.hidden __ve_hw_specific01
__ve_hw_specific01:
	.zero   1024


/* Not much to do in this function, so as placeholder */
/* the prologue functionality is embedded. */
	.section .fini,"ax",@progbits
	.balign 1                /* fini section is 1 byte aligned in VE */
	.globl _fini
	.type _fini, @function
_fini:
	st      %fp,0x0(,%sp)
        st      %lr,0x8(,%sp)
        st      %got,0x18(,%sp)
        st      %plt,0x20(,%sp)
        or      %fp,0,%sp
        lea     %sp, -0xE0(,%sp)
        brge.l.t        %sp,%sl,.L5.EoP
        or      %s62,0,%s0
	ld      %s61,0x18(,%tp)
        lea     %s63,0x13b
        shm.w   %s63,0x0(%s61)
        shm.w   %sl,0x8(%s61)
        shm.w   %sp,0x10(%s61)
        monc
        or      %s0,0,%s62
.L5.EoP:
