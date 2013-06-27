/*****************************************************************************/
// 
// Definition for FAT Application using Telechips Software.
//
//	Copyright	2008				Telechips, Inc.
//
/*****************************************************************************/

#include "FSAPP.h"

#ifndef __FAT_H__
#define __FAT_H__

//=============================================================================
//*
//*
//*                       [ ERROR CODE ENUMERATION ]
//*
//*
//=============================================================================
enum 
{
	FS_SUCCESS = 0,
	FS_FAIL,	

	ERR_FS_NOT_VALID_MBR,
	ERR_FS_NOT_READ_MBR,
	ERR_FS_NOT_FORMATTED,
	ERR_FS_NOT_DELETE_FILE,
	ERR_FS_NOT_DELETE_DIR,
	ERR_FS_NOT_FIND_PARTITION_INFO,
	ERR_FS_NOT_VALID_FAT_TYPE,
	ERR_FS_NOT_VALID_READ_BOOTSECTOR,
	ERR_FS_INVALID_SECTOR_SIZE,
	ERR_FS_NOT_VALID_CLUSTER_PARAMETER,
	ERR_FS_NOT_READ_SECTOR,
	ERR_FS_NOT_WRITE_SECTOR,	
	ERR_FS_NOT_GET_LOGDRV_INFO,	
	ERR_FS_NOT_GET_PREVIOUS_DIR,
	ERR_FS_NOT_GET_NEXT_CLUSTER,
	ERR_FS_NOT_GET_PREV_CLUSTER,
	ERR_FS_NOT_GET_EMPTY_CLUSTER,	
	ERR_FS_NOT_GET_INFO_FILEORDIR,
	ERR_FS_NOT_GET_TOTAL_FILEORDIR,
	ERR_FS_NOT_GET_PARENT_DIR_CLUSTER,	
	ERR_FS_NOT_FIND_DIR_CLUSTER_WITHDIRNAME,
	ERR_FS_NOT_CREATE_DIRECTORY,
	ERR_FS_NOT_DIRECTORY_CLUSTER,
	ERR_FS_NOT_UPDATE_ENTRY,
	ERR_FS_NOT_GET_FILEINFO_WITHFILENAME,
	ERR_FS_NOT_OPEN_FAT_CHCHE,
	ERR_FS_NOT_FLUSH_FAT_CACHE,
	ERR_FS_NOT_SAME_FAT1_FAT2,
	ERR_FS_NOT_READY_VIRTUAL_FAT1_FORSMC,
	ERR_FS_NOT_READ_FROM_VIRTUAL_FAT1_FORSMC,
	ERR_FS_NOT_WRITE_FAT_CONTENT_TO_VIRTUAL_FAT1_FORSMC,
	ERR_FS_FAILED_READ_SECTOR,
	ERR_FS_FAILED_FORMAT,
	ERR_FS_PARAMETER_GET_DRV_PARTITON,
	ERR_FS_NOT_VALID_PARAMETER
};


//=============================================================================
//*
//*
//*                       [ EXTERNAL DEFINATION ]
//*
//*
//=============================================================================
#define		FAT_BOOTSIG		0xAA55

// Some useful cluster numbers
#define			FAT_MSDOSFSROOT				0	/* cluster 0 means the root dir */
#define			FAT_CLUST_FREE				0	/* cluster 0 also means a free cluster */
#define			FAT_CLUST_RSRVD				0xfffffff6	/* reserved cluster range */
#define 		FAT_CLUST_BAD				0xfffffff7	/* a cluster with a defect */
#define 		FAT_CLUST_EOFS				0xfffffff8	/* start of eof cluster range */
#define 		FAT_CLUST_EOFE				0xffffffff	/* end of eof cluster range */

#define 		FAT12_MASK					0x00000fff	/* mask for 12 bit cluster numbers */
#define 		FAT16_MASK					0x0000ffff	/* mask for 16 bit cluster numbers */
#define 		FAT32_MASK					0x0fffffff	/* mask for FAT32 cluster numbers */

