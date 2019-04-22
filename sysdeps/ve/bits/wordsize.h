/* Changes by NEC Corporation for the VE port, 2017-2019 */
/* Determine the wordsize from the preprocessor defines.  */

# define __WORDSIZE	64

/* 64-bit system call interface.  */
# define __WORDSIZE_TIME64_COMPAT32     1
# define __SYSCALL_WORDSIZE		64
