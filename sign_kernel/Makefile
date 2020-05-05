obj-m := sign.o
sign-objs += module.o
sign-objs += sys_hook.o
sign-objs += hooks.o
sign-objs += pass_pid.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
