LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include

OBJS += \
	$(LOCAL_DIR)/axp192.o \
	$(LOCAL_DIR)/axp202.o \
	$(LOCAL_DIR)/act8810.o \
	$(LOCAL_DIR)/rn5t614.o
