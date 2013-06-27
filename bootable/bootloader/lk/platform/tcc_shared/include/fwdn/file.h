/*****************************************************************************/
// File Defition for Telechips Software.
//
//	Copyright	2008				Telechips, Inc.
//
/*****************************************************************************/

#ifndef __FILE_H__
#define __FILE_H__

//=============================================================================
//*
//*
//*                       [ EXTERNAL DEFINATION ]
//*
//*
//=============================================================================
#define					ROOT_DIR_ID						0xFFFF0000

// FILE ACCESS MODE
#define					FILE_ACCESSMASK					0x07
#define					FILE_READ						0x00			/* File Read Only */
#define					FILE_WRITE						0x01			/* File Write Only */
#define					FILE_REWRITE					0x02			/* File Read/Write */
#define					FILE_COPY						0x10

#define					FILE_OEXIST						0x00010000		/* Open Existing File */
#define					FILE_OTRUNC						0x00020000		/* Truncate Existing File */
#define					FILE_OAPPEND					0x00040000		/* Open Exisging File at EOF */
#define					FILE_OCREAT						0x00100000		/* Create Unexisting File */

// FILE ATTRIBUTE
#define					FILE_NORMAL						0x00
#define					FILE_READ_ONLY					0x01
#define					FILE_HIDDEN						0x02
#define					FILE_SYSTEM						0x04
#define					FILE_VOLUME_ID					0x08
#define					FILE_DIRECTORY					0x10
#define					FILE_ARCHIVE					0x20

// FILE CREATE OPTION
#define					FILE_NO_TILDE					0x0100			/* do not make file with numeric-tail */
#define					FILE_NO_CMP						0x0200

// REFERENCE POINT OF FILE POINTER.
#define					FSEEK_SET						0	// file beginning
#define					FSEEK_CUR						1	// current FP position
#define					FSEEK_END						2	// file ending

#define					FILE_FIND_FIRST					0x00
#define					FILE_FIND_NEXT					0x01
#define					FILE_SKIP_LFN					0x02
#define					FILE_SAVE_S_ENTRY				0x04

//=============================================================================
//*
//*
//*                       [ ERROR CODE ENUMERATION ]
//*
//*
//=============================================================================
typedef enum
{
	ERR_FS_FAIL					=		-1,
	ERR_FS_FULL_ROOT_ENTRY		=		-2
} FS_ERROR;

//=============================================================================
//*
//*
//*                       [ STRUCT DEFINE ]
//*
//*
//=============================================================================
typedef struct _HANDLER	// FILE HANDLER STRUCTURE.
{
	int					lock;				// used handle
	unsigned long		usage;				// Usage of this handle. Read, Write, Copy etc...
	unsigned char		*pEntry;			// entry chunk pointer
	short				nEntry;				// number of entry in entry chunk data. 32 [byte]
}HANDLERstruc;

typedef struct _FD	// FILE DESCRIPTOR STRUCTURE.
{
	int					part_id;			// current partition index
	int					file_id;			// file id in a parent directory. NOT used anymore.
	unsigned long		dirCluster;			// number of the start cluster of a parent directory

	char				lFileName[258];		// long file or dir name
	char				sFileName[13];		// short file or dir name
	long				fileSize;			// file size [byte].
	unsigned long		startCluster;		// number of the start cluster of a file or directory

 	int					pre_clusterCnt;
	 unsigned long		cntCluster;

	int					entryOffset;		// short entry offset(slot) in current sector
	int					entrySectorOffset;	// current sector number in current directory
	unsigned long		entryCluster;		// just debugging
	
	unsigned long		clusterSize;		// cluster size of the current partition [byte]
	unsigned short		type;				// file type or attribute. (READ ONLY, HIDDEN, SYSTEM, DIRECTORY, ARCHIVE) 
	unsigned long		mode;				// Access Mode of file. (READ, WRITE, REWRITE)
	unsigned long		offset;				// file pointer [byte]
	int					refCnt;				// referenct flag. lower 16 bit : referenct count, upper 16 bit : dirty bit
}FDstruc;

