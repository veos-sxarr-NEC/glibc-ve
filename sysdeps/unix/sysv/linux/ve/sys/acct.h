/* Copyright (C) 1996-2020 Free Software Foundation, Inc.
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

/* Changes by NEC Corporation for the VE port, 2022 */

#ifndef _SYS_ACCT_H
#define _SYS_ACCT_H	1

#include <sys/types.h>
#include <stdint.h>
#include <bits/endian.h>
#include <bits/types/time_t.h>
#include <linux/types.h>

__BEGIN_DECLS

#define ACCT_COMM 16

/*
  comp_t is a 16-bit "floating" point number with a 3-bit base 8
  exponent and a 13-bit fraction. See linux/kernel/acct.c for the
  specific encoding system used.
*/

typedef uint16_t comp_t;

/* Accounting structure for v14 format */
struct ve_acct {
        char     ac_flag;                /*!< Flags */
        char     ac_version;             /*!< Always set to ACCT_VERSION */
        __u16    ac_tty;                 /*!< Control Terminal */
        __u32    ac_exitcode;            /*!< Exitcode */
        __u32    ac_uid;                 /*!< Real User ID */
        __u32    ac_gid;                 /*!< Real Group ID */
        __u32    ac_pid;                 /*!< Process ID */
        __u32    ac_ppid;                /*!< Parent Process ID */
        __u32    ac_btime;               /*!< Process Creation Time */
        float    ac_etime;               /*!< Elapsed Time */
        comp_t   ac_utime;               /*!< User Time */
        comp_t   ac_stime;               /*!< System Time */
        comp_t   ac_mem;                 /*!< Average Memory Usage */
        comp_t   ac_io;                  /*!< Chars Transferred */
        comp_t   ac_rw;                  /*!< Blocks Read or Written */
        comp_t   ac_minflt;              /*!< Minor Pagefaults */
        comp_t   ac_majflt;              /*!< Major Pagefaults */
        comp_t   ac_swaps;               /*!< Number of Swaps */
        char    ac_comm[ACCT_COMM];      /*!< Command Name */
        __u32   ac_sid;                  /* session ID */
        __u32   ac_timeslice;            /* timeslice [μs] */
        __u16   ac_max_nthread;          /* max number of thread */
        __u16   ac_numanode;             /* the number of NUMA node */
        double   ac_total_mem;           /* VE's total memory usage in clicks */
        __u64   ac_maxmem;               /* VE's max memory usage [kb] */
        __u64   ac_syscall;              /* the number of systemcall */
        double   ac_transdata;           /* data transfer amount between VE-VH [kb] */ 
        __u64   ac_ex;                   /* Execution count */
        __u64   ac_vx;                   /* Vector execution count */
        __u64   ac_fpec;                 /* Floating point data element count */
        __u64   ac_ve;                   /* Vector elements count */
        __u64   ac_l1lmc;                /* L1 instruction cache miss count */
        __u64   ac_vecc;                 /* Vector execution in microseconds */
        __u64   ac_l1mcc;                /* L1 cache miss in microseconds */
        __u64   ac_l2mcc;                /* L2 cache miss in microseconds */
        __u64   ac_ve2;                  /* Vector elements count 2 */
        __u64   ac_varec;                /* Vector arithmetic execution in microseconds */
        __u64   ac_l1lmcc;               /* L1 instruction cache miss in microseconds */
        __u64   ac_vldec;                /* Vector load execution in microseconds */
        __u64   ac_l1omcc;               /* L1 operand cache miss in microseconds */
        __u64   ac_pccc;                 /* Port conflict in microseconds */
        __u64   ac_ltrc;                 /* Load instruction traffic count */
        __u64   ac_vldcc;                /* Vector load delayed in microseconds */
        __u64   ac_strc;                 /* Store instruction traffic count */
        __u64   ac_vlec;                 /* Vector load element count */
        __u64   ac_vlcme;                /* Vector load cache miss element count */
        __u64   ac_vlcme2;               /* Vector load cache miss element count 2 */
        __u64   ac_fmaec;                /* Fused multiply add element count */
        __u64   ac_ptcc;                 /* Power throttling in microseconds */
        __u64   ac_ttcc;                 /* Thermal throttling in microseconds */
};

