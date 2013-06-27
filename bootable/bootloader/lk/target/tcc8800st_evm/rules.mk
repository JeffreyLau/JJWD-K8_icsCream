LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/tcc88xx

PLATFORM := tcc88xx

MEMBASE	:= 0x42000000
MEMSIZE := 0x00800000 # 8MB

#------------------------------------------------------------------
# Define board revision
# 0x0320 : TCC8801_STB
HW_REV=0x0320

#------------------------------------------------------------------
# SDRAM CONTROLLER TYPE
#------------------------------------------------------------------
TCC_MEM_TYPE := DRAM_DDR2
#TCC_MEM_TYPE := DRAM_DDR3
#TCC_MEM_TYPE := DRAM_LPDDR2

#------------------------------------------------------------------
# Define memory size in MB
#TCC_MEM_SIZE := 256
TCC_MEM_SIZE := 512
#TCC_MEM_SIZE := 1024

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
#DEFINES += AXP192_PMIC

# Define if target board is STB
TARGET_BOARD_STB := true

BASE_ADDR := 0x40000000

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
 ifeq ($(TCC_MEM_SIZE), 512)
  DEFINES += CONFIG_DDR3_K4B2G1646C_HCK0
  #DEFINES += CONFIG_DDR3_H5TQ2G63BFR_H9C
 else
  DEFINES += CONFIG_DDR3_K4B1G1646E_HCH9
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
ifeq ($(TARGET_BOARD_STB), true)
DEFINES += TARGET_BOARD_STB
DEFINES += BOARD_TCC880X_STB_DEMO
DEFINES += TCC_HDMI_UI_SIZE_1280_720
DISP_DEFINES := DISPLAY_STB
else
DISP_DEFINES := DISPLAY_NORMAL
endif

#------------------------------------------------------------------
# Support video displaying by hw vsyn interrupt
#------------------------------------------------------------------
TCC_VIDEO_DISPLAY_BY_VSYNC_INT := true
#TCC_VIDEO_DISPLAY_BY_VSYNC_INT := false

ifeq ($(TCC_VIDEO_DISPLAY_BY_VSYNC_INT), true)
DEFINES += TCC_VIDEO_DISPLAY_BY_VSYNC_INT
endif

#------------------------------------------------------------------

DEFINES += _TCC8800ST_

#------------------------------------------------------------------
TAGS_ADDR        := BASE_ADDR+0x00000100
KERNEL_ADDR      := BASE_ADDR+0x00008000
RAMDISK_ADDR     := BASE_ADDR+0x01000000
SCRATCH_ADDR     := BASE_ADDR+0x03000000

#------------------------------------------------------------------
# Keypad Type
#------------------------------------------------------------------
KEYS_USE_GPIO_KEYPAD := 1
#KEYS_USE_ADC_KEYPAD := 1

#------------------------------------------------------------------
MODULES += \
	dev/keys \
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

#------------------------------------------------------------------
# Defines Default Display
ifeq ($(DISP_DEFINES), DISPLAY_STB)
DEFINES += DEFAULT_DISPLAY_OUTPUT_DUAL
else
DEFINES += DEFAULT_DISPLAY_LCD
endif

#------------------------------------------------------------------
# Defines Display Type
ifeq ($(DISP_DEFINES), DISPLAY_STB)
DEFINES += DISPLAY_STB_DUAL
#DEFINES += DISPLAY_STB_SINGLE
#DEFINES += DISPLAY_STB_AUTO_DETECT
endif

#------------------------------------------------------------------
# Defines for Dual UI Display
ifeq ($(DISP_DEFINES), DISPLAY_STB)
#DEFINES += DISPLAY_OUTPUT_DUAL_UI
endif

#------------------------------------------------------------------
# Defined for component chip
ifeq ($(DISP_DEFINES), DISPLAY_STB)
DEFINES += COMPONENT_CHIP_THS8200
else
DEFINES += COMPONENT_CHIP_CS4954
endif

# Define Default Splash
ifeq ($(DISP_DEFINES), DISPLAY_STB)
DEFINES += DISPLAY_SPLASH_SCREEN_DIRECT=1
else
DEFINES += DISPLAY_SPLASH_SCREEN=1
DEFINES += DISPLAY_SPLASH_SCREEN_DIRECT=1
endif

#------------------------------------------------------------------
# Define EXCLUSIVE UI
ifeq ($(DISP_DEFINES), DISPLAY_STB)
#DEFINES += TCC_EXCLUSIVE_UI_SUPPORT
endif

#------------------------------------------------------------------
DEFINES += FB_TCC_OVERLAY_EXT

# Defines LCD panel
ifeq ($(DISP_DEFINES), DISPLAY_STB)
DEFINES += HDMI_1280X720 # 1280x720
else
#DEFINES += AT070TN93      # 800x480
#DEFINES += LMS350DF01    # 320x480
DEFINES += LMS480KF       # 800x480
#DEFINES += DX08D11VM0AAA # 480x800
#DEFINES += LB070WV6
#DEFINES += CLAA104XA01CW # 1024x768
#DEFINES += HT121WX2      # 1280x800
#DEFINES += N101L6      # 1024x600
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

#------------------------------------------------------------------
# Defines Remocon
BOOT_REMOCON := 0
ifeq ($(BOOT_REMOCON), 1)
#DEFINES += CONFIG_DARE_IR
DEFINES += CONFIG_CS_2000_IR_X_CANVAS
#DEFINES += CONFIG_NOVATEK_IR
endif


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

