# Copyright (C) 2011 The Android Open Source Project
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

# Include this file to gain access to functions to build native CTS
# test packages. Replace "include $(BUILD_EXECUTABLE)" with
# "include $(BUILD_CTS_EXECUTABLE)".

LOCAL_PATH := $(call my-dir)
BUILD_CTS_EXECUTABLE := $(LOCAL_PATH)/tools/build/test_executable.mk

CTS_NATIVE_XML_OUT := $(HOST_OUT)/cts-native-xml

CTS_NATIVE_XML_GENERATOR := $(HOST_OUT_EXECUTABLES)/cts-native-xml-generator

define cts-get-native-paths
	$(foreach exe,$(1),$(call intermediates-dir-for,EXECUTABLES,$(exe))/$(exe))
endef

define cts-get-native-xmls
	$(foreach exe,$(1),$(CTS_NATIVE_XML_OUT)/$(exe).xml)
endef
