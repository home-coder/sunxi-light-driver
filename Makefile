export ARCH=arm
#export CROSS_COMPILE=/home/jiangxiujie/h2222/lichee/buildroot/output/external-toolchain/bin/arm-linux-gnueabi-
export CROSS_COMPILE=/home/jiangxiujie/h2222/lichee/brandy/gcc-linaro/bin/arm-linux-gnueabi-
EXTRA_CFLAGS += $(DEBFLAGS) -Wall

obj-m += yang.o
obj-m += sunxi_pinctrl_test.o
#obj-m += key_input.o

KDIR ?= ~/h2222/lichee/linux-3.4
PWD := $(shell pwd)
all:
	make $(EXTRA_CFLAGS) -C $(KDIR) M=$(PWD) modules
clean:
	rm *.o *.ko *.mod.c *.symvers modules.order
