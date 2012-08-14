ifneq ($(KERNELRELEASE),)
obj-m := hpt.o
else
ifeq ($(KERNELDIR),)	
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
endif
PWD := $(shell pwd)
all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -f *.o *.ko
endif
