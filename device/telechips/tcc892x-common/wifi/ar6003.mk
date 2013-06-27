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

$(call inherit-product, device/telechips/tcc892x-common/wifi/atheros.mk)

PRODUCT_COPY_FILES += \
	$(LOCAL_PATH)/atheros/ar6003/hw2.1.1/athtcmd_ram.bin:/system/etc/firmware/ath6k/AR6003/hw2.1.1/athtcmd_ram.bin \
	$(LOCAL_PATH)/atheros/ar6003/hw2.1.1/athwlan.bin:/system/etc/firmware/ath6k/AR6003/hw2.1.1/athwlan.bin \
	$(LOCAL_PATH)/atheros/ar6003/hw2.1.1/bdata.bin:/system/etc/firmware/ath6k/AR6003/hw2.1.1/bdata.bin \
	$(LOCAL_PATH)/atheros/ar6003/hw2.1.1/fw-3.bin:/system/etc/firmware/ath6k/AR6003/hw2.1.1/fw-3.bin \
	$(LOCAL_PATH)/atheros/ar6003/hw2.1.1/nullTestFlow.bin:/system/etc/firmware/ath6k/AR6003/hw2.1.1/athwlan_tablet.bin \
	$(LOCAL_PATH)/atheros/ar6003/hw2.1.1/utf.bin:/system/etc/firmware/ath6k/AR6003/hw2.1.1/utf.bin \
	$(LOCAL_PATH)/atheros/ar6003/hw2.0/otp.bin.z77:/system/etc/firmware/ath6k/AR6003/hw2.0/otp.bin.z77 \
	$(LOCAL_PATH)/atheros/ar6003/hw2.0/device.bin:/system/etc/firmware/ath6k/AR6003/hw2.0/device.bin \
	$(LOCAL_PATH)/atheros/ar6003/hw2.0/data.patch.hw2_0.bin:/system/etc/firmware/ath6k/AR6003/hw2.0/data.patch.hw2_0.bin \
	$(LOCAL_PATH)/atheros/ar6003/hw2.0/bdata.SD31.bin:/system/etc/firmware/ath6k/AR6003/hw2.0/bdata.SD31.bin \
	$(LOCAL_PATH)/atheros/ar6003/hw2.0/athwlan.bin.z77:/system/etc/firmware/ath6k/AR6003/hw2.0/athwlan.bin.z77 \
	$(LOCAL_PATH)/atheros/ar6003/hw2.0/athtcmd_ram.bin:/system/etc/firmware/ath6k/AR6003/hw2.0/athtcmd_ram.bin \
	$(LOCAL_PATH)/atheros/ar6003/hw2.0/data.patch.bin:/system/etc/firmware/ath6k/AR6003/hw2.0/data.patch.bin \
	$(LOCAL_PATH)/atheros/ar6003/ath6kl_sdio.ko:/system/wifi/ath6kl_sdio.ko \
	$(LOCAL_PATH)/atheros/ar6003/cfg80211.ko:/system/wifi/cfg80211.ko
