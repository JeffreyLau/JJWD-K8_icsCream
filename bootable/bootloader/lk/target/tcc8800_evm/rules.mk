LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/tcc88xx

PLATFORM := tcc88xx

MEMBASE	:= 0x42000000
MEMSIZE := 0x00800000 # 8MB

#------------------------------------------------------------------
# Define board revision
# 0x0601 : TCC8803F_D3_16X2_SV6.0
# 0x0602 : TCC93&8803_D2_16X4_2CS_SV6.0
# 0x0610 : TCC8801F_D2_08X4_SV6.1
# 0x0612 : TCC93&8803_D2_16X4_2CS_SV6.1
# 0x0613 : TCC8801F_D3_16X2_SV6.0
# 0x0614 : TCC8803F_D2_16X4_2CS_SV6.0, TCC8803F_D2_16X4_2CS_SV6.1
# 0x0615 : TCC8801F_D3_16X2_SV6.1
# 0x0620 : TCC8801F_D2_08X4_SV6.2
# 0x0621 : TCC8801F_D2_08X4_SV6.3
# 0x0622 : TCC8803F_D3_16X2_SV6.1
# 0x0623 : TCC8801F_D3_08X4_SV6.0
# 0x0624 : TCC8803F_LPD2_32X1_SV6.0
#HW_REV=0x0500
#HW_REV=0x0600
#HW_REV=0x0601
#HW_REV=0x0602
HW_REV=0x0610
#HW_REV=0x0612
#HW_REV=0x0613
#HW_REV=0x0614
#HW_REV=0x0615
#HW_REV=0x0620
#HW_REV=0x0621
#HW_REV=0x0622
#HW_REV=0x0623
#HW_REV=0x0624

#------------------------------------------------------------------
# SDRAM CONTROLLER TYPE
#------------------------------------------------------------------
ifeq ($(HW_REV),0x0601)
 TCC_MEM_TYPE := DRAM_DDR3
else
 ifeq ($(HW_REV),0x0613)
  TCC_MEM_TYPE := DRAM_DDR3
 else
  ifeq ($(HW_REV),0x0615)
   TCC_MEM_TYPE := DRAM_DDR3
  else
   ifeq ($(HW_REV),0x0622)
    TCC_MEM_TYPE := DRAM_DDR3
   else
    ifeq ($(HW_REV),0x0623)
     TCC_MEM_TYPE := DRAM_DDR3
    else
     ifeq ($(HW_REV),0x0624)
      TCC_MEM_TYPE := DRAM_LPDDR2
     else
      TCC_MEM_TYPE := DRAM_DDR2
      #TCC_MEM_TYPE := DRAM_DDR3
      #TCC_MEM_TYPE := DRAM_LPDDR2
     endif
    endif
   endif
  endif
 endif
endif

#------------------------------------------------------------------
# Define memory size in MB
ifeq ($(HW_REV),0x0623)
 TCC_MEM_SIZE := 1024
else
 #TCC_MEM_SIZE := 256
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
# TCC88xx series
#DEFINES += CONFIG_TCC8801
#DEFINES += CONFIG_TCC8803

ifeq ($(HW_REV),0x0601)
 DEFINES += CONFIG_TCC8803
else
 ifeq ($(HW_REV),0x0602)
  DEFINES += CONFIG_TCC8803
 else
  ifeq ($(HW_REV),0x0612)
   DEFINES += CONFIG_TCC8803
  else
   ifeq ($(HW_REV),0x0614)
    DEFINES += CONFIG_TCC8803
   else
    ifeq ($(HW_REV),0x0622)
     DEFINES += CONFIG_TCC8803
    else
     ifeq ($(HW_REV),0x0624)
      DEFINES += CONFIG_TCC8803
     else
      DEFINES += CONFIG_TCC8801
     endif
    endif
   endif
  endif
 endif
endif

#------------------------------------------------------------------
# Power Managemant IC
#------------------------------------------------------------------
#DEFINES += ACT8810_PMIC
DEFINES += AXP192_PMIC


# Define if target board is STB
#TARGET_BOARD_STB := true
#UI_SIZE_1280x720 := true

BASE_ADDR       := 0x40000000

#------------------------------------------------------------------
ifeq ($(TCC_MEM_TYPE), DRAM_DDR2)
 ifeq ($(HW_REV),0x0602)
  DEFINES += CONFIG_DDR2_HY5PS1G1631CFPS6
 else
  ifeq ($(HW_REV),0x0610)
   DEFINES += CONFIG_DDR2_HY5PS1G831CFPS6
  else
   ifeq ($(HW_REV),0x0612)
    DEFINES += CONFIG_DDR2_HY5PS1G1631CFPS6
   else
    ifeq ($(HW_REV),0x0614)
     DEFINES += CONFIG_DDR2_HY5PS1G1631CFPS6
    else
     ifeq ($(HW_REV),0x0620)
      DEFINES += CONFIG_DDR2_HY5PS1G831CFPS6
     else
      ifeq ($(HW_REV),0x0621)
       DEFINES += CONFIG_DDR2_HY5PS1G831CFPS6
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
    endif
   endif
  endif
 endif

 DEFINES += DRAM_DDR2
endif

#------------------------------------------------------------------
ifeq ($(TCC_MEM_TYPE), DRAM_DDR3)
 ifeq ($(HW_REV),0x0601)
  DEFINES += CONFIG_DDR3_H5TQ2G63BFR_H9C
 else  
  ifeq ($(HW_REV),0x0613)
   DEFINES += CONFIG_DDR3_K4B2G1646C_HCK0
  else
   ifeq ($(HW_REV),0x0615)
    DEFINES += CONFIG_DDR3_H5TQ2G63BFR_H9C
   else
    ifeq ($(HW_REV),0x0622)
     DEFINES += CONFIG_DDR3_K4B2G1646C_HCK0
    else
     ifeq ($(HW_REV),0x0623)
      DEFINES += CONFIG_DDR3_H5TQ2G83BFR_H9C
     else
      DEFINES += CONFIG_DDR3_K4B2G1646C_HCK0
      #DEFINES += CONFIG_DDR3_H5TQ2G63BFR_H9C
      #DEFINES += CONFIG_DDR3_K4B1G1646E_HCH9
      #DEFINES += CONFIG_DDR3_H5TQ2G83BFR_H9C
      #DEFINES += CONFIG_DDR3_H5TQ1G83BFR_H9C
     endif
    endif
   endif
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

DEFINES += _TCC8800_

#------------------------------------------------------------------
TAGS_ADDR        := BASE_ADDR+0x00000100
KERNEL_ADDR      := BASE_ADDR+0x00008000
RAMDISK_ADDR     := BASE_ADDR+0x01000000
SCRATCH_ADDR     := BASE_ADDR+0x03000000

#------------------------------------------------------------------
# Keypad Type
#------------------------------------------------------------------
ifeq ($(HW_REV),0x0610)
 KEYS_USE_GPIO_KEYPAD := 1
else
 ifeq ($(HW_REV),0x0602)
  KEYS_USE_GPIO_KEYPAD := 1
 else
  ifeq ($(HW_REV),0x0612)
   KEYS_USE_GPIO_KEYPAD := 1
  else
   ifeq ($(HW_REV),0x0613)
    KEYS_USE_GPIO_KEYPAD := 1
   else
    ifeq ($(HW_REV),0x0614)
     KEYS_USE_GPIO_KEYPAD := 1
    else
     ifeq ($(HW_REV),0x0615)
      KEYS_USE_GPIO_KEYPAD := 1
     else
      ifeq ($(HW_REV),0x0620)
       KEYS_USE_GPIO_KEYPAD := 1
      else
       ifeq ($(HW_REV),0x0621)
        KEYS_USE_GPIO_KEYPAD := 1
       else
        ifeq ($(HW_REV),0x0622)
         KEYS_USE_GPIO_KEYPAD := 1
        else
         ifeq ($(HW_REV),0x0623)
          KEYS_USE_GPIO_KEYPAD := 1
         else
          ifeq ($(HW_REV),0x0624)
           KEYS_USE_GPIO_KEYPAD := 1
          else
           KEYS_USE_ADC_KEYPAD := 1
          endif
         endif
        endif
       endif
      endif
     endif
    endif
   endif
  endif
 endif
endif

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
 ifeq ($(HW_REV),0x0610)
 #DEFINES += DX08D11VM0AAA # 480x800
 #DEFINES += LMS350DF01    # 320x480
 DEFINES += AT070TN93      # 800x480
#DEFINES += ED090NA        # 1280x800 COBY
#DEFINES += KR080PA2S	   # 1024x768 EMDOOR 
 #DEFINES += LMS480KF       # 800x480
 else
  ifeq ($(HW_REV),0x0612)
   DEFINES += AT070TN93      # 800x480
  else
   ifeq ($(HW_REV),0x0613)
    DEFINES += AT070TN93      # 800x480
   else
    ifeq ($(HW_REV),0x0614)
     #DEFINES += LMS480KF       # 800x480
     DEFINES += AT070TN93      # 800x480
    else
     ifeq ($(HW_REV),0x0615)
      DEFINES += AT070TN93      # 800x480
     else
      ifeq ($(HW_REV),0x0620)
       DEFINES += AT070TN93      # 800x480
      else
       ifeq ($(HW_REV),0x0621)
        DEFINES += AT070TN93      # 800x480
       else
        ifeq ($(HW_REV),0x0622)
         DEFINES += AT070TN93      # 800x480
        else
         ifeq ($(HW_REV),0x0623)
          DEFINES += AT070TN93      # 800x480
         else
          ifeq ($(HW_REV),0x0624)
           DEFINES += AT070TN93      # 800x480
          else
           #DEFINES += LMS350DF01    # 320x480
           DEFINES += LMS480KF       # 800x480
           #DEFINES += DX08D11VM0AAA # 480x800
           #DEFINES += LB070WV6
           #DEFINES += CLAA104XA01CW # 1024x768
           #DEFINES += HT121WX2      # 1280x800
           #DEFINES += N101L6      # 1024x600
          endif
         endif
        endif
       endif
      endif
     endif
    endif
   endif
  endif
 endif
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

