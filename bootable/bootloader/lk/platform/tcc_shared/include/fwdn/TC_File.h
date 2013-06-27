/****************************************************************************
 *   FileName    : TC_File.h
 *   Description : File System Abstraction Layer
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/

#ifndef	__TC_File_H__
#define	__TC_File_H__ 

#if defined(_LINUX_)
	#include <fwdn/file.h>
	#include <fwdn/Disk.h>
#elif defined(_WINCE_)
	#include "file.h"
	#include "Disk.h"
#endif

#ifndef _UINT8_
	#define _UINT8_
	typedef unsigned char UINT8;
#endif

typedef	int	TC_STAT;
typedef	unsigned long long	TC_U64;
typedef	unsigned int		TC_U32;
typedef	unsigned short		TC_U16;
typedef	unsigned char		TC_U8;

typedef	signed long long	TC_S64;
typedef	signed int			TC_S32;
typedef	signed short		TC_S16;
typedef	signed char			TC_S8;

typedef	char *			TC_STR8;
#ifndef _CHAR_
	#define _CHAR_
	typedef char CHAR;
#endif

typedef struct {
	unsigned char	second;
	unsigned char	minute;
	unsigned char	hour;
	unsigned char	day;
	unsigned char	date;
	unsigned char	month;
	unsigned short	year;
} TC_DATETIME;


typedef unsigned long		_dev_t;
typedef unsigned short		_ino_t;
typedef long				_off_t;
typedef unsigned long		_mode_t;
typedef unsigned long		_time_t;

typedef struct// _stat
{
	_dev_t			st_dev;			/* device */
	_ino_t			st_ino;			/* inode */
	_mode_t			st_mode;		/* protection */
	short			st_nlink;		/* number of hard links */
	short			st_uid;			/* user ID of owner */
	short			st_gid;			/* group ID of owner */
	_dev_t			st_rdev;		/* device type (if inode device) */
	_off_t			st_size;		/* total size, in bytes */
	_time_t			st_atime;		/* time of last access */
	_time_t			st_mtime;		/* time of last modification */
	_time_t			st_ctime;		/* time of last change */
} stat;

 	#define	TC_O_RDONLY	0x0000
	#define	TC_O_WRONLY	0x0001
	#define	TC_O_RDWR	0x0002
	#define	TC_O_APPEND	0x0008
	#define	TC_O_CREAT	0x0100
	#define	TC_O_TRUNC	0x0200

/* File creation permissions for open */
/* Note: OCTAL */
	#define	TC_A_READ		0000400	/* Write permitted      */
	#define	TC_A_WRITE		0000200 	/* Read permitted. (Always true anyway)*/
	#define 	TC_SUCCESS		0
	typedef	TC_S32		TC_DRIVE;
	typedef	int			TC_DIR;
	typedef	int			TC_PDIR;
	typedef	FDIRENTstruc TC_DS;
	typedef	DIRENTstruc	TC_DENTRY;

	#define	TC_ISDIRSP(X)		(0)

	#define	TC_StartDrive		1/* DISK_DEVICE_INTERNAL */
	#define	TC_DriveNum			uNDRIVES

	#define	TC_INTERNAL_DRIVE(X)	(0)

	#define	TC_UFD_DEVICE		DISK_DEVICE_UHP
	#define	TC_HDD_DEVICE		DISK_DEVICE_HDD
	#define	TC_NAND_DEVICE		DISK_DEVICE_NAND
	#define	TC_TRIFLASH_DEVICE	DISK_DEVICE_TRIFLASH
	#define	TC_SDMMC_DEVICE	DISK_DEVICE_MMC
	#define	TC_ISINTDRIVE(X)		((X) == TC_StartDrive)

	#define	TC_Set_Drive(X, Y)	((X) = (Y))
	#define	TC_Inc_Drive(X, Y)	((X) += (Y))
	#define	TC_Get_DriveNo(X)	((X))

	#define	TC_Get_Attribute(X)	((X)->type)
	#define	TC_Get_Cluster(X, Y)	((X)->startCluster)
	#define	TC_Get_Size(X, Y)		((X)->fileSize)
	#define	TC_Get_LFN(X)		((X)->lFileName)
	#define	TC_Get_SFName(X)	((X)->sFileName)
	#define	TC_Get_SFext(X)		((X)->sFileName+9)

	#define	TC_A_RDONLY	    FILE_READ_ONLY
	#define	TC_A_HIDDEN	    FILE_HIDDEN
	#define	TC_A_SYSTEM	    FILE_SYSTEM
	#define	TC_A_VOLUME	    FILE_VOLUME_ID
	#define	TC_A_DIRENT		FILE_DIRECTORY
	#define	TC_A_ARCHIVE	FILE_ARCHIVE
	#define	TC_A_NORMAL	    FILE_NORMAL

	#define	TC_MAX_PATH		255
	#define	TC_MAX_DIR			MAX_DIR

#define	TC_ISERR(X)			((signed)(X) < 0)
#define	TC_ISOK(X)			((signed)(X) >= 0)

#define	TC_ISHERR(X)		((signed)(X) < 0)
#define	TC_ISHOK(X)			((signed)(X) >= 0)

#define	TC_ISRWERR(X)		((signed)(X) <= 0)

#define	TC_SEEK_SET		0
#define	TC_SEEK_CUR	1
#define	TC_SEEK_END	2

#define	TC_LOWLEVEL_YES	1
#define	TC_LOWLEVEL_NO		0

#define UNDEFINED_HANDLE    (-1)

TC_STAT TC_Open(char *name, TC_U32 uFlag, TC_U32 uMode, TC_U32 uDirNum);
TC_S32	TC_Read(TC_S32 iHandle, void *pBuff, TC_S32 iCount);
TC_S32	TC_Write(TC_S32 iHandle, void *pBuff, TC_S32 iCount);
TC_S32	TC_Seek(TC_S32 iHandle, TC_S32 iOffset, TC_S32 iOrigin);
TC_STAT	TC_Close(TC_S32 iHandle);
TC_S32	TC_Length(TC_S32 iHandle);
TC_STAT	TC_DeleteIndex(TC_U32 uIndex, TC_U32 uDirNum);
TC_STAT	TC_Make_Dir(TC_STR8 pName, TC_U32 uDirNum);
TC_STAT	TC_SyncDrives(TC_S32 uDriveNo, TC_U32 uNum);
TC_U32	TC_Get_FileIndex(TC_S32 iHandle, TC_U32 uDirNum);
TC_STAT	TC_Set_Current_Dir(TC_S32 iPartID, TC_STR8 pPath, TC_U32 uDirNum);
TC_STAT	TC_CloseNGetFileProperty(TC_S32 iHandle, unsigned int *iFileProperty);

//ENHANCED_DELETION_SPEED

 #endif
/* end of file */
