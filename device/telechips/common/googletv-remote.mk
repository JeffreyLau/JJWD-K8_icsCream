#
# Copyright (C) 2012 Telechips, Inc.  All rights reserved.
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

# Prebuilt kl and kcm keymaps for Google TV remote
PRODUCT_COPY_FILES += \
	device/telechips/common/anymote.kl:system/usr/keylayout/anymote.kl \
	device/telechips/common/anymote.kcm:system/usr/keychars/anymote.kcm

# Prebuilt JNI library for Google TV remote
PRODUCT_COPY_FILES += \
	device/telechips/common/libinputbridge.so:system/lib/libinputbridge.so

PRODUCT_PACKAGES += \
	TelechipsTvRemote