#define 		FAT12_EOFS					0x0FF8
#define 		FAT16_EOFS					0xFFF8
#define 		FAT32_EOFS					0x0FFFFFF8
// Partition Type used in the partition record
#define			PART_TYPE_UNKNOWN			0x00
#define			PART_TYPE_FAT12				0x01
#define			PART_TYPE_DOSFAT16			0x04
#define			PART_TYPE_EXTDOS			0x05
#define			PART_TYPE_FAT16				0x06
#define			PART_TYPE_FAT32				0x0B
#define			PART_TYPE_FAT32LBA			0x0C
#define			PART_TYPE_FAT16LBA			0x0E
#define			PART_TYPE_EXTDOSLBA			0x0F

#define			FS_MAX_SHIFT_FACTOR			17

#define			DRIVE0						0
#define			MAX_MULTI_SECTOR			32		// 32 SECTORS
#define			MAX_MULTI_BYTE				16384	// 16 [Kbyte] = 512[byte] * 32
#define			MAX_SECTOR_SIZE				4096	// [byte] = maximum bytes per sector
#define			MAX_CLUSTER_SIZE			65536	// 512(byte) * 128(sec)

#define			FAT_BOOTSIG					0xAA55
#define			EXTENDED_BOOTSIG			0x29

#define			FAT12						2
#define			FAT16						1
#define			FAT32						0

#define			FAT_DIR_EMPTY				0x00	// directory entry is free. never been used.
#define			FAT_DIR_E5					0x05	// value for 0xE5. because 0xE5 is a real char in KANJI
#define			FAT_DIR_DELETED				0xE5	// this directory entry is free.

#define			ATTR_NORMAL					0x00	// normal file
#define			ATTR_READ_ONLY				0x01	// file is read only
#define			ATTR_HIDDEN					0x02	// file is hidden
#define			ATTR_SYSTEM					0x04	// file is a system file
#define			ATTR_VOLUME_ID				0x08	// entry is a volume label
#define			ATTR_DIRECTORY				0x10	// entry is a directory name
#define			ATTR_ARCHIVE				0x20	// file is new or modified
#define			ATTR_LONG_NAME				0x0F	// this is a long file name entry

#define			LCASE_BASE					0x08	// filename base in lower case
#define			LCASE_EXT					0x10	// filename extension in lower case

#define			DIR_ENTRY_PER_SECTOR		0x10	// 16 = 512/32

#define			LAST_LONG_ENTRY				0x40	// mask for the last dir entry
#define			LDIR_ORD_MAX				0x3F	// Max. count of LDIR_Ord

#define			LDIR_CHAR_PER_ENTRY			13		// Number of character of a long dir entry.
#define			LDIR_MAX_CHAR				255		// max. number of character of the long file name
#define			MAX_ROOT_ENTRY_COUNT		1024	// max. number of the root entry count

#define			FAT_CACHE_SECTOR			3		// 3 sectors = 512 * 3 [byte]

// LOCK FLAG OF THE FILE SYSTEM TEMPORARY BUFFER
#define			LOCK_BUFF					0x0001
#define			UNLOCK_BUFF					0x0000
#define			DIRTY_BIT					0x0100

//	MAXIMUM NUMBER OF PARTITIONS
#define			FS_MAX_PART_NUMBER			FSAPP_MAX_PART_NUMBER

#define			START_YEAR					1980	// Year 1980

#define			FILE_OP_WRITE				0x00010000	// write file
#define			FILE_OP_OPEN				0x00000001	// opened file
#define			FILE_OP_CH_SIZE				0x00020000	// changed file size
#define			FILE_OP_UP_ENT				0x00100000   // update short entry. else create new entry

#define			ADDITION					1
#define			SUBTRACTION					2
// special option
#define _NEW_WRITE_ENTRY


//=============================================================================
//*
//*
//*                       [ STRUCT DEFINE ]
//*
//*
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
/////////////////////////    MBR    ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FOR EACH PARTITION ENTRY INFORMATION(16 BYTES) STRUCTURE
#if defined(_WINCE_)
#pragma pack(1)
typedef struct _PARTITION_INFO			// length 16 bytes partion entry information
#elif defined(_LINUX_)
typedef struct _PARTITION_INFO			// length 16 bytes partion entry information
#endif
{

 	unsigned char		boot_id;					// Bootable? 0=no , 128(0x80) = yes
 	unsigned char		begin_head;					// beginning head number
 	unsigned char		begin_sector;				// beginning sector number
 	unsigned short		begin_cylinder;				// 10bit number [high 2bit], with high 2bits put in begin sector
 	unsigned char		system_id;					// Operating System type indicator code
 	unsigned char		end_head;					// ending head number
	unsigned char		end_sector;					// ending sector numer
 	unsigned short		end_cylinder;				// 10bit number [high 2bit] , with high 2bits put in end sector
 	unsigned long		relative_first_sector;		// first sector relative to start of disk
 	unsigned long		number_sector_partition;	// number of sectors in partion	

 	
#if defined(_WINCE_)
}PARTINFOstruc;
#pragma pack()
#elif defined(_LINUX_)
} __attribute__((packed)) PARTINFOstruc;
#endif



// FOR EACH PARTITION AREA ALL DATA STRUCTURE
#if defined(_WINCE_)
#pragma pack(1)
struct _MBR_INFO
#elif defined(_LINUX_)
struct _MBR_INFO
#endif
{

	PARTINFOstruc		parts[4];
	unsigned short		signature;					// two signature bytes (2 bytes)

	
#if defined(_WINCE_)
};
#pragma pack()
#elif defined(_LINUX_)
} __attribute__((packed));
#endif
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
/////////////////////////    PBR    ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// COMMON BPB SPEC. FOR FAT16/32.	(36 BYTES)
#if defined(_WINCE_)
#pragma pack(1)
struct _BPB
#elif defined(_LINUX_)
struct _BPB	
#endif
{

	unsigned char		BS_jmpBoot[3];				// jump instruction to boot code. 0xE9xxx or 0xEBxx90
	char				BS_OEMName[8];				// OEM name and version
	unsigned short		BPB_BytsPerSec;				// Count of bytes per sector. default 512
	unsigned char		BPB_SecPerClus;				// Number of sectors per cluster
	unsigned short		BPB_RsvdSecCnt;				// Number of reserved sectors
	unsigned char		BPB_NumFATs;				// Count of FAT data on the volume. default 2
	unsigned short		BPB_RootEntCnt;				// Number of the Root directory entries. FAT32 = 0
	unsigned short		BPB_TotSec16;				// 16 bit total count of sector < 65536. FAT32 = 0
	unsigned char		BPB_Media;					// media discriptor. 0xF8 --> "Fixed Disk"
	unsigned short		BPB_FATSz16;				// Number of sectors per FAT
	unsigned short		BPB_SecPerTrk;				// sector per track
	unsigned short		BPB_NumHeads;				// Number of heads
	unsigned long		BPB_HiddSec;				// count of hidden sectors
	unsigned long		BPB_TotSec32;				// total count of sectors on the volume.

	
#if defined(_WINCE_)
};
#pragma pack()
#elif defined(_LINUX_)
} __attribute__((packed));
#endif



// EXTENDED BS(BOOT SECTOR) SPEC. FOR FAT16/32.		(26 BYTES)
#if defined(_WINCE_)
#pragma pack(1)
struct _EXTENDED_BS
#elif defined(_LINUX_)
struct _EXTENDED_BS
#endif
{

	unsigned char		BS_DrvNum;					// drive number : HDD(0x80), FDD(0x00)
	unsigned char		BS_Reserved1;				// reserved(used by Windows NT)
	unsigned char		BS_BootSig;					// Extended boot signature (0x29)
	unsigned char		BS_VolID[4];				// Volume serial number
	char				BS_VolLab[11];				// Volume label
	char				BS_FilSysType[8];			// FS type string "FAT12","FAT16","FAT"

	
#if defined(_WINCE_)
};
#pragma pack()
#elif defined(_LINUX_)
} __attribute__((packed));
#endif



