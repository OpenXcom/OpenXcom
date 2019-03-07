# Copyright 1999-2013 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=5

EGIT_REPO_URI="https://github.com/OpenXcom/OpenXcom.git"

inherit git-2 cmake-utils

DESCRIPTION="OpenXcom is an open-source clone of the popular UFO: Enemy Unknown"
HOMEPAGE="https://openxcom.org/"
SRC_URI=""

LICENSE="GPL-3"
SLOT="0"
KEYWORDS=""
IUSE="clang"

DEPEND="
	dev-cpp/yaml-cpp
	media-libs/libsdl
	media-libs/sdl-mixer
	media-libs/sdl-gfx
	media-libs/sdl-image
	virtual/opengl
"
RDEPEND="${DEPEND}"

src_configure() {
	if use clang; then
		export CC=clang
		export CXX=clang++
	fi
	cmake-utils_src_configure
}

pkg_postinst() {
    einfo "You will need to manually the original XCOM game data files to:"
    einfo "/usr/share/openxcom/UFO/ and/or /usr/share/openxcom/TFTD/"
	einfo "see http://ufopaedia.org/index.php?title=Installing_(OpenXcom)"
	einfo "for more info."
}
