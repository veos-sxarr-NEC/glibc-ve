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

#include <linux/types.h>
#include <linux/socket.h>

struct atalk_addr {
        __be16  s_net;
        __u8    s_node;
};

struct sockaddr_at {
        __kernel_sa_family_t sat_family;
        __u8              sat_port;
        struct atalk_addr sat_addr;
        char              sat_zero[8];
};
