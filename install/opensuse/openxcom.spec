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
Version:        1.0.0
Release:        0
Group:          Amusements/Games/Strategy/Turn Based
License:        GPL-3.0
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
URL:            https://openxcom.org/
Source0:        %{name}-%{version}.tar.gz
Summary:        Open-source clone of UFO: Enemy Unknown
BuildRequires:  cmake
BuildRequires:  gcc-c++
BuildRequires:  gcc
BuildRequires:  fdupes
BuildRequires:	pkg-config
BuildRequires:	libSDL-devel
BuildRequires:	libSDL_mixer-devel
BuildRequires:	libSDL_image-devel
BuildRequires:	libSDL_gfx-devel >= 2.0.21
BuildRequires:	yaml-cpp-devel >= 0.5.0

%description
OpenXcom is an open-source clone of the popular UFO: Enemy Unknown (X-COM: UFO Defense in USA) turn-based strategy game by MicroProse. It is written in C++ / SDL and licensed under the GPL. OpenXcom requires game resources from the original game (DOS, Windows or Steam version).

%prep
%setup -q

%build
cd ..
mkdir temp
cd temp
cmake  -DBUILD_PACKAGE:BOOL=ON \
       -DDEV_BUILD:BOOL=OFF \
       -DCMAKE_INSTALL_PREFIX:PATH=/usr \
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
%dir /usr/share/openxcom
/usr/share/openxcom/*
/usr/bin/openxcom

%changelog
