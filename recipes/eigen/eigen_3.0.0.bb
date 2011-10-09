DESCRIPTION = "A C++ template library for linear algebra: matrices, vectors, and related algorithms."\
HOMEPAGE = "http://eigen.tuxfamily.org/index.php?title=Main_Page"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = "LGPL3 or GPL2"
PR = "r7"

SRC_URI = "http://bitbucket.org/eigen/eigen/get/${PV}.tar.bz2 \
"

S = "${WORKDIR}/eigen-eigen-65ee2328342f"

DEPENDS = "fftw"

#setup a shadow build, required by Eigen
OECMAKE_SOURCEPATH = "${S}"
OECMAKE_BUILDPATH = "${WORKDIR}/build"

OECMAKE_FORTRAN_COMPILER ?= "`echo ${F77} | sed 's/^\([^ ]*\).*/\1/'`"

inherit cmake

do_generate_toolchain_file_append() {
      echo "set( CMAKE_Fortran_COMPILER ${OECMAKE_FORTRAN_COMPILER} )" >> ${WORKDIR}/toolchain.cmake
}

do_compile() {
    cd ${OECMAKE_BUILDPATH}
    #oe_runmake blas
    oe_runmake lapack
}

SRC_URI[md5sum] = "046baf7072f008653361f8321560a26f"
SRC_URI[sha256sum] = "e60efc5b18331b2e6c23ac5a8180a13b987f0aeb6fc6dca316ae338fa0513931"

