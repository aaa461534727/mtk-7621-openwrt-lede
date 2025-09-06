#
# Copyright (C) 2015 CaryStudio
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=libcommon
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk
	
define Package/libcommon
  SECTION:=bob Properties
  CATEGORY:=bob Properties
  SUBMENU:=libs
  TITLE:= common library
  DEPENDS:=+libpthread +libcjson +libuci +libblobmsg-json +libubus +libubox +libkvcutil +zlib
endef


define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libcommon.so $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/*.h $(1)/usr/include
endef


define Package/libcommon/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libcommon.so $(1)/usr/lib/
endef

	
$(eval $(call BuildPackage,libcommon))