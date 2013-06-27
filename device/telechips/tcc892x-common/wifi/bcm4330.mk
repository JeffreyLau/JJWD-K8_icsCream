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

PRODUCT_COPY_FILES += \
	$(LOCAL_PATH)/iwconfig:system/bin/iwconfig \
	$(LOCAL_PATH)/iwlist:system/bin/iwlist

PRODUCT_COPY_FILES += \
	$(LOCAL_PATH)/broadcom/bcm4330/bcm4330.ko:system/wifi/bcm4330.ko \
	$(LOCAL_PATH)/broadcom/bcm4330/bcmdhd.cal:system/wifi/bcmdhd.cal

$(call inherit-product-if-exists, hardware/broadcom/wlan/bcmdhd/firmware/bcm4330/device-bcm.mk)