LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/msm_shared

PLATFORM := msm7k

MEMBASE := 0x00000000 # SMI
MEMSIZE := 0x00100000 # 1MB

BASE_ADDR        := 0x00200000

TAGS_ADDR        := BASE_ADDR+0x00000100
KERNEL_ADDR      := BASE_ADDR+0x00008000
RAMDISK_ADDR     := BASE_ADDR+0x01000000
SCRATCH_ADDR     := BASE_ADDR+0x02008000

KEYS_USE_GPIO_KEYPAD := 1

DEFINES += DISPLAY_TYPE_MDDI=0
DEFINES += DISPLAY_TYPE_LCDC=1
DEFINES += DISPLAY_SPLASH_SCREEN=0

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
	SCRATCH_ADDR=$(SCRATCH_ADDR)


OBJS += \
	$(LOCAL_DIR)/init.o \
	$(LOCAL_DIR)/keypad.o \
	$(LOCAL_DIR)/atags.o \
	$(LOCAL_DIR)/panel.o
