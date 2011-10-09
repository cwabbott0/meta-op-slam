DESCRIPTION = "Parallel Tracking and Mapping - George Klein's bundle adjustment - based SLAM solution"
HOMEPAGE = "http://www.robots.ox.ac.uk/~gk/PTAM/"
SECTION = "console/scientific"
PRIORITY = "optional"
LICENSE = "see http://www.robots.ox.ac.uk/~gk/PTAM/download.html"

PR = "r14"

DEPENDS = "opencv libcvd libgvars3 mesa readline"

RDEPENDS = "opencv libcvd libgvars3 mesa"

SRC_URI = "file://PTAM_1.0-r114/*.cc \
file://PTAM_1.0-r114/*.h \
file://PTAM_1.0-r114/settings.cfg \
file://PTAM_1.0-r114/calibrator_settings.cfg \
file://PTAM_1.0-r114/video_source_settings.cfg \
file://PTAM_1.0-r114/Makefile \
file://PTAM_1.0-r114/README.txt \
file://PTAM_1.0-r114/LICENSE.txt \
file://VideoSource_Linux_OpenCV.cc \
"

do_configure() {
	cp ${WORKDIR}/PTAM_1.0-r114/* ${WORKDIR}
	rm -rf ${WORKDIR}/PTAM_1.0-r114
}

do_compile() {
	cd ${WORKDIR}
	oe_runmake All
}

do_install() {
	install -m 0755 -d ${D}${bindir} ${D}${docdir}/ptam
	install -m 0755 ${WORKDIR}/PTAM ${WORKDIR}/CameraCalibrator ${D}${bindir}
	install -m 0755 ${WORKDIR}/settings.cfg ${WORKDIR}/calibrator_settings.cfg ${WORKDIR}/video_source_settings.cfg ${D}${bindir}
	install -m 0644 ${WORKDIR}/README.txt ${WORKDIR}/LICENSE.txt ${D}${docdir}/ptam
}
