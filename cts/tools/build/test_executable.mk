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

#
# Builds an executable and defines a rule to generate the associated test
# package XML needed by CTS.
#
# 1. Replace "include $(BUILD_EXECUTABLE)"
#    with "include $(BUILD_CTS_EXECUTABLE)"
#
# 2. Define LOCAL_CTS_TEST_PACKAGE to group the tests under a package
#    as needed by CTS.
#

include $(BUILD_EXECUTABLE)

cts_executable_xml := $(CTS_NATIVE_XML_OUT)/$(LOCAL_MODULE).xml

$(cts_executable_xml): PRIVATE_PATH := $(LOCAL_PATH)
$(cts_executable_xml): PRIVATE_TEST_PACKAGE := $(LOCAL_CTS_TEST_PACKAGE)
$(cts_executable_xml): PRIVATE_EXECUTABLE := $(LOCAL_MODULE)
$(cts_executable_xml): $(addprefix $(LOCAL_PATH)/,$(LOCAL_SRC_FILES)) | $(CTS_NATIVE_XML_GENERATOR)
	$(hide) echo Generating test description for native package $(PRIVATE_TEST_PACKAGE)
	$(hide) $(CTS_NATIVE_XML_GENERATOR) -p $(PRIVATE_TEST_PACKAGE) \
			-n $(PRIVATE_EXECUTABLE) \
			-s $(PRIVATE_PATH) \
			-o $@
