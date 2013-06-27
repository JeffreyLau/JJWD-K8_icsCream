# -*- mode: makefile -*-
# Copyright (C) 2007 The Android Open Source Project
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
# Definitions for building the Java library and associated tests.
#

#
# Common definitions for host and target.
#

# libcore is divided into modules.
#
# The structure of each module is:
#
#   src/
#       main/               # To be shipped on every device.
#            java/          # Java source for library code.
#            native/        # C++ source for library code.
#            resources/     # Support files.
#       test/               # Built only on demand, for testing.
#            java/          # Java source for tests.
#            native/        # C++ source for tests (rare).
#            resources/     # Support files.
#
# All subdirectories are optional (hence the "2> /dev/null"s below).

define all-main-java-files-under
$(foreach dir,$(1),$(patsubst ./%,%,$(shell cd $(LOCAL_PATH) && find $(dir)/src/main/java -name "*.java" 2> /dev/null)))
endef

define all-test-java-files-under
$(foreach dir,$(1),$(patsubst ./%,%,$(shell cd $(LOCAL_PATH) && find $(dir)/src/test/java -name "*.java" 2> /dev/null)))
endef

define all-core-resource-dirs
$(shell cd $(LOCAL_PATH) && ls -d */src/$(1)/{java,resources} 2> /dev/null)
endef

# The Java files and their associated resources.
core_src_files := $(call all-main-java-files-under,dalvik dom json luni support xml)
core_resource_dirs := $(call all-core-resource-dirs,main)
test_resource_dirs := $(call all-core-resource-dirs,test)

ifeq ($(EMMA_INSTRUMENT),true)
    core_src_files += $(call all-java-files-under, ../external/emma/core ../external/emma/pregenerated)
    core_resource_dirs += ../external/emma/core/res ../external/emma/pregenerated/res
endif

local_javac_flags=-encoding UTF-8
#local_javac_flags+=-Xlint:all -Xlint:-serial,-deprecation,-unchecked
local_javac_flags+=-Xmaxwarns 9999999

#
# Build for the target (device).
#

# Definitions to make the core library.

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(core_src_files)
LOCAL_JAVA_RESOURCE_DIRS := $(core_resource_dirs)

LOCAL_NO_STANDARD_LIBRARIES := true
LOCAL_JAVACFLAGS := $(local_javac_flags)
LOCAL_DX_FLAGS := --core-library

LOCAL_NO_EMMA_INSTRUMENT := true
LOCAL_NO_EMMA_COMPILE := true
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := core

include $(BUILD_JAVA_LIBRARY)

core-intermediates := ${intermediates}


# Make core-junit
include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(call all-main-java-files-under,junit)
LOCAL_NO_STANDARD_LIBRARIES := true
LOCAL_JAVA_LIBRARIES := core
LOCAL_JAVACFLAGS := $(local_javac_flags)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := core-junit
include $(BUILD_JAVA_LIBRARY)

# Make the core-tests library.
include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(call all-test-java-files-under,dalvik dom json luni support xml)
LOCAL_JAVA_RESOURCE_DIRS := $(test_resource_dirs)
LOCAL_NO_STANDARD_LIBRARIES := true
LOCAL_JAVA_LIBRARIES := bouncycastle core core-junit
LOCAL_STATIC_JAVA_LIBRARIES := sqlite-jdbc mockwebserver
LOCAL_JAVACFLAGS := $(local_javac_flags)
LOCAL_MODULE_TAGS := tests
LOCAL_MODULE := core-tests
LOCAL_NO_EMMA_INSTRUMENT := true
LOCAL_NO_EMMA_COMPILE := true
include $(BUILD_STATIC_JAVA_LIBRARY)

# This one's tricky. One of our tests needs to have a
# resource with a "#" in its name, but Perforce doesn't
# allow us to submit such a file. So we create it here
# on-the-fly.
TMP_RESOURCE_DIR := $(intermediates.COMMON)/tmp/
TMP_RESOURCE_FILE := org/apache/harmony/luni/tests/java/lang/test\#.properties

$(TMP_RESOURCE_DIR)$(TMP_RESOURCE_FILE):
	@mkdir -p $(dir $@)
	@echo "Hello, world!" > $@

$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_EXTRA_JAR_ARGS := $(extra_jar_args) -C "$(TMP_RESOURCE_DIR)" "$(TMP_RESOURCE_FILE)"
$(LOCAL_INTERMEDIATE_TARGETS): $(TMP_RESOURCE_DIR)$(TMP_RESOURCE_FILE)


#
# Build for the host.
#

ifeq ($(WITH_HOST_DALVIK),true)

    # Definitions to make the core library.

    include $(CLEAR_VARS)

    LOCAL_SRC_FILES := $(core_src_files)
    LOCAL_JAVA_RESOURCE_DIRS := $(core_resource_dirs)

    LOCAL_NO_STANDARD_LIBRARIES := true
    LOCAL_JAVACFLAGS := $(local_javac_flags)
    LOCAL_DX_FLAGS := --core-library

    LOCAL_NO_EMMA_INSTRUMENT := true
    LOCAL_NO_EMMA_COMPILE := true
    LOCAL_BUILD_HOST_DEX := true

    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE := core-hostdex

    include $(BUILD_HOST_JAVA_LIBRARY)

    # Make core-junit
    include $(CLEAR_VARS)
    LOCAL_SRC_FILES := $(call all-main-java-files-under,junit)
    LOCAL_NO_STANDARD_LIBRARIES := true
    LOCAL_JAVA_LIBRARIES := core-hostdex
    LOCAL_JAVACFLAGS := $(local_javac_flags)
    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE := core-junit-hostdex
    LOCAL_BUILD_HOST_DEX := true
    include $(BUILD_HOST_JAVA_LIBRARY)

    # Make the core-tests library.
    include $(CLEAR_VARS)
    LOCAL_SRC_FILES := $(call all-test-java-files-under,dalvik dom json luni support xml)
    LOCAL_JAVA_RESOURCE_DIRS := $(test_resource_dirs)
    LOCAL_NO_STANDARD_LIBRARIES := true
    LOCAL_JAVA_LIBRARIES := bouncycastle-hostdex core-hostdex core-junit-hostdex
    LOCAL_STATIC_JAVA_LIBRARIES := sqlite-jdbc-host mockwebserver-hostdex
    LOCAL_JAVACFLAGS := $(local_javac_flags)
    LOCAL_MODULE_TAGS := tests
    LOCAL_MODULE := core-tests-hostdex
    LOCAL_NO_EMMA_INSTRUMENT := true
    LOCAL_NO_EMMA_COMPILE := true
    LOCAL_BUILD_HOST_DEX := true
    include $(BUILD_HOST_JAVA_LIBRARY)
endif

#
# Local droiddoc for faster libcore testing
#
#
# Run with:
#     m libcore-docs
#
# Main output:
#     out/target/common/docs/libcore/reference/packages.html
#
# All text for proofreading (or running tools over):
#     out/target/common/docs/libcore-proofread.txt
#
# TODO list of missing javadoc, etc:
#     out/target/common/docs/libcore-docs-todo.html
#
# Rerun:
#     rm -rf out/target/common/docs/libcore-timestamp && m libcore-docs
#
include $(CLEAR_VARS)

# for shared defintion of libcore_to_document
include $(LOCAL_PATH)/Docs.mk

LOCAL_SRC_FILES:=$(call libcore_to_document,$(LOCAL_PATH))
# rerun doc generation without recompiling the java
LOCAL_JAVA_LIBRARIES:=
LOCAL_JAVACFLAGS := $(local_javac_flags)
LOCAL_MODULE_CLASS:=JAVA_LIBRARIES

LOCAL_MODULE := libcore

LOCAL_DROIDDOC_OPTIONS:= \
 -offlinemode \
 -title "libcore" \
 -proofread $(OUT_DOCS)/$(LOCAL_MODULE)-proofread.txt \
 -todo ../$(LOCAL_MODULE)-docs-todo.html \
 -hdf android.whichdoc offline

LOCAL_DROIDDOC_CUSTOM_TEMPLATE_DIR:=build/tools/droiddoc/templates-sdk

include $(BUILD_DROIDDOC)
