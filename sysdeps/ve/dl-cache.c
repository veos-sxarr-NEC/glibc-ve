/* Support for reading /etc/ld.so.cache files written by Linux ldconfig.
   Copyright (C) 1996-2020 Free Software Foundation, Inc.
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

#include <assert.h>
#include <unistd.h>
#include <ldsodefs.h>
#include <sys/mman.h>
#include <dl-cache.h>
#include <dl-procinfo.h>
#include <stdint.h>
#include <_itoa.h>
#include <dl-hwcaps.h>

#ifndef _DL_PLATFORMS_COUNT
# define _DL_PLATFORMS_COUNT 0
#endif

#ifndef MIN_ELOOP_THRESHOLD
# define MIN_ELOOP_THRESHOLD    40
#endif


/* This is the starting address and the size of the mmap()ed file.  */
static struct cache_file *cache;
static struct cache_file_new *cache_new;
static size_t cachesize;


#define VE_EXCLUDE_PATH() \
{\
	char *ld_exclude_path = getenv("VE3_LD_LIBRARY_EXCLUDE_PATH");		  \
	/* Check VE3_LD_LIBRARY_EXCLUDE_PATH environment variable */		  \
	if(__glibc_unlikely (ld_exclude_path != NULL && *ld_exclude_path != '\0'))\
	  {									  \
	    char *lib_path_name=NULL;						  \
	    lib_path_name = strdupa(best);					  \
	    /*Check the searched cache library path is in the exclude path list or not*/\
	    ret = ve_exclude_search_path(lib_path_name, ld_exclude_path, ":");  \
	  }									  \
}


/* 1 if cache_data + PTR points into the cache.  */
#define _dl_cache_verify_ptr(ptr) (ptr < cache_data_size)

#define SEARCH_CACHE(cache) \
/* We use binary search since the table is sorted in the cache file.	      \
   The first matching entry in the table is returned.			      \
   It is important to use the same algorithm as used while generating	      \
   the cache file.  */							      \
do									      \
  {									      \
    left = 0;								      \
    right = cache->nlibs - 1;						      \
    bool ret=false ; 		              				      \
									      \
    while (left <= right)						      \
      {									      \
	__typeof__ (cache->libs[0].key) key;				      \
									      \
	middle = (left + right) / 2;					      \
									      \
	key = cache->libs[middle].key;					      \
									      \
	/* Make sure string table indices are not bogus before using	      \
	   them.  */							      \
	if (! _dl_cache_verify_ptr (key))				      \
	  {								      \
	    cmpres = 1;							      \
	    break;							      \
	  }								      \
									      \
	/* Actually compare the entry with the key.  */			      \
	cmpres = _dl_cache_libcmp (name, cache_data + key);		      \
	if (__glibc_unlikely (cmpres == 0))				      \
	  {								      \
	    /* Found it.  LEFT now marks the last entry for which we	      \
	       know the name is correct.  */				      \
	    left = middle;						      \
									      \
	    /* There might be entries with this name before the one we	      \
	       found.  So we have to find the beginning.  */		      \
	    while (middle > 0)						      \
	      {								      \
		__typeof__ (cache->libs[0].key) key;			      \
									      \
		key = cache->libs[middle - 1].key;			      \
		/* Make sure string table indices are not bogus before	      \
		   using them.  */					      \
		if (! _dl_cache_verify_ptr (key)			      \
		    /* Actually compare the entry.  */			      \
		    || _dl_cache_libcmp (name, cache_data + key) != 0)	      \
		  break;						      \
		--middle;						      \
	      }								      \
									      \
	    do								      \
	      {								      \
		int flags;						      \
		__typeof__ (cache->libs[0]) *lib = &cache->libs[middle];      \
									      \
		/* Only perform the name test if necessary.  */		      \
		if (middle > left					      \
		    /* We haven't seen this string so far.  Test whether the  \
		       index is ok and whether the name matches.  Otherwise   \
		       we are done.  */					      \
		    && (! _dl_cache_verify_ptr (lib->key)		      \
			|| (_dl_cache_libcmp (name, cache_data + lib->key)    \
			    != 0)))					      \
		  break;						      \
									      \
		flags = lib->flags & FLAG_TYPE_MASK;			      \
		if (_dl_cache_check_flags (flags)			      \
		    && _dl_cache_verify_ptr (lib->value))		      \
		  {							      \
		    if (best == NULL || flags == GLRO(dl_correct_cache_id))   \
		      {							      \
			HWCAP_CHECK;					      \
			best = cache_data + lib->value;			      \
									      \
			if (flags == GLRO(dl_correct_cache_id))		      \
			  {						      \
			  /* We've found an exact match for the shared	      \
			     object and no general `ELF' release.  Stop	      \
			     searching.  */				      \
			    VE_EXCLUDE_PATH();				      \
			    if (true == ret)				      \
			    best = NULL;				      \
			    if (false == ret)				      \
			    break;					      \
			  }						      \
		      }							      \
		  }							      \
	      }								      \
	    while (++middle <= right);					      \
	    break;							      \
	}								      \
									      \
	if (cmpres < 0)							      \
	  left = middle + 1;						      \
	else								      \
	  right = middle - 1;						      \
      }									      \
  }									      \