/* Accounting structure for v15 format */
struct ve_acct_v15 {
        char     ac_flag;                /*!< Flags */
        char     ac_version;             /*!< Always set to ACCT_VERSION */
        __u16    ac_tty;                 /*!< Control Terminal */
        __u32    ac_exitcode;            /*!< Exitcode */
        __u32    ac_uid;                 /*!< Real User ID */
        __u32    ac_gid;                 /*!< Real Group ID */
        __u32    ac_pid;                 /*!< Process ID */
        __u32    ac_ppid;                /*!< Parent Process ID */
        __u32    ac_btime;               /*!< Process Creation Time */
        float    ac_etime;               /*!< Elapsed Time */
        comp_t   ac_utime;               /*!< User Time */
        comp_t   ac_stime;               /*!< System Time */
        comp_t   ac_mem;                 /*!< Average Memory Usage */
        comp_t   ac_io;                  /*!< Chars Transferred */
        comp_t   ac_rw;                  /*!< Blocks Read or Written */
        comp_t   ac_minflt;              /*!< Minor Pagefaults */
        comp_t   ac_majflt;              /*!< Major Pagefaults */
        comp_t   ac_swaps;               /*!< Number of Swaps */
        char    ac_comm[ACCT_COMM];      /*!< Command Name */
        __u32   ac_sid;                  /* session ID */
        __u32   ac_timeslice;            /* timeslice [μs] */
        __u16   ac_max_nthread;          /* maximum number of threads whose state are  "RUNNING" or "WAIT"  at the same time*/
        __u16   ac_numanode;             /* the number of NUMA node */
        double   ac_total_mem;           /* VE's total memory usage in clicks */
        __u64   ac_maxmem;               /* VE's max memory usage [kb] */
        __u64   ac_syscall;              /* the number of systemcall */
        double   ac_transdata;           /* data transfer amount between VE-VH [kb] */
        __u64   ac_ex;                   /* Execution count */
        __u64   ac_vx;                   /* Vector execution count */
        __u64   ac_fpec;                 /* Floating point data element count */
        __u64   ac_ve;                   /* Vector elements count */
        __u64   ac_l1lmc;                /* L1 instruction cache miss count */
        __u64   ac_vecc;                 /* Vector execution in microseconds */              
        __u64   ac_l1mcc;                /* L1 cache miss in microseconds */
        __u64   ac_l2mcc;                /* L2 cache miss in microseconds */
        __u64   ac_ve2;                  /* Vector elements count 2 */
        __u64   ac_varec;                /* Vector arithmetic execution in microseconds */
        __u64   ac_l1lmcc;               /* L1 instruction cache miss in microseconds */
        __u64   ac_vldec;                /* Vector load execution in microseconds */
        __u64   ac_l1omcc;               /* L1 operand cache miss in microseconds */
        __u64   ac_pccc;                 /* Port conflict in microseconds */
        __u64   ac_ltrc;                 /* Load instruction traffic count */
        __u64   ac_vlpc;                 /* Vector load packet count */
        __u64   ac_strc;                 /* Store instruction traffic count */
        __u64   ac_vlec;                 /* Vector load element count */
        __u64   ac_vlcme;                /* Vector load cache miss element count */
        __u64   ac_vlcme2;               /* Vector load cache miss element count 2 */
        __u64   ac_fmaec;                /* Fused multiply add element count */
        __u64   ac_ptcc;                 /* Power throttling in microseconds */
        __u64   ac_ttcc;                 /* Thermal throttling in microseconds */
        __u64   ac_corebitmap;           /* core bitmap */
        __u64   padding_1[5];            /* PMC 16-20 */
        __u64   padding_2[5];            /* padding */
};

/* Accounting structure for v16 format */
struct ve_acct_v16 {
        char     ac_flag;                /*!< Flags */
        char     ac_version;             /*!< Always set to ACCT_VERSION */
        __u16    ac_tty;                 /*!< Control Terminal */
        __u32    ac_exitcode;            /*!< Exitcode */
        __u32    ac_uid;                 /*!< Real User ID */
        __u32    ac_gid;                 /*!< Real Group ID */
        __u32    ac_pid;                 /*!< Process ID */
        __u32    ac_ppid;                /*!< Parent Process ID */
        __u32    ac_btime;               /*!< Process Creation Time */
        float    ac_etime;               /*!< Elapsed Time */
        comp_t   ac_utime;               /*!< User Time */
        comp_t   ac_stime;               /*!< System Time */
        comp_t   ac_mem;                 /*!< Average Memory Usage */
        comp_t   ac_io;                  /*!< Chars Transferred */
        comp_t   ac_rw;                  /*!< Blocks Read or Written */
        comp_t   ac_minflt;              /*!< Minor Pagefaults */
        comp_t   ac_majflt;              /*!< Major Pagefaults */
        comp_t   ac_swaps;               /*!< Number of Swaps */
        char    ac_comm[ACCT_COMM];      /*!< Command Name */
        __u32   ac_sid;                  /* session ID */
        __u32   ac_timeslice;            /* timeslice [μs] */
        __u16 ac_max_nthread;            /* maximum number of threads whose state are  "RUNNING" or "WAIT"  at the same time */
        __u16   ac_numanode;             /* the number of NUMA node */
        double   ac_total_mem;           /* VE's total memory usage in clicks */
        __u64   ac_maxmem;               /* VE's max memory usage [kb] */
        __u64   ac_syscall;              /* the number of systemcall */
        double   ac_transdata;           /* data transfer amount between VE-VH [kb] */
        __u64   ac_ex;                   /* Execution count */
        __u64   ac_vx;                   /* Vector execution count */
        __u64   ac_fpec;                 /* Floating point data element count */
        __u64   ac_ve;                   /* Vector elements count */
        __u64   ac_l1lmc;                /* L1 instruction cache miss count */
        __u64   ac_vecc;                 /* Vector execution in microseconds */              
        __u64   ac_l1mcc;                /* L1 cache miss in microseconds */
        __u64   ac_l2mcc;                /* L2 cache miss in microseconds */
        __u64   ac_ve2;                  /* Vector elements count 2 */
        __u64   ac_varec;                /* Vector arithmetic execution in microseconds */
        __u64   ac_l1lmcc;               /* L1 instruction cache miss in microseconds */
        __u64   ac_vldec;                /* Vector load execution in microseconds */
        __u64   ac_l1omcc;               /* L1 operand cache miss in microseconds */
        __u64   ac_pccc;                 /* Port conflict in microseconds */
        __u64   ac_ltrc;                 /* Load instruction traffic count */
        __u64   ac_vlpc;                 /* Vector load packet count */
        __u64   ac_strc;                 /* Store instruction traffic count */
        __u64   ac_vlec;                 /* Vector load element count */
        __u64   ac_vlcme;                /* Vector load cache miss element count */
        __u64   ac_vlcme2;               /* Vector load cache miss element count 2 */
        __u64   ac_fmaec;                /* Fused multiply add element count */
        __u64   ac_ptcc;                 /* Power throttling in microseconds */
        __u64   ac_ttcc;                 /* Thermal throttling in microseconds */
        __u64   ac_corebitmap;           /* core bitmap */
        __u64   ac_l3vsac;               /* VLD+SLD elements accessing L3 count */
        __u64   ac_l3vsme;               /* L3 VLD+SLD miss-hit element count */
        __u64   ac_l3vsml;               /* L3 VLD+SLD miss-hit cache line count */
        __u64   ac_llvsme;               /* LLC miss-hit element count */
        __u64   ac_llvml;                /* LLC miss-hit cache line count */
        __u64   padding_2[5];            /* padding */
};

enum
  {
    AFORK = 0x01,		/* Has executed fork, but no exec.  */
    ASU = 0x02,			/* Used super-user privileges.  */
    ACORE = 0x08,		/* Dumped core.  */
    AXSIG = 0x10		/* Killed by a signal.  */
  };

#if __BYTE_ORDER == __BIG_ENDIAN
# define ACCT_BYTEORDER 0x80	/* Accounting file is big endian.  */
#else
# define ACCT_BYTEORDER 0x00	/* Accounting file is little endian.  */
#endif

#define AHZ     100


/* Switch process accounting on and off.  */
extern int acct (const char *__filename) __THROW;

__END_DECLS

#endif	/* sys/acct.h */
