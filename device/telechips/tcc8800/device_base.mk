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

DEVICE_PACKAGE_OVERLAYS += device/telechips/tcc8800/overlay

PRODUCT_AAPT_CONFIG := hdpi mdpi

PRODUCT_COPY_FILES := \
	$(LOCAL_KERNEL):kernel \
	device/telechips/tcc8800/init.tcc8800.rc:root/init.tcc8800.rc \
	device/telechips/tcc8800/init.tcc8800.usb.rc:root/init.tcc8800.usb.rc \
	device/telechips/tcc8800/init.tcc8800.wifi.rc:root/init.tcc8800.wifi.rc

# Prebuilt kl keymaps
PRODUCT_COPY_FILES += \
	$(LOCAL_PATH)/tcc8801-matrix-key.kl:system/usr/keylayout/tcc8801-matrix-key.kl

# Generated kcm keymaps
PRODUCT_PACKAGES := \
	tcc8801-matrix-key.kcm

PRODUCT_PROPERTY_OVERRIDES := \
	ro.QB.enable = 1 \
	wifi.interface=wlan0 \
	wifi.supplicant_scan_interval=15

PRODUCT_PROPERTY_OVERRIDES += \
        ro.system.hdmi_max_resolution = fullhd \
	persist.sys.renderer_onthefly = true

PRODUCT_PROPERTY_OVERRIDES += \
        ro.system.hdmi_active = true

PRODUCT_PROPERTY_OVERRIDES += \
        ro.system.composite_active = true \
        persist.sys.composite_resize_x = 3 \
	persist.sys.composite_resize_y = 3

PRODUCT_PROPERTY_OVERRIDES += \
        ro.system.component_active = false \
        tcc.output.component_mode = 0

PRODUCT_PROPERTY_OVERRIDES += \
	tcc.video.surface.support = 1 \
	tcc.video.vsync.support = 1

# Define NTP default timezone
PRODUCT_PROPERTY_OVERRIDES += \
	tcc.default.timezone = Asia/Shanghai

PRODUCT_PROPERTY_OVERRIDES += \
        ro.system.hdmi_portable = true \
	ro.system.osd_active = false

PRODUCT_PROPERTY_OVERRIDES += \
        ro.system.audio_active = false \


PRODUCT_PROPERTY_OVERRIDES += \
	persist.sys.strictmode.disable = true \
	persist.sys.strictmode.visual = false

# Set default USB interface
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
	persist.sys.usb.config=mass_storage

# Live Wallpapers
PRODUCT_PACKAGES += \
	LiveWallpapers \
	LiveWallpapersPicker \
	VisualizationWallpapers \
	PhaseBeam \
	librs_jni \
	Camera	\
	Gallery2

# Input device calibration files
PRODUCT_COPY_FILES += \
	device/telechips/tcc8800/tcc-ts.idc:system/usr/idc/tcc-ts.idc

PRODUCT_COPY_FILES += \
	device/telechips/tcc8800/tcc-ts-sitronix.idc:system/usr/idc/tcc-ts-sitronix.idc

PRODUCT_COPY_FILES += \
	device/telechips/tcc8800/tcc-ts-goodix-cap.idc:system/usr/idc/tcc-ts-goodix-cap.idc

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
    #frameworks/base/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml

#ifeq ($(TARGET_HAVE_TSLIB),true)
PRODUCT_COPY_FILES += \
         $(LOCAL_PATH)/../../../external/tslib/ts.conf:system/ts.conf
#endif

PRODUCT_PROPERTY_OVERRIDES += \
	ro.opengles.version=131072

PRODUCT_PROPERTY_OVERRIDES += \
	ro.sf.lcd_density=120

PRODUCT_PROPERTY_OVERRIDES += \
	hwui.render_dirty_regions=false

PRODUCT_TAGS += dalvik.gc.type-precise

PRODUCT_PACKAGES += \
	librs_jni \
	com.android.future.usb.accessory

#ifeq ($(TARGET_HAVE_TSLIB),true)
PRODUCT_PACKAGES += \
        TSCalibration \
        libtslib \
        inputraw \
        pthres \
        dejitter \
        linear
#endif

# AKMD8975 sensor
PRODUCT_PACKAGES += \
	akmd8975

PRODUCT_COPY_FILES += \
	device/telechips/common/initlogo800x480.rle:root/initlogo.rle \
	device/telechips/common/initlogo800x480.rle:root/logo.rle
#	device/telechips/common/initlogo1280x720.rle:root/initlogo.rle

# for bugmailer
ifneq ($(TARGET_BUILD_VARIANT),user)
	PRODUCT_PACKAGES += send_bug
	PRODUCT_COPY_FILES += \
		system/extras/bugmailer/bugmailer.sh:system/bin/bugmailer.sh \
		system/extras/bugmailer/send_bug:system/bin/send_bug
endif

# telechips System apk 
PRODUCT_PACKAGES +=\
		   TelechipsSystemUpdater

#filesystem management
PRODUCT_PACKAGES +=\
		   setup_fs\
		   make_ext4fs\
		   mkmtdimg \
		   quickboot

$(call inherit-product, frameworks/base/build/tablet-dalvik-heap.mk)

# Atheros Wi-Fi module
#$(call inherit-product, device/telechips/tcc88xx-common/wifi/ar6002.mk)
$(call inherit-product, device/telechips/tcc88xx-common/wifi/ar6003.mk)

# Realtek Wi-Fi module
#$(call inherit-product, device/telechips/tcc88xx-common/wifi/rt81xxcu.mk)
#$(call inherit-product, device/telechips/tcc88xx-common/wifi/rt8189es.mk)

# stuff common to all Telechips TCC88xx devices
$(call inherit-product, device/telechips/tcc88xx-common/device_tcc88xx-common.mk)
