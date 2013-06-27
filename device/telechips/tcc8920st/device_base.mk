#
# Copyright (C) 2011 Telechips, Inc.
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
#

ifeq ($(TARGET_PREBUILT_KERNEL),)
LOCAL_KERNEL := kernel/arch/arm/boot/Image
else
LOCAL_KERNEL := $(TARGET_PREBUILT_KERNEL)
endif

DEVICE_PACKAGE_OVERLAYS := device/telechips/tcc8920st/overlay

PRODUCT_AAPT_CONFIG := hdpi mdpi

PRODUCT_PROPERTY_OVERRIDES += \
	ro.system.hdmi_max_resolution = fullhd \
	ro.system.hdmi_active = true \
	ro.QB.enable = 1 \
	persist.sys.hdmi_resolution = 125 \
	persist.sys.hdmi_resize_x = 0 \
	persist.sys.hdmi_resize_y = 0 \
	tcc.hdmi.uisize = 1280x720 \
	persist.sys.renderer_onthefly = true

PRODUCT_PROPERTY_OVERRIDES += \
        ro.system.composite_active = true \
	persist.sys.composite_mode = 0 \
        persist.sys.composite_resize_x = 3 \
	persist.sys.composite_resize_y = 3

PRODUCT_PROPERTY_OVERRIDES += \
        ro.system.component_active = true \
	persist.sys.component_mode = 1 \
        persist.sys.component_resize_x = 3 \
	persist.sys.component_resize_y = 3 \
	persist.sys.auto_resolution = 0

PRODUCT_PROPERTY_OVERRIDES += \
        ro.system.hdmi_portable = false \
	ro.system.osd_active = true

PRODUCT_PROPERTY_OVERRIDES += \
        ro.system.audio_active = true \
	persist.sys.spdif_setting = 0

# Define TCC set-top/media box solution
PRODUCT_PROPERTY_OVERRIDES += \
	tcc.solution.mbox = 1 \
	tcc.solution.video = 1 \
	tcc.solution.preview = 0 \
	tcc.solution.mbox.sleep = 1 \

# Can hide the statusbar
PRODUCT_PROPERTY_OVERRIDES += \
	tcc.statusbar_hide.support = 1

# this property invoke deskclock Application after the value(minutes) at Home Screen.  ( 0: disable , value : enable)
PRODUCT_PROPERTY_OVERRIDES += \
	tcc.solution.deskclock = 0

# this property don't get thumbnail image using video decoder but get static image from Resource. ( 0:displable , 1:enable)
PRODUCT_PROPERTY_OVERRIDES += \
	tcc.solution.nothumb = 1

# this property play next video preview after the value(minutes).  ( 0: disable ,  value : enable )
PRODUCT_PROPERTY_OVERRIDES += \
	tcc.solution.previewduration = 0

# Define TCC internal subtitle
PRODUCT_PROPERTY_OVERRIDES += \
	tcc.internal.subtitle = 1

# Define TCC video vsync mode
PRODUCT_PROPERTY_OVERRIDES += \
	tcc.video.vsync.support = 1 \
	tcc.video.vsync.threshold = 0 \
	tcc.video.surface.support = 1

# Define TCC video deinteralce mode, this is a sub item of vsync mode, so you have to enable vsync mode first to use deinterlace mode.
PRODUCT_PROPERTY_OVERRIDES += \
	tcc.video.deinterlace.support = 0

# Gallery is used only image
PRODUCT_PROPERTY_OVERRIDES += \
	tcc.solution.onlyimage = 1

# Define EXCLUSIVE UI
PRODUCT_PROPERTY_OVERRIDES += \
	tcc.exclusive.ui.enable = 0

# Define auto-detection mode of output 
PRODUCT_PROPERTY_OVERRIDES += \
	persist.sys.auto_detection = 0

# Define attach mode for dual output (0:none, 1:dual, 2:auto-dual, 3:hdmi/cvbs)  
PRODUCT_PROPERTY_OVERRIDES += \
	persist.sys.attach.mode = 0 

# Define properties releated STB for using files which will be release library
PRODUCT_PROPERTY_OVERRIDES += \
	tcc.display.output.stb = 1 \
	tcc.component.chip = ths8200

# Define key long press power shutdown popery 
PRODUCT_PROPERTY_OVERRIDES += \
        tcc.shutdown.active = 0

# Define screen ratio of app in portrait mode
PRODUCT_PROPERTY_OVERRIDES += \
        persist.sys.app_rotation = 1

# Define wifi & bluetooth for STB
PRODUCT_PROPERTY_OVERRIDES += \
        tcc.wifi_blue.stb = 1

# Define cec enable for STB
PRODUCT_PROPERTY_OVERRIDES += \
        ro.system.cec_active = true \
	persist.sys.hdmi_cec = 1

# Define NTP default timezone
PRODUCT_PROPERTY_OVERRIDES += \
	tcc.default.timezone = Asia/Shanghai

ifdef TARGET_BOARD_HDMI_DONGLE
PRODUCT_PROPERTY_OVERRIDES += \
	tcc.wifi.forced.enable = 1
endif

