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

PRODUCT_PACKAGES += \
    libhostapd_client \
    hostapd \
    hostapd_wps \
    hostapd_cli \
    hostapd.conf

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/realtek/rt8189es/wlan.ko:system/wifi/wlan.ko \
    $(LOCAL_PATH)/realtek/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf \
    $(LOCAL_PATH)/realtek/android_dhcpcd.conf:system/etc/dhcpcd.conf \
    $(LOCAL_PATH)/iwconfig:system/bin/iwconfig \
    $(LOCAL_PATH)/iwlist:system/bin/iwlist
