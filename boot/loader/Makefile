CC=gcc

.PHONY : all

all : bootloader bootloaderusb

bootloader : bootloader.s
	$(CC) -v -nostdlib -Wl,--oformat,binary -o bootloader bootloader.s

bootloaderusb : bootloaderusb.s
	$(CC) -v -nostdlib -Wl,--oformat,binary -o bootloaderusb bootloaderusb.s
