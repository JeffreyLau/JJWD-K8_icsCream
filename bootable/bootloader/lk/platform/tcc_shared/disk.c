/****************************************************************************
 *   FileName    :disk.c
 *   Description :
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/

#include <fwdn/FSAPP.h>
//#include "KFSutils.h"
#include <tnftl/nand_drv.h>
#include <fwdn/Disk.h>
#include <sdmmc/sd_memory.h>



/*****************************************************************************
 *	disk device name string array
 *	the order should be matched with DISK_DEVICE type.
 *****************************************************************************/
#ifndef NULL
#define NULL		0
#endif
extern int fwdn_connect;

/*********************************************************************
* tDeviceDriver Structure Member Item Definition macro

DISK_PROPERTY						Property;
(const char *Name, DISK_LOCK_FLAG LockFlag, DISK_DEVICE DiskType, DISK_MOUNT_STATE MountState, DISK_BUSY_STATE BusyState, DISK_MSC_DRV_STATE MSCDrvSupport, DISK_MOUNT_TYPE_STATE DrvMountType, int PartitionIndex)
tDeviceRwFunctions 					ReadSector;
tDeviceRwFunctions 					WriteSector;
tDeviceReadMultiStartFunctions		ReadMultiStart;
tDeviceReadMultiFunctions			ReadMultiSector;
tDeviceReadMultiStopFunctions		ReadMultiStop;
tDeviceWriteMultiStartFunctions		WriteMultiStart;
tDeviceWriteMultiFunctions			WriteMultiSector;
tDeviceWriteMultiStopFunctions		WriteMultiStop;
tDeviceHiddenRWFunctions			HDReadSector;
tDeviceHiddenRWFunctions			HDWriteSector;
tDeviceHiddenClearPageFunctions		HDClearSector;
tDeviceIoctlFunctions 				Ioctl;
*********************************************************************/
#define NULL_DRIVER		{	{NULL, DISK_DEVICE_UNLOCK, MAX_DEVICE_NUM, DISK_STATE_NOTMOUNTED, DISK_STATE_FREE, DISK_MSC_DRV_NOT_SUPPORT, DISK_MOUNT_TYPE_INTERNAL, 0}, \
							NULL,							\
							NULL,							\
							NULL,							\
							NULL,							\
							NULL,							\
							NULL,							\
							NULL,							\
							NULL,							\
							NULL,							\
							NULL,							\
							NULL,							\
							NULL	}

#define HDD_DRIVER		{	{"HDD",DISK_DEVICE_UNLOCK, DISK_DEVICE_HDD, DISK_STATE_NOTMOUNTED, DISK_STATE_FREE, DISK_MSC_DRV_SUPPORT, DISK_MOUNT_TYPE_INTERNAL, 0 }, \
							HDD_ReadSector,					\
							HDD_WriteSector,				\
							HDD_ReadMultiSectorStart,		\
							HDD_ReadMultiSector,			\
							HDD_ReadMultiSectorStop,		\
							HDD_WriteMultiSectorStart,		\
							HDD_WriteMultiSector,			\
							HDD_WriteMultiSectorStop,		\
							HDD_HDReadPage,					\
							HDD_HDWritePage,				\
							HDD_HDClearPages,				\
							HDD_Ioctl	}

#define NAND_DRIVER		{	{"NAND",DISK_DEVICE_UNLOCK, DISK_DEVICE_NAND, DISK_STATE_NOTMOUNTED, DISK_STATE_FREE, DISK_MSC_DRV_SUPPORT, DISK_MOUNT_TYPE_INTERNAL, 0}, \
							NAND_ReadSector,				\
							NAND_WriteSector,				\
							NULL,							\
							NAND_ReadSector,				\
							NULL,							\
							NULL,							\
							NAND_WriteSector,				\
							NULL,							\
							NAND_HDReadPage,				\
							NAND_HDWritePage,				\
							NAND_HDClearPages,				\
							NAND_Ioctl	}

#define NAND_HD_DRIVER	{	{"NAND_HD",DISK_DEVICE_UNLOCK, DISK_DEVICE_NAND_HD, DISK_STATE_NOTMOUNTED, DISK_STATE_FREE, DISK_MSC_DRV_NOT_SUPPORT, DISK_MOUNT_TYPE_INTERNAL, 0},	\
							NAND_HDReadSector,				\
							NAND_HDWriteSector,				\
							NULL,							\
							NAND_HDReadSector,				\
							NULL,							\
							NULL,							\
							NAND_HDWriteSector,				\
							NULL,							\
							NULL,							\
							NULL,							\
							NULL,							\
							NAND_HDIoctl	}

#define MMC_DRIVER		{	{"MMC",DISK_DEVICE_UNLOCK, DISK_DEVICE_MMC, DISK_STATE_NOTMOUNTED, DISK_STATE_FREE, DISK_MSC_DRV_SUPPORT, DISK_MOUNT_TYPE_EXTERNAL, 0},	\
							SD_Read,						\
							SD_Write,						\
							SD_ReadMultiStart,				\
							SD_ReadMulti,					\
							SD_ReadMultiStop,				\
							SD_WriteMultiStart,				\
							SD_WriteMulti,					\
							SD_WriteMultiStop,				\
							NULL, 							\
							NULL, 							\
							NULL, 							\
							SD_Ioctl	}
#define TRIFLASH_DRIVER	{	{"TRIFLASH", DISK_DEVICE_UNLOCK, DISK_DEVICE_TRIFLASH, DISK_STATE_NOTMOUNTED, DISK_STATE_FREE, DISK_MSC_DRV_SUPPORT, DISK_MOUNT_TYPE_EXTERNAL, 0}, \
							BOOTSD_Read,						\
							BOOTSD_Write,						\
							BOOTSD_ReadMultiStart,			\
							BOOTSD_ReadMulti,					\
							BOOTSD_ReadMultiStop,				\
							BOOTSD_WriteMultiStart,			\
							BOOTSD_WriteMulti,				\
							BOOTSD_WriteMultiStop,	 		\
							NULL,				\
							NULL,				\
							NULL,				\
							BOOTSD_Ioctl	}
#define UFD_DRIVER		{	{"UFD", DISK_DEVICE_UNLOCK, DISK_DEVICE_UHP, DISK_STATE_NOTMOUNTED, DISK_STATE_FREE, DISK_MSC_DRV_NOT_SUPPORT, DISK_MOUNT_TYPE_EXTERNAL, 0}, \
							UHP_FS_ReadSector,				\
							UHP_FS_WriteSector,				\
							NULL,							\
							NULL,							\
							NULL,							\
							NULL,							\
							NULL,							\
							NULL,	 						\
							NULL,							\
							NULL,							\
							NULL,							\
							UHP_Ioctl	}

/*****************************************************************************
 * 	Default Disk Device Settings
 *
 ****************************************************************************/
#if	defined(NAND_INCLUDE)
const unsigned int DISK_DefaultDriveType = DISK_DEVICE_NAND;
#elif defined(HDD_INCLUDE)
const unsigned int DISK_DefaultDriveType = DISK_DEVICE_HDD;
#elif defined(BOOTSD_INCLUDE)
const unsigned int DISK_DefaultDriveType = DISK_DEVICE_TRIFLASH;
#elif defined(MMC_INCLUDE)
const unsigned int DISK_DefaultDriveType = DISK_DEVICE_MMC;
#elif defined(UHP_INCLUDE)
const unsigned int DISK_DefaultDriveType = DISK_DEVICE_UHP;
#else
#error		You Must Select NAND_INCLUDE or HDD_INCLUDE
#endif
/*****************************************************************************
 * 	DiskList Array Definition
 *
 * 	Structure array for device driver.
 * 	you can add another device before NULL_DRIVER.
 *
 * 	CAUTION!!
 * 	Do Not Delete NULL_DRIVER entry.
 ****************************************************************************/
tDeviceDriver DiskList[] = {
#ifdef HDD_INCLUDE
	HDD_DRIVER,
#endif
#ifdef NAND_INCLUDE
	NAND_DRIVER,
	NAND_HD_DRIVER,
#endif
#ifdef BOOTSD_INCLUDE
	TRIFLASH_DRIVER,
#endif
#ifdef MMC_INCLUDE
	MMC_DRIVER,
#endif
#ifdef UHP_INCLUDE
	UFD_DRIVER,
#endif
	NULL_DRIVER
};

const char UnknownDeviceName[]="UNKNOWN";
/************************************************************************
* FUNCTION
*			DISK_GetDeviceStatus
*
* DESCRIPTION
*
* INPUTS
*			iDeviceNum - Device Number
*			iUSBMode   - Except USB host or not
*			iExtDevice - External card only
*
* OUTPUTS
*			This device's status
*************************************************************************/
unsigned int DISK_GetDeviceStatus(int iDeviceNum,int iUSBMode,int iExtDevice)
{
	unsigned int uiStatus = FALSE;

	switch(iDeviceNum)
	{
#ifdef HDD_INCLUDE
	case DISK_DEVICE_HDD:
		if(!iExtDevice)
			uiStatus = TRUE;
		break;
#endif
#ifdef NAND_INCLUDE
	case DISK_DEVICE_NAND:
		if(!iExtDevice)
			uiStatus = TRUE;
		break;
#ifdef NAND_HD_DRV_INCLUDE
	case DISK_DEVICE_NAND_HD:
		if(!iExtDevice)
			uiStatus = TRUE;
		break;
#endif
#endif
#ifdef UHP_INCLUDE
	case DISK_DEVICE_UHP:
		if(!iUSBMode)
			uiStatus = TRUE;	// Just Boot Mode
		break;
#endif
#ifdef BOOTSD_INCLUDE
	case DISK_DEVICE_TRIFLASH:
		if(!iExtDevice)
			uiStatus = TRUE;
		break;
#endif
#ifdef MMC_INCLUDE
	case DISK_DEVICE_MMC:
		if (!fwdn_connect)
			uiStatus = TRUE;
		break;
#endif
	default:
		uiStatus	= FALSE;
		iUSBMode	= FALSE;
		iExtDevice	= FALSE;
	}

	return uiStatus;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_GetDiskType
 *
 *	Description :	Returns DiskType in the DiskList Array given index
 *
 * 	Parameter 	:	index - index
 *
 *  Returns		:	Returns DiskType if success. Returns -1 if fail.
 *****************************************************************************/
int DISK_GetDiskType( int index )
{
	if ( index < 0 || index > ((sizeof(DiskList)/sizeof(tDeviceDriver)) -1))
	{
		return -1;
	}
	return DiskList[index].Property.DiskType;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_GetDiskTypeByPartID
 *
 *	Description :	Returns DiskType in the DiskList Array given index
 *
 * 	Parameter 	:	index - index
 *
 *  Returns		:	Returns DiskType if success. Returns -1 if fail.
 *****************************************************************************/
int DISK_GetDiskTypeByPartID( int PartID )
{
	int index = 0;
	//while (DiskList[index].DiskType != TOTAL_DEVICE_NUM) {
	while ( index < (sizeof(DiskList)/sizeof(tDeviceDriver)) ) {
		if ( DiskList[index].Property.PartitionIndex== PartID) {
			return DiskList[index].Property.DiskType;
		}
		index++;
	}
	return -1;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_GetSupportMSCDrive
 *
 *	Description :	Returns whether or not support as drive on MSC protocol
 *
 * 	Parameter 	:	index - index
 *
 *  Returns		:	Returns DiskType if success. Returns -1 if fail.
 *****************************************************************************/
int DISK_GetSupportMSCDrive( int index )
{
	if ( index < 0 || index > ((sizeof(DiskList)/sizeof(tDeviceDriver)) -1))
	{
		return -1;
	}
	return DiskList[index].Property.MSCDrvSupport;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_GetDiskMountType
 *
 *	Description :	Returns type of disk mount
 *
 * 	Parameter 	:	index - index
 *
 *  Returns		:	Returns DiskType if success. Returns -1 if fail.
 *****************************************************************************/
int DISK_GetDiskMountType( int index )
{
	if ( index < 0 || index > ((sizeof(DiskList)/sizeof(tDeviceDriver)) -1))
	{
		return -1;
	}
	return DiskList[index].Property.DrvMountType;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_FindDisk
 *
 *	Description :	Returns Index in the DiskList array given DiskType
 *
 * 	Parameter 	:	drv_type - Disk Type(NAND, MMC, HDD ... )
 *
 *  Returns		:	Returns index if success. Returns negative value if fail.
 *****************************************************************************/
int	DISK_FindDisk( int drv_type )
{
	int index = 0;
	//while (DiskList[index].DiskType != TOTAL_DEVICE_NUM) {
	while(index < (sizeof(DiskList)/sizeof(tDeviceDriver)) )
	{
		if( DiskList[index].Property.DiskType == drv_type)
		{
			return index;
		}
		index++;
	}
	return -1;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_ReadSector
 *
 *	Description :	Read sector(s) from Lun at the drv_type given Address
 *
 * 	Parameter 	:	drv_type 	- Drive Type(NAND, MMC, HDD ...)
 * 					lun			- Physical Drive Number. If there is two HDD disks in
 * 								  the set, the first is 0 and second is 1
 * 					lba_addr	- Sector Address
 * 					nSector		- Sector Count
 * 					buff		- buffer pointer
 *
 *  Returns		:	Returns zero if success or returns minus value if fail.
 *
 *
 *  TODO		:	The lun parameter is experimental now. Fix "0"
 *****************************************************************************/
int		DISK_ReadSector( int drv_type, int lun, unsigned long lba_addr, unsigned short nSector, void *buff )
{
	int		res;
	int		index = 0;

	index = DISK_FindDisk( drv_type );

	if ( index < 0 )
		return index;

	if ( DiskList[index].ReadSector != NULL )
	{
		while ( DiskList[index].Property.LockFlag == DISK_DEVICE_LOCK );

		DiskList[index].Property.LockFlag = DISK_DEVICE_LOCK;
		res = DiskList[index].ReadSector(lun, lba_addr, nSector, buff);
		DiskList[index].Property.LockFlag = DISK_DEVICE_UNLOCK;

		return res;
	}

	return -1;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_WriteSector
 *
 *	Description :	Write sector(s) from Lun at the drv_type given Address
 *
 * 	Parameter 	:	drv_type 	- Drive Type(NAND, MMC, HDD ...)
 * 					lun			- Physical Drive Number. If there is two HDD disks in
 * 								  the set, the first is 0 and second is 1
 * 					lba_addr	- Sector Address
 * 					nSector		- Sector Count
 * 					buff		- buffer pointer
 *
 *  Returns		:	Returns zero if success or returns minus value if fail.
 *
 *  TODO		:	The lun parameter is experimental now. Fix "0"
 *****************************************************************************/
int		DISK_WriteSector( int drv_type, int lun, unsigned long lba_addr, unsigned short nSector, void *buff )
{
	int		res;
	int		index = 0;

	index = DISK_FindDisk( drv_type );

	if ( index < 0 )
		return index;

	if ( DiskList[index].WriteSector != NULL )
	{
		while ( DiskList[index].Property.LockFlag == DISK_DEVICE_LOCK );

		DiskList[index].Property.LockFlag = DISK_DEVICE_LOCK;
		res = DiskList[index].WriteSector(lun, lba_addr, nSector, buff);
		DiskList[index].Property.LockFlag = DISK_DEVICE_UNLOCK;
		return res;
	}
	return -1;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_ReadMultiStart
 *
 *	Description :	Notifying Function that Sequential data reading operations will
 *					proceed at the future.
 *
 * 	Parameter 	:	drv_type	- Disk Type (NAND, MMC, HDD ...)
 * 					lba_addr	- Starting Sector Address
 *
 *  Returns		:	Returns zero if success or returns minus value if fail.
 *
 *****************************************************************************/
int		DISK_ReadMultiStart( int drv_type, int lba_addr, int size )
{
	int		res;
	int		index = 0;

	index = DISK_FindDisk( drv_type );

	if ( index < 0 )
		return index;

	if ( DiskList[index].ReadMultiStart != NULL )
	{
		while ( DiskList[index].Property.LockFlag == DISK_DEVICE_LOCK );

		DiskList[index].Property.LockFlag = DISK_DEVICE_LOCK;
		res = DiskList[index].ReadMultiStart( lba_addr, size);
		DiskList[index].Property.LockFlag = DISK_DEVICE_UNLOCK;

		return res;
	}

	return -1;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_ReadMultiSector
 *
 *	Description :	Read Sequential sector function after calling DISK_ReadMultiStart.
 *
 * 	Parameter 	:	drv_type 	- Drive Type(NAND, MMC, HDD ...)
 * 					lun			- Physical Drive Number. If there is two HDD disks in
 * 								  the set, the first is 0 and second is 1
 * 					lba_addr	- Sector Address
 * 					nSector		- Sector Count
 * 					buff		- buffer pointer
 *
 *  Returns		:	Returns "0" if success. Returns -1 if fail.
 *
 *  TODO		:	The lun parameter is experimental now. Fix "0"
 *
 *****************************************************************************/
int		DISK_ReadMultiSector( int drv_type, int lun, unsigned long lba_addr, unsigned short nSector, void *buff )
{
	int		res;
	int		index = 0;

	index = DISK_FindDisk( drv_type );

	if ( index < 0 )
		return index;

	if ( DiskList[index].ReadMultiSector != NULL )
	{
		while ( DiskList[index].Property.LockFlag == DISK_DEVICE_LOCK );

		DiskList[index].Property.LockFlag = DISK_DEVICE_LOCK;
		res = DiskList[index].ReadMultiSector( lun, lba_addr, nSector, buff);
		DiskList[index].Property.LockFlag = DISK_DEVICE_UNLOCK;

		return res;
	}

	return -1;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_ReadMultiStop
 *
 *	Description :	Terminate Sequential read operation.
 *
 * 	Parameter 	:	drv_type 	- disk type(NAND, MMC, HDD ...)
 *
 *  Returns		:	Returns "0" if success. Returns -1 if fail.
 *
 *****************************************************************************/
int		DISK_ReadMultiStop( int drv_type )
{
	int		res;
	int		index = 0;

	index = DISK_FindDisk( drv_type  );

	if ( index < 0 )
		return index;

	if ( DiskList[index].ReadMultiStop != NULL )
	{
		while ( DiskList[index].Property.LockFlag == DISK_DEVICE_LOCK );

		DiskList[index].Property.LockFlag = DISK_DEVICE_LOCK;
		res = DiskList[index].ReadMultiStop();
		DiskList[index].Property.LockFlag = DISK_DEVICE_UNLOCK;

		return res;
	}

	return -1;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_WriteMultiStart
 *
 *	Description :	Notifying Function that Sequential data writing operations will
 *					proceed at the future.
 *
 * 	Parameter 	:	drv_type	- disk type (NAND, MMC, HDD ...)
 * 					lba_addr	- Starting Sector Address
 * 					size		- Sector Size
 *
 *  Returns		:	Returns "0" if success. Returns -1 if fail.
 *
 *****************************************************************************/
int		DISK_WriteMultiStart( int drv_type, int lba_addr, int size )
{
	int		res;
	int		index = 0;

	index = DISK_FindDisk( drv_type );

	if ( index < 0 )
		return index;

	if ( DiskList[index].WriteMultiStart != NULL )
	{
		while ( DiskList[index].Property.LockFlag == DISK_DEVICE_LOCK );

		DiskList[index].Property.LockFlag = DISK_DEVICE_LOCK;
		res = DiskList[index].WriteMultiStart( lba_addr, size);
		DiskList[index].Property.LockFlag = DISK_DEVICE_UNLOCK;

		return res;
	}

	return -1;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_WriteMultiSector
 *
 *	Description :	write Sequential sector function after calling DISK_ReadMultiStart.
 *
 * 	Parameter 	:	drv_type 	- Drive Type(NAND, MMC, HDD ...)
 * 					lun			- Physical Drive Number. If there is two HDD disks in
 * 								  the set, the first is 0 and second is 1
 * 					lba_addr	- Sector Address
 * 					nSector		- Sector Count
 * 					buff		- buffer pointer
 *
 *  Returns		:	Returns "0" if success. Returns -1 if fail.
 *
 *  TODO		:	The lun parameter is experimental now. Fix "0"
 *
 *****************************************************************************/
int		DISK_WriteMultiSector( int drv_type, int lun, unsigned long lba_addr, unsigned short nSector, void *buff )
{
	int		res;
	int		index = 0;

	index = DISK_FindDisk( drv_type );

	if ( index < 0 )
		return index;

	if ( DiskList[index].WriteMultiSector != NULL )
	{
		while ( DiskList[index].Property.LockFlag == DISK_DEVICE_LOCK );

		DiskList[index].Property.LockFlag = DISK_DEVICE_LOCK;
		res = DiskList[index].WriteMultiSector( lun, lba_addr, nSector, buff);
		DiskList[index].Property.LockFlag = DISK_DEVICE_UNLOCK;

		return res;
	}

	return -1;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_WriteMultiStop
 *
 *	Description :	Terminate Sequential read operation.
 *
 * 	Parameter 	:	drv_type 	- disk type(NAND, MMC, HDD ...)
 *
 *  Returns		:	Returns "0" if success. Returns -1 if fail.
 *
 *****************************************************************************/
int		DISK_WriteMultiStop( int drv_type )
{
	int		res;
	int		index = 0;

	index = DISK_FindDisk( drv_type );

	if ( index < 0 )
		return index;

	if ( DiskList[index].WriteMultiStop != NULL )
	{
		while ( DiskList[index].Property.LockFlag == DISK_DEVICE_LOCK )

		DiskList[index].Property.LockFlag = DISK_DEVICE_LOCK;
		res = DiskList[index].WriteMultiStop();
		DiskList[index].Property.LockFlag = DISK_DEVICE_UNLOCK;

		return res;
	}

	return -1;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_HDReadSector
 *
 *	Description :	Read hidden sector(s) given drv_type media.
 *
 * 	Parameter 	:	drv_type 	- disk type (NAND, MMC, HDD ...)
 * 					page_addr	- Hidden Page Address (starts from "0")
 * 					count		- Page count
 * 					buff		- buffer pointer
 *
 *  Returns		:	Returns "0" if success. Returns "-1" if fail.
 *****************************************************************************/
int		DISK_HDReadSector( unsigned int drv_type, unsigned long page_addr, unsigned short count, unsigned char *buff )
{
	int		res;
	int		index = 0;

	index = DISK_FindDisk( drv_type );

	if ( index < 0 )
		return index;

	if ( DiskList[index].HDReadSector != NULL )
	{
		while ( DiskList[index].Property.LockFlag == DISK_DEVICE_LOCK );

		DiskList[index].Property.LockFlag = DISK_DEVICE_LOCK;
		res = DiskList[index].HDReadSector( page_addr, count, buff);
		DiskList[index].Property.LockFlag = DISK_DEVICE_UNLOCK;

		return res;
	}

	return -1;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_HDWriteSector
 *
 *	Description :	Write hidden sector(s) given drv_type media.
 *
 * 	Parameter 	:	drv_type 	- disk type(NAND, MMC, HDD ...)
 * 					page_addr	- Hidden Page Address (starts from "0")
 * 					count		- Write할 Page갯수
 * 					buff		- Write할 DATA가 있는 buffer pointer
 *
 *  Returns		:	Returns "0" if success. Returns "-1" if fail.
 *****************************************************************************/
int		DISK_HDWriteSector(unsigned int drv_type,  unsigned long page_addr, unsigned short count, unsigned char *buff )
{
	int		res;
	int		index = 0;

	index = DISK_FindDisk( drv_type );

	if ( index < 0 )
		return index;

	if ( DiskList[index].HDWriteSector != NULL )
	{
		while ( DiskList[index].Property.LockFlag == DISK_DEVICE_LOCK );

		DiskList[index].Property.LockFlag = DISK_DEVICE_LOCK;
		res = DiskList[index].HDWriteSector( page_addr, count, buff);
		DiskList[index].Property.LockFlag = DISK_DEVICE_UNLOCK;

		return res;
	}

	return -1;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_HDClearSector
 *
 *	Description :	Delete hidden sector(s) given drv_type media for the writing
 *					performance.
 *
 * 	Parameter 	:	drv_type 	- disk type(NAND, MMC, HDD ...)
 * 					start_page_addr - start page address
 * 					end_page_addr - end page address
 *
 *  Returns		:	Returns "0" if success. Returns "-1" if fail.
 *****************************************************************************/
int		DISK_HDClearSector(unsigned int drv_type, unsigned long start_page_addr, unsigned long end_page_addr )
{
	int		res;
	int		index = 0;

	index = DISK_FindDisk( drv_type );

	if ( index < 0 )
		return index;

	if ( DiskList[index].HDClearSector != NULL )
	{
		while ( DiskList[index].Property.LockFlag == DISK_DEVICE_LOCK );

		DiskList[index].Property.LockFlag = DISK_DEVICE_LOCK;
		res = DiskList[index].HDClearSector( start_page_addr, end_page_addr);
		DiskList[index].Property.LockFlag = DISK_DEVICE_UNLOCK;

		return res;
	}

	return -1;
}
/*******************************************************************************
 *	FUNCTION 	: 	DISK_Ioctl
 *
 *	Description :	Execute disk-specific functions.
 *					(Refer to IOCTL_FUNCTIONS enumerations at disk.h)
 *
 * 	Parameter 	:	drv_type 	- disk type (NAND, MMC, HDD ...)
 * 					function	- function number(DEV_INITIALIZE, DEV_GET_DISKINFO...)
 * 					param		- the parameter pointer which depends on function
 *
 *  Returns		:	Returns "0" if success. Returns "-1" if fail.
 *****************************************************************************/
int		DISK_Ioctl( unsigned int drv_type, int function, void *param )
{
	int		res;
	int		index = 0;

	index = DISK_FindDisk( drv_type );

	if ( index < 0 )
		return index;

	if ( DiskList[index].Ioctl != NULL )
	{
		if (( function != DEV_GET_POWER)&&(function != DEV_GET_INSERTED))
		{
			while ( DiskList[index].Property.LockFlag == DISK_DEVICE_LOCK );

			DiskList[index].Property.LockFlag = DISK_DEVICE_LOCK;
		}
		res = DiskList[index].Ioctl( function, param );
		if (( function != DEV_GET_POWER)&&(function != DEV_GET_INSERTED)){
			DiskList[index].Property.LockFlag = DISK_DEVICE_UNLOCK;
		}

		return res;
	}

	return -1;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_GetTotalDiskCount
 *
 *	Description :	Returns number of disk in the systen
 *
 * 	Parameter 	:	NONE
 *
 *  Returns		:	number of disk
 *****************************************************************************/
int		DISK_GetTotalDiskCount( void )
{
	return (sizeof(DiskList) / sizeof(tDeviceDriver) - 1);
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_SetState
 *
 *	Description :	Set Mount state variable
 *
 * 	Parameter 	:	drv_type - disk type(NAND, MMC, HDD ... )
 * 					state	 - disk mount status
 * 						(DISK_MOUNT_STATE:
 * 							DISK_STATE_NOTMOUNTED	: not tried to mount yet.
 * 							DISK_STATE_MOUNTSUCCEED : mount successed. able to use.
 * 							DISK_STATE_MOUNTERR		: mount fail.
 *
 *  Returns		:	Change the status of mounting.
 *  				if fail, it returns negative value.
 *
 *  Note		: 	we currently use two status - DISK_STATE_MOUNTSUCCEED, DISK_STATE_MOUNTERR
 *****************************************************************************/
int		DISK_SetState(DISK_DEVICE disk,	DISK_MOUNT_STATE	state)
{
	int	res;
	res	=	DISK_FindDisk(disk);
	if ( res < 0) return res;

	DiskList[res].Property.MountState=	state;

	return 1;
}
/*******************************************************************************
 *	FUNCTION 	: 	DISK_GetState
 *
 *	Description :	Returns Mount state
 *
 * 	Parameter 	:	drv_type - disk type(NAND, MMC, HDD ... )
 * 					state	 - the status of disk
 *
 *  Returns		:
 * 						DISK_MOUNT_STATE:
 * 							DISK_STATE_NOTMOUNTED	: not tried to mount yet.
 * 							DISK_STATE_MOUNTSUCCEED : mount successed. able to use.
 * 							DISK_STATE_MOUNTERR		: mount fail.
 *
 *  Note		: 	we currently use two status - DISK_STATE_MOUNTSUCCEED, DISK_STATE_MOUNTERR
 *****************************************************************************/
int		DISK_GetState(DISK_DEVICE disk)
{
	int	res;
	res	=	DISK_FindDisk(disk);
	if ( res < 0)
	{
		//return res;
		return	DISK_STATE_MOUNTERR;
	}

	return DiskList[res].Property.MountState;
}
/*******************************************************************************
 *	FUNCTION 	: 	DISK_SetDiskPartitionIndex
 *
 *	Description :	Set the PartitionIndex of given disk.
 *
 * 	Parameter 	:	disk 	 - disk type(NAND, MMC, HDD ... )
 * 					index	 - disk partition index value
 *
 *  Returns		:	Returns partition index if succeed.
 *  				Returns negative value if fail.
 *****************************************************************************/
int		DISK_SetDiskPartitionIndex(	DISK_DEVICE disk, int	index)
{
	int	res;
	res	=	DISK_FindDisk(disk);
	if ( res < 0) return res;
	DiskList[res].Property.PartitionIndex	=	index;

	return 0;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_GetDiskPartitionIndex
 *
 *	Description :	Get the PartitionIndex of given disk.
 *
 * 	Parameter 	:	disk 	 - disk type(NAND, MMC, HDD ... )
 * 					index	 - disk partition index value
 *
 *  Returns		:	Returns partition index if succeed.
 *  				Returns negative value if fail.
 *****************************************************************************/
int		DISK_GetDiskPartitionIndex(	DISK_DEVICE disk)
{
	int	res;
	res	=	DISK_FindDisk(disk);
	if ( res < 0) return res;

	return DiskList[res].Property.PartitionIndex;
}
/*******************************************************************************
 *	FUNCTION 	: 	DISK_SetBusyState
 *
 *	Description :	Sets busy status of given disk.
 *
 * 	Parameter 	:	disk 	 - disk type(NAND, MMC, HDD ... )
 * 					busyFlag	-	Status of Busy
 * 						DISK_STATE_FREE	: idle
 * 						DISK_STATE_BUSY	: busy
 *
 *  Returns		:	Sets busy status as given busyFlag and returns zero if succeed.
 *  				Returns negative value if fail.
 *****************************************************************************/
int		DISK_SetBusyState(DISK_DEVICE disk, DISK_BUSY_STATE busyFlag)
{
	int	res;
	res	=	DISK_FindDisk(disk);
	if ( res < 0) return res;
	DiskList[res].Property.BusyState	=	busyFlag;
	return 0;
}
/*******************************************************************************
 *	FUNCTION 	: 	DISK_GetBusyState
 *
 *	Description :	Gets busy status of given disk.
 *
 * 	Parameter 	:	disk 	 - disk type(NAND, MMC, HDD ... )
 * 					busyFlag	-	Status of Busy
 * 						DISK_STATE_FREE	: idle
 * 						DISK_STATE_BUSY	: busy
 *
 *  Returns		:	Gets busy status as given disk type and returns it if succeed.
 *  				Returns negative value if fail.
 *****************************************************************************/
int		DISK_GetBusyState(DISK_DEVICE disk)
{
	int	res;
	res	=	DISK_FindDisk(disk);
	if ( res < 0)
	{
		return res;
	}

	return DiskList[res].Property.BusyState;
}

/*******************************************************************************
 *	FUNCTION 	: 	DISK_GetDeviceName
 *
 *	Description :	Gets Device Name
 *
 * 	Parameter 	:	disk 	 - disk type(NAND, MMC, HDD ... )
 * 					busyFlag	-	Status of Busy
 * 						DISK_STATE_FREE	: idle
 * 						DISK_STATE_BUSY	: busy
 *
 *  Returns		:	Gets device name as given disk type and returns it if succeed.
 *  				Returns negative value if fail.
 *****************************************************************************/
unsigned char *DISK_GetDeviceName(DISK_DEVICE disk)
{
	int	res;

	res	=	DISK_FindDisk(disk);
	if ( res < 0)
	{
		return UnknownDeviceName;
	}

	return DiskList[res].Property.Name;
}
