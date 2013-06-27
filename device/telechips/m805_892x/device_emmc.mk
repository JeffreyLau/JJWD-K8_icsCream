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

DEVICE_PACKAGE_OVERLAYS := device/telechips/m805_892x/overlay_emmc

PRODUCT_COPY_FILES += \
	device/telechips/m805_892x/init.m805_892x.emmc.rc:root/init.m805_892x.fs.rc \
	device/telechips/m805_892x/vold.emmc.fstab:/system/etc/vold.fstab \
	device/telechips/m805_892x/ueventd.m805_892x.emmc.rc:root/ueventd.m805_892x.rc

PRODUCT_COPY_FILES += \
	device/telechips/m805_892x/recovery.emmc.fstab:root/factory.fstab

$(call inherit-product, device/telechips/m805_892x/device_base.mk)
