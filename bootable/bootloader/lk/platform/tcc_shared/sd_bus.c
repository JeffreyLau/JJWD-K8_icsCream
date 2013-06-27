/****************************************************************************
 *   FileName    : mmc_ext.c
 *   Description :
 ****************************************************************************
*
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
*
 ****************************************************************************/
#include <sdmmc/sd_slot.h>
#include <sdmmc/sd_bus.h>
#include <debug.h>

const unsigned char SDMMC_Library_Version[] = {"SIGBYAHONG_SD_DRV_V0000"};

/************************************************************************
*	Constant DEFINE
************************************************************************/
#define	BLOCKLEN	512

/************************************************************************
*	Type DEFINE
************************************************************************/
#ifndef ON
	#define	ON		0x01
#endif

#ifndef OFF
	#define	OFF		0x00
#endif

/************************************************************************
*	Externs
************************************************************************/
extern void IO_DBG_Printf_(char *format, ...);
extern void IO_DBG_SerialPrintf_(char *format, ...);
extern void SD_SLOT_AssertFunc(unsigned uNum, char *Name, unsigned uLine, ...);

/************************************************************************
*	MACRO DEFINE
************************************************************************/
// Enable below definition for debug by serial.
#define SD_Debug	(1) ? (void) 0 : EdbgOutputDebugString
#define SD_DebugL1	(1) ? (void) 0 : EdbgOutputDebugString

//#define	MMC_ASSERT()	SD_SLOT_AssertFunc(0, __FUNCTION__, __LINE__)
//#define	MMC_ASSERT1(X)	SD_SLOT_AssertFunc(1, __FUNCTION__, __LINE__, (X))

#define	IsEVEN(X)		(((X) & 1) == 0)
#define	IsODD(X)		(((X) & 1) == 1)
#define	GetEVEN(X)		((X) & ~1)

/************************************************************************
*	VARIABLE DEFINE
************************************************************************/

//#define DEBUG_EMMC

#if defined(DEBUG_EMMC)
#define TRACE	printf
#else
#define TRACE	
#endif


SD_SLOT_T sSD_BUS_Slot[SD_SLOT_TOTAL_COUNT];

unsigned char *SDMMC_Get_Library_Version(void)
{
	return (unsigned char*)SDMMC_Library_Version;
}

static int SD_BUS_SlotIndex_Validate(int iSlotIndex)
{
	if(0<=iSlotIndex && iSlotIndex<SD_SLOT_GetTotalSlotCount() && iSlotIndex<SD_SLOT_TOTAL_COUNT)
		return 1;
	else
		return 0;
}

static void SD_BUS_InitRecord(PSD_SLOT_T pSlot)
{
	int iIndex = pSlot->iIndex;

	memset(pSlot, 0, sizeof(SD_SLOT_T));
	pSlot->iIndex = iIndex;

	SD_SLOT_InitRecord(pSlot);
}

void SD_BUS_Initialize(void)
{
	int i;

	SD_SLOT_Initialize();

	for ( i=0 ; i<SD_SLOT_TOTAL_COUNT && i<SD_SLOT_GetTotalSlotCount() ; i++ )
	{

		PSD_SLOT_T pSlot = &sSD_BUS_Slot[i];
		pSlot->iIndex = i;
		SD_BUS_InitRecord(pSlot);
		SD_SLOT_Start(pSlot);
		/*
			for every detach
		*/
		SD_SLOT_Detach(pSlot);

	}

}

int SD_BUS_IsWriteProtected(int iSlotIndex)
{
	PSD_SLOT_T pSlot;
	if(SD_BUS_SlotIndex_Validate(iSlotIndex)==0)
		return 0;
	pSlot = &sSD_BUS_Slot[iSlotIndex];

	if(SD_SLOT_IsCardDetected(pSlot))
	{
		// Check WP Status
		return SD_SLOT_IsWriteProtected(pSlot);
	}

	return 0;
}

