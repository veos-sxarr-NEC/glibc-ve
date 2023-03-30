/* Machine-dependent ELF dynamic relocation inline functions.  VE version.
   Copyright (C) 2001-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Andreas Jaeger <aj@suse.de>.

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

#ifndef dl_machine_h
#define dl_machine_h

#define ELF_MACHINE_NAME "VE"

#include <sys/param.h>
#include <sysdep.h>
#include <tls.h>
#include <dl-tlsdesc.h>
#include <assert.h>

/* Return nonzero iff ELF header is compatible with the running host.  */
static inline int __attribute__ ((unused))
elf_machine_matches_host (const ElfW(Ehdr) *ehdr)
{
/* Processor identification in ELF header's e_machine member with value 0x3776*/
 return ehdr->e_machine == EM_VE;
}


/* Return the link-time address of _DYNAMIC.  Conveniently, this is the
   first element of the GOT.  This must be inlined in a function which
   uses global data.  */
static inline ElfW(Addr) __attribute__ ((unused))
elf_machine_dynamic (void)
{
  /* This produces an IP-relative reloc which is resolved at link time. */
  extern const ElfW(Addr) _GLOBAL_OFFSET_TABLE_[] attribute_hidden;
  return _GLOBAL_OFFSET_TABLE_[0];
}


/* Return the run-time load address of the shared object.  */
static inline ElfW(Addr) __attribute__ ((unused))
elf_machine_load_address (void)
{
  /* Compute the difference between the runtime address of _DYNAMIC as seen
     by an IP-relative reference, and the link-time address found in the
     special unrelocated first GOT entry.  */
  extern ElfW(Dyn) _DYNAMIC[] attribute_hidden;
  return (ElfW(Addr)) &_DYNAMIC - elf_machine_dynamic ();
}

/* Set up the loaded object described by L so its unrelocated PLT
   entries will jump to the on-demand fixup code in dl-runtime.c.  */

static inline int __attribute__ ((unused, always_inline))
elf_machine_runtime_setup (struct link_map *l, int lazy, int profile)
{
  Elf64_Addr *got;
  extern void _dl_runtime_resolve (ElfW(Word)) attribute_hidden;
  extern void _dl_runtime_profile (ElfW(Word)) attribute_hidden;

  if (l->l_info[DT_JMPREL] && lazy)
    {
      /* The GOT entries for functions in the PLT have not yet been filled
	 in.  Their initial contents will arrange when called to push an
	 offset into the .rel.plt section, push _GLOBAL_OFFSET_TABLE_[1],
	 and then jump to _GLOBAL_OFFSET_TABLE_[2].  */
      got = (Elf64_Addr *) D_PTR (l, l_info[DT_PLTGOT]);
      /* If a library is prelinked but we have to relocate anyway,
	 we have to be able to undo the prelinking of .got.plt.
	 The prelinker saved us here address of .plt + 0x16.  */
      if (got[1])
	{
	  l->l_mach.plt = got[1] + l->l_addr;
	  l->l_mach.gotplt = (ElfW(Addr)) &got[3];
	}
      /* Identify this shared object.  */
	/* For VE, the link_map addr has to be stored in got[2] */
      *(ElfW(Addr) *) (got + 2) = (ElfW(Addr)) l;

      /* The got[2] entry contains the address of a function which gets
	 called to get the address of a so far unresolved function and
	 jump to it.  The profiling extension of the dynamic linker allows
	 to intercept the calls to collect information.  In this case we
	 don't store the address in the GOT so that all future calls also
	 end in this function.  */
      if (__glibc_unlikely (profile))
	{
	  *(ElfW(Addr) *) (got + 2) = (ElfW(Addr)) &_dl_runtime_profile;

	  if (GLRO(dl_profile) != NULL
	      && _dl_name_match_p (GLRO(dl_profile), l))
	    /* This is the object we are looking for.  Say that we really
	       want profiling and the timers are started.  */
	    GL(dl_profile_map) = l;
	}
      else
	/* This function will get called to fix up the GOT entry indicated by
	   the offset on the stack, and then jump to the resolved address.  */
	/* For VE, the _dl_runtime_resolve addr has to be stored in got[1] */
	*(ElfW(Addr) *) (got + 1) = (ElfW(Addr)) &_dl_runtime_resolve;
    }

  if (l->l_info[ADDRIDX (DT_TLSDESC_GOT)] && lazy)
    *(ElfW(Addr)*)(D_PTR (l, l_info[ADDRIDX (DT_TLSDESC_GOT)]) + l->l_addr)
      = (ElfW(Addr)) &_dl_tlsdesc_resolve_rela;

  return lazy;
}

