LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/tcc892x

PLATFORM := tcc892x

MEMBASE	:= 0x82000000
MEMSIZE := 0x00800000 # 8MB

#------------------------------------------------------------------
# Define board revision
# 0x2000 = TCC8803_M805S_D3_08x4_V0.1(Change to TCC89 - DDR3_512MB  (32Bit)
# 0x2001 = TCC8803/8923_M805S_D3_08x4_V1.1            - DDR3_1024MB (32Bit)
# 0x2002 = TCC8925_M805S_D3_08x4_2CS_V0.1             - DDR3_1024MB (16Bit)
# 0x2003 = TCC8925_M805S_D3_08X4_2CS_V1.2 2012.03.15  - DDR3_1024MB (16Bit)
# 0x2004 = TCC8925_M805S_D3_08x2_4L_V0.1A             - DDR3_512MB  (16Bit)
# 0x2005 = TCC8925_M805S_D3_08X4_2CS_V1.4A 2012.05.08 - DDR3_1024MB (16Bit)
#HW_REV=0x2000
#HW_REV=0x2001
HW_REV=0x2002
#HW_REV=0x2003
#HW_REV=0x2004
#HW_REV=0x2005

#------------------------------------------------------------------
ifneq ($(filter $(HW_REV), 0x2000 0x2001),)
DEFINES += _M805_8923_
DEFINES += _TCC8923_0XA_
else
DEFINES += _M805_8925_
endif

#------------------------------------------------------------------
# SDRAM CONTROLLER TYPE
#------------------------------------------------------------------
#TCC_MEM_TYPE := DRAM_DDR2
TCC_MEM_TYPE := DRAM_DDR3
#TCC_MEM_TYPE := DRAM_LPDDR2

#------------------------------------------------------------------
# Define memory bus width

ifneq ($(filter $(HW_REV),0x2002 0x2003 0x2004 0x2005),)
 DEFINES += CONFIG_DRAM_16BIT_USED
else
 DEFINES += CONFIG_DRAM_32BIT_USED
endif

#------------------------------------------------------------------
# Define memory size in MB
ifneq ($(filter $(HW_REV), 0x2001 0x2002 0x2003 0x2005),)
 TCC_MEM_SIZE := 1024
else
 TCC_MEM_SIZE := 512
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
# TCC892x series
#DEFINES += CONFIG_TCC8923
#DEFINES += CONFIG_TCC8925

#------------------------------------------------------------------
# GPU Scaling Option
#------------------------------------------------------------------
DEFINES += CONFIG_GPU_BUS_SCALING

#------------------------------------------------------------------
# Power Managemant IC
#------------------------------------------------------------------
#DEFINES += ACT8810_PMIC

ifneq ($(filter $(HW_REV),0x2002 0x2003 0x2004 0x2005),)
DEFINES += RN5T614_PMIC
else
  ifneq ($(filter $(HW_REV),0x2000 0x2001),)
  DEFINES += AXP192_PMIC
  else
  #Used Discrete Power Board
  endif
endif


# Define if target board is STB
#TARGET_BOARD_STB := true
#UI_SIZE_1280x720 := true

BASE_ADDR       := 0x80000000

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
  ifneq ($(filter $(HW_REV),0x2001 0x2002 0x2003 0x2004 0x2005),)
    DEFINES += CONFIG_DDR3_H5TQ2G83BFR_PBC
  else
    ifneq ($(filter $(HW_REV),0x2000),)
      DEFINES += CONFIG_DDR3_H5TQ1G83BFR_H9C
     #DEFINES += CONFIG_DDR3_H5TQ2G83BFR_H9C
    else
      DEFINES += CONFIG_DDR3_K4B2G1646C_HCK0
     #DEFINES += CONFIG_DDR3_K4B1G1646E_HCH9
     #DEFINES += CONFIG_DDR3_H5TQ2G63BFR_H9C
     #DEFINES += CONFIG_DDR3_H5TQ2G83BFR_H9C
     #DEFINES += CONFIG_DDR3_H5TQ1G83BFR_H9C
    endif
  endif

  DEFINES += DRAM_DDR3
endif

#------------------------------------------------------------------
ifeq ($(TCC_MEM_TYPE), DRAM_LPDDR2)
 DEFINES += CONFIG_LPDDR2_K4P4G324EB_AGC1

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

DEFINES += FB_TCC_OVERLAY_EXT

# Defines LCD panel
#DEFINES += LMS350DF01 # 320x480
#DEFINES += LMS480KF # 800x480
#DEFINES += DX08D11VM0AAA # 480x800
#DEFINES += LB070WV6
#DEFINES += CLAA104XA01CW # 1024x768
#DEFINES += HT121WX2 # 1280x800
DEFINES += AT070TN93 # 800 * 480 M801
#DEFINES += TD070RDH11 # 800 * 480 M801
#DEFINES += N101L6     # 1024 * 600  M803
#DEFINES += CLAA070NP01 # 1024 * 600 COBY

#------------------------------------------------------------------
#DEFINES += LOW_CLOCK_BOOT

#------------------------------------------------------------------
OBJS += \
	$(LOCAL_DIR)/init.o \
	$(LOCAL_DIR)/clock.o \
	$(LOCAL_DIR)/gpio.o \
	$(LOCAL_DIR)/keypad.o \
	$(LOCAL_DIR)/atags.o \
	$(LOCAL_DIR)/battery.o


TCC_I2C_USE := 1
TCC_LCD_USE := 1
TCC_NAND_USE := 1
TCC_FWDN_USE := 1
TCC_USB_USE := 1
#TCC_GPSB_USE := 1
#TCC_HDMI_USE := 1
#TCC_PCA953X_USE := 1

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
ifeq ($(TCC_PCA953X_USE), 1)
  DEFINES += TCC_PCA953X_USE
endif