// BPB SPEC. FOR FAT32 ONLY.		(54 BYTES)
#if defined(_WINCE_)
#pragma pack(1)
struct _BPB_FAT32
#elif defined(_LINUX_)
struct _BPB_FAT32
#endif
{

	unsigned long		BPB_FATSz32;				// Number of sectors per FAT for FAT32
	unsigned short		BPB_ExtFlags;				// extended flags
	unsigned short		BPB_FSVer;					// FAT file system version.
	unsigned long		BPB_RootClus;				// starting cluster number of the root directory.
	unsigned short		BPB_FSInfo;					// sector number of FSINFO structure. Usually 1.
	unsigned short		BPB_BkBootSec;				// backup boot sector. Usually 6.
	unsigned char		BPB_Reserved[12];			// reserved for future expansion. Must be all zero.
	unsigned char		BS_DrvNum;
	unsigned char		BS_Reserved1;
	unsigned char		BS_BootSig;
	unsigned char		BS_VolID[4];
	char				BS_VolLab[11];				// Volume label
	char				BS_FilSysType[8];			// Reserved 12 bytes for future expansion. all zeros.

	
#if defined(_WINCE_)
};
#pragma pack()
#elif defined(_LINUX_)
} __attribute__((packed));
#endif



// BS(BOOT SECTOR) AND BPB(BIOS PARAMETER BLOCK) STRUCTURE FOR FAT32
#if defined(_WINCE_)
#pragma pack(1)
typedef struct _BSBPB_INFO
#elif defined(_LINUX_)
typedef struct _BSBPB_INFO
#endif
{

	struct _BPB			BPB_data;					// Common BPB data (25 bytes)
	struct _BPB_FAT32	BPB_FAT32;					// Extended BPB Info. for FAT32 only. (28 bytes)
	struct _EXTENDED_BS	BS_extended; 				// Extended BS for FAT12/16  (26 bytes)


#if defined(_WINCE_)
}BSBPBstruc;
#pragma pack()
#elif defined(_LINUX_)
} __attribute__((packed)) BSBPBstruc;
#endif



