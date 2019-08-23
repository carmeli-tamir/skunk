MODULE_NAME := skunk

obj-m := $(MODULE_NAME).o
$(MODULE_NAME)-objs := $(patsubst %.c, %.o, $(notdir $(wildcard $M/*.c)))

EXTRA_CFLAGS=-I $(PWD)/../third_party/protobuf-c/

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean