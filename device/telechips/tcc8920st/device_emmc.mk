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

DEVICE_PACKAGE_OVERLAYS := device/telechips/tcc8920st/overlay_emmc

PRODUCT_COPY_FILES += \
	device/telechips/tcc8920st/init.tcc8920st.emmc.rc:root/init.tcc8920st.fs.rc \
	device/telechips/tcc8920st/vold.emmc.fstab:/system/etc/vold.fstab \
	device/telechips/tcc8920st/ueventd.tcc8920st.emmc.rc:root/ueventd.tcc8920st.rc

PRODUCT_COPY_FILES += \
	device/telechips/tcc8920st/recovery.emmc.fstab:root/factory.fstab


$(call inherit-product, device/telechips/tcc8920st/device_base.mk)