/****************************************************************************
 *   FileName    : sd_hw.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#ifndef _SD_HW_H
#define _SD_HW_H

#include "sd_regs.h"

#define SLOT_ATTR_BOOT			(1<<0)
#define SLOT_ATTR_UPDATE		(1<<1)

typedef enum {
	SLOT_MAX_SPEED_NORMAL,
	SLOT_MAX_SPEED_HIGH
} SLOT_MAX_SPEED_T;

typedef struct {
	unsigned char ucHostNum;
	unsigned long ulSdSlotRegAddr;

	unsigned int uiSlotAttr;
	int iMaxBusWidth;
	SLOT_MAX_SPEED_T eSlotSpeed;
 	int (*CardDetect)(void);
 	int (*WriteProtect)(void);
} SD_HW_SLOTINFO_T;

void SD_HW_Initialize(void);
int SD_HW_Get_CardDetectPort(int iSlotIndex);
int SD_HW_Get_WriteProtectPort(int iSlotIndex);
PSDSLOTREG_T SD_HW_GetSdSlotReg(int iSlotIndex);
int SD_HW_GetMaxBusWidth(int iSlotIndex);
int SD_HW_IsSupportHighSpeed(int iSlotIndex);
int SD_HW_GetTotalSlotCount(void);
int SD_HW_GetBootSlotIndex(void);
int SD_HW_GetUpdateSlotIndex(void);

#endif //_SD_HW_H
