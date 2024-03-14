CFLAGS_MODULE += -std=gnu11
PWD := $(shell pwd)

# obj-m += BeginningVMOS.o
# BeginningVMOS-objs := BeginningVMOS_intel.o vmexit_handler.o
obj-m +=  main.o
obj-m += netlink/netlink_kern.o
obj-m += device/dev_kern.o
obj-m += ioctl/ioctl.o
obj-m += VMX/vmx.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
