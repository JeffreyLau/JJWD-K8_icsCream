/****************************************************************************
 *   FileName    : TC_File.c
 *   Description : File System Abstraction Layer
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/

/****************************************************************************

  Revision History

 ****************************************************************************

 [1052] : 1. Option is added for ignoring file or directory attribute such as
             RDONLY, HIDDEN, SYSTEM when deleteing.
 [1113] : Unmounting function revised to be executed only for mounted device.
 [1168] : 1. Add TC_Remove_SubDir() function to support removing all sub-directories.
          2. TC_Remove_Dir(), TC_Remove_DirIndex(), TC_Remove_DirUnicode() functions are
             revised to support the option for removing non-empty directory.
 [1198] : 1. fix comment about TC_Become_File_User().
 [1262] : Change display message sequence, when device unmount
 [1275] : 1. Add result check code in TC_Set_Current_Cluster() function
 [1276] : 1. Fix check code error in TC_Set_Current_Cluster() function
 [1304] : 1. add code for internal hidden drive
 [1307] : 1. insert DISK_SetState() function in Mount, Unmount() functions.
         2. Change DISK_STATE_MOUNTERR to DISK_STATE_NOTMOUNTED in Unmount() functions.
 [1317] : 1. Fix format problem in 1GB data area.
          2. Move setup_disk() function out from library (NU_File.a).
 [1343] : 1. Add flag for supporting 4GB-FAT16 type.
          2. Add flag for set minimum cluster size
 [1353] : 1. Add flag for FAT32 format
 [1354] : 1. Add DEV_TELL_DATASTARTSECTOR command when TC_Open_Disk(). (NAND only)
 [1356] : 1. Fix [1354] version.
 [1371] : 1. NU_Sync***() functions can return the status of I/O driver function call.
          2. Add codes for processing return value of NU_Sync***() functions.

 ****************************************************************************/
#if defined(_LINUX_) || defined(_WINCE_)
//#include "globals.h"
#include <tnftl/IO_TCCXXX.h>
#include <fwdn/TC_File.h>
#include <fwdn/FSAPP.h>
#endif

#if defined (NKUSE)
#include "windows.h"
#include "stdlib.h"
#elif defined(__KERNEL__)
#endif


#ifndef ISONE
#define	ISONE(X, MASK)				( (unsigned int)(X) & ((unsigned int)(MASK)) )
#endif

#ifdef	FILE_CACHE_INCLUDE
#include "FileBufferCtrl.h"
#endif

/************************************************************************
* FUNCTION
*			TC_Open
*
* DESCRIPTION
*			Open the file for access as specified in flag.
*			If creating, use mode to set the access permissions on the file.
*
* INPUTS
*			name	= path name to a file.
*			uFlag	= flag for open
*				TC_O_CREAT		: create a file
*				TC_O_RDONLY	: open with read-only
*				TC_O_WRONLY	: open with write-only
*				TC_O_RDWR		: open with read/write
*				TC_O_APPEND	: open with append
*				TC_O_TRUNC		: create a file with truncate
*				TC_O_EXCL		: in creating, error returned if same file already exist.
*			uMode	= mode for open
*				TC_A_READ		: read operation possible.
*				TC_A_WRITE		: write operation possible.
*			uDirNum	= value for K-filesystem. don't care in NU-filesystem.
*
* OUTPUTS
*			TC_STAT - Return Type
*  			= 0 or positive : successful, it means file handle.
*			= negative : failure.
*************************************************************************/
int MakeLFN(char *SFN, unsigned short *LFN)
{
	int i;

	for(i = 0 ; i < ((126>>1)-1) && *SFN; i++)
	{
		*LFN++ = (unsigned short) *SFN++;
	}

	*LFN++ = 0;
	*LFN++ = 0;

	return i;
}

