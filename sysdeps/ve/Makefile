long-double-fcts = yes

ifeq ($(subdir),gmon)
sysdep_routines += _mcount
endif

ifeq ($(subdir),malloc)
tests += tst-mallocalign1
endif

ifeq ($(subdir),string)
sysdep_routines += strcasecmp_l-nonascii strncase_l-nonascii
endif


ifeq ($(subdir),csu)
gen-as-const-headers += link-defines.sym
endif

ifeq ($(subdir),elf)
sysdep-dl-routines += dl-tlsdesc tlsdesc
sysdep_routines += dl-tlsdesc tlsdesc
sysdep-rtld-routines += dl-tlsdesc tlsdesc
CFLAGS-tst-quad1pie.c = -O0
CFLAGS-tst-quad2pie.c = -O0
tests += tst-quad1 tst-quad2
modules-names += tst-quadmod1 tst-quadmod2

$(objpfx)tst-quad1: $(objpfx)tst-quadmod1.so
$(objpfx)tst-quad2: $(objpfx)tst-quadmod2.so

quad-pie-test += tst-quad1pie tst-quad2pie
tests += $(quad-pie-test)
tests-pie += $(quad-pie-test)

$(objpfx)tst-quad1pie: $(objpfx)tst-quadmod1pie.o
$(objpfx)tst-quad2pie: $(objpfx)tst-quadmod2pie.o

# tst-audit3 file removed as it needs header emmintrin.h
# that is compiler header not present for ve.
# tst-auditmod3a removed because of dependency of tst-audit3
# tst-auditmod3b removed because of dependency of tst-audit3
# tst-auditmod3b file is also using emmintrin.h
# tst-auditmod3b removed because of dependency of tst-audit4
# Both files need emmintrin.h
# tst-audit4 and tst-audit5 removed needed emmintrin.h
# tst-audit6 removed needed emmintrin.h
# tst-audit10 removed same reason
#
tests +=
ifeq (yes,$(config-cflags-avx))
tests += tst-audit7
endif
modules-names +=

#$(objpfx)tst-audit3: $(objpfx)tst-auditmod3a.so
$(objpfx)tst-audit3.out: $(objpfx)tst-auditmod3b.so
tst-audit3-ENV = LD_AUDIT=$(objpfx)tst-auditmod3b.so

#$(objpfx)tst-audit4: $(objpfx)tst-auditmod4a.so
#$(objpfx)tst-audit4.out: $(objpfx)tst-auditmod4b.so
tst-audit4-ENV = LD_AUDIT=$(objpfx)tst-auditmod4b.so

#$(objpfx)tst-audit5: $(objpfx)tst-auditmod5a.so
#$(objpfx)tst-audit5.out: $(objpfx)tst-auditmod5b.so
tst-audit5-ENV = LD_AUDIT=$(objpfx)tst-auditmod5b.so

#$(objpfx)tst-audit6: $(objpfx)tst-auditmod6a.so
#$(objpfx)tst-audit6.out: $(objpfx)tst-auditmod6b.so \
			 $(objpfx)tst-auditmod6c.so
tst-audit6-ENV = LD_AUDIT=$(objpfx)tst-auditmod6b.so:$(objpfx)tst-auditmod6c.so

#$(objpfx)tst-audit7: $(objpfx)tst-auditmod7a.so
#$(objpfx)tst-audit7.out: $(objpfx)tst-auditmod7b.so
tst-audit7-ENV = LD_AUDIT=$(objpfx)tst-auditmod7b.so

#$(objpfx)tst-audit10: $(objpfx)tst-auditmod10a.so
#$(objpfx)tst-audit10.out: $(objpfx)tst-auditmod10b.so
tst-audit10-ENV = LD_AUDIT=$(objpfx)tst-auditmod10b.so
tst-audit12-ENV = LD_AUDIT=$(objpfx)tst-auditmod12.so
endif

ifeq ($(subdir),csu)
gen-as-const-headers += tlsdesc.sym
endif

ifeq ($(subdir),elf)
ldd-rewrite = -e 's%@RTLD@%$(rtlddir)/$(rtld-installed-name)%g' \
              -e 's%@VERSION@%$(version)%g' \
              -e 's|@PKGVERSION@|$(PKGVERSION)|g' \
              -e 's|@REPORT_BUGS_TO@|$(REPORT_BUGS_TO)|g' \
              -e 's%@BASH@%$(BASH)%g' \
              -e 's%@TEXTDOMAINDIR@%$(msgcatdir)%g'

ifeq ($(ldd-rewrite-script),no)
define gen-ldd
LC_ALL=C sed $(ldd-rewrite) < $< > $@.new
endef
else
define gen-ldd
LC_ALL=C sed $(ldd-rewrite) < $< \
| LC_ALL=C sed -f $(patsubst $(..)/%,/%,$(..)$(ldd-rewrite-script)) > $@.new
endef
endif

$(objpfx)ldd: ../sysdeps/ve/ldd.bash.in $(common-objpfx)soversions.mk \
	$(common-objpfx)config.make
	$(gen-ldd)
	chmod 555 $@.new
	mv -f $@.new $@

define ldd-wrapper-script
#!/bin/bash
# Copyright (c) 2020 NEC Corporation.
# All rights reserved.
#
# Description:
# This wrapper scipt is responsible to execute the different
# version of ldd binary provided by glibc-ve as per the VE_LINKER_VERSION
# environment variable value and VE3 ldd existance.

VE1_CMD_PATH="/opt/nec/ve/glibc/bin/ldd"
VE3_CMD_PATH="/opt/nec/ve3/glibc/bin/ldd"
ldd_arg="$$@"

if [[ -x "$$VE3_CMD_PATH" ]]; then
        exec $$VE3_CMD_PATH $$ldd_arg
else
        exec $$VE1_CMD_PATH $$ldd_arg
fi
endef

$(objpfx)ve-ldd: $(common-objpfx)config.make
	$(file >$@T,$(ldd-wrapper-script))
	chmod a+x $@T
	mv -f $@T $@

define ldconfig-wrapper-script
#!/bin/bash
# Copyright (c) 2020 NEC Corporation.
# All rights reserved.
#
# Description:
# This wrapper scipt is responsible to execute the different
# version of ldconfig provided by different version glibc-ve.

VE1_CMD_PATH="/opt/nec/ve/glibc/sbin/ldconfig"
VE3_CMD_PATH="/opt/nec/ve3/glibc/sbin/ldconfig"

ldconfig_arg="$$@"

arch=$$(find /sys/devices -iname ve_arch_class | head -1 | xargs cat)
if test  "$$arch" = "ve1"; then
  if [[ -x "$$VE1_CMD_PATH" ]]; then
        $$VE1_CMD_PATH $$ldconfig_arg
  fi
else
  if [[ -x "$$VE3_CMD_PATH" ]]; then
        $$VE3_CMD_PATH $$ldconfig_arg
  fi
fi
endef

$(objpfx)ve-ldconfig: $(common-objpfx)config.make
	$(file >$@T,$(ldconfig-wrapper-script))
	chmod a+x $@T
	mv -f $@T $@

endif
