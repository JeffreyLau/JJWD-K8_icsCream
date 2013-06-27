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

DEVICE_PACKAGE_OVERLAYS := device/telechips/tcc8800/overlay_v7

PRODUCT_COPY_FILES += \
	device/telechips/tcc8800/init.tcc8800.v7.rc:root/init.tcc8800.fs.rc \
	device/telechips/tcc8800/vold.v7.fstab:/system/etc/vold.fstab \
	device/telechips/tcc8800/ueventd.tcc8800.v7.rc:root/ueventd.tcc8800.rc

PRODUCT_COPY_FILES += \
	device/telechips/tcc8800/recovery.fstab:root/factory.fstab


$(call inherit-product, device/telechips/tcc8800/device_base.mk)
