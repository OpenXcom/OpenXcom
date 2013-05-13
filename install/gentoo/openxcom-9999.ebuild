# Copyright 1999-2011 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $
EAPI=3

inherit git autotools


DESCRIPTION="OpenXcom is an open-source clone of the popular
UFO: Enemy Unknown (X-Com: UFO Defense in USA)"
HOMEPAGE="http://openxcom.org/"
EGIT_REPO_URI="https://github.com/SupSuper/OpenXcom.git"

LICENSE="GPL"
SLOT="0"
KEYWORDS="x86 amd64"
IUSE=""

DEPEND="media-libs/libsdl
	media-libs/sdl-mixer
	media-sound/timidity++
	media-libs/sdl-gfx
	media-libs/sdl-image
	dev-cpp/yaml-cpp"
RDEPEND="${DEPEND}"

src_prepare() {
	eautoreconf
}
src_compile() {
	emake || die "emake failed"
}

src_install() {
	emake DESTDIR="${D}" install || die "emake install failed"
}

pkg_postinst() {
    einfo "You will need to install the Xcom data files in:"
    einfo "\$XDG_DATA_HOME/openxcomi/data or \$XDG_CONFIG_HOME/openxcom/data"
	einfo "see http://ufopaedia.org/index.php?title=Installing_(OpenXcom)"
	einfo "for more info."
}

 