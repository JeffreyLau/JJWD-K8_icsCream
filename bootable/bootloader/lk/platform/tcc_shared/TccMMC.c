/****************************************************************************
 *   FileName    : TccMMC.c
 *   Description :
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/

#ifndef FWDN_DOWNLOADER_INCLUDE
//#include "Main.h"
//#include "Globals.h"
//#include "datatype.h"
//#include "ui_api.h"
//#include "lcd/lcd.h"
#include <fwdn/Disk.h>
#else
#include "Globals.h"
#include <fwdn/Disk.h>
#endif

#if	1  //defined(MMC_INCLUDE) || defined(BOOTSD_INCLUDE)

#include <tnftl/IO_TCCXXX.h>
//#include <fwdn/FSAPP.h>
#include <sdmmc/TccMMC.h>

//#include "TC_Utils.h"

#include <sdmmc/mmc_basic.h>
#include <sdmmc/mmc_ext.h>

/**************************************************************************
*  FUNCTION NAME :
*
*      int MMC_HDReadPage(int drv_num,  unsigned long LBA_addr, unsigned short nSector, void *buff);
*
*  DESCRIPTION : Read Sector within hidden area.
*
*  INPUT:
*			drv_num	= 0 or 1
*			LBA_addr	= sector adress (must be less than hidden size)
*			nSector	= sector number
*			buff	= pointer to store data from SDMMC.
*
*  OUTPUT:	int - Return Type
*			= 0 : successful.
*  			= -1 : hidden is not enabled.
*			= -2 : LBA address or Sector number is invalid.
*
**************************************************************************/
int MMC_HDReadPage(int drv_num,  unsigned long LBA_addr, unsigned short nSector, void *buff)
{
	int	iSector, res;

	if ((LBA_addr + nSector) > g_mmc[drv_num].hidden_p0.size)
	{
		iSector	= g_mmc[drv_num].hidden_p0.size - LBA_addr;
		if (iSector <= 0)
			return -2;
	}
	else
		iSector	= nSector;

	res = MMC_Read(drv_num, LBA_addr + g_mmc[drv_num].hidden_p0.start, (unsigned short)iSector, buff);
	if(res < 0)
		return MMC_Read(drv_num, LBA_addr + g_mmc[drv_num].hidden_p0.start, (unsigned short)iSector, buff);

	else
		return res;
}

/**************************************************************************
*  FUNCTION NAME :
*
*      int MMC_HDWritePage(int drv_num,  unsigned long LBA_addr, unsigned short nSector, void *buff);
*
*  DESCRIPTION : Write Sector within hidden area.
*
*  INPUT:
*			drv_num	= 0 or 1
*			LBA_addr	= sector adress (must be less than hidden size)
*			nSector	= sector number
*			buff	= pointer to store data to SDMMC.
*
*  OUTPUT:	int - Return Type
*			= 0 : successful.
*  			= -1 : hidden is not enabled.
*			= -2 : LBA address or Sector number is invalid.
*
**************************************************************************/
int MMC_HDWritePage(int drv_num,  unsigned long LBA_addr, unsigned short nSector, void *buff)
{
	int	iSector, res;

	if ((LBA_addr + nSector) > g_mmc[drv_num].hidden_p0.size)
	{
		iSector	= g_mmc[drv_num].hidden_p0.size - LBA_addr;
		if (iSector <= 0)
			return -2;
	}
	else
		iSector	= nSector;

	res = MMC_Write(drv_num, LBA_addr + g_mmc[drv_num].hidden_p0.start, (unsigned short)iSector, buff);
	if(res < 0)
		return MMC_Write(drv_num, LBA_addr + g_mmc[drv_num].hidden_p0.start, (unsigned short)iSector, buff);
	else
		return res;
}

int MMC_HDClearPages(int drv_num, unsigned long start_sector, unsigned long end_sector)
{
	return 0;
}

int MMC_HDCheck(int drv_num, unsigned LBA_addr, unsigned nSector)
{
	sMMCrecord	*pMMC	= &g_mmc[drv_num];

	if ((pMMC->area_en & SDMMC_AREA_HIDDEN_FS) == 0)
		return -1;
	if ((LBA_addr + nSector) > pMMC->filesys_hd.size)
		return -1;

	return pMMC->filesys_hd.start;
}

int MMC_HD_Ioctl(int drv_num, int function, void *param)
{
	int	iRet;
	sMMCrecord	*pMMC	= &g_mmc[drv_num];

	switch (function)
	{
		case DEV_GET_DISKINFO:
		{
			ioctl_diskinfo_t *info	= (ioctl_diskinfo_t *)param;

			if (ISZERO(pMMC->area_en, SDMMC_AREA_HIDDEN_FS))
			{
				info->sector_size	= 512;
				info->cylinder		= 0;
				info->head		= 0;
				info->sector		= 0;
			}
			else
			{
				info->sector_size	= 512;
				info->cylinder		= (unsigned short)pMMC->filesys_hd.cyl;
				info->head			= pMMC->filesys_hd.head;
				info->sector		= pMMC->filesys_hd.sect;
			}
		}
		break;

		default:
			iRet	= MMC_Ioctl(drv_num, function, param);
			return	iRet;
	}

	return 0;	// SUCCESS
}

#ifdef BOOTSD_INCLUDE
int HARP_Read(int drv_num, unsigned long LBA_addr, unsigned short nSector, void *buff)
{
	checkAndChangePUN(HARP_PUN);
	return MMC_Read( HARP_PUN, LBA_addr, nSector, buff);
}
int HARP_ReadMultiStart(unsigned long ulAddr, unsigned long size)
{
	checkAndChangePUN(HARP_PUN);
	return MMC_ReadMultiStart(HARP_PUN,  ulAddr, size);
}
int HARP_ReadMulti(int drv_num, unsigned long LBA_addr, unsigned short nSector, void *buff)
{
	checkAndChangePUN(HARP_PUN);
	return MMC_ReadMulti(HARP_PUN, LBA_addr, nSector, buff);
}
int HARP_ReadMultiStop(void)
{
	checkAndChangePUN(HARP_PUN);
	return MMC_ReadMultiStop(HARP_PUN);
}
int HARP_Write(int drv_num, unsigned long LBA_addr, unsigned short nSector, void *buff)
{
	checkAndChangePUN(HARP_PUN);
	return MMC_Write(HARP_PUN,LBA_addr, nSector, buff);
}
int HARP_WriteMultiStart(unsigned long ulAddr, unsigned long size)
{
	checkAndChangePUN(HARP_PUN);
	return MMC_WriteMultiStart(HARP_PUN, ulAddr, size);
}
int HARP_WriteMulti(int drv_num, unsigned long LBA_addr, unsigned short nSector, void *buff)
{
	checkAndChangePUN(HARP_PUN);
	return MMC_WriteMulti(HARP_PUN, LBA_addr, nSector, buff);
}
int HARP_WriteMultiStop(void)
{
	checkAndChangePUN(HARP_PUN);
	return MMC_WriteMultiStop(HARP_PUN);
}
int HARP_HDReadPage( unsigned long LBA_addr, unsigned short nSector, void *buff)
{
	checkAndChangePUN(HARP_PUN);
	return MMC_HDReadPage(HARP_PUN, LBA_addr,nSector,buff);
}
int HARP_HDWritePage( unsigned long LBA_addr, unsigned short nSector, void *buff)
{
	checkAndChangePUN(HARP_PUN);
	return MMC_HDWritePage(HARP_PUN, LBA_addr,nSector,buff);
}
int HARP_HDClearPages(unsigned long start_sector, unsigned long end_sector)
{
	checkAndChangePUN(HARP_PUN);
	return MMC_HDClearPages( HARP_PUN, start_sector, end_sector);
}
int HARP_Ioctl(int function, void *param)
{
	checkAndChangePUN(HARP_PUN);
	return MMC_Ioctl(HARP_PUN,function, param);
}
#endif

#ifdef SDMMC_INTERNAL_HIDDEN_STORAGE_INCLUDE
int HARP_HDCheck(unsigned LBA_addr, unsigned nSector)
{
	return MMC_HDCheck(HARP_PUN, LBA_addr, nSector);
}

int HARP_HD_Read(int drv_num, unsigned long LBA_addr, unsigned short nSector, void *buff)
{
	int	harp_start;
	if ((harp_start=HARP_HDCheck(LBA_addr, nSector)) < 0)
		return -1;

	checkAndChangePUN(HARP_PUN);
	return MMC_Read(HARP_PUN, LBA_addr + harp_start, nSector, buff);
}

int HARP_HD_ReadMultiStart(unsigned long LBA_addr, unsigned long nSector)
{
	int	harp_start;
	if ((harp_start=HARP_HDCheck(LBA_addr, nSector)) < 0)
		return -1;

	checkAndChangePUN(HARP_PUN);
	return MMC_ReadMultiStart(HARP_PUN, LBA_addr + harp_start, nSector);
}

int HARP_HD_ReadMulti(int drv_num, unsigned long LBA_addr, unsigned short nSector, void *buff)
{
	int	harp_start;
	if ((harp_start=HARP_HDCheck(LBA_addr, nSector)) < 0)
		return -1;

	checkAndChangePUN(HARP_PUN);
	return MMC_ReadMulti(HARP_PUN, LBA_addr + harp_start, nSector, buff);
}

int HARP_HD_ReadMultiStop(void)
{
	checkAndChangePUN(HARP_PUN);
	return MMC_ReadMultiStop(HARP_PUN);
}

int HARP_HD_Write(int drv_num, unsigned long LBA_addr, unsigned short nSector, void *buff)
{
	int	harp_start;
	if ((harp_start=HARP_HDCheck(LBA_addr, nSector)) < 0)
		return -1;

	checkAndChangePUN(HARP_PUN);
	return MMC_Write(HARP_PUN, LBA_addr + harp_start, nSector, buff);
}

int HARP_HD_WriteMultiStart(unsigned long LBA_addr, unsigned long nSector)
{
	int	harp_start;
	if ((harp_start=HARP_HDCheck(LBA_addr, nSector)) < 0)
		return -1;

	checkAndChangePUN(HARP_PUN);
	return MMC_WriteMultiStart(HARP_PUN, LBA_addr + harp_start, nSector);
}

int HARP_HD_WriteMulti(int drv_num, unsigned long LBA_addr, unsigned short nSector, void *buff)
{
	int	harp_start;
	if ((harp_start=HARP_HDCheck(LBA_addr, nSector)) < 0)
		return -1;

	checkAndChangePUN(HARP_PUN);
	return MMC_WriteMulti(HARP_PUN, LBA_addr + harp_start, nSector, buff);
}

int HARP_HD_WriteMultiStop(void)
{
	checkAndChangePUN(HARP_PUN);
	return MMC_WriteMultiStop(HARP_PUN);
}
int HARP_HD_Ioctl(int function, void *param)
{
	checkAndChangePUN(HARP_PUN);
	return	MMC_HD_Ioctl(HARP_PUN, function, param);
}
#endif

int SD_Read(int drv_num, unsigned int LBA_addr, unsigned short nSector, void *buff)
{
	int		iRet;
	checkAndChangePUN(SD_PUN);
	iRet	= MMC_Read( SD_PUN, LBA_addr, nSector, buff);
	return	iRet;
}

int SD_ReadMultiStart(unsigned int ulAddr, unsigned int size)
{
	checkAndChangePUN(SD_PUN);
	return MMC_ReadMultiStart(SD_PUN,  ulAddr, size);
}

int SD_ReadMulti(int drv_num, unsigned int LBA_addr, unsigned short nSector, void *buff)
{
	checkAndChangePUN(SD_PUN);
	return MMC_ReadMulti(SD_PUN, LBA_addr, nSector, buff);
}

int SD_ReadMultiStop(void)
{
	checkAndChangePUN(SD_PUN);
	return MMC_ReadMultiStop(SD_PUN);
}

int SD_Write(int drv_num, unsigned int LBA_addr, unsigned short nSector, void *buff)
{
	int		iRet;
	checkAndChangePUN(SD_PUN);
	iRet	= MMC_Write(SD_PUN,LBA_addr, nSector, buff);
	return	iRet;
}

int SD_WriteMultiStart(unsigned int ulAddr, unsigned int size)
{
	checkAndChangePUN(SD_PUN);
	return MMC_WriteMultiStart(SD_PUN, ulAddr, size);
}

int SD_WriteMulti(int drv_num, unsigned int LBA_addr, unsigned short nSector, void *buff)
{
	checkAndChangePUN(SD_PUN);
	return MMC_WriteMulti(SD_PUN, LBA_addr, nSector, buff);
}

int SD_WriteMultiStop(void)
{
	checkAndChangePUN(SD_PUN);
	return MMC_WriteMultiStop(SD_PUN);
}

int SD_Ioctl(int function, void *param)
{
	checkAndChangePUN(SD_PUN);
	return MMC_Ioctl(SD_PUN, function, param);
}

#ifdef SDMMC_EXTERNAL_HIDDEN_STORAGE_INCLUDE
int SD_HDCheck(unsigned LBA_addr, unsigned nSector)
{
	return MMC_HDCheck(SD_PUN, LBA_addr, nSector);
}

int SD_HD_Read(int drv_num, unsigned long LBA_addr, unsigned short nSector, void *buff)
{
	int	sd_start;
	if ((sd_start=SD_HDCheck(LBA_addr, nSector)) < 0)
		return -1;

	checkAndChangePUN(SD_PUN);
	return MMC_Read(SD_PUN, LBA_addr + sd_start, nSector, buff);
}

int SD_HD_ReadMultiStart(unsigned long LBA_addr, unsigned long nSector)
{
	int	sd_start;
	if ((sd_start=SD_HDCheck(LBA_addr, nSector)) < 0)
		return -1;

	checkAndChangePUN(SD_PUN);
	return MMC_ReadMultiStart(SD_PUN, LBA_addr + sd_start, nSector);
}

int SD_HD_ReadMulti(int drv_num, unsigned long LBA_addr, unsigned short nSector, void *buff)
{
	int	sd_start;
	if ((sd_start=SD_HDCheck(LBA_addr, nSector)) < 0)
		return -1;

	checkAndChangePUN(SD_PUN);
	return MMC_ReadMulti(SD_PUN, LBA_addr + sd_start, nSector, buff);
}

int SD_HD_ReadMultiStop(void)
{
	checkAndChangePUN(SD_PUN);
	return MMC_ReadMultiStop(SD_PUN);
}

int SD_HD_Write(int drv_num, unsigned long LBA_addr, unsigned short nSector, void *buff)
{
	int	sd_start;
	if ((sd_start=SD_HDCheck(LBA_addr, nSector)) < 0)
		return -1;

	checkAndChangePUN(SD_PUN);
	return MMC_Write(SD_PUN, LBA_addr + sd_start, nSector, buff);
}

int SD_HD_WriteMultiStart(unsigned long LBA_addr, unsigned long nSector)
{
	int	sd_start;
	if ((sd_start=SD_HDCheck(LBA_addr, nSector)) < 0)
		return -1;

	checkAndChangePUN(SD_PUN);
	return MMC_WriteMultiStart(SD_PUN, LBA_addr + sd_start, nSector);
}

int SD_HD_WriteMulti(int drv_num, unsigned long LBA_addr, unsigned short nSector, void *buff)
{
	int	sd_start;
	if ((sd_start=SD_HDCheck(LBA_addr, nSector)) < 0)
		return -1;

	checkAndChangePUN(SD_PUN);
	return MMC_WriteMulti(SD_PUN, LBA_addr + sd_start, nSector, buff);
}

int SD_HD_WriteMultiStop(void)
{
	checkAndChangePUN(SD_PUN);
	return MMC_WriteMultiStop(SD_PUN);
}

int SD_HD_Ioctl(int function, void *param)
{
	checkAndChangePUN(SD_PUN);
	return	MMC_HD_Ioctl(SD_PUN, function, param);
}
#endif

#endif //MMC_INCLUDE

#ifdef	NU_FILE_INCLUDE
/************************************************************************
* FUNCTION
*
*       MMC_IO_Open
*
* DESCRIPTION
*
*       This function prepares the NAND Flash for usage by allocating the
*       pages necessary for usage.
*
* INPUTS
*
*       driveno                             The number assigned to the NAND Flash.
*
* OUTPUTS
*
*       YES                                 Successful Completion.
*       NO                                  Couldn't allocate all of the
*                                            pages.
*
*************************************************************************/
INT MMC_IO_Open(UINT16 driveno)
{
	#if defined(BOOTSD_INCLUDE)
	{
		if (TC_ISINTDRIVE(driveno))
		{
			HARP_Ioctl(DEV_INITIALIZE, NULL);
			return	(YES);
		}
	}
	#endif

	#if defined(MMC_INCLUDE)
	{
		SD_Ioctl(DEV_INITIALIZE, NULL);
	    	return	(YES);
	}
	#else
	{
		return	(NO);
	}
	#endif
}

/************************************************************************
* FUNCTION
*
*       MMC_IO_RAW_Open
*
* DESCRIPTION
*
*       This function doesn't do anything for the NAND Flash.  It is
*       included for devtable consistency.
*
* INPUTS
*
*       None.
*
* OUTPUTS
*
*       None.
*
*************************************************************************/
INT MMC_IO_RAW_Open(UINT16 driveno)
{
	return	(NO);
}

/************************************************************************
* FUNCTION
*
*       MMC_IO_Close
*
* DESCRIPTION
*
*       This function deallocates all of the pages associated with the
*       NAND Flash. The actual code here is commented out since we
*       probably don't want to loose the data on a close.
*
* INPUTS
*
*       driveno                             The number assigned to the NAND Flash.
*
* OUTPUTS
*
*       YES                                 Successful Completion.
*       NO                                  Couldn't allocate all of the
*                                            pages.
*
*************************************************************************/
INT MMC_IO_Close(UINT16 driveno)
{
	return	(YES);
}

/************************************************************************
* FUNCTION
*
*       MMC_IO_ReadWrite
*
* DESCRIPTION
*
*       This function reads or writes data from and to the NAND Flash
*       based on the 'reading' parameter.
*
* INPUTS
*
*       driveno                             The number assigned to the
*                                            RAM Disk (not used)
*       block                               The block number to read or
*                                            write
*       buffer                              Pointer to the data to be
*                                            placed from a read or
*                                            stored on a write
*       count                               Number of bytes to be read
*                                            or written
*       reading                             Indicates whether or not we
*                                            are reading or writing
*
* OUTPUTS
*
*       YES                                 Successful Completion.
*       NO                                  Block number is out of range.
*
*************************************************************************/
INT MMC_IO_ReadWrite(UINT16 driveno, UINT32 block, VOID *buffer, UINT16 count, INT reading)
{
	int result;

	result = result;

	#if defined(BOOTSD_INCLUDE)
	{
		if (TC_ISINTDRIVE(driveno))
		{
			if (reading)
				result = HARP_Read(driveno, block, count, buffer);
			else
				result = HARP_Write(driveno, block, count, buffer);

			return	(!result) ? YES : NO;
		}
	}
	#endif

	#if defined(MMC_INCLUDE)
	{
		if (reading)
			result = SD_Read(driveno, block, count, buffer);
		else
			result = SD_Write(driveno, block, count, buffer);

		return	(!result) ? YES : NO;
	}
	#else
	{
		return	(NO);
	}
	#endif
}

/************************************************************************
* FUNCTION
*
*       MMC_IO_Ioctl
*
* DESCRIPTION
*
*       This function doesn't do anything for the NAND Flash.  It is
*       included for devtable consistency.
*
* INPUTS
*
*       None.
*
* OUTPUTS
*
*       None.
*
*************************************************************************/
INT MMC_IO_Ioctl(UINT16 driveno, UINT16 command, VOID *buffer)
{
	#if defined(BOOTSD_INCLUDE)
		if (TC_ISINTDRIVE(driveno))
			return (HARP_Ioctl(command, buffer));
	#endif

	#if defined(MMC_INCLUDE)
		return (SD_Ioctl(command, buffer));
	#else
		return (NO);
	#endif
}

/************************************************************************
* FUNCTION
*
*       MMC_DskChk
*
* DESCRIPTION
*
*       This function doesn't do anything for the NAND Flash.  It is
*       included for devtable consistency.
*
* INPUTS
*
*       None.
*
* OUTPUTS
*
*       None.
*
*************************************************************************/
INT MMC_DskChk(UINT16 driveno)
{
	#if defined(BOOTSD_INCLUDE)
	{
		if (TC_ISINTDRIVE(driveno))
			return (YES);
	}
	#endif

	#if defined(MMC_INCLUDE)
		return (MMC_Ioctl(SD_PUN, DEV_GET_INSERTED, NULL));
	#else
		return (NO);
	#endif
}

#if defined(SDMMC_INTERNAL_HIDDEN_STORAGE_INCLUDE) || defined(SDMMC_EXTERNAL_HIDDEN_STORAGE_INCLUDE)

INT MMC_HD_IO_Open(UINT16 driveno)
{
	#if defined(SDMMC_INTERNAL_HIDDEN_STORAGE_INCLUDE)
	{
		if (driveno == SDMMC_INTERNAL_HIDDEN_STORAGE_START_NO)
		{
			HARP_HD_Ioctl(DEV_INITIALIZE, NULL);
			return (YES);
		}
	}
	#endif

	#if defined(SDMMC_EXTERNAL_HIDDEN_STORAGE_INCLUDE)
	{
		SD_HD_Ioctl(DEV_INITIALIZE, NULL);
	    	return	(YES);
	}
	#else
	{
		return	NO;
	}
	#endif
}

INT MMC_HD_IO_RAW_Open(UINT16 driveno)
{
	return	(NO);
}

INT MMC_HD_IO_Close(UINT16 driveno)
{
	return	(YES);
}

INT MMC_HD_IO_ReadWrite(UINT16 driveno, UINT32 block, VOID *buffer, UINT16 count, INT reading)
{
	int result;

	#if defined(SDMMC_INTERNAL_HIDDEN_STORAGE_INCLUDE)
	{
		if (driveno == SDMMC_INTERNAL_HIDDEN_STORAGE_START_NO)
		{
			if (reading)
				result = HARP_HD_Read(driveno, block, count, buffer);
			else
				result = HARP_HD_Write(driveno, block, count, buffer);

			return	(!result) ? YES : NO;
		}
	}
	#endif

	#if defined(SDMMC_EXTERNAL_HIDDEN_STORAGE_INCLUDE)
	{
		if (reading)
			result = SD_HD_Read(driveno, block, count, buffer);
		else
			result = SD_HD_Write(driveno, block, count, buffer);

		return	(!result) ? YES : NO;
	}
	#else
	{
		return	(NO);
	}
	#endif
}

INT MMC_HD_IO_Ioctl(UINT16 driveno, UINT16 command, VOID *buffer)
{
	#if defined(SDMMC_INTERNAL_HIDDEN_STORAGE_INCLUDE)
		if (driveno == SDMMC_INTERNAL_HIDDEN_STORAGE_START_NO)
			return (HARP_HD_Ioctl(command, buffer));
	#endif

	#if defined(SDMMC_EXTERNAL_HIDDEN_STORAGE_INCLUDE)
		return (SD_HD_Ioctl(command, buffer));
	#else
		return (NO);
	#endif
}

#endif	// defined(SDMMC_EXTERNAL_HIDDEN_STORAGE_INCLUDE) || defined(SDMMC_INTERNAL_HIDDEN_STORAGE_INCLUDE)

#endif	//NU_FILE_INCLUDE

/* end of file */

