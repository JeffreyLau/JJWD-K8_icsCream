/****************************************************************************
 *   FileName    : mmc_basic.c
 *   Description : SD/MMC execution function
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

 [1142] : Change _TCC7801_ to _BOARD_VERSION_TCC7801_010_
 [1448] : 1. Add code for SD Boot & Hidden Area
		  2. Fix code for time-out process not to wait too long period.

 ****************************************************************************/
#include <platform/globals.h>
#include <sdmmc/sd_hw.h>
#include <sdmmc/sd_slot.h>
#include <fwdn/Disk.h>
#include <debug.h>
/************************************************************************

                       CUSTOMIZING DRIVER OPTION for each PLATFORM

************************************************************************/
/* Maximum Acceptable TAAC time for writing = 15ms
    If Card has more than TAAC time than this value, it waits internal FIFO state rather than time-delaying */
//	#define	SDMMC_MAX_TAAC			15000

#if defined(_WINCE_)
#define SD_Debug		(1) ? (void) 0 : EdbgOutputDebugString
#define SD_DebugL1		(1) ? (void) 0 : EdbgOutputDebugString
#define SD_DebugL2		(1) ? (void) 0 : EdbgOutputDebugString
#define SD_LOG			B_RETAILMSG
#elif defined(_LINUX_)
#define SD_Debug		(1) ? (void) 0 : printf
#define SD_DebugL1		(1) ? (void) 0 : printf
#define SD_DebugL2		(1) ? (void) 0 : printf
#define SD_LOG			printf
#endif

/************************************************************************
*	Externs DEFINE
************************************************************************/
extern int FWUG_GetSerialNumberTriFlash(unsigned char *ucSID);

/************************************************************************
*	VARIABLE DEFINE
************************************************************************/
#define SD_SLOT_SOURCE_CLOCK_IN_KHZ			48000

#undef ON
#undef OFF

#define	ON	1
#define	OFF	0

#ifndef ISONE
#define	ISONE(X, MASK)				( (unsigned int)(X) & ((unsigned int)(MASK)) )
#endif

unsigned char	g_mmc_ASCIISerialNumber[32];	/* [1448] */
int	g_mmc_SizeOfASCIISerialNumber;	/* [1448] */

#if 0

/**************************************************************************
*  FUNCTION NAME :
*
*      int	MMC_setBootCodeArea(sMMCrecord *pSlot, unsigned uCodeSize);
*
*  DESCRIPTION : Resize boot code area & file system area according to uCodeSize
*
*  INPUT:
*			pSlot	= pointer of MMC struct
*			uCodeSize	= Boot Code area size (with sector unit)
*
*  OUTPUT:	int - Return Type
*  			= file system size. (unit of sector)
*			= -1 : if file system size is too small.
*
**************************************************************************/
int	MMC_setBootCodeArea(PSD_SLOT_T pSlot, unsigned uCodeSize)	/* [1448] */
{
	// fill g_mmc[0].bootcode structure
	if(uCodeSize > 1)
	{
#if defined(FEATURE_SDMMC_MMC43_BOOT)
		if(pSlot->ulBootPartitionSize>=uCodeSize)
		{
			pSlot->bootcode.start	= 0;
			pSlot->bootcode.area		= 1;
		}
		else
#endif
		{
			if(pSlot->ulBootPartitionSize>0)
			{
				SD_Debug("[SD] boot partition is too small for bootloader (size=%d)\n",pSlot->ulBootPartitionSize);
				return -1;
			}
			pSlot->bootcode.start	= pSlot->size - 1/*header*/ - uCodeSize;
#if defined(FEATURE_SDMMC_MMC43_BOOT)
			pSlot->bootcode.area		= 0;
#endif

			pSlot->filesys.size		= pSlot->bootcode.start;
		}
		pSlot->bootcode.size		= uCodeSize;

		BITSET(pSlot->area_en, SDMMC_AREA_BOOTROM);

		// resize g_mmc[0].filesys area
		pSlot->filesys.cyl		= pSlot->filesys.size / pSlot->filesys.sect / pSlot->filesys.head;

		if(pSlot->filesys.cyl == 0)
			return (-1);
		else
			return 0;
	}
	else
		return (-1);
}

/**************************************************************************
*  FUNCTION NAME :
*
*      int MMC_GetInitDriveAreaFlag(int drv_num);
*
*  DESCRIPTION : Return Flag for Area (Hidden Sector/Hidden FS/FS) Re-Initialization.
*
*  INPUT:
*			drv_num	= 0 or 1
*
*  OUTPUT:	int - Return Type
*  			= 0 : Re-Initialization is Off.
*			= Non-Zero : Re-Initialization is On.
*
**************************************************************************/
int MMC_GetInitDriveAreaFlag(int drv_num)
{
	sMMCrecord *pSlot	= &g_mmc[drv_num];
	return	(pSlot->area_en & SDMMC_AREA_REINIT);
}

/**************************************************************************
*  FUNCTION NAME :
*
*      int MMC_SetInitDriveAreaFlag(int drv_num);
*
*  DESCRIPTION : Set Flag for Area (Hidden Sector/Hidden FS/FS) Re-Initialization.
*
*  INPUT:
*			drv_num	= 0 or 1
*
*  OUTPUT:	int - Return Type
*  			= pSlot->area_en
*
**************************************************************************/
int MMC_SetInitDriveAreaFlag(int drv_num)
{
	sMMCrecord *pSlot	= &g_mmc[drv_num];
	return BITSET(pSlot->area_en, SDMMC_AREA_REINIT);
}

/**************************************************************************
*  FUNCTION NAME :
*
*      int MMC_ClrInitDriveAreaFlag(int drv_num);
*
*  DESCRIPTION : Clear Flag for Area (Hidden Sector/Hidden FS/FS) Re-Initialization.
*
*  INPUT:
*			drv_num	= 0 or 1
*
*  OUTPUT:	int - Return Type
*  			= pSlot->area_en
*
**************************************************************************/
int MMC_ClrInitDriveAreaFlag(int drv_num)
{
	sMMCrecord *pSlot	= &g_mmc[drv_num];
	return BITCLR(pSlot->area_en, SDMMC_AREA_REINIT);
}
#endif	// BOOTSD_INCLUDE

int SD_SLOT_IsCardDetected(PSD_SLOT_T pSlot)
{
	if(SD_HW_Get_CardDetectPort(pSlot->iIndex))
		return 0; // Card is not inserted.
	else
		return 1;	// Card is inserted.
}

int SD_SLOT_IsWriteProtected(PSD_SLOT_T pSlot)
{
	if(SD_HW_Get_WriteProtectPort(pSlot->iIndex))
		return 1;	// Card is write protected.
	else
		return 0; // Card is not write protected.
}

int SD_SLOT_GetBootSlotIndex(void)
{
	return SD_HW_GetBootSlotIndex();
}

int SD_SLOT_GetUpdateSlotIndex(void)
{
	return SD_HW_GetUpdateSlotIndex();
}

int SD_SLOT_GetMaxBusWidth(PSD_SLOT_T pSlot)
{
	int bw = SD_HW_GetMaxBusWidth(pSlot->iIndex);

	if(pSlot->id == MMC_CARD_MMC && bw>1)
	{
		bw = 1;
	}
	else if(pSlot->id == MMC_CARD_SD)
	{
		if( !(pSlot->stSCR.sd_bus_widths&0x4) )
		{
			// This SD Card doesn't support 4 bit bus.
			bw = 1;
		}
		else if(bw>4)
		{
			bw = 4;
		}
	}
	else if(pSlot->id == MMC_CARD_MMCPLUS && bw>8)
	{
		bw = 8;
	}

	return bw;
}

/**************************************************************************
*  FUNCTION NAME :
*
*      void SD_SLOT_InitMMCRecord(int drv_num);
*
*  DESCRIPTION : Initialize each MMC Structure.
*
*  INPUT:
*			drv_num	= 0 or 1
*
*  OUTPUT:	void - Return Type
*
**************************************************************************/
void SD_SLOT_InitRecord(PSD_SLOT_T pSlot)
{
	int iIndex = pSlot->iIndex;

	pSlot->pHwSdSlotReg = SD_HW_GetSdSlotReg(iIndex);

	BITSET(pSlot->ctrl_prop, SDMMC_CTRL_AUTOSTOP);

	//pSlot->dma_addr	= IO_NFC_BUFFER1_BASE;
	pSlot->dma_addr	= 0;
	pSlot->ulClockRateInKHz = 100;
	pSlot->usSdClock = (0x80<<8)|INTERNAL_CLOCK_ENABLE;
	pSlot->bw = 1;

	//pSlot->filesys.sect		= 32;
	//pSlot->filesys.head		= 31;

	if(SD_HW_GetBootSlotIndex() == pSlot->iIndex)
		BITSET(pSlot->card_prop, SDMMC_PROP_INTERNAL);
}

/**************************************************************************
*  FUNCTION NAME :
*
*      int SD_SLOT_Initialize(void);
*
*  DESCRIPTION : Initialize SDMMC Driver. (Should be executed once at booting time)
*
*  INPUT:
*			None
*
*  OUTPUT:	int - Return Type
*  			= 0 : Successful
*
**************************************************************************/
void SD_SLOT_Initialize(void)
{
	SD_HW_Initialize();
}

static int SD_SLOT_WaitPresentState(PSD_SLOT_T pSlot, unsigned long ulMask, unsigned long ulCondition)
{
	PSDSLOTREG_T pSdSlotReg = ((PSDSLOTREG_T)pSlot->pHwSdSlotReg);
	int i;
	for(i=0 ; i<1000/*ms*/ ; i++)
	{
		if((pSdSlotReg->ulPresentState&ulMask) == ulCondition)
			break;
		SDMMC_ms_delay(1);
	}
	return 0;
}

static int SD_SLOT_SW_Reset(PSD_SLOT_T pSlot, unsigned char ucType)
{
	PSDSLOTREG_T pSdSlotReg = (PSDSLOTREG_T)pSlot->pHwSdSlotReg;

	BITSET(pSdSlotReg->ucSoftReset,ucType);
	while(pSdSlotReg->ucSoftReset&ucType);

	return 0;
}

static void SD_SLOT_SetTimeout(PSD_SLOT_T pSlot, unsigned char ucTimeout)
{
	PSDSLOTREG_T pSdSlotReg = (PSDSLOTREG_T)pSlot->pHwSdSlotReg;
	BITCSET(pSdSlotReg->ucTimeoutControl,0x0F,ucTimeout&0x0F);
}

static void SD_SLOT_SetVoltage(PSD_SLOT_T pSlot, unsigned char voltsel)
{
	PSDSLOTREG_T pSdSlotReg = (PSDSLOTREG_T)pSlot->pHwSdSlotReg;
	unsigned char ucPowerCtrl = (((voltsel)<<1)|0x01) & 0xF;
	BITCSET(pSdSlotReg->ucPowerControl,0x0F,ucPowerCtrl);
}

static void SD_SLOT_SetClockRate(PSD_SLOT_T pSlot)
{
	unsigned long ulTargetClock = SD_SLOT_SOURCE_CLOCK_IN_KHZ;
	unsigned long ulSdClkSel = 0;
	while( ulTargetClock > pSlot->ulClockRateInKHz && ulSdClkSel < 0x80 )
	{
		if(ulSdClkSel == 0)
			ulSdClkSel = 1;
		else
			ulSdClkSel  = ulSdClkSel<<1;

		ulTargetClock = ulTargetClock>>1;
	}
	pSlot->ulClockRateInKHz = ulTargetClock;
	pSlot->usSdClock = (unsigned short)(ulSdClkSel<<8) | INTERNAL_CLOCK_ENABLE;
}

static int SD_SLOT_ClockOn(PSD_SLOT_T pSlot)
{
	PSDSLOTREG_T pSdSlotReg = (PSDSLOTREG_T)pSlot->pHwSdSlotReg;
	pSdSlotReg->usClockControl = pSlot->usSdClock;
	while(!pSdSlotReg->usClockControl&INTERNAL_CLOCK_STABLE);
	pSdSlotReg->usClockControl = pSlot->usSdClock | SD_CLOCK_ENABLE;
	return 0;
}

static void SD_SLOT_ClockOff(PSD_SLOT_T pSlot)
{
	PSDSLOTREG_T pSdSlotReg = (PSDSLOTREG_T)pSlot->pHwSdSlotReg;
	pSdSlotReg->usClockControl = pSlot->usSdClock;
}

/**************************************************************************
*  FUNCTION NAME :
*
*      int SD_SLOT_SendCommand(sMMCrecord *pSlot, int iRspType, int iIndex, int iArgument);
*
*  DESCRIPTION : Send Command
*
*  INPUT:
*			pSlot	= Pointer of MMC structure.
*			iRspType	= Response Type
*			iIndex	= Command index
*					[8] = with Data Option
*					[9] = HC Option
*					[10] = No wait Option (used instead for auto-stop)
*			iArgument	= Command Argument.
*
*  OUTPUT:	0 or 1 = successful
*			negative = Time-out Error
*
**************************************************************************/
static int _SD_SLOT_SendCommand(PSD_SLOT_T pSlot, int iArgument)
{
	unsigned long ulCommandReg;
	PSDSLOTREG_T pSdSlotReg = ((PSDSLOTREG_T)pSlot->pHwSdSlotReg);

	ulCommandReg = (pSlot->ulCmd << 24) & 0x3F000000;
	switch(pSlot->ulRspType)
	{
		case NoRsp:
			break;
		case RspType1:
		case RspType5:
		case RspType6:
		case RspType7:
			ulCommandReg |= HwSD_TCMD_RTYPE_R48|HwSD_TCMD_CRCHK_EN|HwSD_TCMD_CICHK_EN;
			break;
		case RspType2:
			ulCommandReg |= HwSD_TCMD_RTYPE_R136|HwSD_TCMD_CRCHK_EN;
			break;
		case RspType3:
		case RspType4:
			ulCommandReg |= HwSD_TCMD_RTYPE_R48;
			break;
		case RspType1b:
			ulCommandReg|= HwSD_TCMD_RTYPE_R48CHK|HwSD_TCMD_CRCHK_EN|HwSD_TCMD_CICHK_EN;
			break;
		default:
			return -1;
	}

	// check command inhibit
	if(SD_SLOT_WaitPresentState(pSlot,HwSD_STATE_NOCMD_INHIBIT,0) != 0)
		return -1;

	if(pSlot->ucCommandType == SDMMC_COMMAND)
	{
		if(pSlot->ulCmd == CMD12)
		{
			ulCommandReg |= HwSD_TCMD_CTYPE_ABORT;
		}
		pSdSlotReg->usTransferMode = 0;
		pSdSlotReg->ulSDMA = 0;
		pSdSlotReg->usBlockSize = 0;
		pSdSlotReg->usBlockCount = 0;
	}
	else
	{
		unsigned short usTransferMode = 0;
		ulCommandReg |= HwSD_TCMD_DATSEL_PRESENT;
		pSdSlotReg->usBlockSize = pSlot->usBlockSize&0x0FFF;
		if(pSlot->usBlockCount>1)
		{
			usTransferMode |= HwSD_TCMD_ACMD12_EN|HwSD_TCMD_MS_MULTI|HwSD_TCMD_BCNTEN_EN;
		}
		pSdSlotReg->usBlockCount = pSlot->usBlockCount;

		if(pSlot->ucCommandType == SDMMC_READ)
		{
			usTransferMode |= HwSD_TCMD_DIR_READ;
		}

		if(SD_SLOT_WaitPresentState(pSlot, HwSD_STATE_NODAT_INHIBIT, 0) != 0)
			return -1;

		pSdSlotReg->usTransferMode = usTransferMode;
	}

	pSdSlotReg->ulArgument = iArgument;

	SD_SLOT_ClockOn(pSlot);

	pSdSlotReg->usCommand = (unsigned short)(ulCommandReg>>16);

	return 0;
}

static void _SD_SLOT_HandleError(PSD_SLOT_T pSlot)
{
	PSDSLOTREG_T pSdSlotReg = (PSDSLOTREG_T)pSlot->pHwSdSlotReg;
	unsigned short usErrorIntStatus;

	usErrorIntStatus = pSdSlotReg->usErrorIntStatus;
	if(usErrorIntStatus&0x000F)
	{
		// command line error
		SD_SLOT_SW_Reset(pSlot,SW_RESET_CMD);
	}
	if(usErrorIntStatus&0x0070)
	{
		// data line error
		SD_SLOT_SW_Reset(pSlot,SW_RESET_DAT);
	}
	pSdSlotReg->usErrorIntStatus = usErrorIntStatus;
}

static int SD_SLOT_WaitIntStatus(PSD_SLOT_T pSlot, unsigned short usIntStatus)
{
	PSDSLOTREG_T pSdSlotReg = (PSDSLOTREG_T)pSlot->pHwSdSlotReg;

	for(;;)
	{
		unsigned short usNormalIntStatus = pSdSlotReg->usNormalIntStatus;
		if(usNormalIntStatus&usIntStatus)
		{
			pSdSlotReg->usNormalIntStatus = usIntStatus;
			return 0;
		}
		if(usNormalIntStatus&HwSD_STS_ERR)
		{
			_SD_SLOT_HandleError(pSlot);
			return -1;
		}
		if(!SD_SLOT_IsCardDetected(pSlot))
		{
			return -1;
		}
	}
}

static int _SD_SLOT_WaitResponse(PSD_SLOT_T pSlot)
{
	PSDSLOTREG_T pSdSlotReg = (PSDSLOTREG_T)pSlot->pHwSdSlotReg;

	if(SD_SLOT_WaitIntStatus(pSlot,HwSD_STS_CDONE)!=0)
		return -1;

	if(pSlot->ulRspType != NoRsp)
	{
		unsigned short usCommand = pSdSlotReg->usCommand;
		if((usCommand&COMMAND_RTYPE_MASK) == COMMAND_RTYPE_R1b_R5b)
		{
			SD_SLOT_WaitIntStatus(pSlot,HwSD_STS_TDONE);
			SD_SLOT_SW_Reset(pSlot,SW_RESET_CMD|SW_RESET_DAT);
		}

		pSlot->ulResponse[0] = pSdSlotReg->ulResponse[0];
		pSlot->ulResponse[1] = pSdSlotReg->ulResponse[1];
		pSlot->ulResponse[2] = pSdSlotReg->ulResponse[2];
		pSlot->ulResponse[3] = pSdSlotReg->ulResponse[3];
	}

	return 0;
}

static void _SD_SLOT_Complete(PSD_SLOT_T pSlot)
{
	PSDSLOTREG_T pSdSlotReg = (PSDSLOTREG_T)pSlot->pHwSdSlotReg;

	if(pSlot->bw == 1)
		SD_SLOT_ClockOff(pSlot);

	pSdSlotReg->usNormalIntStatus = NORMAL_INT_CDONE
									| NORMAL_INT_TDONE
									| NORMAL_INT_DMA
									| NORMAL_INT_WRRDY
									| NORMAL_INT_RDRDY;
}

int SD_SLOT_SendCommand(PSD_SLOT_T pSlot, int iRspType, int iIndex, int iArgument)
{
	int res;


	pSlot->ucCommandType = SDMMC_COMMAND;
	pSlot->ulCmd = iIndex;
	pSlot->ulRspType = iRspType;
	_SD_SLOT_SendCommand(pSlot,iArgument);
	res = _SD_SLOT_WaitResponse(pSlot);
	_SD_SLOT_Complete(pSlot);

	return res;
}

int SD_SLOT_RW_Start(PSD_SLOT_T pSlot, int iRspType, int iIndex, int iArgument,int rwFlag, unsigned short usBlockCount, unsigned short usBlockSize)
{
	int res;

	if(rwFlag)
		pSlot->ucCommandType = SDMMC_WRITE;
	else
		pSlot->ucCommandType = SDMMC_READ;
	pSlot->ulCmd = iIndex;
	pSlot->ulRspType = iRspType;
	pSlot->usBlockCount = usBlockCount;
	pSlot->usBlockSize = usBlockSize;
	_SD_SLOT_SendCommand(pSlot,iArgument);
	res = _SD_SLOT_WaitResponse(pSlot);

	return res;
}

unsigned short SD_SLOT_RW_Buffer(PSD_SLOT_T pSlot, int rwFlag,void *pBuf, unsigned short usBlockCount)
{
	PSDSLOTREG_T pSdSlotReg = (PSDSLOTREG_T)pSlot->pHwSdSlotReg;
	unsigned short usBlockCountResult = 0;
	int fUseTempBuffer=0;
	unsigned long *pulBuf;
	unsigned long ulTempBuffer[BLOCKLEN];

	if(((unsigned int)pBuf)&0x03)
		fUseTempBuffer = 1;

	if(fUseTempBuffer)
		pulBuf = ulTempBuffer;
	else
		pulBuf = (unsigned long*)pBuf;

	while(usBlockCount && pSlot->usBlockCount)
	{
		unsigned short usBufCount = pSlot->usBlockSize>>2;
		unsigned short usRemained = pSlot->usBlockSize&0x3;
		volatile unsigned char *pucBufferDataPort = (volatile unsigned char *)(&pSdSlotReg->ulBufferDataPort);
		unsigned char *pucTempBuf;

		pSlot->usBlockCount--;
		usBlockCountResult++;
		usBlockCount--;

		if(rwFlag)
		{
			if(SD_SLOT_WaitIntStatus(pSlot,HwSD_STS_WRRDY)!=0)
				return 0;

			while(usBufCount--)
			{
				pSdSlotReg->ulBufferDataPort = *pulBuf++;
			}
			pucTempBuf = (unsigned char*)pulBuf;
			while(usRemained--)
			{
				*pucBufferDataPort++ = *pucTempBuf++;
			}
		}
		else
		{
			if(SD_SLOT_WaitIntStatus(pSlot,HwSD_STS_RDRDY)!=0)
				return 0;

			while(usBufCount--)
			{
				*pulBuf++ = pSdSlotReg->ulBufferDataPort;
			}
			pucTempBuf = (unsigned char*)pulBuf;
			while(usRemained--)
			{
				*pucBufferDataPort++ = *pucTempBuf++;
			}
		}
	}

	if(fUseTempBuffer)
		SDMMC_memcpy(pBuf,pulBuf,usBlockCountResult*pSlot->usBlockSize);

	if(pSlot->usBlockCount==0)
	{
		if(SD_SLOT_WaitIntStatus(pSlot,HwSD_STS_TDONE)!=0)
			return 0;
	}

	return usBlockCountResult;
}

void SD_SLOT_RW_Complete(PSD_SLOT_T pSlot)
{
	_SD_SLOT_Complete(pSlot);
}

static unsigned long SD_SLOT_GetResponse0(PSD_SLOT_T pSlot)
{
	return	pSlot->ulResponse[0];
}

static void SD_SLOT_GetResponse(PSD_SLOT_T pSlot, unsigned long pulBuf[4])
{
	unsigned char ucTempResp[16];
	unsigned char *pucTargetBuf = (unsigned char*)pulBuf;
	SDMMC_memcpy(ucTempResp,pSlot->ulResponse,16);
	SDMMC_memcpy(&pucTargetBuf[1],ucTempResp,15);
}

/**************************************************************************
*  FUNCTION NAME :
*
*      void SD_SLOT_SendReset(sMMCrecord *pSlot);
*
*  DESCRIPTION : Set SDMMC Card as Idle Mode.
*
*  INPUT:
*			pSlot	= Pointer of MMC structure.
*
*  OUTPUT:	void - Return Type
*
**************************************************************************/
static void SD_SLOT_SendReset(PSD_SLOT_T pSlot, unsigned long ulArgument)
{
	SD_SLOT_SendCommand(pSlot, NoRsp, CMD0, (int)ulArgument);
}

/**************************************************************************
*  FUNCTION NAME :
*
*      void SD_SLOT_GetAllCID(sMMCrecord *pSlot, int *iResBuf);
*
*  DESCRIPTION : Get CID (Card ID) for all card connected.
*
*  INPUT:
*			pSlot	= Pointer of MMC structure.
*			iResBuf	= pointer to store CID.
*
*  OUTPUT:	void - Return Type
*
**************************************************************************/
static void SD_SLOT_GetAllCID(PSD_SLOT_T pSlot, sCIDREG *pCIDREG)
{
	unsigned long ulBuf[4];
	SD_SLOT_SendCommand(pSlot, RspType2, CMD2, 0x00000000);
	SD_SLOT_GetResponse(pSlot, ulBuf);

	pCIDREG->MID		= ((ulBuf[3]&0xFF000000)>>24);
	pCIDREG->OID		= ((ulBuf[3]&0x00FFFF00)>>8);
	pCIDREG->PNM_U		= (ulBuf[3]&0x000000FF);
	pCIDREG->PNM_L		= ulBuf[2];
	pCIDREG->PRV		= ((ulBuf[1]&0xFF000000)>>24);
	pCIDREG->PSN		= ((ulBuf[1]&0x00FFFFFF)<<8)|((ulBuf[0]&0xFF000000)>>24);
	pCIDREG->MDT		= ((ulBuf[0]&0x000FFF00)>>8);
	pCIDREG->CRC		= ((ulBuf[0]&0x000000FE)>>1);
}

/**************************************************************************
*  FUNCTION NAME :
*
*      int SD_SLOT_GetOCR(sMMCrecord *pSlot);
*
*  DESCRIPTION : Get OCR of SD card. (MMC is not supported)
*
*  INPUT:
*			pSlot	= Pointer of MMC structure.
*
*  OUTPUT:	int - Return Type
*  			= OCR Word from Card
*
**************************************************************************/
static int SD_SLOT_GetOCR(PSD_SLOT_T pSlot, unsigned long *pOCR)
{
	int iArg;

	if (ISONE(pSlot->card_prop, SDMMC_PROP_VER20))
		iArg = 0x40780000;
	else
		iArg = 0x00780000;

	if (SD_SLOT_SendCommand(pSlot, RspType1, CMD55, 0x00000000) == 0)
	{
		if(SD_SLOT_SendCommand(pSlot, RspType3, ACMD41, iArg) == 0)
		{
			*pOCR = SD_SLOT_GetResponse0(pSlot);
			return 0;
		}
	}
	return -1;
}

