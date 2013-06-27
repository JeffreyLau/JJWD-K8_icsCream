/****************************************************************************
 *   FileName    : fwdn_SDMMC.c
 *   Description : SDMMC F/W downloader function
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#if defined(_LINUX_) || defined(_WINCE_)
#include <platform/globals.h>
#include "IO_TCCXXX.h"
#include <string.h>
//#include "fwdn_protocol.h"
#include <fwdn/fwdn_drv_v7.h>
#include <fwdn/fwupgrade.h>

#include <fwdn/Disk.h>
#include <sdmmc/sd_memory.h>
#endif

#if defined(_LINUX_)
#include <printf.h>
#define	LPRINTF	printf
#elif defined(_WINCE_)
#define	LPRINTF	EdbgOutputDebugString
#else
#define	LPRINTF
#endif

//=============================================================================
//*
//*
//*                     [ EXTERN VARIABLE & FUNCTIONS DEFINE ]
//*
//*
//=============================================================================
#ifdef BOOTSD_INCLUDE
extern const unsigned int		CRC32_TABLE[256];
extern unsigned int				g_uiFWDN_OverWriteSNFlag;
extern unsigned int				g_uiFWDN_WriteSNFlag;

extern unsigned int				FWDN_FNT_SetSN(unsigned char* ucTempData, unsigned int uiSNOffset);
extern void						FWDN_FNT_InsertSN(unsigned char * pSerialNumber);
extern void						FWDN_FNT_VerifySN(unsigned char* ucTempData, unsigned int uiSNOffset);
extern void						IO_DBG_SerialPrintf_(char *format, ...);

int FwdnUpdateGetBootSDSerial(char* SerialNum);

#define			CRC32_SIZE	4
#define			FWDN_WRITE_BUFFER_SIZE		(32*1024)
unsigned int	tempBuffer[FWDN_WRITE_BUFFER_SIZE/4];

static int _loc_BootSD_Write_N_Verify(ioctl_diskrwpage_t *rwpage, unsigned long Size)
{
	if(DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_BOOTCODE_WRITE_PAGE, (void *) rwpage))
		return -1;
	{//Verify
		ioctl_diskrwpage_t	veri_rwpage;
		unsigned char 		*pVerifyData	= rwpage->buff + ((rwpage->rw_size) << 9);
		
		veri_rwpage.rw_size		= rwpage->rw_size;
		veri_rwpage.buff		= pVerifyData;
		veri_rwpage.start_page	= rwpage->start_page;

		if (DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_BOOTCODE_READ_PAGE, (void *) & veri_rwpage))
			return -1;
		if (memcmp(rwpage->buff, pVerifyData, Size) != 0)
			return -1;		// Verifying Error
	}
	return SUCCESS;
}

static int FwdnUpdateCodeBootSD(unsigned long ulROMFileSize, unsigned long ulConfCodeSize, unsigned char *pWriteBufAddr)
{
	ioctl_diskrwpage_t	sBootCodeWrite;
	

	sBootCodeWrite.start_page	=   (unsigned long)BOOTSD_GetROMAddr(ulROMFileSize, ulConfCodeSize + CRC32_SIZE) ;
	sBootCodeWrite.buff			= 	pWriteBufAddr;
	sBootCodeWrite.rw_size 		= 	BYTE_TO_SECTOR(ulROMFileSize);

	if(_loc_BootSD_Write_N_Verify(&sBootCodeWrite, ulROMFileSize))
	{		
		LPRINTF("[FWDN        ] Write Code Failed\n");
		return ERR_FWUG_FAIL_CODEWRITETFLASH;
	}
	LPRINTF("[FWDN        ] Write Code Completed\n");
	return SUCCESS;
	
	return SUCCESS;
}

int FwdnUpdateConfigBootSD(unsigned long ulConfigSize, unsigned char *pConfigAddr)
{
	unsigned long 		ulCRC;
	ioctl_diskrwpage_t	rwpage;
	unsigned char *pTempData = (unsigned char*)tempBuffer;
	
	memset(pTempData, 0, FWDN_WRITE_BUFFER_SIZE);
	{
		//Copy from Config code to Buffer
		memcpy(pTempData, pConfigAddr, ulConfigSize);
		
		ulCRC = FWUG_CalcCrc8((unsigned char*) pTempData, ulConfigSize);
		
		word_of(pTempData + ulConfigSize) = ulCRC;

		ulConfigSize += CRC32_SIZE;
	}


	rwpage.start_page	= (unsigned long)BOOTSD_GetConfigCodeAddr(ulConfigSize);
	rwpage.buff			= pTempData;
	rwpage.rw_size		= BYTE_TO_SECTOR(ulConfigSize);

	if(_loc_BootSD_Write_N_Verify(&rwpage, ulConfigSize))
	{
		LPRINTF("[FWDN        ] Write Config Code Failed\n");
		return ERR_FWUG_FAIL_HEADERWRITETFLASH;
	}	
	LPRINTF("[FWDN        ] Write Config Code Completed\n");
	return SUCCESS;
}

static int FwdnUpdateHeaderBootSD(unsigned long ulFirmwareBase, unsigned long ulROMFileSize, unsigned long ulConfCodeSize)
{
	//=====================================================================
	// Set SD Booting Header's Value ======================================
	//=====================================================================	
	ioctl_diskrwpage_t	rwpage;
	BOOTSD_Header_Info_T sHeaderInfo;
	unsigned char		*pTempData   = (unsigned char*)tempBuffer;
	
	memset(&sHeaderInfo, 0, sizeof(BOOTSD_Header_Info_T));
	
	sHeaderInfo.ulEntryPoint 		= ulFirmwareBase;		
	sHeaderInfo.ulBaseAddr 			= ulFirmwareBase;
	sHeaderInfo.ulConfSize			= ulConfCodeSize + CRC32_SIZE;			//CRC plus			
	sHeaderInfo.ulROMFileSize		= ulROMFileSize;
	sHeaderInfo.ulBootConfig		= (BOOTCONFIG_NORMAL_SPEED | BOOTCONFIG_4BIT_BUS);
	#if !defined(FEATURE_SDMMC_MMC43_BOOT)
	if(sHeaderInfo.ulBootConfig & BOOTCONFIG_8BIT_BUS)
	{
		LPRINTF("[FWDN        ] SD Boot can't support 8 bit data rate\n");
		return -1;
	}
	#endif
	{
		sHidden_Size_T stHidden;
		BOOTSD_Hidden_Info(&stHidden);
		sHeaderInfo.ulHiddenStartAddr = stHidden.ulHiddenStart[0] + stHidden.ulSectorSize[0];
		memcpy((void *)sHeaderInfo.ulHiddenSize, (const void*)stHidden.ulSectorSize, (BOOTSD_HIDDEN_MAX_COUNT << 2));
	}
	
	FwdnUpdateGetBootSDSerial(sHeaderInfo.ucSerialNum);

	FWDN_FNT_InsertSN(sHeaderInfo.ucSerialNum);								//Write Serial number
	memcpy(sHeaderInfo.ucHeaderFlag, "HEAD", 4);
	
	sHeaderInfo.ulCRC				= FWUG_CalcCrc8((unsigned char*) &sHeaderInfo, 512-4);
	
	//=====================================================================
	// Begin Writing Header to T-Flash ====================================
	//=====================================================================
	
	memset(pTempData, 0x00, FWDN_WRITE_BUFFER_SIZE);
	memcpy(pTempData, &sHeaderInfo, 512);
	
	rwpage.start_page	= BOOTSD_GetHeaderAddr();
	rwpage.buff			= pTempData;
	rwpage.rw_size		= 1;

	if(_loc_BootSD_Write_N_Verify(&rwpage, 512))
		return ERR_FWUG_FAIL_HEADERWRITETFLASH;
	LPRINTF("[FWDN        ] Write Header Completed!!\n");
	return SUCCESS;
}

int FwdnUpdateBootSDFirmware(unsigned int uiROMFileSize)
{
	int					res = 1;
	ioctl_diskinfo_t 	disk_info;
		
	unsigned long	*InitRoutine_Start 	=	CONFIG_CODE_START_ADDRESS;
	unsigned long	*InitRoutine_End	=	CONFIG_CODE_END_ADDRESS;
	unsigned long 	*FirmwareBase 		=	FIRMWARE_BASE_ADDRESS;
	LPRINTF("%x , %x , %x\n" , CONFIG_CODE_START_ADDRESS , CONFIG_CODE_END_ADDRESS , FIRMWARE_BASE_ADDRESS);

	unsigned long InitCode_Offset_ROM   = (*InitRoutine_Start) - (*FirmwareBase);

	unsigned long Init_Start = InitCode_Offset_ROM + ROMFILE_TEMP_BUFFER;
	
	unsigned long ulConfigSize = *InitRoutine_End - *InitRoutine_Start;

	//=====================================================================	
	// Check                   ============================================
	//=====================================================================
	if (DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_GET_DISKINFO, (void *)&disk_info))
		return -1;
	if (disk_info.sector_size != 512 || !uiROMFileSize)
		return -1;

	//=====================================================================	
	// Firware Write Start     ============================================
	//=====================================================================
#if defined(FEATURE_SDMMC_MMC43_BOOT)
	LPRINTF("[FWDN        ] Begin eMMC Bootloader write to T-Flash\n");
#else
	LPRINTF("[FWDN        ] Begin SD Bootloader write to T-Flash\n");
#endif
	
	res = FwdnUpdateHeaderBootSD(*FirmwareBase, uiROMFileSize, ulConfigSize);
	if(res != SUCCESS)
		return res;
	
	res = FwdnUpdateConfigBootSD(ulConfigSize, (unsigned char *)Init_Start);
	if(res != SUCCESS)
		return res;
	
	res = FwdnUpdateCodeBootSD(uiROMFileSize, ulConfigSize, (unsigned char *)ROMFILE_TEMP_BUFFER);
	if(res != SUCCESS)
		return res;
	
	LPRINTF("[FWDN        ] Bootloader write Complete!\n");
	return SUCCESS;
}

unsigned int FwdnUpdateReadBootSDFirmware(unsigned int master)
{
	return 0;
}

int FwdnUpdateGetBootSDSerial(char* SerialNum)
{
	ioctl_diskinfo_t 	disk_info;
	ioctl_diskrwpage_t	bootCodeRead;
	unsigned char		readData[512];
	int				res;

	/*----------------------------------------------------------------
					G E T	S E R I A L	N U M B E R
	----------------------------------------------------------------*/
	if (DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_GET_DISKINFO, (void *)&disk_info) < 0)
		return -1;
	
	if (disk_info.sector_size != 512)
		return -1;

	bootCodeRead.start_page	= BOOTSD_GetHeaderAddr();
	bootCodeRead.rw_size	= 1;
	bootCodeRead.buff		= readData;
	res = DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_BOOTCODE_READ_PAGE, (void *) &bootCodeRead);
	if ( res != 0) return res;

	/*----------------------------------------------------------------
					V E R I F Y	S E R I A L	N U M B E R
	----------------------------------------------------------------*/

	memcpy(SerialNum ,((BOOTSD_Header_Info_T*)readData)->ucSerialNum, 64);

	FWDN_FNT_VerifySN(readData, 32);
	switch (FWDN_DeviceInformation.DevSerialNumberType)
	{
		case SN_VALID_16:	return 16;
		case SN_VALID_32:	return 32;
		case SN_INVALID_16:	return -16;
		case SN_INVALID_32:	return -32;
		default:	return -1;
	}
}

int FwdnUpdateSetBootSDSerial(unsigned char *ucData, unsigned int overwrite)
{
	ioctl_diskinfo_t 	disk_info;
	unsigned char		ucTempData[512];
	int				iRev;
	ioctl_diskrwpage_t	header;

	if (DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_GET_DISKINFO, (void *)&disk_info) < 0)
	{
		return -1;
	}
	if (disk_info.sector_size != 512)
		return -1;

	memcpy( FWDN_DeviceInformation.DevSerialNumber, ucData , 32);
	g_uiFWDN_OverWriteSNFlag	=  overwrite;
	g_uiFWDN_WriteSNFlag		= 0;

	if ( overwrite == 0 )
	{
		/*----------------------------------------------------------------
						G E T	S E R I A L	N U M B E R
		----------------------------------------------------------------*/
		header.start_page	= BOOTSD_GetHeaderAddr();
		header.rw_size		= 1;
		header.buff			= ucTempData;
		iRev	=	DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_BOOTCODE_READ_PAGE, (void *)&header);
		if(iRev != SUCCESS)
			return (-1);

		iRev = FWDN_FNT_SetSN(ucTempData, 32);
	}
	return 0;
}

#endif



