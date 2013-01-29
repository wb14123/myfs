KERNEL_RELEASE = $(shell uname -r)
KERNEL_BUILDROOT ?= /lib/modules/$(KERNEL_RELEASE)/build
BUILDROOT = $(shell pwd)

TARGETS = myfs.ko
OBJECTS = myfs.o myfs.mod.c myfs.mod.o .myfs.ko.cmd .myfs.mod.o.cmd .myfs.o.cmd .tmp_versions

obj-m := myfs.o

all :
	$(MAKE) -C $(KERNEL_BUILDROOT) SUBDIRS=$(BUILDROOT) modules

clean :
	-rm -fr $(OBJECTS)

distclean : clean
	-rm -fr $(TARGETS)

test :
	rmmod myfs || true
	insmod $(PWD)/myfs.ko
	mount -t myfs none /mnt
	cd /mnt
	cd /
	umount /mnt
	rmmod myfs
	dmesg | grep -i --color=auto myfs


.PHONY : all clean distclean test
