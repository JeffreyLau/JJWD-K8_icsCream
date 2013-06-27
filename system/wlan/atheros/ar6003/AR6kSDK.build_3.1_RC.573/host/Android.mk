#------------------------------------------------------------------------------
# <copyright file="makefile" company="Atheros">
#    Copyright (c) 2005-2010 Atheros Corporation.  All rights reserved.
# 
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
#
#------------------------------------------------------------------------------
#==============================================================================
# Author(s): ="Atheros"
#==============================================================================

ifneq ($(TARGET_SIMULATOR),true)
ifeq ($(BOARD_USES_ATH_WIFI),true)
ifeq ($(filter-out ar6003 ar6103,$(BOARD_WIFI_MODULE)),)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := ar6000
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .ko
LOCAL_MODULE_PATH := $(TARGET_OUT)/wifi
LOCAL_SRC_FILES := os/linux/$(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
OVERRIDE_BUILT_MODULE_PATH := $(TARGET_OUT_INTERMEDIATE_LIBRARIES)
include $(BUILD_PREBUILT)

BOARD_WLAN_ATHEROS_SDK := system/wlan/atheros/ar6003/AR6kSDK.build_3.1_RC.573

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
export  ATH_SOFTMAC_FILE_USED=yes
export  ATH_CFG80211_ENV=no
export  ATH_DEBUG_DRIVER=yes
export  ATH_HTC_RAW_INT_ENV=yes
export  ATH_AR6K_OTA_TEST_MODE=no
export  ATH_BUILD_P2P=yes

ATH_HIF_TYPE:=sdio

export ATH_ANDROID_BUILD_FLAGS
ATH_CROSS_COMPILE_TYPE:=$(ANDROID_TOOLCHAIN)/arm-eabi-

$(LOCAL_PATH)/os/linux/ar6000.ko: ar6003-modules

.PHONY: ar6003-modules

ar6003-modules:
	$(hide) echo Building ar6003 modules...
	$(hide) $(MAKE) -C $(TOP)/kernel -f Makefile M=../system/wlan/atheros/ar6003/AR6kSDK.build_3.1_RC.573/host/os/linux modules

include $(LOCAL_PATH)/tools/Android.mk

endif
endif
endif
