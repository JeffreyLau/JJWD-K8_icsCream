#
# Copyright (C) 2007 The Android Open Source Project
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

# This is a generic phone product that isn't specialized for a specific device.
# It includes the base Android platform.

PRODUCT_POLICY := android.policy_phone

PRODUCT_PACKAGES := \
    DeskClock \
    AlarmProvider \
    Bluetooth \
    Calculator \
    Calendar \
    CertInstaller \
    DrmProvider \
    Email \
    Exchange \
    LatinIME \
    Launcher2 \
    Music \
    MusicFX \
    Provision \
    Phone \
    QuickSearchBox \
    Settings \
    Sync \
    SystemUI \
    Updater \
    CalendarProvider \
    SyncProvider \
    bluetooth-health \
    hostapd \
    wpa_supplicant.conf

PRODUCT_PACKAGES += \
    icu.dat

PRODUCT_PACKAGES += \
    librs_jni \
    libvideoeditor_jni \
    libvideoeditorplayer \
    libvideoeditor_core

PRODUCT_PACKAGES += \
    audio.primary.default \
    audio_policy.default

PRODUCT_COPY_FILES := \
        system/bluetooth/data/audio.conf:system/etc/bluetooth/audio.conf \
        system/bluetooth/data/auto_pairing.conf:system/etc/bluetooth/auto_pairing.conf \
        system/bluetooth/data/blacklist.conf:system/etc/bluetooth/blacklist.conf \
        system/bluetooth/data/input.conf:system/etc/bluetooth/input.conf \
        system/bluetooth/data/network.conf:system/etc/bluetooth/network.conf \
        frameworks/base/media/libeffects/data/audio_effects.conf:system/etc/audio_effects.conf

$(call inherit-product-if-exists, frameworks/base/data/fonts/fonts.mk)
$(call inherit-product-if-exists, external/lohit-fonts/fonts.mk)
$(call inherit-product-if-exists, frameworks/base/data/keyboards/keyboards.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/core.mk)

# Overrides
PRODUCT_BRAND := generic
PRODUCT_DEVICE := generic
PRODUCT_NAME := generic_no_telephony
