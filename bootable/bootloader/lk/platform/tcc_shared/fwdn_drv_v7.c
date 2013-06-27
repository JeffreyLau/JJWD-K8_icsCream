/****************************************************************************
 *   FileName    : Fwdn_drv_v7.c
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/

/****************************************************************************

  Revision History

 ****************************************************************************

 ****************************************************************************/
#if defined(PLATFORM_TCC92XX) || defined(PLATFORM_TCC93XX) || defined(PLATFORM_TCC88XX) || defined(PLATFORM_TCC892X)

#if defined(FWDN_V7)

#if defined(_LINUX_)
//#include "common.h"
#endif

#if defined(_LINUX_) || defined(_WINCE_)
#include <fwdn/fwupgrade.h>
#include <fwdn/Disk.h>
#include <fwdn/TC_File.h>
#include <fwdn/fwdn_drv_v7.h>
#if defined(TNFTL_V8_INCLUDE)
#include "nand_drv_v8.h"
#else
#include <tnftl/nand_drv.h>
#endif
#endif
#include <sdmmc/sd_memory.h>
#include <debug.h>

#if !defined(_LINUX_)
#include "memory.h"
#endif

#if defined(_WINCE_)
extern void KITLOutputDebugString(const char* fmt, ...);
#endif

extern void target_reinit(int);

#define	byte_of(X)					( *(volatile unsigned char *)((X)) )

#define TRANSFER_UNIT_SIZE			(64*512)
#define TRANSFER_UNIT_SECTOR_SIZE	(TRANSFER_UNIT_SIZE>>9)
#ifdef TNFTL_V7_INCLUDE
#define Tbuffersize  				(264*512)	// It should be larger then TRANSFER_UNIT_SECTOR_SIZE
#else
#define Tbuffersize  				(128*512)	// It should be larger then TRANSFER_UNIT_SECTOR_SIZE
#endif
#define TbufferSectorSize			(Tbuffersize>>9)

static unsigned int fwdndBuf[Tbuffersize/4];
static unsigned int veriBuf[Tbuffersize/4];

//==============================================================
//
//		Global Variables
//
//==============================================================
static const unsigned char gFWDN_FirmwareStorage		=
	#ifdef SNOR_BOOT_INCLUDE
		FWDN_DISK_SNOR
	#elif defined(NAND_BOOT_INCLUDE)
		FWDN_DISK_NAND
	#elif defined(BOOTSD_BOOT_INCLUDE)
		FWDN_DISK_TRIFLASH
	#elif defined(HDD_BOOT_INCLUDE)
		FWDN_DISK_NOR
	#else
	0//	#error	there is no device to store F/W image.
	#endif
;

static unsigned int				gImagePosition;
static unsigned int				ResetHandler_ptr;

unsigned int					g_uiFWDN_OverWriteSNFlag;
unsigned int					g_uiFWDN_WriteSNFlag;
FWDN_DEVICE_INFORMATION			FWDN_DeviceInformation;
static NAND_DEVICE_INFO			gNAND_DEVICE_INFO;

static FXN_FWDN_DRV_FirmwareWrite_ReadFromHost		gfxnFWDN_DRV_FirmwareWrite_ReadFromHost;
static FXN_FWDN_DRV_Progress		gfxnFWDN_DRV_Progress = NULL;

unsigned int				gFWDN_DRV_ErrorCode;

#define SESSION_FLAG_MTD_MASK				(0x00000001)
#define SESSION_FLAG_MTD					(0x00000001)
static unsigned int				s_FWDN_DRV_SessionFlag=0xFFFFFFFF;

#ifdef NAND_INCLUDE
#if defined(TNFTL_V8_INCLUDE)
// NAND's each disk's name
#if defined(NAND_KERNEL_INCLUDE)
static const char FWDN_NAND_AREA_0_NAME[]	= "KERNEL";
#else
static const char FWDN_NAND_AREA_0_NAME[]	= "NAND RO 0";
#endif
#if defined(_WINCE_) && defined(NAND_KERNEL_INCLUDE)
static const char FWDN_NAND_AREA_1_NAME[]	= "LOGO";
#else
static const char FWDN_NAND_AREA_1_NAME[]	= "NAND RO 1";
#endif
static const char FWDN_NAND_AREA_2_NAME[]	= "NAND RO 2";
static const char FWDN_NAND_AREA_3_NAME[]	= "NAND RO 3";
static const char FWDN_NAND_AREA_4_NAME[]	= "NAND RW 0";
static const char FWDN_NAND_AREA_5_NAME[]	= "NAND RW 1";
static const char FWDN_NAND_AREA_6_NAME[]	= "NAND RW 2";
static const char FWDN_NAND_AREA_7_NAME[]	= "NAND RW 3";
static const char FWDN_NAND_AREA_8_NAME[]	= "NAND RW 4";
static const char FWDN_NAND_AREA_9_NAME[]	= "NAND Data";
static const char *FWDN_NAND_AREA_NAME[] = {
	FWDN_NAND_AREA_0_NAME,
	FWDN_NAND_AREA_1_NAME,
	FWDN_NAND_AREA_2_NAME,
	FWDN_NAND_AREA_3_NAME,
	FWDN_NAND_AREA_4_NAME,
	FWDN_NAND_AREA_5_NAME,
	FWDN_NAND_AREA_6_NAME,
	FWDN_NAND_AREA_7_NAME,
	FWDN_NAND_AREA_8_NAME,
	FWDN_NAND_AREA_9_NAME,
};

#else
// NAND's each disk's name
#if defined(NAND_KERNEL_INCLUDE)
static const char FWDN_NAND_HD0_AREA_NAME[]	= "KERNEL";
#else
static const char FWDN_NAND_HD0_AREA_NAME[]	= "NAND Hidden 0";
#endif
#if defined(_WINCE_) && defined(NAND_KERNEL_INCLUDE)
static const char FWDN_NAND_HD1_AREA_NAME[]	= "LOGO";
#else
static const char FWDN_NAND_HD1_AREA_NAME[]	= "NAND Hidden 1";
#endif
static const char FWDN_NAND_HD2_AREA_NAME[]	= "NAND Hidden 2";
static const char FWDN_NAND_HD3_AREA_NAME[]	= "NAND Hidden 3";
static const char FWDN_NAND_HD4_AREA_NAME[]	= "NAND Hidden 4";
static const char FWDN_NAND_HD5_AREA_NAME[]	= "NAND Hidden 5";
static const char FWDN_NAND_HD6_AREA_NAME[]	= "NAND Hidden 6";
static const char FWDN_NAND_HD7_AREA_NAME[]	= "NAND Hidden 7";
static const char FWDN_NAND_HD8_AREA_NAME[]	= "NAND Hidden 8";
static const char FWDN_NAND_HD9_AREA_NAME[]	= "NAND Hidden 9";
static const char FWDN_NAND_HD10_AREA_NAME[]	= "NAND Hidden 10";
static const char *FWDN_NAND_HD_AREA_NAME[] = {
	FWDN_NAND_HD0_AREA_NAME,
	FWDN_NAND_HD1_AREA_NAME,
	FWDN_NAND_HD2_AREA_NAME,
	FWDN_NAND_HD3_AREA_NAME,
	FWDN_NAND_HD4_AREA_NAME,
	FWDN_NAND_HD5_AREA_NAME,
	FWDN_NAND_HD6_AREA_NAME,
	FWDN_NAND_HD7_AREA_NAME,
	FWDN_NAND_HD8_AREA_NAME,
	FWDN_NAND_HD9_AREA_NAME,
	FWDN_NAND_HD10_AREA_NAME,
};
static const char FWDN_NAND_DATA_AREA_NAME[]	= "NAND Data";
#if defined(_LINUX_)
static const char FWDN_NAND_MTD_AREA_NAME[]	= "MTD";
#endif
#endif
#endif //NAND_INCLUDE

#ifdef BOOTSD_INCLUDE
// BOOTSD_INCLUDE's each disk's name
#if defined(BOOTSD_KERNEL_INCLUDE)
static const char FWDN_SD_HD0_AREA_NAME[]	= "KERNEL";
#else
static const char FWDN_SD_HD0_AREA_NAME[]	= "SD Hidden 0";
#endif
#if defined(BOOTSD_KERNEL_INCLUDE)
static const char FWDN_SD_HD1_AREA_NAME[]	= "LOGO";
#else
static const char FWDN_SD_HD1_AREA_NAME[]	= "SD Hidden 1";
#endif
static const char FWDN_SD_HD2_AREA_NAME[]	= "SD Hidden 2";
static const char FWDN_SD_HD3_AREA_NAME[]	= "SD Hidden 3";
static const char FWDN_SD_HD4_AREA_NAME[]	= "SD Hidden 4";
static const char FWDN_SD_HD5_AREA_NAME[]	= "SD Hidden 5";
static const char FWDN_SD_HD6_AREA_NAME[]	= "SD Hidden 6";
static const char FWDN_SD_HD7_AREA_NAME[]	= "SD Hidden 7";
static const char FWDN_SD_HD8_AREA_NAME[]	= "SD Hidden 8";
static const char FWDN_SD_HD9_AREA_NAME[]	= "SD Hidden 9";
static const char FWDN_SD_HD10_AREA_NAME[]	= "SD Hidden 10";
static const char *FWDN_SD_HD_AREA_NAME[] = {
	FWDN_SD_HD0_AREA_NAME,
	FWDN_SD_HD1_AREA_NAME,
	FWDN_SD_HD2_AREA_NAME,
	FWDN_SD_HD3_AREA_NAME,
	FWDN_SD_HD4_AREA_NAME,
	FWDN_SD_HD5_AREA_NAME,
	FWDN_SD_HD6_AREA_NAME,
	FWDN_SD_HD7_AREA_NAME,
	FWDN_SD_HD8_AREA_NAME,
	FWDN_SD_HD9_AREA_NAME,
	FWDN_SD_HD10_AREA_NAME,
};
static const char FWDN_SD_DATA_AREA_NAME[]	= "SD Data";

#if defined(_LINUX_) && !defined(TNFTL_V8_INCLUDE)
static const char FWDN_NAND_MTD_AREA_NAME[] = "MTD";
#endif

MMC_DISK_INFO_T		triflashDiskInfo;
#endif

//==============================================================
//
//		External Variables
//
//==============================================================
#if defined(_LINUX_) || defined(_WINCE_)
extern unsigned long				_vectors;
#else
extern unsigned long				ResetHandler;
#endif
extern unsigned long				FirmwareSize;

//extern unsigned long				ImageRwBase;

//extern char							FirmwareVersion[];

extern unsigned int					gMAX_ROMSIZE;
extern const unsigned				CRC32_TABLE[];
//==============================================================
//
//		External Functionss
//
//==============================================================
void initSourcePosition(void)
{
#if defined (NKUSE)
#else
#if defined(_LINUX_) || defined(_WINCE_)
	ResetHandler_ptr	= (unsigned int ) &_vectors;
#else
	ResetHandler_ptr	= (unsigned int ) & ResetHandler;
#endif
	gImagePosition = ResetHandler_ptr;
#endif
}

#define FWDN_DRV_MESSAGE_SIZE_MAX		200
#define FWDN_DRV_MESSAGE_LINE_MAX		5
static unsigned short sFWDN_DRV_usMessageSize[FWDN_DRV_MESSAGE_LINE_MAX];
static unsigned char sFWDN_DRV_ucMessageLinePush;
static unsigned char sFWDN_DRV_ucMessageLinePop;
static unsigned int sFWDN_DRV_pMessage[FWDN_DRV_MESSAGE_LINE_MAX][FWDN_DRV_MESSAGE_SIZE_MAX/4];
void FWDN_DRV_ClearMessage(void)
{
	sFWDN_DRV_ucMessageLinePush = 0;
	sFWDN_DRV_ucMessageLinePop = 0;
}

int FWDN_DRV_AddMessage(const char *pErrMsg)
{
	if(sFWDN_DRV_ucMessageLinePush<FWDN_DRV_MESSAGE_LINE_MAX)
	{
		strncpy((char*)sFWDN_DRV_pMessage[sFWDN_DRV_ucMessageLinePush],pErrMsg,FWDN_DRV_MESSAGE_SIZE_MAX-1);
		sFWDN_DRV_usMessageSize[sFWDN_DRV_ucMessageLinePush] = strlen((char*)sFWDN_DRV_pMessage[sFWDN_DRV_ucMessageLinePush]) + 1;
		sFWDN_DRV_ucMessageLinePush++;
		return TRUE;
	}
	else
		return FALSE;
}

