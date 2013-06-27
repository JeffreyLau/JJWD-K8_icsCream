#------------------------------------------------------------------------------
# <copyright file="makefile" company="Atheros">
#    Copyright (c) 2005-2010 Atheros Corporation.  All rights reserved.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation;
#
# Software distributed under the License is distributed on an "AS
# IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
# implied. See the License for the specific language governing
# rights and limitations under the License.
#
#
#------------------------------------------------------------------------------
#==============================================================================
# Author(s): ="Atheros"
#==============================================================================

ifneq ($(TARGET_SIMULATOR),true)
ifeq ($(BOARD_USES_ATH_WIFI),true)
ifeq ($(filter-out ar6002 ar6102,$(BOARD_WIFI_MODULE)),)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := ar6000
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .ko
LOCAL_MODULE_PATH := $(TARGET_OUT)/wifi
LOCAL_SRC_FILES := os/linux/$(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
OVERRIDE_BUILT_MODULE_PATH := $(TARGET_OUT_INTERMEDIATE_LIBRARIES)
include $(BUILD_PREBUILT)

BOARD_WLAN_ATHEROS_SDK := system/wlan/atheros/ar6002/AR6kSDK.2.2.1.135

ATH_ANDROID_SRC_BASE:= $(BOARD_WLAN_ATHEROS_SDK)
export  ATH_BUILD_TYPE=ANDROID_ARM_NATIVEMMC
export  ATH_BUS_TYPE=sdio
export  ATH_OS_SUB_TYPE=linux_2_6

ATH_ANDROID_ROOT:= $(CURDIR)
export ATH_SRC_BASE:=$(ATH_ANDROID_ROOT)/$(BOARD_WLAN_ATHEROS_SDK)/host
KDIR := kernel
ATH_TARGET_OUTPUT:=$(ATH_ANDROID_ROOT)

export  ATH_ARCH_CPU_TYPE=arm
export  ATH_BUS_SUBTYPE=linux_sdio
export  ATH_ANDROID_ENV=yes
export  ATH_EEPROM_FILE_USED=yes
export  ATH_SOFTMAC_USED=yes

ATH_HIF_TYPE:=sdio

export ATH_ANDROID_BUILD_FLAGS
ATH_CROSS_COMPILE_TYPE:=$(ANDROID_TOOLCHAIN)/arm-eabi-

$(LOCAL_PATH)/os/linux/ar6000.ko: ar6002-modules

.PHONY: ar6002-modules

ar6002-modules:
	$(hide) echo Building ar6000 modules...
	$(hide) $(MAKE) -C $(TOP)/kernel -f Makefile M=../system/wlan/atheros/ar6002/AR6kSDK.2.2.1.135/host/os/linux modules

include $(LOCAL_PATH)/tools/Android.mk

endif
endif
endif
