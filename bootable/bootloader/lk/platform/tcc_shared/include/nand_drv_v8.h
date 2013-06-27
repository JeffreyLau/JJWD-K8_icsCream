/****************************************************************************
 *   FileName    : nand_drv_v8.h
 *   Description : NAND driver for application.
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#ifndef _NAND_DRV_H
#define _NAND_DRV_H

//*****************************************************************************
//*
//*
//*                       [ General DEFINE & TYPEDEF ]
//*
//*
//*****************************************************************************
#ifndef DISABLE
#define DISABLE					0
#endif
#ifndef ENABLE
#define	ENABLE					1
#endif
#ifndef FALSE
#define FALSE           		0
#endif
#ifndef TRUE
#define TRUE            		1
#endif

#define NAND_KERNEL_AREA_INDEX			0 /*NAND_RO_0_AREA_INDEX*/

// mirror check define
#define TCBOOT_MIRROR_CHECK		(0x1 << 2)
#define MEMINIT_MIRROR_CHECK		(0x1 << 3)

typedef enum {
	NAND_INIT_TYPE_NORMAL,
	NAND_INIT_TYPE_FAST,
	NAND_INIT_TYPE_LOWFORMAT,
	NAND_INIT_TYPE_LOWFORMAT_DEBUG_ERASE_ONLY
} NAND_INIT_TYPE_T;

#define NAND_CTRL_AREA_INFO_VERIFY		0
#define NAND_CTRL_AREA_INFO_RESET		1

typedef enum {
	NAND_SUCCESS						= 0x00000000,
	NAND_ERROR							= 0xF1000000,
	NAND_ERROR_WRONG_PARAMETER			= 0xF1000001,
	NAND_ERROR_INIT_FAILURE				= 0xF1000002,
	NAND_ERROR_INIT_AREA_CHANGE			= 0xF1000003,
	NAND_ERROR_MEMORY_ALLOCATION		= 0xF1000004,
} NAND_ERROR_T;

typedef struct _NAND_USER_INFO_TABLE {
	unsigned char ucSerialNumber[32];
	unsigned long ulSerialNumberCRC;
} NAND_USER_INFO_TABLE;


//*****************************************************************************
//*
//*
//*                       [ EXTERNAL FUCTIONS DEFINE ]
//*
//*
//*****************************************************************************
extern NAND_ERROR_T NAND_Init(unsigned int uiDrvNum, NAND_INIT_TYPE_T eType);
extern unsigned int NAND_LowFormat(unsigned int uiDrvNum);
extern int NAND_ReadSector(unsigned int uiDrvNum, unsigned long ulSectorAddress, unsigned int uiSectorCount, void *pvBuffer);
extern int NAND_WriteSector(unsigned int uiDrvNum, unsigned long ulSectorAddress, unsigned int uiSectorCount, void *pvBuffer);
extern int NAND_Area_WriteSector(unsigned int uiDrvNum, unsigned int uiAreaIndex, unsigned long ulSectorAddress, unsigned int uiSectorCount, void *pvBuffer);
extern int NAND_Area_ReadSector(unsigned int uiDrvNum, unsigned int uiAreaIndex, unsigned long ulSectorAddress, unsigned int uiSectorCount, void *pvBuffer);
extern int NAND_RO_Area_Flush_WriteCache(unsigned int uiDrvNum);
extern int	NAND_GetSerialNumber(unsigned char *pucSN, unsigned int uiSize);
extern int NAND_Ioctl( int function, void *param );
extern void NAND_RecoveryBootArea(unsigned long ulBootStatus);

#if defined(_LINUX_)
extern unsigned int NAND_DumpExtArea( unsigned char *buf );
#endif

#endif