int FWDN_DRV_GetMessage(unsigned int **ppMessage, unsigned int *pLength)
{
	if(sFWDN_DRV_ucMessageLinePop<sFWDN_DRV_ucMessageLinePush)
	{
		*pLength = (unsigned int)sFWDN_DRV_usMessageSize[sFWDN_DRV_ucMessageLinePop];
		*ppMessage = sFWDN_DRV_pMessage[sFWDN_DRV_ucMessageLinePop++];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void FWDN_DRV_Reset(void)
{
	// goto reset vector

	FWDN_DRV_SetErrorCode(ERR_FWDN_DRV_RESET_NOT_SUPPORT);
}

int FWDN_DRV_SessionStart(void)
{
#if defined(_WINCE_)
	KITLOutputDebugString("===== FWDN Session Start! =====\n");
#endif

	s_FWDN_DRV_SessionFlag = 0x00000000;

	return TRUE;
}

int FWDN_DRV_SessionEnd(unsigned int bSuccess)
{
#if defined(NAND_INCLUDE) && defined(TNFTL_V8_INCLUDE)
	NAND_RO_Area_Flush_WriteCache(0);
#endif

	if(bSuccess)
	{
#if defined(_WINCE_)
		KITLOutputDebugString("===== FWDN Session End! =====\n");
#endif
	}
	else
	{
#if defined(_WINCE_)
		KITLOutputDebugString("===== FWDN Session is Failed!=====\n");
#endif
	}

	s_FWDN_DRV_SessionFlag = 0xFFFFFFFF;

	return TRUE;
}

#if defined(NAND_INCLUDE) && !defined(TNFTL_V8_INCLUDE)
static void FWDN_DRV_TNFTL_CallBackHandler( unsigned short SMC_DrvNo, unsigned short STATUS_CODE, unsigned int WParam )
{
	if( STATUS_CODE == TNFTL_CALLBACK_LCD_MAKE_HIDDEN_PAGE_PROCESS
		|| STATUS_CODE == TNFTL_CALLBACK_LCD_FORMAT_PROCESS
		|| STATUS_CODE == TNFTL_CALLBACK_LCD_BMP_UPDATE_PROCESS)
	{
		if(gfxnFWDN_DRV_Progress != NULL)
		{
			(*gfxnFWDN_DRV_Progress)((unsigned int)WParam);
		}
	}
	else if ( STATUS_CODE == TNFTL_CALLBACK_LCD_MAKE_HIDDEN_PAGE_START )
	{
		FWDN_DRV_AddMessage("NAND:Make Hidden Page");
	}
	else if ( STATUS_CODE == TNFTL_CALLBACK_LCD_FORMAT_START )
	{
		FWDN_DRV_AddMessage("NAND:Format");
	}
	else if ( STATUS_CODE == TNFTL_CALLBACK_LCD_BMP_UPDATE_START )
	{
		FWDN_DRV_AddMessage("NAND:BMP update");
	}
}

#ifdef TNFTL_V7_INCLUDE
static void FWDN_DRV_NAND_ProgressHandler(NAND_PROGRESS_TYPE_T eType, int iPercent)
{
	if( eType == NAND_PROGRESS_TYPE_FORMAT_PROCESS )
	{
		if(gfxnFWDN_DRV_Progress != NULL)
		{
			(*gfxnFWDN_DRV_Progress)((unsigned int)iPercent);
		}
	}
	else if( eType == NAND_PROGRESS_TYPE_FORMAT_START )
	{
		FWDN_DRV_AddMessage("NAND:Format");
	}
}
#endif
#endif

int FWDN_DRV_Init(unsigned int bmFlag, const FXN_FWDN_DRV_Progress fxnFwdnDrvProgress, char *message, unsigned int messageSize)
{
	int res = 0;

	g_uiFWDN_OverWriteSNFlag = 0;

	gfxnFWDN_DRV_Progress = fxnFwdnDrvProgress;

	if(bmFlag & FWDN_DEVICE_INIT_BITMAP_RESERVED_MASK)
	{
		FWDN_DRV_AddMessage("Unknown Flag.");
		return -1;
	}

#if defined(NAND_INCLUDE) && !defined(TNFTL_V8_INCLUDE)
	TNFTL_SetCallBackHandler( FWDN_DRV_TNFTL_CallBackHandler );

	if(bmFlag & FWDN_DEVICE_INIT_BITMAP_LOW_FORMAT)
	{
		//Low Format
		NAND_LowFormat(NAND_DRV_LOWFORMAT_TYPE_DEFAULT,FWDN_DRV_NAND_ProgressHandler);
	}

	if(bmFlag & FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_MASK)
	{
		switch(bmFlag&FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_MASK)
		{
			case FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_ERASE_ONLY:
				FWDN_DRV_AddMessage("Format - Erase Only");
				NAND_LowFormat(NAND_DRV_LOWFORMAT_TYPE_DEBUG_LEVEL_ERASE_ONLY,FWDN_DRV_NAND_ProgressHandler);
				break;
			case FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_V1_RO:
				FWDN_DRV_AddMessage("Format - V1 RO");
				NAND_LowFormat(NAND_DRV_LOWFORMAT_TYPE_DEBUG_LEVEL_V1_ROAREA,FWDN_DRV_NAND_ProgressHandler);
				break;
			case FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_V1_WHOLE:
				FWDN_DRV_AddMessage("Format - V1 Whole");
				NAND_LowFormat(NAND_DRV_LOWFORMAT_TYPE_DEBUG_LEVEL_V1_TOTALAREA,FWDN_DRV_NAND_ProgressHandler);
				break;
			case FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_V2_RO:
				FWDN_DRV_AddMessage("Format - V2 RO");
				NAND_LowFormat(NAND_DRV_LOWFORMAT_TYPE_DEBUG_LEVEL_V2_ROAREA,FWDN_DRV_NAND_ProgressHandler);
				break;
			case FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_V2_WHOLE:
				FWDN_DRV_AddMessage("Format - V2 Whole");
				NAND_LowFormat(NAND_DRV_LOWFORMAT_TYPE_DEBUG_LEVEL_V2_TOTALAREA,FWDN_DRV_NAND_ProgressHandler);
				break;
			default:
				FWDN_DRV_AddMessage("Unknown Init Flag.");
				return -1;
				break;
		}
	}

	if(bmFlag & FWDN_DEVICE_INIT_BITMAP_DUMP)
	{
		//Total-Image Dump
		NAND_InitForDump();
	}

    NAND_Init();

	if(bmFlag & FWDN_DEVICE_INIT_BITMAP_UPDATE)
	{
		// RomFile의 Size로 Init
		NAND_SetFlagOfChangeAreaSize( ENABLE );
		if( DISK_Ioctl( DISK_DEVICE_NAND, DEV_INITIALIZE, NULL ) != SUCCESS )
		{
			FWDN_DRV_SetErrorCode(ERR_FWDN_DRV_IOCTRL_DEV_INITIALIZE);
			res = -1;
		}

	}
	else
	{
		if ( TNFTL_CheckIsExistBMP( gNAND_DrvInfo[0].NFTLDrvInfo ) != SUCCESS )	// NAND에 기존에 사용하던 BMP가 있는지 확인
		{
			// BMP가 없는 경우 RomFile의 Size로 Init
			NAND_SetFlagOfChangeAreaSize( ENABLE );
			if( DISK_Ioctl( DISK_DEVICE_NAND, DEV_INITIALIZE, NULL ) != SUCCESS )
			{
				FWDN_DRV_SetErrorCode(ERR_FWDN_DRV_IOCTRL_DEV_INITIALIZE);
				res = -1;
			}
		}
		else
		{
			// BMP가 있는 경우 NAND의 Hidden, Multi Hidden의 Seeting을 그대로 유지.
			NAND_SetFlagOfChangeAreaSize( DISABLE );
			if( DISK_Ioctl( DISK_DEVICE_NAND, DEV_INITIALIZE, NULL ) != SUCCESS )
			{
				if ( gNAND_DrvInfo[0].NFTLDrvInfo->DrvReturnValue != ERR_TNFTL_OVERSIZE_MULTIHIDDEN_NUM )
				{
					FWDN_DRV_SetErrorCode(ERR_FWDN_DRV_IOCTRL_DEV_INITIALIZE);
					res = -1;
				}
			}
		}

		if(res == 0)// check partition difference
		{
			unsigned int i;
			NAND_PARTITION_INFO partitionRom,partitionNAND;

			memset((void*)&partitionRom,0x00,sizeof(partitionRom));
			memset((void*)&partitionNAND,0x00,sizeof(partitionNAND));

			//================================
			// Rom File Hidden Info
			//================================
			partitionRom.HiddenNum = gTNFTL_ExtPartitionInfo[0].ExtendedPartitionNum;
			for ( i = 0 ; i < partitionRom.HiddenNum ; ++i )
				partitionRom.HiddenPageSize[i] = gTNFTL_ExtPartitionInfo[0].ExtPartitionSize[i];
			partitionRom.RO_PageSize = gTNFTL_ExtPartitionInfo[0].ROAreaSize;

			//================================
			// Current NAND Hidden Info
			//================================
			if ( gNAND_DrvInfo[0].NFTLDrvInfo->DrvReturnValue == ERR_TNFTL_OVERSIZE_MULTIHIDDEN_NUM )
			{
				partitionNAND.HiddenNum = 0xFFFFFFFF;
			}
			else
			{
				partitionNAND.HiddenNum = gNAND_DrvInfo[0].NFTLDrvInfo->ExtendedPartitionNo;
				for ( i = 0 ; i < partitionNAND.HiddenNum ; ++i )
					partitionNAND.HiddenPageSize[i] = gNAND_DrvInfo[0].NFTLDrvInfo->ExtPartition[i].TotalSectorSize;
			}
			partitionNAND.RO_PageSize = gNAND_DrvInfo[0].NFTLDrvInfo->ROAreaSize >> 20;

			unsigned char 	*cPageBuffer, *cSpareBuffer;
			TNFTL_INIT_INFO	*sInitInfo;

			cPageBuffer		= &gNAND_PageBuffer[0];		
			cSpareBuffer	= &gNAND_PageBuffer[gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PageSize];

			res = NAND_IO_ReadPage( &gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0],
									0, 0, 1, cPageBuffer, cSpareBuffer, ECC_ON );
			
			sInitInfo 		= (TNFTL_INIT_INFO*)&cPageBuffer[NAND_SB_BOOT_PAGE_ECC_SIZE];
			
			if(memcmp((void*)&partitionRom,(void*)&partitionNAND,sizeof(partitionRom))!=0)
			{
				memcpy(message, "NAND partition changes is detected.\nIt will erase whole NAND.", messageSize);
				message[messageSize-1] = 0;
				res = 1;
			}
			else if( ( partitionNAND.RO_PageSize!= 0 ) && ( sInitInfo->ROAreaStPB == 0 ) )
			{
				ND_TRACE("Golden Information changes is detected.\nIt will erase whole NAND.\n");
				memcpy(message, "Golden Information changes is detected.\nIt will erase whole NAND.", messageSize);
				message[messageSize-1] = 0;
				res = 1;				
			}
		}
	}

	TNFTL_SetCallBackHandler( NULL );

#elif defined(NAND_INCLUDE) && defined(TNFTL_V8_INCLUDE)
	if(bmFlag & FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_MASK)
	{
		switch(bmFlag&FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_MASK)
		{
			case FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_ERASE_ONLY:
				FWDN_DRV_AddMessage("Format - Erase Only");
				res = NAND_Init(0,NAND_INIT_TYPE_LOWFORMAT_DEBUG_ERASE_ONLY);
				break;
			//case FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_V1_RO:
			//	FWDN_DRV_AddMessage("Format - V1 RO");
			//	NAND_LowFormat(NAND_DRV_LOWFORMAT_TYPE_DEBUG_LEVEL_V1_ROAREA,FWDN_DRV_NAND_ProgressHandler);
			//	break;
			//case FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_V1_WHOLE:
			//	FWDN_DRV_AddMessage("Format - V1 Whole");
			//	NAND_LowFormat(NAND_DRV_LOWFORMAT_TYPE_DEBUG_LEVEL_V1_TOTALAREA,FWDN_DRV_NAND_ProgressHandler);
			//	break;
			//case FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_V2_RO:
			//	FWDN_DRV_AddMessage("Format - V2 RO");
			//	NAND_LowFormat(NAND_DRV_LOWFORMAT_TYPE_DEBUG_LEVEL_V2_ROAREA,FWDN_DRV_NAND_ProgressHandler);
			//	break;
			//case FWDN_DEVICE_INIT_BITMAP_DEBUG_LEVEL_FORMAT_V2_WHOLE:
			//	FWDN_DRV_AddMessage("Format - V2 Whole");
			//	NAND_LowFormat(NAND_DRV_LOWFORMAT_TYPE_DEBUG_LEVEL_V2_TOTALAREA,FWDN_DRV_NAND_ProgressHandler);
			//	break;
			default:
				FWDN_DRV_AddMessage("Unknown Init Flag.");
				return -1;
				break;
		}
	}
	else if(bmFlag & FWDN_DEVICE_INIT_BITMAP_UPDATE)
	{
		res = NAND_Init(0,NAND_INIT_TYPE_LOWFORMAT);
	}
	else if(bmFlag & FWDN_DEVICE_INIT_BITMAP_DUMP)
	{
		//Total-Image Dump
		//NAND_InitForDump();
	}
	else
	{
		NAND_ERROR_T errNAND = NAND_Init(0,NAND_INIT_TYPE_NORMAL);
		if(errNAND == NAND_SUCCESS)
		{
			res = 0;
		}
		else if(errNAND == NAND_ERROR_INIT_AREA_CHANGE)
		{
			memcpy(message, "NAND partition changes is detected.\nIt will erase whole NAND.", messageSize);
			message[messageSize-1] = 0;
			res = 1;
		}
		else
		{
			FWDN_DRV_SetErrorCode(ERR_FWDN_DRV_IOCTRL_DEV_INITIALIZE);
			res = -1;
		}
	}

#elif defined(BOOTSD_INCLUDE)
	if(DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_INITIALIZE, NULL ) < 0 )
	{
		FWDN_DRV_SetErrorCode(ERR_FWDN_DRV_IOCTRL_DEV_INITIALIZE);
		res = -1;
	}

	if( bmFlag & FWDN_DEVICE_INIT_BITMAP_LOW_FORMAT )
	{
		if( erase_emmc(0, 0, 1) )
			printf("eMMC low format error has occurred!\n");
		else
			printf("eMMC erasing completed!\n");
	}
#endif

	return res;
}

static void _loc_strncpy(char *dst, const char *src, unsigned int n)
{
	while(n--)
	{
		*dst++=*src;
		if(*src++==0)
			break;
	}
#if 0		/* 010.03.29 */

	int i = 0;

	while (src[i] != '\0' && i < n)
	{
		dst[i] = src[i];
		i++;
	}
	while(i < n)
	{
		dst[i] = '\0';
		i++;
	}
 
return &dst[0];
#endif /* 0 */

	
}

static int _loc_strcmp(const char *dst, const char *src)
{
	while( *dst!=0 || *src!=0 )
	{
		if( *dst++ != *src++ )
			return -1;
	}
	return 0;
}

//static int _loc_strncmp(void *dst, void *src, unsigned int n)
//{
//	while(n--)
//	{
//		if(*dst++!=*src)
//			return -1;
//		if(*src++==0)
//			break;
//	}
//	return 0;
//}

static void _loc_register_area(unsigned int index, const char *name, unsigned int nSector)
{
	memset(FWDN_DeviceInformation.area[index].name,0,16);
	_loc_strncpy(FWDN_DeviceInformation.area[index].name,name,16);
	FWDN_DeviceInformation.area[index].nSector = nSector;
}

//static void _loc_register_default_disk(unsigned int diskID)
//{
//	_loc_register_disk(0,diskID);
//}

pFWDN_DEVICE_INFORMATION FWDN_DRV_GetDeviceInfo(void)
{
#if defined (NKUSE)
	memset(&FWDN_DeviceInformation,0,sizeof(FWDN_DeviceInformation));
#else
	unsigned int i,nDisk=0;

	memset(&FWDN_DeviceInformation,0,sizeof(FWDN_DeviceInformation));

	//switch(gFWDN_SerialNumberStorage)
	{
		#if defined(NAND_BOOT_INCLUDE)
		//case FWDN_DISK_NAND:
		{
			#if defined(TNFTL_V8_INCLUDE)
			int iSize = FwdnGetNandSerial(FWDN_DeviceInformation.DevSerialNumber,sizeof(FWDN_DeviceInformation.DevSerialNumber));
			if(iSize == 32)
			{
				FWDN_DeviceInformation.DevSerialNumberType = SN_VALID_32;
			}
			else
			{
				FWDN_DeviceInformation.DevSerialNumberType = SN_NOT_EXIST;
			}
			#else
			FwdnGetNandSerial();
			#endif
			//break;
		}
			//break;
		#elif defined(	BOOTSD_BOOT_INCLUDE)
		//case FWDN_DISK_TRIFLASH:
			FwdnGetBootSDSerial();
			//break;
		#elif defined(HDD_BOOT_INCLUDE)
		//case FWDN_DISK_HDD:
			FwdnGetNorSerial();
			//break;
		#elif defined(SNOR_BOOT_INCLUDE)
		//case FWDN_DISK_SNOR:
			FwdnGetSerialNorSerial();
			//break;
		#endif
	}

	// Reset All Area Information
	for(i=0;i<FWDN_AREA_LIST_MAX;i++)
		_loc_register_area(i,"",0);

#if defined(NAND_INCLUDE)
#if defined(TNFTL_V8_INCLUDE)
	for(i=0; i<NAND_Area_GetCount(0);i++)
	{
		unsigned int uiTotalSector = NAND_Area_GetTotalSectorCount(0, i);
		if(uiTotalSector != 0)
		{
			_loc_register_area(
							nDisk++,
							FWDN_NAND_AREA_NAME[i],
							uiTotalSector
			);
		}
	}
#else
	for(i=0; i<gNAND_DrvInfo[0].NFTLDrvInfo->ExtendedPartitionNo;i++)
	{
		_loc_register_area(
						nDisk++,
						FWDN_NAND_HD_AREA_NAME[i],
						gNAND_DrvInfo[0].NFTLDrvInfo->ExtPartition[i].TotalSectorSize
		);
	}

	_loc_register_area(
					nDisk++,
					FWDN_NAND_DATA_AREA_NAME,
					gNAND_DrvInfo[0].NFTLDrvInfo->PriPartition.TotalSectorSize
	);

#if defined(_LINUX_)
	if(gNAND_DrvInfo[0].NFTLDrvInfo->ROAreaSize)
	{
		_loc_register_area(
						nDisk++,
						FWDN_NAND_MTD_AREA_NAME,
						(gNAND_DrvInfo[0].NFTLDrvInfo->ROAreaSize+511)>>9
		);
	}
#endif
#endif
#endif //NAND_INCLUDE

#ifdef BOOTSD_INCLUDE
	{
		unsigned long 	ulDataTotalSector;
		unsigned long	ulHiddenCount;

		DISK_Ioctl(DISK_DEVICE_TRIFLASH,DEV_GET_HIDDEN_COUNT, (void *)&ulHiddenCount);
		for(i=0;i<ulHiddenCount;i++)
		{
			ioctl_diskhidden_t stDiskHidden;
			stDiskHidden.ulIndex = (int)i;
			stDiskHidden.ulTotalSector = 0;
			DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_GET_HIDDEN_SIZE, (void *)&stDiskHidden);

			if(stDiskHidden.ulTotalSector)
			{
				_loc_register_area(
								nDisk++,
								FWDN_SD_HD_AREA_NAME[i],
								stDiskHidden.ulTotalSector
				);
			}
		}

		//SD data area total size
		DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_GET_DISK_SIZE, (void *)&ulDataTotalSector);
		_loc_register_area(
						nDisk++,
						FWDN_SD_DATA_AREA_NAME,
						ulDataTotalSector
		);
	}

#endif

#endif
	return &FWDN_DeviceInformation;
}

int	FWDN_DRV_SerialNumberWrite(unsigned char *serial, unsigned int overwrite)
{
	int	res	= -1;

	#ifndef WITHOUT_FILESYSTEM
		TC_SyncDrives(-1, 1);	// clean & flush entire cache
	#endif

	//switch (gFWDN_SerialNumberStorage)
	{
		#if defined(NAND_BOOT_INCLUDE)
		//case FWDN_DISK_NAND:
			res = FwdnSetNandSerial( serial, overwrite);
			//break;
		#elif defined(BOOTSD_BOOT_INCLUDE)
		//case FWDN_DISK_TRIFLASH:
			res = FwdnSetBootSDSerial( serial, overwrite);
			//break;
		#elif defined(HDD_BOOT_INCLUDE)
		//case FWDN_DISK_HDD:
			res = FwdnSetNorSerial( serial, overwrite);
			//break;
		#elif defined(SNOR_BOOT_INCLUDE)
		//case FWDN_DISK_SNOR:
			res = FwdnSetSerialFlashSerial( serial, overwrite);
			//break;
		#endif
	}

	if ( res == 0)
		return TRUE;
	else
		return FALSE;
}

int FWDN_DRV_FirmwareWrite(unsigned int fwSize, FXN_FWDN_DRV_FirmwareWrite_ReadFromHost fxnFWDN_DRV_FirmwareWrite_ReadFromHost)
{
	gfxnFWDN_DRV_FirmwareWrite_ReadFromHost = fxnFWDN_DRV_FirmwareWrite_ReadFromHost;

	#ifndef WITHOUT_FILESYSTEM
		TC_SyncDrives(-1, 1);	// clean & flush entire cache
	#endif
	switch (gFWDN_FirmwareStorage)
	{
		//#ifndef WITHOUT_FILESYSTEM
		#ifdef NAND_BOOT_INCLUDE
		case FWDN_DISK_NAND:
		{
			#if defined(TNFTL_V8_INCLUDE)
				unsigned char *pucRomBuffer = FWUG_GetTempBuffer(fwSize);
				if(pucRomBuffer == NULL)
					return ERR_FWUG_FAIL_MEMORY_ALLOCATION;
				if(FWDN_DRV_FirmwareWrite_Read(0, pucRomBuffer, fwSize, 100))
						return ERR_FWUG_FAIL_COMMUNICATION;
				return 	FwdnWriteNandFirmware(pucRomBuffer,fwSize);
			#elif defined(TNFTL_V7_INCLUDE)
				if(FWDN_DRV_FirmwareWrite_Read(0, (unsigned char *)ROMFILE_TEMP_BUFFER, fwSize, 100))
						return ERR_FWUG_FAIL_COMMUNICATION;
				return 	FwdnWriteNandFirmware(fwSize);
			#endif			
		}
		#endif
		#ifdef BOOTSD_BOOT_INCLUDE
		case FWDN_DISK_TRIFLASH:
			return	FwdnWriteBootSDFirmware(fwSize);
		#endif
		//#endif //WITHOUT_FILESYSTEM

		#ifdef HDD_BOOT_INCLUDE
		case FWDN_DISK_HDD:
		case FWDN_DISK_NOR:
			return	FwdnWriteNorFlashFirmware(fwSize);
		#endif

		#ifdef SNOR_BOOT_INCLUDE
		case FWDN_DISK_SNOR:
			return	FwdnWriteSerialNorFirmware(fwSize);
		#endif
	}

	return	0;
}

int FWDN_DRV_FirmwareWrite_Read(unsigned int addr, unsigned char *buff, unsigned int size, unsigned int percent)
{
	return (*gfxnFWDN_DRV_FirmwareWrite_ReadFromHost)(buff, size, addr, percent);
}

int _loc_AreaRead(char *name, unsigned int lba, unsigned short nSector, void *buff)
{
	unsigned int i;
#ifdef NAND_INCLUDE
#if defined(TNFTL_V8_INCLUDE)
	for(i=0 ; i<NAND_Area_GetCount(0); i++)
	{
		if(_loc_strcmp(FWDN_NAND_AREA_NAME[i],name) == 0)
		{
			return NAND_Area_ReadSector(0, i, lba, nSector, buff);
		}
	}
#else
	for( i = 0; i < gNAND_DrvInfo[0].NFTLDrvInfo->ExtendedPartitionNo; i++ )
	{
		if ( i == 0 )	// Hidden		--> ExtPartition [0]
		{
			if( _loc_strcmp(FWDN_NAND_HD_AREA_NAME[0],name) == 0 )
			{
				return DISK_HDReadSector( DISK_DEVICE_NAND, lba, nSector, buff );
			}
		}
		else			// Multi Hidden --> ExtPartition [1 ~ ]
		{
			if( _loc_strcmp(FWDN_NAND_HD_AREA_NAME[i],name) == 0 )
			{
				//return NAND_MHD_ReadSector( 0, i, lba, (U32)nSector, buff );
				return DISK_ReadSector(DISK_DEVICE_NAND_HD, i-1, lba, nSector, buff );
			}
		}
	}

	if( _loc_strcmp(FWDN_NAND_DATA_AREA_NAME,name) == 0 )
		return DISK_ReadSector(DISK_DEVICE_NAND, 0, lba, nSector, buff);
#endif
#endif //NAND_INCLUDE

#ifdef BOOTSD_INCLUDE
	{
		ioctl_diskrwpage_t rwHiddenPage;
		
		if( _loc_strcmp(FWDN_SD_DATA_AREA_NAME,name) == 0 )
		{
			return DISK_ReadSector(DISK_DEVICE_TRIFLASH, 0, lba, nSector, buff);
		}

		for( i = 0; i < BOOTSD_HIDDEN_MAX_COUNT; i++ )
		{
			if( _loc_strcmp(FWDN_SD_HD_AREA_NAME[i],name) == 0 )
			{
				rwHiddenPage.start_page		= lba;
				rwHiddenPage.rw_size		= nSector;
				rwHiddenPage.hidden_index	= i;
				rwHiddenPage.buff			= buff;
				return DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_HIDDEN_WRITE_PAGE, (void *) &rwHiddenPage);
			}
		}
	}
#endif

	return -1;
}

#if defined(_LINUX_) && defined(NAND_INCLUDE) && defined(TNFTL_V7_INCLUDE)
const unsigned char TCC_MTD_Signature[] ={"TCCMTDIMAGE"};
extern int NAND_MTD_Init( NAND_IO_DEVINFO *nDevInfo, U32* rMTDStBlk, U32* rMTDEdBlk, U32 bErase );
extern int NAND_MTD_GetPhyBlkAddr( NAND_IO_DEVINFO *nDevInfo, U32 nMTDBlkAddr, U32 *rMTDPhyBlkAddr, U32 *rChipNo);
extern int NAND_MTD_WritePage( U32 nPageAddr, U8* nPageBuffer );
extern int NAND_MTD_ReadPage( U32 nPageAddr, U8* nPageBuffer );

#include <debug.h>
#include <lib/ptable.h>
#define MTD_PART_BOOT				0x0001
#define MTD_PART_SYSTEM				0x0002
#define MTD_PART_RECOVERY			0x0003
#define MTD_PART_USERDATA			0x0004
#define MTD_PART_SPLASH				0x0005
	//boot_size       = readBuf32[4];
	//system_size     = readBuf32[5];
	//recovery_size   = readBuf32[6];
	//userdata_size	= readBuf32[7];
	//splash_size		= readBuf32[8];
extern int flash_write_by_vtc(struct ptentry *ptn, unsigned extra_per_page, unsigned long long bytes
							,FXN_FWDN_DRV_Response_RequestData fxnFwdnDrvResponseRequestData
							,FXN_FWDN_DRV_DirectReadFromHost fxnFwdnDrvDirectReadFromHost
							,unsigned nStartOffSet, unsigned nExpectBadNum);


extern int flash_read_by_vtc(struct ptentry *ptn, unsigned extra_per_page, unsigned long long bytes
							,FXN_FWDN_DRV_Response_NotifyData fxnFwdnDrvResponseNotifyData
							,FXN_FWDN_DRV_SendToHost fxnFwdnDrvSendToHost );
							
extern int flash_calc_crc_by_vtc(struct ptentry *ptn, unsigned extra_per_page, unsigned long long bytes,
							unsigned int *crc, FXN_FWDN_DRV_SendStatus fxnFwdnDrvSendStatus);

int _loc_MtdWrite(unsigned short wPart
					,unsigned int long long nLength
					,FXN_FWDN_DRV_Response_RequestData fxnFwdnDrvResponseRequestData
					,FXN_FWDN_DRV_DirectReadFromHost fxnFwdnDrvDirectReadFromHost)
{
	static NAND_IO_DEVINFO		sDevInfo[2];
	unsigned int				i;
	unsigned int				rMTDStBlk, rMTDEdBlk, rMTDFTLBlk;
	NAND_IO_ERROR				res = 0;
	unsigned int				spare_size;
	
	struct ptentry				*ptn;
	struct ptable				*ptable;

	unsigned int				nPartNum;
	TNFTL_MTDBadBlkInfo			*nMTDBadBlkInfo;

	char						*szPartName = NULL;
	unsigned int				extra_per_page = 0;

	if( !flash_check_table() )
	{
		flash_init_ptable();
		target_init();
	}		
		
	if((s_FWDN_DRV_SessionFlag&SESSION_FLAG_MTD_MASK) != SESSION_FLAG_MTD)
	{
		// first time
		s_FWDN_DRV_SessionFlag |= SESSION_FLAG_MTD;

		// initialize & erase all mtd blocks
		dprintf(INFO, "Erase all MTD blocks.\n");
		res = NAND_MTD_Init( &sDevInfo, &rMTDStBlk, &rMTDEdBlk, 1/*erase all*/ );
	}

	if ( res != 0 )
		return FALSE;

	nPartNum		= flash_get_partnum();
	nMTDBadBlkInfo	= (TNFTL_MTDBadBlkInfo *)malloc( sizeof( TNFTL_MTDBadBlkInfo ) * nPartNum );

	// Set Partition Bad Block Table
	NAND_MTD_GetBadBlkInfo( nMTDBadBlkInfo );

	if (sDevInfo[0].Feature.PageSize >= 16384)
		spare_size = 512;
	else if (sDevInfo[0].Feature.PageSize >= 8192)
		spare_size = 256;
	else if (sDevInfo[0].Feature.PageSize >= 4096)
		spare_size = 128;
	else
		spare_size = 64;

	// get partition status
	ptable = flash_get_ptable();
	if (ptable == NULL) {
		dprintf(INFO, "partition table doesn't exist\n");
		return FALSE;
	}

	switch(wPart)
	{
		case MTD_PART_BOOT:
			// flash boot
			szPartName = "boot";
			break;
		case MTD_PART_SYSTEM:
			// flash system
			szPartName = "system";
			extra_per_page = spare_size;
			break;
		case MTD_PART_RECOVERY:
			// flash recovery
			szPartName = "recovery";
			break;
		case MTD_PART_USERDATA:
			// flash userdata
			szPartName = "userdata";
			extra_per_page = spare_size;
			break;
		case MTD_PART_SPLASH:
			// flash splash
			szPartName = "splash";
			break;
	}

	if(szPartName!=NULL)
	{
		dprintf(INFO, "MTD image size ==> %s:0x%X\n", szPartName, nLength);
		ptn			= ptable_find(ptable, szPartName);
		nPartNum	= (unsigned int)ptable_GetPartNum( ptable, ptn );
		if (ptn == NULL) 
		{
			dprintf(INFO, "unknown partition name - %s\n",szPartName);
			return FALSE;
		}
		else if (flash_write_by_vtc(ptn, extra_per_page, nLength, fxnFwdnDrvResponseRequestData, fxnFwdnDrvDirectReadFromHost
								, nMTDBadBlkInfo[nPartNum].AccBadBlkNum, nMTDBadBlkInfo[nPartNum].BadBlkNum) !=0) 
		{
			dprintf(INFO, "############## error #############\n");
			return FALSE;
		}
	}
	else
	{
		dprintf(INFO, "unknown partiton code - %d\n",wPart);
		return FALSE;
	}

	return TRUE;
}


int _loc_MtdRead(unsigned short wPart
					,unsigned long long nLength
					,FXN_FWDN_DRV_Response_NotifyData fxnFwdnDrvResponseNotifyData
					,FXN_FWDN_DRV_SendToHost fxnFwdnDrvSendToHost )
{

	unsigned int		rMTDStBlk;
	unsigned int		rMTDEdBlk;
	NAND_IO_ERROR		res;
	NAND_IO_DEVINFO		sDevInfo;
	unsigned int 		spare_size;

	struct ptentry		*ptn;
	struct ptable		*ptable;
	
	unsigned int		nPartNum;
	TNFTL_MTDBadBlkInfo	*nMTDBadBlkInfo;

	char				*szPartName = NULL;
	unsigned int		extra_per_page = 0;

	// initialize & erase all mtd blocks
	res = NAND_MTD_Init( &sDevInfo, &rMTDStBlk, &rMTDEdBlk, 0 );
	if ( res != 0 )
		return FALSE;

	nPartNum		= flash_get_partnum();
	nMTDBadBlkInfo	= (TNFTL_MTDBadBlkInfo *)malloc( sizeof( TNFTL_MTDBadBlkInfo ) * nPartNum );

	// Set Partition Bad Block Table
	NAND_MTD_GetBadBlkInfo( nMTDBadBlkInfo );

	if (sDevInfo.Feature.PageSize >= 16384)
		spare_size = 512;
	else if (sDevInfo.Feature.PageSize >= 8192)
		spare_size = 256;
	else if (sDevInfo.Feature.PageSize >= 4096)
		spare_size = 128;
				else
		spare_size = 64;

	// get partition status
	ptable = flash_get_ptable();
	if (ptable == NULL) 
	{
		dprintf(INFO, "partition table doesn't exist\n");
					return FALSE;
	}

	switch(wPart)
	{
		case MTD_PART_BOOT:
			// flash boot
			szPartName = "boot";
			break;
		case MTD_PART_SYSTEM:
			// flash system
			szPartName = "system";
			extra_per_page = spare_size;
			break;
		case MTD_PART_RECOVERY:
			// flash recovery
			szPartName = "recovery";
			break;
		case MTD_PART_USERDATA:
			// flash userdata
			szPartName = "userdata";
			extra_per_page = spare_size;
			break;
		case MTD_PART_SPLASH:
			// flash splash
			szPartName = "splash";
			break;
	}

	if(szPartName!=NULL)
	{
		dprintf(INFO, "MTD image size ==> %s:0x%X\n", szPartName, nLength);
		ptn			= ptable_find(ptable, szPartName);
		nPartNum	= ptable_GetPartNum( ptable, ptn );
		if (ptn == NULL) 
		{
			dprintf(INFO, "unknown partition name - %s\n",szPartName);
			return FALSE;
		}
		else if (flash_read_by_vtc(ptn, extra_per_page, nLength, fxnFwdnDrvResponseNotifyData, fxnFwdnDrvSendToHost) !=0)
		{
			dprintf(INFO, "############## error #############\n");
			return FALSE;
		}
	}
	else
	{
		dprintf(INFO, "unknown partiton code - %d\n",wPart);
		return FALSE;
	}

		return TRUE;
}


int _loc_MtdCalcCRC(unsigned short wPart
					,unsigned long long nLength
					,unsigned int *crc
					,FXN_FWDN_DRV_SendStatus fxnFwdnDrvSendStatus)
{
	unsigned int		rMTDStBlk;
	unsigned int		rMTDEdBlk;
	NAND_IO_ERROR		res;

	NAND_IO_DEVINFO		sDevInfo[2];
	unsigned int 		spare_size;

	struct ptentry		*ptn;
	struct ptable		*ptable;

	unsigned int		nPartNum;
	TNFTL_MTDBadBlkInfo	*nMTDBadBlkInfo;

	char				*szPartName = NULL;
	unsigned int		extra_per_page = 0;

	// initialize & erase all mtd blocks
	res = NAND_MTD_Init( &sDevInfo, &rMTDStBlk, &rMTDEdBlk, 0 );
	if ( res != 0 )
		return FALSE;

	nPartNum		= flash_get_partnum();
	nMTDBadBlkInfo	= (TNFTL_MTDBadBlkInfo *)malloc( sizeof( TNFTL_MTDBadBlkInfo ) * nPartNum );

	// Set Partition Bad Block Table
	NAND_MTD_GetBadBlkInfo( nMTDBadBlkInfo );

	if (sDevInfo[0].Feature.PageSize >= 16384)
		spare_size = 512;
	else if (sDevInfo[0].Feature.PageSize >= 8192)
		spare_size = 256;
	else if (sDevInfo[0].Feature.PageSize >= 4096)
		spare_size = 128;
	else
		spare_size = 64;

	// get partition status
	ptable = flash_get_ptable();
	if (ptable == NULL) {
		dprintf(INFO, "partition table doesn't exist\n");
		return FALSE;
	}

	switch(wPart)
	{
		case MTD_PART_BOOT:
			// flash boot
			szPartName = "boot";
			break;
		case MTD_PART_SYSTEM:
			// flash system
			szPartName = "system";
			extra_per_page = spare_size;
			break;
		case MTD_PART_RECOVERY:
			// flash recovery
			szPartName = "recovery";
			break;
		case MTD_PART_USERDATA:
			// flash userdata
			szPartName = "userdata";
			extra_per_page = spare_size;
			break;
		case MTD_PART_SPLASH:
			// flash splash
			szPartName = "splash";
			break;
	}

	if(szPartName!=NULL)
	{
		dprintf(INFO, "MTD image size ==> %s:0x%X\n", szPartName, nLength);
		ptn			= ptable_find(ptable, szPartName);
		nPartNum	= ptable_GetPartNum( ptable, ptn );
		if (ptn == NULL) {
			dprintf(INFO, "unknown partition name - %s\n",szPartName);
			return FALSE;
		}
		else if (flash_calc_crc_by_vtc(ptn, extra_per_page, nLength, crc, fxnFwdnDrvSendStatus) !=0) {
			dprintf(INFO, "############## error #############\n");
			return FALSE;
		}
	}
	else
	{
		dprintf(INFO, "unknown partiton code - %d\n",wPart);
		return FALSE;
	}

	return TRUE;
}
#endif //defined(_LINUX_) && defined(NAND_INCLUDE)

int _loc_AreaWrite(char *name, unsigned int lba, unsigned short nSector, void *buff)
{
	unsigned int i;
#ifdef BOOTSD_INCLUDE
	ioctl_diskrwpage_t rwHiddenPage;
#endif

#if defined(NAND_INCLUDE)
#if defined(TNFTL_V8_INCLUDE)
	for(i=0 ; i<NAND_Area_GetCount(0); i++)
	{
		if(_loc_strcmp(FWDN_NAND_AREA_NAME[i],name) == 0)
		{
			return NAND_Area_WriteSector(0, i, lba, nSector, buff);
		}
	}
#else
	for( i = 0; i < gNAND_DrvInfo[0].NFTLDrvInfo->ExtendedPartitionNo; i++ )
	{
		if ( i == 0 )
		{
			if( _loc_strcmp(FWDN_NAND_HD_AREA_NAME[0],name) == 0 )
			{
				return DISK_HDWriteSector( DISK_DEVICE_NAND, lba, nSector, buff );
			}
		}
		else
		{
			if( _loc_strcmp(FWDN_NAND_HD_AREA_NAME[i],name) == 0 )
			{
				//return NAND_MHD_WriteSector( 0, i, lba, (U32)nSector, buff );
				return DISK_WriteSector(DISK_DEVICE_NAND_HD, i-1, lba, nSector, buff );
			}
		}
	}

	if( _loc_strcmp(FWDN_NAND_DATA_AREA_NAME,name) == 0 )
		return DISK_WriteSector(DISK_DEVICE_NAND, 0, lba, nSector, buff);
#endif
#endif //NAND_INCLUDE

#if defined(BOOTSD_INCLUDE)
	{
		ioctl_diskrwpage_t rwHiddenPage;

		if( _loc_strcmp(FWDN_SD_DATA_AREA_NAME,name) == 0 )
		{
			return DISK_WriteSector(DISK_DEVICE_TRIFLASH, 0, lba, nSector, buff);
		}
		
		for(i = 0; i < BOOTSD_HIDDEN_MAX_COUNT; i++)
		{
			if(_loc_strcmp(FWDN_SD_HD_AREA_NAME[i],name) == 0)
			{
				rwHiddenPage.start_page		= lba;
				rwHiddenPage.rw_size		= nSector;
				rwHiddenPage.hidden_index	= i;
				rwHiddenPage.buff			= buff;
				return DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_HIDDEN_WRITE_PAGE, (void *) &rwHiddenPage);
			}
		}

	}
#endif

	return -1;
}

#if 0//TNFTL_V7_INCLUDE
int FWDN_DRV_NAND_GANG_Format(void)
{
#ifdef NAND_INCLUDE
	NAND_LowFormat(NAND_DRV_LOWFORMAT_TYPE_DEFAULT);
#endif
	return -1;
}

int FWDN_DRV_NAND_GANG_Write( NAND_PART_INFO *sNandPartInfo, FXN_FWDN_DRV_RquestData fxnFwdnDrvRequestData )
{
	unsigned int 		i, j;
	unsigned int		nBlockNum;
	unsigned int		nBlockAddr, nPageAddr;
	unsigned int		nGangStBlk, nGangEdBlk;
	unsigned int		nBlockSize, nBlockSectorSize;
	unsigned int		nGoodBlockIndex;
	unsigned short 		nReceiveSector, nReceiveSectorCount;
	unsigned char		bBlkArea;
	unsigned char		cSpare[32];
	unsigned char 		*receiveBuf = (unsigned char *)fwdndBuf;
	unsigned char		*nPageBuffer;
	int			 		res = -1;
	unsigned int		*pGoodBlockList;
	unsigned int 		nCRC;

	nBlockNum 			= sNandPartInfo->TotalBlockNum;
	nGangStBlk 			= sNandPartInfo->BlockUpperLimit;
	nGangEdBlk 			= sNandPartInfo->BlockLowerLimit;

	nBlockSize 			= ( gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PageSize 
						+ gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.SpareSize ) << gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].ShiftPpB;

	pGoodBlockList		= gDisplayBuff2;
	nGoodBlockIndex		= 0;

	nBlockSectorSize  	= ((nBlockSize + 511 ) >> 9 );
	
	//==============================
	// Area Block Clear
	//==============================
	if ( sNandPartInfo->PartitionBlockMark == TNFTL_BLOCK_AREA_NANDBOOT_AREA )
	{
		for ( i = nGangStBlk; i < nGangEdBlk; ++i )			// Physical Block
		{
			nPageAddr = i << gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].ShiftPpB;
			NAND_IO_EraseBlock( &gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0], nPageAddr, INTER_LEAVE_OFF );
		}
	}
	else
	{
		for ( i = nGangStBlk; i < nGangEdBlk; ++i )			// FTL Block
		{	
			res = TNFTL_SpareGetDataBlock( gNAND_DrvInfo[0].NFTLDrvInfo, i, 0, 1, cSpare );
		
			bBlkArea = ( cSpare[2] & TNFTL_BLOCK_AREA_MASK );

			if ( bBlkArea == sNandPartInfo->PartitionBlockMark )
			{
				nPageAddr = i << gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].ShiftPpB;
				NAND_IO_EraseBlock( &gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0], nPageAddr, INTER_LEAVE_OFF );
			}
		}
	}

	}

	while ( nBlockNum )
	{	
		if ( nGangStBlk == (nGangEdBlk + 1 ))
			return FALSE;
		
		for ( i = nGangStBlk; i < nGangEdBlk + 1; ++i )
		{
			if ( sNandPartInfo->PartitionBlockMark == TNFTL_BLOCK_AREA_NANDBOOT_AREA )
			{
				nPageAddr = i << gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].ShiftPpB;
				res = NAND_IO_EraseBlock( &gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0], nPageAddr, INTER_LEAVE_OFF );
				if ( res == SUCCESS )
				{
					pGoodBlockList[nGoodBlockIndex] = i;
					++nGoodBlockIndex;
					
					//res = TRUE;
					nGangStBlk = ( i + 1 );
					break;
				}
			}
			else
			{
				res = TNFTL_SpareGetDataBlock( gNAND_DrvInfo[0].NFTLDrvInfo, i, 0, 1, cSpare );
				if (( cSpare[2] == 0xFF ) && ( cSpare[5] == 0xFF ) && ( cSpare[6] == 0xFF ) && ( cSpare[7] == 0xFF ))
				{
					nPageAddr = i << gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].ShiftPpB;
					res = NAND_IO_EraseBlock( &gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0], nPageAddr, INTER_LEAVE_OFF );
					if ( res == SUCCESS )
					{
						pGoodBlockList[nGoodBlockIndex] = i;
						++nGoodBlockIndex;
						
						//res = TRUE;
						nGangStBlk = ( i + 1 );
						break;
					}
				}				
			}
		}

		nReceiveSectorCount = nBlockSectorSize;
		nPageBuffer = receiveBuf;

		while(nReceiveSectorCount)
		{
			if( nReceiveSectorCount > TbufferSectorSize )
				nReceiveSector = TbufferSectorSize;
			else
				nReceiveSector = (unsigned short)nReceiveSectorCount;

			if( fxnFwdnDrvRequestData(receiveBuf, nReceiveSector << 9 ) != 0 )
				res = -1;
			
			nReceiveSectorCount -= nReceiveSector;
		}

		if ( res == SUCCESS )
		{
			for ( j = 0; j < gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PpB; ++j )
			{
				NAND_GANG_WritePage( nPageAddr + j, nPageBuffer );
				nPageBuffer += ( gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PageSize + gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.SpareSize );
			}
		}

		nBlockNum -= 1;

		if ( nBlockNum == 0 )
		{
			res = SUCCESS;
			goto GANG_WRITE_COMPLETE;
		}
	}

GANG_WRITE_COMPLETE:

	// Check CRC
	nPageBuffer		= receiveBuf;
	nBlockNum 		= sNandPartInfo->TotalBlockNum;
	nCRC 			= 0;
	
	for ( i = 0; i < nBlockNum; ++i )
	{
		nBlockAddr	= pGoodBlockList[i];
		nPageAddr 	= nBlockAddr << gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].ShiftPpB;

		for ( j = 0; j < gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PpB; ++j )
		{
			res = NAND_GANG_ReadPage( nPageAddr + j, nPageBuffer );
			if ( res != SUCCESS )
				return FALSE;

			nCRC = FWUG_CalcCrc8I( nCRC, nPageBuffer,(unsigned int)(gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PageSize),CRC32_TABLE );				
		}
	}

	if(res==SUCCESS)
		return TRUE;
	else
		return FALSE;
}
#endif

int FWDN_DRV_AREA_Write( char *name
								,unsigned int lba
								,unsigned int nSector
								,unsigned int nRemain
								,FXN_FWDN_DRV_Response_RequestData fxnFwdnDrvResponseRequestData
								,FXN_FWDN_DRV_DirectReadFromHost fxnFwdnDrvDirectReadFromHost )
{
//#define BUNCH_SECTOR_SIZE		(0x80000000>>9)
#define BUNCH_SECTOR_SIZE		(1024/*KB*/*2)

	unsigned char *receiveBuf;// = (unsigned char *)fwdndBuf;
	//unsigned char *verifyBuf  = (unsigned char *)veriBuf;
	unsigned short nReceiveSector;
	unsigned int nReceiveBytes;
	int _result = TRUE;
	unsigned int nBunch;

#if defined(_LINUX_) && defined(NAND_INCLUDE) && defined(TNFTL_V7_INCLUDE)
	if( _loc_strcmp(FWDN_NAND_MTD_AREA_NAME,name) == 0 )
	{
		unsigned short wPart = (unsigned short)((nRemain>>16)&0xFFFF);
		unsigned int nSectorRemain = nRemain&0xFFFF;
		unsigned long long  nLength = (unsigned long long)((nSector<<9) + nSectorRemain);
		return _loc_MtdWrite(wPart,nLength,fxnFwdnDrvResponseRequestData,fxnFwdnDrvDirectReadFromHost);
	}
#endif

	if( nSector > BUNCH_SECTOR_SIZE )
	{
		nBunch = BUNCH_SECTOR_SIZE;
	}
	else
	{
		nBunch = nSector;
	}
	nSector -= nBunch;
	fxnFwdnDrvResponseRequestData(nBunch<<9);

	{
		while(nBunch)
		{
#if 0//TEST
			if( fxnFwdnDrvReadFromHost(NULL, nReceiveSector<<9) != nReceiveSector<<9 )
			{
				return FALSE;
			}
#else
			nReceiveBytes = fxnFwdnDrvDirectReadFromHost((void**)&receiveBuf);
			if((nReceiveBytes&0x1FF)!=0 || receiveBuf == NULL)
			//if( fxnFwdnDrvReadFromHost(receiveBuf, nReceiveSector<<9) != nReceiveSector<<9 )
			{
				return FALSE;
			}
			nReceiveSector = (unsigned short)(nReceiveBytes>>9);
			nBunch -= nReceiveSector;

			if(nBunch == 0 && nSector > 0 && _result == TRUE)
			{
				if( nSector > BUNCH_SECTOR_SIZE )
				{
					nBunch = BUNCH_SECTOR_SIZE;
				}
				else
				{
					nBunch = nSector;
				}
				nSector -= nBunch;
				fxnFwdnDrvResponseRequestData(nBunch<<9);
			}

			if(_result == TRUE && _loc_AreaWrite(name,lba,nReceiveSector,receiveBuf)!=0)
			{
				FWDN_DRV_SetErrorCode(ERR_FWDN_DRV_AREA_WRITE);
				_result = FALSE;
			}
			//else if(_result == TRUE && _loc_DiskRead(name,lba,nReceiveSector,verifyBuf)!=0)
			//{
			//	FWDN_DRV_SetErrorCode(ERR_FWDN_DRV_AREA_READ);
			//	_result = FALSE;
			//}
			//else if _result == TRUE && (memcmp(receiveBuf, verifyBuf, nReceiveSector<<9) != 0)
			//{
			//	FWDN_DRV_SetErrorCode(ERR_FWDN_DRV_AREA_WRITE_COMPARE);
			//	_result = FALSE;
			//}
#endif

			lba += nReceiveSector;
		}
	}
	return _result;
}


int FWDN_DRV_AREA_Read( char *name
								,unsigned int lba
								,unsigned int nSector
								,unsigned int nRemain
								,FXN_FWDN_DRV_Response_NotifyData fxnFwdnDrvResponseNotifyData
								,FXN_FWDN_DRV_SendToHost fxnFwdnDrvSendToHost )
{
//#define BUNCH_SECTOR_SIZE		(0x80000000>>9)
#define BUNCH_SECTOR_SIZE		(1024/*KB*/*2)

	unsigned char *receiveBuf;// = (unsigned char *)fwdndBuf;
	//unsigned char *verifyBuf  = (unsigned char *)veriBuf;
	unsigned short nReceiveSector;
	unsigned int nReceiveBytes;
	int _result = TRUE;
	unsigned int nBunch;

#if defined(_LINUX_) && defined(NAND_INCLUDE) && defined(TNFTL_V7_INCLUDE)
	if( _loc_strcmp(FWDN_NAND_MTD_AREA_NAME,name) == 0 )
	{
		unsigned short wPart = (unsigned short)((nRemain>>16)&0xFFFF);
		unsigned int nSectorRemain = nRemain&0xFFFF;
		unsigned long long nLength = (unsigned long long)((nSector<<9) + nSectorRemain);
		return _loc_MtdRead(wPart,nLength,fxnFwdnDrvResponseNotifyData,fxnFwdnDrvSendToHost);
	}
	else
#endif
	{
		if(nRemain!=0)
		{
			return FALSE;
		}
	}

	return FALSE;
}


int FWDN_DRV_AREA_CalcCRC( char *name
							,unsigned int lba
							,unsigned int nSector
							,unsigned int nRemain
							,unsigned int *pCrc
							,FXN_FWDN_DRV_SendStatus fxnFwdnDrvSendStatus )
{
#define SEND_STATUS_EVERY_N_SECTOR		2048
	unsigned char *_buf  = (unsigned char *)fwdndBuf;

	unsigned short nReadSector;
	unsigned int nCalcedSector = 0;
	unsigned int nNextSendStatusSector = SEND_STATUS_EVERY_N_SECTOR;

#if defined(_LINUX_) && defined(NAND_INCLUDE) && defined(TNFTL_V7_INCLUDE)
	if( _loc_strcmp(FWDN_NAND_MTD_AREA_NAME,name) == 0 )
	{
		unsigned short wPart = (unsigned short)((nRemain>>16)&0xFFFF);
		unsigned int nSectorRemain = nRemain&0xFFFF;
		unsigned long long nLength = (unsigned long long)((nSector<<9) + nSectorRemain);
		return _loc_MtdCalcCRC(wPart,nLength,pCrc,fxnFwdnDrvSendStatus);
	}
	else
#endif
	{
		if(nRemain!=0)
		{
			return FALSE;
		}
	}

	while(nSector)
	{
		if(nCalcedSector>nNextSendStatusSector)
		{
			nNextSendStatusSector += SEND_STATUS_EVERY_N_SECTOR;
			fxnFwdnDrvSendStatus(nCalcedSector,0,0);
		}

		if( nSector > TbufferSectorSize )
			nReadSector = TbufferSectorSize;
		else
			nReadSector = (unsigned short)nSector;

		if(_loc_AreaRead(name,lba,nReadSector,_buf)!=0)
		{
			FWDN_DRV_SetErrorCode(ERR_FWDN_DRV_AREA_READ);
			return FALSE;
		}
		*pCrc = FWUG_CalcCrc8I(*pCrc,_buf,(unsigned int)nReadSector<<9);

		lba += nReadSector;
		nCalcedSector += nReadSector;
		nSector -= nReadSector;
	}

	return TRUE;
}

#if defined(NAND_INCLUDE) && defined(TNFTL_V7_INCLUDE)
#define	FWDN_DRV_BLOCKREAD_BUFFER_SIZE		(32*1024)		// because of LGE CDC
static int _loc_NAND_DUMP_BlockRead(unsigned int blockAddrParam, unsigned int nBlockParam, unsigned int mediaIndex,
													FXN_FWDN_DRV_Response_NotifyData fxnFwdnDrvResponseNotifyData,
													FXN_FWDN_DRV_SendToHost fxnFwdnDrvSendToHost)
{
	// nBlockAddr	: Read Block Address
	// nPageAddr	: Page Number Per Block
	// nMediaNum	: Media Num
	// nReadBuffer	: Buffer Pointer
	// 한번에 1개의 Physical Page를 Read한다.  Data Size = ( Nand PageSize + Nand SpareSize ) Byte

	unsigned char *pUsbBuffer = (unsigned char *) fwdndBuf;
	unsigned int usbBufferIndex;
	unsigned int nPageAddr;
	unsigned int blockAddr = blockAddrParam;
	unsigned int nNandPhyReadSize;
	unsigned int nBlockSize;
	NAND_IO_DEVINFO *nDevInfo;
	unsigned int requestSize;

	//if( (pUsbBuffer = TC_Allocate_Memory(FWDN_DRV_BLOCKREAD_BUFFER_SIZE)) == NULL )
	//	return FALSE;

	nDevInfo = &gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[mediaIndex];

	nNandPhyReadSize	= nDevInfo->Feature.PageSize + nDevInfo->Feature.SpareSize;
	nBlockSize			= nNandPhyReadSize << nDevInfo->ShiftPpB;

	if( sizeof(fwdndBuf) < nBlockSize || sizeof(fwdndBuf) < FWDN_DRV_BLOCKREAD_BUFFER_SIZE )
	{
		FWDN_DRV_SetErrorCode(ERR_FWDN_DRV_INSUFFICIENT_MEMORY);
		return FALSE;
	}

	while( nBlockParam-- )
	{
		usbBufferIndex = 0;
		nPageAddr = 0;

		while(nBlockSize>usbBufferIndex)
		{
			NAND_PhyReadPage( blockAddr, nPageAddr++, mediaIndex, &pUsbBuffer[usbBufferIndex] );
			usbBufferIndex += nNandPhyReadSize;
		}

		requestSize = nBlockSize;
		usbBufferIndex = 0;

		fxnFwdnDrvResponseNotifyData(requestSize,0);
		while( requestSize>0 )
		{
			unsigned int packetSize = requestSize>FWDN_DRV_BLOCKREAD_BUFFER_SIZE? FWDN_DRV_BLOCKREAD_BUFFER_SIZE : requestSize;
			requestSize -= packetSize;
			fxnFwdnDrvSendToHost( &pUsbBuffer[usbBufferIndex], packetSize );
			usbBufferIndex += packetSize;
		}
		blockAddr++;
	}

	//TC_Deallocate_Memory(pUsbBuffer);
	return TRUE;
}

static int _loc_NAND_DUMP_BlockWrite(unsigned int blockAddrParam, unsigned int nBlockParam, unsigned int mediaIndex,
													FXN_FWDN_DRV_Response_RequestData fxnFwdnDrvResponseRequestData,
													FXN_FWDN_DRV_ReadFromHost fxnFwdnDrvReadFromHost)
{
#if 0
	unsigned int 	nNandPhyWriteSize;
	unsigned int	nBlockSize;
	unsigned int 	nPageAddr;
	unsigned char 	*receiveBuf = (unsigned char *)fwdndBuf;
	NAND_IO_DEVINFO *nDevInfo;
	unsigned int	usbBufferIndex=0;
	unsigned int	requestSize=0;

	nDevInfo = &gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[mediaIndex];

	nNandPhyWriteSize 	= nDevInfo->Feature.PageSize + nDevInfo->Feature.SpareSize;
	nBlockSize			= nNandPhyWriteSize << nDevInfo->ShiftPpB;

	if( sizeof(fwdndBuf) < nBlockSize || sizeof(fwdndBuf)<FWDN_DRV_BLOCKREAD_BUFFER_SIZE )
	{
		FWDN_DRV_SetErrorCode(ERR_FWDN_DRV_INSUFFICIENT_MEMORY);
		return FALSE;
	}

	while(nBlockParam--)
	{
		// Block Erase
		NAND_IO_EraseBlock( nDevInfo, blockAddrParam << nDevInfo->ShiftPpB, INTER_LEAVE_OFF );

		requestSize = nBlockSize;
		fxnFwdnDrvResponseRequestData(requestSize);
		usbBufferIndex=0;
		while(requestSize>0)
		{
			unsigned int packetSize = (requestSize>FWDN_DRV_BLOCKREAD_BUFFER_SIZE)? FWDN_DRV_BLOCKREAD_BUFFER_SIZE : requestSize;
			requestSize -= packetSize;
			fxnFwdnDrvReadFromHost(&receiveBuf[usbBufferIndex],packetSize);
			usbBufferIndex += packetSize;
		}

		usbBufferIndex=0;
		nPageAddr = blockAddrParam << nDevInfo->ShiftPpB;
		while(nBlockSize>usbBufferIndex)
		{
			NAND_GANG_WritePage(nPageAddr++,&receiveBuf[usbBufferIndex]);
			usbBufferIndex += nNandPhyWriteSize;
		}
		blockAddrParam++;
	}
	return TRUE;
#else
	return FALSE;
#endif
}

const unsigned char ImageFileHeaderSignature[] =
{
	"Image file with header"
};

int _loc_NAND_MTD_GetPhyBlkAddr( ROM_HEADER_INFO *sNandInfo, U32 nAreaStBlk, U32 nMTDBlkAddr, U32 *rMTDPhyBlkAddr )
{  
	unsigned int		nBlockPageAddr;
	unsigned int		nPhyBlkAddr;

	//sROM_PartitionInfo.Partition[2].BlockUpperLimit	= sInitInfo->ROAreaStPB;

	if ( sNandInfo->FTLBlk_Composite_Num == NAND_FTL_BLK_COMPOSITE_4WAY )
	{
		nMTDBlkAddr 	-= nAreaStBlk;
		nPhyBlkAddr 	= nMTDBlkAddr >> 1;
		nBlockPageAddr 	= ( nPhyBlkAddr + nAreaStBlk ) * sNandInfo->PhyPpB;

		if ( nMTDBlkAddr & 1 )
		{
			nBlockPageAddr	+= ( ( sNandInfo->PBpV >> 1 ) * sNandInfo->PhyPpB );
		}

		*rMTDPhyBlkAddr = ( nBlockPageAddr / sNandInfo->PhyPpB );
	}
	else
	{		
		*rMTDPhyBlkAddr = nMTDBlkAddr;
	}

	return 0;
}

static void _loc_NAND_TOTALBIN_ConvertIPAtoPPA( ROM_HEADER_INFO *sNandInfo, U32 nImagePageAddr, U16 *rMediaNum, U32* rPHYPageAddr )
{
	unsigned int		i;
	unsigned int		nPhyPageAddr, nPhyBlkAddr;
	unsigned int		nLogPageAddr, nLogBlkAddr, nAddPageIndex;
	unsigned int		MediaTotalFTLPageAddr, dwMediaPageBound;
	//unsigned long int	dwTemp;
	unsigned short int	wMediaOrder, wMdeiaOrderAddNum;

	/* Get Boundary Page Address */
	MediaTotalFTLPageAddr = (unsigned long int)( sNandInfo->FBpV * sNandInfo->PhyPpB );

	dwMediaPageBound = 0;

	if ( sNandInfo->ExtInterleaveUsable == FALSE )
	{
		for ( i = 0; i < sNandInfo->NAND_MEDIA_NUM; ++i )
		{
			wMediaOrder		= i;

			dwMediaPageBound += MediaTotalFTLPageAddr;

			if ( nImagePageAddr < dwMediaPageBound )
			{
				dwMediaPageBound -= MediaTotalFTLPageAddr;
				nImagePageAddr -= dwMediaPageBound;
				break;
			}
		}
	}
	else
	{
		for ( i = 0; i < sNandInfo->NAND_MEDIA_NUM; i+= 2 )
		{
			wMediaOrder		  = i;

			dwMediaPageBound += MediaTotalFTLPageAddr;
			wMediaOrder		  = i + 1;
			dwMediaPageBound += MediaTotalFTLPageAddr;

			if ( nImagePageAddr < dwMediaPageBound )
			{
				dwMediaPageBound -= MediaTotalFTLPageAddr;
				dwMediaPageBound -= MediaTotalFTLPageAddr;
				nImagePageAddr -= dwMediaPageBound;
				break;
			}
		}

		if ( wMediaOrder < 2 )
			wMdeiaOrderAddNum = 0;
		else
			wMdeiaOrderAddNum = 2;
	}

	nLogBlkAddr		= nImagePageAddr / sNandInfo->FTLPpB;
	
	if ( sNandInfo->FTLBlk_Composite_Num == NAND_FTL_BLK_COMPOSITE_1WAY )
		nLogPageAddr	= (nImagePageAddr % (sNandInfo->PhyPpB * sNandInfo->FTLBlk_Composite_Num )) / sNandInfo->FTLBlk_Composite_Num;
	else
		nLogPageAddr	= nImagePageAddr / sNandInfo->FTLBlk_Composite_Num;


	nAddPageIndex	= nImagePageAddr % 4;

	nPhyPageAddr	= (nLogBlkAddr * sNandInfo->PhyPpB) + nLogPageAddr + sNandInfo->RemapPageWeight[nAddPageIndex];
	nPhyBlkAddr		= nPhyPageAddr / sNandInfo->PhyPpB;

	//if ( ( sDevInfo->Feature.DeviceID.Code[0] == TOSHIBA_NAND_MAKER_ID ) && ( ( sDevInfo->Feature.MediaType & S_MP ) || ( sDevInfo->Feature.MediaType & S_MP2 )) )
	//{
	//	j = (U16)(dwLOGBlkNo >> sDevInfo->ShiftDistrictNum);
	//	dwPHYPageAddr[i] += ( sDevInfo->DistrictNum << sDevInfo->ShiftPpB ) * j;
	//}

	if ( sNandInfo->ExtInterleaveUsable == TRUE )
	{
		if ( sNandInfo->FTLBlk_Composite_Num == 2 )
		{
			if ( nAddPageIndex & 1 )
			{
				wMediaOrder = 1;
			}
			else
			{
				wMediaOrder = 0;
			}
		}
		else if ( sNandInfo->FTLBlk_Composite_Num == 4 )
		{
			if ( ( nAddPageIndex >> 1 ) & 1 )
			{
				wMediaOrder = 1;
			}
			else
			{
				wMediaOrder = 0;
			}
		}

		wMediaOrder += wMdeiaOrderAddNum;
	}

	*rMediaNum = wMediaOrder;
	*rPHYPageAddr	= nPhyPageAddr;
}

