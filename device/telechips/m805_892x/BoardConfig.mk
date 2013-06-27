#
# Copyright (C) 2011-2012 Telechips, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# BoardConfig.mk
#
# Product-specific compile-time definitions.
#

ifneq ($(filter nand_v7,$(PRODUCT_TAGS)),)
BOARD_HAVE_NAND_FLASH := true
TARGET_NAND_TNFTL_VERSION := 7
endif
ifneq ($(filter nand_v8,$(PRODUCT_TAGS)),)
BOARD_HAVE_NAND_FLASH := true
TARGET_NAND_TNFTL_VERSION := 8
TARGET_USERIMAGES_USE_EXT4 := true
endif
ifneq ($(filter emmc,$(PRODUCT_TAGS)),)
BOARD_HAVE_NAND_FLASH := false
TARGET_USERIMAGES_USE_EXT4 := true
endif

# Kernel/Bootloader defines
ifeq ($(BOARD_HAVE_NAND_FLASH),true)
TARGET_BOOTLOADER_BOARD_NAME := m805_892x_evm
else
TARGET_BOOTLOADER_BOARD_NAME := m805_892x_evm_emmc
endif
TARGET_NO_BOOTLOADER := true
TARGET_NO_RADIOIMAGE := true

KERNEL_DEFCONFIG := m805_892x_defconfig

BOARD_CFLAGS := -DTCC892X
TARGET_BOOTLOADER_BOARD_CFLAGS := -D_TCC8920_

ifeq ($(TARGET_USERIMAGES_USE_EXT4),true)
BOARD_FLASH_BLOCK_SIZE := 4096
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 314572800 #200MB System
BOARD_USERDATAIMAGE_PARTITION_SIZE := 1073741824 #1GB UserData
else
BOARD_NAND_PAGE_SIZE := 8192
BOARD_NAND_SPARE_SIZE := 256
BOARD_KERNEL_PAGESIZE := 8192
BOARD_FLASH_BLOCK_SIZE := 8192
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 314572800
endif

# Wi-Fi defines
BOARD_USES_ATH_WIFI := false
BOARD_USES_REALTEK_WIFI := true
BOARD_USES_BROADCOM_WIFI := false

ifeq ($(BOARD_USES_ATH_WIFI), true)
BOARD_HAS_ATH_WLAN := true
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_HOSTAPD_DRIVER := NL80211
WPA_SUPPLICANT_VERSION := VER_0_8_ATHEROS
HOSTAPD_VERSION := VER_0_8_ATHEROS
WIFI_DRIVER_MODULE_PATH         := "/system/wifi/ath6kl_sdio.ko"
WIFI_DRIVER_MODULE_NAME         := "ath6kl_sdio"
WIFI_DRIVER_MODULE_ARG          := ""
WIFI_DRIVER_FW_PATH_STA         := "sta"
WIFI_DRIVER_FW_PATH_AP          := "ap"
WIFI_DRIVER_FW_PATH_P2P         := "p2p"
WIFI_SDIO_IF_DRIVER_MODULE_PATH := "/system/wifi/cfg80211.ko"
WIFI_SDIO_IF_DRIVER_MODULE_NAME := "cfg80211"
WIFI_SDIO_IF_DRIVER_MODULE_ARG  := ""
WIFI_TEST_INTERFACE             := "sta"
# Choose the vendor of WLAN for wlan_mfg
WLAN_VENDOR := 2
WLAN_CHIP := ar6003
endif

ifeq ($(BOARD_USES_REALTEK_WIFI), true)
    WPA_SUPPLICANT_VERSION := VER_0_8_X
    BOARD_WPA_SUPPLICANT_DRIVER := NL80211
    BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_rtl
    BOARD_HOSTAPD_DRIVER        := NL80211
    BOARD_HOSTAPD_PRIVATE_LIB   := lib_driver_cmd_rtl

    WIFI_DRIVER_MODULE_NAME   := wlan
    WIFI_DRIVER_MODULE_PATH   := "/system/wifi/wlan.ko"

    WIFI_DRIVER_MODULE_ARG    := ""
    WIFI_FIRMWARE_LOADER      := ""
    WIFI_DRIVER_FW_PATH_STA   := ""
    WIFI_DRIVER_FW_PATH_AP    := ""
    WIFI_DRIVER_FW_PATH_P2P   := ""
    WIFI_DRIVER_FW_PATH_PARAM := ""