/**************************************************************************
*  FUNCTION NAME :
*
*      unsigned long SD_BUS_Scan(sMMCrecord *pSlot, int mmcplus_buswidth);
*
*  DESCRIPTION : Check and Mount SD/MMC.
*
*  INPUT:
*			drv_num	= 0 for Harp, 1 for SD/MMC
*			mmcplus_buswidth	= Bus width Selection
*
*  OUTPUT:	long - Return Type
*  			= sector count : Successful
*			= 0 : Card didn't exist.
*
**************************************************************************/
static unsigned long SD_BUS_Scan(PSD_SLOT_T pSlot)
{
	int		iResBuf[4];
	int		iRCABuf[3];
	//unsigned	buf[2];
	char		cPNMBuf[5];

	if (SD_SLOT_MMC_ResetCMD(pSlot, iResBuf, cPNMBuf, iRCABuf) < 0)
	{
		if (SD_SLOT_SD_ResetCMD(pSlot, iResBuf, cPNMBuf, iRCABuf) < 0)
			return 0;
		else
			pSlot->id = MMC_CARD_SD;
	}
	else
	{
		pSlot->id = MMC_CARD_MMC;
	}

	SDMMC_ms_delay(1);

	// Get Card Specific Data
	SD_SLOT_GetCSD(pSlot, pSlot->rca, &pSlot->stCSD);

	pSlot->stCSD.RealTAAC	= SD_SLOT_CalcTAAC(&pSlot->stCSD);
	pSlot->stCSD.TranFreq	= SD_SLOT_CalcTRAN(&pSlot->stCSD);

	// Check if Card is MMC+ type.
	if (pSlot->id == MMC_CARD_MMC)
	{
		if (pSlot->stCSD.SPEC_VERS >= 4)
			pSlot->id	= MMC_CARD_MMCPLUS;
	}

	// Send Card to Transfer Mode.
	SD_SLOT_GoTransfer(pSlot, pSlot->rca);
	SDMMC_ms_delay(1);

	if (pSlot->id == MMC_CARD_MMCPLUS)
	{
		// A certain mmc4.x card needs this before data transfer.
		SD_SLOT_MMC_ReadEXTCSD(pSlot, (unsigned char*)&pSlot->stEXTCSD);
		SD_SLOT_MMC_ReadEXTCSD(pSlot, (unsigned char*)&pSlot->ext_csd_441);	// by B120040-ksjung
		if(pSlot->stEXTCSD.EXTCSDRev>=3)
			pSlot->ulBootPartitionSize = ((unsigned long)pSlot->stEXTCSD.BootSizeMult/*unit=128KB*/) << 8;
	}
	else if (pSlot->id == MMC_CARD_SD)
	{
		SD_SLOT_GetSCR(pSlot, pSlot->rca, &pSlot->stSCR);
		SD_SLOT_Read_SDStatus(pSlot, pSlot->rca);
	}

	pSlot->size	 = SD_SLOT_GetSize(pSlot);
	pSlot->sect_per_block = 128;

	pSlot->ulClockRateInKHz = pSlot->stCSD.TranFreq;
	if(SD_SLOT_SendBWCommand(pSlot, SD_SLOT_GetMaxBusWidth(pSlot)) == 0)
	{
		pSlot->bw = SD_SLOT_GetMaxBusWidth(pSlot);
	}

	SD_SLOT_SetInterface(pSlot);
	SDMMC_ms_delay(1);

	SD_SLOT_SendCommand(pSlot, RspType1, CMD16, BLOCKLEN);		// Block length in bytes

#if 0
	TRACE("[ ksjung ] SIZE : %d\n", pSlot->size);
	TRACE("[ ksjung ] sect_per_block : %d\n", pSlot->sect_per_block);
	TRACE("[ ksjung ] usBlockSize : %d\n", pSlot->usBlockSize );
	TRACE("[ ksjung ] usBlockCount : %d\n", pSlot->usBlockCount );
	TRACE("[ ksjung ] pSlot->stCSD.ERASE_BL_EN : %d\n", pSlot->stCSD.ERASE_BL_EN);
	TRACE("[ ksjung ] pSlot->stCSD.SECTOR_SIZE : %d\n", pSlot->stCSD.SECTOR_SIZE);
	TRACE("[ ksjung ] pSlot->stCSD.CSDS: %d\n", pSlot->stCSD.CSDS);
	TRACE("[ ksjung ] pSlot->stCSD.SPEC_VERS: %d\n", pSlot->stCSD.SPEC_VERS);
	TRACE("[ ksjung ] pSlot->stEXTCSD.EraseGroupDef : %X\n", (unsigned)pSlot->stEXTCSD.EraseGroupDef);

	TRACE("[ ksjung ] pSlot->EXTCSD_441.ERASE_GROUP_DEF : %X\n", 
			(unsigned)pSlot->ext_csd_441.ERASE_GROUP_DEF);
	TRACE("[ ksjung ] pSlot->EXTCSD_441.CSD_STRUCTURE : %ld\n", 
			(unsigned long) pSlot->ext_csd_441.CSD_STRUCTURE ); 
	TRACE("[ ksjung ] pSlot->EXTCSD_441.SEC_COUNT : %X\n",
			(unsigned long) pSlot->ext_csd_441.SEC_COUNT);
	TRACE("[ ksjung ] pSlot->EXTCSD_441.EXT_CSD_REV : %ld\n",
			(unsigned long) pSlot->ext_csd_441.EXT_CSD_REV);
	TRACE("[ ksjung ] pSlot->EXTCSD_441.BUS_WIDTH : %ld\n",
			(unsigned long) pSlot->ext_csd_441.BUS_WIDTH);
	TRACE("[ ksjung ] pSlot->EXTCSD_441.S_CMD_SET : %ld\n",
			(unsigned long) pSlot->ext_csd_441.S_CMD_SET);
	TRACE("[ ksjung ] pSlot->EXTCSD_441.CARD_TYPE : %ld\n",
			(unsigned long) pSlot->ext_csd_441.CARD_TYPE);


	TRACE("[ ksjung ] pSlot->stEXTCSD.SecCount : %X\n",
			(unsigned long) pSlot->stEXTCSD.SecCount);

	unsigned long erase_unit = (((unsigned long)pSlot->csd_441.ERASE_GRP_SIZE) + 1) * 
		(((unsigned long)pSlot->csd_441.ERASE_GRP_MULT) + 1); 
	TRACE("[ ksjung ] Erase Unit Size is %ld\n", erase_unit);
	/*
	TRACE("[ ksjung ] Erase Count == pSlot->EXTCSD_441.SEC_COUNT * 512 / erase_unit : %ld\n",
			(unsigned long)pSlot->EXTCSD_441.SEC_COUNT * 512 / erase_unit );
	*/
#endif
	return pSlot->size;
}