static int _loc_NAND_TOTALBIN_Read( FXN_FWDN_DRV_Response_NotifyData fxnFwdnDrvResponseNotifyData,
									 FXN_FWDN_DRV_SendToHost fxnFwdnDrvSendToHost)
{
	unsigned int			i = 0;
	unsigned int			j = 0;
	unsigned int			k = 0;
	unsigned int			nNandBlkSize;
	unsigned int			nUseBlockNum;
	unsigned int			nPartitionBlockNum;
	unsigned int			nCRC;
	unsigned int			nBufferIndex;
	unsigned int			rMTDFTLBlk;
	unsigned int 			nPageAddr;
	unsigned int			*nLPTBuffer;
	unsigned char			bBlkArea, bAreaParameter, bBlkType;
	unsigned char			nGodelInfoBuf[512];	
	unsigned char			*pNAND_Buffer;
	unsigned char			*nPageBuffer;	
	unsigned char			*nReadBuffer;
	unsigned char			*nSpareBuffer;
	unsigned char			*cSpare;
	NAND_GOLDEN_INFO		*sGoldenInfo;
	NAND_BOOTLOADER_INFO	*sBootLoaderInfo;
	TNFTL_INIT_INFO			*sInitInfo;
	ROM_HEADER_INFO			sROM_HeaderInfo;
	ROM_PARTITION_INFO		sROM_PartitionInfo;
	TNFTL_ERROR				res;
	unsigned int			nTotalBlockNum,nReadBlockNum,nRWAreaBlkThreshHoldForProgress;

	pNAND_Buffer	= (unsigned char*)0x40400000;
	nReadBuffer		= pNAND_Buffer + gNAND_DrvInfo[0].NFTLDrvInfo->PageSize + 1024;
	nSpareBuffer	= nReadBuffer + gNAND_DrvInfo[0].NFTLDrvInfo->PageSize;
	cSpare			= nSpareBuffer;
	nPageBuffer 	= nSpareBuffer + 1024;		// nPageBuffer Size = PageSize + SpareSize

	ND_TRACE("\n\n+Make Gang Image\n");

	//--------------------------------
	//	Initialize Parameter
	//--------------------------------
	nNandBlkSize = ( gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PageSize + gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.SpareSize ) * gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PpB;
	nLPTBuffer		= (unsigned int*)(nPageBuffer + nNandBlkSize + 512);

	NAND_IO_ReadPage( &gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0], 0, 0, 1, nGodelInfoBuf, cSpare, ECC_ON );
	sGoldenInfo 	= (NAND_GOLDEN_INFO *)&nGodelInfoBuf[0];
	sBootLoaderInfo = (NAND_BOOTLOADER_INFO *)&nGodelInfoBuf[sizeof( NAND_GOLDEN_INFO )];
	sInitInfo 		= (TNFTL_INIT_INFO *)&nGodelInfoBuf[NAND_SB_BOOT_PAGE_ECC_SIZE];
	// Set BMP Signature
	sInitInfo->BMPSignature[0] = 'B';
	sInitInfo->BMPSignature[1] = 'M';
	sInitInfo->BMPSignature[2] = 'P';
	sInitInfo->BMPSignature[3] = 'G';
	
	//--------------------------------
	// WCache Flush
	//--------------------------------
	ND_TRACE("+WCache Flush\n");
	TNFTL_WCacheFourceFlushCache_ALL( gNAND_DrvInfo[0].NFTLDrvInfo, &gNAND_DrvInfo[0].NFTLDrvInfo->PriPartition );

	for ( i = 0; i < gNAND_DrvInfo[0].NFTLDrvInfo->ExtendedPartitionNo; ++i )
		TNFTL_WCacheFourceFlushCache_ALL( gNAND_DrvInfo[0].NFTLDrvInfo, &gNAND_DrvInfo[0].NFTLDrvInfo->ExtPartition[i] );
	ND_TRACE("-WCache Flush\n");

	//--------------------------------
	// Make Image Header
	//--------------------------------

	// Set Header Information
	sROM_HeaderInfo.PartitionTableOffset	= (gNAND_DrvInfo[0].NFTLDrvInfo->PageSize + gNAND_DrvInfo[0].NFTLDrvInfo->SpareSize);//ROM_HEADER_INFO_SIZE;
	sROM_HeaderInfo.BinaryImageOffset		= nNandBlkSize; //(ROM_HEADER_INFO_SIZE +ROM_PARTITION_INFO_SIZE);
	
	memset(&sROM_HeaderInfo.HeaderSignature[0],0x00, 24 );
	memcpy(&sROM_HeaderInfo.HeaderSignature[0], ImageFileHeaderSignature, sizeof(ImageFileHeaderSignature) );
	sROM_HeaderInfo.ImageVersion			= 7;//nImageVersion;		// Image Version
	
	// Target NAND Info
	sROM_HeaderInfo.NAND_DRIVER_VERSION[0]	= 'V';
	sROM_HeaderInfo.NAND_DRIVER_VERSION[1]	= '7';
	sROM_HeaderInfo.NAND_DRIVER_VERSION[2]	= '0';
	sROM_HeaderInfo.NAND_DRIVER_VERSION[3]	= '0';
	sROM_HeaderInfo.NAND_DRIVER_VERSION[4]	= '7';

	for ( i = 0; i < 6; ++i)
		sROM_HeaderInfo.NAND_DEVICE_CODE[i]= (U8)gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.DeviceID.Code[i];

	sROM_HeaderInfo.NAND_MEDIA_NUM			= (U8)gNAND_DrvInfo[0].NFTLDrvInfo->MediaNums;
	sROM_HeaderInfo.PBpV					= (U16)gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PBpV;	// Physical all Block Number
	sROM_HeaderInfo.FBpV					= (U16)gNAND_DrvInfo[0].NFTLDrvInfo->PBpV;						// FTL all Block Number
	sROM_HeaderInfo.PpB						= gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PpB;// Page Number Per Block
	sROM_HeaderInfo.PageSize				= gNAND_DrvInfo[0].NFTLDrvInfo->PageSize;					// Page Size
	sROM_HeaderInfo.SpareSize				= gNAND_DrvInfo[0].NFTLDrvInfo->SpareSize;					// Spare Size

	if ( ( gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.MediaType & S_MP ) || ( gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.MediaType & S_MCP ) )
	{
		if ( gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.MediaType & S_IL )
		{
			sROM_HeaderInfo.FTLBlk_Composite_Num	= NAND_FTL_BLK_COMPOSITE_4WAY;		// Normal NAND : 1, MP : 2, MP+IL: 4
		}
		else
		{
			sROM_HeaderInfo.FTLBlk_Composite_Num	= NAND_FTL_BLK_COMPOSITE_2WAY;		// Normal NAND : 1, MP : 2, MP+IL: 4
		}
	}
	else
	{
		sROM_HeaderInfo.FTLBlk_Composite_Num	= NAND_FTL_BLK_COMPOSITE_1WAY;			// Normal NAND : 1, MP : 2, MP+IL: 4
	}

	if ( gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].ExtInterleaveUsable == TRUE )
		sROM_HeaderInfo.ExtInterleaveUsable = TRUE;
	else
		sROM_HeaderInfo.ExtInterleaveUsable = FALSE;

	sROM_HeaderInfo.ECCType	= gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].EccType;
	
	// FTL Info	: Multi-Plane, InterLeave
	sROM_HeaderInfo.FTLPpB					= gNAND_DrvInfo[0].NFTLDrvInfo->PpB;		// FTL Pages Per Block	(PhyPpB * FTL
	sROM_HeaderInfo.PhyPpB					= gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PpB;		// NAND Pages Per Block
	sROM_HeaderInfo.RemapPageWeight[0]		= gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].RemapPageWeight[0];
	sROM_HeaderInfo.RemapPageWeight[1]		= gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].RemapPageWeight[1];
	sROM_HeaderInfo.RemapPageWeight[2]		= gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].RemapPageWeight[2];
	sROM_HeaderInfo.RemapPageWeight[3]		= gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].RemapPageWeight[3];
			
	//--------------------------------
	// Make Partition Info
	//--------------------------------
	sROM_PartitionInfo.NAND_Partition_Num	= 4;	//  ROArea:2 + RWArea:1  

	// ROArea [0]: 2nd bootloader
	sROM_PartitionInfo.Partition[0].BlockUpperLimit	= 0;
	sROM_PartitionInfo.Partition[0].BlockLowerLimit	= sGoldenInfo->BlockLowerLimit;

	// ROArea [1]: tcboot.rom
	sROM_PartitionInfo.Partition[1].BlockUpperLimit	= sBootLoaderInfo->BL_BlockUppuerLimit;
	sROM_PartitionInfo.Partition[1].BlockLowerLimit	= sBootLoaderInfo->BL_BlockLowerLimit;//gNAND_DrvInfo[0].NFTLDrvInfo->RwAreaStBlk-1;

	// MTD Area
	if ( sInitInfo->ROAreaSize != 0 )
	{
		sROM_PartitionInfo.Partition[2].BlockUpperLimit	= sInitInfo->ROAreaStPB;
		sROM_PartitionInfo.Partition[2].BlockLowerLimit	= sInitInfo->ROAreaSize;

		if ( gTNFTL_DrvInfo[0].MediaDevInfo->Feature.PageSize == 16384 )
			sROM_PartitionInfo.Partition[2].BlockLowerLimit = sROM_PartitionInfo.Partition[2].BlockLowerLimit >> 13;	// 8192 Page
		else
			sROM_PartitionInfo.Partition[2].BlockLowerLimit = sROM_PartitionInfo.Partition[2].BlockLowerLimit >> gTNFTL_DrvInfo[0].MediaDevInfo->ShiftPageSize;

		if ( gTNFTL_DrvInfo[0].MediaDevInfo->Feature.PpB > 128 )
			sROM_PartitionInfo.Partition[2].BlockLowerLimit = sROM_PartitionInfo.Partition[2].BlockLowerLimit >> 7;		// 128 PpB
		else
			sROM_PartitionInfo.Partition[2].BlockLowerLimit = sROM_PartitionInfo.Partition[2].BlockLowerLimit >> gTNFTL_DrvInfo[0].MediaDevInfo->ShiftPpB;

		sROM_PartitionInfo.Partition[2].BlockLowerLimit += sROM_PartitionInfo.Partition[2].BlockUpperLimit;		
	}
	else
	{
		sROM_PartitionInfo.Partition[2].BlockUpperLimit	= 0;
		sROM_PartitionInfo.Partition[2].BlockLowerLimit	= 0;
	}

	for ( i = 0; i < 8; ++i )
	{
		//FTL Block Address --> Physical Block Address
		sROM_PartitionInfo.Partition[3 + i].BlockUpperLimit = gNAND_DrvInfo[0].NFTLDrvInfo->RwAreaStBlk;
		sROM_PartitionInfo.Partition[3 + i].BlockLowerLimit = gNAND_DrvInfo[0].NFTLDrvInfo->RwAreaEdBlk - 1;
	}

	//=================================
	sROM_PartitionInfo.NAND_Gang_Partition[0].BlockUpperLimit = sROM_PartitionInfo.Partition[0].BlockUpperLimit;
	sROM_PartitionInfo.NAND_Gang_Partition[0].BlockLowerLimit = sROM_PartitionInfo.Partition[0].BlockLowerLimit;
	sROM_PartitionInfo.NAND_Gang_Partition[0].PartitionName[0] = '1';
	sROM_PartitionInfo.NAND_Gang_Partition[0].PartitionName[1] = 's';
	sROM_PartitionInfo.NAND_Gang_Partition[0].PartitionName[2] = 't';
	sROM_PartitionInfo.NAND_Gang_Partition[0].PartitionName[3] = 'B';

	sROM_PartitionInfo.NAND_Gang_Partition[1].BlockUpperLimit = sROM_PartitionInfo.Partition[1].BlockUpperLimit;
	sROM_PartitionInfo.NAND_Gang_Partition[1].BlockLowerLimit = sROM_PartitionInfo.Partition[1].BlockLowerLimit;
	sROM_PartitionInfo.NAND_Gang_Partition[1].PartitionName[0] = '2';
	sROM_PartitionInfo.NAND_Gang_Partition[1].PartitionName[1] = 'n';
	sROM_PartitionInfo.NAND_Gang_Partition[1].PartitionName[2] = 'd';
	sROM_PartitionInfo.NAND_Gang_Partition[1].PartitionName[3] = 'B';

	sROM_PartitionInfo.NAND_Gang_Partition[2].BlockUpperLimit = sROM_PartitionInfo.Partition[2].BlockUpperLimit;
	sROM_PartitionInfo.NAND_Gang_Partition[2].BlockLowerLimit = sROM_PartitionInfo.Partition[2].BlockLowerLimit;
	sROM_PartitionInfo.NAND_Gang_Partition[2].PartitionName[0] = 'M';
	sROM_PartitionInfo.NAND_Gang_Partition[2].PartitionName[1] = 'T';
	sROM_PartitionInfo.NAND_Gang_Partition[2].PartitionName[2] = 'D';
	sROM_PartitionInfo.NAND_Gang_Partition[2].PartitionName[3] = '-';

	sROM_PartitionInfo.NAND_Gang_Partition[3].BlockUpperLimit = sROM_PartitionInfo.Partition[3].BlockUpperLimit;
	sROM_PartitionInfo.NAND_Gang_Partition[3].BlockLowerLimit = sROM_PartitionInfo.Partition[3].BlockLowerLimit;
	sROM_PartitionInfo.NAND_Gang_Partition[3].PartitionName[0] = 'M';
	sROM_PartitionInfo.NAND_Gang_Partition[3].PartitionName[1] = 'A';
	sROM_PartitionInfo.NAND_Gang_Partition[3].PartitionName[2] = 'I';
	sROM_PartitionInfo.NAND_Gang_Partition[3].PartitionName[3] = 'N';


	for ( i = 0; i < 4; ++i )
		ND_TRACE("+Area[%d] - BlkUp:%d ~ BlkDown:%d\n", i, sROM_PartitionInfo.Partition[i].BlockUpperLimit, sROM_PartitionInfo.Partition[i].BlockLowerLimit );


	//--------------------------------
	// Make LPT
	//--------------------------------
	ND_TRACE("+LPT Buffer Init\n");
	ND_TRACE(":LPT Table Num:%d\n", gNAND_DrvInfo[0].NFTLDrvInfo->PBpV );
	for ( i = 0; i < gNAND_DrvInfo[0].NFTLDrvInfo->PBpV; ++i )
		nLPTBuffer[i] = 0xFFFFFFFF;
	ND_TRACE("-LPT Buffer Init\n");

	//==============================================
	// ROArea[0] Block List-Up --> Physical Block
	//==============================================
	nCRC = 0;
	nUseBlockNum = 0;
	ND_TRACE("+Scan Block ROArea[0]\n");

	nTotalBlockNum = sROM_PartitionInfo.Partition[0].BlockLowerLimit - sROM_PartitionInfo.Partition[0].BlockUpperLimit;
	nReadBlockNum = 0;
	
	for ( i = sROM_PartitionInfo.Partition[0].BlockUpperLimit; i < sROM_PartitionInfo.Partition[0].BlockLowerLimit; ++i )
	{
		++nReadBlockNum;

		if ( i == 0 )
		{
			nLPTBuffer[nUseBlockNum] = i;		// Golden Block 
			//ND_TRACE("\nLPTBlk[%d]", nLPTBuffer[nUseBlockNum] );
			++nUseBlockNum;
			for ( k = 0; k < gTNFTL_DrvInfo[0].MediaDevInfo->Feature.PpB; ++k )
			{
				NAND_IO_ReadPage( gTNFTL_DrvInfo[0].MediaDevInfo, 
								 ((i << gTNFTL_DrvInfo[0].MediaDevInfo->ShiftPpB ) + k ), 0, gTNFTL_DrvInfo[0].MediaDevInfo->PPages, 
								 nReadBuffer, nSpareBuffer, ECC_ON );

				nCRC = FWUG_CalcCrc8I( nCRC, nReadBuffer,(unsigned int)(gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PageSize));
			}
		}
		else if ( i == 1 )
		{
			nLPTBuffer[nUseBlockNum] = i;		// Golden Block Back
			//ND_TRACE("\nLPTBlk[%d]", nLPTBuffer[nUseBlockNum] );
			++nUseBlockNum;
			for ( k = 0; k < gTNFTL_DrvInfo[0].MediaDevInfo->Feature.PpB; ++k )
			{
				NAND_IO_ReadPage( gTNFTL_DrvInfo[0].MediaDevInfo, 
								 ((i << gTNFTL_DrvInfo[0].MediaDevInfo->ShiftPpB ) + k ), 0, gTNFTL_DrvInfo[0].MediaDevInfo->PPages, 
								 nReadBuffer, nSpareBuffer, ECC_ON );

				nCRC = FWUG_CalcCrc8I( nCRC, nReadBuffer,(unsigned int)(gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PageSize));
			}
		}
		else
		{
			res = TNFTL_IOReadPhySpare( gTNFTL_DrvInfo[0].MediaDevInfo, i, 0, cSpare );

			if ( res == SUCCESS )
			{
				if (!(( cSpare[2] == 0xFF ) && ( cSpare[5] == 0xFF ) && ( cSpare[6] == 0xFF ) && ( cSpare[7] == 0xFF )))
				{
					nLPTBuffer[nUseBlockNum] = i;		// Golden Block Back
					//ND_TRACE("\nLPTBlk[%d]", nLPTBuffer[nUseBlockNum] );
					++nUseBlockNum;

					for ( k = 0; k < gTNFTL_DrvInfo[0].MediaDevInfo->Feature.PpB; ++k )
					{
						NAND_IO_ReadPage( gTNFTL_DrvInfo[0].MediaDevInfo, 
										 ((i << gTNFTL_DrvInfo[0].MediaDevInfo->ShiftPpB ) + k ), 0, gTNFTL_DrvInfo[0].MediaDevInfo->PPages, 
										 nReadBuffer, nSpareBuffer, ECC_ON );

						nCRC = FWUG_CalcCrc8I( nCRC, nReadBuffer,(unsigned int)(gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PageSize));
					}
				}
			}
		}

		fxnFwdnDrvResponseNotifyData(0,(nReadBlockNum*7)/nTotalBlockNum);
	}
	
	sROM_PartitionInfo.Partition[0].PartitionCRC = nCRC;
	sROM_PartitionInfo.Partition[0].TotalBlockNum = nUseBlockNum;
	sROM_PartitionInfo.NAND_Gang_Partition[0].TotalBlockNum = sROM_PartitionInfo.Partition[0].TotalBlockNum;
	sROM_PartitionInfo.Partition[0].PartitionBlockMark = TNFTL_BLOCK_AREA_NANDBOOT_AREA;
	ND_TRACE(":ROArea[0] StartBlk:%d, EndBlk:%d, TotalBlockNum:%d\n", 
	sROM_PartitionInfo.Partition[0].BlockUpperLimit, sROM_PartitionInfo.Partition[0].BlockLowerLimit, sROM_PartitionInfo.Partition[0].TotalBlockNum );		
	ND_TRACE("-Scan Block ROArea[0]\n");

	//==============================================
	// ROArea[1] Block List-Up	 --> Phy Block
	//==============================================
	nCRC = 0;
	ND_TRACE("+Scan Block ROArea[1]\n");

	nTotalBlockNum = sROM_PartitionInfo.Partition[1].BlockLowerLimit - sROM_PartitionInfo.Partition[1].BlockUpperLimit;
	nReadBlockNum = 0;
	
	for ( i = sROM_PartitionInfo.Partition[1].BlockUpperLimit; i < sROM_PartitionInfo.Partition[1].BlockLowerLimit; ++i )
	{
		++nReadBlockNum;
	
		res = TNFTL_IOReadPhySpare( gTNFTL_DrvInfo[0].MediaDevInfo, i, 0, cSpare );
		
		if ( res == SUCCESS )
		{
			if (!(( cSpare[2] == 0xFF ) && ( cSpare[5] == 0xFF ) && ( cSpare[6] == 0xFF ) && ( cSpare[7] == 0xFF )))
			{
				nLPTBuffer[nUseBlockNum] = i;		// Golden Block Back
				//ND_TRACE("\nLPTBlk[%d]", nLPTBuffer[nUseBlockNum] );
				++nUseBlockNum;

				for ( k = 0; k < gTNFTL_DrvInfo[0].MediaDevInfo->Feature.PpB; ++k )
				{
					NAND_IO_ReadPage( gTNFTL_DrvInfo[0].MediaDevInfo, 
									((i << gTNFTL_DrvInfo[0].MediaDevInfo->ShiftPpB ) + k ), 0, gTNFTL_DrvInfo[0].MediaDevInfo->PPages, 
									nReadBuffer, nSpareBuffer, ECC_ON );

					nCRC = FWUG_CalcCrc8I( nCRC, nReadBuffer,(unsigned int)(gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PageSize));
				}
			}
		}
		fxnFwdnDrvResponseNotifyData(0,7+(nReadBlockNum*8)/nTotalBlockNum);

	}

	sROM_PartitionInfo.Partition[1].PartitionCRC = nCRC;
	sROM_PartitionInfo.Partition[1].TotalBlockNum = nUseBlockNum - sROM_PartitionInfo.Partition[0].TotalBlockNum;
	sROM_PartitionInfo.NAND_Gang_Partition[1].TotalBlockNum = sROM_PartitionInfo.Partition[1].TotalBlockNum;
	sROM_PartitionInfo.Partition[1].PartitionBlockMark = TNFTL_BLOCK_AREA_NANDBOOT_AREA;
	ND_TRACE(":ROArea[1] StartBlk:%d, EndBlk:%d, TotalBlockNum:%d\n", 
	sROM_PartitionInfo.Partition[1].BlockUpperLimit, sROM_PartitionInfo.Partition[1].BlockLowerLimit, sROM_PartitionInfo.Partition[1].TotalBlockNum );	
	ND_TRACE("-Scan Block ROArea[1]\n");

	//==============================================
	// MTD Area Block List-Up	 --> Interleave Block
	//==============================================
	nCRC = 0;
	ND_TRACE("+Scan Block MTD Area\n");

	nTotalBlockNum = sROM_PartitionInfo.Partition[2].BlockLowerLimit - sROM_PartitionInfo.Partition[2].BlockUpperLimit;

	ND_TRACE("nTotalBlockNum:%d\n", nTotalBlockNum);

	if ( nTotalBlockNum != 0 )
	{
		nReadBlockNum = 0;
		
		for ( i = sROM_PartitionInfo.Partition[2].BlockUpperLimit; i < sROM_PartitionInfo.Partition[2].BlockLowerLimit; ++i )
		{
			U32 nChipNo;
			++nReadBlockNum;

			NAND_MTD_GetPhyBlkAddr( gTNFTL_DrvInfo[0].MediaDevInfo, i, &rMTDFTLBlk, &nChipNo );
			nPageAddr = rMTDFTLBlk << gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].ShiftPpB;

			res = NAND_IO_ReadSpareMTD( gTNFTL_DrvInfo[0].MediaDevInfo, nPageAddr, cSpare );
			
			if ( res == SUCCESS )
			{
				if ( ( cSpare[0] == 0xFF ) && ( cSpare[1] == 0xFF ) && ( cSpare[2] != 0xFF ) )
				{					
					nLPTBuffer[nUseBlockNum] = rMTDFTLBlk;
					//ND_TRACE("\nLPTBlk[%d]", nLPTBuffer[nUseBlockNum] );
					++nUseBlockNum;

					for ( k = 0; k < gTNFTL_DrvInfo[0].MediaDevInfo->Feature.PpB; ++k )
					{
						NAND_MTD_ReadPage( nPageAddr + k, nReadBuffer );
						if ( gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PageSize == 16384 )
							nCRC = FWUG_CalcCrc8I( nCRC, nReadBuffer, 8192  );
						else
							nCRC = FWUG_CalcCrc8I( nCRC, nReadBuffer,(unsigned int)(gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PageSize) );							
					}
				}
			}
			
			//fxnFwdnDrvResponseNotifyData(0,7+(nReadBlockNum*8)/nTotalBlockNum);
		}		
	}

	sROM_PartitionInfo.Partition[2].PartitionCRC = nCRC;
	sROM_PartitionInfo.Partition[2].TotalBlockNum = nUseBlockNum - ( sROM_PartitionInfo.Partition[0].TotalBlockNum + sROM_PartitionInfo.Partition[1].TotalBlockNum );
	sROM_PartitionInfo.NAND_Gang_Partition[2].TotalBlockNum = sROM_PartitionInfo.Partition[2].TotalBlockNum;
	sROM_PartitionInfo.Partition[2].PartitionBlockMark = TNFTL_BLOCK_AREA_NANDBOOT_AREA;
	ND_TRACE(":MTD Area StartBlk:%d, EndBlk:%d, TotalBlockNum:%d\n", 
	sROM_PartitionInfo.Partition[2].BlockUpperLimit, sROM_PartitionInfo.Partition[2].BlockLowerLimit, sROM_PartitionInfo.Partition[2].TotalBlockNum );	
	ND_TRACE("-Scan Block MTD Area\n");



	
	//==============================================
	// RWArea Block List-Up		--> FTL Block
	//==============================================
	ND_TRACE("+Scan Block RWArea\n");
	nRWAreaBlkThreshHoldForProgress = (gNAND_DrvInfo[0].NFTLDrvInfo->RwAreaEdBlk - gNAND_DrvInfo[0].NFTLDrvInfo->RwAreaStBlk)/2;
	nRWAreaBlkThreshHoldForProgress += gNAND_DrvInfo[0].NFTLDrvInfo->RwAreaStBlk;
	for ( j = 0; j < 10; ++j )
	{
		bAreaParameter = (( 0x10 * j ) + 0x10);
		sROM_PartitionInfo.Partition[j + 3].PartitionBlockMark = bAreaParameter;

		nPartitionBlockNum = 0;
		nCRC = 0;

		for ( i = gNAND_DrvInfo[0].NFTLDrvInfo->RwAreaStBlk; i < gNAND_DrvInfo[0].NFTLDrvInfo->RwAreaEdBlk; ++i )
		{
			if(i == gNAND_DrvInfo[0].NFTLDrvInfo->RwAreaStBlk)
			{
				fxnFwdnDrvResponseNotifyData(0,15+(j*(2*35))/22);
			}
			else if(i == nRWAreaBlkThreshHoldForProgress)
			{
				fxnFwdnDrvResponseNotifyData(0,15+((j+1)*(2*35))/22);
			}

		 	res = TNFTL_SpareGetDataBlock( gNAND_DrvInfo[0].NFTLDrvInfo, i, 0, 1, cSpare );
			if ( res == SUCCESS ) 
			{
				if (( cSpare[2] == 0xFF ) && ( cSpare[5] == 0xFF ) && ( cSpare[6] == 0xFF ) && ( cSpare[7] == 0xFF ))
				{
					
				}
				else
				{
					bBlkArea = ( cSpare[2] & TNFTL_BLOCK_AREA_MASK );
					bBlkType = ( cSpare[2] & TNFTL_BLOCK_TYPE_MASK );
					
					if (( bBlkArea == bAreaParameter ) && ( bBlkType != TNFTL_BLOCK_TYPE_LBT ))
					{
						nLPTBuffer[nUseBlockNum] = i;		// FTL Block Address
						//ND_TRACE("\nLPTBlk[%d]", nLPTBuffer[nUseBlockNum] );
						
						++nUseBlockNum;
						++nPartitionBlockNum;

						for ( k = 0; k < gNAND_DrvInfo[0].NFTLDrvInfo->PpB; ++k )
						{
							TNFTL_IOReadPage( &gTNFTL_DrvInfo[0],
									  		  ((i << gTNFTL_DrvInfo[0].ShiftPpB) + k ),
									  		  0, gTNFTL_DrvInfo[0].PPages,
									  		  nReadBuffer,
									  		  nSpareBuffer,
									  		  ECC_ON );

							nCRC = FWUG_CalcCrc8I( nCRC, nReadBuffer,(unsigned int)(gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PageSize));
						}
					}
				}
			}
		}
	
		sROM_PartitionInfo.Partition[j + 3].TotalBlockNum = nPartitionBlockNum;
		sROM_PartitionInfo.Partition[j + 3].PartitionCRC = nCRC;
	}

	ND_TRACE(":RWArea StartFTLBlk:%d, EndFTLBlk:%d, TotalFTLBlockNum:%d\n", 
	sROM_PartitionInfo.Partition[3].BlockUpperLimit, sROM_PartitionInfo.Partition[3].BlockLowerLimit, sROM_PartitionInfo.Partition[3].TotalBlockNum );
	
	ND_TRACE("-Scan Block RWArea\n");

	memset(sROM_PartitionInfo.Partition[3].PartitionName,0x00,16);
	_loc_strncpy( sROM_PartitionInfo.Partition[3].PartitionName, FWDN_NAND_DATA_AREA_NAME, 16 );
	
	for ( i = 0; i < 7; ++i )
	{
		memset(sROM_PartitionInfo.Partition[4+i].PartitionName,0x00,16);
		_loc_strncpy( sROM_PartitionInfo.Partition[4+i].PartitionName, FWDN_NAND_HD_AREA_NAME[i], 16 );
	}

	// Init Gang Partition Info
	sROM_PartitionInfo.NAND_Gang_Partition[3].TotalBlockNum = nUseBlockNum - (sROM_PartitionInfo.Partition[0].TotalBlockNum + sROM_PartitionInfo.Partition[1].TotalBlockNum + sROM_PartitionInfo.Partition[2].TotalBlockNum );
	
	//==============================================
	// Write Total Image
	//==============================================
	// 1. Write [nInfoBuffer: 4kbyte]
	memset( nPageBuffer, 0xFF, nNandBlkSize );
	memcpy( nPageBuffer, &sROM_HeaderInfo, sizeof(ROM_HEADER_INFO) );
	memcpy( &nPageBuffer[(gNAND_DrvInfo[0].NFTLDrvInfo->PageSize + gNAND_DrvInfo[0].NFTLDrvInfo->SpareSize)], &sROM_PartitionInfo, sizeof(ROM_PARTITION_INFO) );

	fxnFwdnDrvResponseNotifyData(nNandBlkSize,50);
	fxnFwdnDrvSendToHost( &nPageBuffer[0], nNandBlkSize );

	if(nUseBlockNum == 0)
		return FALSE;

	// 2. Virtual NAND Image --> Total Image

	nTotalBlockNum = nUseBlockNum;
	nReadBlockNum = 0;

	nUseBlockNum = 0;

	for ( j = 0; j < 4; ++j )	// R/O Area[0], R/O Area[1], MTDArea, R/W Area
	{	
		ND_TRACE("\n+Area[%d] Send to Host\n", j);

		if ( j == 3 )
		{
			for ( i = 0; i < sROM_PartitionInfo.NAND_Gang_Partition[j].TotalBlockNum; ++i )
			{
				nReadBlockNum++;

				ND_TRACE("FTL Block Address[%d]\n", nLPTBuffer[i + nUseBlockNum]);

				nReadBuffer = nPageBuffer;
				nSpareBuffer = nReadBuffer + gNAND_DrvInfo[0].NFTLDrvInfo->PageSize;
				for ( k = 0; k < gNAND_DrvInfo[0].NFTLDrvInfo->PpB; ++k )
				{
					TNFTL_IOReadPageWithECC( &gTNFTL_DrvInfo[0],
								  			  ((nLPTBuffer[i + nUseBlockNum] << gTNFTL_DrvInfo[0].ShiftPpB) + k ),
								  			  0, gTNFTL_DrvInfo[0].PPages,
								  			  nReadBuffer,
								  			  nSpareBuffer,
								  			  ECC_ON );

					fxnFwdnDrvResponseNotifyData(gTNFTL_DrvInfo[0].PageSize + gTNFTL_DrvInfo[0].SpareSize,50+(nReadBlockNum*50)/nTotalBlockNum);
					fxnFwdnDrvSendToHost( &nReadBuffer[0], gTNFTL_DrvInfo[0].PageSize + gTNFTL_DrvInfo[0].SpareSize );				
				}
			}
		}
		else if ( j == 2 )
		{
			memset( &gTNFTL_DrvInfo[0].FTLPageBuffer[0], 0xFF, gNAND_DrvInfo[0].NFTLDrvInfo->PageSize );
			
			for ( i = 0; i < sROM_PartitionInfo.NAND_Gang_Partition[j].TotalBlockNum; ++i )
			{
				nReadBlockNum++;

				ND_TRACE("MTD Physical Block Address[%d]\n", nLPTBuffer[i + nUseBlockNum]);

				nBufferIndex = 0;
				for ( k = 0; k < gTNFTL_DrvInfo[0].MediaDevInfo->Feature.PpB; ++k )
				{					
					NAND_MTD_ReadPage( ( nLPTBuffer[i + nUseBlockNum] << gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].ShiftPpB) + k, gTNFTL_DrvInfo[0].FTLPageBuffer );

					if ( gTNFTL_DrvInfo[0].MediaDevInfo->Feature.PageSize == 16384 )
					{
						memcpy( &nPageBuffer[nBufferIndex], &gTNFTL_DrvInfo[0].FTLPageBuffer[0], 8192 );
						memcpy( &nPageBuffer[nBufferIndex + gNAND_DrvInfo[0].NFTLDrvInfo->PageSize], &gTNFTL_DrvInfo[0].FTLPageBuffer[8192], gNAND_DrvInfo[0].NFTLDrvInfo->SpareSize );
					}
					else
					{
						memcpy( &nPageBuffer[nBufferIndex], &gTNFTL_DrvInfo[0].FTLPageBuffer[0], ( gNAND_DrvInfo[0].NFTLDrvInfo->PageSize + gNAND_DrvInfo[0].NFTLDrvInfo->SpareSize ));
					}
					
					nBufferIndex += (gNAND_DrvInfo[0].NFTLDrvInfo->PageSize + gNAND_DrvInfo[0].NFTLDrvInfo->SpareSize);
				}

				fxnFwdnDrvResponseNotifyData(nNandBlkSize,50+(nReadBlockNum*50)/nTotalBlockNum);
				fxnFwdnDrvSendToHost( &nPageBuffer[0], nNandBlkSize );
			}			
		}
		else
		{
			for ( i = 0; i < sROM_PartitionInfo.NAND_Gang_Partition[j].TotalBlockNum; ++i )
			{
				nReadBlockNum++;

				ND_TRACE("Physical Block Address[%d]\n", nLPTBuffer[i + nUseBlockNum]);

				nBufferIndex = 0;
				for ( k = 0; k < gTNFTL_DrvInfo[0].MediaDevInfo->Feature.PpB; ++k )
				{
					NAND_PhyReadPage( nLPTBuffer[i + nUseBlockNum], k, 0, &nPageBuffer[nBufferIndex] );
					nBufferIndex += (gNAND_DrvInfo[0].NFTLDrvInfo->PageSize + gNAND_DrvInfo[0].NFTLDrvInfo->SpareSize);
				}					

				fxnFwdnDrvResponseNotifyData(nNandBlkSize,50+(nReadBlockNum*50)/nTotalBlockNum);
				fxnFwdnDrvSendToHost( &nPageBuffer[0], nNandBlkSize );
			}			
		}

		nUseBlockNum += sROM_PartitionInfo.NAND_Gang_Partition[j].TotalBlockNum;

