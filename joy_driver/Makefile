obj-m += usb_2.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
