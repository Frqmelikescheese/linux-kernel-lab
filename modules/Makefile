obj-m += protector.o
obj-m += ghost.o
obj-m += logger.o

all:
	make -C $(HOME)/linux M=$(PWD) modules

clean:
	make -C $(HOME)/linux M=$(PWD) clean