endif

ifeq ($(BOARD_USES_BROADCOM_WIFI), true)
    BOARD_WPA_SUPPLICANT_DRIVER 	:= NL80211
    WPA_SUPPLICANT_VERSION      	:= VER_0_8_X
    BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_bcmdhd
    BOARD_HOSTAPD_DRIVER        	:= NL80211
    BOARD_HOSTAPD_PRIVATE_LIB   	:= lib_driver_cmd_bcmdhd
    BOARD_WLAN_DEVICE           	:= bcmdhd
    WIFI_DRIVER_MODULE_NAME 		:= bcmdhd
    WIFI_DRIVER_FW_PATH_PARAM   	:= "/data/misc/wifi/firmware_path"
    WIFI_DRIVER_MODULE_PATH     	:= "/system/wifi/bcm4330.ko"
    WIFI_DRIVER_FW_PATH_STA     	:= "/system/wifi/fw_bcmdhd.bin"
    WIFI_DRIVER_FW_PATH_P2P     	:= "/system/wifi/fw_bcmdhd_p2p.bin"
    WIFI_DRIVER_FW_PATH_AP      	:= "/system/wifi/fw_bcmdhd_apsta.bin"
    BOARD_WLAN_DEVICE_REV              := bcm4330_0412
endif

# Bluetooth defines
#
BOARD_HAVE_BLUETOOTH := true
CUSTOM_BLUETOOTH_VENDOR := csr
#CUSTOM_BLUETOOTH_VENDOR := bcm

BOARD_VIDEO_DISPLAY_BY_VSYNC_INT_FLAG := true
#BOARD_VIDEO_DISPLAY_BY_VSYNC_INT_FLAG := false 
BOARD_VIDEO_DEINTERLACE_SUPPORT_FLAG := true
#BOARD_VIDEO_DEINTERLACE_SUPPORT_FLAG := false
BOARD_SKIP_UI_UPDATE_DURING_VIDEO_FLAG := true

#if you want use bluetooth and wifi coex , you should check below option.
#BOARD_ANTENNA_SHARING_BT_WIFI := true

#if you want use csr bluetooth host wake up, you should check below option.
#BOARD_BT_HOST_WAKE_UP := true
# 
# /Bluetooth defines

# TV output defines
BOARD_HAVE_HDMI_OUTPUT := true
#BOARD_HDMI_CEC := -DTCC_HDMI_CEC
#BOARD_HDMI_HDCP := -DTCC_HDMI_HDCP
BOARD_HAVE_TV_ANALOG_OUTPUT := true
BOARD_TV_COMPOSITE := -DTCC_OUTPUT_COMPOSITE
#BOARD_TV_COMPONENT := -DTCC_OUTPUT_COMPONENT
# to use vpu library in kernel
BOARD_USE_VPU_IN_KERNEL_FLAGS := true
BOARD_VPU_IN_KERNEL_FLAGS := -DMOVE_VPU_IN_KERNEL


# GPS define
BOARD_GPS_LIBRARIES := libgps
# #BOARD_GPS_MAKER := tcc_gps
BOARD_GPS_MAKER := surf_gps

# Audio Codec Chip
TARGET_BOARD_AUDIO_CODEC := cs42l52

# Touch Calibration - use only pressure sensitive touch screen
#TARGET_HAVE_TSLIB := true

TARGET_RECOVERY_UI_LIB := librecovery_ui_m805_892x
TARGET_RECOVERY_UPDATER_LIBS += librecovery_updater_telechips
TARGET_RECOVERY_INIT_RC := device/telechips/m805_892x/recovery/init.m805_892x.rc

include device/telechips/tcc892x-common/BoardConfigCommon.mk
