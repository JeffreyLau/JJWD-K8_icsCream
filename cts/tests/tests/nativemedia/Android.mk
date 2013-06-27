# Build the unit tests.

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES:= \
    bionic \
    bionic/libstdc++/include \
    external/gtest/include \
    system/media/wilhelm/include \
    external/stlport/stlport \
    system/media/wilhelm/src/ut

LOCAL_SRC_FILES:= \
    src/SLObjectCreationTest.cpp

LOCAL_SHARED_LIBRARIES := \
  libutils \
  libOpenSLES \
  libstlport

LOCAL_STATIC_LIBRARIES := \
    libOpenSLESUT \
    libgtest

LOCAL_CFLAGS += -DXP_UNIX

LOCAL_MODULE := CtsNativeMediaTestCases

LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/nativetest

LOCAL_CTS_TEST_PACKAGE := android.nativemedia
include $(BUILD_CTS_EXECUTABLE)
