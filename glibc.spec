# Debug package giving build-id errors so
# currently removed
%define debug_package %{nil}
%define _unpackaged_files_terminate_build 0
Name:		glibc-ve
Version:	2.21
Release:	3%{?dist}
Group:		System/Libraries
Summary:	glibc library ported for VE
#Group:
License:	LGPLv2+ and LGPLv2+ with exceptions and GPLv2+
#URL:
Source0:	glibc-ve-2.21.tar.gz
Vendor:		NEC Corporation

%description
Standard glibc build and install with this spec file for VE arch.

%define	GCC /opt/nec/ve/bin/gcc
%define _prefix /opt/nec/ve/
%define __strip /opt/nec/ve/bin/nstrip
%define _glibc_src_name %name-%version
%define _glibc_etc_base /etc/opt/nec/ve/
%define _glibc_var_base /var/opt/nec/ve/
%global __debug_install_post /opt/nec/ve/libexec/find-debuginfo.sh %{nil}

%package devel
Summary: Development files for glibc
Group: Development/C
Requires: %name = %version-%release
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

%prep
%setup -q

%build
mkdir ../glibc_build
pushd ../glibc_build
../%{_glibc_src_name}/configure --exec-prefix=%{_prefix} --prefix=%{_prefix} --host=ve-unknown-linux-gnu --with-headers=%{_prefix}/include --enable-obsolete-rpc
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

#Moving bin and sbin files
mv $RPM_BUILD_ROOT%{_prefix}/bin $RPM_BUILD_ROOT%{_prefix}/glibc/bin
mv $RPM_BUILD_ROOT%{_prefix}/sbin $RPM_BUILD_ROOT%{_prefix}/glibc/sbin
#Moving libexec files
mv $RPM_BUILD_ROOT%{_prefix}/libexec $RPM_BUILD_ROOT%{_prefix}/glibc/libexec

#Moving var files
mv $RPM_BUILD_ROOT%{_prefix}/var/* $RPM_BUILD_ROOT%{_glibc_var_base}/glibc/

rm -f $RPM_BUILD_ROOT%{_prefix}share/info/dir
popd

#1445, To support shm_open() issue during static linking
pushd $RPM_BUILD_ROOT%{_prefix}/lib/
%{GCC} -r -nostdlib -o libpthread.o -Wl,--whole-archive ./libpthread.a
rm libpthread.a
%{_prefix}/bin/nar rcs libpthread.a libpthread.o
rm libpthread.o
popd

%files
%{_prefix}/lib/*
%{_prefix}/glibc/libexec/*
%{_prefix}/share/*
%{_glibc_etc_base}/*
%{_glibc_var_base}/glibc/*
%{_prefix}/glibc/sbin/ldconfig

%files devel
%{_prefix}/include/*

%files bins
%{_prefix}/glibc/bin/*
%{_prefix}/glibc/sbin/*
%exclude %{_prefix}/glibc/sbin/ldconfig
%exclude %{_prefix}/glibc/bin/ldd
%exclude %{_prefix}/glibc/bin/pldd

%clean
rm -rf ../glibc_build