int SD_BUS_Probe(int iSlotIndex)
{
	PSD_SLOT_T pSlot;
	if(SD_BUS_SlotIndex_Validate(iSlotIndex)==0)
		return 0;
	pSlot = &sSD_BUS_Slot[iSlotIndex];
	if(SD_SLOT_IsCardDetected(pSlot))
	{
		SD_BUS_InitRecord(pSlot);

		SD_SLOT_Detach(pSlot);

		SD_SLOT_Attach(pSlot);

		SD_BUS_Scan(pSlot);

		return 1;
	}
	else
	{
		return 0;
	}
}

int SD_BUS_IsCardDetected(int iSlotIndex)
{
	PSD_SLOT_T pSlot;
	if(SD_BUS_SlotIndex_Validate(iSlotIndex)==0)
		return 0;
	pSlot = &sSD_BUS_Slot[iSlotIndex];

	return SD_SLOT_IsCardDetected(pSlot);
}

int SD_BUS_IsMemoryCard(int iSlotIndex)
{
	PSD_SLOT_T pSlot;
	if(SD_BUS_SlotIndex_Validate(iSlotIndex)==0)
		return 0;
	pSlot = &sSD_BUS_Slot[iSlotIndex];

	if(SD_SLOT_IsCardDetected(pSlot))
	{
		if(pSlot->id == MMC_CARD_MMC || pSlot->id == MMC_CARD_SD || pSlot->id == MMC_CARD_MMCPLUS)
			return 1;
	}

	return 0;
}

unsigned long SD_BUS_GetCardCapacity(int iSlotIndex)
{
	PSD_SLOT_T pSlot;
	if(SD_BUS_SlotIndex_Validate(iSlotIndex)==0)
		return 0;
	pSlot = &sSD_BUS_Slot[iSlotIndex];

	return pSlot->size;
}

int SD_BUS_GetBootSlotIndex(void)
{
	return SD_SLOT_GetBootSlotIndex();
}

int SD_BUS_GetUpdateSlotIndex(void)
{
	return SD_SLOT_GetUpdateSlotIndex();
}

/**********************************************************
*
*	Function of
*
*
*
*	Input	:
*	Output	:
*	Return	:
*
*	Description	:
**********************************************************/
int SD_BUS_MEM_Request(int iSlotIndex, int rwFlag, unsigned long LBA_addr, unsigned long nSector, void *buff)
{
	PSD_SLOT_T pSlot;
	int iCmd;
	int iError = -1;

	if(SD_BUS_SlotIndex_Validate(iSlotIndex)==0)
		return -1;

	pSlot = &sSD_BUS_Slot[iSlotIndex];

	if (SD_SLOT_IsCardDetected(pSlot) == 0)
		return MMC_ERR_NODETECT;

	if(nSector == 0 || buff == (void*)0)
		return -1;

	if (ISZERO(pSlot->card_prop, SDMMC_PROP_HCS))
		LBA_addr = LBA_addr * BLOCKLEN;

	if(rwFlag == SD_BUS_MEM_READ)
		iCmd = (nSector>1)? CMD18 : CMD17;
	else if(rwFlag == SD_BUS_MEM_WRITE)
		iCmd = (nSector>1)? CMD25 : CMD24;
	else
		return -1;

	while(nSector>0)
	{
		unsigned short usPartialSector = (nSector>=0x10000)? 0xFFFF : (unsigned short)nSector;
		nSector -= usPartialSector;

		iError = -1;
		if(SD_SLOT_RW_Start(pSlot,RspType1,iCmd,LBA_addr,rwFlag,usPartialSector,BLOCKLEN)==0)
		{
			if(SD_SLOT_RW_Buffer(pSlot, rwFlag, buff, usPartialSector) == usPartialSector)
				iError = 0;
		}

		SD_SLOT_RW_Complete(pSlot);

		if(iError)
			break;
	}

	return iError;
}

void SD_BUS_StopTransfer(int iSlotIndex)
{
	PSD_SLOT_T pSlot;

	if(SD_BUS_SlotIndex_Validate(iSlotIndex)==0)
		return;

	pSlot = &sSD_BUS_Slot[iSlotIndex];

	SD_SLOT_SendCommand(pSlot, RspType1b, CMD12, 0x00000000);
}

sEXTCSDREG * SD_BUS_GetEXTCSD(int iSlotIndex)
{
	if(SD_BUS_SlotIndex_Validate(iSlotIndex)==0)
		return (sEXTCSDREG *)0;

	return &sSD_BUS_Slot[iSlotIndex].stEXTCSD;
}

int SD_BUS_switchEXTCSD(int iSlotIndex, unsigned short usIndex, unsigned char ucValue, unsigned char ucCmdSet)
{
	if(SD_BUS_SlotIndex_Validate(iSlotIndex)==0)
		return -1;

	return SD_SLOT_switchEXTCSD(&sSD_BUS_Slot[iSlotIndex], usIndex, ucValue, ucCmdSet);
}

/* End of File */

#define EMMC_BOOT_CARD_STATUS(x)	((x>>9) & 0x0F)
#define EMMC_BOOT_TRAN_STATE		4
#define EMMC_BOOT_PROG_STATE		7

static unsigned int emmc_get_card_status(PSD_SLOT_T pSlot, unsigned int *status)
{

	int result = 0;

	result = SD_SLOT_SendCommand(pSlot , RspType1, CMD13, pSlot->rca);
	return result;
}

static int check_response(PSD_SLOT_T pSlot)
{
	return ( pSlot->ulResponse[0] & 0xFFFFE000 );
}

static int do_emmc_erase(PSD_SLOT_T pSlot, unsigned long start_addr, unsigned long erase_size,
		    int low_format)
{
	TRACE("[eMMC erase] start_addr : %ld\n", start_addr);
	TRACE("[eMMC erase] start_addr + erase_size : %ld\n", start_addr+erase_size);

	int result = 0, cmd = 0, arg = 0, i = 0;
	unsigned int status = 0;

	cmd = (pSlot->id == MMC_CARD_SD) ? CMD32 : CMD35;
	result = SD_SLOT_SendCommand(pSlot, RspType1, cmd, start_addr);
	TRACE("pSlot->ulResponse[0] : %X\n", pSlot->ulResponse[0]);
	if( result != 0 || check_response(pSlot) ) {
		TRACE("[eMMC erase] Error CMD35\n");
		result = -1;
	}

	cmd = (pSlot->id == MMC_CARD_SD) ? CMD33 : CMD36;
	result = SD_SLOT_SendCommand(pSlot, RspType1, cmd, start_addr + erase_size - 1);
	TRACE("pSlot->ulResponse[0] : %08X\n", pSlot->ulResponse[0]);
	if( result != 0 || check_response(pSlot) ) {
		TRACE("[eMMC erase] Error CMD36\n");
		result = -1;
	}

	printf("Waiting erase complete response..");
	do{
		if( (i++ % 50000) == 0 )
			printf(".");
		result = emmc_get_card_status(pSlot, status);
		TRACE("CARD_STATUS : %08X\n", EMMC_BOOT_CARD_STATUS(pSlot->ulResponse[0]));
		if(EMMC_BOOT_CARD_STATUS(pSlot->ulResponse[0]) == EMMC_BOOT_TRAN_STATE)
			break;
	}while((result == 0) && (EMMC_BOOT_CARD_STATUS(pSlot->ulResponse[0]) == EMMC_BOOT_PROG_STATE));


	if( (pSlot->id != MMC_CARD_SD) && !low_format )
		arg = 0x00000001;
	result = SD_SLOT_SendCommand(pSlot, RspType1b, CMD38, arg);
	TRACE("pSlot->ulResponse[0] : %08X\n", pSlot->ulResponse[0]);

	if( result != 0) {
		TRACE("[eMMC erase] Error CMD38\n");
		result = -1;
	}

	do{
		if( (i++ % 50000) == 0 )
			printf(".");
		result = emmc_get_card_status(pSlot, status);
		TRACE("CARD_STATUS : %08X\n", EMMC_BOOT_CARD_STATUS(pSlot->ulResponse[0]));
		if(EMMC_BOOT_CARD_STATUS(pSlot->ulResponse[0]) == EMMC_BOOT_TRAN_STATE)
			break;
	}while((result == 0) && (EMMC_BOOT_CARD_STATUS(pSlot->ulResponse[0]) == EMMC_BOOT_PROG_STATE));
	printf("\n");

	if( result != 0  || check_response(pSlot) ) {
		TRACE("[eMMC erase] Error Response[0] : %d\n", pSlot->ulResponse[0]);
		TRACE("[eMMC erase] Error CMD13\n");
		result = -1;
	}

	return result;
}

static int do_emmc_erase_write(unsigned long start_addr, unsigned long erase_size,
		    unsigned int val)
{
	unsigned char *buf = NULL;
	unsigned long erase_unit = 20480;
	int i = 0;

	printf("erasing partition..");
	do
	{
		if( (i++ % 15) == 0 )
			printf(".");
		if( erase_size < erase_unit )
			erase_unit = erase_size;
		buf = calloc( 0xFF, erase_unit*sizeof(char) );
		if ( emmc_write((unsigned long long) start_addr, erase_unit, buf) != 0 )
			return -1;
		free( buf );
		erase_size -= erase_unit;
		start_addr += erase_unit;
	} while( erase_size > 0 );
	printf("\n");
	TRACE(" final addr : %ld\n", start_addr);
	return 0;
}

static unsigned long get_sdcard_erase_unit(PSD_SLOT_T pSlot)
{
	unsigned long sector_size = 1;
	if( ISZERO(pSlot->card_prop, SDMMC_PROP_HCS) ||
			pSlot->sd_status.AU_SIZE <= 0 ||
			(pSlot->sd_status.ERASE_SIZE & 0x0000FFFF) == 0 )
		return 512;

	sector_size = 512;

	TRACE("[ ksjung ] pSlot->sd_status.ERASE_SIZE : %ld\n", pSlot->sd_status.ERASE_SIZE);
	TRACE("[ ksjung ] pSlot->sd_status.AU_SIZE : %d\n", pSlot->sd_status.AU_SIZE);
	TRACE("[ ksjung ] pSlot->stCSD.ERASE_BL_EN : %d\n", pSlot->stCSD.ERASE_BL_EN);

	return pSlot->sd_status.ERASE_SIZE  * 16 * 1024 / sector_size
		* ( 1 << (pSlot->sd_status.AU_SIZE-1) );
}

static unsigned long get_emmc_erase_unit(PSD_SLOT_T pSlot)
{
	if( pSlot->ext_csd_441.ERASE_GROUP_DEF )
		return ( 1024 * (unsigned long)pSlot->ext_csd_441.HC_ERASE_GRP_SIZE );
	return ((unsigned long)pSlot->csd_441.ERASE_GRP_SIZE + 1) *
		((unsigned long)pSlot->csd_441.ERASE_GRP_MULT + 1);
}

