DESCRIPTION = "Parallel Tracking and Mapping - George Klein's bundle adjustment - based SLAM solution"
HOMEPAGE = "http://www.robots.ox.ac.uk/~gk/PTAM/"
SECTION = "console/scientific"
PRIORITY = "optional"
LICENSE = "see http://www.robots.ox.ac.uk/~gk/PTAM/download.html"

PR = "r0"

LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=eb23bcd19c4214e95d4363756df2e544"

DEPENDS = "opencv libcvd libgvars3 virtual/libgl readline"

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

S = "${WORKDIR}/PTAM_1.0-r114"

do_configure() {
	cp ${WORKDIR}/VideoSource_Linux_OpenCV.cc ${S}
}

do_compile() {
	oe_runmake All
}

do_install() {
	install -m 0755 -d ${D}${bindir} ${D}${docdir}/ptam ${D}${sysconfdir}/PTAM
	install -m 0755 ${S}/PTAM ${S}/CameraCalibrator ${D}${bindir}
	install -m 0755 ${S}/settings.cfg ${S}/calibrator_settings.cfg ${S}/video_source_settings.cfg ${D}${sysconfdir}/PTAM
	install -m 0644 ${S}/README.txt ${S}/LICENSE.txt ${D}${docdir}/ptam
}
