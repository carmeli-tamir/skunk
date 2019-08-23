#TODO: Get all .c files in dir instead of adding one by one
skunk-objs += main.o skunk_device.o skunk.pb-c.o call_functions.o
obj-m += skunk.o

EXTRA_CFLAGS=-I $(PWD)/../third_party/protobuf-c/

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean