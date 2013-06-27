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

TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := false
TARGET_ARCH_VARIANT := armv7-a-neon
ARCH_ARM_HAVE_TLS_REGISTER := true

TARGET_BOARD_PLATFORM := tcc892x
TARGET_BOARD_PLATFORM_GPU := arm-mali400

# Change vpu libraries copied for TCC892x or TCC8925S on device_tcc892x-common.mk.
TARGET_BOARD_SOC := tcc892x
#TARGET_BOARD_SOC := tcc8925s

BOARD_KERNEL_BASE := 0x80000000

ifneq ($(filter userdebug user,$(TARGET_BUILD_VARIANT)),)
BOARD_KERNEL_CMDLINE := console=null
else
BOARD_KERNEL_CMDLINE := console=ttyTCC0,115200n8 androidboot.console=ttyTCC0
endif

BOARD_EGL_CFG := device/telechips/tcc892x-common/egl.cfg

USE_OPENGL_RENDERER := true

