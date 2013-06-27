/****************************************************************************
 *   FileName    : sd_memory.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/

#ifndef _SD_MEMORY_H
#define _SD_MEMORY_H

#define BOOTCONFIG_HIGH_SPEED	Hw2
#define BOOTCONFIG_NORMAL_SPEED	HwZERO

#define BOOTCONFIG_1BIT_BUS		HwZERO
#define BOOTCONFIG_4BIT_BUS		Hw0
#define BOOTCONFIG_8BIT_BUS		Hw1

#define BYTE_TO_SECTOR(X)			((X + 511) >> 9)

unsigned BOOTSD_GetHeaderAddr(void);	/* [1448] */
unsigned BOOTSD_GetConfigCodeAddr(unsigned int uiConfigCodeBytes);	/* [1448] */
unsigned BOOTSD_GetROMAddr(unsigned int uiROM_Bytes, unsigned int uiConfigCodeBytes);	/* [1448] */

#if defined(BOOTSD_INCLUDE)
// Set Default Hidden Size
#define BOOTSD_BOOT_PAGESIZE			(2/*MB*/*1024*2)	// 2MB (= 2*1024*1024/512)

#define BOOTSD_HIDDEN_MAX_COUNT			10
#define BOOTSD_HIDDEN0_PAGESIZE			0				// 50MB (= 50*1024*1024/512)
#define BOOTSD_HIDDEN1_PAGESIZE			0				// 10MB (= 10*1024*1024/512)
#define BOOTSD_HIDDEN2_PAGESIZE			0				// 5MB  (= 5*1024*1024/512)
#define BOOTSD_HIDDEN3_PAGESIZE			0					// 0MB  (= 1*1024*1024/512)
#define BOOTSD_HIDDEN4_PAGESIZE			0					// 50MB (= 50*1024*1024/512)
#define BOOTSD_HIDDEN5_PAGESIZE			0					// 10MB (= 10*1024*1024/512)
#define BOOTSD_HIDDEN6_PAGESIZE			0					// 5MB  (= 5*1024*1024/512)
#define BOOTSD_HIDDEN7_PAGESIZE			0					// 0MB  (= 1*1024*1024/512)
#define BOOTSD_HIDDEN8_PAGESIZE			0					// 5MB  (= 5*1024*1024/512)
#define BOOTSD_HIDDEN9_PAGESIZE			0					// 0MB  (= 1*1024*1024/512)

typedef struct BOOTSD_Hidden_Size{
	unsigned long ulSectorSize[BOOTSD_HIDDEN_MAX_COUNT];
	unsigned long ulHiddenStart[BOOTSD_HIDDEN_MAX_COUNT];
}sHidden_Size_T,* pHidden_Size_T;

typedef struct BOOTSD_Header_info{
	unsigned long 	ulEntryPoint;
	unsigned long 	ulBaseAddr;
	unsigned long 	ulConfSize;
	unsigned long	ulROMFileSize;
	unsigned long 	ulBootConfig;
	unsigned long	ulResv20[3];
	unsigned char 	ucSerialNum[64];
	unsigned long 	ulResv96[8];
	unsigned long 	ulHiddenStartAddr;
	unsigned long 	ulHiddenSize[BOOTSD_HIDDEN_MAX_COUNT];
	unsigned long 	ulResv176[83];
	unsigned char	ucHeaderFlag[4];
	unsigned long 	ulCRC;
}BOOTSD_Header_Info_T;


// BOOTSD
int BOOTSD_Hidden_Info(pHidden_Size_T psHiddenSector);
int BOOTSD_Read(int reserved, unsigned long ulLBA_addr, unsigned long ulSector, void *buff);
int BOOTSD_ReadMultiStart(unsigned long ulAddr, unsigned long size);
int BOOTSD_ReadMulti(int reserved, unsigned long ulLBA_addr, unsigned long ulSector, void *buff);
int BOOTSD_ReadMultiStop(void);
int BOOTSD_Write(int reserved, unsigned long ulLBA_addr, unsigned long ulSector, void *buff);
int BOOTSD_WriteMultiStart(unsigned long ulAddr, unsigned long size);
int BOOTSD_WriteMulti(int reserved, unsigned long ulLBA_addr, unsigned long ulSector, void *buff);
int BOOTSD_WriteMultiStop(void);
int BOOTSD_Ioctl(int function, void *param);
#endif

// SD UPDATE
int SD_Read(int reserved, unsigned long ulLBA_addr, unsigned long ulSector, void *buff);
int SD_ReadMultiStart(unsigned long ulLBA_addr, unsigned long ulSize);
int SD_ReadMulti(int reserved, unsigned long ulLBA_addr, unsigned long ulSector, void *buff);
int SD_ReadMultiStop(void);
int SD_Write(int reserved, unsigned long ulLBA_addr, unsigned long ulSector, void *buff);
int SD_WriteMultiStart(unsigned long ulLBA_addr, unsigned long ulSize);
int SD_WriteMulti(int reserved, unsigned long ulLBA_addr, unsigned long ulSector, void *buff);
int SD_WriteMultiStop(void);
int SD_Ioctl(int function, void *param);

#endif //_SD_MEMORY_H

/* end of file */

