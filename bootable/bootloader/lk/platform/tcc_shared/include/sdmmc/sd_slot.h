/****************************************************************************
 *   FileName    : sd_slot.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#ifndef _SD_SLOT_H
#define _SD_SLOT_H

#include "sd_regs.h"

enum __MMC_CardType
{
	MMC_CARD_UNKNOWN = 0,
	MMC_CARD_MMC,
	MMC_CARD_SD,
	MMC_CARD_MMCPLUS,
	MMC_CARD_COUNT
};

enum __MMC_CardVersion
{
	MMC_CARD_Ver1xx = 1,
	MMC_CARD_Ver200
};

#define MMC_NO_ERROR		0
#define MMC_ERR_TIMEOUT		-1
#define MMC_ERR_CRCRSP		-2
#define MMC_ERR_NODETECT	-3
#define MMC_ERR_WRPROTECT	-4
#define MMC_ERR_NOTRAN		-5

//#define MMC_STATUS_IDLE		0x00000000
//#define MMC_STATUS_READY	0x00000200
//#define MMC_STATUS_STANDBY	0x00000600
//#define MMC_STATUS_TRAN		0x00000800
//#define MMC_STATUS_DATA		0x00000A00

#define SDMMC_COMMAND		0
#define SDMMC_READ			1
#define SDMMC_WRITE			2

//typedef struct mmc_hidden_t
//{
//	unsigned int	start;		// In Sector Address
//	unsigned int	size;		// Sector Count
//#if defined(FEATURE_SDMMC_MMC43_BOOT)
//	unsigned char	area;
//#endif
//} sMMC_hidden;

typedef struct mmc_filesys_t
{
	unsigned long		start;		// In Sector Address
	unsigned long		size;
	unsigned short		head;
	unsigned short		sect;
	unsigned long		cyl;
} sMMC_filesys;

#define	SDMMC_AREA_BOOTROM				(1 << 0)
#define	SDMMC_AREA_HIDDEN				(1 << 1)
#define	SDMMC_AREA_HIDDEN_FS			(1 << 2)
#define	SDMMC_AREA_REINIT				(1 << 7)

#define	SDMMC_PROP_VER20				(1 << 0)
#define	SDMMC_PROP_INTERNAL				(1 << 1)
#define	SDMMC_PROP_MUTATION_CARD		(1 << 2)
#define	SDMMC_PROP_HS					(1 << 3)
#define	SDMMC_PROP_HCS					(1 << 4)

#define	SDMMC_CTRL_CHKTRAN_MASK		(0xF)
#define	SDMMC_CTRL_AUTOSTOP			(1 << 4)
#define	SDMMC_CTRL_DMA				(1 << 5)
#define	SDMMC_CTRL_BY_MSC			(1 << 6)

typedef struct 
{
	unsigned char		id;
	unsigned char		card_prop;			// SDMMC_PROP_VER20, SDMMC_PROP_INTERNAL, SDMMC_PROP_MUTATION_CARD
	unsigned char		area_en;			// SDMMC_AREA_BOOTROM, SDMMC_AREA_HIDDEN, SDMMC_AREA_HIDDEN_FS
	unsigned char		ctrl_prop;			// SDMMC_CTRL_CHK_TRAN, SDMMC_CTRL_AUTOSTOP, SDMMC_CTRL_DMA, SDMMC_CTRL_BY_MSC
	unsigned char		bw;					// bus-width (1, 4, 8)
	unsigned char		longtimeout;
	unsigned long		size;				// sector count
	unsigned long		ulBootPartitionSize;
	unsigned long		ulClockRateInKHz;
	unsigned long		ulCmd;
	unsigned long		ulRspType;
	unsigned long		ulResponse[4];
	unsigned short		sect_per_block;
	unsigned short		usSdClock;
	int					iIndex;
	unsigned long		rca;
	unsigned long		buf_addr;
	unsigned long		dma_addr;
	unsigned short		remain_sectors;

	sCIDREG				stCID;
	sCSDREG				stCSD;
	sEXTCSDREG			stEXTCSD;
	sSCR_Register		stSCR;

	struct csd_441 csd_441;
	struct ext_csd_441 ext_csd_441;
	struct sd_status sd_status;
	//sMMC_filesys		filesys;
	//sMMC_filesys		filesys_hd;

	//sMMC_hidden			hidden_p0;
	//sMMC_hidden			hidden_p1;
	//sMMC_hidden			hidden_p2;
	//sMMC_hidden			hidden_p3;
	//sMMC_hidden			bootcode;

	PSDSLOTREG_T		pHwSdSlotReg;

	unsigned short		usBlockSize;
	unsigned short		usBlockCount;
	unsigned char		ucCommandType;
} SD_SLOT_T, *PSD_SLOT_T;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void SDMMC_us_delay(unsigned long ul_ms);
extern void SDMMC_ms_delay(unsigned long ul_ms);
extern void SDMMC_memcpy(void *pvTgt, const void *pvSrc, unsigned int uiLen);
extern void *memset(void *, int , unsigned int);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int SD_SLOT_IsCardDetected(PSD_SLOT_T pSlot);
int SD_SLOT_IsWriteProtected(PSD_SLOT_T pSlot);
int SD_SLOT_GetBootSlotIndex(void);
int SD_SLOT_GetUpdateSlotIndex(void);
int SD_SLOT_GetMaxBusWidth(PSD_SLOT_T pSlot);
void SD_SLOT_InitRecord(PSD_SLOT_T pSlot);
void SD_SLOT_Initialize(void);
int SD_SLOT_SendCommand(PSD_SLOT_T pSlot, int iRspType, int iIndex, int iArgument);
int SD_SLOT_RW_Start(PSD_SLOT_T pSlot, int iRspType, int iIndex, int iArgument,int rwFlag, unsigned short usBlockCount, unsigned short usBlockSize);
unsigned short SD_SLOT_RW_Buffer(PSD_SLOT_T pSlot, int rwFlag,void *pBuf, unsigned short usBlockCount);
void SD_SLOT_RW_Complete(PSD_SLOT_T pSlot);
int SD_SLOT_SD_ResetCMD(PSD_SLOT_T pSlot, int *iResBuf, char *cPNMBuf, int * iRCABuf);
int SD_SLOT_MMC_ResetCMD(PSD_SLOT_T pSlot, int *iResBuf, char *cPNMBuf, int * iRCABuf);
int SD_SLOT_GetSCR(PSD_SLOT_T pSlot, int iRCA,sSCR_Register *pSCR);
void SD_SLOT_GetCSD(PSD_SLOT_T pSlot, int iRCA, sCSDREG *pCSDREG);
int SD_SLOT_SendBWCommand(PSD_SLOT_T pSlot, int mmcplus_buswidth);
unsigned SD_SLOT_CalcTAAC(sCSDREG *pstCSD);
unsigned SD_SLOT_CalcTRAN(sCSDREG *pstCSD);
void SD_SLOT_GoTransfer(PSD_SLOT_T pSlot, int iRCA);
int SD_SLOT_MMC_ReadEXTCSD(PSD_SLOT_T pSlot, unsigned char *ucBuf);
unsigned long SD_SLOT_GetSize(PSD_SLOT_T pSlot);
int SD_SLOT_switchEXTCSD(PSD_SLOT_T pSlot, unsigned short index, unsigned char value, unsigned char cmdSet);
void SD_SLOT_SetInterface(PSD_SLOT_T pSlot);
int SD_SLOT_SendHighSpeedCommand(PSD_SLOT_T pSlot);
void SD_SLOT_Start(PSD_SLOT_T pSlot);
void SD_SLOT_Attach(PSD_SLOT_T pSlot);
void SD_SLOT_Detach(PSD_SLOT_T pSlot);
int SD_SLOT_GetTotalSlotCount(void);

#endif //_SD_SLOT_H