PRODUCT_PROPERTY_OVERRIDES += \
	wifi.interface=wlan0 \
	wifi.supplicant_scan_interval=15

PRODUCT_PROPERTY_OVERRIDES += \
	ro.opengles.version=131072

PRODUCT_PROPERTY_OVERRIDES += \
	ro.sf.lcd_density=160


PRODUCT_COPY_FILES += \
	$(LOCAL_KERNEL):kernel \
	device/telechips/tcc8920st/init.tcc8920st.rc:root/init.tcc8920st.rc \
	device/telechips/tcc8920st/init.tcc8920st.usb.rc:root/init.tcc8920st.usb.rc \
	device/telechips/tcc8920st/ueventd.tcc8920st.rc:root/ueventd.tcc8920st.rc \
	device/telechips/tcc8920st/init.tcc8920st.wifi.rc:root/init.tcc8920st.wifi.rc

PRODUCT_COPY_FILES += \
	device/telechips/tcc8920st/vold.fstab:/system/etc/vold.fstab

PRODUCT_COPY_FILES += \
        device/telechips/common/initlogo1280x720.rle:root/initlogo.rle \
        device/telechips/common/initlogo1280x720.rle:root/logo.rle

PRODUCT_COPY_FILES += \
	device/telechips/tcc8920st/recovery.fstab:root/factory.fstab



# Set default USB interface
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
	persist.sys.usb.config=mass_storage

# Input device calibration files
PRODUCT_COPY_FILES += \
	device/telechips/tcc8920st/tcc-ts.idc:system/usr/idc/tcc-ts.idc

# These are the hardware-specific features
PRODUCT_COPY_FILES += \
	frameworks/base/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
	frameworks/base/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \
	frameworks/base/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
	frameworks/base/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
	frameworks/base/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
	frameworks/base/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
	frameworks/base/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml \
	frameworks/base/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
	frameworks/base/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
	packages/wallpapers/LivePicker/android.software.live_wallpaper.xml:system/etc/permissions/android.software.live_wallpaper.xml \
    frameworks/base/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml

# Prebuilt kl and kcm keymaps
#PRODUCT_COPY_FILES += \
	device/telechips/tcc8920st/telechips_remote_controller.kl:system/usr/keylayout/telechips_remote_controller.kl \
	device/telechips/tcc8920st/telechips_remote_controller.kcm:system/usr/keychars/telechips_remote_controller.kcm

PRODUCT_CHARACTERISTICS := tablet,sdcard

PRODUCT_TAGS += dalvik.gc.type-precise

# Live Wallpapers
PRODUCT_PACKAGES += \
	LiveWallpapers \
	LiveWallpapersPicker \
	VisualizationWallpapers \
	PhaseBeam \
	librs_jni \
	TelechipsSystemUpdater \
	VideoPlayer	\
	Gallery

# Initial SettingScreen
PRODUCT_PACKAGES += \
        SettingScreen

# File system management packages
PRODUCT_PACKAGES += \
	setup_fs \
	make_ext4fs \
	mkmtdimg \
	quickboot

PRODUCT_PACKAGES += \
	librs_jni \
	com.android.future.usb.accessory

# AKMD8975 sensor
PRODUCT_PACKAGES += \
	akmd8975

# for bugmailer
ifneq ($(TARGET_BUILD_VARIANT),user)
	PRODUCT_PACKAGES += send_bug
	PRODUCT_COPY_FILES += \
		system/extras/bugmailer/bugmailer.sh:system/bin/bugmailer.sh \
		system/extras/bugmailer/send_bug:system/bin/send_bug
endif

$(call inherit-product, frameworks/base/build/tablet-dalvik-heap.mk)

ifdef TARGET_BOARD_HDMI_DONGLE
# Atheros Wi-Fi module
#$(call inherit-product, device/telechips/tcc892x-common/wifi/ar6003.mk)

# Realtek Wi-Fi module
$(call inherit-product, device/telechips/tcc892x-common/wifi/rt8189es.mk)
#$(call inherit-product, device/telechips/tcc892x-common/wifi/rt81xxcu.mk)
#$(call inherit-product, device/telechips/tcc892x-common/wifi/rt8188eus.mk)

# Broadcom Wi-Fi module
#$(call inherit-product, device/telechips/tcc892x-common/wifi/bcm4330.mk)
else
# Atheros Wi-Fi module
#$(call inherit-product, device/telechips/tcc892x-common/wifi/ar6003.mk)

# Realtek Wi-Fi module
$(call inherit-product, device/telechips/tcc892x-common/wifi/rt81xxcu.mk)
#$(call inherit-product, device/telechips/tcc892x-common/wifi/rt8189es.mk)
#$(call inherit-product, device/telechips/tcc892x-common/wifi/rt8188eus.mk)

# Broadcom Wi-Fi module
#$(call inherit-product, device/telechips/tcc892x-common/wifi/bcm4330.mk)
endif

# Google TV remote
#$(call inherit-product, device/telechips/common/googletv-remote.mk)
# stuff common to all Telechips TCC892x devices
$(call inherit-product, device/telechips/tcc892x-common/device_tcc892x-common.mk)