// BS(BOOT SECTOR) AND BPB(BIOS PARAMETER BLOCK) STRUCTURE FOR FAT12/16
#if defined(_WINCE_)
#pragma pack(1)
typedef struct _BSBPB1216_INFO
#elif defined(_LINUX_)
typedef struct _BSBPB1216_INFO
#endif
{

	struct _BPB			BPB_data;					// Common BPB data (25 bytes)
	struct _EXTENDED_BS	BS_extended;	 			// Extended BS for FAT12/16  (26 bytes)


#if defined(_WINCE_)
}BSBPB1216struc;
#pragma pack()
#elif defined(_LINUX_)
} __attribute__((packed)) BSBPB1216struc;
#endif
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
/////////////////////    Reserved Area(FSInfo)    /////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FAT32 FSINFO SECTOR STRUCTURE & BACKUP BOOT SECTOR.
#if defined(_WINCE_)
#pragma pack(1)
typedef struct _FSINFO_SECTOR				// for FAT32 FSInfo
#elif defined(_LINUX_)
typedef struct _FSINFO_SECTOR				// for FAT32 FSInfo
#endif
{


	unsigned long		FSI_LeadSig;				// lead signature for FSInfo. Usually 0x41615252
	unsigned char		FSI_Reserved1[480];			// reserved field. initial value is all zeros.
	unsigned long		FSI_StrucSig;				// usually 0x61417272 in spec.
	unsigned long		FSI_Free_Count;				// (free cluster count on the volume) <= (volume cluster count)
	unsigned long		FSI_Nxt_Free;				// next available cluster number. typically last cluster no.
	unsigned char		FSI_Reserved2[12];			// reserved for future expansion. all zeros.
	unsigned long		FSI_TrailSig;				// trail signature = 0xAA550000 for FSInfo Sector.


#if defined(_WINCE_)
}FSINFOstruc;
#pragma pack()
#elif defined(_LINUX_)
} __attribute__((packed)) FSINFOstruc;
#endif
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
/////////////////////    Logical Drv Structure    /////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#if defined(_WINCE_)
#pragma pack(1)
typedef struct _DRIVE_INFO
#elif defined(_LINUX_)
typedef struct _DRIVE_INFO
#endif
{

	struct _PARTITION_INFO	part;					// partition info : 18 Bytes
	struct _BSBPB_INFO		BS;

	unsigned char		Drv_Type;					// 0: HDD, 1: MMC/SD, 2: USB_DRV, 3: NAND_DRV	
	unsigned char		FAT_Type;					// 0: FAT32	1: FAT16	2:FAT32
	unsigned short		LUN;						// logical unit number	
	unsigned short		part_index;					// logical partition or drive index(number).
	unsigned long		nCluster;					// Temporary Variable for Disk Size
	unsigned long		FAT1StartSector;			// FAT1 Start Sector
	unsigned long		FAT2StartSector;			// FAT2 Start Sector
	unsigned long		FATSize;					// FAT Size
	unsigned long		FirstDataSector;			// Cluster Start Sector	2TH CLUSTER
	unsigned long		DirStartSector;				// Root Start Sector
	unsigned long		DataSec;					// Total Data Sector
	unsigned long		CountofClusters;			// Total Count of clusters
	unsigned long		ClusterSize;				// Cluster Byte Size
	unsigned short		ClusterShift;				// Cluster Shift Factor Size	
	unsigned short		BytsPerSecShift;			// Sector Bytes Shift Factor Size	
	unsigned short		SecPerClusShift;			// Sectors Per Cluster Shift Factor Size
	unsigned long		UsedClusters;				// Used total data capacity [byte]

	
#if defined(_WINCE_)
}DRIVE_INFOstruc;
#pragma pack()
#elif defined(_LINUX_)
} __attribute__((packed)) DRIVE_INFOstruc;
#endif
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
//////////////////////    File Entry Structure    /////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FAT DIRECTORY ENTRY STRUCTURE(32 BYTES DATA).
#if defined(_WINCE_)
#pragma pack(1)
typedef struct _FAT_DIR_ENTRY				// FAT 32 bytes for directory entry
#elif defined(_LINUX_)
typedef struct _FAT_DIR_ENTRY				// FAT 32 bytes for directory entry
#endif
{

	unsigned char		Name[8];					// file name field
	unsigned char		Extension[3];				// file extension field
	unsigned char		Attr;						// File attributes
	unsigned char		NTRes;						// reserved for Windows NT VFAT lower case flags.
	unsigned char		CrtTimeTenth;				// Millisecond stamp at file creation time. 0-199/(2 sec)
	unsigned short		CrtTime;					// Time file was created.
	unsigned short		CrtDate;					// Date file was created.
	unsigned short		LstAccDate;					// Last access date.
	unsigned short		FstClusHI;					// High word of this entry's first cluster number. zero for FAT12/16.
	unsigned short		WrtTime;					// Time of last write.
	unsigned short		WrtDate;					// Date of last write.
	unsigned short		FstClusLO;					// Low word of this entry's first cluster number.
	unsigned long		FileSize;					// this file's size in bytes.

	
#if defined(_WINCE_)
}ENTRY_INFO;
#pragma pack()
#elif defined(_LINUX_)
} __attribute__((packed)) ENTRY_INFO;
#endif



// FAT LONG DIRECTORY ENTRY STRUCTURE.
#if defined(_WINCE_)
#pragma pack(1)
typedef struct _FAT_LONG_DIR_ENTRY			// FAT 32 bytes for long directory entry
#elif defined(_LINUX_)
typedef struct _FAT_LONG_DIR_ENTRY			// FAT 32 bytes for long directory entry
#endif
{

	unsigned char		LDIR_Ord;					// order of this entry in the sequence of long dir entries.
	char				LDIR_Name1[10];				// char 1-5 of the long-name sub-component
	unsigned char		LDIR_Attr;					// Attributes - must be ATTR_LONG_NAME(0x0F)
	unsigned char		LDIR_Type;					// If zero, this entry is a sub-component of a long name.
	unsigned char		LDIR_Chksum;				// checksum of name in the short dir entry
	char				LDIR_Name2[12];				// char 6-11 of the long-name sub-component
	unsigned short		LDIR_FstClusLO;				// must be zero.
	char				LDIR_Name3[4];				// char 12-13 of the long-name sub-component

	
#if defined(_WINCE_)
}FAT_LDIR;
#pragma pack()
#elif defined(_LINUX_)
} __attribute__((packed)) FAT_LDIR;
#endif



// FAT32 DIRECTORY ENTRY.
typedef struct _FAT32DIRENT
{
	union
	{
		ENTRY_INFO		s;
		FAT_LDIR		l;
	}entry;
}FAT32DIRENTstruc;