while (0)


int
_dl_cache_libcmp (const char *p1, const char *p2)
{
  while (*p1 != '\0')
    {
      if (*p1 >= '0' && *p1 <= '9')
        {
          if (*p2 >= '0' && *p2 <= '9')
            {
	      /* Must compare this numerically.  */
	      int val1;
	      int val2;

	      val1 = *p1++ - '0';
	      val2 = *p2++ - '0';
	      while (*p1 >= '0' && *p1 <= '9')
	        val1 = val1 * 10 + *p1++ - '0';
	      while (*p2 >= '0' && *p2 <= '9')
	        val2 = val2 * 10 + *p2++ - '0';
	      if (val1 != val2)
		return val1 - val2;
	    }
	  else
            return 1;
        }
      else if (*p2 >= '0' && *p2 <= '9')
        return -1;
      else if (*p1 != *p2)
        return *p1 - *p2;
      else
	{
	  ++p1;
	  ++p2;
	}
    }
  return *p1 - *p2;
}
static char *get_realpath(const char *,char *);


static void revstr(char * str)
{
  int l, i;
  char *begin_ptr, *end_ptr, ch;

  // Get the length of the string
  l = strlen(str);
  begin_ptr = str;
  end_ptr = str;

  // Move the end_ptr to the last character
  for (i = 0; i < l - 1; i++)
  end_ptr++;

  // Swap the char from start and end
  // index using begin_ptr and end_ptr
  for (i = 0; i < l / 2; i++)
    {
      // swap character
      ch = *end_ptr;
      *end_ptr = *begin_ptr;
      *begin_ptr = ch;
      // update pointers positions
      begin_ptr++;
      end_ptr--;
    }
}

/*  This function checks the searched library path into
 *  the VE3_LD_LIBRARY_EXCLUDE_PATH. If the searched
 *  library path is matched in VE3_LD_LIBRARY_EXCLUDE_PATH
 *  then returns true otherwise returns false.
 *  @param[in] Searched library path with name
 *  @param[in] VE3_LD_LIBRARY_EXCLUDE_PATH string
 *  @param[in] Seprator
*/
static bool
ve_exclude_search_path(char *lib_path_name, char *ld_exclude_path, const char*sep)
{
  char *result_path=NULL;	/* For allocating buffer */
  char *ep = NULL;		/* Raw exclude path */
  char *ld_exclude_path_dup = NULL;	/* For duplicate exclude path environment string */
  char real_ep[PATH_MAX] = {0};		/* Resolved exclude path */
  char *lib_path_name_dup = NULL;	/* For duplicate lib_path_name string */

  lib_path_name_dup = strdupa (lib_path_name);
  revstr(lib_path_name_dup); // Reverse the string
  /* This loop extract the path from lib_path_name*/
  while(*lib_path_name_dup!='\0')
    {
      if (*lib_path_name_dup == '/')
	{
	  lib_path_name_dup = lib_path_name_dup+1;
	  break;
	}
      *lib_path_name_dup++;
    }
  revstr(lib_path_name_dup); //Again Reverse the same string
  ld_exclude_path_dup  = strdupa (ld_exclude_path);
  result_path = (char *) malloc(PATH_MAX+1);
  if (result_path == NULL)
    {
      __set_errno (ENOMEM);
      return false;
    }
  while (( ep = __strsep (&ld_exclude_path_dup, sep)) != NULL)
    {
      get_realpath(ep,real_ep); /* Convert to realpath  */
      result_path[0]='\0';
      /* Copy the resolved real path into result_path buffer */
      strcpy(result_path,real_ep);
      /* Compare searched cache library path with ld_exclude_path */
      if(strcmp(lib_path_name_dup, result_path) == 0 )
	{
	 /* This is an exact match of searched library
	  * path with VE3_LD_LIBRARY_EXCLUDE_PATH */
	  free(result_path);
	  return true;
	}
  }
  free(result_path);
  return false;
}


