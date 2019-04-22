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

#ifndef _SYS_USER_H
#define _SYS_USER_H	1
#include <stdint.h>
/* The whole purpose of this file is for GDB and GDB only.  Don't read
   too much into it.  Don't use it for anything other than GDB unless
   you know what you are doing.  */

typedef uint64_t reg_t;

#define SR_NUM    64               /* Number of Scalar Registers */
#define VR_NUM    64               /* Number of Vector Registers */
#define VE_MVL   256              /* MAX Length of Vector */

/* VE process user regs */
struct ve_user_regs
{
        /* Performance Counters */
        reg_t USRCC;                    /*     0x0 -     0x7 */
        reg_t PMC[16];                  /*     0x8 -    0x87 */
        uint8_t pad0[0x1000 - 0x88];    /*    0x88 -   0xFFF */
        /* Control Registers */
        reg_t PSW;                      /*  0x1000 -  0x1007 */
        reg_t EXS;                      /*  0x1008 -  0x100F */
        reg_t IC;                       /*  0x1010 -  0x1017 */
        reg_t ICE;                      /*  0x1018 -  0x101F */
        reg_t VIXR;                     /*  0x1020 -  0x1027 */
        reg_t VL;                       /*  0x1028 -  0x102F */
        reg_t SAR;                      /*  0x1030 -  0x1047 */
        reg_t PMMR;                     /*  0x1038 -  0x103F */
        reg_t PMCR[4];                  /*  0x1040 -  0x105F */
        uint8_t pad1[0x1400 - 0x1060];  /*  0x1060 -  0x13FF */
        /* Scalar Registers */
        reg_t SR[SR_NUM];               /*  0x1400 -  0x15FF */
        uint8_t pad2[0x1800 - 0x1600];  /*  0x1600 -  0x17FF */
};

/* VE process Vector regs */
struct ve_user_vregs
{
        /* Vector Mask Registers */
        reg_t VMR[16][4];               /*     0x0 -  0x200  */
        uint8_t pad3[0x3E800 - 0x200];  /*   0x200 - 0x3E800 */
        /* Vector Registers */
        reg_t VR[VR_NUM][VE_MVL];      /* 0x3E800 - 0x5E800 */
        uint8_t pad4[0x7E800 - 0x5E800];/* 0x5E800 - 0x7E800 */
};

struct ve_user
{
        struct ve_user_regs regs;      /*     0x0 - 0x1800  */
        struct ve_user_vregs vregs;    /*  0x1800 - 0x80000 */
};


#endif	/* _SYS_USER_H */