//	FILE OR DIRECTORY ENTRY POSITION STRUCTURE
#if defined(_WINCE_)
#pragma pack(1)
typedef struct _FAT_ENTRY_POS
#elif defined(_LINUX_)
typedef struct _FAT_ENTRY_POS
#endif
{

	int					entryOffset;
	int					entrySectorOffset;
	unsigned long		dirCluster;
	short				entryCount;


#if defined(_WINCE_)
}ENTPOSstruc;
#pragma pack()
#elif defined(_LINUX_)
} __attribute__((packed)) ENTPOSstruc;
#endif
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
//////////////////////    FAT Cache Structure    //////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FAT CACHE CONTROL
#if defined(_WINCE_)
#pragma pack(1)
typedef struct _FAT_CACHE
#elif defined(_LINUX_)
typedef struct _FAT_CACHE
#endif
{

	unsigned int		InCache;
	int					drvTypeID;
	int					partID;
	unsigned short		status;

	
#if defined(_WINCE_)
}FATCACHEstruc;
#pragma pack()
#elif defined(_LINUX_)
} __attribute__((packed)) FATCACHEstruc;
#endif
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
////////     Lock/Unlock struc. of Entry Sector bufferEntry     ///////////////
///////////////////////////////////////////////////////////////////////////////
// ENTRY SECTOR BUFFER CONTROL
#if defined(_WINCE_)
#pragma pack(1)
typedef struct _SECTOR_BUFF
#elif defined(_LINUX_)
typedef struct _SECTOR_BUFF
#endif
{

	unsigned int		InSector;					// sector buffer address
	int					part_id;					// partition index
	unsigned short		status;						// lock or dirty bit


#if defined(_WINCE_)
}SECBUFFstruc;
#pragma pack()
#elif defined(_LINUX_)
} __attribute__((packed)) SECBUFFstruc;
#endif
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



struct _FDIRENT;
struct _FD;
struct _DISK_INFO;

//=============================================================================
//*
//*
//*                       [ EXTERNAL VARIABLE DEFINE ]
//*
//*
//=============================================================================
//FAT File System Global Variables.
extern int						fat_total_partition;							// total partition counter
extern unsigned short			fat_valid_partition;							// primary & logical partition counter.

extern unsigned char			max_handle;
extern unsigned char			max_fd;
extern unsigned char			max_dir;
extern HANDLERstruc				*fHandler;
extern FDstruc					*fD;
extern FDIRENTstruc				*fDir;

extern unsigned int				fat_sbuffer[];
extern unsigned short			fat_cache[(512 * FAT_CACHE_SECTOR) >> 1];		// fat cache buffer = 512 bytes * 3

extern DRIVE_INFOstruc			driveInfo[FS_MAX_PART_NUMBER];					// current drive information
extern FATCACHEstruc			fatCache;
extern SECBUFFstruc				secBuffer;
 
extern unsigned char			*entry_buffer;								// entry chunk buffer

// FILE DATE & TIME DEFINITION
extern volatile unsigned short	file_year;										// initial value = 1980
extern volatile unsigned char	file_month;										// initial value = 1
extern volatile unsigned char	file_day;										// initial value = 1
extern volatile unsigned char	file_hours;										// initial value = 0
extern volatile unsigned char	file_min;										// initial value = 0
extern volatile unsigned char	file_sec;										// initial value = 0