static char *
get_realpath (const char *name, char *resolved)
{
  char *rpath, *dest, *extra_buf = NULL;
  const char *start, *end, *rpath_limit;
  long int path_max;
  int num_links = 0;

  if (name == NULL)
    {
      /* As per Single Unix Specification V2 we must return an error if
         either parameter is a null pointer.  We extend this to allow
         the RESOLVED parameter to be NULL in case the we are expected to
         allocate the room for the return value.  */
      __set_errno (EINVAL);
      return NULL;
    }

  if (name[0] == '\0')
    {
      /* As per Single Unix Specification V2 we must return an error if
         the name argument points to an empty string.  */
      __set_errno (ENOENT);
      return NULL;
    }

#ifdef PATH_MAX
  path_max = PATH_MAX;
#else
  path_max = __pathconf (name, _PC_PATH_MAX);
  if (path_max <= 0)
    path_max = 1024;
#endif

  if (resolved == NULL)
    {
      rpath = malloc (path_max);
      if (rpath == NULL)
        return NULL;
    }
  else
    rpath = resolved;
  rpath_limit = rpath + path_max;

  if (name[0] != '/')
    {
      if (!__getcwd (rpath, path_max))
        {
          rpath[0] = '\0';
          goto error;
        }
      dest = __rawmemchr (rpath, '\0');
    }
  else
    {
      rpath[0] = '/';
      dest = rpath + 1;
    }

  for (start = end = name; *start; start = end)
    {
      struct stat64 st;
      int n;

      /* Skip sequence of multiple path-separators.  */
      while (*start == '/')
        ++start;

      /* Find end of path component.  */
      for (end = start; *end && *end != '/'; ++end)
        /* Nothing.  */;

      if (end - start == 0)
        break;
      else if (end - start == 1 && start[0] == '.')
        /* nothing */;
      else if (end - start == 2 && start[0] == '.' && start[1] == '.')
        {
          /* Back up to previous component, ignore if at root already.  */
          if (dest > rpath + 1)
            while ((--dest)[-1] != '/');
        }
      else
        {
          size_t new_size;

          if (dest[-1] != '/')
            *dest++ = '/';

          if (dest + (end - start) >= rpath_limit)
            {
              ptrdiff_t dest_offset = dest - rpath;
              char *new_rpath;

              if (resolved)
                {
                  __set_errno (ENAMETOOLONG);
                  if (dest > rpath + 1)
                    dest--;
                  *dest = '\0';
                  goto error;
                }
              new_size = rpath_limit - rpath;
              if (end - start + 1 > path_max)
                new_size += end - start + 1;
              else
                new_size += path_max;
              new_rpath = (char *) realloc (rpath, new_size);
              if (new_rpath == NULL)
                goto error;
              rpath = new_rpath;
              rpath_limit = rpath + new_size;

              dest = rpath + dest_offset;
            }

          dest = __mempcpy (dest, start, end - start);
          *dest = '\0';

          if (__lxstat64 (_STAT_VER, rpath, &st) < 0)
            goto error;

          if (S_ISLNK (st.st_mode))
            {
              char *buf = __alloca (path_max);
              size_t len;

              if (++num_links > MIN_ELOOP_THRESHOLD)
                {
                  __set_errno (ELOOP);
                  goto error;
                }

              n = __readlink (rpath, buf, path_max - 1);
              if (n < 0)
                goto error;
              buf[n] = '\0';

              if (!extra_buf)
                extra_buf = __alloca (path_max);

              len = strlen (end);
              if (path_max - n <= len)
                {
                  __set_errno (ENAMETOOLONG);
                  goto error;
                }

              /* Careful here, end may be a pointer into extra_buf... */
              memmove (&extra_buf[n], end, len + 1);
              name = end = memcpy (extra_buf, buf, n);

              if (buf[0] == '/')
                dest = rpath + 1;       /* It's an absolute symlink */
              else
                /* Back up to previous component, ignore if at root already: */
                if (dest > rpath + 1)
                 while ((--dest)[-1] != '/');
            }
          else if (!S_ISDIR (st.st_mode) && *end != '\0')
            {
              __set_errno (ENOTDIR);
              goto error;
            }
        }
    }
  if (dest > rpath + 1 && dest[-1] == '/')
    --dest;
  *dest = '\0';

  assert (resolved == NULL || resolved == rpath);
  return rpath;

error:
  assert (resolved == NULL || resolved == rpath);
  if (resolved == NULL)
    free (rpath);
  return NULL;
}

/* Look up NAME in ld.so.cache and return the file name stored there, or null
   if none is found.  The cache is loaded if it was not already.  If loading
   the cache previously failed there will be no more attempts to load it.
   The caller is responsible for freeing the returned string.  The ld.so.cache
   may be unmapped at any time by a completing recursive dlopen and
   this function must take care that it does not return references to
   any data in the mapping.  */
