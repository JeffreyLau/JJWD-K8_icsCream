LOCAL_DIR := $(GET_LOCAL_DIR)

OBJS += \
	$(LOCAL_DIR)/aboot.Ao \

ifeq ($(TCC_NAND_USE), 1)
OBJS += \
	$(LOCAL_DIR)/recovery.o
endif

ifeq ($(TCC_USB_USE), 1)
OBJS += \
	$(LOCAL_DIR)/fastboot.o
endif
