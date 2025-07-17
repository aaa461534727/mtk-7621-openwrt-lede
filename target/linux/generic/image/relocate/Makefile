#
# Makefile for the kernel relocation stub for MIPS devices
#
# Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
# Copyright (C) 2015 Felix Fietkau <nbd@nbd.name>
#
# Some parts of this file was based on the OpenWrt specific lzma-loader
# for the BCM47xx and ADM5120 based boards:
#	Copyright (C) 2004 Manuel Novoa III (mjn3@codepoet.org)
#	Copyright (C) 2005 Mineharu Takahara <mtakahar@yahoo.com>
#	Copyright (C) 2005 by Oleg I. Vdovikin <oleg@cs.msu.su>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.
#

LOADADDR	:=
LOADER_DATA	:=
BOARD		:=
FLASH_OFFS	:=
FLASH_MAX	:=
PLATFORM	:=
LZMA_TEXT_START := 0x81000000

CC		:= $(CROSS_COMPILE)gcc
LD		:= $(CROSS_COMPILE)ld
OBJCOPY		:= $(CROSS_COMPILE)objcopy
OBJDUMP		:= $(CROSS_COMPILE)objdump

BIN_FLAGS	:= -O binary -R .reginfo -R .note -R .comment -R .mdebug \
		   -R .MIPS.abiflags -S


LDFLAGS		= -static --gc-sections -no-warn-mismatch
LDFLAGS		+= -e startup -T loader.lds -Ttext $(LZMA_TEXT_START) 

O_FORMAT 	= $(shell $(OBJDUMP) -i | head -2 | grep elf32)

OBJECTS		:= head.o


ZWASFLAGS=$(shell ./ZWASFLAGS)

all: head.o loader.bin

# Don't build dependencies, this may die if $(CC) isn't gcc
dep:

install:

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o : %.S
	$(CC) $(ZWASFLAGS) -c -o $@ $<

loader: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

loader.bin: loader
	$(OBJCOPY) $(BIN_FLAGS) $< $@

mrproper: clean

clean:
	rm -f loader *.elf *.bin *.o
