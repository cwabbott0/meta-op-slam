DESCRIPTION = "TooN - an object-oriented numerics library from the University of Cambridge"
HOMEPAGE = "http://mi.eng.cam.ac.uk/~er258/cvd/toon.html"
SECTION = "devel/libs"
PRIORITY = "optional"
LICENSE = "GPLv2"

PR = "r0"

LIC_FILES_CHKSUM = "file://COPYING;md5=2f90f5227144ac9888eaaa632cb7ebcf \
file://GPL.txt;md5=6731edef2727e51a061f498b5d6a282a \
"

SRC_URI = "cvs://anoncvs@cvs.savannah.nongnu.org/cvsroot/toon;module=TooN \
file://config.hh.lapack \
file://config.hh.no_lapack \
"

DEPENDS = "clapack"

S = "${WORKDIR}/TooN"

do_configure() {
	cp ${WORKDIR}/config.hh.lapack ${S}/internal/config.hh
}

do_compile() {
}

do_install() {
	install -m 0755 -d ${D}${includedir}
	cp -rf ${S} ${D}${includedir}
}
