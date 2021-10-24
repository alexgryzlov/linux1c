obj-m := phonebook.o

all:
	make -C /lib/modules/5.14.9/build M=$(PWD) modules
clean:
	make -C /lib/modules/5.14.9/build M=$(PWD) clean

