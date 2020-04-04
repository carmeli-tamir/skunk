MODULE_NAME := skunk

obj-m := $(MODULE_NAME).o
$(MODULE_NAME)-objs := $(patsubst %.c, %.o, $(notdir $(wildcard $M/*.c)))
$(MODULE_NAME)-objs += $(patsubst %.c, %.o, $(addprefix function_call/, $(notdir $(wildcard $M/function_call/*.c))))
$(MODULE_NAME)-objs += $(patsubst %.c, %.o, $(addprefix mock/, $(notdir $(wildcard $M/mock/*.c))))

EXTRA_CFLAGS=-I $(PWD)/../third_party/protobuf-c/ -I $(PWD) -I $(PWD)/mock/

KDIR?=/lib/modules/$(shell uname -r)/build

all:
	python3 function_call/generate_function_calls.py
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean