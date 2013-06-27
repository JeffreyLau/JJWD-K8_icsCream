LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/tcc88xx

PLATFORM := tcc88xx

MEMBASE	:= 0x42000000
MEMSIZE := 0x00800000 # 8MB

#------------------------------------------------------------------
# Define board revision
# 0x0001 = TCC8801_M803_D3 16x2 V0.1 HYNIX DDR3, HYNIX NAND
# 0x0002 = TCC8801_M803_D3 08x4 V0.1 HYNIX DDR3, HYNIX NAND
# 0x0012 = TCC8801_M803_D3 16x2 V0.2A HYNIX DDR3(512MB), HYNIX NAND
# 0x0013 = TCC8801_M803_D3 08x4 V0.2A HYNIX DDR3(1024MB), HYNIX NAND
#HW_REV=0x0001
#HW_REV=0x0002
HW_REV=0x0012
#HW_REV=0x0013

#------------------------------------------------------------------
# SDRAM CONTROLLER TYPE
#------------------------------------------------------------------
#TCC_MEM_TYPE := DRAM_DDR2
TCC_MEM_TYPE := DRAM_DDR3
#TCC_MEM_TYPE := DRAM_LPDDR2

#------------------------------------------------------------------
# Define memory size in MB
ifneq ($(filter $(HW_REV), 0x0001 0x0012),)
 TCC_MEM_SIZE := 512
else
 ifneq ($(filter $(HW_REV), 0x0002 0x0013),)
  TCC_MEM_SIZE := 1024
 else
  #TCC_MEM_SIZE := 256
  TCC_MEM_SIZE := 512
  #TCC_MEM_SIZE := 1024
 endif
endif

ifeq ($(TCC_MEM_SIZE), 256)
DEFINES += CONFIG_TCC_MEM_256MB
endif
ifeq ($(TCC_MEM_SIZE), 512)
DEFINES += CONFIG_TCC_MEM_512MB
endif
ifeq ($(TCC_MEM_SIZE), 1024)
DEFINES += CONFIG_TCC_MEM_1024MB
endif

#------------------------------------------------------------------
# TCC88xx series
DEFINES += CONFIG_TCC8801
#DEFINES += CONFIG_TCC8803

#------------------------------------------------------------------
# Power Managemant IC
#------------------------------------------------------------------
#DEFINES += ACT8810_PMIC
DEFINES += AXP192_PMIC


# Define if target board is STB
#TARGET_BOARD_STB := true

BASE_ADDR       := 0x40000000

#------------------------------------------------------------------
ifeq ($(TCC_MEM_TYPE), DRAM_DDR2)
#DEFINES += CONFIG_DDR2_HXB18T2G160AF
#DEFINES += CONFIG_DDR2_HY5PS1G1631CFPS6
DEFINES += CONFIG_DDR2_HY5PS1G831CFPS6
#DEFINES += CONFIG_DDR2_K4T1G164QE_HCF7
#DEFINES += CONFIG_DDR2_K4T1G084QF_BCE7
#DEFINES += CONFIG_DDR2_CT83486C1
#DEFINES += CONFIG_DDR2_MT47H256M8EB25E

DEFINES += DRAM_DDR2
endif

#------------------------------------------------------------------
ifeq ($(TCC_MEM_TYPE), DRAM_DDR3)
 ifneq ($(filter $(HW_REV), 0x0001 0x0012),)
  #DEFINES += CONFIG_DDR3_H5TQ2G63BFR_H9C
  DEFINES += CONFIG_DDR3_K4B2G1646C_HCK0
 else
  ifneq ($(filter $(HW_REV), 0x0002, 0x0013),)
   DEFINES += CONFIG_DDR3_H5TQ2G83BFR_H9C
  else
   #DEFINES += CONFIG_DDR3_K4B2G1646C_HCK0
   #DEFINES += CONFIG_DDR3_K4B1G1646E_HCH9
   DEFINES += CONFIG_DDR3_H5TQ2G63BFR_H9C
   #DEFINES += CONFIG_DDR3_H5TQ2G83BFR_H9C
   #DEFINES += CONFIG_DDR3_H5TQ1G83BFR_H9C
  endif
 endif

DEFINES += DRAM_DDR3
endif

#------------------------------------------------------------------
ifeq ($(TCC_MEM_TYPE), DRAM_LPDDR2)
 ifeq ($(HW_REV),0x0624)
  DEFINES += CONFIG_LPDDR2_K4P4G324EB_AGC1
 endif

 DEFINES += DRAM_LPDDR2
endif

#------------------------------------------------------------------
DISP_DEFINES := DISPLAY_NORMAL

#------------------------------------------------------------------
# Support video displaying by hw vsyn interrupt
#------------------------------------------------------------------
TCC_VIDEO_DISPLAY_BY_VSYNC_INT := true
#TCC_VIDEO_DISPLAY_BY_VSYNC_INT := false

ifeq ($(TCC_VIDEO_DISPLAY_BY_VSYNC_INT), true)
DEFINES += TCC_VIDEO_DISPLAY_BY_VSYNC_INT
endif

#------------------------------------------------------------------

DEFINES += _M803_

#------------------------------------------------------------------
TAGS_ADDR        := BASE_ADDR+0x00000100
KERNEL_ADDR      := BASE_ADDR+0x00008000
RAMDISK_ADDR     := BASE_ADDR+0x01000000
SCRATCH_ADDR     := BASE_ADDR+0x03000000

KEYS_USE_GPIO_KEYPAD := 1
#KEYS_USE_ADC_KEYPAD := 1

MODULES += \
	dev/keys \
	dev/pmic \
	lib/ptable

DEFINES += \
	SDRAM_SIZE=$(MEMSIZE) \
	MEMBASE=$(MEMBASE) \
	BASE_ADDR=$(BASE_ADDR) \
	TAGS_ADDR=$(TAGS_ADDR) \
	KERNEL_ADDR=$(KERNEL_ADDR) \
	RAMDISK_ADDR=$(RAMDISK_ADDR) \
	SCRATCH_ADDR=$(SCRATCH_ADDR) \
	TCC_MEM_SIZE=$(TCC_MEM_SIZE) \
	HW_REV=$(HW_REV)

# Defines debug level
DEBUG := 1

DEFINES += DEFAULT_DISPLAY_LCD

# Define Default Splash
ifeq ($(DISP_DEFINES), DISPLAY_DUAL)
DEFINES += DISPLAY_SPLASH_SCREEN_DIRECT=1
else
DEFINES += DISPLAY_SPLASH_SCREEN=1
DEFINES += DISPLAY_SPLASH_SCREEN_DIRECT=1
endif

#DEFINES += USE_LVDS

DEFINES += FB_TCC_OVERLAY_EXT

# Defines LCD panel
#DEFINES += LMS350DF01 # 320x480
#DEFINES += LMS480KF # 800x480
#DEFINES += DX08D11VM0AAA # 480x800
#DEFINES += LB070WV6
#DEFINES += CLAA104XA01CW # 1024x768
#DEFINES += HT121WX2 # 1280x800
#DEFINES += AT070TN93 # 800 * 480 M801
#DEFINES += TD070RDH11 # 800 * 480 M801
ifneq ($(filter $(HW_REV), 0x0012 0x0013),)
DEFINES += CLAA102NA0DCW # 102 * 600 M803
else
DEFINES += N101L6     # 1024 * 600  M803
endif

#------------------------------------------------------------------
#DEFINES += LOW_CLOCK_BOOT

#------------------------------------------------------------------

OBJS += \
	$(LOCAL_DIR)/init.o \
	$(LOCAL_DIR)/clock.o \
	$(LOCAL_DIR)/gpio.o \
	$(LOCAL_DIR)/keypad.o \
	$(LOCAL_DIR)/atags.o


TCC_I2C_USE := 1
TCC_LCD_USE := 1
TCC_NAND_USE := 1
TCC_FWDN_USE := 1
TCC_USB_USE := 1
TCC_GPSB_USE := 1
TCC_HDMI_USE := 1

ifeq ($(TCC_I2C_USE), 1)
  DEFINES += TCC_I2C_USE
endif
ifeq ($(TCC_LCD_USE), 1)
  DEFINES += TCC_LCD_USE
endif
ifeq ($(TCC_NAND_USE), 1)
  DEFINES += TCC_NAND_USE
endif
ifeq ($(TCC_FWDN_USE), 1)
  DEFINES += TCC_FWDN_USE
endif
ifeq ($(TCC_USB_USE), 1)
  DEFINES += TCC_USB_USE
endif
ifeq ($(TCC_GPSB_USE), 1)
  DEFINES += TCC_GPSB_USE
endif
ifeq ($(TCC_HDMI_USE), 1)
  DEFINES += TCC_HDMI_USE
endif