char *
_dl_load_cache_lookup (const char *name)
{
  int left, right, middle;
  int cmpres;
  const char *cache_data;
  uint32_t cache_data_size;
  const char *best;

  /* Print a message if the loading of libs is traced.  */
  if (__glibc_unlikely (GLRO(dl_debug_mask) & DL_DEBUG_LIBS))
    _dl_debug_printf (" search cache=%s\n", LD_SO_CACHE);

  if (cache == NULL)
    {
      /* Read the contents of the file.  */
      void *file = _dl_sysdep_read_whole_file (LD_SO_CACHE, &cachesize,
					       PROT_READ);

      /* We can handle three different cache file formats here:
	 - the old libc5/glibc2.0/2.1 format
	 - the old format with the new format in it
	 - only the new format
	 The following checks if the cache contains any of these formats.  */
      if (file != MAP_FAILED && cachesize > sizeof *cache
	  && memcmp (file, CACHEMAGIC, sizeof CACHEMAGIC - 1) == 0
	  /* Check for corruption, avoiding overflow.  */
	  && ((cachesize - sizeof *cache) / sizeof (struct file_entry)
	      >= ((struct cache_file *) file)->nlibs))
	{
	  size_t offset;
	  /* Looks ok.  */
	  cache = file;

	  /* Check for new version.  */
	  offset = ALIGN_CACHE (sizeof (struct cache_file)
				+ cache->nlibs * sizeof (struct file_entry));

	  cache_new = (struct cache_file_new *) ((void *) cache + offset);
	  if (cachesize < (offset + sizeof (struct cache_file_new))
	      || memcmp (cache_new->magic, CACHEMAGIC_VERSION_NEW,
			 sizeof CACHEMAGIC_VERSION_NEW - 1) != 0)
	    cache_new = (void *) -1;
	}
      else if (file != MAP_FAILED && cachesize > sizeof *cache_new
	       && memcmp (file, CACHEMAGIC_VERSION_NEW,
			  sizeof CACHEMAGIC_VERSION_NEW - 1) == 0)
	{
	  cache_new = file;
	  cache = file;
	}
      else
	{
	  if (file != MAP_FAILED)
	    __munmap (file, cachesize);
	  cache = (void *) -1;
	}

      assert (cache != NULL);
    }

  if (cache == (void *) -1)
    /* Previously looked for the cache file and didn't find it.  */
    return NULL;

  best = NULL;

  if (cache_new != (void *) -1)
    {
      uint64_t platform;

      /* This is where the strings start.  */
      cache_data = (const char *) cache_new;

      /* Now we can compute how large the string table is.  */
      cache_data_size = (const char *) cache + cachesize - cache_data;

      platform = _dl_string_platform (GLRO(dl_platform));
      if (platform != (uint64_t) -1)
	platform = 1ULL << platform;

      uint64_t hwcap_mask = GET_HWCAP_MASK();

#define _DL_HWCAP_TLS_MASK (1LL << 63)
      uint64_t hwcap_exclude = ~((GLRO(dl_hwcap) & hwcap_mask)
				 | _DL_HWCAP_PLATFORM | _DL_HWCAP_TLS_MASK);

      /* Only accept hwcap if it's for the right platform.  */
#define HWCAP_CHECK \
      if (lib->hwcap & hwcap_exclude)					      \
	continue;							      \
      if (GLRO(dl_osversion) && lib->osversion > GLRO(dl_osversion))	      \
	continue;							      \
      if (_DL_PLATFORMS_COUNT						      \
	  && (lib->hwcap & _DL_HWCAP_PLATFORM) != 0			      \
	  && (lib->hwcap & _DL_HWCAP_PLATFORM) != platform)		      \
	continue
      SEARCH_CACHE (cache_new);
    }
  else
    {
      /* This is where the strings start.  */
      cache_data = (const char *) &cache->libs[cache->nlibs];

      /* Now we can compute how large the string table is.  */
      cache_data_size = (const char *) cache + cachesize - cache_data;

#undef HWCAP_CHECK
#define HWCAP_CHECK do {} while (0)
      SEARCH_CACHE (cache);
    }

  /* Print our result if wanted.  */
  if (__builtin_expect (GLRO(dl_debug_mask) & DL_DEBUG_LIBS, 0)
      && best != NULL)
    _dl_debug_printf ("  trying file=%s\n", best);

  if (best == NULL)
    return NULL;

  /* The double copy is *required* since malloc may be interposed
     and call dlopen itself whose completion would unmap the data
     we are accessing. Therefore we must make the copy of the
     mapping data without using malloc.  */
  char *temp;
  temp = alloca (strlen (best) + 1);
  strcpy (temp, best);
  return __strdup (temp);
}

#ifndef MAP_COPY
/* If the system does not support MAP_COPY we cannot leave the file open
   all the time since this would create problems when the file is replaced.
   Therefore we provide this function to close the file and open it again
   once needed.  */
void
_dl_unload_cache (void)
{
  if (cache != NULL && cache != (struct cache_file *) -1)
    {
      __munmap (cache, cachesize);
      cache = NULL;
    }
}
#endif
