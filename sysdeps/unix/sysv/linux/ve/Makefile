# We don't need any header files.
abi-includes :=

abi-64-options := -DVE
abi-64-condition := defined __ve__

ifeq ($(subdir),misc)
sysdep_routines += ioperm iopl
sysdep_headers += sys/perm.h
endif

ifeq ($(subdir),stdlib)
gen-as-const-headers += ucontext_i.sym
sysdep_routines += __start_context
endif

ifeq ($(subdir),elf)
sysdep_routines += dl-vdso
sysdep-dl-routines += dl-static
sysdep_routines += $(sysdep-dl-routines)
sysdep-rtld-routines += $(sysdep-dl-routines)
endif

ifeq (io,$(subdir))
sysdep_routines += io_hook
endif

sysdep_headers += io_hook.h