TC_STAT TC_Open(char *name, TC_U32 uFlag, TC_U32 uMode, TC_U32 uDirNum)
{
#if defined (NKUSE)
	HANDLE hFile;
	hFile = CreateFile( (LPCTSTR)name, GENERIC_READ, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	return 	hFile;
#elif defined(__KERNEL__)
	/* if Linux Kernel, not use */
#else
	TC_STAT	iTC_Ret;
	unsigned char	*pusName;
	unsigned int	nLength;

	unsigned short LFN[126];

	if (ISONE(uFlag, TC_O_RDONLY))
		iTC_Ret	= FILE_OpenName(name, "r", uDirNum);
	else if (ISONE(uFlag, TC_O_CREAT))
	{
		pusName = (unsigned char *)name;
		//nLength = str_len16(pusName);	// size of LFN(unicode char.)
        nLength = str_len(pusName);
            #if 0
		if(nLength > 11)
		{
			memset(LFN,0x00,126);
			MakeLFN(name, LFN);
			iTC_Ret = FILE_CreateLFN(LFN , uMode|FILE_ARCHIVE, uDirNum);
		}
		else
		{
		iTC_Ret	= FILE_Create(name, uMode|FILE_ARCHIVE, uDirNum);
		}
		#else
            //memset(LFN,0x00,126);
            //MakeLFN(name, LFN);
			//iTC_Ret = FILE_CreateLFN(LFN , uMode|FILE_ARCHIVE, uDirNum); //zminc
            iTC_Ret = FILE_CreateLFN(name , uMode|FILE_ARCHIVE, uDirNum);
		#endif
	}
	else
		iTC_Ret	= FILE_OpenName(name, "w", uDirNum);

	if(iTC_Ret <= 0 )
		return -1;

	return	iTC_Ret;
#endif
}

/**************************************************************************
*  FUNCTION NAME :
*
*      TC_S32	TC_Read(TC_S32 iHandle, void *pBuff, TC_S32 iCount);
*
*  DESCRIPTION : Read data from file.
*       Attempt to read iCount bytes from the current file pointer of
*       file at iHandle and put them in pBuff. The file pointer is updated.
*
*  INPUT:
*			iHandle	= file handle.
*			pBuff	= destination pointer
*			iCount	= byte count to read
*
*  OUTPUT:	TC_S32 - Return Type
*  			= 0 or positive : byte count read
*			= negative : failure.
*
*  REMARK:	created on 2006년 8월 25일 금요일 오전 12:16:32    by vizirdo
**************************************************************************/
TC_S32	TC_Read(TC_S32 iHandle, void *pBuff, TC_S32 iCount)
{
	TC_S32	iTC_Ret;
#if defined (NKUSE)
	ReadFile((HANDLE)iHandle, pBuff, iCount, &iTC_Ret, NULL);
#elif defined(__KERNEL__)
#else
	iTC_Ret	= FILE_Read(iHandle, pBuff, iCount);
#endif
	return	iTC_Ret;
}

/**************************************************************************
*  FUNCTION NAME :
*
*      TC_S32	TC_Write(TC_S32 iHandle, void *pBuff, TC_S32 iCount);
*
*  DESCRIPTION : Write data to file.
*       Attempt to write iCount bytes from pBuff to the current file
*       pointer of file at iHandle. The file pointer is updated.
*
*  INPUT:
*			iHandle	= file handle.
*			pBuff	= source buffer
*			iCount	= byte count to write
*
*  OUTPUT:	TC_S32 - Return Type
*  			= 0 or positive : byte count written
*			= negative : failure.
*
*  REMARK:	created on 2006년 8월 25일 금요일 오전 12:17:29    by vizirdo
**************************************************************************/
TC_S32	TC_Write(TC_S32 iHandle, void *pBuff, TC_S32 iCount)
{
#if defined(NKUSE)
	return 0;
#elif defined(__KERNEL__)
	return 0;
#else
	TC_S32	iTC_Ret;

	iTC_Ret	= FILE_Write(iHandle, pBuff, iCount);
	return	iTC_Ret;
#endif
}

/**************************************************************************
*  FUNCTION NAME :
*
*      TC_S32	TC_Seek(TC_S32 iHandle, TC_S32 iOffset, TC_S32 iOrigin);
*
*  DESCRIPTION : Move file pointer
*       Move the file pointer iOffset bytes from the origin described by "iOrigin".
*
*  INPUT:
*			iHandle	= file handle.
*			iOffset	= offset value to move pointer.
*			iOrigin	= origin type
*				TC_SEEK_SET : offset is from the start of file.
*				TC_SEEK_CUR : offset is from the current pointer.
*				TC_SEEK_END : offset is from the end of file.
*
*  OUTPUT:	TC_S32 - Return Type
*  			= 0 or positive : file pointer after seeking.
*			= negative : failure.
*
*  REMARK:	created on 2006년 8월 25일 금요일 오전 12:18:20    by vizirdo
**************************************************************************/
TC_S32	TC_Seek(TC_S32 iHandle, TC_S32 iOffset, TC_S32 iOrigin)
{
	TC_S32	iTC_Ret;
#if defined (NKUSE)
	iTC_Ret = SetFilePointer((HANDLE)iHandle, iOffset, NULL, iOrigin);
#elif defined(__KERNEL__)
#else
	iTC_Ret	= FILE_Seek(iHandle, iOffset, iOrigin);
#endif
	return	iTC_Ret;
}

/**************************************************************************
*  FUNCTION NAME :
*
*      TC_STAT	TC_Close(TC_S32 iHandle);
*
*  DESCRIPTION : Close a file
*       Close the file updating the disk and freeing all core DB associated with iHandle.
*
*  INPUT:
*			iHandle	= file handle.
*
*  OUTPUT:	TC_STAT - Return Type
*  			= 0 : successful
*			= negative : failure.
*
*  REMARK:	created on 2006년 8월 25일 금요일 오전 12:21:33    by vizirdo
**************************************************************************/
TC_STAT	TC_Close(TC_S32 iHandle)
{
	TC_STAT	iTC_Ret;
#if defined (NKUSE)
	iTC_Ret = CloseHandle((HANDLE)iHandle);
#elif defined(__KERNEL__)
#else
	iTC_Ret	= FILE_Close(iHandle);
#endif
	return	iTC_Ret;
}

/**************************************************************************
*  FUNCTION NAME :
*
*      TC_S32	TC_Length(TC_S32 iHandle);
*
*  DESCRIPTION : Get file size with iHandle.
*
*  INPUT:
*			iHandle	= file handle.
*
*  OUTPUT:	TC_S32 - Return Type
*  			= 0 or positive : length of a file (byte unit)
*			= negative : failure.
*
*  REMARK:	created on 2006년 8월 25일 금요일 오전 12:23:15    by vizirdo
**************************************************************************/
TC_S32	TC_Length(TC_S32 iHandle)
{
	TC_S32	iTC_Ret;
#if defined (NKUSE)
	iTC_Ret = GetFileSize((HANDLE)iHandle, NULL);//length;
#elif defined(__KERNEL__)
	/* if Linux Kernel, not use */
#else
	iTC_Ret	= FILE_Length(iHandle);
#endif
	return	iTC_Ret;
}

/**************************************************************************
*  FUNCTION NAME :
*
*      TC_STAT	TC_DeleteIndex(TC_U32 uIndex, TC_U32 uDirNum);
*
*  DESCRIPTION : Delete a file with Index
*       Delete the file in "uIndex". Fail if not a simple file, if it is open, does not exist, or is read only.
*
*  INPUT:
*			uIndex	= Index value.  Index means an 32bit integer value can locate the file in one drive.
*						the file must exist before calling this function.
*			uOption	=
*					[6] : when 1, it Ignore special attribute SYSTEM
*					[5] : when 1, it Ignore special attribute HIDDEN
*					[4] : when 1, it Ignore special attribute RDONLY
*					[3:0] : same as TC_Open()
*
*  OUTPUT:	TC_STAT - Return Type
*  			= 0 : successful
*			= negative : failure
*
*  REMARK:	created on 2006년 8월 25일 금요일 오전 12:26:32    by vizirdo
**************************************************************************/
TC_STAT	TC_DeleteIndex(TC_U32 uIndex, TC_U32 uOption)
{
#if defined (NKUSE)
	return 0;
#elif defined(__KERNEL__)
	return 0;
#else
	TC_STAT	iTC_Ret;

	iTC_Ret	= FILE_RemoveByEntryId(uOption, uIndex);
	return	iTC_Ret;
#endif
}

/**************************************************************************
*  FUNCTION NAME :
*
*      TC_STAT	TC_Make_Dir(TC_STR8 pName, TC_U32 uDirNum);
*
*  DESCRIPTION : Make a directory
*       Create a sudirectory in the path specified by pName. Fails if a
*       file or directory of the same name already exists or if the path is not found.
*
*  INPUT:
*			pName	= Path name to directory
*			uDirNum	= Same as TC_Open()
*
*  OUTPUT:	TC_STAT - Return Type
*  			= 0 : successful
*			= negative : failure
*
*  REMARK:	created on 2006년 8월 25일 금요일 오전 12:28:14    by vizirdo
**************************************************************************/
TC_STAT	TC_Make_Dir(TC_STR8 pName, TC_U32 uDirNum)
{
#if defined (NKUSE)
	return 0;
#elif defined(__KERNEL__)
	return 0;
#else
	TC_STAT	iTC_Ret;
	unsigned char	*pusName;
	unsigned int	nLength;
	unsigned short LFN[126];

	if (pName[0] == '\\' && pName[1] == '/')
		iTC_Ret	= FILE_MakeHideDir(pName, uDirNum);
	else
	{
		pusName = (unsigned char *)pName;
		#if 0
		nLength = str_len(pusName);	// size of LFN(unicode char.)
		#else
		{
			int count;
			unsigned short *str = (unsigned char *)pusName;
			for(count=0; str[count] != 0; count++);
			nLength = count;
		}
		#endif

		//if(nLength > 8)
		{
			//memset(LFN,0x00,126);
			//MakeLFN(pName, LFN);
			//iTC_Ret = FILE_CreateLFN(LFN, 0x10, uDirNum); //zminc
			iTC_Ret = FILE_CreateLFN(pName, 0x10, uDirNum);
			//FILE_Refresh(uDirNum);
			//FILE_Close(uDirNum);
		}
		//else
		//{
		//iTC_Ret	= FILE_MakeDir(pName, uDirNum);
		//}
	}
	return	iTC_Ret;
#endif
}

/**************************************************************************
*  FUNCTION NAME :
*
*      TC_STAT	TC_SyncDrives(TC_S32 uDriveNo, TC_U32 uNum);
*
*  DESCRIPTION : Synchronize the block cache with disk.
*       Same as TC_Sync() except that uDriveNo and uNum should be an integer.
*
*  INPUT:
*			uDriveNo	= drive number starts from 0.
*						if (uDriveNo is negative), uDriveNo is converted to -(uDriveNo + 1)
*						and cache is flushed after synchronizing.
*			uNum	= number of drive to synchronize.
*
*  OUTPUT:	TC_STAT - Return Type
*  			= 0 : successful
*			= negative : failure
*
*  REMARK:	created on 2006년 8월 25일 금요일 오전 12:32:00    by vizirdo
**************************************************************************/
TC_STAT	TC_SyncDrives(TC_S32 uDriveNo, TC_U32 uNum)
{
#if defined (NKUSE)
	return 0;
#elif defined(__KERNEL__)
	return 0;
#else
	TC_STAT	iTC_Ret	= 0;

	iTC_Ret	= TC_SUCCESS;
	return	iTC_Ret;
#endif
}

/**************************************************************************
*  FUNCTION NAME :
*
*      TC_U32	TC_Get_FileIndex(TC_S32 iHandle, TC_U32 uDirNum);
*
*  DESCRIPTION : You can add file description here.
*       Given the file handle, Get the index addressed by iHandle.
*
*  INPUT:
*			iHandle	= file handle
*			uDirNum	= same as TC_Open()
*
*  OUTPUT:	TC_U32 - Return Type
*  			= Index of the file handle
*
*  REMARK:	created on 2006년 8월 25일 금요일 오전 12:32:23    by vizirdo
**************************************************************************/
TC_U32	TC_Get_FileIndex(TC_S32 iHandle, TC_U32 uDirNum)
{
#if defined (NKUSE)
	return 0;
#elif defined(__KERNEL__)
	return 0;
#else
	TC_U32	iTC_Ret;

	iTC_Ret	= TC_SUCCESS;
	return	iTC_Ret;
#endif
}


/**************************************************************************
*  FUNCTION NAME :
*
*      TC_STAT	TC_Set_Current_Dir(TC_S32 iPartID, TC_STR8 pPath, TC_U32 uDirNum);
*
*  DESCRIPTION :
*       Find path. If it is a subdirectory make it the current working  directory for the drive.
*
*  INPUT:
*			iPartID	= drive number starts from 0.
*			pPath	= path name of directory.
*			uDirNum	= same as TC_Open()
*
*  OUTPUT:	TC_STAT - Return Type
*  			= 0 : successful
*			= negative : failure
*
*  REMARK:	created on 2006년 8월 25일 금요일 오전 12:34:19    by vizirdo
**************************************************************************/
TC_STAT	TC_Set_Current_Dir(TC_S32 iPartID, TC_STR8 pPath, TC_U32 uDirNum)
{
#if defined (NKUSE)
	return 0;
#elif defined(__KERNEL__)
	return 0;
#else
	TC_STAT	iTC_Ret;

	if (pPath[0] == '\\')
		iTC_Ret = FILE_OpenDirByEntryId(iPartID, 0, ROOT_DIR_ID, uDirNum);
	else
		iTC_Ret = FILE_OpenDirName(pPath, "r", uDirNum);

	if( iTC_Ret != uDirNum)
		iTC_Ret = -1;

	return	iTC_Ret;
#endif
}

/************* end of file *************************************************************/
