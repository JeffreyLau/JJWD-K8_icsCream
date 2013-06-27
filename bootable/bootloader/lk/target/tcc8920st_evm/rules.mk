LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/tcc892x

PLATFORM := tcc892x

MEMBASE	:= 0x82000000
MEMSIZE := 0x00800000 # 8MB

#------------------------------------------------------------------
# Define board revision
# 0x1000 : TCC892X_STB_D2_08X4_SV01 - DDR2 512MB(32BIT)
# 0x1001 : TCC892x_STB_D2_08X4_SV01 - DDR2 1024MB(32BIT)
# 0x1002 : TCC892X_STB_D3_08X4_SV01 - DDR3 512MB(32BIT)
# 0x1003 : TCC892X_STB_D3_08X4_SV01 - DDR3 1024MB(32BIT)
# 0x1004 : TCC892X_STB_D3_08X2_SV01 - DDR3 512MB(16Bit)
# 0x2000 : TCC8925_HDMI_DONGLE - DDR3 512MB(16Bit)
# 0x2001 : TCC8925_HDB892F_1CS_SV0.1 - DDR3 512MB(16Bit)
# 0x2002 : TCC8925_HDB892F_2CS_SV0.1 - DDR3 1024MB(16Bit)
# 0x2003 : TCC8925_ISDBT_MODULE - DDR3 512MB(16Bit)
# 0x2004 : TCC8925_HDMI_DONGLE_EXT_DCDC - DDR3 512MB(16Bit)

#HW_REV=0x1000
#HW_REV=0x1001
HW_REV=0x1002
#HW_REV=0x1003
#HW_REV=0x1004
#HW_REV=0x2000
#HW_REV=0x2001
#HW_REV=0x2002
#HW_REV=0x2003
#HW_REV=0x2004

#------------------------------------------------------------------
# SDRAM CONTROLLER TYPE
#------------------------------------------------------------------
ifneq ($(filter $(HW_REV),0x1000 0x1001),)
 TCC_MEM_TYPE := DRAM_DDR2
else
 ifneq ($(filter $(HW_REV),0x1002 0x1003 0x1004 0x2000 0x2001 0x2002 0x2003 0x2004),)
  TCC_MEM_TYPE := DRAM_DDR3
 else
  #TCC_MEM_TYPE := DRAM_LPDDR2
  TCC_MEM_TYPE := DRAM_DDR2
  #TCC_MEM_TYPE := DRAM_DDR3
 endif
endif

#------------------------------------------------------------------
# Define memory bus width

ifneq ($(filter $(HW_REV),0x1004 0x2000 0x2001 0x2002 0x2003 0x2004),)
 DEFINES += CONFIG_DRAM_16BIT_USED
else
 DEFINES += CONFIG_DRAM_32BIT_USED
endif


#------------------------------------------------------------------
# Define memory size in MB
ifneq ($(filter $(HW_REV),0x1000 0x1002 0x1004 0x2000 0x2001 0x2003 0x2004),)
 TCC_MEM_SIZE := 512
else
 ifneq ($(filter $(HW_REV),0x1001 0x1003 0x2002),)
  TCC_MEM_SIZE := 1024
 else
  TCC_MEM_SIZE := 512
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
# Define target board
ifneq ($(filter $(HW_REV),0x2000 0x2004),)
 DEFINES += TARGET_TCC8925_STB_DONGLE
endif

ifneq ($(filter $(HW_REV),0x2001 0x2002),)
 DEFINES += TARGET_TCC8925_HDB892F
endif

ifneq ($(filter $(HW_REV),0x2003),)
 DEFINES += TARGET_TCC8925_ISDBT_MODULE
endif

#------------------------------------------------------------------
# Power Managemant IC
#------------------------------------------------------------------
ifneq ($(filter $(HW_REV),0x2000 0x2004),)
 DEFINES += RN5T614_PMIC
endif


# Define if target board is STB
TARGET_BOARD_STB := true
DEFINES += BOARD_TCC892X_STB_DEMO

BASE_ADDR       := 0x80000000

#------------------------------------------------------------------
ifeq ($(TCC_MEM_TYPE), DRAM_DDR2)
 ifneq ($(filter $(HW_REV),0x1000),)
  DEFINES += CONFIG_DDR2_HY5PS1G831CFPS6
 else
  ifneq ($(filter $(HW_REV),0x1001),)
   DEFINES += CONFIG_DDR2_HY5PS1G831CFPS6 #should be change to fit some memory..
  else
   #DEFINES += CONFIG_DDR2_HXB18T2G160AF
   #DEFINES += CONFIG_DDR2_HY5PS1G1631CFPS6
   DEFINES += CONFIG_DDR2_HY5PS1G831CFPS6
   #DEFINES += CONFIG_DDR2_K4T1G164QE_HCF7
   #DEFINES += CONFIG_DDR2_K4T1G084QF_BCE7
   #DEFINES += CONFIG_DDR2_CT83486C1
   #DEFINES += CONFIG_DDR2_MT47H256M8EB25E
  endif
 endif

 DEFINES += DRAM_DDR2
endif

#------------------------------------------------------------------
ifeq ($(TCC_MEM_TYPE), DRAM_DDR3)
 ifneq ($(filter $(HW_REV),0x1002),)
   DEFINES += CONFIG_DDR3_H5TQ1G83BFR_H9C
 else
  ifneq ($(filter $(HW_REV),0x1003 0x1004 0x2000 0x2004),)
   DEFINES += CONFIG_DDR3_H5TQ2G83BFR_H9C
  else  
    ifneq ($(filter $(HW_REV), 0x2001 0x2002 0x2003),)
     DEFINES += CONFIG_DDR3_H5TQ2G83BFR_PBC
    else
     DEFINES += CONFIG_DDR3_K4B2G1646C_HCK0
     #DEFINES += CONFIG_DDR3_H5TQ2G63BFR_H9C
     #DEFINES += CONFIG_DDR3_K4B1G1646E_HCH9
     #DEFINES += CONFIG_DDR3_H5TQ2G83BFR_H9C
     #DEFINES += CONFIG_DDR3_H5TQ2G83BFR_PBC
     #DEFINES += CONFIG_DDR3_H5TQ1G83BFR_H9C
     #DEFINES += CONFIG_DDR3_IN5CB256M8BN_CG
     #DEFINES += CONFIG_DDR3_NT5CB256M8GN_CG
     #DEFINES += CONFIG_DDR3_H5TC4G83MFR_H9A
  endif
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
ifeq ($(TARGET_BOARD_STB),true)
 DEFINES += TARGET_BOARD_STB
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
DEFINES += DISPLAY_STB_SINGLE
#DEFINES += DISPLAY_STB_DUAL
#DEFINES += DISPLAY_STB_AUTO_DETECT
#DEFINES += DISPLAY_STB_ATTACH_DUAL
#DEFINES += DISPLAY_STB_ATTACH_DUAL_AUTO
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
#Define Factory Reset
ifeq ($(DISP_DEFINES), DISPLAY_STB)
#DEFINES += TCC_FACTORY_RESET_SUPPORT
endif

#------------------------------------------------------------------
DEFINES += FB_TCC_OVERLAY_EXT

# Defines LCD panel
ifeq ($(DISP_DEFINES), DISPLAY_STB)
DEFINES += HDMI_1280X720
else
 #DEFINES += DX08D11VM0AAA # 480x800
 #DEFINES += LMS350DF01    # 320x480
 DEFINES += AT070TN93      # 800x480
 #DEFINES += LMS480KF       # 800x480
endif

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
#TCC_GPSB_USE := 1
TCC_HDMI_USE := 1
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

