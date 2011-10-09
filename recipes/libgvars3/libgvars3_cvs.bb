DESCRIPTION = "GVars3 - configuration system library"
HOMEPAGE = "http://mi.eng.cam.ac.uk/~er258/cvd/toon.html"
SECTION = "devel/libs"
PRIORITY = "optional"
LICENSE = "LGPLv2"

PR = "r3"

SRC_URI = "cvs://anoncvs@cvs.savannah.nongnu.org/cvsroot/libcvd;module=gvars3 \
file://fix-cppflags.patch \
"

S = "${WORKDIR}/gvars3"

DEPENDS = "libcvd"

inherit autotools

EXTRA_OECONF = "--disable-widgets --with-debug"
