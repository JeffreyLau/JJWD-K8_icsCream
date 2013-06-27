/****************************************************************************
 *   FileName    : Disk.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#ifndef __DISK_H__
#define __DISK_H__

/*******************************************************************************
 * DISK Interface Error Code Macro
 ******************************************************************************/
#define 	_ERR(x)			(0x80000000 | (x))
#define		ENOTSUPPORT		_ERR(0x10)		// Function does not support
#define		EINITFAIL 		_ERR(0x11)		// Device Initialization Failed

/*****************************************************************************
 *	disk device type enumeration value
 *****************************************************************************/
typedef enum
{
	DISK_DEVICE_NONE,
	DISK_DEVICE_HDD,
	DISK_DEVICE_NAND,
	DISK_DEVICE_NAND_HD,
	DISK_DEVICE_UHP,
	DISK_DEVICE_TRIFLASH,
	DISK_DEVICE_MMC,
	DISK_DEVICE_MS,
	DISK_DEVICE_TRIFLASH_HD,
	#ifdef SDMMC_EXTERNAL_HIDDEN_STORAGE_INCLUDE
		DISK_DEVICE_MMC_HD,
	#endif
	MAX_DEVICE_NUM
}DISK_DEVICE;

#define DISK_DEVICE_INTERNAL 1

/*****************************************************************************
 *	disk device property and sub-property enumeration value
 *****************************************************************************/
typedef enum
{
	DISK_DEVICE_UNLOCK=0,
	DISK_DEVICE_LOCK
}DISK_LOCK_FLAG;

typedef enum
{
	DISK_STATE_NOTMOUNTED,
	DISK_STATE_MOUNTSUCCEED,
	DISK_STATE_MOUNTERR
}DISK_MOUNT_STATE;

typedef enum	
{
	DISK_STATE_FREE,
	DISK_STATE_BUSY
}DISK_BUSY_STATE;

typedef enum	
{
	DISK_STATE_INIT,
	DISK_STATE_POWERON,
	DISK_STATE_POWEROFF,
	DISK_STATE_STANDBY,
	DISK_STATE_IDLE,
	DISK_STATE_SLEEP,
	DISK_STATE_RESET
}DISK_POWER_STATE;

typedef enum
{
	DISK_MSC_DRV_NOT_SUPPORT,
	DISK_MSC_DRV_SUPPORT
}DISK_MSC_DRV_STATE;

typedef enum
{
	DISK_MOUNT_TYPE_INTERNAL,
	DISK_MOUNT_TYPE_EXTERNAL,
	DISK_MOUNT_TYPE_MAXIUM
}DISK_MOUNT_TYPE_STATE;

typedef	struct
{
	const char				*Name;
	DISK_LOCK_FLAG			LockFlag;
	DISK_DEVICE				DiskType;
	DISK_MOUNT_STATE		MountState;
	DISK_BUSY_STATE			BusyState;
	DISK_MSC_DRV_STATE		MSCDrvSupport;
	DISK_MOUNT_TYPE_STATE	DrvMountType;
	int						PartitionIndex;
}DISK_PROPERTY;

/*******************************************************************************
 * DISK Interface Function Type Definitions
 ******************************************************************************/
typedef int (*tDeviceRwFunctions)(int, unsigned long, unsigned short, void *);

typedef int (*tDeviceWriteMultiStartFunctions)(unsigned long, unsigned long);
typedef int (*tDeviceWriteMultiFunctions)(int, unsigned long, unsigned short, void *);
typedef int (*tDeviceWriteMultiStopFunctions)(void);

typedef int (*tDeviceReadMultiStartFunctions)(unsigned long, unsigned long);
typedef int (*tDeviceReadMultiFunctions)(int, unsigned long, unsigned short, void *);
typedef int (*tDeviceReadMultiStopFunctions)(void);

typedef unsigned long (*tDeviceHiddenRWFunctions)(unsigned long , unsigned short, unsigned char *);
typedef int (*tDeviceHiddenClearPageFunctions)(unsigned long , unsigned long);

typedef int (*tDeviceIoctlFunctions)(int, void *);

/*******************************************************************************
 * DISK Interface Function Definitions
 ******************************************************************************/
