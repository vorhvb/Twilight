# ──────────────────────────────────────────────────────────────────────
#
# Name					Makefile
# Project				Twilight
# Author				Elisey Konstantinov
#
# License				GPL v3
#
# ──────────────────────────────────────────────────────────────────────

FLAGS=-m16 -target i386-elf
C=-ffreestanding -nostdlib -fno-pic
LD=--image-base 0 --oformat=binary

# ──────────────────────────────────────────────────────────────────────

all: compile link pack
compile:
	clang -c Bootloader.asm -o Build/Objects/Bootloader.o \
		$(FLAGS)
	clang -c Menu.asm -o Build/Objects/Menu.o $(FLAGS)
	clang -c Kernel.c -o Build/Objects/Kernel.o \
		$(FLAGS) $(C)

	clang -c Modules/*.c \
		$(FLAGS) $(C) \
	&& mv *.o Build/Objects/Modules

	clang -c Modules/Display/*.c \
		$(FLAGS) $(C) \
	&& mv *.o Build/Objects/Modules/Display
	clang -c Modules/Storage/*.c \
		$(FLAGS) $(C) \
	&& mv *.o Build/Objects/Modules/Storage
link:
	ld.lld Build/Objects/Bootloader.o -o Build/Binaries/Bootloader \
		-e _start -Ttext 0x7C00 $(LD)
	ld.lld Build/Objects/Menu.o -o Build/Binaries/Menu \
		-e menu -Ttext 0x7E00 $(LD)

	ld.lld Build/Objects/Kernel.o Build/Objects/Modules/*.o Build/Objects/Modules/Display/* Build/Objects/Modules/Storage/* -o Build/Binaries/Kernel \
		-e main -Ttext 0x8E00 $(LD)
pack:
	dd if=/dev/zero of=Build/Live.img \
		bs=1K count=360
	mkfs.fat -F 12 -n "Twilight" -R 9 \
		Build/Live.img

	dd if=Build/Binaries/Bootloader of=Build/Live.img \
		bs=1 seek=62 conv=notrunc
	dd if=Build/Binaries/Menu of=Build/Live.img \
		seek=1 conv=notrunc

	sudo mount -o loop -t msdos \
		Build/Live.img /mnt
	sudo mkdir /mnt/Boot
	sudo cp Build/Binaries/Kernel /mnt/Boot
	sudo umount /mnt

	printf '\130' | dd of=Build/Live.img \
		bs=1 seek=1 conv=notrunc

prepare:
	-mkdir Build
	-mkdir Build/Objects
	-mkdir Build/Binaries

	-mkdir Build/Objects/Modules
	-mkdir Build/Objects/Modules/Display
	-mkdir Build/Objects/Modules/Storage

# ──────────────────────────────────────────────────────────────────────

clean:
	rm -r Build/Objects
	rm -r Build/Binaries
scrub:
	rm -r Build
run:
	qemu-system-x86_64 -fda Build/Live.img

# ──────────────────────────────────────────────────────────────────────

dump:
	tree || find .

	cat *.asm
	cat Kernel.c

	cat Modules/*
	cat Modules/Display/*
	cat Modules/Storage/*

	cat Makefile

edit:
	zed . || $(VISUAL) . || $(EDITOR) .

# ──────────────────────────────────────────────────────────────────────
