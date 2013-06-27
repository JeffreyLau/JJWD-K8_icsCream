/*****************************************************************************/
// File System refer for K-FileSystem
//	FSAPP.C
//	Copyright	2003				Telechips, Inc.
//
// 2004. 06. 4
/*****************************************************************************/
#ifndef __FSAPP_H__
#define __FSAPP_H__

#if defined(_LINUX_)
#include <fwdn/file.h>
#include <fwdn/Disk.h>
#elif defined(_WINCE_)
#include "file.h"
#include "Disk.h"
#endif

#ifndef CLEAR
#define CLEAR				0
#endif
#ifndef DIRTY
#define DIRTY				1
#endif

//=============================================================================
//*
//*
//*                       [ EXTERNAL DEFINATION ]
//*
//*
//=============================================================================
#define					_NOTMUSICFILE				-1
//	DEFINITION FOR MAXIMUM HANDLE OF FILES OR DIRECTORIES
#define					MAX_HANDLE					5
#define					MAX_FD						MAX_HANDLE
#define					MAX_DIR						2
//	DEFAULT FILE FORMAT
#define					DEFAULT_ROOT_ENTRY_COUNT	512		// normal number of the root entry count
#define					HIDDEN_SIZE					31		// NAND, UFD etc... But NOT HDD

#define					ENTRY_SIZE					32    // 1 entry size [byte]
#define					ENTRY_CHUNK					11
#define					ENTRY_BUFFER_SIZE			(ENTRY_CHUNK * ENTRY_SIZE)

#define					FSAPP_MAX_PART_NUMBER		10		//driveInfo[FSAPP_MAX_PART_NUMBER] / physicalDrvType[FSAPP_MAX_PART_NUMBER]

//=============================================================================
//*
//*
//*                       [ EXTERNAL VARIABLE DEFINE ]
//*
//*
//=============================================================================

extern int				totalHdlr;
extern int				gDiskIdleTime;

extern FDstruc			fd[MAX_FD];
extern HANDLERstruc		fhandler[MAX_HANDLE];
extern FDIRENTstruc		fdir[MAX_DIR];
extern unsigned char	physicalDrvType[FSAPP_MAX_PART_NUMBER];

//=============================================================================
//*
//*
//*                       [ FUCTIONS DEFINE ]
//*
//*
//=============================================================================
extern void				FAT_InitDriveInfo( void );
extern int				FAT_InitFS( void );
extern void				FAT_InitVariable( void );
extern int				FAT_MountDrive(int drvTypeID, unsigned int lun);
extern int				FAT_UnmountDrive(int drvTypeID);
extern void				FAT_InitializeForFlexibility(unsigned char valueOfHandle, unsigned char *entryBufferPointer, HANDLERstruc *fhandlerStruc,
													unsigned char valueOfFd, FDstruc *fdStruc, unsigned char valueOfDir, FDIRENTstruc *fdirStruc);

extern int 				FSAPP_InitDiskDevice( int DeviceID );
extern int  			FSAPP_GetMaxCopySize( void );
extern unsigned char 	*FSAPP_GetFileBuffer( void );
extern void				FSAPP_InitializeFS( void );
extern unsigned long	FSAPP_GetDiskSector( unsigned char drvType, DISKINFOstruc *disk );

extern int				FSAPP_Get_part_id(unsigned char drvType, unsigned char mount);