typedef struct DeviceDriverStruct
{
	DISK_PROPERTY						Property;
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
}tDeviceDriver;

/*******************************************************************************
 * DISK Ioctl Function List ( Enumeration Value )
 *
 * 		DEV_INITIALIZE
 * 		Initialize Variable , Register and Hardware
 *
 * 		DEV_GET_DISKINFO
 * 		Get the environmant variables like head, cylinder, sector ...
 *
 * 		DEV_FORMAT_DISK
 * 		low level format command ( if it necessary )
 * 		
 * 		DEV_ERASE_INIT
 * 		prepare erasing command
 *
 * 		DEV_ERASE_BLOCK
 * 		erase sector command
 *
 * 		DEV_ERASE_CLOSE
 * 		finish erasing command
 *
 * 		DEV_WRITEBACK_ON_IDLE
 * 		flush data cache command while system is in idle state
 ******************************************************************************/

#define	YES					1
#define	NO					0

#define	TC_LOWLEVEL_YES		1
#define	TC_LOWLEVEL_NO		0

/*******************************************************************************
 * DISK Ioctl DEV_GET_DISKINFO Function Parameter structure
 ******************************************************************************/
typedef struct ioctl_diskinfo_t
{
	unsigned short	head;
	unsigned short	cylinder;
	unsigned short	sector;
	unsigned short	sector_size;
	unsigned int	Total_sectors;	
}ioctl_diskinfo_t;

/*******************************************************************************
 * DISK Ioctl DEV_ERASE_INIT Function Parameter structure
 ******************************************************************************/
typedef struct ioctl_diskeraseinit_t
{
	unsigned short		sector_per_cluster;
	unsigned long		data_start_sector;
}ioctl_diskeraseinit_t;

/*******************************************************************************
 * DISK Ioctl DEV_ERASE_BLOCK Function Parameter structure
 ******************************************************************************/
typedef struct ioctl_diskerase_t
{
	unsigned long	current_cluster;
	unsigned long	content_fat;
}ioctl_diskerase_t;

/*******************************************************************************
 * DISK Ioctl DEV_HIDDEN_CLEAR_PAGE Function Parameter structure
 ******************************************************************************/
// typedef struct ioctl_diskhdclear_t {
// 	unsigned long	start_page;
// 	unsigned long	end_page;
// }ioctl_diskhdclear_t;

/*******************************************************************************
 * DISK Ioctl DEV_HIDDEN_READ/WRITE_PAGE Function Parameter structure
 ******************************************************************************/
typedef struct ioctl_diskhdread4_t
{
	unsigned long	start_page;
	unsigned long	page_offset;
	unsigned long	read_size;
	unsigned char	*buff;
}ioctl_diskhdread4_t;

/*******************************************************************************
 * DISK Ioctl DEV_BOOTCODE_READ/WRITE_PAGE Function Parameter structure
 ******************************************************************************/
typedef struct ioctl_diskrwpage_t
{
	unsigned long	start_page;
	unsigned long	rw_size;
	unsigned int	hidden_index;
	unsigned char	*buff;
}ioctl_diskrwpage_t;

/*******************************************************************************
 * DISK Ioctl Disk Hidden partition Function Parameter structure
 ******************************************************************************/
typedef struct ioctl_diskhidden_t
{
	unsigned long	ulIndex;
	unsigned long	ulTotalSector;
}ioctl_diskhidden_t;



/*******************************************************************************
 * DISK Interface Function Definitions
 ******************************************************************************/

int		DISK_FindDisk(int drv_type);
int		DISK_ReadSector(int drv_type, int lun, unsigned long lba_addr, unsigned short nSector, void *buff);
int		DISK_WriteSector(int drv_type, int lun, unsigned long lba_addr, unsigned short nSector, void *buff);

int		DISK_ReadMultiStart(int drv_type, int lba_addr, int size);
int		DISK_ReadMultiSector(int drv_type, int lun, unsigned long lba_addr, unsigned short nSector, void *buff);
int		DISK_ReadMultiStop(int drv_type);

int		DISK_WriteMultiStart(int drv_type, int lba_addr, int size);
int		DISK_WriteMultiSector(int drv_type, int lun, unsigned long lba_addr, unsigned short nSector, void *buff);
int		DISK_WriteMultiStop(int drv_type);

int		DISK_HDReadSector(unsigned int drv_type, unsigned long page_addr, unsigned short count, unsigned char *buff);
int		DISK_HDWriteSector(unsigned int drv_type,  unsigned long page_addr, unsigned short count, unsigned char *buff);
int		DISK_HDClearSector(unsigned int drv_type, unsigned long start_page_addr, unsigned long end_page_addr);

int		DISK_Ioctl(unsigned int drv_type, int function, void *param);
int		DISK_GetTotalDiskCount(void);
int 	DISK_GetDiskType(int index);
int 	DISK_GetDiskTypeByPartID(int PartID );
int		DISK_GetSupportMSCDrive(int index );
int		DISK_GetDiskMountType(int index );
int		DISK_SetState(DISK_DEVICE disk,	DISK_MOUNT_STATE state);
int		DISK_GetState(DISK_DEVICE disk);
int		DISK_SetDiskPartitionIndex(DISK_DEVICE disk, int index);
int		DISK_GetDiskPartitionIndex(DISK_DEVICE disk);
int		DISK_SetBusyState(DISK_DEVICE disk, DISK_BUSY_STATE busyFlag);
int		DISK_GetBusyState(DISK_DEVICE disk);

extern unsigned char *DISK_GetDeviceName(DISK_DEVICE disk);
extern unsigned int DISK_GetDeviceStatus(int iDeviceNum,int iUSBMode,int iExtDevice);


/*******************************************************************************
 * DISK List Array pre-definition ( in disk.c)
 ******************************************************************************/
extern tDeviceDriver DiskList[];
extern const unsigned int DISK_DefaultDriveType;

typedef enum
{
	/* Do Not Change below functions*/
	DEV_INITIALIZE	=	0,					// Triflash
	DEV_MOUNT,
	DEV_GET_DISKINFO,						// Triflash
	DEV_FORMAT_DISK,						// Triflash
	DEV_ERASE_INIT,
	DEV_ERASE_BLOCK,
	DEV_ERASE_CLOSE,
	DEV_WRITEBACK_ON_IDLE,
	/* You can add new function from here */
	DEV_HIDDEN_READ_PAGE_4,					// Triflash
	DEV_GET_MAXMULTISECTOR,
	DEV_SET_POWER,
	DEV_GET_POWER,
	DEV_GET_MAX_SECTOR_PER_BLOCK,			// Triflash
	DEV_GET_INSERTED,						// Triflash
	DEV_GET_INITED,							// Triflash
	DEV_GET_WRITE_PROTECT,					// Triflash
	DEV_GET_PLAYABLE_STATUS,				// Triflash
	DEV_TELL_DATASTARTSECTOR,
	DEV_CHECK_CRC_NANDBOOT_IMAGE_ROM,
	DEV_GET_DISK_SIZE,
	DEV_GET_HIDDEN_SIZE,					// Triflash
	DEV_GET_HIDDEN_COUNT,
	DEV_GET_SUPPORT_FAT_FORMAT,
	DEV_FORCE_FLUSH_CACHE_DATA,
	DEV_SET_ALIGEN_CACHE,
	DEV_RECOVERY_TCBOOT,
	DEV_SET_MULTISECTOR,
	/* Be supported by Triflash DRV. */
	DEV_GET_BOOTCODE_SIZE,					// Triflash (ONLY)
	DEV_BOOTCODE_READ_PAGE,					// Triflash (ONLY)
	DEV_BOOTCODE_WRITE_PAGE,				// Triflash (ONLY)
	DEV_HIDDEN_READ_PAGE,					// Triflash (ONLY)
	DEV_HIDDEN_WRITE_PAGE,					// Triflash (ONLY)
	DEV_SERIAL_PROCESS,						// Triflash (ONLY)
	DEV_SET_REMOVED,						// Triflash (ONLY)
	DEV_GET_PREV_STATUS,					// Triflash (ONLY)
	DEV_STOP_TRANSFER,						// Triflash (ONLY)	
	DEV_END_OF_FUNCTION,
	DEV_POWER_ON_RESET
}IOCTL_FUNCTIONS;

#endif	// __DISK_H__
/* end of file */

