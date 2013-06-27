ifeq ($(filter-out tcc92xx tcc93xx tcc88xx tcc892x tcc891x,$(TARGET_BOARD_PLATFORM)),)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := recovery_updater.c firmware.c bootloader.c

LOCAL_STATIC_LIBRARIES += libmtdutils

LOCAL_C_INCLUDES += bootable/recovery

LOCAL_MODULE := librecovery_updater_telechips

include $(BUILD_STATIC_LIBRARY)

endif
