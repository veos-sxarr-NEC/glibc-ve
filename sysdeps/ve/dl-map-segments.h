/* Map in a shared object's segments.  VE version.
   Copyright (C) 1995-2020 Free Software Foundation, Inc.
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
/* Changes by NEC Corporation for the VE port, 2020 */

#include <dl-load.h>

/* This implementation assumes (as does the corresponding implementation
   of _dl_unmap_segments, in dl-unmap-segments.h) that shared objects
   are always laid out with all segments contiguous (or with gaps
   between them small enough that it's preferable to reserve all whole
   pages inside the gaps with PROT_NONE mappings rather than permitting
   other use of those parts of the address space).  */
#define PAGE_SIZE_2MB	0x200000
#define MAP_ADDR_64GB_SPACE	((uint64_t)1<<35)
#define MAP_ADDR_64GB_FIXED	((uint64_t)1<<36)

/* p_align is local to this module */
static size_t p_align;
static __always_inline const char *
_dl_map_segments (struct link_map *l, int fd,
                  const ElfW(Ehdr) *header, int type,
                  const struct loadcmd loadcmds[], size_t nloadcmds,
                  const size_t maplength, bool has_holes,
                  struct link_map *loader)
{
  /* #1410 */
  const uint64_t  flag = (((p_align == PAGE_SIZE_2MB) ? MAP_2MB : MAP_64MB)
						| MAP_ADDR_64GB_SPACE);
  const struct loadcmd *c = loadcmds;

  if (__glibc_likely (type == ET_DYN))
    {
      /* This is a position-independent shared object.  We can let the
         kernel map it anywhere it likes, but we must have space for all
         the segments in their specified positions relative to the first.
         So we map the first segment without MAP_FIXED, but with its
         extent increased to cover all the segments.  Then we remove
         access from excess portion, and there is known sufficient space
         there to remap from the later segments.

         As a refinement, sometimes we have an address that we would
         prefer to map such objects at; but this is only a preference,
         the OS can do whatever it likes. */
      ElfW(Addr) mappref
        = (ELF_PREFERRED_ADDRESS (loader, maplength,
                                  c->mapstart & GLRO(dl_use_load_bias))
           - MAP_BASE_ADDR (l));

      /* Remember which part of the address space this object uses.  */
      l->l_map_start = (ElfW(Addr)) __mmap_ve ((void *) mappref, maplength,
                                            c->prot,
                                            flag|MAP_COPY|MAP_FILE,
                                            fd, c->mapoff);
      if (__glibc_unlikely ((void *) l->l_map_start == MAP_FAILED))
        return DL_MAP_SEGMENTS_ERROR_MAP_SEGMENT;
      l->l_map_end = l->l_map_start + maplength;
      l->l_addr = l->l_map_start - c->mapstart;

      if (has_holes)
        /* Change protection on the excess portion to disallow all access;
           the portions we do not remap later will be inaccessible as if
           unallocated.  Then jump into the normal segment-mapping loop to
           handle the portion of the segment past the end of the file
           mapping.  */
        __mprotect ((caddr_t) (l->l_addr + c->mapend),
                    loadcmds[nloadcmds - 1].mapstart - c->mapend,
                    PROT_NONE);

      l->l_contiguous = 1;

      goto postmap;
    }

  /* Remember which part of the address space this object uses.  */
  l->l_map_start = c->mapstart + l->l_addr;
  l->l_map_end = l->l_map_start + maplength;
  l->l_contiguous = !has_holes;

  while (c < &loadcmds[nloadcmds])
    {
      if (c->mapend > c->mapstart
          /* Map the segment contents from the file.  */
          && (__mmap_ve ((void *) (l->l_addr + c->mapstart),
                      c->mapend - c->mapstart, c->prot,
                      flag|MAP_COPY|MAP_FILE|MAP_ADDR_64GB_FIXED,
                      fd, c->mapoff)
              == MAP_FAILED))
        return DL_MAP_SEGMENTS_ERROR_MAP_SEGMENT;

    postmap:
      _dl_postprocess_loadcmd (l, header, c);

      if (c->allocend > c->dataend)
        {
          /* Extra zero pages should appear at the end of this segment,
             after the data mapped from the file.   */
          ElfW(Addr) zero, zeroend, zeropage;

          zero = l->l_addr + c->dataend;
          zeroend = l->l_addr + c->allocend;
          zeropage = ((zero + p_align - 1)
                      & ~(p_align - 1));

          if (zeroend < zeropage)
            /* All the extra data is in the last page of the segment.
               We can just zero it.  */
            zeropage = zeroend;

          if (zeropage > zero)
            {
              /* Zero the final part of the last page of the segment.  */
              if (__glibc_unlikely ((c->prot & PROT_WRITE) == 0))
                {
                  /* Dag nab it.  */
                  if (__mprotect ((caddr_t) (zero
                                             & ~(p_align - 1)),
                                  p_align, c->prot|PROT_WRITE) < 0)
                    return DL_MAP_SEGMENTS_ERROR_MPROTECT;
                }
              memset ((void *) zero, '\0', zeropage - zero);
              if (__glibc_unlikely ((c->prot & PROT_WRITE) == 0))
                __mprotect ((caddr_t) (zero & ~(p_align - 1)),
                            p_align, c->prot);
            }

          if (zeroend > zeropage)
            {
              /* Map the remaining zero pages in from the zero fill FD.  */
              caddr_t mapat;
              mapat = __mmap_ve ((caddr_t) zeropage, zeroend - zeropage,
                              c->prot,flag|MAP_ANON|MAP_PRIVATE|
                              MAP_ADDR_64GB_FIXED, -1, 0);
              if (__glibc_unlikely (mapat == MAP_FAILED))
                return DL_MAP_SEGMENTS_ERROR_MAP_ZERO_FILL;
            }
        }

      ++c;
    }

  /* Notify ELF_PREFERRED_ADDRESS that we have to load this one
     fixed.  */
  ELF_FIXED_ADDRESS (loader, c->mapstart);

  return NULL;
}