/**************************************************************************
*  FUNCTION NAME :
*
*      int SD_SLOT_GetIFCOND(sMMCrecord *pSlot);
*
*  DESCRIPTION : Get Card I/F Type. (V2.0 or V1.x)
*
*  INPUT:
*			pSlot	= Pointer of MMC structure.
*
*  OUTPUT:	int - Return Type
*  			= 0 : Sucessful
*			= -1 : Time-Out error
*
**************************************************************************/
static int SD_SLOT_GetIFCOND(PSD_SLOT_T pSlot)
{
	unsigned long ulOCR;
	unsigned int iArg = 0x000001AA;
	unsigned char GetOCRTrial=0;

	BITCLR(pSlot->card_prop, SDMMC_PROP_VER20);
	BITCLR(pSlot->card_prop, SDMMC_PROP_HCS);

	if(SD_SLOT_SendCommand(pSlot, RspType7, CMD8, (int)iArg) == 0)
	{
		if (SD_SLOT_GetResponse0(pSlot) == iArg)
		{
			BITSET(pSlot->card_prop, SDMMC_PROP_VER20);
		}
	}

	for(;;)
	{
		if(SD_SLOT_GetOCR(pSlot,&ulOCR) != 0)
			return -1;

		if( (ulOCR & 0x80000000) != 0 )
			break;

		SDMMC_ms_delay(10);

		if(GetOCRTrial >= 100)
			return -1;
		else
			GetOCRTrial++;
	}

	// Check HCS(Host Capacity Support) Option
	if(ISONE(ulOCR, Hw30))
		BITSET(pSlot->card_prop, SDMMC_PROP_HCS);

	return	0;
}

/**************************************************************************
*  FUNCTION NAME :
*
*      void SD_SLOT_SendRCA(sMMCrecord *pSlot, unsigned long ulRCA);
*
*  DESCRIPTION :
*
*  INPUT:
*			pSlot	= Pointer of MMC structure.
*			ulRCA	= RCA
*
*  OUTPUT:	void - Return Type
*
**************************************************************************/
static unsigned long SD_SLOT_SendRCA(PSD_SLOT_T pSlot, unsigned long ulRCA)
{
	SD_SLOT_SendCommand(pSlot, RspType6, CMD3, ulRCA);
	return SD_SLOT_GetResponse0(pSlot) & 0xFFFF0000;
}

/**************************************************************************
*  FUNCTION NAME :
*
*      int SD_SLOT_SD_ResetCMD(sMMCrecord *pSlot, int *iResBuf, char *cPNMBuf, int * iRCABuf);
*
*  DESCRIPTION : Processing Reset Sequence for SD Card. (OCR, CID are acquired)
*
*  INPUT:
*			pSlot	= Pointer of MMC structure.
*			iResBuf	= Response buffer
*			cPNMBuf	= pointer to store Product Name.
*			iRCABuf	= pointer to store RCA.
*
*  OUTPUT:	int - Return Type
*  			= -1 : Error occurred in processing reset sequence for SD card. (ex. OCR failure)
*			= 0 : SD Card reset sequence is successful.
*
**************************************************************************/
int SD_SLOT_SD_ResetCMD(PSD_SLOT_T pSlot, int *iResBuf, char *cPNMBuf, int * iRCABuf)
{
	SD_SLOT_SendReset(pSlot,0);

	// Check V2.00 or V1.xx with CMD8
	if (SD_SLOT_GetIFCOND(pSlot) < 0)
		return -1;

	SD_SLOT_GetAllCID(pSlot, &pSlot->stCID);

	cPNMBuf[0]	= pSlot->stCID.PNM_U;
	cPNMBuf[1]	= (pSlot->stCID.PNM_L & 0xFF000000)>>24;
	cPNMBuf[2]	= (pSlot->stCID.PNM_L & 0x00FF0000)>>16;
	cPNMBuf[3]	= (pSlot->stCID.PNM_L & 0x0000FF00)>>8;
	cPNMBuf[4]	= pSlot->stCID.PNM_L & 0x000000FF;

	pSlot->rca = SD_SLOT_SendRCA(pSlot,0);

	return 0;
}

/**************************************************************************
*  FUNCTION NAME :
*
*      int SD_SLOT_MMC_ResetCMD(sMMCrecord *pSlot, int *iResBuf, char *cPNMBuf, int * iRCABuf);
*
*  DESCRIPTION : Processing Reset Sequence for MMC Card. (OCR, CID are acquired)
*
*  INPUT:
*			pSlot	= Pointer of MMC structure.
*			iResBuf	= Response buffer
*			cPNMBuf	= pointer to store Product Name.
*			iRCABuf	= pointer to store RCA.
*
*  OUTPUT:	int - Return Type
*  			= -1 : Error occurred in processing reset sequence for MMC card. (ex. OCR failure)
*			= 0 : MMC Card reset sequence is successful.
*
**************************************************************************/
int SD_SLOT_MMC_ResetCMD(PSD_SLOT_T pSlot, int *iResBuf, char *cPNMBuf, int * iRCABuf)
{
	int iRspValue;
	unsigned char ucTrial=0;

	SD_SLOT_SendReset(pSlot,0);

	while (1)
	{
		SDMMC_ms_delay(10);

		if(SD_SLOT_SendCommand(pSlot, RspType3, CMD1, 0x40FF8000))
			return (-1);

		iRspValue = SD_SLOT_GetResponse0(pSlot);

		if (ISONE(iRspValue, Hw31))
			break;

		if(ucTrial >= 100)
			return -1;
		else
			ucTrial++;
	}

	// Check "Access mode"
	// 00b - byte mode, 10b - sector mode
	if( (iRspValue&0x60000000) == 0 )
	{
		BITCLR(pSlot->card_prop, SDMMC_PROP_HCS);
	}
	else if( (iRspValue&0x60000000) == 0x40000000 )
	{
		BITSET(pSlot->card_prop, SDMMC_PROP_HCS);
	}
	else
	{
		return -1;
	}

	SD_SLOT_GetAllCID(pSlot, &pSlot->stCID);

	cPNMBuf[0]	= pSlot->stCID.PNM_U;
	cPNMBuf[1]	= (pSlot->stCID.PNM_L & 0xFF000000)>>24;
	cPNMBuf[2]	= (pSlot->stCID.PNM_L & 0x00FF0000)>>16;
	cPNMBuf[3]	= (pSlot->stCID.PNM_L & 0x0000FF00)>>8;
	cPNMBuf[4]	= pSlot->stCID.PNM_L & 0x000000FF;

	// for MMC cards, RCA should be unique
	pSlot->rca = (unsigned long)(pSlot->stCID.OID + pSlot->iIndex);
	pSlot->rca = pSlot->rca<<16;
	SD_SLOT_SendRCA(pSlot, pSlot->rca);

	return 0;
}

/**************************************************************************
*  FUNCTION NAME :
*
*      int SD_SLOT_GetSCR(sMMCrecord *pSlot, int iRCA,unsigned int *buf);
*
*  DESCRIPTION : Read SCR(SD Configuration Register).
*
*  INPUT:
*			pSlot	= Pointer of MMC structure.
*			iRCA	= RCA value (must be valid RCA)
*			buf	= pointer to store SCR.
*
*  OUTPUT:	int - Return Type
*  			= Response word of ACMD51 command.
*
**************************************************************************/
int SD_SLOT_GetSCR(PSD_SLOT_T pSlot, int iRCA,sSCR_Register *pSCR)
{
	unsigned uARGU0=0;
	PSDSLOTREG_T pSdSlotReg = pSlot->pHwSdSlotReg;
	unsigned char ucBuf[8];
	pSdSlotReg->usBlockSize = 8;

	if(SD_SLOT_SendCommand(pSlot, RspType1, CMD55, iRCA) == 0)
	{
		if(SD_SLOT_RW_Start(pSlot, RspType1, ACMD51, 0x00000000, 0, 1, 8) == 0)
		{
			if(SD_SLOT_RW_Buffer(pSlot, 0, (void *)ucBuf, 1) == 1)
				uARGU0 = SD_SLOT_GetResponse0(pSlot);
		}
		SD_SLOT_RW_Complete(pSlot);

		pSCR->scr_structure = (ucBuf[0]&0xF0)>>4;
		pSCR->sd_spec = ucBuf[0]&0x0F;
		pSCR->data_stat_after_erase = (ucBuf[1]&0x80)>>7;
		pSCR->sd_security = (ucBuf[1]&0x70)>>4;
		pSCR->sd_bus_widths = ucBuf[1]&0x0F;
	}

	return -1;
}

void SD_SLOT_GetCSD_441(PSD_SLOT_T pSlot, int iRCA, sCSDREG *pCSDREG, unsigned long *ulBuf)
{
	pSlot->csd_441.CRC               = (unsigned char)( (ulBuf[0] & 0x000000FE) >> 1 );		// CSD[0]--0000_0000_000x_xxx0__0000_0000_1111_1110 [ 7: 1]
	pSlot->csd_441.ECC               = (unsigned char)( (ulBuf[0] & 0x00000300) >> 8 );		// CSD[0]--0000_0000_000x_xxx0__0000_0011_0000_0000 [ 9: 8]
	pSlot->csd_441.FILE_FORMAT       = (unsigned char)( (ulBuf[0] & 0x00000C00) >> 10 );	// CSD[0]--0000_0000_000x_xxx0__0000_1100_0000_0000	[11:10]
	pSlot->csd_441.TMP_WRITE_PROTECT = (unsigned char)( (ulBuf[0] & 0x00001000) >> 12 );	// CSD[0]--0000_0000_000x_xxx0__0001_0000_0000_0000 [12:12]
	pSlot->csd_441.PERM_WRITE_PROTECT= (unsigned char)( (ulBuf[0] & 0x00002000) >> 13 );	// CSD[0]--0000_0000_000x_xxx0__0010_0000_0000_0000 [13:13]
	pSlot->csd_441.COPY              = (unsigned char)( (ulBuf[0] & 0x00004000) >> 14 );	// CSD[0]--0000_0000_000x_xxx0__0100_0000_0000_0000 [14:14]
	pSlot->csd_441.FILE_FORMAT_GRP   = (unsigned char)( (ulBuf[0] & 0x00008000) >> 15 );	// CSD[0]--0000_0000_000x_xxx0__1000_0000_0000_0000 [15:15]
	pSlot->csd_441.CONTENT_PROT_APP  = (unsigned char)( (ulBuf[0] & 0x00010000) >> 16 );  // CSD[0]--0000_0000_000x_xxx1__0000_0000_0000_0000 [16:16]
	pSlot->csd_441.WRITE_BL_PARTIAL  = (unsigned char)( (ulBuf[0] & 0x00200000) >> 21 );	// CSD[0]--0000_0000_001x_xxx0__0000_0000_0000_0000 [21:21]
	pSlot->csd_441.WRITE_BL_LEN      = (unsigned char)( (ulBuf[0] & 0x03C00000) >> 22 );  // CSD[0]--0000_0011_110x_xxx0__0000_0000_0000_0000 [25:22]
	pSlot->csd_441.R2W_FACTOR        = (unsigned char)( (ulBuf[0] & 0x1C000000) >> 26 );	// CSD[0]--0001_1100_000x_xxx0__0000_0000_0000_0000 [28:26]
	pSlot->csd_441.DEFAULT_ECC       = (unsigned char)( (ulBuf[0] & 0x60000000) >> 29 );	// CSD[0]--0110_0000_000x_xxx0__0000_0000_0000_0000 [30:29]
	pSlot->csd_441.WP_GRP_ENABLE     = (unsigned char)( (ulBuf[0] & 0x80000000) >> 31 );	// CSD[0]--1000_0000_000x_xxx0__0000_0000_0000_0000 [31:31]

	pSlot->csd_441.WP_GRP_SIZE       = (unsigned char)( (ulBuf[1] & 0x0000001F) >> 0  );	// CSD[1]--0000_0000_0000_0000__0000_0000_0001_1111 [ 4: 0] [36:32]
	pSlot->csd_441.ERASE_GRP_MULT    = (unsigned char)( (ulBuf[1] & 0x000003E0) >> 5  );	// CSD[1]--0000_0000_0000_0000__0000_0011_1110_0000 [ 9: 5] [41:37]
	pSlot->csd_441.ERASE_GRP_SIZE    = (unsigned char)( (ulBuf[1] & 0x00007C00) >> 10 );	// CSD[1]--0000_0000_0000_0000__0111_1100_0000_0000 [14:10] [46:37]
	pSlot->csd_441.C_SIZE_MULT       = (unsigned char)( (ulBuf[1] & 0x00038000) >> 15 );	// CSD[1]--0000_0000_0000_0011__1000_0000_0000_0000 [17:15] [49:47]
	pSlot->csd_441.VDD_W_CURR_MAX    = (unsigned char)( (ulBuf[1] & 0x001C0000) >> 18 );  // CSD[1]--0000_0000_0001_1100__0000_0000_0000_0000 [20:18] [52:50]
	pSlot->csd_441.VDD_W_CURR_MIN    = (unsigned char)( (ulBuf[1] & 0x00E00000) >> 21 );	// CSD[1]--0000_0000_1110_0000__0000_0000_0000_0000 [23:21] [55:53]
	pSlot->csd_441.VDD_R_CURR_MAX    = (unsigned char)( (ulBuf[1] & 0x07000000) >> 24 );	// CSD[1]--0000_0111_0000_0000__0000_0000_0000_0000 [26:24] [58:56]
	pSlot->csd_441.VDD_R_CURR_MIN    = (unsigned char)( (ulBuf[1] & 0x07000000) >> 27 );	// CSD[1]--0011_1000_0000_0000__0000_0000_0000_0000 [29:27] [61:59]
	pSlot->csd_441.C_SIZE            = (unsigned long)( ((ulBuf[2] & 0x000003FF) << 2) | ((ulBuf[1] & 0xC0000000) >> 30) );
	// CSD[2]--0000_0000_0000_0000__0000_0011_1111_1111 [9:0] [73:64] | CSD[1]--1100_0000_0000_0000__0000_0000_0000_0000 [31:30] [63:62]
	pSlot->csd_441.DSR_IMP           = (unsigned char)( (ulBuf[2] & 0x00001000) >> 12 );	// CSD[2]--0000_0000_0000_0000__0001_XX00_0000_0000 [12:12] [76:76]
	pSlot->csd_441.READ_BLK_MISALIGN = (unsigned char)( (ulBuf[2] & 0x00002000) >> 13 );	// CSD[2]--0000_0000_0000_0000__0010_XX00_0000_0000 [13:13] [77:77]
	pSlot->csd_441.WRITE_BLK_MISALIGN= (unsigned char)( (ulBuf[2] & 0x00004000) >> 14 );	// CSD[2]--0000_0000_0000_0000__0100_XX00_0000_0000 [14:14] [78:78]
	pSlot->csd_441.READ_BL_PARTIAL   = (unsigned char)( (ulBuf[2] & 0x00008000) >> 15 );	// CSD[2]--0000_0000_0000_0000__1000_XX00_0000_0000 [15:15] [79:79]
	pSlot->csd_441.READ_BL_LEN       = (unsigned char)( (ulBuf[2] & 0x000F0000) >> 16 );	// CSD[2]--0000_0000_0000_1111__0000_XX00_0000_0000 [19:16] [83:80]
	pSlot->csd_441.CCC               = (unsigned char)( (ulBuf[2] & 0xFFF00000) >> 20 );	// CSD[2]--1111_1111_1111_0000__0000_XX00_0000_0000 [31:20] [95:84]

	pSlot->csd_441.TRAN_SPEED        = (unsigned char)( (ulBuf[3] & 0x000000FF) >> 0  );	// CSD[3]--0000_00xx_0000_0000__0000_0000_1111_1111 [7:0] 	[103:96]
	pSlot->csd_441.NSAC              = (unsigned char)( (ulBuf[3] & 0x0000FF00) >> 8  );	// CSD[3]--0000_00xx_0000_0000__1111_1111_0000_0000 [15:8] 	[111:104]
	pSlot->csd_441.TAAC              = (unsigned char)( (ulBuf[3] & 0x00FF0000) >> 16 );	// CSD[3]--0000_00xx_1111_1111__0000_0000_0000_0000 [23:16] [119:112]
	pSlot->csd_441.SPEC_VERS         = (unsigned char)( (ulBuf[3] & 0x3C000000) >> 26 );	// CSD[3]--0011_11xx_0000_0000__0000_0000_0000_0000 [29:26] [125:122]
	pSlot->csd_441.CSD_STRUCTURE     = (unsigned char)( (ulBuf[3] & 0xC0000000) >> 30 );	// CSD[3]--1100_0000_0000_0000__0000_0000_0000_0000 [31:30] [127:126]
}

/**************************************************************************
*  FUNCTION NAME :
*
*      void SD_SLOT_GetCSD(sMMCrecord *pSlot, int iRCA, int *iResBuf);
*
*  DESCRIPTION : Get CSD (Card Specific Data)
*
*  INPUT:
*			pSlot	= Pointer of MMC structure.
*			iRCA	= RCA value for Card.
*			iResBuf	= pointer to store CSD.
*
*  OUTPUT:	void - Return Type
*
**************************************************************************/
void SD_SLOT_GetCSD(PSD_SLOT_T pSlot, int iRCA, sCSDREG *pCSDREG)
{
	unsigned long ulBuf[4];
	SD_SLOT_SendCommand(pSlot, RspType2, CMD9, iRCA);
	SD_SLOT_GetResponse(pSlot, ulBuf);

	pSlot->stCSD.CSDS			= (unsigned char)((ulBuf[3]&0xC0000000)>>30);

	// Check CSD structure version
	if( (pSlot->id == MMC_CARD_SD) && (pSlot->stCSD.CSDS >= 1) )
	{
		/* SD's CSD Version 2.0 or higher */
		pSlot->stCSD.SPEC_VERS				= (unsigned char)((ulBuf[3]&0x3C000000)>>26);
		pSlot->stCSD.TAAC					= (unsigned char)((ulBuf[3]&0x00FF0000)>>16);
		pSlot->stCSD.NSAC					= (unsigned char)((ulBuf[3]&0x0000FF00)>>8);
		pSlot->stCSD.TRAN_SPEED				= (unsigned char)(ulBuf[3]&0x000000FF);
		pSlot->stCSD.CCC					= (unsigned short)((ulBuf[2]&0xFFF00000)>>20);
		pSlot->stCSD.READ_BL_LEN			= (unsigned char)((ulBuf[2]&0x000F0000)>>16);
		pSlot->stCSD.READ_BL_PARTIAL		= (unsigned char)((ulBuf[2]&0x00008000)>>15);
		pSlot->stCSD.WRITE_BL_MISALIGN		= (unsigned char)((ulBuf[2]&0x00004000)>>14);
		pSlot->stCSD.READ_BL_MISALIGN		= (unsigned char)((ulBuf[2]&0x00002000)>>13);
		pSlot->stCSD.DSR_IMP				= (unsigned char)((ulBuf[2]&0x00001000)>>12);
		pSlot->stCSD.C_SIZE					= ((ulBuf[2]&0x0000003F)<<16)|((ulBuf[1]&0xFFFF0000)>>16);
		pSlot->stCSD.ERASE_BL_EN			= (unsigned char)((ulBuf[1]&0x00004000)>>14);
		pSlot->stCSD.SECTOR_SIZE			= (unsigned char)((ulBuf[1]&0x00003F80)>>7);
		pSlot->stCSD.WP_GRP_SIZE			= (unsigned char)(ulBuf[1]&0x0000007F);
		pSlot->stCSD.WP_GRP_ENABLE			= (unsigned char)((ulBuf[0]&0x80000000)>>31);
		pSlot->stCSD.R2W_FACTOR				= (unsigned char)((ulBuf[0]&0x1C000000)>>26);
		pSlot->stCSD.WRITE_BL_LEN			= (unsigned char)((ulBuf[0]&0x03C00000)>>22);
		pSlot->stCSD.WRITE_BL_PARTIAL		= (unsigned char)((ulBuf[0]&0x00200000)>>21);
		pSlot->stCSD.FILE_FORMAT_GRP		= (unsigned char)((ulBuf[0]&0x00008000)>>15);
		pSlot->stCSD.COPY					= (unsigned char)((ulBuf[0]&0x00004000)>>14);
		pSlot->stCSD.PERM_WRITE_PROTECT		= (unsigned char)((ulBuf[0]&0x00002000)>>13);
		pSlot->stCSD.TMP_WRITE_PROTECT		= (unsigned char)((ulBuf[0]&0x00001000)>>12);
		pSlot->stCSD.FILE_FORMAT			= (unsigned char)((ulBuf[0]&0x00000C00)>>10);
		pSlot->stCSD.CRC					= (unsigned char)((ulBuf[0]&0x000000FE)>>1);
	}
	else
	{
		/* SD's CSD Version 1.0 or MMC's CSD Version any */
		pSlot->stCSD.SPEC_VERS				= (unsigned char)((ulBuf[3]&0x3C000000)>>26);
		pSlot->stCSD.TAAC					= (unsigned char)((ulBuf[3]&0x00FF0000)>>16);
		pSlot->stCSD.NSAC					= (unsigned char)((ulBuf[3]&0x0000FF00)>>8);
		pSlot->stCSD.TRAN_SPEED				= (unsigned char)(ulBuf[3]&0x000000FF);
		pSlot->stCSD.CCC					= (unsigned short)((ulBuf[2]&0xFFF00000)>>20);
		pSlot->stCSD.READ_BL_LEN			= (unsigned char)((ulBuf[2]&0x000F0000)>>16);
		pSlot->stCSD.READ_BL_PARTIAL		= (unsigned char)((ulBuf[2]&0x00008000)>>15);
		pSlot->stCSD.WRITE_BL_MISALIGN		= (unsigned char)((ulBuf[2]&0x00004000)>>14);
		pSlot->stCSD.READ_BL_MISALIGN		= (unsigned char)((ulBuf[2]&0x00002000)>>13);
		pSlot->stCSD.DSR_IMP				= (unsigned char)((ulBuf[2]&0x00001000)>>12);
		pSlot->stCSD.C_SIZE					= ((ulBuf[2]&0x000003FF)<<2)|(ulBuf[1]&0xC0000000)>>30;
		pSlot->stCSD.VDD_R_CURR_MIN			= (unsigned char)((ulBuf[1]&0x38000000)>>27);
		pSlot->stCSD.VDD_R_CURR_MAX			= (unsigned char)((ulBuf[1]&0x07000000)>>24);
		pSlot->stCSD.VDD_W_CURR_MIN			= (unsigned char)((ulBuf[1]&0x00E00000)>>21);
		pSlot->stCSD.VDD_W_CURR_MAX			= (unsigned char)((ulBuf[1]&0x001C0000)>>18);
		pSlot->stCSD.C_SIZE_MULT			= (unsigned char)((ulBuf[1]&0x00038000)>>15);
		pSlot->stCSD.ERASE_BL_EN			= (unsigned char)((ulBuf[1]&0x00004000)>>14);
		pSlot->stCSD.SECTOR_SIZE			= (unsigned char)((ulBuf[1]&0x00003F80)>>7);
		pSlot->stCSD.WP_GRP_SIZE			= (unsigned char)(ulBuf[1]&0x0000007F);
		pSlot->stCSD.WP_GRP_ENABLE			= (unsigned char)((ulBuf[0]&0x80000000)>>31);
		pSlot->stCSD.R2W_FACTOR				= (unsigned char)((ulBuf[0]&0x1C000000)>>26);
		pSlot->stCSD.WRITE_BL_LEN			= (unsigned char)((ulBuf[0]&0x03C00000)>>22);
		pSlot->stCSD.WRITE_BL_PARTIAL		= (unsigned char)((ulBuf[0]&0x00200000)>>21);
		pSlot->stCSD.FILE_FORMAT_GRP		= (unsigned char)((ulBuf[0]&0x00008000)>>15);
		pSlot->stCSD.COPY					= (unsigned char)((ulBuf[0]&0x00004000)>>14);
		pSlot->stCSD.PERM_WRITE_PROTECT		= (unsigned char)((ulBuf[0]&0x00002000)>>13);
		pSlot->stCSD.TMP_WRITE_PROTECT		= (unsigned char)((ulBuf[0]&0x00001000)>>12);
		pSlot->stCSD.FILE_FORMAT			= (unsigned char)((ulBuf[0]&0x00000C00)>>10);
		pSlot->stCSD.CRC					= (unsigned char)((ulBuf[0]&0x000000FE)>>1);
		
		SD_SLOT_GetCSD_441(pSlot, iRCA, pCSDREG, ulBuf);	// by B120040-ksjung
	}
}

/**************************************************************************
*  FUNCTION NAME :
*
*      int SD_SLOT_SelectBWCommand(sMMCrecord *pSlot, int mmcplus_buswidth);
*
*  DESCRIPTION : Set BusWidth to Card.
*
*  INPUT:
*			pSlot	= Pointer of MMC structure.
*			mmcplus_buswidth	= 1, 4, 8
*
*  OUTPUT:	0 or 1 = Sucessful
*			negative = Time-out Error
*
**************************************************************************/
int SD_SLOT_SendBWCommand(PSD_SLOT_T pSlot, int mmcplus_buswidth)
{
	int	iRet = -1;

	if (pSlot->id == MMC_CARD_MMCPLUS)
	{
		if (mmcplus_buswidth == 8)
			iRet	= SD_SLOT_SendCommand(pSlot, RspType1b, CMD6, 0x03B70200);	// 8bit Mode
		else if (mmcplus_buswidth == 4)
			iRet	= SD_SLOT_SendCommand(pSlot, RspType1b, CMD6, 0x03B70100);	// 4bit Mode
		else if (mmcplus_buswidth == 1)
			iRet	= SD_SLOT_SendCommand(pSlot, RspType1b, CMD6, 0x03B70000);	// 1bit Mode
	}
	else if (pSlot->id == MMC_CARD_SD)
	{
		if(mmcplus_buswidth == 4)
		{
			if(SD_SLOT_SendCommand(pSlot, RspType1, CMD55, pSlot->rca) == 0)
				iRet = SD_SLOT_SendCommand(pSlot, RspType1, ACMD6, 2/*4 bit*/);
		}
	}

	return	iRet;
}

/**************************************************************************
*  FUNCTION NAME :
*
*      unsigned SD_SLOT_CalcTAAC(sCSDREG *pstCSD);
*
*  DESCRIPTION : Calculate Access Time for Writing.
*
*  INPUT:
*			pstCSD	= pointer of structure CSDREG
*
*  OUTPUT:	unsigned - Return Type
*  			= Calculated Access Time for Writing. (unit of 2.67us)
*
**************************************************************************/
unsigned SD_SLOT_CalcTAAC(sCSDREG *pstCSD)
{
	unsigned	uRealTAAC	= 1;
	unsigned	uUnit	= (pstCSD->TAAC & 7);
	unsigned	uIndex	= (pstCSD->TAAC & 0x78) >> 3;
	unsigned	uNSAC;

	if (uIndex == 0)
		return	1;

	while (uUnit > 0)
	{
		uRealTAAC *= 10;
		uUnit --;
	}
	//WINCE
	uNSAC	= (pstCSD->NSAC*5000) / 2670;
	uNSAC	= (pstCSD->NSAC*5000) / 1000;

	if (uIndex == 1)
		//uRealTAAC	= (uRealTAAC / 1000 + uNSAC);
		uRealTAAC	= (uRealTAAC / 2670 + uNSAC);
	else if (uIndex < 4)
		//uRealTAAC	= (((10+uIndex)*uRealTAAC) / 10000 + uNSAC);
		uRealTAAC	= (((10+uIndex)*uRealTAAC) / 26700 + uNSAC);
	else if (uIndex < 13)
		//uRealTAAC	= (((5*uIndex-5)*uRealTAAC) / 10000 + uNSAC);
		uRealTAAC	= (((5*uIndex-5)*uRealTAAC) / 26700 + uNSAC);
	else
		//uRealTAAC	= (((uIndex - 7)*uRealTAAC) / 1000 + uNSAC);
		uRealTAAC	= (((uIndex - 7)*uRealTAAC) / 2670 + uNSAC);

	return	(uRealTAAC << pstCSD->R2W_FACTOR);
}

/**************************************************************************
*  FUNCTION NAME :
*
*      unsigned SD_SLOT_CalcTRAN(sCSDREG *pstCSD);
*
*  DESCRIPTION : Calculate Transfer Frequency
*
*  INPUT:
*			pstCSD	= pointer of structure CSDREG
*
*  OUTPUT:	unsigned - Return Type
*  			= Calculated Transfer frequency. (unit of 1KHz)
*
**************************************************************************/
unsigned SD_SLOT_CalcTRAN(sCSDREG *pstCSD)
{
	int	uUnit	= pstCSD->TRAN_SPEED & 0x07;
	int	uIndex	= (pstCSD->TRAN_SPEED>>3) & 0x0F;
	const unsigned int uiFreqUnit[] = {
		10, 100, 1000, 10000, 0, 0, 0, 0
	};
	const unsigned int uiMultFactor[] = {
		0, 10, 12, 13, 15, 20, 26, 30, 35, 40, 45, 52, 55, 60, 70, 80
	};
	return uiFreqUnit[uUnit]*uiMultFactor[uIndex];
}

/**************************************************************************
*  FUNCTION NAME :
*
*      void SD_SLOT_GoTransfer(sMMCrecord *pSlot, int iRCA);
*
*  DESCRIPTION : Set SDMMC Card into Transfer Mode.
*
*  INPUT:
*			pSlot	= Pointer of MMC structure.
*			iRCA	= RCA value.
*
*  OUTPUT:	void - Return Type
*
**************************************************************************/
void SD_SLOT_GoTransfer(PSD_SLOT_T pSlot, int iRCA)
{
	SD_SLOT_SendCommand(pSlot, RspType1, CMD7, iRCA);
}

/**************************************************************************
*  FUNCTION NAME :
*
*      int	SD_SLOT_MMC_ReadEXTCSD(sMMCrecord *pSlot, unsigned char *ucBuf);
*
*  DESCRIPTION : Read Extended CSD Register.
*
*  INPUT:
*			pSlot	= Pointer of MMC structure.
*			ucBuf	= pointer of EXTCSD structure.
*
*  OUTPUT:	int - Return Type
*  			= 0 : successful, -1 : Error
*
**************************************************************************/
int SD_SLOT_MMC_ReadEXTCSD(PSD_SLOT_T pSlot, unsigned char *ucBuf)
{
	int iError = -1;

	if(SD_SLOT_RW_Start(pSlot, RspType1, CMD8, 0x00000000, 0, 1, 512) == 0)
	{
		if(SD_SLOT_RW_Buffer(pSlot, 0, ucBuf, 1) == 1)
			iError = 0;
	}
	SD_SLOT_RW_Complete(pSlot);

	return iError;
}

/**************************************************************************
*  FUNCTION NAME :
*
*      unsigned long SDMMC_GetSize(sMMCrecord *pSlot);
*
*  DESCRIPTION : Get SDMMC Size from CSD word.
*
*  INPUT:
*			pSlot	= Pointer of MMC structure.
*
*  OUTPUT:	unsigned long - Return Type
*  			= total number of sectors
*
**************************************************************************/
unsigned long SD_SLOT_GetSize(PSD_SLOT_T pSlot)
{
	int	iRead;
	unsigned long ulSize;
	int	iBlock, iSizeMul, iMult, iBlockNr;

	if ((pSlot->id == MMC_CARD_MMCPLUS) && (pSlot->stEXTCSD.SecCount > 0))
	{
		return pSlot->stEXTCSD.SecCount;
	}
	else
	{
		if (ISZERO(pSlot->card_prop, SDMMC_PROP_VER20))	// Ver1.xx
		{
			iRead = pSlot->stCSD.READ_BL_LEN;
			iBlock = 1 << iRead;				// BLOCK_LEN

			iSizeMul = pSlot->stCSD.C_SIZE_MULT;
			iMult = 1<< (iSizeMul+2);			// MULT

			ulSize = pSlot->stCSD.C_SIZE;

			iBlockNr = (ulSize+1) *iMult;			// BLOCKNR
			ulSize = iBlockNr;
			ulSize = (ulSize* iBlock) / 0x200;
		}
		else
		{
			ulSize = pSlot->stCSD.C_SIZE;
			ulSize += 1;
			//ulSize = (ulSize / 0x200) * 0x80000;
			ulSize = (ulSize * (0x80000 / 0x200));
		}

		return (ulSize);
	}

	return	0;
}

int SD_SLOT_switchEXTCSD(PSD_SLOT_T pSlot, unsigned short index, unsigned char value, unsigned char cmdSet)
{
	unsigned int arg=0;
	arg = ((0x3/*WR_BYTES*/) << 24)
		| ((index & 0xff) << 16)
		| ((value & 0xff) << 8)
		| (cmdSet & 0x7);
	return SD_SLOT_SendCommand(pSlot, RspType1b, CMD6, arg);
}

void SD_SLOT_SetInterface(PSD_SLOT_T pSlot)
{
#if 1
	PSDSLOTREG_T pSdSlotReg = (PSDSLOTREG_T)pSlot->pHwSdSlotReg;
	if(pSlot->bw == 1)
	{
		BITCLR(pSdSlotReg->ucHostControl,HOSTCTRL_SD8|HOSTCTRL_SD4);
	}
	else if(pSlot->bw == 4)
	{
		BITCSET(pSdSlotReg->ucHostControl,HOSTCTRL_SD8,HOSTCTRL_SD4);
	}
	else if(pSlot->bw == 8)
	{
		BITCSET(pSdSlotReg->ucHostControl,HOSTCTRL_SD4,HOSTCTRL_SD8);
	}
	SD_SLOT_SetClockRate(pSlot);
#else
	sHwSDMMC *pHwSdSlotReg = (sHwSDMMC*)pSlot->pHwSdSlotReg;
	if(pSlot->bw == 1)
	{
		BITCLR(pHwSdSlotReg->CTRL,HwSD_CTRL_SD8_EN|HwSD_CTRL_SD4_EN);
	}
	else if(pSlot->bw == 4)
	{
		BITCSET(pHwSdSlotReg->CTRL,HwSD_CTRL_SD8_EN,HwSD_CTRL_SD4_EN);
	}
	else if(pSlot->bw == 8)
	{
		BITCSET(pHwSdSlotReg->CTRL,HwSD_CTRL_SD4_EN,HwSD_CTRL_SD8_EN);
	}
	SD_SLOT_SetClockRate(pSlot);
#endif
}

