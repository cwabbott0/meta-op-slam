DESCRIPTION = "simple test for the imu-3000 driver"
PRIORITY = "optional"
SECTION = "base"
LICENSE = "GPL"
PR = "r6"

DEPENDS = "imu3000"

SRC_URI = "file://* \
"

S = "${WORKDIR}/imu3000test"

do_compile() {
	${CC} ${LDFLAGS} -o imu3000test main.c
}

do_install() {
	install -m 0755 -d ${D}${bindir}
	install -m 0755 ${S}/imu3000test ${D}${bindir}
}
