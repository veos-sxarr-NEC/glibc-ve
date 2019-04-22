/* Startup code compliant to the ELF VE ABI.
   Copyright (C) 2001-2015 Free Software Foundation, Inc.
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

/* This is the canonical entry point, usually the first thing in the text
   segment.  The VE ABI says that when the entry
   point runs, most registers' values are unspecified, except for:

   %sp		The stack contains the arguments and environment:
		0(%rsp)				argc
		LP_SIZE(%rsp)			argv[0]
		...
		(LP_SIZE*argc)(%rsp)		NULL
		(LP_SIZE*(argc+1))(%rsp)	envp[0]
		...
						NULL
*/

#include <sysdep.h>
ENTRY (_start)

	or	%s5, 0, %s2		/* Address of the shared library */
					/* termination function. */
	/* Clear the frame pointer.  The ABI suggests this be done, to mark
	   the outermost frame obviously.  */
	xor     %fp, %fp, %fp

	lea     %sp, -176-80(,%sp)
	st	%s5, 176+64(, %sp)
	lea     %s12, __setPthreadStruct@CALL_LO
	and     %s12, %s12, (32)0
	lea.sl  %s12, __setPthreadStruct@CALL_HI(,%s12)
	bsic    %lr, (,%s12)
	ld	%s5, 176+64(, %sp)
	lea     %sp, 176+80(,%sp)

	/* Extract the arguments as encoded on the stack and set up
	   the arguments for __libc_start_main (int (*main) (int, char **, char **),
		   int argc, char *argv,
		   void (*init) (void), void (*fini) (void),
		   void (*rtld_fini) (void), void *stack_end).
	   The arguments are passed via registers and on the stack:
	main:		%s0
	argc:		%s1
	argv:		%s2
	init:		%s3
	fini:		%s4
	rtld_fini:	%s5
	stack_end:	stack.	*/


	ld      %s1, 0x0(%sp)	/* get the argument count, 2nd arg  */
	lea     %s2, 0x8(%sp)   /* get the argv address, 3rd arg    */

	/* Pass address of our own entry points to __libc_csu_init and __libc_csu_fini */
	lea     %s3, __libc_csu_init@CALL_LO
	and     %s3, %s3,(32)0
	lea.sl  %s3, __libc_csu_init@CALL_HI(,%s3)

	lea     %s4, __libc_csu_fini@CALL_LO
	and     %s4, %s4,(32)0
	lea.sl  %s4, __libc_csu_fini@CALL_HI(,%s4)

	/* Pass address of our own main() */
	lea     %s0, main@CALL_LO
	and     %s0, %s0,(32)0
	lea.sl  %s0, main@CALL_HI(,%s0)

	lea     %sp, -176-80(,%sp)

/* make %sp aligned to 16 bytes */
        and     %sp, %sp, (60)1

/* set the arguments */
        st      %s0, 0xB0(,%sp)
        st      %s1, 0xC0(,%sp)
        st      %s2, 0xC8(,%sp)
        st      %s3, 0xD0(,%sp)
        st      %s4, 0xE0(,%sp)
        st      %s5, 0xF0(,%sp)

/* Call the user's main function, and exit with its value.
           But let the libc call main.    */
	lea     %s12, __libc_start_main@CALL_LO
	and     %s12, %s12,(32)0
	lea.sl  %s12, __libc_start_main@CALL_HI(,%s12)

# call __libc_start_main
	bsic    %lr, (%s12)

/* currently hlt is not supported in ncc */
#hlt                     /* Crash if somehow `exit' does return. */
END (_start)

/* Define a symbol for the first piece of initialized data.  */
#.comm data_start,8,8
	.data
	.globl	__data_start
	.balign	8
	.type	__data_start,@object
	.size   __data_start,8
__data_start:
	.long 0

	.weak data_start
	data_start = __data_start
