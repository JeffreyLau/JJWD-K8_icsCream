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

PRODUCT_COPY_FILES := \
	device/telechips/tcc892x-common/bluetooth/key_921600.psr:system/key_921600.psr

PRODUCT_COPY_FILES += \
 vendor/telechips/proprietary/tcc892x/ba_svc:system/bin/ba_svc

ifeq ($(BOARD_ANTENNA_SHARING_BT_WIFI), true)
PRODUCT_COPY_FILES += \
	device/telechips/tcc892x-common/bluetooth/key_3000000coex.psr:system/key_3000000.psr
else
 ifeq ($(BOARD_BT_HOST_WAKE_UP), true)
 PRODUCT_COPY_FILES += \
	 device/telechips/tcc892x-common/bluetooth/key_3000000hwake.psr:system/key_3000000.psr
 else
 PRODUCT_COPY_FILES += \
	 device/telechips/tcc892x-common/bluetooth/key_3000000.psr:system/key_3000000.psr
 endif
endif

PRODUCT_COPY_FILES += \
	device/telechips/tcc892x-common/bluetooth/bcm4330.hcd:system/bcm4330.hcd

#for bluetooth
PRODUCT_PACKAGES += \
	avinfo \
	hciconfig \
	hcitool \
	l2ping \
	bccmd \
	bcmtool \
	rfcomm \
	audio.a2dp.default

# Prebuilted NTFS driver modules
#
PRODUCT_COPY_FILES += \
	device/telechips/tcc892x-common/ufsd.ko:root/lib/modules/ufsd.ko

# Prebuilt NTFS tools
#
PRODUCT_COPY_FILES += \
	device/telechips/tcc892x-common/mkntfs:system/bin/mkntfs \
	device/telechips/tcc892x-common/chkntfs:system/bin/chkntfs

# VPU(Video Manager/Encoder/Decoder)
# for TCC8925s
#PRODUCT_COPY_FILES += \
        $(LOCAL_PATH)/vpu_8925s_manager.ko:system/lib/modules/vpu_manager.ko \
        $(LOCAL_PATH)/vpu_8925s_decoder.ko:system/lib/modules/vpu_decoder.ko \
        $(LOCAL_PATH)/vpu_8925s_decoder_ext.ko:system/lib/modules/vpu_decoder_ext.ko \
        $(LOCAL_PATH)/vpu_8925s_encoder.ko:system/lib/modules/vpu_encoder.ko
# for TCC892x
PRODUCT_COPY_FILES += \
	$(LOCAL_PATH)/vpu_8920_manager.ko:system/lib/modules/vpu_manager.ko \
	$(LOCAL_PATH)/vpu_8920_decoder.ko:system/lib/modules/vpu_decoder.ko \
	$(LOCAL_PATH)/vpu_8920_decoder_ext.ko:system/lib/modules/vpu_decoder_ext.ko \
	$(LOCAL_PATH)/vpu_8920_encoder.ko:system/lib/modules/vpu_encoder.ko

# DXB(Telechips DTV Broadcasting) LinuxTV Driver
#
#PRODUCT_COPY_FILES += \
	$(LOCAL_PATH)/dxb/tcc_dxb_drv.ko:system/lib/modules/tcc_dxb_drv.ko

# DXB(Telechips DTV Broadcasting) LinuxTV Driver
#
PRODUCT_COPY_FILES += \
	$(LOCAL_PATH)/dxb/tcc_dxb_drv.ko:system/lib/modules/tcc_dxb_drv.ko

# Media Profiles
PRODUCT_COPY_FILES += \
	device/telechips/tcc892x-common/media_profiles.xml:system/etc/media_profiles.xml

#tinyalsa utilities
PRODUCT_PACKAGES += \
		    tinyplay \
		    tinycap \
		    tinymix
		    
# stuff common to all Telechips devices
$(call inherit-product, hardware/telechips/common/tcc892x.mk)
$(call inherit-product-if-exists, vendor/telechips/proprietary/tcc-vendor.mk)
$(call inherit-product, device/telechips/common/common.mk)
