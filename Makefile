KERNEL_RELEASE = $(shell uname -r)
KERNEL_BUILDROOT ?= /lib/modules/$(KERNEL_RELEASE)/build
BUILDROOT = $(shell pwd)

TARGETS = myfs.ko
OBJECTS = myfs.o myfs.mod.c myfs.mod.o .myfs.ko.cmd .myfs.mod.o.cmd .myfs.o.cmd .tmp_versions modules.order Module.symvers

obj-m := myfs.o

all :
	$(MAKE) -C $(KERNEL_BUILDROOT) SUBDIRS=$(BUILDROOT) modules

clean :
	-rm -frv $(OBJECTS)

distclean : clean
	-rm -frv $(TARGETS)

test :
	umount /mnt || true
	rmmod myfs || true
	insmod $(BUILDROOT)/myfs.ko
	mount -t myfs none /mnt
	cd /mnt
	touch /mnt/test
	mkdir -p /mnt/d1/d2
	cd /
	umount /mnt
	rmmod myfs
	dmesg | grep -i --color=auto myfs


.PHONY : all clean distclean test
