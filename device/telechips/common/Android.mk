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

LOCAL_PATH := $(call my-dir)

ifeq ($(filter-out tcc92xx tcc93xx tcc88xx tcc892x tcc891x,$(TARGET_BOARD_PLATFORM)),)

include $(CLEAR_VARS)
LOCAL_MODULE := flash_eraseall
LOCAL_SRC_FILES := flash_eraseall
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE_CLASS := EXECUTABLES
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := mem
LOCAL_SRC_FILES := mem
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE_CLASS := EXECUTABLES
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := TelechipsTvRemote
LOCAL_SRC_FILES := $(LOCAL_MODULE).apk
LOCAL_MODULE_CLASS := APPS
LOCAL_MODULE_TAGS := optional
LOCAL_CERTIFICATE := platform
LOCAL_MODULE_SUFFIX := $(COMMON_ANDROID_PACKAGE_SUFFIX)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := quickboot
LOCAL_SRC_FILES := quickboot
LOCAL_MODULE_TAGS := eng debug optional
LOCAL_MODULE_CLASS := EXECUTABLES
include $(BUILD_PREBUILT)

include $(call first-makefiles-under,$(LOCAL_PATH))

endif