//=============================================================================
//*
//*
//*                       [ FUCTIONS DEFINE ]
//*
//*
//=============================================================================
extern int				FAT_GetNextCluster(int drvTypeID, int partID, unsigned long curr_cluster, unsigned long *next_cluster, void *fat_buff);
extern int				FAT_SetNextCluster(int drvTypeID, int partID, unsigned long curr_cluster, unsigned long next_cluster, void *fat_buff);
extern unsigned long	FAT_ReadEntrySector(int drvTypeID, int partID, unsigned long dir_cluster, unsigned long SectorIndex, void *buff);
extern unsigned long	FAT_ReadDirSector(int drvTypeID, int partID, unsigned long dir_cluster, unsigned long SectorIndex, void *buff, int *preSkip);
extern int				FAT_FindFileEntryWithID(int drvTypeID, unsigned long dirCluster, int file_id, struct _FDIRENT *pFDir);
extern int				FAT_FindFileEntryWithEntryOffset(int drvTypeID, int partID, unsigned long dirCluster, int file_id, struct _FDIRENT *pFDir);
extern int				FAT_FindDirEntryWithID(int drvTypeID, unsigned long dirCluster, int dir_id, struct _FDIRENT *pFDir);
extern int				FAT_FindDirEntryWithEntryOffset(int drvTypeID, int partID, unsigned long dirCluster, int dir_id, struct _FDIRENT *pDir);
extern int				FAT_GetDirContents(int drvTypeID, int partID, unsigned long start_cluster, int *file_count, int *subdir_count); // 0923
extern int				FAT_GetParentDirEntry(int drvTypeID, int partID, struct _FDIRENT *pDir, unsigned long dir_cluster);
extern int				FAT_FindDirEntryWithCluster(int drvTypeID, int partID, unsigned long start_cluster, unsigned long dir_cluster, struct _FDIRENT *pDir);
extern int				FAT_ReadCluster(int drvTypeID, int partID, unsigned short nSector, unsigned long currCluster, unsigned long offset, unsigned char *pBuff, int nRead, int clusterSize);
extern int				FAT_WriteCluster(int drvTypeID, int partID, unsigned short nSector, unsigned long currCluster, unsigned long offset, unsigned char *pBuff, int nWrite, int clusterSize);
extern int				FAT_GetEmptyCluster(int drvTypeID, int partID, unsigned long curr_cluster, unsigned long *Empty_Clus, void *fat_buff);
extern int				FAT_MakeSubDir(int drvTypeID, int partID, char *pName, unsigned long dirCluster, int option);
extern int				FAT_GetFreeEntryCount(int drvTypeID, int partID, unsigned long dirCluster, int *entryOffset,
												int *entrySectorOffset, unsigned long *entryCluster, int entryNeeds );
extern unsigned long	FAT_GetFileStartCluster(int drvTypeID, int partID, unsigned long curr_cluster);
extern int				FAT_WriteEntrySector(int drvTypeID, int partID, unsigned long dir_cluster, int entrySectorOffset, void *buff);
extern unsigned long	FAT_WriteSector(int drvTypeID, int partID, unsigned long dir_cluster, int SectorIndex, void *buff);
extern int				FAT_ClearFileCluster(int drvTypeID, int partID, unsigned long curr_cluster, void *buff);
extern int				FAT_DeleteFileEntry(int drvTypeID, int partID, unsigned long dir_cluster, int sector_offset, int entry_offset, void *pBuff);
extern int				FAT_UpdateFATCache(void *buff);
extern int				FAT_MakeEmptyEntry(int drvTypeID, int partID, unsigned long uwStrCluster, int *npEntryOffset, int *npEntrySecOffset, unsigned long *lpEntryCluster, int entryNeeds );
extern int				FAT_MakeNewFD(int drvTypeID, struct _FDIRENT *pDir, char *pName, struct _FD *pFD);
extern int				FAT_MountDrive(int drvTypeID, unsigned int lun);
extern int				FAT_UnmountDrive(int drvTypeID);

extern int				FAT_MakeShortEntryFile(int drvTypeID, int partID, struct _FD *pFD, unsigned char *dir_ent);
extern int				FAT_FindShortFile(int drvTypeID, int partID, unsigned long dir_cluster, char *pName, struct _FDIRENT *pFDir);
extern int 				FAT_readdir(int drvTypeID, int partID, unsigned long dir_cluster, struct _FDIRENT *pFDir, int mode, void *entry_buff);
extern int 				FAT_GetEntryData(int drvTypeID, int partID, ENTPOSstruc *pEntPos, void *pBuff);
extern unsigned char 	FAT_ChkSum(unsigned char *pFcbName);
extern int				FAT_FormatFAT(int drvTypeID, unsigned int partID, unsigned char SecPerClus,
										void *SecBuff, struct _DISK_INFO *disk, unsigned char writeVolume);
