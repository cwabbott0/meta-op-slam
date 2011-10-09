require /home/connor/angstrom/sources/meta-angstrom/recipes-angstrom/images/systemd-image.bb

IMAGE_INSTALL += " \
	task-xfce-base \
	task-xserver \
	xfce-terminal \
"

export IMAGE_BASENAME = "systemd-XFCE-image"