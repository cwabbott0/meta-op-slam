DESCRIPTION = "CLAPACK - f2c'd LAPACK"
HOMEPAGE = "http://www.netlib.org/clapack/"
SECTION = "console/scientific"
PRIORITY = "optional"
LICENSE = "modified BSD"

PR = "r9"

LIC_FILES_CHKSUM = "file://COPYING;md5=50a26e7a8b67816e681718673ec547a7"

S = "${WORKDIR}/CLAPACK-${PV}"

SRC_URI = "http://www.netlib.org/clapack/clapack-${PV}.tgz \
file://make.inc \
file://arith.h \
file://fix-libf2c-makefile.patch \
"

do_configure() {
	rm -f ${S}/make.inc
	cp ${WORKDIR}/make.inc ${S}
	cp ${WORKDIR}/arith.h ${S}/F2CLIBS/libf2c
}

do_compile() {
	oe_runmake f2clib
	oe_runmake blaslib
	cd SRC
	oe_runmake
}

do_install() {
	install -m 0755 -d ${D}${docdir}/clapack ${D}${includedir} ${D}${libdir}
	install -m 0644 ${S}/INCLUDE/blaswrap.h ${S}/INCLUDE/clapack.h ${S}/INCLUDE/f2c.h ${D}${includedir}
	install -m 0644 ${S}/COPYING ${D}${docdir}/clapack
	oe_libinstall -C ${S} -a liblapack ${D}${libdir}
	oe_libinstall -C ${S} -a libblas ${D}${libdir}
	oe_libinstall -C ${S}/F2CLIBS -a libf2c ${D}${libdir}
}

SRC_URI[md5sum] = "a94e28a0ab6f0454951e7ef9c89b3a38"
SRC_URI[sha256sum] = "aa14528e8901c4e970814d44610e5377d33f62077e151965660b2d0690459428"
