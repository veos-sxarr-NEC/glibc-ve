/* Dump registers.
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

#include <sys/uio.h>
#include <_itoa.h>
#include <stdio.h>

/* We will print the register dump in this format:

  Performance Counters:
  USRCC:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  PMC0:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  PMC1:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  .
  .
  .
  PMC14:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX

  Control Registers:
  PSW:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  EXS:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  IC:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  ICE:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  VIXR:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  VL:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  SAR:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  PMMR:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  PMCR:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX

  Scalar Registers:
  S0:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  S1:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  .
  .
  .
  S2:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX

  Vector Mask Registers:
  VM0_0:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  .
  .
  .
  VM0_3:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  VM1_0:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  .
  .
  .
  VM1_3:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  .
  .
  .
  .
  .
  .
  VM15_1:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  .
  .
  VM15_3:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX

  Vector Registers:
  V0_0:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  V0_1:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  .
  .
  .
  V0_255:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  V1_0:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  .
  .
  .
  V1_255:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  .
  .
  .
  .
  .
  .
  .
  V64_0:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX
  .
  .
  V64_255:  XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX

 */

/*--------------- Following register structure would be dumped-----------------*/
#if 0
struct ve_thread_struct {
        /* Performance Counters */
        reg_t USRCC;                    /*     0x0 -     0x8 (8)*/
        reg_t PMC[15];                  /*     0x8 -    0x80 (8)*/
        uint8_t pad0[0x1000 - 0x80];    /*    0x80 -  0x1000 (1) For alignment, do not dump*/
        /* Control Registers */
        reg_t PSW;                      /*  0x1000 -  0x1008 (8)*/
        reg_t EXS;                      /*  0x1008 -  0x1010 (8)*/
        reg_t IC;                       /*  0x1010 -  0x1018 (8)*/
        reg_t ICE;                      /*  0x1018 -  0x1020 (8)*/
        reg_t VIXR;                     /*  0x1020 -  0x1028 (8)*/
        reg_t VL;                       /*  0x1028 -  0x1030 (8)*/
        reg_t SAR;                      /*  0x1030 -  0x1048 (8)*/
        reg_t PMMR;                     /*  0x1038 -  0x1040 (8)*/
        reg_t PMCR;                     /*  0x1040 -  0x1048 (8)*/
        uint8_t pad1[0x1400 - 0x1048];  /*  0x1048 -  0x1400 (1) For alignment, do not dump*/
        /* Scalar Registers */
        reg_t SR[SR_NUM];               /*  0x1400 -  0x1600 (64 * 8)*/
        uint8_t pad2[0x1800 - 0x1600];  /*  0x1600 -  0x1800 (1)*/
        /* Vector Mask Registers */
        reg_t VMR[16][4];               /*  0x1800 -  0x1A00 (16*4 * 8)*/
        uint8_t pad3[0x40000 - 0x1A00]; /*  0x1A00 - 0x40000 (1) For alignment, do not dump*/
        /* Vector Registers */
        reg_t VR[VR_NUM][VE_MVL];       /* 0x40000 - 0x60000 (64 * 256 * 8)*/
        uint8_t pad4[0x80000 - 0x60000];/* 0x60000 - 0x80000 (1) For alignment, do not dump*/
};
#endif
/*--------------------------------------------------------------------------------*/


static void
hexvalue (unsigned long int value, char *buf, size_t len)
{
  char *cp = _itoa_word (value, buf + len, 16, 0);
  while (cp > buf)
    *--cp = '0';
}