static int SD_SwitchHighSpeed(PSD_SLOT_T pSlot, int mode)
{
	CMD6_STATUS_T stStatus;
	int res = -1;
	unsigned long arg = 0x00FFFFF1;

	if(mode)
		arg |= 0x80000000;

	if(SD_SLOT_RW_Start(pSlot, RspType1, CMD6, arg, 0, 1, 64) == 0)
	{
		if(SD_SLOT_RW_Buffer(pSlot, 0, (void*)&stStatus, 1) == 1)
			res = 0;
	}
	SD_SLOT_RW_Complete(pSlot);
	if(res != 0)
		return -1;
	if((stStatus.ucFunctionGroup1_L&0x2) == 0)
		return -1;
	if((stStatus.ucSelectedFunctionGroup2_1&0xF) != 0x1)
		return -1;
	if(stStatus.ucDataStructureVersion>=0x01 && stStatus.ucBusyGroup1_L&0x02)
		return -1;

	return 0;
}

int SD_SLOT_SendHighSpeedCommand(PSD_SLOT_T pSlot)
{
	if(pSlot->id == MMC_CARD_SD)
	{
		if(pSlot->stSCR.sd_spec > 0)		// Version 1.10 or later supports CMD6
		{
			if(SD_SwitchHighSpeed(pSlot,0) != 0)
				return -1;
			if(SD_SwitchHighSpeed(pSlot,1) != 0)
				return -1;

			// wait 8 clocks for function switching

			return 0;
		}
	}
	else if(pSlot->id == MMC_CARD_MMCPLUS)
	{
		if(SD_SLOT_switchEXTCSD(pSlot,185,1,0) != 0)		// high speed mode
			return -1;

		return 0;
	}

	return -1;
}

void SD_SLOT_Start(PSD_SLOT_T pSlot)
{
	SD_SLOT_SetClockRate(pSlot);
	SD_SLOT_ClockOn(pSlot);
	SDMMC_ms_delay(1/*ms*/);
	SD_SLOT_ClockOff(pSlot);
}

void SD_SLOT_Attach(PSD_SLOT_T pSlot)
{
	SD_SLOT_SetVoltage(pSlot,7);
	//SDMMC_ms_delay(100/*ms*/);	// It doesn't needed actually.
	SD_SLOT_SetClockRate(pSlot);
	SD_SLOT_ClockOn(pSlot);
	SDMMC_ms_delay(1/*ms*/);
	SD_SLOT_ClockOff(pSlot);

	// wait card power up
	SDMMC_ms_delay(50/*ms*/);
}

void SD_SLOT_Detach(PSD_SLOT_T pSlot)
{
	PSDSLOTREG_T pSdSlotReg = (PSDSLOTREG_T)pSlot->pHwSdSlotReg;

	// turn off power and clock
	SD_SLOT_SetVoltage(pSlot,0);
	SD_SLOT_ClockOff(pSlot);

	// software reset for all
	SD_SLOT_SW_Reset(pSlot,SW_RESET_ALL);

	// It needs slight delay
	SDMMC_ms_delay(1);

	SD_SLOT_SetTimeout(pSlot,0xE);

	pSdSlotReg->usNormalIntStatusEnable = 0xFFFF;
	pSdSlotReg->usErrorIntStatusEnable = 0x0FFF;
}

int SD_SLOT_GetTotalSlotCount(void)
{
	return SD_HW_GetTotalSlotCount();
}

int SD_SLOT_Read_SDStatus(PSD_SLOT_T pSlot, int iRCA)
{
	int i = 0;
	unsigned uARGU0=0;
	PSDSLOTREG_T pSdSlotReg = pSlot->pHwSdSlotReg;
	unsigned char ucBuf[64];
	pSdSlotReg->usBlockSize = 64;

	if(SD_SLOT_SendCommand(pSlot, RspType1, CMD55, iRCA) == 0)
	{
		if(SD_SLOT_RW_Start(pSlot, RspType1, ACMD13, 0x00000000, 0, 1, 64) == 0)
		{
			if(SD_SLOT_RW_Buffer(pSlot, 0, (void *)ucBuf, 1) == 1)
				uARGU0 = SD_SLOT_GetResponse0(pSlot);
			printf("[ ksjung ] Response : %lx\n", pSlot->ulResponse[0]);
		}
		SD_SLOT_RW_Complete(pSlot);

// 511==buf[0], 503==buf[1], 495==buf[2], 487==buf[3], 479==buf[4], 471==buf[5], 463==buf[6]
// 455==buf[7], 447==buf[8], 439==buf[9], 431==buf[10], 423==buf[11], 415==buf[12], 407=buf[13]
// 399==buf[14], 391==buf[15], 439==buf[9], 431==buf[10], 423==buf[11], 415==buf[12], 407=buf[13]
		pSlot->sd_status.DAT_BUS_WIDTH = (unsigned long)( (ucBuf[0] & 0xC0) >> 6 ); 
		pSlot->sd_status.SECURED_MODE = (unsigned long)( (ucBuf[0] & 0x20) >> 5 );
		pSlot->sd_status.SD_CARD_TYPE = (unsigned long)( (ucBuf[2] << 8) | (ucBuf[3]) );
		pSlot->sd_status.SIZE_OF_PROTECTED_AREA = (unsigned long)( (ucBuf[4] << 24) | (ucBuf[5] << 16) | (ucBuf[6] << 8) | ucBuf[7] );
		pSlot->sd_status.SPEED_CLASS = (unsigned long)( ucBuf[8] );
		pSlot->sd_status.PERFORMANCE_MOVE = (unsigned long)( ucBuf[9] );
		pSlot->sd_status.AU_SIZE = (unsigned long)( (ucBuf[10] & 0xF0) >> 4 );
		pSlot->sd_status.ERASE_SIZE = 
			(unsigned long)( ((ucBuf[11]) << 8) | ucBuf[12] );
		pSlot->sd_status.ERASE_TIMEOUT = (unsigned long)( (ucBuf[13] & 0xFC) >> 2 );
		pSlot->sd_status.ERASE_OFFSET = (unsigned long)( (ucBuf[14] & 0x03) );
		//pSlot->sd_status.UHS_SPEED_GRADE = (ucBuf[0] & 0x00) >> 1;
		//pSlot->sd_status.UHS_AU_SIZE = (ucBuf[0] & 0x00) >> 1;
#if 0
		printf("[ ksjung ] pSlot->sd_status.DAT_BUF_WIDTH : %ld\n", 
				pSlot->sd_status.DAT_BUS_WIDTH );
		printf("[ ksjung ] pSlot->sd_status.SECURED_MODE: %ld\n", 
				pSlot->sd_status.SECURED_MODE);
		printf("[ ksjung ] pSlot->sd_status.SD_CARD_TYPE  : %ld\n", 
				pSlot->sd_status.SD_CARD_TYPE  );
		printf("[ ksjung ] pSlot->sd_status.SIZE_OF_PROTECTED_AREA : %ld\n", 
				pSlot->sd_status.SIZE_OF_PROTECTED_AREA );
		printf("[ ksjung ] pSlot->sd_status.SPEED_CLASS : %ld\n", 
				pSlot->sd_status.SPEED_CLASS );
		printf("[ ksjung ] pSlot->sd_status.PERFORMANCE_MOVE : %ld\n", 
				pSlot->sd_status.PERFORMANCE_MOVE );
		printf("[ ksjung ] pSlot->sd_status.AU_SIZE : %ld\n", 
				pSlot->sd_status.AU_SIZE );
		printf("[ ksjung ] pSlot->sd_status.ERASE_SIZE : %ld\n", 
				pSlot->sd_status.ERASE_SIZE );
		printf("[ ksjung ] pSlot->sd_status.ERASE_TIMEOUT : %ld\n", 
				pSlot->sd_status.ERASE_TIMEOUT );
		printf("[ ksjung ] pSlot->sd_status.ERASE_OFFSET : %ld\n", 
				pSlot->sd_status.ERASE_OFFSET );
		printf("[ ksjung ] ERASE_UNIT : %ld \n", 
				pSlot->sd_status.ERASE_SIZE * (1<<(pSlot->sd_status.AU_SIZE-1)) * 16 * 1024 );
#endif
		return 0;
	}

	return -1;
}