extern int				FSAPP_disk_RWsector(int drvTypeID, unsigned char drv_num,unsigned long LBA_addr, unsigned short nSector, void *buff, unsigned char RWflag);
extern int				FSAPP_diskIoctl_DEV_GET_MAXMULTISECTOR(int drvTypeID, unsigned short *nSector);
extern int				FSAPP_diskIoctl_DEV_ERASE_INIT(int drvTypeID, unsigned char secPerClus, unsigned long dataStartSec);
extern int				FSAPP_diskIoctl_DEV_ERASE_BLOCK(int drvTypeID, unsigned long currCluster, unsigned long contentFAT);
extern int				FSAPP_diskIoctl_DEV_ERASE_CLOSE(int drvTypeID);
extern int				FSAPP_diskIoctl_DEV_SET_MULTISECTOR(int drvTypeID, unsigned short *max_multi_sector);
extern int				FSAPP_diskIoctl_DEV_TELL_DATASTARTSECTOR(int drvTypeID, unsigned long int data_start_sector);
extern int				FSAPP_diskIoctl_DEV_GET_MAX_SECTOR_PER_BLOCK(int drvTypeID, unsigned short int *SpB);
extern int				FSAPP_diskIoctl_DEV_STOP_TRANSFER(int drvTypeID);

extern int				FSAPP_DISK_WriteMultiStart(int drvTypeID, int lba_addr, int size);
extern int				FSAPP_DISK_WriteMultiSector(int drvTypeID, int lun, unsigned long lba_addr, unsigned short nSector, void *buff);
extern int				FSAPP_DISK_ReadMultiStart(int drvTypeID, int lba_addr, int size);
extern int				FSAPP_DISK_ReadMultiSector(int drvTypeID, int lun, unsigned long lba_addr, unsigned short nSector, void *buff);
extern int				FSAPP_DISK_ReadMultiStop(int drvTypeID);
extern int				FSAPP_DISK_WriteMultiStop(int drvTypeID);

extern int				FSAPP_physicalStorage_HDD(int drvTypeID);

extern int				FSAPP_ReadSector_Common(int drvTypeID, int lun, unsigned long lba_addr, unsigned short nSector, void *buff);
extern int				FSAPP_WriteSector_Common(int drvTypeID, int lun, unsigned long lba_addr, unsigned short nSector, void *buff, unsigned char FormatFlag);

extern int				FSAPP_getUsedClus_PS(int drvTypeID, int partID, unsigned FAT1_PhySector, unsigned short nSector);
extern int				FSAPP_cleanFATcache_PS(int drvTypeID, int partID, unsigned long FAT2_PhySector, unsigned long FAT_Sector, unsigned char *sbuffer);
extern int				FSAPP_changeFATcache_PS(int drvTypeID, int partID, unsigned long FAT1_PhySector, unsigned int Offset_Sector, unsigned char *fatBuff);
extern int				FSAPP_fatWriteClus_PS(int drvTypeID, unsigned short nCount, unsigned long sector_addr, unsigned short nSector, unsigned short multi_sector, int multi_byte, unsigned char *temp_buff);
extern unsigned int		FSAPP_FormatClear_PS(int drvTypeID, unsigned long sectorPerFAT, unsigned int nSector, unsigned char *pBuff, void *SecBuff);

extern unsigned int		FSAPP_FormatDrive( DISK_DEVICE diskDevice, unsigned int *multiPartition_SectorSize, unsigned int *validFAT);

extern int				FSAPP_FormatRootEntry_PS(int drvTypeID, int halfEntryNum, unsigned nSector, unsigned char writeVolume, void *pBuff, unsigned char *BS_VolLab);

extern unsigned int		FSAPP_decide_MakeMBR(int drvTypeID, unsigned char SecPerClus, unsigned int *numOfSecPR, unsigned int *validFAT, void *pBuff, struct _DISK_INFO *disk);
extern unsigned char	FSAPP_decide_DrvNum(int drvTypeID);
extern int				Initialize_FileSystem(DISK_DEVICE mDeviceNum, int partID);



extern unsigned char	*FSAPP_SetVolumeLabel(unsigned int partID, void* VolLab);
extern unsigned char	*FSAPP_GetVolumeLabel(unsigned int partID);

extern unsigned int FSAPP_PartitionWrite(DISK_DEVICE diskDevice, unsigned int partitionID, unsigned int lba, void *pBuff, unsigned short nSector);
extern unsigned int FSAPP_PartitionRead(DISK_DEVICE diskDevice, unsigned int partitionID, unsigned int lba, void *pBuff, unsigned short nSector);
#endif