/* Initial entry point code for the dynamic linker.
   The C function `_dl_start' is the real entry point;
   its return value is the user program's entry point.  */
#define RTLD_START asm ("\n\
.text\n\
	.balign 16\n\
.globl _start\n\
.globl _dl_start_user\n\
_start:\n\
	or 	%s0, 0, %sp\n\
	lea     %s11, -176-80(,%s11)\n\
	ld	%s56,  0x0(0,%s0)\n\
	lea     %s12, _dl_start@PC_LO(-24)\n\
	and     %s12, %s12,(32)0\n\
	sic     %s62\n\
	lea.sl  %s12, _dl_start@PC_HI(%s12,%s62)\n\
	bsic    %lr, (%s12)\n\
	lea     %sp, 176+80(,%sp)\n\
	# After jumping to dl_start, we return to dl_start_user \n\
_dl_start_user:\n\
	# Save the user entry point address in %s33.\n\
	or 	%s33, 0, %s0\n\
	# Call _dl_init (struct link_map *main_map, int argc, char **argv, char **env)\n\
	# Pop argument count: argc in %s1.\n\
	ld      %s1, 0x0(0,%sp)\n\
	# Pop argument addr: argv in %s2.\n\
	lea	%s2, 0x8(0,%sp)\n\
	xor	%s59, %s59 , %s59\n\
	xor	%s60, %s60 , %s60\n\
	\n\
	# Pop Environment var: env in %s3.\n\
	mulu.l	%s59, 8 , %s1\n\
	addu.l	%s60, %s59, %s2\n\
	lea	%s3, 0x8(,%s60)\n\
	\n\
	# Fetch the link_map object of main_map in %s0.\n\
	lea     %s55,  _rtld_local@PC_LO(-24)\n\
        and     %s55, %s55,(32)0\n\
        sic     %s35\n\
        lea.sl  %s55,  _rtld_local@PC_HI(%s55, %s35)\n\
	ld	%s0, 0x0(, %s55) \n\
	\n\
	# Now %s0 -> main/link map, %s1 -> argc, %s2 -> argv, %s3 ->envn\n\
        xor     %s35, %s35, %s35\n\
	xor	%fp, %fp, %fp\n\
	# Call the function to run the initializers.\n\
	lea     %s11, -176-32(,%s11)\n\
	lea     %s12, _dl_init@PC_LO(-24)\n\
        and     %s12, %s12,(32)0\n\
	sic     %s62\n\
	lea.sl  %s12, _dl_init@PC_HI(%s12,%s62)\n\
        bsic    %lr, (%s12)\n\
	lea     %s11, 176+32(,%s11)\n\
	\n\
	# Pass our finalizer function to the user in %s2, as per ELF ABI.\n\
	lea     %s2, _dl_fini@PC_LO(-24)\n\
        and     %s2, %s2, (32)0\n\
        sic     %s35\n\
        lea.sl  %s2, _dl_fini@PC_HI(%s2,%s35)\n\
        xor     %s35, %s35, %s35\n\
	# Jump to the user's entry point.\n\
	b.l     (,%s33)\n\
	.previous\n\
");

/* ELF_RTYPE_CLASS_PLT iff TYPE describes relocation of a PLT entry or
   TLS variable, so undefined references should not be allowed to
   define the value.
   ELF_RTYPE_CLASS_NOCOPY iff TYPE should not be allowed to resolve to one
   of the main executable's symbols, as for a COPY reloc.  */
#define elf_machine_type_class(type)					      \
  ((((type) == R_VE_JUMP_SLOT					      \
     || (type) == R_VE_DTPMOD64					      \
     || (type) == R_VE_DTPOFF64					      \
     || (type) == R_VE_TPOFF64)					      \
    * ELF_RTYPE_CLASS_PLT)						      \
   | (((type) == R_VE_COPY) * ELF_RTYPE_CLASS_COPY))

/* A reloc type used for ld.so cmdline arg lookups to reject PLT entries.  */
#define ELF_MACHINE_JMP_SLOT	R_VE_JUMP_SLOT

/* The relative ifunc relocation.  */
// XXX This is a work-around for a broken linker.  Remove!
#define ELF_MACHINE_IRELATIVE	R_VE_RELATIVE

/* The x86-64 never uses Elf64_Rel/Elf32_Rel relocations.  */
#define ELF_MACHINE_NO_REL 1
#define ELF_MACHINE_NO_RELA 0

/* We define an initialization function.  This is called very early in
   _dl_sysdep_start.  */
#define DL_PLATFORM_INIT dl_platform_init ()

static inline void __attribute__ ((unused))
dl_platform_init (void)
{
  if (GLRO(dl_platform) != NULL && *GLRO(dl_platform) == '\0')
    /* Avoid an empty string which would disturb us.  */
    GLRO(dl_platform) = NULL;
}

static inline ElfW(Addr)
elf_machine_fixup_plt (struct link_map *map, lookup_t t,
			const ElfW(Sym) *refsym, const ElfW(Sym) *sym,
		       const ElfW(Rela) *reloc,
		       ElfW(Addr) *reloc_addr, ElfW(Addr) value)
{
  return *reloc_addr = value;
}

/* Return the final value of a PLT relocation.  On x86-64 the
   JUMP_SLOT relocation ignores the addend.  */
static inline ElfW(Addr)
elf_machine_plt_value (struct link_map *map, const ElfW(Rela) *reloc,
		       ElfW(Addr) value)
{
  return value;
}


/* Names of the architecture-specific auditing callback functions.  */
/* _TODO_PORT_HCLT: Define these functions */
#define ARCH_LA_PLTENTER
#define ARCH_LA_PLTEXIT

#endif /* !dl_machine_h */

#ifdef RESOLVE_MAP

/* Perform the relocation specified by RELOC and SYM (which is fully resolved).
   MAP is the object containing the reloc.  */

auto inline void
__attribute__ ((always_inline))
elf_machine_rela (struct link_map *map, const ElfW(Rela) *reloc,
		  const ElfW(Sym) *sym, const struct r_found_version *version,
		  void *const reloc_addr_arg, int skip_ifunc)
{
  ElfW(Addr) *const reloc_addr = reloc_addr_arg;
  const unsigned long int r_type = ELFW(R_TYPE) (reloc->r_info);

# if !defined RTLD_BOOTSTRAP || !defined HAVE_Z_COMBRELOC
  if (__glibc_unlikely (r_type == R_VE_RELATIVE))
    {
#  if !defined RTLD_BOOTSTRAP && !defined HAVE_Z_COMBRELOC
      /* This is defined in rtld.c, but nowhere in the static libc.a;
	 make the reference weak so static programs can still link.
	 This declaration cannot be done when compiling rtld.c
	 (i.e. #ifdef RTLD_BOOTSTRAP) because rtld.c contains the
	 common defn for _dl_rtld_map, which is incompatible with a
	 weak decl in the same file.  */
#   ifndef SHARED
      weak_extern (GL(dl_rtld_map));
#   endif
      if (map != &GL(dl_rtld_map)) /* Already done in rtld itself.  */
#  endif
	*reloc_addr = map->l_addr + reloc->r_addend;
    }
  else
# endif
# if !defined RTLD_BOOTSTRAP
  /* l_addr + r_addend may be > 0xffffffff and R_X86_64_RELATIVE64
     relocation updates the whole 64-bit entry.  */
  if (__glibc_unlikely (r_type == R_VE_RELATIVE))
    *(Elf64_Addr *) reloc_addr = (Elf64_Addr) map->l_addr + reloc->r_addend;
  else
# endif
  if (__glibc_unlikely (r_type == R_VE_NONE))
    return;
  else
    {
      struct link_map *sym_map = RESOLVE_MAP (&sym, version, r_type);
      ElfW(Addr) value = SYMBOL_ADDRESS (sym_map, sym, true);

      if (sym != NULL
	  && __builtin_expect (ELFW(ST_TYPE) (sym->st_info) == STT_GNU_IFUNC,
			       0)
	  && __builtin_expect (sym->st_shndx != SHN_UNDEF, 1)
	  && __builtin_expect (!skip_ifunc, 1))
	value = ((ElfW(Addr) (*) (void)) value) ();

      switch (r_type)
	{
	case R_VE_GLOB_DAT:
	case R_VE_JUMP_SLOT:
	*reloc_addr = value;
		break;
	case R_VE_REFLONG:
	case R_VE_REFQUAD:
	*(Elf64_Addr *) reloc_addr = ((Elf64_Addr) value + reloc->r_addend);
		break;
	case R_VE_SREL32:
	*(Elf64_Addr *) reloc_addr = ((Elf64_Addr) value + reloc->r_addend) - (ElfW(Addr)) reloc_addr;
		break;

# ifndef RESOLVE_CONFLICT_FIND_MAP
	case R_VE_DTPMOD64:
#  ifdef RTLD_BOOTSTRAP
	  /* During startup the dynamic linker is always the module
	     with index 1.
	     XXX If this relocation is necessary move before RESOLVE
	     call.  */
	  *reloc_addr = 1;
#  else
	  /* Get the information from the link map returned by the
	     resolve function.  */

	  if (sym_map != NULL) {
#ifndef SHARED
		  CHECK_STATIC_TLS (map, sym_map);
#endif
		  *reloc_addr = sym_map->l_tls_modid;
	  }
#  endif
	  break;
	case R_VE_DTPOFF64:
#  ifndef RTLD_BOOTSTRAP
	  /* During relocation all TLS symbols are defined and used.
	     Therefore the offset is already correct.  */
	  if (sym != NULL)
	    {
	      value = sym->st_value + reloc->r_addend;
#   ifdef __ILP32__
	      /* This relocation type computes a signed offset that is
		 usually negative.  The symbol and addend values are 32
		 bits but the GOT entry is 64 bits wide and the whole
		 64-bit entry is used as a signed quantity, so we need
		 to sign-extend the computed value to 64 bits.  */
	      *(Elf64_Sxword *) reloc_addr = (Elf64_Sxword) (Elf32_Sword) value;
#   else
	      *reloc_addr = value;
#   endif
	    }
#  endif
	  break;
	case R_VE_TPOFF64:
	  /* The offset is negative, forward from the thread pointer.  */
#  ifndef RTLD_BOOTSTRAP
	  if (sym != NULL)
#  endif
	    {
#  ifndef RTLD_BOOTSTRAP
	      CHECK_STATIC_TLS (map, sym_map);
#  endif
	      /* We know the offset of the object the symbol is contained in.
		 It is a negative value which will be added to the
		 thread pointer.  */
	      value = (sym->st_value + reloc->r_addend
		       - sym_map->l_tls_offset);
#  ifdef __ILP32__
	      /* The symbol and addend values are 32 bits but the GOT
		 entry is 64 bits wide and the whole 64-bit entry is used
		 as a signed quantity, so we need to sign-extend the
		 computed value to 64 bits.  */
	      *(Elf64_Sxword *) reloc_addr = (Elf64_Sxword) (Elf32_Sword) value;
#  else
	      *reloc_addr = value;
#  endif
	    }
	  break;
#endif

# ifndef RTLD_BOOTSTRAP
	case R_VE_HI32:
	case R_VE_CALL_HI32:
	  *(Elf32_Addr *) reloc_addr = ((Elf64_Addr) value + reloc->r_addend) >> 32;
	  break;
	case R_VE_LO32:
	case R_VE_CALL_LO32:
	  *(Elf32_Addr *) reloc_addr = ((Elf64_Addr) value + reloc->r_addend) & 0xffffffff;
	  break;
#  ifndef RESOLVE_CONFLICT_FIND_MAP
	  /* Not needed for dl-conflict.c.  */
	case R_VE_PC_HI32:
	 value += reloc->r_addend - (ElfW(Addr)) reloc_addr;
	 value = value >> 32;
	 *(unsigned int *) reloc_addr = value;
	const char *fmt;
	const char *strtab;
          if (__glibc_unlikely (value != (int) value))
            {
              fmt = "\
%s: Symbol `%s' causes overflow in R_VE_PC_HI32 relocation\n";
      const ElfW(Sym) *const refsym = sym;
	strtab = (const char *) D_PTR (map, l_info[DT_STRTAB]);
	_dl_error_printf(fmt, RTLD_PROGNAME, strtab + refsym->st_name);
            } 
	  break;
	  case R_VE_PC_LO32:
	   value += reloc->r_addend - (ElfW(Addr)) reloc_addr;
           value = value & 0xffffffff;
           *(unsigned int *) reloc_addr = value;
           if (__glibc_unlikely (value != (int) value))
           {
		fmt = "\
%s: Symbol `%s' causes overflow in R_VE_PC_LO32 relocation\n";
      const ElfW(Sym) *const refsym = sym;
	strtab = (const char *) D_PTR (map, l_info[DT_STRTAB]);
	_dl_error_printf(fmt, RTLD_PROGNAME, strtab + refsym->st_name);
           }
          break;
	case R_VE_COPY:
	  if (sym == NULL)
	    /* This can happen in trace mode if an object could not be
	       found.  */
	    break;
      	const ElfW(Sym) *const refsym = sym;
	  memcpy (reloc_addr_arg, (void *) value,
		  MIN (sym->st_size, refsym->st_size));
	  if (__builtin_expect (sym->st_size > refsym->st_size, 0)
	      || (__builtin_expect (sym->st_size < refsym->st_size, 0)
		  && GLRO(dl_verbose)))
	    {
	      fmt = "\
%s: Symbol `%s' has different size in shared object, consider re-linking\n";
	strtab = (const char *) D_PTR (map, l_info[DT_STRTAB]);
	_dl_error_printf(fmt, RTLD_PROGNAME, strtab + refsym->st_name);
	    }
	  break;
#  endif
	case R_VE_RELATIVE: 
	value = map->l_addr + reloc->r_addend;
	*reloc_addr = value;
		break;
	default:
	  _dl_reloc_bad_type (map, r_type, 0);
	  break;
# endif
	}
    }
}

auto inline void
__attribute ((always_inline))
elf_machine_rela_relative (ElfW(Addr) l_addr, const ElfW(Rela) *reloc,
			   void *const reloc_addr_arg)
{
  ElfW(Addr) *const reloc_addr = reloc_addr_arg;
#if !defined RTLD_BOOTSTRAP
  /* l_addr + r_addend may be > 0xffffffff and R_X86_64_RELATIVE64
     relocation updates the whole 64-bit entry.  */
  /*R_X86_64_RELATIVE64 not defined in VE, using R_VE_RELATIVE */
  if (__glibc_unlikely (ELFW(R_TYPE) (reloc->r_info) == R_VE_RELATIVE))
    *(Elf64_Addr *) reloc_addr = (Elf64_Addr) l_addr + reloc->r_addend;
  else
#endif
    {
      assert (ELFW(R_TYPE) (reloc->r_info) == R_VE_RELATIVE);
      *reloc_addr = l_addr + reloc->r_addend;
    }
}

auto inline void __attribute ((always_inline))
elf_machine_lazy_rel (struct link_map *map,
		      ElfW(Addr) l_addr, const ElfW(Rela) *reloc,
		      int skip_ifunc)
{
  ElfW(Addr) *const reloc_addr = (void *) (l_addr + reloc->r_offset);
  const unsigned long int r_type = ELFW(R_TYPE) (reloc->r_info);

  /* Check for unexpected PLT reloc type.  */
  if (__glibc_likely (r_type == R_VE_JUMP_SLOT))
    {
      if (__builtin_expect (map->l_mach.plt, 0) == 0)
	*reloc_addr += l_addr;
      else
	*reloc_addr =
	  map->l_mach.plt
	  + (((ElfW(Addr)) reloc_addr) - map->l_mach.gotplt) * 2;
    }
   /*
  else if (__glibc_likely (r_type == R_X86_64_TLSDESC)) //TODO: not defined in VE 
    {
      struct tlsdesc volatile * __attribute__((__unused__)) td =
	(struct tlsdesc volatile *)reloc_addr;

      td->arg = (void*)reloc;
      td->entry = (void*)(D_PTR (map, l_info[ADDRIDX (DT_TLSDESC_PLT)])
			  + map->l_addr);
    }
    */
  else if (__glibc_unlikely (r_type == R_VE_RELATIVE))  
    {
      ElfW(Addr) value = map->l_addr + reloc->r_addend;
      if (__glibc_likely (!skip_ifunc))
	value = ((ElfW(Addr) (*) (void)) value) ();
      *reloc_addr = value;
    }
  else
    _dl_reloc_bad_type (map, r_type, 1);
}

#endif /* RESOLVE_MAP */
