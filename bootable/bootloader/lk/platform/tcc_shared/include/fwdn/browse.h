/*******************************************************************************			
***                                                                          
***   TELECHIPS.                                     
***                                                                          
***                                                                           
***                                                                          
*******************************************************************************/
#ifndef _BROWSE_H_
#define _BROWSE_H_

#if defined(_LINUX_)
#include <common.h>
#endif

//#ifndef _MAIN_H_
//	#include "main.h"
//#endif

#define 	ROOTDIR		0
#define 	_MUSICFILE 		1
#define 	_NOTMUSICFILE	-1

#define 	WITHNULL 		0

#define 	_DIR			0
#define 	_FILE			1
//#define _DONOTUSE	2

#ifndef KILO
#define KILO	1024
#endif

// Error Message 
enum {
	BRWS_NO_ERROR = 0,
	BRWS_ERROR_NODIR,
	BRWS_ERROR_NOFILE,
	BRWS_ERROR_CANTGO,
	BRWS_ERROR_UNDEF_ATTR,
	BRWS_ERROR_INVALID_INDEX,
	BRWS_ERROR_FAILE_DELETE,
	BRWS_ERROR_UHP_BUSY,
	BRWS_UNDEF_ERROR
};

enum{
BRWS_EXT_MP3,
#ifdef MTX_INCLUDE	
BRWS_EXT_MTX,
#endif
#ifdef WMA_INCLUDE
BRWS_EXT_WMA,
#ifdef TRUSTED_FLASH_INCLUDE
BRWS_EXT_SMA,
#endif
#endif
#ifdef MP2_INCLUDE
BRWS_EXT_MP2,
#endif
#ifdef OGG_INCLUDE
BRWS_EXT_OGG,
#endif
#ifdef	EAACPlus_INCLUDE
BRWS_EXT_EAACPlus_AAC,
BRWS_EXT_EAACPlus_M4A,
BRWS_EXT_EAACPlus_MP4,
BRWS_EXT_EAACPlus_ADIF,
BRWS_EXT_EAACPlus_ADTS,
BRWS_EXT_EAACPlus_3GP,
#endif	
#ifdef WAV_INCLUDE
BRWS_EXT_WAV,
#endif
#ifdef MP4_INCLUDE
BRWS_EXT_MP4,
#endif
#ifdef WMV_INCLUDE
BRWS_EXT_WMV,
#ifdef TRUSTED_FLASH_INCLUDE
BRWS_EXT_SMV,
#endif
#endif
#if defined(JPG_INCLUDE) || defined (MULTI_CODEC_INCLUDE)
BRWS_EXT_JPG,
#endif
#if defined(PNG_INCLUDE) 
BRWS_EXT_PNG,
#endif
#if defined(BMP_INCLUDE) 
BRWS_EXT_BMP,
#endif
#if defined(GIF_INCLUDE) 
BRWS_EXT_GIF,
#endif
#ifdef AUDIBLE_INCLUDE
BRWS_EXT_AA,
#endif
#if defined (MULTI_CODEC_INCLUDE)
BRWS_EXT_TXT,
#endif
#ifdef M3U_INCLUDE
BRWS_EXT_M3U,
#endif
#if defined(MTP_CUSTOM_IMAGE_INCLUDE)
BRWS_EXT_FIL,
#endif
#ifdef RHAPSODY_INCLUDE
BRWS_EXT_XML,
#endif
BRWS_EXT_MAX
};

/* Start - skott 's  code */
#define 	MAX_PLAYLIST_NUM			1*KILO
	
// 2M Buffer config
#ifdef BRWS_STR_NAND_INCLUDE
#define BRWS_NAME_BUFFER_SIZE	(4096)
#elif defined(MTP_INCLUDE)
#define BRWS_NAME_BUFFER_SIZE	(256)
#else
#define BRWS_NAME_BUFFER_SIZE	(KILO*KILO*2)
#endif
#define MAX_INDEX_LIST_NUM		1*KILO

#ifdef MTP_INCLUDE
#define	MAX_FOLDER_NUM			MTPDB_DEVICE_FOLDER_MAX
#define	MAX_MUSICFILE_NUM		MTPDB_DEVICE_FILE_MAX
#else
#define	MAX_FOLDER_NUM			2*KILO
#define	MAX_MUSICFILE_NUM		1*KILO
#endif


typedef enum
{
	DB_NOT_USED =0,
	DB_USED
#ifdef MTP_INCLUDE
	,DB_BE_DELETED
#endif
}DB_USAGE_TYPE;



/*			Status Define
==================================================================
|B7			|  B6  		| B5			| B4			| B3	/ B2		| B1  	     	| B0		|
==================================================================
|reserved	| Reference	| Reference	| AlbumJacket	| Protection	|  Protection	Status		|
|			| Flag 		| member flag| Exist Flag	| Status		| 			 			|
==================================================================
*/
typedef struct{
	unsigned int	ucMotherIndex;
	unsigned int	ucFileIndex;
	unsigned char 	ucAttr;
	unsigned char 	ucPartition_ID;
	unsigned char 	ucStatus;
	unsigned char 	ucCodec;
#ifndef MTP_INCLUDE
	unsigned int 	ucShortFileName;
	unsigned int 	ucLongFileName;
	unsigned char 	fLfn;
	unsigned char 	dummy[3];
#else
	unsigned long	ulCluster;			//start cluster in FAT
	unsigned short	usCRC16FileName;	//verify filename
//MTPDB2DB-->
	unsigned short	usPacketOffset;
//<--MTPDB2DB
#ifdef	VIDEOART_INCLUDE
	unsigned int	AlbumArtIndex;
#else
	unsigned short	AlbumArtIndex;
#endif
	unsigned short	ObjectFormat; 

#ifdef MEDIA_RESUME_INCLUDE
	unsigned short	usLastTimeSec; 
#endif

#endif
}_FILE_LIST_TYPE,*pFILE_LIST_TYPE;

typedef struct{
	unsigned short	ucFileCount;    	// Total File Number
	unsigned short	ucFolderCount;		// Total Folder Number
	unsigned int	ucMotherIndex;		// Mother Index Number
	unsigned int	ucFolderIndex;		// The order of folder in entry field like 1st, 2nd ..
	unsigned long	ulCluster;			// Start Cluster in FAT
	unsigned char 	ucPartition_ID;		// Partition ID
	unsigned char 	fUsed;				// Used or Not used
#ifndef MTP_INCLUDE
	unsigned int 	ucShortFolderName;	
	unsigned int 	ucLongFolderName;	
	unsigned char 	fLfn;			
	unsigned char 	dummy[3];		
#else
	unsigned short	usCRC16FileName;	//verify filename
//MTPDB2DB-->
	unsigned short 	usPacketOffset;
//<--MTPDB2DB
	unsigned char 	ucStatus;
	unsigned char		ReferenceCount;
#endif
}_FOLDER_LIST_TYPE,*pFOLDER_LIST_TYPE;


typedef struct{
	unsigned char 		ucName[9];			// Disk name
	unsigned char 		ucUsed;				// Used or not
	unsigned char 		ucDeviceType;		// Device type from enum DISK_DEVICE
	unsigned char 		ucPartID;			// Partition ID
	unsigned int		uiFolderNum;		// Total folder number in disk
	unsigned int		uiFileNum;			// Total file number in disk
	unsigned int		uiRootIndex;		// Root index at stTotalFolderList[]	
//	unsigned char 		dummy;
}_DEVICE_TYPE_INFO;

#define 	BRWS_MAKE_LIST_OF_FOLDER	0x01
#define 	BRWS_MAKE_LIST_OF_FILE		0x02
#define 	BRWS_MAKE_LIST_OF_ALL		BRWS_MAKE_LIST_OF_FOLDER | BRWS_MAKE_LIST_OF_FILE

#define 	BRWS_MAKE_PLAY_LIST_ALL		0x80
#define 	BRWS_MAKE_PLAY_LIST_SIMAGE	0x40
#define 	BRWS_MAKE_PLAY_LIST_TEXT	0x20
#define 	BRWS_MAKE_PLAY_LIST_MPG4	0x10
#define 	BRWS_MAKE_PLAY_LIST_AUDIO	0x08
#define	BRWS_MAKE_PLAY_LIST_M3U		0x04

#define 	BRWS_SEARCH_ALL_FOLDER		-1

#define		BRWS_NAME_AREA_SIZE		 	4096

enum
{
	FOLDER_RECORD = 10,
#ifdef _RECORD_SUB_FOLDER_INCLUDE_
	#ifdef _RECORD_SUB_FOLDER_LINEIN_INCLUDE_
		FOLDER_RECORD_LINEIN,
	#endif
	#ifdef _RECORD_SUB_FOLDER_FM_INCLUDE_
		FOLDER_RECORD_FM,
	#endif
	#ifdef _RECORD_SUB_FOLDER_VOICE_INCLUDE_
		FOLDER_RECORD_VOICE,
	#endif
	#ifdef _RECORD_SUB_FOLDER_CAM_INCLUDE_
		FOLDER_RECORD_CAM,
	#endif
	#ifdef _RECORD_SUB_FOLDER_TV_INCLUDE_
		FOLDER_RECORD_TV,
	#endif
#endif
	FOLDER_PHOTO,
	FOLDER_AUDIBLE,
	FOLDER_CONFIG,
	FOLDER_ROOT,
	FOLDER_DEFAULT,
	FOLDER_UNKNOWN
};

#ifdef _RECORD_SUB_FOLDER_INCLUDE_
typedef enum
{
#ifdef _RECORD_SUB_FOLDER_LINEIN_INCLUDE_
	RECORD_FOLDER_LINEIN,
#endif
#ifdef _RECORD_SUB_FOLDER_FM_INCLUDE_
	RECORD_FOLDER_FM,
#endif
#ifdef _RECORD_SUB_FOLDER_VOICE_INCLUDE_
	RECORD_FOLDER_VOICE,
#endif
#ifdef _RECORD_SUB_FOLDER_CAM_INCLUDE_
	RECORD_FOLDER_CAM,
#endif
#ifdef _RECORD_SUB_FOLDER_TV_INCLUDE_
	RECORD_FOLDER_TV,
#endif
	RECORD_SUB_FOLDER_UNKNOWN,
	RECORD_SUB_FOLDER_MAX = RECORD_SUB_FOLDER_UNKNOWN
}RECORD_SUB_FOLDER_TYPE;
#endif


/*--------------------------------------------------------------
 Folder Index Function 
----------------------------------------------------------------*/
extern unsigned int  BRWS_GetRecordFileIndex(void);
extern unsigned int  BRWS_GetRecordFolderIndex(void);
extern unsigned int  BRWS_GetRecordFolderCluster(void);
#ifdef _RECORD_SUB_FOLDER_INCLUDE_
extern unsigned int  BRWS_GetRecordSubFolderIndex(unsigned int rec_sub_type);
extern unsigned int  BRWS_GetRecordSubFolderCluster(unsigned int rec_sub_type);
#endif
extern unsigned int  BRWS_GetCurrFolderIndex(void);
extern void BRWS_SaveFolderIndex(unsigned int iIndex);


/*--------------------------------------------------------------
 Functions for making File List

 name : BRWS_MakePlayList
----------------------------------------------------------------*/
extern unsigned int BRWS_MakePlayList(int uiTotalIndex, unsigned short *ptrList, unsigned char ucMode );

extern unsigned int BRWS_FindFileNumInPlayList(int uiTotalIndex, unsigned int uiDBIndex, unsigned char ucMode );


/*--------------------------------------------------------------
Get Count Value Function 
----------------------------------------------------------------*/
extern unsigned 	int BRWS_GetTotalFolderNum( void );
extern unsigned 	int BRWS_GetTotalFileCount( void );

/*--------------------------------------------------------------
utility Function
----------------------------------------------------------------*/
extern int BRWS_FindMusicFile(const char *filename, int mode);
extern int BRWS_CorrectDirName(char *path);
extern void BRWS_CharCopy(const char *input, char *output, int num);
extern int BRWS_CharNICmp(const char *src1, const char *src2, int cmplen);
extern int BRWS_IsReadOnly(int attr);

/*--------------------------------------------------------------
Check the main device if config folder is existed. if not existing config folder, create it.
----------------------------------------------------------------*/
extern int BRWS_CheckConfigFolderIs(int iPartID, int dir_num);

/*--------------------------------------------------------------
Audible function 
----------------------------------------------------------------*/
#ifdef AUDIBLE_INCLUDE
extern int BRWS_CheckAudibleFolderIs(int);
#endif
/*--------------------------------------------------------------
Recoding function 
----------------------------------------------------------------*/
extern int BRWS_CheckRecodingFolderIs(int);
extern int BRWS_ChangeRecodingFolder(void);
extern int BRWS_UpdateRecodingDB(void);

/*--------------------------------------------------------------
Check File Attrive Function
----------------------------------------------------------------*/
extern int BRWS_CheckFileAttrive(unsigned int uiFileIdx);


/*--------------------------------------------------------------
Format Drive Function

----------------------------------------------------------------*/
extern int BRWS_FormatDrive(unsigned short mode, int iPartID);


/*--------------------------------------------------------------
Delete File Function
----------------------------------------------------------------*/
extern int BRWS_DeleteFile(unsigned int uiCurrFileNum, unsigned int uiFolderIdx);