typedef struct _FDIRENT	// FILE OR DIRECTORY ENTRY STRUCTURE.
{
	char				lFileName[258];		// long file name
	char				sFileName[13];		// short file name
	unsigned char		ref;
	long				fileSize;			// file size [byte]
	unsigned short		type;				// file attribute.
	
	unsigned long		startCluster;		// cluster of the current file or directory
	unsigned long		parentCluster;		// cluster of the parent directory
	
	int					entryOffset;		// short entry offset(slot) in current sector
	int					entrySectorOffset;	// current sector number in current directory
	unsigned long		entryCluster;		// just debugging
	
	int					file_cnt;			// file count. only valid when FILE_Refresh() was excuted.
	int					subdir_cnt;			// sub-directory(folder) count. only valid when FILE_Refresh() was excuted.
	int					part_id;			// current partition index
	short				entryCount;			// total entry count
}FDIRENTstruc;

// SHORT DIRECTORY ENTRY STRUCTURE(32 BYTES DATA).
#if defined(_WINCE_)
#pragma pack(1)
typedef struct _SFN_DIR_ENTRY		// FAT 32 bytes for directory entry
#elif defined(_LINUX_)
typedef struct _SFN_DIR_ENTRY		// FAT 32 bytes for directory entry
#endif
{
	unsigned char		Name[8];
	unsigned char		Extension[3];
	unsigned char		Attr;
	unsigned char		NTRes;
	unsigned char		CrtTimeTenth;
	unsigned short		CrtTime;
	unsigned short		CrtDate;
	unsigned short		LstAccDate;
	unsigned short		FstClusHI;
	unsigned short		WrtTime;
	unsigned short		WrtDate;
	unsigned short		FstClusLO;
	unsigned long		FileSize;
#if defined(_WINCE_)
}SFN_INFO;
#pragma pack()
#elif defined(_LINUX_)
} __attribute__((packed)) SFN_INFO;
#endif


#if defined(_WINCE_)
#pragma pack()
typedef struct _DIRENT	// FILE OR DIRECTORY ENTRY STRUCTURE. - for FILE_ReadDir()
#elif defined(_LINUX_)
typedef struct _DIRENT	// FILE OR DIRECTORY ENTRY STRUCTURE. - for FILE_ReadDir()
#endif
{
	SFN_INFO			sEntry;				// short entry slot
	char				*lFileName;			// pointer of the long file name
	char				*sFileName;			// pointer of the short file name
	unsigned char		LFNflag;			// 0: no LFN, 1: has LFN
	unsigned short		type;				// file attribute. file or directory
	unsigned long		startCluster;		// cluster of the directory or file
	unsigned int		offset;
	int					part_id;			// current partition index
#if defined(_WINCE_)
}DIRENTstruc;
#pragma pack()
#elif defined(_LINUX_)
}DIRENTstruc;
#endif

#ifndef _DISKINFOstruc_
#define _DISKINFOstruc_
#if defined(_WINCE_)
#pragma pack(1)
typedef struct _DISK_INFO	// DISK INFORMATION STRUCTURE. - for File_Format()
#elif defined(_LINUX_)
typedef struct _DISK_INFO	// DISK INFORMATION STRUCTURE. - for File_Format()
#endif
{
	unsigned short		head;
	unsigned short		cylinder;
	unsigned short		sector;
	unsigned short		sector_size;
#if defined(_WINCE_)
}DISKINFOstruc;
#pragma pack()
#elif defined(_LINUX_)
} __attribute__((packed)) DISKINFOstruc;
#endif
#endif //#ifndef _DISKINFOstruc_


#if defined(_WINCE_)
#pragma pack(1)
typedef struct _DRV_INFO	// PARTITION (OR DRIVE) SIMPLE INFORMATION
#elif defined(_LINUX_)
typedef struct _DRV_INFO	// PARTITION (OR DRIVE) SIMPLE INFORMATION
#endif
{
	unsigned char		Drv_Type;			// HDD_DRV, MMC_DRV, USB_DRV, NAND_DRV	
	unsigned char		FAT_Type;			// FAT32, FAT16, FAT32
	unsigned short		part_id;			// logical partition or drive index(number).
	unsigned short		BytsPerSec;			// Count of bytes per sector. normally 512
	unsigned long		ClusterSize;		// Cluster Byte Size
	unsigned long		CountofClusters;	// total number of clusters
	unsigned long		UsedClusters;		// Used total data capacity [byte]
#if defined(_WINCE_)
}DRVINFOstruc;
#pragma pack()
#elif defined(_LINUX_)
} __attribute__((packed)) DRVINFOstruc;
#endif

//=============================================================================
//*
//*
//*                       [ FUCTIONS DEFINE ]
//*
//*
//=============================================================================
extern FDstruc			*FILE_pGetFD(int nFD);
extern FDIRENTstruc		*FILE_pGetDIR(int dir_num);
extern FDIRENTstruc		*FILE_pGetFDir(int nFDir);
extern int				FILE_InitSystem(void);
extern int				FILE_Write(int nHandle, void *pBuff, int size);									// write a file
extern int				FILE_OpenDirByEntryId(int drvTypeID, int partID, int dir_id, int dir_num);	// open directory.
extern int				FILE_MakeDir(char *pName, int dir_num);											// make sub-directory
extern int				FILE_MakeDirWithHDAttr(char *pName, int dir_num);								// make sub-directory with HIDDEN attribute
extern int				FILE_MakeHideDir(char *pName, int dir_num);
extern int				FILE_Create(char *pName, unsigned short type, int dir_num);						// create a file.
extern int				FILE_CreateLFN(void *pName, unsigned short type, int dir_num);					// create a file that name has unicode
extern int				FILE_RemoveByEntryId(int dir_num, int file_id);									// remove a file
extern int				FILE_Change2ParentDir(int dir_num);												//change directory to parent directory
extern int				FILE_Refresh(int dir_num);														// reload current directory information
extern unsigned long	FILE_TotalDiskCluster(int drvTypeID);
extern unsigned long	FILE_AvailableDiskCluster(int drvTypeID);										// get free size of the current drive
extern unsigned long	FILE_GetClusterSize(int drvTypeID);
extern int				FILE_OpenName(char *pName, char *mode, int dir_num);
extern int				FILE_OpenDirName(char *pName, char *mode, int dir_num);
extern int				FILE_ReadDir(int mode, int dir_num, FDIRENTstruc *pFDir, DIRENTstruc *pEnt);
extern void				FILE_InitFDIRstruc(FDIRENTstruc *pFDir);
extern int				FILE_OpenDirWithCluster(int partID, unsigned long dir_cluster, int dir_num);
extern int				FILE_Copy(int dir_num, int sHandle, long *copy_status);
extern int				FILE_Format(int drvTypeID, int partID, unsigned char writeVolume,
									unsigned char SecPerClus, unsigned int CntOfClus, struct _DISK_INFO *disk);

extern int				FILE_FormatMBR(int drvTypeID, unsigned char writeVolume, unsigned char SecPerClus, unsigned int *numOfSecPR, unsigned int *validFAT, struct _DISK_INFO *disk);
									

extern int				FILE_DiskInfo(int partID, DRVINFOstruc *pDrive);
extern int				FILE_GetValidPartNum(void);
extern int				FILE_RemoveDirByEntryId(int dir_num, int dir_id);
extern int				FILE_RemoveWithHandle(int dir_num, int nHandle);
extern void				FILE_UpdateDate(unsigned short year, unsigned char month, unsigned char day);
extern void				FILE_UpdateTime(unsigned char hours, unsigned char min, unsigned char sec);
extern int				FILE_CheckRootDir(int dir_num);
extern int				FILE_GetUsedClusters(int drvTypeID);
extern int				FILE_RenameLFN(int nHandle, void *pName, int dir_num);
extern int				FILE_OpenDirOnlySearch(int partID, unsigned long dir_cluster, int dir_num);


#ifdef NED_INCLUDE
#include "NED/NED_FS.H"
#else
extern int				FILE_OpenByEntryId(int file_id, unsigned long mode, int dir_num);		// open a file. get handle
extern int				FILE_Close(int nHandle);												// close a file. return handle
extern int				FILE_Read(int nHandle, void *pBuff, int size);							// read a file
extern int				FILE_Seek(int nHandle, long offset, int whence);						// move file pointer.
extern int				FILE_Read_bs(int nHandle, void *pBuff, int size);						// special function
extern long				FILE_Tell(int nHandle);													// get a current file pointer
extern int				FILE_Length(int nHandle);												// get a current file size
#endif

#endif

