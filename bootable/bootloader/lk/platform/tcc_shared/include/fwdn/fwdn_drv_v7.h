/****************************************************************************
 *   FileName    : Fwdn_drv_v7.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/

#ifndef _FWDN_DRV_V7_H_
#define _FWDN_DRV_V7_H_

#define FWDN_DEVICE_INIT_BITMAP_LOW_FORMAT								(1<<0)
#define FWDN_DEVICE_INIT_BITMAP_DUMP									(1<<1)
#define FWDN_DEVICE_INIT_BITMAP_UPDATE									(1<<2)
#define FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_MASK					(0x1F<<3)
#define FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_V1_RO				(0x04<<3)
#define FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_V1_WHOLE				(0x05<<3)
#define FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_V2_RO				(0x06<<3)
#define FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_V2_WHOLE				(0x02<<3)	// 0x2 for backward compatibility
#define FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_ERASE_ONLY			(0x01<<3)
#define FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_DO_NOT_USE			(0x03<<3)	// for backward compatibility
#define FWDN_DEVICE_INIT_BITMAP_RESERVED_MASK							(0xFFFFFF00)

enum
{
	ERR_FWDN_DRV_WRONG_PARAMETER = 0x10000000,
	ERR_FWDN_DRV_IOCTRL_DEV_INITIALIZE,
	ERR_FWDN_DRV_AREA_WRITE_COMPARE,
	ERR_FWDN_DRV_RESET_NOT_SUPPORT,
	ERR_FWDN_DRV_INSUFFICIENT_MEMORY,
	ERR_FWDN_DRV_AREA_WRITE,
	ERR_FWDN_DRV_AREA_READ,
};

enum
{
	FWDN_DISK_NONE,
	FWDN_DISK_HDD,
	FWDN_DISK_MMC,
	FWDN_DISK_UHP,
	FWDN_DISK_NAND,
	FWDN_DISK_TRIFLASH,
	FWDN_DISK_NOR,
	FWDN_DISK_SNOR,
	FWDN_DISK_MAX
};

enum
{
	SN_NOT_EXIST = 0,
	SN_VALID_16,
	SN_INVALID_16,
	SN_VALID_32,
	SN_INVALID_32
};


typedef int (*FXN_FWDN_DRV_FirmwareWrite_ReadFromHost)(unsigned char *buff, unsigned int size, unsigned int srcAddr, unsigned int percent);
typedef unsigned int (*FXN_FWDN_DRV_ReadFromHost)(void *buff, unsigned int size);
typedef unsigned int (*FXN_FWDN_DRV_DirectReadFromHost)(void **ppBuf);
typedef unsigned int (*FXN_FWDN_DRV_SendToHost)(void *buff, unsigned int size);
typedef void (*FXN_FWDN_DRV_Response_NotifyData)(unsigned int dataSize, unsigned int param0/*0 for normal*/);
typedef void (*FXN_FWDN_DRV_Response_RequestData)(unsigned int dataSize);
typedef int (*FXN_FWDN_DRV_RquestData)(unsigned char *buff, unsigned int size);
typedef void (*FXN_FWDN_DRV_SendStatus)(unsigned int param0, unsigned int param1, unsigned int param2);
typedef void (*FXN_FWDN_DRV_Progress)( unsigned int percent );

typedef struct _tag_FWDN_AREA_T {
	unsigned int		nSector;
	char				name[16];
} FWDN_AREA_T;

#define FWDN_AREA_LIST_MAX		20
typedef struct	 _tag_DeviceInfoType {
	unsigned int		DevSerialNumberType;					// Device Serial Number type SN_NOT_EXIST..
	unsigned char		DevSerialNumber[32];
	FWDN_AREA_T			area[FWDN_AREA_LIST_MAX];				// Default Area = area[0]
} FWDN_DEVICE_INFORMATION, *pFWDN_DEVICE_INFORMATION;

typedef struct	 _tag_NAND_HiddenSizeInfo {
	unsigned int				HiddenNum;						// Total Hidden Area Num	
	unsigned int 				HiddenPageSize[12];				// Each Hidden Area's Size
	unsigned int				RO_PageSize;
} NAND_PARTITION_INFO, *pNAND_HIDDEN_INFO;

 typedef	struct	__NAND_DeviceInfo {
	unsigned short int	DevID[8];
	unsigned int		MediaNums;			// Media Number of NANDFLASH
	unsigned int		MAX_ROMSize;
	unsigned int		ExtendedPartitionNum;
	unsigned int		ExtPartitionSize[12];
	unsigned int		ExtPartitionWCacheNum[12];
	unsigned int		ROAreaSize;
	unsigned short int  		PBpV;				// Physical all Block Number
	unsigned short int  		PpB;				// Page Number Per Block
	unsigned short int  		PageSize;			// Page Size
	unsigned short int  SpareSize;		
} NAND_DEVICE_INFO, *pNAND_DEVICE_INFO;

typedef struct _tag_NAND_DISK_INFO_T {
	unsigned int	bootSize_MB;
	unsigned int	diskSize_MB;
} NAND_DISK_INFO_T;

