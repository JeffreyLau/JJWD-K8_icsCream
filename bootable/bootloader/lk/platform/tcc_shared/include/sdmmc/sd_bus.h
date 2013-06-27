/****************************************************************************
 *   FileName    : sd_bus.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/

#ifndef _SD_BUS_H
#define _SD_BUS_H

#include "sd_regs.h"

#define SD_SLOT_TOTAL_COUNT			2

#define SD_BUS_MEM_READ				0
#define SD_BUS_MEM_WRITE			1

void SD_BUS_Initialize(void);
int SD_BUS_IsWriteProtected(int iSlotIndex);
int SD_BUS_Probe(int iSlotIndex);
int SD_BUS_IsCardDetected(int iSlotIndex);
int SD_BUS_IsMemoryCard(int iSlotIndex);
unsigned long SD_BUS_GetCardCapacity(int iSlotIndex);
unsigned long SD_BUS_GetBootStart(int iBootSize);
int SD_BUS_GetBootSlotIndex(void);
int SD_BUS_GetUpdateSlotIndex(void);
int SD_BUS_MEM_Request(int iSlotIndex, int rwFlag, unsigned long LBA_addr, unsigned long nSector, void *buff);
void SD_BUS_StopTransfer(int iSlotIndex);
sEXTCSDREG * SD_BUS_GetEXTCSD(int iSlotIndex);
int SD_BUS_switchEXTCSD(int iSlotIndex, unsigned short usIndex, unsigned char ucValue, unsigned char ucCmdSet);

#endif //_SD_BUS_H