#ifdef GANG_NULL_BLOCK
		if ( j != 3 )
		{	
			ND_TRACE("NULL Block Num[%d]\n", ((( sROM_PartitionInfo.NAND_Gang_Partition[j].BlockLowerLimit - sROM_PartitionInfo.NAND_Gang_Partition[j].TotalBlockNum ) - sROM_PartitionInfo.NAND_Gang_Partition[j].BlockUpperLimit ) + 1));

			for ( i = 0; i < ((( sROM_PartitionInfo.NAND_Gang_Partition[j].BlockLowerLimit - sROM_PartitionInfo.NAND_Gang_Partition[j].TotalBlockNum ) - sROM_PartitionInfo.NAND_Gang_Partition[j].BlockUpperLimit ) + 1); ++i )
			{
				memset( nPageBuffer, 0xFF, nNandBlkSize );
				fxnFwdnDrvResponseNotifyData(nNandBlkSize,50+(nReadBlockNum*50)/nTotalBlockNum);
				fxnFwdnDrvSendToHost( &nPageBuffer[0], nNandBlkSize );
				//WriteFile( gImage_hDrive, nBlockBuffer, nNandBlkSize, &dwSize, NULL );		
			}
		}
#endif
		ND_TRACE("\n-Area[%d] Send to Host\n", j);
	}

	ND_TRACE("-Make Gang Image Complete!!!\n\n");

	return TRUE;
}

static int _loc_NAND_TOTALBIN_Write(FXN_FWDN_DRV_RquestData fxnFwdnDrvRRequestData)
{
	unsigned int	i, j, k;
	//unsigned int	rMTDStBlk, rMTDEdBlk;
	//unsigned int	nBlkAddr;
	unsigned int	nPageAddr;
	unsigned int	nBlockNum;
	unsigned int	nGangStBlk;
	unsigned int	nGangEdBlk;

	unsigned char	*readBuf;
	//unsigned char	*verifyBuf;
	unsigned int	rMTDFTLBlk;
	unsigned int	readBufSize;
	//unsigned int	readBytes, remain;
	//unsigned int	startpageParam;
	unsigned int	nBufferPageCount;
	unsigned int	nAreaStBlk;
	//unsigned char 		*receiveBuf;
	unsigned char		*nPageBuffer;
	unsigned char		*nSpareBuffer;
	ROM_HEADER_INFO		*sROM_HeaderInfo;	
	ROM_PARTITION_INFO	*sNandPartInfo;
	unsigned int 		res = SUCCESS;
	U32					dwPHYPageAddr;
	U16					wCSorder;

	//==============================
	// Read ROM HEADER ( 1Block Size )
	//==============================
	ND_TRACE("\n\n+Get ROM Header Info\n");
	readBufSize 	= ROM_HEADER_INFO_SIZE;
	nPageBuffer 	= (unsigned char*)0x40100000;		// Info buffer
	//fxnFwdnDrvResponseRequestData(readBufSize);
	//if( fxnFwdnDrvDirectReadFromHost(&receiveBuf)!= readBufSize )
	//	res = -1;
	//memcpy(nPageBuffer, receiveBuf, readBufSize);
	fxnFwdnDrvRRequestData(nPageBuffer, readBufSize);

	sROM_HeaderInfo = (ROM_HEADER_INFO *)&nPageBuffer[0];	
	nPageBuffer 	+= ROM_HEADER_INFO_SIZE;
	readBufSize		= (( sROM_HeaderInfo->PageSize + sROM_HeaderInfo->SpareSize ) * sROM_HeaderInfo->PhyPpB ); // 1Block Size	

	//fxnFwdnDrvResponseRequestData(( readBufSize - ROM_HEADER_INFO_SIZE ));
	//if( fxnFwdnDrvDirectReadFromHost( &receiveBuf) != ( readBufSize - ROM_HEADER_INFO_SIZE ) )
	//	res = -1;
	//memcpy(nPageBuffer, receiveBuf, ( readBufSize - ROM_HEADER_INFO_SIZE ));
	fxnFwdnDrvRRequestData(nPageBuffer, ( readBufSize - ROM_HEADER_INFO_SIZE ));

	ND_TRACE(":PageSize: %d, SpareSize: %d, PpB: %d, FTLPpB: %d\n", sROM_HeaderInfo->PageSize, sROM_HeaderInfo->SpareSize, sROM_HeaderInfo->PhyPpB, sROM_HeaderInfo->FTLPpB );
	ND_TRACE(":MediaNum: %d\n", sROM_HeaderInfo->NAND_MEDIA_NUM);

	sNandPartInfo	= (ROM_PARTITION_INFO *)&nPageBuffer[( sROM_HeaderInfo->PageSize + sROM_HeaderInfo->SpareSize - ROM_HEADER_INFO_SIZE)];
	readBuf			= (unsigned char *)&nPageBuffer[readBufSize - ROM_HEADER_INFO_SIZE];

	if ( sROM_HeaderInfo->SpareSize < 512 )
		nBufferPageCount = 512 / sROM_HeaderInfo->SpareSize;
	else
		nBufferPageCount = 1;

	readBufSize 	= (( sROM_HeaderInfo->PageSize + sROM_HeaderInfo->SpareSize ) * nBufferPageCount );

	ND_TRACE("-Get ROM Header Info\n");
	//==============================
	// Low Level Format
	//==============================
	ND_TRACE("+Low-Level Format\n");
	NAND_LowFormat(NAND_DRV_LOWFORMAT_TYPE_DEFAULT,NULL);
	ND_TRACE("-Low-Level Format\n");

	for ( k = 0; k < 4; ++k )
	{
		// Write Boot loader
		nBlockNum 		= sNandPartInfo->NAND_Gang_Partition[k].TotalBlockNum;
		nGangStBlk 		= sNandPartInfo->NAND_Gang_Partition[k].BlockUpperLimit;
		nGangEdBlk 		= sNandPartInfo->NAND_Gang_Partition[k].BlockLowerLimit;
		nAreaStBlk		= sNandPartInfo->NAND_Gang_Partition[k].BlockUpperLimit;
		
		ND_TRACE("+[%d]Partition Write\n", k);
		if ( k == 3 )
			ND_TRACE(":Start_FTLBlk: %d, End FTLBlk: %d, TotalBlockNum: %d\n", nGangStBlk, nGangEdBlk, nBlockNum );
		else			
			ND_TRACE(":StartBlk: %d, EndBlk: %d, TotalBlockNum: %d\n", nGangStBlk, nGangEdBlk, nBlockNum );

		while ( nBlockNum )
		{
			if ( nGangStBlk == (nGangEdBlk + 1 ))
				return FALSE;

			if ( k == 3 )
			{
				for ( i = nGangStBlk; i < nGangEdBlk + 1; ++i )
				{
					nPageAddr = i * sROM_HeaderInfo->FTLPpB;
	
					res = SUCCESS;
					for ( j = 0; j < sROM_HeaderInfo->FTLBlk_Composite_Num; ++j )
					{
						_loc_NAND_TOTALBIN_ConvertIPAtoPPA( sROM_HeaderInfo, nPageAddr +j, &wCSorder, &dwPHYPageAddr );
						res |= NAND_IO_EraseBlock( &gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[wCSorder], dwPHYPageAddr, INTER_LEAVE_OFF );
					}

					if ( res == SUCCESS )
					{
						ND_TRACE(":Write FTL_BlockAddr: %d\n", i);
						nGangStBlk = ( i + 1 );
						break;
					}
				}
			}
			else if ( k == 2 )
			{
				for ( i = nGangStBlk; i < nGangEdBlk + 1; ++i )
				{
//					NAND_MTD_GetPhyBlkAddr( gTNFTL_DrvInfo[0].MediaDevInfo, i, &rMTDFTLBlk );
					_loc_NAND_MTD_GetPhyBlkAddr( sROM_HeaderInfo, nAreaStBlk, i,&rMTDFTLBlk);
					dwPHYPageAddr = rMTDFTLBlk * sROM_HeaderInfo->PhyPpB;

					res = NAND_IO_EraseBlock( &gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0], dwPHYPageAddr, INTER_LEAVE_OFF );					
					if ( res == SUCCESS )
					{
						ND_TRACE(":Write Phy_BlockAddr: %d\n", rMTDFTLBlk);
						nGangStBlk = ( i + 1 );
						break;
					}
				}				
			}
			else
			{
				for ( i = nGangStBlk; i < nGangEdBlk + 1; ++i )
				{
					dwPHYPageAddr = i * sROM_HeaderInfo->PhyPpB; //gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].ShiftPpB;
					if ( dwPHYPageAddr != 0 )
					{
						res = NAND_IO_EraseBlock( &gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0], dwPHYPageAddr, INTER_LEAVE_OFF );						
					}
					else
					{
						for ( j = 0; j < sROM_HeaderInfo->NAND_MEDIA_NUM; ++j )
							res = NAND_IO_EraseBlock( &gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[j], dwPHYPageAddr, INTER_LEAVE_OFF );					
					}
					
					if ( res == SUCCESS )
					{
						ND_TRACE(":Write Phy_BlockAddr: %d\n", i);
						nGangStBlk = ( i + 1 );
						break;
					}
				}
			}

			if ( res == SUCCESS )
			{
				if ( k == 3 )
				{
					for ( j = 0; j < sROM_HeaderInfo->FTLPpB; ++j )
					{
						if (!( j % nBufferPageCount ))
						{
							//fxnFwdnDrvResponseRequestData(readBufSize);
							//if( fxnFwdnDrvDirectReadFromHost(&receiveBuf) != readBufSize )
							//	res = -1;
							//memcpy(readBuf, receiveBuf, readBufSize);
							fxnFwdnDrvRRequestData(readBuf, readBufSize);

							nPageBuffer = &readBuf[0];
							nSpareBuffer = &readBuf[sROM_HeaderInfo->PageSize];
						}

						_loc_NAND_TOTALBIN_ConvertIPAtoPPA( sROM_HeaderInfo, nPageAddr +j, &wCSorder, &dwPHYPageAddr );

						res = NAND_IO_WritePageGANG( &gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[wCSorder],
												     dwPHYPageAddr,
												     nPageBuffer );

						nPageBuffer += ( sROM_HeaderInfo->PageSize + sROM_HeaderInfo->SpareSize );
						nSpareBuffer += ( sROM_HeaderInfo->PageSize + sROM_HeaderInfo->SpareSize );
					}
				}
				else
				{
					for ( j = 0; j < sROM_HeaderInfo->PhyPpB; ++j )
					{
						if (!( j % nBufferPageCount ))
						{
							//fxnFwdnDrvResponseRequestData(readBufSize);							
							//if( fxnFwdnDrvDirectReadFromHost(&receiveBuf) != readBufSize )
							//	res = -1;
							//memcpy(readBuf, receiveBuf, readBufSize);
							fxnFwdnDrvRRequestData(readBuf, readBufSize);

							nPageBuffer = &readBuf[0];
							nSpareBuffer = &readBuf[gNAND_DrvInfo[0].NFTLDrvInfo->PageSize];							
						}

						if ( dwPHYPageAddr != 0 )
						{
							res = NAND_IO_WritePageGANG( &gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0], 
													     dwPHYPageAddr + j,
													     nPageBuffer );							
						}
						else
						{
							for ( i = 0; i < sROM_HeaderInfo->NAND_MEDIA_NUM; ++i )
							{
								res = NAND_IO_WritePageGANG( &gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[i],
														     dwPHYPageAddr + j,
														     nPageBuffer );
							}
						}

						nPageBuffer += ( sROM_HeaderInfo->PageSize + sROM_HeaderInfo->SpareSize );
					}
				}
			}

			nBlockNum -= 1;

			if ( nBlockNum == 0 )
			{
				if ( k != 3 )
				{
				#ifdef GANG_NULL_BLOCK
	
					for ( i = nGangStBlk; i < nGangEdBlk + 1; ++i )
					{
						for ( j = 0; j < sROM_HeaderInfo->PhyPpB; ++j )
						{
							if (!( j % nBufferPageCount ))
							{
								//fxnFwdnDrvResponseRequestData(readBufSize);														
								//if( fxnFwdnDrvDirectReadFromHost(&receiveBuf) != readBufSize )
								//	res = -1;							
								//memcpy(readBuf, receiveBuf, readBufSize);
								fxnFwdnDrvRRequestData(readBuf, readBufSize);
							}
						}
					}
				#endif
				}

				res = SUCCESS;
				ND_TRACE("-[%d]Partition Write\n",k);

				goto GANG_WRITE_COMPLETE;
			}
		}

		GANG_WRITE_COMPLETE:

		res = SUCCESS;
	}

	ND_TRACE("Gang Write Complete!!!\n\n");

	if( res == SUCCESS )
		return TRUE;
	else
		return FALSE;
}
#endif