#ifdef BOOTSD_INCLUDE
#define MMC_DISK_MAX_HIDDEN_NUMBER		4

typedef struct _tag_MMC_DISK_INFO_T {
        unsigned int    nTotalSector;
        unsigned int    nBootSector;
        unsigned int    nHiddenNum;
        unsigned int    nHiddenSector[MMC_DISK_MAX_HIDDEN_NUMBER];
        unsigned int    nBytePerSector;
} MMC_DISK_INFO_T;
#endif
//==============================================================
//
//		Global Variables
//
//==============================================================
extern FWDN_DEVICE_INFORMATION		FWDN_DeviceInformation;
extern unsigned int					gFWDN_DRV_ErrorCode;

#define FWDN_DRV_GetErrorCode()		gFWDN_DRV_ErrorCode
#define FWDN_DRV_ClearErrorCode()	gFWDN_DRV_ErrorCode = 0
#define FWDN_DRV_SetErrorCode(a)	gFWDN_DRV_ErrorCode = a

//==============================================================
//
//		Function Prototypes
//
//==============================================================
void						initSourcePosition(void);

//void						FWDN_CheckOption(void);

void						FWDN_DRV_ClearMessage(void);
int							FWDN_DRV_AddMessage(const char *pErrMsg);
int							FWDN_DRV_GetMessage(unsigned int **ppMessage, unsigned int *pLength);

// for Device
void						FWDN_DRV_Reset(void);
int							FWDN_DRV_SessionStart(void);
int							FWDN_DRV_SessionEnd(unsigned int bSuccess);
int							FWDN_DRV_Init(unsigned int bmFlag, const FXN_FWDN_DRV_Progress fxnFwdnDrvProgress, char *message, unsigned int messageSize);
pFWDN_DEVICE_INFORMATION	FWDN_DRV_GetDeviceInfo(void);
int							FWDN_DRV_SerialNumberWrite(unsigned char *serial, unsigned int overwrite);
int							FWDN_DRV_FirmwareWrite(unsigned int fwSize, FXN_FWDN_DRV_FirmwareWrite_ReadFromHost fxnFWDN_DRV_FirmwareWrite_ReadFromHost);
int							FWDN_DRV_FirmwareWrite_Read(unsigned int addr, unsigned char *buff, unsigned int size, unsigned int percent);

#if 0//TNFTL_V7_INCLUDE
int 						FWDN_DRV_NAND_GANG_Format(void);
int							FWDN_DRV_NAND_GANG_Write( NAND_PART_INFO *sNandPartInfo, FXN_FWDN_DRV_RquestData fxnFwdnDrvRequestData );
#endif

// for Disk
int							FWDN_DRV_AREA_Write( char *name
													,unsigned int lba
													,unsigned int nSector
													,unsigned int nRemain
													,FXN_FWDN_DRV_Response_RequestData fxnFwdnDrvResponseRequestData
													,FXN_FWDN_DRV_DirectReadFromHost fxnFwdnDrvDirectReadFromHost );
int							FWDN_DRV_AREA_Read( char *name
													,unsigned int lba
													,unsigned int nSector
													,unsigned int nRemain
													,FXN_FWDN_DRV_Response_NotifyData fxnFwdnDrvResponseNotifyData
													,FXN_FWDN_DRV_SendToHost fxnFwdnDrvSendToHost );
int							FWDN_DRV_AREA_CalcCRC( char *name
													,unsigned int lba
													,unsigned int nSector
													,unsigned int nRemain
													,unsigned int *pCrc
													,FXN_FWDN_DRV_SendStatus fxnFwdnDrvSendStatus );

unsigned char				FWDN_DRV_DUMP_InfoRead(void *pBuf);
int							FWDN_DRV_DUMP_BlockRead(unsigned int Param0, unsigned int Param1, unsigned int Param2, 
																	FXN_FWDN_DRV_Response_NotifyData fxnFwdnDrvResponseNotifyData,
																	FXN_FWDN_DRV_SendToHost fxnFwdnDrvSendToHost);
int							FWDN_DRV_DUMP_BlockWrite(unsigned int Param0, unsigned int Param1, unsigned int Param2, 
																	FXN_FWDN_DRV_Response_RequestData fxnFwdnDrvResponseRequestData,
																	FXN_FWDN_DRV_ReadFromHost fxnFwdnDrvReadFromHost);
int							FWDN_DRV_TOTALBIN_Read( FXN_FWDN_DRV_Response_NotifyData fxnFwdnDrvResponseNotifyData,
																	FXN_FWDN_DRV_SendToHost fxnFwdnDrvSendToHost);
int							FWDN_DRV_TOTALBIN_Write(FXN_FWDN_DRV_RquestData fxnFwdnDrvRRequestData);

unsigned int				FWDN_FNT_SetSN(unsigned char* ucTempData, unsigned int uiSNOffset);
void						FWDN_FNT_VerifySN(unsigned char* ucTempData, unsigned int uiSNOffset);
void						FWDN_FNT_InsertSN(unsigned char *pSerialNumber);
unsigned char				FWDN_DRV_FirmwareStorageID(void);
#endif	// _FWDN_DRV_V7_H_

/* end of file */

