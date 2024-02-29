%define MainVersion    2.31

Name:		glibc-ve3
Version:	2.31
Release:	14%{?dist}
Group:		System/Libraries
Summary:	glibc library ported for VE
License:	LGPLv2+ and LGPLv2+ with exceptions and GPLv2+
Source0:	glibc-ve3-2.31.tar.gz
Vendor:		NEC Corporation
AutoReq:	no
BuildRequires:	kheaders-ve3
Requires:       glibc-ve-wrapper

%description
Standard glibc build and install with this spec file for VE arch.

#added debuginfo support #1577
%global __debug_package 1
%define	GCC /opt/nec/ve3/gcc/bin/ve-gcc
%define _prefix /opt/nec/ve3/
%define _ve1_prefix /opt/nec/ve/
%define __strip /opt/nec/ve/bin/nstrip
%define _glibc_src_name %name-%{MainVersion}
%define _glibc_etc_base /etc/opt/nec/ve3/
%define _glibc_var_base /var/opt/nec/ve3/
%if 0%{?rhel} == 8
%global __debug_install_post /opt/nec/ve/libexec/find-debuginfo.sh --unique-debug-suffix "-%{VERSION}-%{RELEASE}.%{_arch}" -S debugsourcefiles.list %{_builddir}/%{buildsubdir} %{nil}
%else
%global __debug_install_post /opt/nec/ve/libexec/find-debuginfo.sh %{nil}
%endif

%define _unpackaged_files_terminate_build 0

%package devel
Summary: Development files for glibc
Group: Development/C
Requires: %name = %version-%release
Requires: kheaders-ve3
Provides: %{_prefix}include/*

%description devel
Development headers for %name.

#Track ticket #1365
%package bins
Summary: Binary files for glibc
Requires: %name = %version-%release
Provides: %{_prefix}glibc/bin/*
Provides: %{_prefix}glibc/sbin/*

%description bins
Binary files for %name

#Track ticket #2224
%package -n glibc-ve-wrapper
Version: 3.1.0
Release: 1%{?dist}
Summary: Wrapper scripts for glibc
Provides: %{_ve1_prefix}/bin/*
Provides: %{_ve1_prefix}/sbin/*

%description -n glibc-ve-wrapper
Wrapper scripts for both glibc-ve1 and glibc-ve3

%prep
%setup -q

%build
mkdir ../glibc_build
pushd ../glibc_build
../%{_glibc_src_name}/configure --exec-prefix=%{_prefix} --prefix=%{_prefix} --host=ve3-unknown-linux-gnu --with-headers=%{_prefix}/include --enable-obsolete-rpc CC=%{GCC} LDFLAGS="-Wl,--build-id"
make -j 4
popd

%install
pushd ../glibc_build
install -m 0755 -d %{buildroot}/%{_prefix}
make DESTDIR=%{buildroot} install

#creating glibc folders for packaging
mkdir $RPM_BUILD_ROOT%{_prefix}/glibc
mkdir -p $RPM_BUILD_ROOT%{_glibc_etc_base}/
mkdir -p $RPM_BUILD_ROOT%{_glibc_var_base}/glibc

# Include ld.so.conf
echo 'include ld.so.conf.d/*.conf' > $RPM_BUILD_ROOT/etc/%{_prefix}/ld.so.conf
truncate -s 0 $RPM_BUILD_ROOT/etc/%{_prefix}/ld.so.cache
chmod 644 $RPM_BUILD_ROOT/etc/%{_prefix}/ld.so.conf
mkdir -p $RPM_BUILD_ROOT/etc/%{_prefix}/ld.so.conf.d

#remove libthread_db
rm -f $RPM_BUILD_ROOT%{_prefix}/lib/libthread_db*

#removing the debug information from startup file(crt)
%{__strip} -g $RPM_BUILD_ROOT%{_prefix}/lib/*.o

#Moving bin and sbin files
mv $RPM_BUILD_ROOT%{_prefix}/bin $RPM_BUILD_ROOT%{_prefix}/glibc/bin
mv $RPM_BUILD_ROOT%{_prefix}/sbin $RPM_BUILD_ROOT%{_prefix}/glibc/sbin
#Moving libexec files
mv $RPM_BUILD_ROOT%{_prefix}/libexec $RPM_BUILD_ROOT%{_prefix}/glibc/libexec

#Moving ldd & ldconfig wrapper files
mkdir -p $RPM_BUILD_ROOT%{_ve1_prefix}/bin
mkdir -p $RPM_BUILD_ROOT%{_ve1_prefix}/sbin
mv $RPM_BUILD_ROOT%{_prefix}/glibc/bin/ve-ldd $RPM_BUILD_ROOT%{_ve1_prefix}/bin/
mv $RPM_BUILD_ROOT%{_prefix}/glibc/sbin/ve-ldconfig $RPM_BUILD_ROOT%{_ve1_prefix}/sbin/

#Moving var files
mv $RPM_BUILD_ROOT%{_prefix}/var/* $RPM_BUILD_ROOT%{_glibc_var_base}/glibc/

rm -f $RPM_BUILD_ROOT%{_prefix}share/info/dir
popd

#1445, To support shm_open() issue during static linking
pushd $RPM_BUILD_ROOT%{_prefix}/lib/
%{GCC} -r -nostdlib -o libpthread.o -Wl,--whole-archive ./libpthread.a
rm libpthread.a
%{_ve1_prefix}/bin/nar rcs libpthread.a libpthread.o
rm libpthread.o
popd

#1682, setlocale issue
%post
if [ ! -L %{_prefix}/lib/locale ] ; then #1820 - If Link exists then do not create link again.
  ln -s /usr/lib/locale %{_prefix}/lib/locale
fi


#1682, setlocale issue link remove
%postun
if [ $1 -le 0 ]; then # rpm -e option
  if [ -L %{_prefix}/lib/locale ] ; then #1820 - If link exists then only unlink.
    unlink %{_prefix}/lib/locale
  fi
fi

%files
%{_prefix}/lib/*
%{_prefix}/glibc/libexec/*
%{_ve1_prefix}/veos/glibc-ve3/share/*
%{_glibc_etc_base}/*
%{_glibc_var_base}/glibc/*
%{_prefix}/glibc/sbin/ldconfig
%exclude %{_ve1_prefix}/sbin/ve-ldconfig
%{_prefix}/glibc/bin/ldd
%exclude %{_ve1_prefix}/bin/ve-ldd
%exclude %{_prefix}lib/debug

%files devel
%{_prefix}/include/*

%files bins
%{_prefix}/glibc/bin/*
%{_prefix}/glibc/sbin/*
%exclude %{_prefix}/glibc/sbin/ldconfig
%exclude %{_ve1_prefix}/sbin/ve-ldconfig
%exclude %{_prefix}/glibc/bin/pldd
%exclude %{_prefix}/glibc/bin/ldd
%exclude %{_ve1_prefix}/bin/ve-ldd

%files -n glibc-ve-wrapper
%{_ve1_prefix}/bin/*
%{_ve1_prefix}/sbin/*

%clean
rm -rf ../glibc_build
