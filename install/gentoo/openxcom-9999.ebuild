# Copyright 1999-2013 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=5

EGIT_REPO_URI="https://github.com/SupSuper/OpenXcom.git"

inherit git-2 autotools

DESCRIPTION="OpenXcom is an open-source clone of the popular UFO: Enemy Unknown"
HOMEPAGE="http://openxcom.org/"
SRC_URI=""

LICENSE="GPL-3"
SLOT="0"
KEYWORDS=""
IUSE=""

DEPEND="
	dev-cpp/yaml-cpp
	media-libs/libsdl
	media-libs/sdl-mixer
	media-libs/sdl-gfx
	media-libs/sdl-image
	virtual/opengl
"
RDEPEND="${DEPEND}"

src_prepare() {
	eautoreconf
}

pkg_postinst() {
    einfo "You will need to install the Xcom data files in:"
    einfo "\$XDG_DATA_HOME/openxcom/data or \$XDG_CONFIG_HOME/openxcom/data"
	einfo "see http://ufopaedia.org/index.php?title=Installing_(OpenXcom)"
	einfo "for more info."
}
