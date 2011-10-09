DESCRIPTION = "CLAPACK - f2c'd LAPACK"
HOMEPAGE = "http://www.netlib.org/clapack/"
SECTION = "console/scientific"
PRIORITY = "optional"
LICENSE = "modified BSD"

PR = "r7"

S = "${WORKDIR}"

SRC_URI = "http://www.netlib.org/clapack/clapack-${PV}.tgz \
file://make.inc \
file://arith.h \
file://fix-libf2c-makefile.patch;striplevel=0 \
"

do_configure() {
	rm -f ${WORKDIR}/CLAPACK-${PV}/make.inc
	cp ${WORKDIR}/make.inc ${WORKDIR}/CLAPACK-${PV}
	cp ${WORKDIR}/arith.h ${WORKDIR}/CLAPACK-${PV}/F2CLIBS/libf2c
}

do_compile() {
	cd ${WORKDIR}/CLAPACK-${PV}
	oe_runmake f2clib
	oe_runmake blaslib
	cd SRC
	oe_runmake
}

do_install() {
	install -m 0755 -d ${D}${docdir}/clapack ${D}${incdir} ${D}${libdir}
	install -m 0644 ${WORKDIR}/CLAPACK-${PV}/INCLUDE/blaswrap.h ${WORKDIR}/CLAPACK-${PV}/INCLUDE/clapack.h ${WORKDIR}/CLAPACK-${PV}/INCLUDE/f2c.h ${D}${incdir}/
	install -m 0644 ${WORKDIR}/CLAPACK-${PV}/COPYING ${D}${docdir}/clapack
	oe_libinstall -C ${WORKDIR}/CLAPACK-${PV}/ -a liblapack ${D}${libdir}
	oe_libinstall -C ${WORKDIR}/CLAPACK-${PV}/ -a libblas ${D}${libdir}
	oe_libinstall -C ${WORKDIR}/CLAPACK-${PV}/F2CLIBS -a libf2c ${libdir}
}

SRC_URI[md5sum] = "a94e28a0ab6f0454951e7ef9c89b3a38"
SRC_URI[sha256sum] = "aa14528e8901c4e970814d44610e5377d33f62077e151965660b2d0690459428"
