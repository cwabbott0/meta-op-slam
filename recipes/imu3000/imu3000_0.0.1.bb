DESCRIPTION = "simple driver for the imu-3000"
PRIORITY = "optional"
SECTION = "base"
LICENSE = "GPLv2"
PR = "r1"

SRCREV = "64a492eaccdf58bb0441325ae734c4352d667380"

SRC_URI = "git://github.com/cwabbott0/imu3000.git \
"
LIC_FILES_CHKSUM = "file://COPYING;md5=8299a99afc88161a11a5cee8a54e2f41"

S = "${WORKDIR}/git"

inherit module

do_install() {
    # kernel module installs with other modules
    install -m 0755 -d ${D}${incdir}/linux ${D}${base_libdir}/modules/${KERNEL_VERSION}/extra/
    # use cp instead of install so the driver doesn't get stripped
    cp ${S}/imu3000.ko ${D}${base_libdir}/modules/${KERNEL_VERSION}/extra/
    install -m 0644 ${S}/imu3000_user.h ${D}${incdir}/linux
}