unsigned char FWDN_DRV_DUMP_InfoRead(void *pBuf)
{
	unsigned int  i = 0;
	unsigned char length = 0;

#if defined(NAND_INCLUDE) && defined(TNFTL_V7_INCLUDE)
	#ifndef WITHOUT_FILESYSTEM
	length = sizeof(NAND_DEVICE_INFO);
	memset( (void*)&gNAND_DEVICE_INFO, 0xFF, length );
	
	for ( i = 0; i < 6; ++i )
		gNAND_DEVICE_INFO.DevID[i] = gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.DeviceID.Code[i];
	gNAND_DEVICE_INFO.MediaNums	 			= gNAND_DrvInfo[0].NFTLDrvInfo->MediaNums;
	gNAND_DEVICE_INFO.MAX_ROMSize			= gMAX_ROMSIZE;
	gNAND_DEVICE_INFO.ExtendedPartitionNum 	= gNAND_DrvInfo[0].NFTLDrvInfo->ExtendedPartitionNo;
	for ( i = 0; i < gNAND_DrvInfo[0].NFTLDrvInfo->ExtendedPartitionNo; ++i )
		gNAND_DEVICE_INFO.ExtPartitionSize[i] = 4096;
	for ( i = 0; i < 12; ++i )
		gNAND_DEVICE_INFO.ExtPartitionWCacheNum[i] = gNAND_DrvInfo[0].NFTLDrvInfo->ExtPartition[i].WCacheNums;
	gNAND_DEVICE_INFO.ROAreaSize			= gNAND_DrvInfo[0].NFTLDrvInfo->ROAreaSize;
	gNAND_DEVICE_INFO.PBpV					= gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PBpV;
	gNAND_DEVICE_INFO.PpB					= gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PpB;
	gNAND_DEVICE_INFO.PageSize		= gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.PageSize;
	gNAND_DEVICE_INFO.SpareSize		= gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].Feature.SpareSize;
	#endif

	memcpy(pBuf, (void*)&gNAND_DEVICE_INFO, length);
#endif 

	return length;
}

int FWDN_DRV_DUMP_BlockRead(unsigned int Param0, unsigned int Param1, unsigned int Param2, 
											FXN_FWDN_DRV_Response_NotifyData fxnFwdnDrvResponseNotifyData,
											FXN_FWDN_DRV_SendToHost fxnFwdnDrvSendToHost)
{
#if defined(NAND_INCLUDE) && defined(TNFTL_V7_INCLUDE)
	return _loc_NAND_DUMP_BlockRead(Param0, Param1, Param2, fxnFwdnDrvResponseNotifyData, fxnFwdnDrvSendToHost);
#else
	return FALSE;
#endif
}

int FWDN_DRV_DUMP_BlockWrite(unsigned int Param0, unsigned int Param1, unsigned int Param2, 
											FXN_FWDN_DRV_Response_RequestData fxnFwdnDrvResponseRequestData,
											FXN_FWDN_DRV_ReadFromHost fxnFwdnDrvReadFromHost)
{
#if defined(NAND_INCLUDE) && defined(TNFTL_V7_INCLUDE)
	return _loc_NAND_DUMP_BlockWrite(Param0, Param1, Param2, fxnFwdnDrvResponseRequestData, fxnFwdnDrvReadFromHost);
#else
	return FALSE;
#endif
}

#define DUMP_ADDR KERNEL_ADDR + 0x100000

int FWDN_DRV_Dump(char *pName, unsigned int uiAddress, unsigned int uiSize,
											FXN_FWDN_DRV_Response_NotifyData fxnFwdnDrvResponseNotifyData,
											FXN_FWDN_DRV_SendToHost fxnFwdnDrvSendToHost)
{
#if defined(NAND_INCLUDE) && defined(TNFTL_V7_INCLUDE)

	int res = FALSE;
	int rGmcStatus = SUCCESS, rExtStatus = SUCCESS, rBootStatus = SUCCESS, rRomCrc = SUCCESS;	

	if( _loc_strcmp(pName,"tcboot0") == 0 )
	{
		NAND_DumpBootArea( 0, DUMP_ADDR, &uiSize, &rGmcStatus, &rExtStatus, &rBootStatus, &rRomCrc );
		fxnFwdnDrvResponseNotifyData(uiSize,0);
		fxnFwdnDrvSendToHost(DUMP_ADDR, uiSize);
		if ( rGmcStatus == SUCCESS )
			FWDN_DRV_AddMessage("NAND:Golden Block crc: Success");
		else
			FWDN_DRV_AddMessage("NAND:Golden Block crc: Fail!!!");	

		if ( rExtStatus == SUCCESS )
			FWDN_DRV_AddMessage("NAND:Externalmemeory block crc: Success");
		else
			FWDN_DRV_AddMessage("NAND:External memeory block crc: Fail!!!");	

		if ( rBootStatus == SUCCESS )
			FWDN_DRV_AddMessage("NAND:2st bootloader[0] block crc: Success");
		else
			FWDN_DRV_AddMessage("NAND:2st bootloader[0] block crc: Fail!!!");	

		if ( rRomCrc == SUCCESS )
			FWDN_DRV_AddMessage("NAND:tcboot[0] crc: Success");
		else
			FWDN_DRV_AddMessage("NAND:tcboot[0] crc: Fail!!!");	
		
		res = TRUE;
	}
	else if( _loc_strcmp(pName,"tcboot1") == 0 )
	{
		NAND_DumpBootArea( 1, DUMP_ADDR, &uiSize, &rGmcStatus, &rExtStatus, &rBootStatus, &rRomCrc );
		fxnFwdnDrvResponseNotifyData(uiSize,0);
		fxnFwdnDrvSendToHost(DUMP_ADDR, uiSize);
		if ( rGmcStatus == SUCCESS )
			FWDN_DRV_AddMessage("NAND:Golden Block crc: Success");
		else
			FWDN_DRV_AddMessage("NAND:Golden Block crc: Fail!!!");	

		if ( rExtStatus == SUCCESS )
			FWDN_DRV_AddMessage("NAND:External memeory block crc: Success");
		else
			FWDN_DRV_AddMessage("NAND:External memeory block crc: Fail!!!");	

		if ( rBootStatus == SUCCESS )
			FWDN_DRV_AddMessage("NAND:2st bootloader[1] block crc: Success");
		else
			FWDN_DRV_AddMessage("NAND:2st bootloader[1] block crc: Fail!!!");	

		if ( rRomCrc == SUCCESS )
			FWDN_DRV_AddMessage("NAND:tcboot[1] crc: Success");
		else
			FWDN_DRV_AddMessage("NAND:tcboot[1] crc: Fail!!!");	
		
		res = TRUE;
	}
	else if( _loc_strcmp(pName,"NAND") == 0 )
	{
		unsigned int uiMedia;
		res = TRUE;
		for(uiMedia=0 ; uiMedia<gNAND_DrvInfo[0].NFTLDrvInfo->MediaNums && res == TRUE ; uiMedia++)
		{
			res = _loc_NAND_DUMP_BlockRead( 0,
									gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[uiMedia].Feature.PBpV,
									uiMedia,
									fxnFwdnDrvResponseNotifyData,fxnFwdnDrvSendToHost);
		}
	}
	else if( _loc_strcmp(pName,FWDN_NAND_DATA_AREA_NAME) == 0 )
	{
		if(uiSize == 0)
		{
			uiAddress = 0;
			uiSize = gNAND_DrvInfo[0].NFTLDrvInfo->PriPartition.TotalSectorSize;
		}
		while( uiAddress < uiSize )
		{
			unsigned int uiPacketSectorSize = (uiSize>(FWDN_DRV_BLOCKREAD_BUFFER_SIZE/512))? (FWDN_DRV_BLOCKREAD_BUFFER_SIZE/512) : uiSize;
			fxnFwdnDrvResponseNotifyData(uiPacketSectorSize<<9,0);
			DISK_ReadSector(DISK_DEVICE_NAND, 0, uiAddress, uiPacketSectorSize, DUMP_ADDR);
			fxnFwdnDrvSendToHost( DUMP_ADDR, uiPacketSectorSize<<9 );
			uiAddress += uiPacketSectorSize;
			uiSize -= uiPacketSectorSize;
		}
		res = TRUE;
	}

	return res;
#elif defined(NAND_INCLUDE) && defined(TNFTL_V8_INCLUDE)
	int res = FALSE;

	if( _loc_strcmp(pName,"boot") == 0 )
	{
		uiSize = NAND_DumpExtArea( (unsigned char *)DUMP_ADDR );
		if( uiSize == 0 )
		{
			dprintf(CRITICAL, " NAND Dump Fail !! \n");
			return FALSE;
		}
		fxnFwdnDrvResponseNotifyData(uiSize,0);
		fxnFwdnDrvSendToHost(DUMP_ADDR, uiSize);

		res = TRUE;
	}
	return res;
#endif
}

int FWDN_DRV_TOTALBIN_Read(FXN_FWDN_DRV_Response_NotifyData fxnFwdnDrvResponseNotifyData,
							FXN_FWDN_DRV_SendToHost fxnFwdnDrvSendToHost)
{
#if defined(NAND_INCLUDE) && defined(TNFTL_V7_INCLUDE)
	return _loc_NAND_TOTALBIN_Read(fxnFwdnDrvResponseNotifyData, fxnFwdnDrvSendToHost);
#else
	return FALSE;
#endif
}

int FWDN_DRV_TOTALBIN_Write(FXN_FWDN_DRV_RquestData fxnFwdnDrvRRequestData)
{
#if defined(NAND_INCLUDE) && defined(TNFTL_V7_INCLUDE)
	return _loc_NAND_TOTALBIN_Write(fxnFwdnDrvRRequestData);
#else
	return FALSE;
#endif
}

unsigned int FWDN_FNT_SetSN(unsigned char* ucTempData, unsigned int uiSNOffset)
{
	unsigned int		uiVerifyCrc;
	unsigned int		uiTempCrc;

	if (memcmp( &ucTempData[20+uiSNOffset], "ZERO", 4 ) == 0)
	{		
		uiVerifyCrc = (ucTempData[uiSNOffset + 16] <<24) ^ ( ucTempData[uiSNOffset + 17] <<16) ^ 
			( ucTempData[uiSNOffset + 18] << 8) ^ ( ucTempData[uiSNOffset + 19]); 
		uiTempCrc = FWUG_CalcCrc8((uiSNOffset + ucTempData), 16 );

		if ( ( uiTempCrc == uiVerifyCrc ) || (uiVerifyCrc == 0x0000 )) 	//16 bytes Serial Exist
		{
			g_uiFWDN_WriteSNFlag = 1;
			return SUCCESS;
		}
	}
	else if (memcmp(&ucTempData[20+uiSNOffset], "FWDN", 4) == 0 || memcmp(&ucTempData[20+uiSNOffset], "GANG", 4) == 0)
	{
		uiVerifyCrc = ( ucTempData[uiSNOffset + 16] <<24) ^ ( ucTempData[uiSNOffset + 17] <<16) ^ 
			( ucTempData[uiSNOffset + 18] << 8) ^ ( ucTempData[uiSNOffset + 19]); 
		uiTempCrc = FWUG_CalcCrc8( (uiSNOffset + ucTempData), 16 );
		if  (uiVerifyCrc == 0x0000 )
		{
			g_uiFWDN_WriteSNFlag = 0; 										// cleared SN
			return 1;
		}
		else
		{
			if (memcmp(&ucTempData[52+uiSNOffset], "FWDN", 4) == 0 || memcmp(&ucTempData[52+uiSNOffset], "GANG", 4) == 0)
			{
				uiVerifyCrc = ( ucTempData[uiSNOffset + 48] <<24) ^ ( ucTempData[uiSNOffset + 49] <<16) ^ 
					( ucTempData[uiSNOffset + 50] << 8) ^ ( ucTempData[uiSNOffset + 51]); 
				uiTempCrc = FWUG_CalcCrc8((uiSNOffset + ucTempData+32), 16 );				
				if (( uiVerifyCrc == uiTempCrc ) && ( uiVerifyCrc != 0x0000 ))
				{
					g_uiFWDN_WriteSNFlag = 1;

				#if defined (NKUSE)
				#else
					//#ifndef WITHOUT_FILESYSTEM
					if ( DISK_DefaultDriveType == DISK_DEVICE_HDD ) //for erasing NOR Flash before writing FW 
					{
						memcpy ( FWDN_DeviceInformation.DevSerialNumber, ucTempData, 16 );
						memcpy ( FWDN_DeviceInformation.DevSerialNumber+16, ucTempData+32, 16 );
					}
					//#endif
				#endif	
					return SUCCESS;
				}				
				else
					g_uiFWDN_WriteSNFlag = 0; 										// cleared SN
			}
			else
				g_uiFWDN_WriteSNFlag = 0; 										// cleared SN
		}
	}
	return 1;
}

/**************************************************************************
*  FUNCTION NAME : 
*  
*      void FWDN_FNT_VerifySN(unsigned char* ucTempData, unsigned int uiSNOffset);
*  
*  DESCRIPTION : Verify the validity of Serial Number format
*  
*  INPUT:
*			ucTempData	= base pointer of serial number format
*			uiSNOffset	= offset of serial number format
*  
*  OUTPUT:	void - Return Type
*			update global structure of FWDN_DeviceInformation according to verification result.
*  
**************************************************************************/
void FWDN_FNT_VerifySN(unsigned char* ucTempData, unsigned int uiSNOffset)
{
	unsigned int		uiVerifyCrc;
	unsigned int		uiTempCrc;

	/*---------------------------------------------------------------------
		Check Type for Serial Number of 16 digits
	----------------------------------------------------------------------*/
	if (memcmp(&ucTempData[20+uiSNOffset], "ZERO", 4) == 0)
	{		
		uiVerifyCrc = (ucTempData[uiSNOffset + 16] <<24) ^ ( ucTempData[uiSNOffset + 17] <<16) ^
			( ucTempData[uiSNOffset + 18] << 8) ^ ( ucTempData[uiSNOffset + 19]); 
		uiTempCrc = FWUG_CalcCrc8((uiSNOffset + ucTempData), 16 );

		if ( ( uiTempCrc == uiVerifyCrc ) || (uiVerifyCrc == 0x0000 ))
		{
			if (uiVerifyCrc == 0x0000 ) 
			{
				memset(FWDN_DeviceInformation.DevSerialNumber, 0x30, 16);
				memset(FWDN_DeviceInformation.DevSerialNumber+16, 0x30, 16);
				FWDN_DeviceInformation.DevSerialNumberType = SN_INVALID_16;
			}
			else
			{
				memcpy(FWDN_DeviceInformation.DevSerialNumber, (uiSNOffset + ucTempData), 16);
				memset(FWDN_DeviceInformation.DevSerialNumber+16, 0x30, 16);
				FWDN_DeviceInformation.DevSerialNumberType = SN_VALID_16;
			}
		}
	}
	/*---------------------------------------------------------------------
		Check Type for Serial Number of 32 digits
	----------------------------------------------------------------------*/
	else if (memcmp(&ucTempData[20+uiSNOffset], "FWDN", 4) == 0 || memcmp(&ucTempData[20+uiSNOffset], "GANG", 4) == 0)
	{
		uiVerifyCrc = ( ucTempData[uiSNOffset + 16] <<24) ^ ( ucTempData[uiSNOffset + 17] <<16) ^ 
			( ucTempData[uiSNOffset + 18] << 8) ^ ( ucTempData[uiSNOffset + 19]); 
		uiTempCrc = FWUG_CalcCrc8(uiSNOffset + ucTempData, 16 );
		if  (uiVerifyCrc == 0x0000 )
		{
			memset(FWDN_DeviceInformation.DevSerialNumber, 0x30, 32);
			FWDN_DeviceInformation.DevSerialNumberType = SN_INVALID_32;			
		}
		else
		{
			if (memcmp(&ucTempData[52+uiSNOffset], "FWDN", 4) == 0 || memcmp(&ucTempData[52+uiSNOffset], "GANG", 4) == 0)
			{
				uiVerifyCrc = ( ucTempData[uiSNOffset + 48] <<24) ^ ( ucTempData[uiSNOffset + 49] <<16) ^ 
					( ucTempData[uiSNOffset + 50] << 8) ^ ( ucTempData[uiSNOffset + 51]); 
				uiTempCrc = FWUG_CalcCrc8(( uiSNOffset + ucTempData + 32), 16 );
				if (( uiVerifyCrc == uiTempCrc ) && ( uiVerifyCrc != 0x0000 ))
				{
					memcpy(FWDN_DeviceInformation.DevSerialNumber, (uiSNOffset + ucTempData), 16);
					memcpy(FWDN_DeviceInformation.DevSerialNumber+16, (uiSNOffset + ucTempData + 32 ), 16);
					FWDN_DeviceInformation.DevSerialNumberType = SN_VALID_32;
				}				
				else
				{
					memset(FWDN_DeviceInformation.DevSerialNumber, 0x30, 32);
					FWDN_DeviceInformation.DevSerialNumberType = SN_INVALID_32;			
				}
			}
			else
			{
				memset(FWDN_DeviceInformation.DevSerialNumber, 0x30, 32);
				FWDN_DeviceInformation.DevSerialNumberType = SN_INVALID_32;			
			}
		}		
	}
	else
	{
		memset(FWDN_DeviceInformation.DevSerialNumber, 0x30, 32);
		FWDN_DeviceInformation.DevSerialNumberType = SN_NOT_EXIST;	
	}
}

void FWDN_FNT_InsertSN(unsigned char *pSerialNumber)
{
	unsigned	uCRC;

	memcpy (pSerialNumber, FWDN_DeviceInformation.DevSerialNumber, 16 );
	uCRC	= FWUG_CalcCrc8(pSerialNumber, 16 );
	
	pSerialNumber[16] = (uCRC & 0xFF000000) >> 24;
	pSerialNumber[17] = (uCRC & 0x00FF0000) >> 16;
	pSerialNumber[18] = (uCRC & 0x0000FF00) >> 8;
	pSerialNumber[19] = (uCRC & 0x000000FF) ;

	memcpy(pSerialNumber+20, "FWDN", 4);
	memset(pSerialNumber+24, 0x00, 8);
	memcpy(pSerialNumber+32, FWDN_DeviceInformation.DevSerialNumber+16, 16);
	uCRC	= FWUG_CalcCrc8(pSerialNumber+32, 16 );

	pSerialNumber[48] = (uCRC & 0xFF000000) >> 24;
	pSerialNumber[49] = (uCRC & 0x00FF0000) >> 16;
	pSerialNumber[50] = (uCRC & 0x0000FF00) >> 8;
	pSerialNumber[51] = (uCRC & 0x000000FF) ;

	memcpy(pSerialNumber+52, "FWDN", 4);
	memset(pSerialNumber+56, 0x00, 8);
}

unsigned char FWDN_DRV_FirmwareStorageID(void)
{
	return (unsigned int)gFWDN_FirmwareStorage;
}

#endif //FWDN_V7

#endif
/************* end of file *************************************************************/
