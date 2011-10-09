DESCRIPTION = "cvd - a very portable and high performance C++ library for computer vision, image, and video processing from the University of Cambridge"
HOMEPAGE = "http://mi.eng.cam.ac.uk/~er258/cvd/"
SECTION = "console/scientific"
PRIORITY = "optional"
LICENSE = "LGPLv2"

PR = "r8"

SRC_URI = "cvs://anoncvs@cvs.savannah.nongnu.org/cvsroot/libcvd;module=libcvd \
file://fix_cppflags.patch \
"

S = "${WORKDIR}/libcvd"

DEPENDS = "libtoon jpeg tiff libpng"

inherit autotools

EXTRA_OECONF = "--without-ffmpeg --without-lapack"

do_configure() {
	autoconf
	oe_runconf
}
