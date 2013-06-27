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

#if defined (NKUSE)
	#include "windows.h"
	#include "stdlib.h"
#else
	#include "TC_File.h"
#endif
#endif

#if defined(_WINCE_)
#include "bsp.h"
#endif

#include <fwdn/Disk.h>
#include <sdmmc/sd_memory.h>

#include <debug.h>

#if defined(_LINUX_)
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
#endif

extern void *memcpy(void *, const void *, unsigned int);
extern void *memset(void *, int, unsigned int);

#define			CRC32_SIZE	4
#define			FWDN_WRITE_BUFFER_SIZE		(32*1024)
unsigned int	tempBuffer[FWDN_WRITE_BUFFER_SIZE/4];

#define FWDN_PROGRESS_RATE(A, B) 	((A *100) / B)

static int FwdnWriteCodeBootSD(unsigned int uiROMFileSize, unsigned int uiConfCodeSize, unsigned char *pBufAddr, unsigned int uiBufSize)
{
#ifdef BOOTSD_INCLUDE
	int		res;
	ioctl_diskrwpage_t	sBootCodeWrite;
	unsigned int		readRomIndex		= 0;
	unsigned int		uiSrcAddr 			= 0;
	unsigned int		uiROMFileSizeSaved 	= uiROMFileSize;

	memset(pBufAddr, 0x00, uiBufSize);

	uiBufSize = uiBufSize >> 1;
	uiBufSize = uiBufSize & 0xFFFFFE00;	// (iBufSize >> 9) << 9

	sBootCodeWrite.start_page	=   BOOTSD_GetROMAddr(uiROMFileSize, uiConfCodeSize + CRC32_SIZE) ;
	sBootCodeWrite.buff			= 	pBufAddr;
	LPRINTF("Write Code Begins\n");

	while(uiROMFileSize)
	{
		unsigned int uiPartialSize = (uiROMFileSize>uiBufSize)? uiBufSize : uiROMFileSize;
		uiROMFileSize -= uiPartialSize;

		sBootCodeWrite.rw_size 	= BYTE_TO_SECTOR(uiPartialSize);

		//read from USB
		FWDN_DRV_FirmwareWrite_Read(uiSrcAddr, pBufAddr, uiPartialSize ,FWDN_PROGRESS_RATE(readRomIndex,uiROMFileSizeSaved));

		if(DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_BOOTCODE_WRITE_PAGE, (void *) & sBootCodeWrite) < 0)
		//	return ERR_FWUG_FAIL_CODEWRITETFLASH;

		#if 1
		// Verify Data
		{
			ioctl_diskrwpage_t	bootCodeRead;
			unsigned char 		*pVerifyData	= pBufAddr + uiBufSize;

			bootCodeRead.rw_size	= sBootCodeWrite.rw_size;
			bootCodeRead.buff		= pVerifyData;
			bootCodeRead.start_page	= sBootCodeWrite.start_page;

			res = DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_BOOTCODE_READ_PAGE, (void *) & bootCodeRead);
			if ( res < 0)
				return ERR_FWUG_FAIL_CODEREADTFLASH;// there is error on reading F/W
			if (memcmp(pBufAddr, pVerifyData, uiBufSize) != 0)
				return ERR_FWUG_FAIL + 100;		// Verifying Error
		}
		#endif

		//Per Byte
		uiSrcAddr		+= (unsigned int)uiBufSize;
		readRomIndex 	+= (unsigned int)uiBufSize;

		//Per Sector
		sBootCodeWrite.start_page	+= sBootCodeWrite.rw_size;
		LPRINTF(".");
	}

	LPRINTF("\nWrite Code Completed\n");
#endif
	return SUCCESS;
}

int FwdnWriteConfigBootSD(unsigned int uiConfigSize, unsigned char *pConfigAddr)
{
	int					res = 1;
	unsigned int 		uiCRC;
	ioctl_diskrwpage_t	rwpage;
	unsigned char *pTempData = (unsigned char*)tempBuffer;

	memset(pTempData, 0x00, FWDN_WRITE_BUFFER_SIZE);

	//Copy from Config code to Buffer
	memcpy(pTempData, pConfigAddr, uiConfigSize);

	uiCRC = FWUG_CalcCrc8(pTempData, uiConfigSize);
	word_of(pTempData + uiConfigSize) = uiCRC;
	uiConfigSize += CRC32_SIZE;

	rwpage.buff			= pTempData;
	rwpage.rw_size		= BYTE_TO_SECTOR(uiConfigSize);
	rwpage.start_page	= (unsigned long)BOOTSD_GetConfigCodeAddr(uiConfigSize);

	res = DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_BOOTCODE_WRITE_PAGE, (void *) & rwpage);
	if (res < 0) return ERR_FWUG_FAIL_HEADERWRITETFLASH;

#if 1 //Verify
	{
		unsigned char		*pVerifyData;

		pVerifyData	    = (unsigned char*)(pTempData + uiConfigSize);
		rwpage.buff	= pVerifyData;

		res = DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_BOOTCODE_READ_PAGE, (void *) & rwpage);
		if (res < 0) return ERR_FWUG_FAIL_HEADERREADTFLASH;

		if (memcmp(pTempData, pVerifyData, uiConfigSize) != 0)
			return ERR_FWUG_FAIL + 103;
	}
#endif

	return SUCCESS;
}

