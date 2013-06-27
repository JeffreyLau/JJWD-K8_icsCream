
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := mkmtdimg.c
LOCAL_STATIC_LIBRARIES := libmincrypt

LOCAL_CFLAGS += -DPAGE_SIZE=$(BOARD_FLASH_PAGE_SIZE)

LOCAL_MODULE := mkmtdimg

LOCAL_MODULE_TAGS := optional

include $(BUILD_HOST_EXECUTABLE)

$(call dist-for-goals,droid,$(LOCAL_BUILT_MODULE))
