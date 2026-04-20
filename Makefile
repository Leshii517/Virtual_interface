# Makefile для модуля vping

obj-m += virtual_interface.o

# Дополнительно можно указать флаги компиляции (необязательно)
# ccflags-y += -DDEBUG

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

help:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) help