/*---------------------------------------------------------------------
	BRWS_ReProfilingMTPUIDB
-----------------------------------------------------------------------*/
extern int BRWS_ReProfilingMTPUIDB(void);

/*--------------------------------------------------------------
Copy File Function
----------------------------------------------------------------*/
extern int	BRWS_CopyFile(unsigned int uiSrcFileIndex, unsigned int uiDstFolderIndex);

/*--------------------------------------------------------------
Check Disk WP Status
----------------------------------------------------------------*/
extern int BRWS_CheckWPStatus(int part_ID);

/*--------------------------------------------------------------
Check CRC of Image on NAND Disk
----------------------------------------------------------------*/
#ifdef NAND_BOOT_INCLUDE
extern int BRWS_CheckCRCOfNANDBOOT( void );
#endif

/*--------------------------------------------------------------
	Change Folder  usign cluster 
----------------------------------------------------------------*/
extern int	BRWS_CheckIsFolderRoot( unsigned int uiIndex );

extern int BRWS_ChangeBackgroundFolder( unsigned int uiTotalIndex );

extern int BRWS_ChangeFolder( unsigned int uiTotalIndex );

extern int	BRWS_InitializeCurrFolderList(unsigned int uiTotalIndex, unsigned short *ptrList, unsigned char ucMode );

extern int	BRWS_InitializeDeviceList(unsigned short *ptrList);

/*--------------------------------------------------------------
Folder Name Function 
----------------------------------------------------------------*/
extern unsigned char *BRWS_GetFolderName( unsigned int uiIndex, unsigned char *fLfn);
extern unsigned char *BRWS_GetFileName( unsigned int uiFileIndex, unsigned char *fLfn);

/*--------------------------------------------------------------
	Search all file and folder in Selected Folder 
----------------------------------------------------------------*/

extern int BRWS_SearchFolder(int iPartID, unsigned int uiTotalIndex);

/*--------------------------------------------------------------
	Make Folder and File List DataBase
		: using BRWS_SearchFolder();

----------------------------------------------------------------*/
extern void BRWS_InitializeFolderDB( void );

extern int BRWS_AddDevice( unsigned char *ucName,  unsigned char ucType );
extern int BRWS_RefreshMainDevice( void );

extern int BRWS_SaveBRWSDB(void);
extern int BRWS_LoadBRWSDB(void);
extern int BRWS_DelBRWSDB(void);

#ifdef BRWS_STR_NAND_INCLUDE
extern unsigned char *BRWS_GetNameString(unsigned int Offset);
extern void BRWS_InitNameStringArea(unsigned int uiStartAddr,unsigned int uiSectorSize);
extern void BRWS_CleanNameString(unsigned char *ucSrcString,unsigned int uiOffset);
#endif
extern void BRWS_PutNameString(unsigned char *ucSrcString,unsigned int uiOffset,unsigned char ucNameChar);
extern unsigned char BRWS_GetDeviceStatus(unsigned int uiIndex);
extern unsigned int  BRWS_GetDefaultDevice(void);
extern void  		 BRWS_SetDefaultDevice(unsigned int uiDefaultDevice);
extern unsigned int  BRWS_GetDeviceCount(void);
extern unsigned int  BRWS_GetTotalDeviceCount(void);
extern void 		 BRWS_RemoveDevice(unsigned char ucDeviceType);
extern unsigned char BRWS_GetFilePartID(unsigned int uiFileIndex);
/* End - skott 's  code */

 extern  int BRWS_CHeckFFREWSeekStatus(void);

extern int BRWS_IsTAGJPGCodec(int iCurrentCodec);
extern int BRWS_IsMP3MP2Codec(int iCurrentCodec);
extern int BRWS_IsAudibleCodec(int iCurrentCodec);
extern int BRWS_IsImageCodec(int iCurrentCodec);
extern int BRWS_IsBackgroundImageCodec(unsigned int uiBackgroundCodec);
extern int BRWS_IsEAACPlusCodec(int iCurrentCodec);
extern int BRWS_IsVideoCodec(int iCurrentCodec);  
extern int BRWS_IsWMVVideoCodec(int iCurrentCodec);  
extern int BRWS_IsDrmCodec(int iCurrentCodec); 
extern void BRWS_FILE_OpenDir(int iCurrentCodec, unsigned int uiTotalIndex) ;
extern void	BRWS_DisplayProgressFile(int iStartIndex, unsigned int Count,unsigned int  MaxFileNum);
extern unsigned long BRWS_GetFileCluster(int index);

#endif
 
