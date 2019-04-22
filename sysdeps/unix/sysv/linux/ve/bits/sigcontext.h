/* Copyright (C) 2002-2015 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* Changes by NEC Corporation for the VE port, 2017-2019 */

#ifndef _BITS_SIGCONTEXT_H
#define _BITS_SIGCONTEXT_H  1

#include <stdint.h>

#if !defined _SIGNAL_H && !defined _SYS_UCONTEXT_H
# error "Never use <bits/sigcontext.h> directly; include <signal.h> instead."
#endif

/* Type for general register.  */
typedef uint64_t reg_t;

/* Number of general registers.  */
#define SREGS    64               /* Number of Scalar Registers */
#define VREGS    64               /* Number of Vector Registers */
#define VR_MVL   256              /* MAX Length of Vector */

/* Context to describe whole processor state.  */
struct sigcontext
{
        /* Performance Counters */
        reg_t USRCC;				/*     0x0 -    0x7 */
        reg_t PMC[16];				/*     0x8 -   0x87 */
        uint8_t pad0[0x1000 - 0x88];		/*    0x88 -  0xFFF */

        /* Control Registers */
        reg_t PSW;				/*  0x1000 -  0x1007 */
        reg_t EXS;				/*  0x1008 -  0x100F */
        reg_t IC;				/*  0x1010 -  0x1017 */
        reg_t ICE;				/*  0x1018 -  0x101F */
        reg_t VIXR;				/*  0x1020 -  0x1027 */
        reg_t VL;				/*  0x1028 -  0x102F */
        reg_t SAR;				/*  0x1030 -  0x1047 */
        reg_t PMMR;				/*  0x1038 -  0x103F */
        reg_t PMCR[4];				/*  0x1040 -  0x105F */
        uint8_t pad1[0x1400 - 0x1060];		/*  0x1060 -  0x13FF */

        /* Scalar Registers */
        reg_t SR[SREGS];			/*  0x1400 -  0x15FF */
        uint8_t pad2[0x1800 - 0x1600];		/*  0x1600 -  0x17FF */

        /* Vector Mask Registers */
        reg_t VMR[16][4];			/*  0x1800 -  0x19FF */
        uint8_t pad3[0x40000 - 0x1A00];		/*  0x1A00 -  0x3FFFF */

        /* Vector Registers */
        reg_t VR[VREGS][VR_MVL];		/* 0x40000 - 0x5FFFF */
        uint8_t pad4[0x80000 - 0x60000];	/* 0x60000 - 0x7FFFF */
};


#endif /* _BITS_SIGCONTEXT_H */
