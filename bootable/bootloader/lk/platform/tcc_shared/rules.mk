LOCAL_DIR := $(GET_LOCAL_DIR)

DEFINES += PLATFORM_TCC=1
DEFINES += ANDROID_BOOT=1
DEFINES += WITH_DMA_ZONE=1

DMA_SIZE := 0x00800000 # 8MB

INCLUDES += -I$(LOCAL_DIR)/include -I$(LOCAL_DIR)/include/fwdn -I$(LOCAL_DIR)/include/tnftl -I$(LOCAL_DIR)/include/tcc_remocon

OBJS += \
	$(LOCAL_DIR)/timer.o \
	$(LOCAL_DIR)/debug.o \
	$(LOCAL_DIR)/uart.o \
	$(LOCAL_DIR)/adc.Ao \
	$(LOCAL_DIR)/pca953x.o \
	$(LOCAL_DIR)/component_cs4954.o \
	$(LOCAL_DIR)/component_ths8200.o \
	$(LOCAL_DIR)/tca_tco.o

ifeq ($(TCC_GPSB_USE), 1)
OBJS += \
	$(LOCAL_DIR)/gpsb.o
endif

ifeq ($(TCC_USB_USE), 1)
OBJS += \
	$(LOCAL_DIR)/otgcore.o \
	$(LOCAL_DIR)/otgdev_io.o \
	$(LOCAL_DIR)/usb_device.o \
	$(LOCAL_DIR)/usbdev_class.o \
	$(LOCAL_DIR)/usbphy.o \
	$(LOCAL_DIR)/vtc.o \
	$(LOCAL_DIR)/fastbootusb.o \
	$(LOCAL_DIR)/hsusb.o \
	$(LOCAL_DIR)/usb_manager.o
endif

ifeq ($(TCC_NAND_USE), 1)
OBJS += \
	$(LOCAL_DIR)/nand.o \
	$(LOCAL_DIR)/TC_File.o \
	$(LOCAL_DIR)/disk.o \
	$(LOCAL_DIR)/FSAPP.o
endif

ifeq ($(TCC_FWDN_USE), 1)
OBJS += \
	$(LOCAL_DIR)/fwdn_drv_v7.o \
	$(LOCAL_DIR)/fwdn_main.Ao \
	$(LOCAL_DIR)/fwdn_nand.o \
	$(LOCAL_DIR)/fwdn_protocol_v7.o
endif

ifeq ($(TCC_LCD_USE), 1)
OBJS += \
	$(LOCAL_DIR)/lcd_lms480kf01.o \
	$(LOCAL_DIR)/lcd_tw8816.o \
	$(LOCAL_DIR)/lcd_dx08d11vm0aaa.o \
	$(LOCAL_DIR)/lcd_lms350df01.o \
	$(LOCAL_DIR)/lcd_td043mgeb1.o \
	$(LOCAL_DIR)/lcd_td070rdh.o \
	$(LOCAL_DIR)/lcd_at070tn93.o \
	$(LOCAL_DIR)/lcd_claa104xa01cw.o\
	$(LOCAL_DIR)/lcd_n101l6.o\
	$(LOCAL_DIR)/lcd_ht121wx2.o\
	$(LOCAL_DIR)/lcd_claa102na0dcw.o \
	$(LOCAL_DIR)/lcd_ED090NA.o\
	$(LOCAL_DIR)/lcd_kr080pa2s.o\
	$(LOCAL_DIR)/lcd_hv070wsa.o\
	$(LOCAL_DIR)/lcd_claa070np01.o
endif

ifeq ($(TCC_HDMI_USE), 1)
OBJS += \
	$(LOCAL_DIR)/hdmi_1280x720.o
endif

ifeq ($(BOOT_REMOCON), 1)	
OBJS+= \
	$(LOCAL_DIR)/tca_remocon.o \
	$(LOCAL_DIR)/tcc_remocon.o
endif

ifeq ($(EMMC_BOOT),1)
OBJS+= \
	$(LOCAL_DIR)/emmc.o \
	$(LOCAL_DIR)/fwupgrade.o \
	$(LOCAL_DIR)/sd_memory.o \
	$(LOCAL_DIR)/sd_hw.o \
	$(LOCAL_DIR)/sd_bus.o \
	$(LOCAL_DIR)/sd_slot.o \
	$(LOCAL_DIR)/sd_update.o \
	$(LOCAL_DIR)/fwdn_sdmmc.o \
	$(LOCAL_DIR)/update.o \
	$(LOCAL_DIR)/partition.o
endif

ifneq ($(EMMC_BOOT),1)
ifeq ($(TCC_NAND_USE), 1)
ifeq ($(TNFTL_V8_USE), 1)
OBJS+= \
	$(LOCAL_DIR)/partition.o

	include ../../../hardware/telechips/nand_v8/Makefile
else
	include ../../../hardware/telechips/nand/Makefile
endif
endif
endif
include platform/tcc_shared/tools/Makefile

