LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:=                                      \
                  BandwidthController.cpp              \
                  CommandListener.cpp                  \
                  DnsProxyListener.cpp                 \
                  NatController.cpp                    \
                  NetdCommand.cpp                      \
                  NetlinkHandler.cpp                   \
                  NetlinkManager.cpp                   \
                  PanController.cpp                    \
                  PppController.cpp                    \
                  ResolverController.cpp               \
                  SecondaryTableController.cpp         \
                  TetherController.cpp                 \
                  ThrottleController.cpp               \
                  oem_iptables_hook.cpp                \
                  logwrapper.c                         \
                  main.cpp                             \

LOCAL_CFLAGS :=

ifeq ($(BOARD_USES_ATH_WIFI), true)
    LOCAL_SRC_FILES += AthSoftapController.cpp
else ifeq ($(BOARD_USES_REALTEK_WIFI), true)
    LOCAL_SRC_FILES += SoftapController_realtek.cpp 
else ifeq ($(BOARD_USES_BROADCOM_WIFI), true)
    LOCAL_SRC_FILES += SoftapController.cpp 
    LOCAL_CFLAGS += -DFIRMWARE_PATH
ifdef WIFI_DRIVER_FW_PATH_PARAM
    LOCAL_CFLAGS += -DWIFI_DRIVER_FW_PATH_PARAM=\"$(WIFI_DRIVER_FW_PATH_PARAM)\"
endif
ifdef WIFI_DRIVER_FW_PATH_STA
    LOCAL_CFLAGS += -DWIFI_DRIVER_FW_PATH_STA=\"$(WIFI_DRIVER_FW_PATH_STA)\"
endif
endif

LOCAL_MODULE:= netd

LOCAL_C_INCLUDES := $(KERNEL_HEADERS) \
                    $(LOCAL_PATH)/../bluetooth/bluedroid/include \
                    $(LOCAL_PATH)/../bluetooth/bluez-clean-headers \
                    external/openssl/include \
                    external/stlport/stlport \
                    $(LOCAL_PATH)/../core/libnetutils \
                    bionic \
                    $(call include-path-for, libhardware_legacy)/hardware_legacy

LOCAL_SHARED_LIBRARIES := libstlport libsysutils libcutils libnetutils \
                          libcrypto libhardware_legacy

ifneq ($(BOARD_HOSTAPD_DRIVER),)
  LOCAL_CFLAGS += -DHAVE_HOSTAPD
ifeq ($(WLAN_VENDOR), 2)
  LOCAL_CFLAGS += -DATH_WLAN
endif
endif

ifeq ($(BOARD_HAVE_BLUETOOTH),true)
  LOCAL_SHARED_LIBRARIES := $(LOCAL_SHARED_LIBRARIES) libbluedroid
  LOCAL_CFLAGS := $(LOCAL_CFLAGS) -DHAVE_BLUETOOTH
endif

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:=          \
                  ndc.c \

LOCAL_MODULE:= ndc

LOCAL_C_INCLUDES := $(LOCAL_C_INCLUDES) $(KERNEL_HEADERS) 

LOCAL_CFLAGS := 

LOCAL_SHARED_LIBRARIES := libcutils

include $(BUILD_EXECUTABLE)
