#
# libbluedroid
#

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	bluetooth.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	system/bluetooth/bluez-clean-headers

LOCAL_SHARED_LIBRARIES := \
	libcutils

LOCAL_MODULE := libbluedroid

ifeq ($(BOARD_HAVE_BT_MODULE),true)
    LOCAL_CFLAGS += -DHAVE_BT_MODULE
endif

include $(BUILD_SHARED_LIBRARY)
