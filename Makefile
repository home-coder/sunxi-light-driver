export ARCH=arm
#export CROSS_COMPILE=/home/jiangxiujie/a3333/lichee/buildroot/output/external-toolchain/bin/arm-linux-gnueabi-
export CROSS_COMPILE=/home/jiangxiujie/a3333/lichee/out/sun8iw5p1/android/common/buildroot/external-toolchain/bin/arm-linux-gnueabi-
EXTRA_CFLAGS += $(DEBFLAGS) -Wall

obj-m += lcd_backlight.o
obj-m += sunxi_pinctrl_test.o
obj-m += key_input.o

KDIR ?= ~/a3333/lichee/linux-3.4
PWD := $(shell pwd)
all:
	make $(EXTRA_CFLAGS) -C $(KDIR) M=$(PWD) modules
clean:
	rm *.o *.ko *.mod.c *.symvers modules.order tags