extern int				FAT_FormatFAT32(int drvTypeID, unsigned int partID, unsigned char SecPerClus,
										void *SecBuff, struct _DISK_INFO *disk, unsigned char writeVolume);					
extern void				FAT_InitVariable(void);
extern void				FAT_InitDriveInfo(void);
extern unsigned long	FAT_ArithmeticOperationForCluster(int partID, int nCluster, char arithOperator);
extern int				FAT_UpdateEntryCountToCache(int drvTypeID, unsigned long dir_cluster, int nCount);
extern void				FAT_SetReservedSector(unsigned long sector_addr);
extern unsigned long	FAT_ReadTotalSectorFromHidden(unsigned long *);
extern int				FAT_GetUsedClusters(int drvTypeID, int partID);
extern int				FAT_MakeDotEntry(int drvTypeID, int partID, unsigned long start_cluster, unsigned long parent_cluster);
extern int				FAT_ClearCluster(int drvTypeID, int partID, unsigned long curr_cluster);
extern unsigned long	FAT_ReadSector(int drvTypeID, int partID, unsigned long dir_cluster, unsigned long SectorIndex, void *buff);

extern int				_FAT_MakeNewFD(struct _FDIRENT *pDir, char *pName, struct _FD *pFD);


extern int 				drv_WriteSector(int drvTypeID, unsigned long LBA_addr, unsigned short nSector, void *buff);
extern int 				drv_ReadSector(int drvTypeID, unsigned long LBA_addr, unsigned short nSector, void *buff);
extern void 			lockSecBuff(void);
extern void 			unlockSecBuff(void);
extern void				lockFATBuff(void);
extern void				unlockFATBuff(void);
extern unsigned short	makeWrtDate(unsigned short year, unsigned char month, unsigned char day);
extern unsigned short 	makeWrtTime(unsigned char hours, unsigned char min, unsigned char sec);

extern int				check_free_entry(int drvTypeID, int partID, unsigned long dir_cluster, int *entryOffset, int *entrySectorOffset, unsigned long *entryCluster, int nEntry);
extern int				str_cmpu(void *sA, void *sB);
extern int				str_len16(void *pString);
extern int 				find_DirEntry(int drvTypeID, int partID, unsigned long startCluster, unsigned char *pFileName);
extern int				makeShortName(void *pN, void *sN, int num_tail, unsigned short type);
extern int				makeDirEntry(void *sName, unsigned char attr, unsigned long entry_cluster, unsigned char *dir_ent);

extern int				FAT_getUsedClus_Common(int drvTypeID, int partID, unsigned short nSector, unsigned long FAT1_PhySector);
extern int				FAT_fatCache_Common(int drvTypeID, int partID, unsigned long FAT2_PhySector, unsigned long FAT_Sector, unsigned short nSector, unsigned char *sbuffer, unsigned char RWflag);
extern unsigned int		FAT_Format_ClearCommon(int drvTypeID, int sectorPerFAT, unsigned long nSector, unsigned char *pBuff, void *SecBuff, unsigned char multiFlag);
extern int				FAT_FormatRE_Common(int drvTypeID, int halfEntryNum, unsigned nSector, unsigned char writeVolume, void *pBuff, unsigned char *BS_VolLab);
						
extern unsigned int		FAT_makeMBR(int drvTypeID, unsigned int *numOfSecPR, unsigned int *validFAT, unsigned char SecPerClus, void *pBuff, struct _DISK_INFO *disk);

extern int				FAT_CheckDirContentsExist(int drvTypeID, int partID, unsigned long start_cluster, int *file_count, int *subdir_count);
extern short			getLongEntryName(int drvTypeID, int partID, struct _FDIRENT *pFDir, unsigned long sector_offset, int entry_offset, struct _FAT_LONG_DIR_ENTRY *entry, void *buff);
extern unsigned long	cluster2sector(int partID, unsigned long cluster);

extern int FDISK_PartitionWrite(int drvTypeID, unsigned int partitionID, unsigned int relativeLBA, void *pBuff, unsigned short nSector);
extern int FDISK_PartitionRead(int drvTypeID, unsigned int partitionID, unsigned int relativeLBA, void *pBuff, unsigned short nSector);

#endif	// __FAT_H__
