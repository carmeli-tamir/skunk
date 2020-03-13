MODULE_NAME := skunk

obj-m := $(MODULE_NAME).o
$(MODULE_NAME)-objs := $(patsubst %.c, %.o, $(notdir $(wildcard $M/*.c)))

EXTRA_CFLAGS=-I $(PWD)/../third_party/protobuf-c/

KDIR?=/lib/modules/$(shell uname -r)/build

all:
	python3 generate_function_calls.py
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean