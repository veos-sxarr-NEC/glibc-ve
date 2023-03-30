/*
   Copyright (C) 2020 NEC Corporation
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it
   and/or modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either version
   2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.
   */

/* We only need to save callee-saved registers plus frame pointer plus
 * link register and stack pointer.  */
#define JB_SRFP	0
#define JB_SRLR	1
#define JB_SRSP	2
#define JB_SR15	4
#define JB_SR16	5
#define JB_SR17	6
#define JB_SR18	7
#define JB_SR19	8
#define JB_SR20	9
#define JB_SR21	10
#define JB_SR22	11
#define JB_SR23	12
#define JB_SR24	13
#define JB_SR25	14
#define JB_SR26 15
#define JB_SR27 16
#define JB_SR28 17
#define JB_SR29 18
#define JB_SR30 19
#define JB_SR31 20
#define JB_SR32 21
#define JB_SR33 22
#define JB_SIZE (8*22)
