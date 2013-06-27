LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/tcc892x

PLATFORM := tcc892x

MEMBASE	:= 0x82000000
MEMSIZE := 0x00800000 # 8MB

#------------------------------------------------------------------
# Define board revision
# 0x1000 : TCC8920_D2_08X4_SV01 - DDR2 512MB(32Bit)
# 0x1001 : TCC8920_D2_08X4_SV01 - DDR2 1024MB(32Bit)
# 0x1002 : TCC8920_D3_08X4_SV01 - DDR3 512MB(32Bit)
# 0x1003 : TCC8920_D3_08X4_SV01 - DDR3 1024MB(32Bit)
# 0x1004 : TCC8920_D3_08X4_SV01 - DDR3 512MB(16Bit)
# 0x1005 : TCC8920_D3_16X4_2CS_SV01 - DDR3 1024MB (32Bit)
# 0x1006 : TCC8925_D3_08X4_2CS_SV01 - DDR3 1024MB (16Bit)
# 0x1007 : TCC8920_D3_08X4_SV60 - DDR3 1024MB(32Bit)
# 0x1008 : TCC8923_D3_08X4_SV01 - DDR3 1024MB(32Bit)
#HW_REV=0x1000
#HW_REV=0x1001
#HW_REV=0x1002
#HW_REV=0x1003
#HW_REV=0x1004
#HW_REV=0x1005
#HW_REV=0x1006
HW_REV=0x1007
#HW_REV=0x1008

#------------------------------------------------------------------
# SDRAM CONTROLLER TYPE
#------------------------------------------------------------------
ifneq ($(filter $(HW_REV),0x1000 0x1001),)
 TCC_MEM_TYPE := DRAM_DDR2
else
 ifneq ($(filter $(HW_REV),0x1002 0x1003 0x1004 0x1005 0x1006 0x1007 0x1008),)
  TCC_MEM_TYPE := DRAM_DDR3
 else
  #TCC_MEM_TYPE := DRAM_LPDDR2
  TCC_MEM_TYPE := DRAM_DDR2
  #TCC_MEM_TYPE := DRAM_DDR3
 endif
endif

#------------------------------------------------------------------
# Define memory bus width

ifneq ($(filter $(HW_REV),0x1004 0x1006),)
 DEFINES += CONFIG_DRAM_16BIT_USED
else
 DEFINES += CONFIG_DRAM_32BIT_USED
endif


#------------------------------------------------------------------
# Define memory size in MB
ifneq ($(filter $(HW_REV),0x1000 0x1002 0x1004),)
 TCC_MEM_SIZE := 512
else
 ifneq ($(filter $(HW_REV),0x1001 0x1003 0x1005 0x1006 0x1007 0x1008),)
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
# GPU Scaling Option
#------------------------------------------------------------------
DEFINES += CONFIG_GPU_BUS_SCALING

#------------------------------------------------------------------
# Power Managemant IC
#------------------------------------------------------------------
#DEFINES += ACT8810_PMIC
DEFINES += AXP192_PMIC
#DEFINES += RN5T614_PMIC

# Define if target board is STB
#TARGET_BOARD_STB := true
#UI_SIZE_1280x720 := true

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
  ifneq ($(filter $(HW_REV),0x1003 0x1004),)
   DEFINES += CONFIG_DDR3_H5TQ2G83BFR_H9C
  else
   ifneq ($(filter $(HW_REV),0x1005),)
    DEFINES += CONFIG_DDR3_H5TQ2G63BFR_PBC
   else
    ifneq ($(filter $(HW_REV),0x1006 0x1007 0x1008),)
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
 DEFINES += TCC_HDMI_UI_SIZE_1280_720
 DISP_DEFINES := DISPLAY_DUAL
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
ifeq ($(UI_SIZE_1280x720),true)
DEFINES += HDMI_1280X720
else
 #DEFINES += DX08D11VM0AAA # 480x800
 #DEFINES += LMS350DF01    # 320x480
 DEFINES += AT070TN93      # 800x480
 #DEFINES += ED090NA       # 1280x800 COBY
 #DEFINES += KR080PA2S     # 1024x768 EMDOOR 
 #DEFINES += LMS480KF      # 800x480
 #DEFINES += CLAA070NP01   # 1024_600 
 #DEFINES += HV070WSA   # 1024_600 

endif

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
#TCC_GPSB_USE := 1
#TCC_HDMI_USE := 1
TCC_PCA953X_USE := 1

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

