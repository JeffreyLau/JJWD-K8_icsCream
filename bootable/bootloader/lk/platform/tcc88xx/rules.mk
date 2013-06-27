LOCAL_DIR := $(GET_LOCAL_DIR)

ARCH := arm
ARM_CPU := cortex-a8
CPU := generic

INCLUDES += -I$(LOCAL_DIR)/include 

DEFINES += \
	WITH_CPU_EARLY_INIT=1

DEFINES += TCC88XX=1

DEFINES += \
	_LINUX_ \
	NAND_BOOT_REV \
	FWDN_V7

ifneq ($(EMMC_BOOT),1)
################################################
##NAND
################################################
DEFINES += NAND_INCLUDE 
DEFINES += NAND_BOOT_INCLUDE 
DEFINES += INTERNAL_HIDDEN_STORAGE_INCLUDE
else
################################################
##SDMMC DEFINES
################################################
DEFINES += BOOTSD_INCLUDE
DEFINES += BOOTSD_BOOT_INCLUDE
DEFINES += BOOTSD_KERNEL_INCLUDE
#DEFINES += FEATURE_SDMMC_MMC43_BOOT
endif

MODULES += dev/fbcon
		
OBJS += \
	$(LOCAL_DIR)/cpu_early_init.Ao \
	$(LOCAL_DIR)/platform.o \
	$(LOCAL_DIR)/interrupts.Ao \
	$(LOCAL_DIR)/tcc_ddr.Ao \
	$(LOCAL_DIR)/clock.Ao \
	$(LOCAL_DIR)/gpio.o \
	$(LOCAL_DIR)/i2c.o \
	$(LOCAL_DIR)/lcdc.o \
	$(LOCAL_DIR)/tcc_lcd_interface.o


ifeq ($(TCC_HDMI_USE), 1)
OBJS += \
	$(LOCAL_DIR)/hdmi.o
endif

ifeq ($(BOOT_REMOCON), 1)
OBJS += \
	$(LOCAL_DIR)/tcc_asm.Ao \
	$(LOCAL_DIR)/pm.Ao
endif

LINKER_SCRIPT += $(BUILDDIR)/system-onesegment.ld

ifneq ($(EMMC_BOOT),1)

ifeq ($(TCC_NAND_USE), 1)
ifeq ($(TNFTL_V8_USE),1)
LIBS += \
		$(LOCAL_DIR)/libs/libtnftl_v8_TCC880X_BL.lo
else
LIBS += \
		$(LOCAL_DIR)/libs/libtnftl_V7081_TCC8800_BL.lo 
#		$(LOCAL_DIR)/libx/libkfs_TCC8800/.lo
endif
endif

endif

include platform/tcc_shared/rules.mk