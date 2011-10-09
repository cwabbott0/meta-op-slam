DRIVERNAME = imu3000

ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m += imu3000.o

imu3000-objs := imu3000-char.o imu3000-i2c.o imu3000-irq.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif
