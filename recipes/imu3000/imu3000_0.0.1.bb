DESCRIPTION = "simple driver for the imu-3000"
PRIORITY = "optional"
SECTION = "base"
LICENSE = "GPL"

SRC_URI = "file://* \
"

S = "${WORKDIR}/imu3000"

inherit module

do_install() {
    # kernel module installs with other modules
    install -m 0755 -d ${D}${incdir}/linux ${D}${base_libdir}/modules/${KERNEL_VERSION}/extra/
    # use cp instead of install so the driver doesn't get stripped
    cp ${S}/imu3000.ko ${D}${base_libdir}/modules/${KERNEL_VERSION}/extra/
    install -m 0644 ${S}/imu3000_user.h ${D}${incdir}/linux
}
