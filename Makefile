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

.PHONY : all clean distclean
