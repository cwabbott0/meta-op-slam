DESCRIPTION = "GVars3 - configuration system library"
HOMEPAGE = "http://mi.eng.cam.ac.uk/~er258/cvd/toon.html"
SECTION = "devel/libs"
PRIORITY = "optional"
LICENSE = "LGPLv2"

PR = "r3"

LIC_FILES_CHKSUM = "file://LICENSE;md5=a6f89e2100d9b6cdffcea4f398e37343"

SRC_URI = "cvs://anoncvs@cvs.savannah.nongnu.org/cvsroot/libcvd;module=gvars3 \
file://fix-cppflags.patch \
"

S = "${WORKDIR}/gvars3"

DEPENDS = "libcvd"

inherit autotools

EXTRA_OECONF = "--disable-widgets --with-debug"