static inline unsigned long get_sdcard_density(PSD_SLOT_T pSlot)
{
	int sector_size = 1;
	if( ISZERO(pSlot->card_prop, SDMMC_PROP_HCS) )
		sector_size = 512;
	return ( (pSlot->stCSD.C_SIZE + 1) * 1024 * sector_size );
}

static unsigned long get_emmc_density(PSD_SLOT_T pSlot, unsigned long erase_unit)
{
	unsigned long BLOCKNR, MULT, BLOCK_LEN;

	if( pSlot->ext_csd_441.SEC_COUNT && pSlot->ext_csd_441.SEC_COUNT>0x00400000 )
		return ( (unsigned long)pSlot->ext_csd_441.SEC_COUNT );

	MULT = ( 2 << ((unsigned long)pSlot->csd_441.C_SIZE_MULT + 2 - 1) );
	BLOCKNR = ( ((unsigned long)pSlot->csd_441.C_SIZE + 1) * MULT );
	BLOCK_LEN = ( 2 << (pSlot->csd_441.READ_BL_LEN - 1) );
	TRACE(" MULT : %ld\n", MULT);
	TRACE(" BLOCKNR : %ld\n", BLOCKNR);
	TRACE(" BLOCK_LEN : %ld\n", BLOCK_LEN);
	TRACE(" C_SIZE_MULT : %ld\n", (unsigned long)pSlot->csd_441.C_SIZE_MULT);
	TRACE(" 2 ^ C_SIZE_MULT : %ld\n",  2 << ((unsigned long)pSlot->csd_441.C_SIZE_MULT + 2 - 1));
	TRACE(" C_SIZE : %ld\n", (unsigned long)pSlot->csd_441.C_SIZE);
	TRACE(" READ_BL_LEN : %ld\n", (unsigned long)pSlot->csd_441.READ_BL_LEN);

	return BLOCKNR * BLOCK_LEN;
}

static inline int below_2GB(PSD_SLOT_T pSlot)
{
	return ( (pSlot->ext_csd_441.SEC_COUNT != 0) ||
			(pSlot->ext_csd_441.SEC_COUNT <= 0x00400000) ) ? 1 : 0;
}


int erase_emmc(unsigned long start_addr, unsigned long erase_size, int low_format)
{
	int iSlotIndex = 0;
	unsigned long erase_unit = 0;
	PSD_SLOT_T pSlot;
	unsigned int val = 0;

	iSlotIndex = SD_BUS_GetBootSlotIndex();

	if( SD_BUS_SlotIndex_Validate(iSlotIndex) == 0 ) {
		TRACE("[eMMC erase] SD_BUS_SlotIndex_Validate error.\n");
		return -1;
	}

	pSlot = &sSD_BUS_Slot[iSlotIndex];

	if( SD_SLOT_IsCardDetected(pSlot) == 0 ) {
		TRACE("[eMMC erase] SD_SLOT_IsCardDetected error.\n");
		return MMC_ERR_NODETECT;
	}

	erase_unit = (pSlot->id == MMC_CARD_SD) ?
		get_sdcard_erase_unit(pSlot) : get_emmc_erase_unit(pSlot);
	TRACE("[eMMC erase] erase_unit : %ld\n", erase_unit);
	if( low_format )
		erase_size =  (pSlot->id == MMC_CARD_SD) ?
			get_sdcard_density(pSlot) : get_emmc_density(pSlot, erase_unit);
	else {
		if( pSlot->id == MMC_CARD_SD &&
				ISZERO(pSlot->card_prop, SDMMC_PROP_HCS) )
			start_addr *= 512;
	}

	TRACE("[eMMC erase] erase_size : %ld\n", erase_size);
	if( pSlot->id == MMC_CARD_SD )
		val = pSlot->stSCR.data_stat_after_erase;

	if( (pSlot->id != MMC_CARD_SD && erase_size < erase_unit) ||
			((pSlot->id != MMC_CARD_SD) && below_2GB(pSlot) && !low_format) )
		return do_emmc_erase_write(start_addr, erase_size, val);
	return do_emmc_erase(pSlot, start_addr, erase_size, low_format);
}