static void
register_dump (int fd, struct ucontext *ctx)
{
  char regs[93][64];
  char vmreg[64][64];
  char vreg[16384][64];
  struct iovec iov[16709];
  char str[64];
  volatile size_t nr = 0;
  volatile int i;
  int j = 1;
  int k;

#define ADD_STRING(str) \
  iov[nr].iov_base = (char *) str;					      \
  iov[nr].iov_len = strlen (str);					      \
  ++nr
#define ADD_MEM(str, len) \
  iov[nr].iov_base = str;						      \
  iov[nr].iov_len = len;						      \
  ++nr

  /* Generate strings of register contents.  */
  hexvalue (ctx->uc_mcontext.USRCC, regs[0], 64);
  for(i=0; i<15;i++) {
	hexvalue (ctx->uc_mcontext.PMC[i], regs[j], 64);
	j++;
  }

  /* Generate the output.  */
  ADD_STRING ("Register dump:\n\n  Performance Counters:\n  USRCC: ");
  ADD_MEM (regs[0], 64);
  j=1;
  for(i=0; i<15;i++) {
	sprintf(str, "\n  PMC%d:  ", i);
	ADD_STRING (str);
	ADD_MEM (regs[j], 64);
	j++;
  }

  /* Generate strings of register contents.  */
  hexvalue (ctx->uc_mcontext.PSW, regs[16], 64);
  hexvalue (ctx->uc_mcontext.EXS, regs[17], 64);
  hexvalue (ctx->uc_mcontext.IC, regs[18], 64);
  hexvalue (ctx->uc_mcontext.ICE, regs[19], 64);
  hexvalue (ctx->uc_mcontext.VIXR, regs[20], 64);
  hexvalue (ctx->uc_mcontext.VL, regs[21], 64);
  hexvalue (ctx->uc_mcontext.SAR, regs[22], 64);
  hexvalue (ctx->uc_mcontext.PMMR, regs[23], 64);
  hexvalue (ctx->uc_mcontext.PMCR[0], regs[24], 64);
  hexvalue (ctx->uc_mcontext.PMCR[1], regs[25], 64);
  hexvalue (ctx->uc_mcontext.PMCR[2], regs[26], 64);
  hexvalue (ctx->uc_mcontext.PMCR[3], regs[27], 64);

  /* Generate the output.  */
  ADD_STRING ("\n\n  Control Registers:\n  PSW: ");
  ADD_MEM (regs[16], 64);
  ADD_STRING ("\n  EXS: ");
  ADD_MEM (regs[17], 64);
  ADD_STRING ("\n  IC: ");
  ADD_MEM (regs[18], 64);
  ADD_STRING ("\n  ICE: ");
  ADD_MEM (regs[19], 64);
  ADD_STRING ("\n  VIXR: ");
  ADD_MEM (regs[20], 64);
  ADD_STRING ("\n  VL: ");
  ADD_MEM (regs[21], 64);
  ADD_STRING ("\n  SAR: ");
  ADD_MEM (regs[22], 64);
  ADD_STRING ("\n  PMMR: ");
  ADD_MEM (regs[23], 64);
  ADD_STRING ("\n  PMCR0: ");
  ADD_MEM (regs[24], 64);
  ADD_STRING ("\n  PMCR1: ");
  ADD_MEM (regs[25], 64);
  ADD_STRING ("\n  PMCR2: ");
  ADD_MEM (regs[26], 64);
  ADD_STRING ("\n  PMCR3: ");
  ADD_MEM (regs[27], 64);


  /* Generate strings of register contents.  */
  j=28;
  for(i=0; i<64;i++) {
        hexvalue (ctx->uc_mcontext.SR[i], regs[j], 64);
        j++;
  }

  /* Generate the output.  */
  ADD_STRING ("\n\n  Scalar Registers:");
  j=28;
  for(i=0; i<64;i++) {
        sprintf(str, "\n  S%d:  ", i);
        ADD_STRING (str);
        ADD_MEM (regs[j], 64);
	j++;
  }

  /* Generate strings of Vector Mask Registers contents.  */
  j=0;
  for(i=0; i<16;i++) {
	for(k=0;k<4;k++) {
		hexvalue (ctx->uc_mcontext.VMR[i][j], vmreg[j], 64);
		j++;
        }
  }

  /* Generate the output.  */
  ADD_STRING ("\n\n  Vector Mask Registers:");
  j=0;
  for(i=0; i<16;i++) {
	for(k=0;k<4;k++) {
		sprintf(str, "\n  VM%d_%d:  ", i, k);
		ADD_STRING (str);
		ADD_MEM (vmreg[j], 64);
		j++;
        }
  }

  /* Generate strings of Vector Registers contents.  */
  j=0;
  for(i=0; i<64;i++) {
        for(k=0;k<256;k++) {
                hexvalue (ctx->uc_mcontext.VR[i][j], vreg[j], 64);
                j++;
        }
  }

  /* Generate the output.  */
  ADD_STRING ("\n\n  Vector Registers:");
  j=0;
  for(i=0; i<64;i++) {
        for(k=0;k<256;k++) {
                sprintf(str, "\n  V%d_%d:  ", i, k);
                ADD_STRING (str);
                ADD_MEM (vreg[j], 64);
                j++;
        }
  }

  ADD_STRING ("\n");

  /* Write the stuff out.  */
  writev (fd, iov, nr);
}

#define REGISTER_DUMP register_dump (fd, ctx)
