#
# release Makefile for the Carystudio mtk openwrt
#
# Copyright (C) 2019, carystudio.com. All Rights Reserved.
# 
#
# Add by CaryStudio
# 2019-12-13
#
#


RELEASE_DIR := $(shell pwd)
RELEASE_IMG_DIR := $(RELEASE_DIR)/images
MENUCONF_DIR := $(RELEASE_DIR)/def_config
MT7621_DIR :=  $(RELEASE_DIR)/mt7621

DATE := $(shell date +%Y%m%d)
TIME := $(shell ( date +%Y%m%d-%H%M%S ) )

#Dynamic update
SVN=$(shell git rev-list HEAD --count 2>/dev/null)

CPUNUM=$(shell cat /proc/cpuinfo  | grep processor | wc -l)

ifeq ($(CS_GIT_USER)$(CS_GIT_PASS),)
JOBS=$(shell expr ${CPUNUM} / 2)
else
JOBS=${CPUNUM}
endif

SIZE4M=4194304
SIZE8M=8388608
SIZE16M=16777216
SIZE32M=33554432
SIZE128M=134217728

#default ID
Brand=CS
CSID=CSID
MODEL=NA
Hardware=IP04381
Chip=MT7621ANoWiFi
FlashType=SPI
FLASH_DRAM_SIZE=16M128M
SoftVer=V9.1.0c
UbootUpg=u
Language=ALL
Vendor=
#release depend
folder=mt7621
map=IP04381
dest=${Brand}_${CSID}_${MODEL}_${Hardware}_${Chip}_${FlashType}_${FLASH_DRAM_SIZE}_${SoftVer}${UbootUpg}.${SVN}_B${DATE}_${Language}
dest_dlx=${Brand}_${CSID}_CMCC_${MODEL}_${Chip}_${FlashType}_${FLASH_DRAM_SIZE}_${SoftVer}${UbootUpg}.${SVN}_B${DATE}_${Language}

#depend platform
WEB_FILE := $(RELEASE_DIR)/../bin/targets/ramips/$(folder)/lede-ramips-mt7621-mt7621-rfb-ax-nor-squashfs-sysupgrade.bin

DEBUG:=99

#default target
t=all

pre_svnup: 
	cd $(RELEASE_DIR)/..; \
	#	git pull

ifneq ($(wildcard ../tmp),)
pre_clean:
	@echo prepare clean...
	cd $(RELEASE_DIR)/..; \
		rm -rf tmp
else
pre_clean:
	@echo no tmp dir, do nothing
endif

pre_defconfig:
	cd $(RELEASE_DIR)/..; \
		cp -f $(MENUCONF_DIR)/$(CSID)/.config .config; \
		cp -f $(MENUCONF_DIR)/$(CSID)/config-4.4 target/linux/ramips/${folder}/config-4.4; \
		make defconfig

trunk_make:
	cd $(RELEASE_DIR)/..; \
		make V=$(DEBUG) -j$(JOBS)

release:
	@echo "Releasing firmware for $(CSID)"
	cp -f "$(WEB_FILE)" "$(RELEASE_IMG_DIR)/${CSID}_${TIME}.bin" 


all: pre_svnup pre_defconfig pre_clean 
	@echo "Building Release $(V_CSID) $(V_MODEL) firmware"
	
	make trunk_make

	make release

re_make: pre_svnup
	@echo " re make $(V_CSID) $(V_MODEL) firmware "
	
	make trunk_make

renew: pre_svnup
	@echo " renew firmware "

	make pre_defconfig

	make trunk_make

	make release	

C8888:
	@$(MAKE) $(t) Brand=CS CSID=C8888 MODEL=X5000R_V2 Hardware=IP04520 Chip=MT7621MT7915 FLASH_SIZE=$(SIZE16M) FLASH_DRAM_SIZE=16M256M Language=ALL folder=mt7621 map=IP04520
help:
	@echo "make [CSID]"
	@echo ""
	@echo "Support CSID List:"
	@echo "[CSID]         [Brand]        [Description]"
	@echo "C8888         	CS           - ( C8888   IP04433, MT7621 MT7915, MESH+TR069, TOTOLINK)"
	@echo ""

.PHONY: help all pre_svnup pre_clean pre_defconfig trunk_make release re_make renew