extern unsigned long	InitRoutine_Start;
extern unsigned long	InitRoutine_End;
int FwdnWriteBootSDFirmware(unsigned int uiROMFileSize)
{
#ifdef BOOTSD_INCLUDE
	int					res = 1;
	ioctl_diskrwpage_t	rwpage_1;
	unsigned char		*pTempData   = (unsigned char*)tempBuffer;
	unsigned int 		uiConfigSize = (unsigned int)&InitRoutine_End - (unsigned int)&InitRoutine_Start;

	{
		//=====================================================================
		// Check                   ============================================
		//=====================================================================
		ioctl_diskinfo_t 		disk_info;

		if ( (res=DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_GET_DISKINFO, (void *)&disk_info)) < 0)
			return res;

		if (disk_info.sector_size != 512 || !uiROMFileSize)
			return -1;

		#if 0
		{
			unsigned int		uiHiddenBootSector;
			if (DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_GET_BOOTCODE_SIZE, (void *) &uiHiddenBootSector) < 0)
				return ERR_FWUG_FAIL + 103;

			if ((uiROMFileSize + uiConfigSize + CRC32_SIZE + 512) > uiHiddenBootSector*512)
				return ERR_FWUG_FAIL_NOT_ENOUGH_SPACE;
		}
		#endif
	}

	{
		//=====================================================================
		// Set SD Booting Header's Value ======================================
		//=====================================================================
		unsigned int uiCRC = 0;
		BOOTSD_Header_Info_T sHeaderInfo;

		memset(&sHeaderInfo, 0, sizeof(BOOTSD_Header_Info_T));

		sHeaderInfo.ulEntryPoint 		= BASE_ADDR;
		sHeaderInfo.ulBaseAddr 			= BASE_ADDR;
		sHeaderInfo.ulConfSize			= uiConfigSize + CRC32_SIZE;			//CRC plus
		sHeaderInfo.ulROMFileSize		= uiROMFileSize;
		{
			sHidden_Size_T stHidden;
			BOOTSD_Hidden_Info(&stHidden);
			sHeaderInfo.ulHiddenStartAddr = stHidden.ulHiddenStart[0] + stHidden.ulSectorSize[0];
			memcpy((void *)sHeaderInfo.ulHiddenSize, (const void*)stHidden.ulSectorSize, (BOOTSD_HIDDEN_MAX_COUNT << 2));
		}
		memcpy(sHeaderInfo.ucHeaderFlag, "HEAD", 4);
		sHeaderInfo.ulCRC				= uiCRC;

		//=====================================================================
		// Begin Writing Header to T-Flash ====================================
		//=====================================================================
		LPRINTF("Begin Writing Header to T-Flash\n");//========================

		memset(pTempData, 0x00, FWDN_WRITE_BUFFER_SIZE);
		rwpage_1.start_page		= BOOTSD_GetHeaderAddr();
		rwpage_1.rw_size		= 1;
		rwpage_1.buff			= pTempData;

		if (g_uiFWDN_WriteSNFlag == 1)
		{
			DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_BOOTCODE_READ_PAGE, (void *) & rwpage_1);
			uiCRC = FWUG_CalcCrc8(pTempData, 512-4 );
			if(word_of(pTempData + 512 - 4) != uiCRC)
				return (-1);

			memcpy(sHeaderInfo.ucSerialNum, pTempData + 16, 64);
		}
		else
		{
			FWDN_FNT_InsertSN(sHeaderInfo.ucSerialNum);
		}

		//=====================================================================
		// Write and Verify Header ============================================
		//=====================================================================
		rwpage_1.buff			= (unsigned char*) &sHeaderInfo;

		uiCRC = FWUG_CalcCrc8((unsigned char*) &sHeaderInfo, 512-4 );
		sHeaderInfo.ulCRC= uiCRC;

		res = DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_BOOTCODE_WRITE_PAGE, (void *) & rwpage_1);
		if (res < 0) return ERR_FWUG_FAIL_HEADERWRITETFLASH;

#if 1 //Verify
		{
			BOOTSD_Header_Info_T sHeaderInfoVerif;
			rwpage_1.buff			= (unsigned char *)&sHeaderInfoVerif;

			res = DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_BOOTCODE_READ_PAGE, (void *) & rwpage_1);

			if (res < 0)
				return ERR_FWUG_FAIL_HEADERREADTFLASH;
			if (memcmp((void *)&sHeaderInfo, (void *)&sHeaderInfoVerif, 512) != 0)
				return ERR_FWUG_FAIL + 103;
		}
#endif

		LPRINTF("Write Header Completed\n");
	}

	res = FwdnWriteConfigBootSD(uiConfigSize, (unsigned char *)&InitRoutine_Start);
	if(res != SUCCESS)
		return res;

	res = FwdnWriteCodeBootSD(uiROMFileSize, uiConfigSize, pTempData, FWDN_WRITE_BUFFER_SIZE);
	if(res != SUCCESS)
		return res;

#endif
	return 0;
}

unsigned int FwdnReadBootSDFirmware(unsigned int master)
{
	return 0;
}

int FwdnGetBootSDSerial(void)
{
#ifdef BOOTSD_INCLUDE
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
	FWDN_FNT_VerifySN(readData, 32);
#endif
	switch (FWDN_DeviceInformation.DevSerialNumberType)
	{
		case SN_VALID_16:	return 16;
		case SN_VALID_32:	return 32;
		case SN_INVALID_16:	return -16;
		case SN_INVALID_32:	return -32;
		default:	return -1;
	}
}

int FwdnSetBootSDSerial(unsigned char *ucData, unsigned int overwrite)
{
#ifdef BOOTSD_INCLUDE
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
#endif
	return 0;
}

