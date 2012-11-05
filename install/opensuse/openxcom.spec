#
# spec file for package openxcom
#
# Copyright (c) 2012 Angelos Tzotsos <tzotsos@opensuse.org>
#
# This file and all modifications and additions to the openxcom
# package are licensed under the GPL license.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

# norootforbuild

Name:           openxcom
Version:        0.4.5
Release:        0
Group:          Amusements/Games/Strategy/Turn Based 
License:        GPL
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
URL:            http://openxcom.org/
Source0:        %{name}-%{version}.tar.gz
Summary:        Open source X-Com
BuildRequires:  cmake 
BuildRequires:  gcc-c++ 
BuildRequires:  gcc 
BuildRequires:  fdupes
BuildRequires:	pkg-config
BuildRequires:	timidity
BuildRequires:	libSDL-devel
BuildRequires:	libSDL_mixer-devel
BuildRequires:	libSDL_gfx-devel >= 2.0.21
BuildRequires:	yaml-cpp-devel

%description
Open source reimplementation of the original X-Com

%prep
%setup -q

%build
cd ..
mkdir temp
cd temp
cmake  -DBUILD_PACKAGE:BOOL=ON \
       -DDEV_BUILD:BOOL=OFF \
       -DCMAKE_INSTALL_PREFIX:PATH=/usr/local \
       -DCMAKE_BUILD_TYPE:STRING="Release" ../%{name}-%{version}/

make VERBOSE=1 %{?_smp_mflags}

%install
cd ../temp
make install DESTDIR=%{buildroot}

%fdupes %{buildroot}

%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%dir /usr/local/share/openxcom
/usr/local/share/openxcom/*
/usr/local/bin/openxcom

%changelog
