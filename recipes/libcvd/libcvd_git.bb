DESCRIPTION = "cvd - a very portable and high performance C++ library for computer vision, image, and video processing from the University of Cambridge"
HOMEPAGE = "http://mi.eng.cam.ac.uk/~er258/cvd/"
SECTION = "console/scientific"
PRIORITY = "optional"
LICENSE = "LGPLv2"

PR = "r0"

LIC_FILES_CHKSUM = "file://LICENSE;md5=a6f89e2100d9b6cdffcea4f398e37343"

SRC_URI = "git://git.savannah.nongnu.org/libcvd.git \
file://fix_cppflags.patch \
"

SRCREV = "7ff942c1c8da792e35d2beedb52814de7a59da14"
PV = "gitr${SRCREV}"

S = "${WORKDIR}/git"

DEPENDS = "libtoon jpeg tiff libpng"

inherit autotools

EXTRA_OECONF = "--without-ffmpeg --without-lapack"

do_configure() {
	autoconf
	oe_runconf
}
