/****************************************************************************
 *   FileName    : nand_drv.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 * 
 ****************************************************************************/
#ifndef __NAND_H
#define __NAND_H

//#include "main.h"

//**********************************************************************
//*		Define NAND Driver version for ChipSet
//**********************************************************************
#if defined(NAND_BOOT_REV)
#define TNFTL_V7_INCLUDE
#else	// K-FILESYSTEM
#define TNFTL_V6_INCLUDE
#endif

//**********************************************************************
//*		Include Header
//**********************************************************************
#if defined(_LINUX_) || defined(_WINCE_)
#if defined(TNFTL_V7_INCLUDE)
#include "tnftl_v7.h"
#include "fwupgrade_NAND_v6.h"
#elif defined(TNFTL_V6_INCLUDE)
#include "tnftl_v6.h"
#include "fwupgrade_NAND_v6.h"
#endif
#endif

#if defined(TCC801X)
#include "tnftl_v7.h"
#include "fwupgrade_NAND_v6.h"
#endif

#if defined(USB_INCLUDE) && defined(FWDN_INCLUDE)
#ifdef TNFTL_V5_INCLUDE
#include "MASS_SCSI.h"
#include "VTC.h"
#include "Fwdn_protocol_v2.h"
#define	TNFTL_DEBUG_INCLUDE
#else
#include "fwdn_protocol.h"
#endif
#endif

//*****************************************************************************
//*
//*
//*                       [ General DEFINE & TYPEDEF ]
//*
//*
//*****************************************************************************
#ifndef DISABLE
#define DISABLE					0
#endif
#ifndef ENABLE
#define	ENABLE					1
#endif
#ifndef FALSE
#define FALSE           		0
#endif
#ifndef TRUE
#define TRUE            		1
#endif

#ifndef _U8_
#define _U8_
typedef unsigned char			U8;
#endif
#ifndef _U16_
#define _U16_
typedef unsigned short int		U16;
#endif
#ifndef _U32_
#define _U32_
typedef unsigned int			U32;
#endif
#ifndef _U64_
#define _U64_
typedef unsigned long long		U64;
#endif
#ifndef _BOOL_
#define _BOOL_
typedef unsigned int			BOOL;
#endif

#ifndef NOT_SUPPORT_NAND
#define NOT_SUPPORT_NAND		0xFFFF
#endif

//*****************************************************************************
//*
//*
//*                         [ ERROR CODE ENUMERATION ]
//*
//*
//*****************************************************************************
#ifndef SUCCESS
#define SUCCESS		0
#endif

typedef enum
{
	ERR_NAND_FAILED_GET_DEVICE_INFO = 0xA300000,
	ERR_NAND_INIT_FAILED,
	ERR_NAND_WRONG_PARAMETER
} NAND_ERROR;

//*****************************************************************************
//*
//*
//*                          [ INTERNAL DEFINATION ]
//*
//*
//*****************************************************************************
#define MAX_NAND_DRIVE								1
#define NAND_DRV_0									0

#if defined( TNFTL_V7_INCLUDE )
	#define TNFTL_ZONE_NUM_FOR_PRIMARY_PARTITION		32
	#if defined(_LINUX_) || defined(_WINCE_)
	#define TNFTL_ZONE_NUM_FOR_EXTENDED_PARTITION		32
	#else
	#define TNFTL_ZONE_NUM_FOR_EXTENDED_PARTITION		1
	#endif
	
	#define TNFTL_WCACHE_NUM_For_PRIMARY_PARTITION		12
	#define TNFTL_WCACHE_NUM_For_EXTENDED_PARTITION		2

	#define TNFTL_LPT_BLK_NUM_For_PRIMARY_PARTITION		TNFTL_ZONE_NUM_FOR_PRIMARY_PARTITION
	#define TNFTL_LPT_BLK_NUM_For_EXTENDED_PARTITION	TNFTL_ZONE_NUM_FOR_EXTENDED_PARTITION
	
	#if defined(_LINUX_) || defined(_WINCE_)
	#define TNFTL_EXTENDED_PARTITION_MAX_NUM			4
	#define TNFTL_EXTENDED_PARTITION_NUM				1
	#define TNFTL_EXTENDED_PARTITION_SIZE_MB			1
	#else
	#ifdef INTERNAL_HIDDEN_STORAGE_INCLUDE
	#define TNFTL_EXTENDED_PARTITION_MAX_NUM			1
	#define TNFTL_EXTENDED_PARTITION_NUM				1
	#else
	#define TNFTL_EXTENDED_PARTITION_MAX_NUM			1
	#define TNFTL_EXTENDED_PARTITION_NUM				0
	#endif
	#endif
#else 
	#define TNFTL_ZONE_NUM_FOR_DATA_AREA				32
	#if defined(_LINUX_) || defined(_WINCE_)
	#define TNFTL_ZONE_NUM_FOR_HIDDEN_AREA				32
	#else
	#define TNFTL_ZONE_NUM_FOR_HIDDEN_AREA				1
	#endif
	
	#if defined( TNFTL_V5_INCLUDE ) || defined(TNFTL_V6_INCLUDE)
	#define TNFTL_WCACHE_NUM_For_DATA_AREA				12
	#else
	#define TNFTL_WCACHE_NUM_For_DATA_AREA				8
	#endif
	#define TNFTL_WCACHE_NUM_For_HIDDEN_AREA			2
	
	#define TNFTL_LPT_BLK_NUM_For_DATA_AREA				TNFTL_ZONE_NUM_FOR_DATA_AREA
	#define TNFTL_LPT_BLK_NUM_For_HIDDEN_AREA			TNFTL_ZONE_NUM_FOR_HIDDEN_AREA
	
	#if defined(_LINUX_) || defined(_WINCE_)
	#define TNFTL_MULTI_HIDDEN_AREA_MAX_NUM				3
	#define TNFTL_MULTI_HIDDEN_AREA_NUM					3
	#define INTERNAL_HIDDEN_STORAGE_SIZE_MB				1
	#else
	#ifdef INTERNAL_HIDDEN_STORAGE_INCLUDE
	#define TNFTL_MULTI_HIDDEN_AREA_MAX_NUM				1
	#define TNFTL_MULTI_HIDDEN_AREA_NUM					1
	#else
	#define TNFTL_MULTI_HIDDEN_AREA_MAX_NUM				1
	#define TNFTL_MULTI_HIDDEN_AREA_NUM					0
	#endif
	#endif
#endif

#define NAND_DRVINFO_SAVE								0
#define NAND_DRVINFO_LOAD								1

#define __USE_MTD_EIL__	

//===========================================================
//
//	Common
//
//===========================================================
#define NAND_TYPE_PURE_NAND							0
#define NAND_TYPE_LBA_NAND							1

#define TNFTL_MAX_SUPPORT_RCACHE_MEMORY_SIZE		32768
#define TNFTL_MAX_SUPPORT_ALIGNCACHE_MEMORY_SIZE	32768					

#if defined(TCC801X)
#define STARTPAGE_APPLICATION_CODE				1		//   4Page Used
#define PAGESIZE_FOR_APPLICATION				8192-1		 // 3Mbytes-512 

//=============================================================================================
//		THE MAP OF HIDDEN AREA
//=============================================================================================
// UNICODE FONT AREA					4096	[ 4096*512 = 2097152 ]
// FIRMWARE UPGRADE AREA				1024	[ 1024*512 =  524288 ]
// AREA For PARAMETER  					 256	[  256*512 =  131072 ]	// BigNAND 1Block = 256Page
// UNICODE CONVERT TABLE for CP437				[    4*512 =    2048 ]	// RealSize   1792Bytes
// UNICODE CONVERT TABLE for CP850				[    1*512 = 	512	]	// RealSize   512Bytes
// UNICODE CONVERT TABLE for CP932				[  132*512 =   67584 ]	// RealSize  67456Bytes
// UNICODE CONVERT TABLE for CP936				[  194*512 =   99328 ]	// RealSize  99264Bytes
// UNICODE CONVERT TABLE for CP949				[  220*512 =  112640 ]	// RealSize 112384Bytes
// UNICODE CONVERT TABLE for CP950				[  154*512 =   78848 ]	// RealSize  78528Bytes
//=============================================================================================
#define STARTPAGE_CONVERT_TABLE_CP437				STARTPAGE_APPLICATION_CODE +  PAGESIZE_FOR_APPLICATION 		//   4Page Used
#define STARTPAGE_CONVERT_TABLE_CP850				STARTPAGE_CONVERT_TABLE_CP437 + 4			//	1Page Used
#define STARTPAGE_CONVERT_TABLE_CP932				STARTPAGE_CONVERT_TABLE_CP850 + 1			// 132Page Used
#define STARTPAGE_CONVERT_TABLE_CP936				STARTPAGE_CONVERT_TABLE_CP932 + 132		// 197Page Used
#define STARTPAGE_CONVERT_TABLE_CP949				STARTPAGE_CONVERT_TABLE_CP936 + 197		// 220Page Used
#define STARTPAGE_CONVERT_TABLE_CP950				STARTPAGE_CONVERT_TABLE_CP949 + 220		// 154Page Used

/**************************************************************************
    Camera Overlay Image File Size		300	 [ 300*512 = 153600 ]	// RealSize 153600Bytes
    CAMERA OVERLAY AREA				1500 [ 300*5(file) ]
***************************************************************************/
#ifdef USE_CAMERA
#define	STARTPAGE_OVERLAY_FILE_1						STARTPAGE_CONVERT_TABLE_CP950 + 154
#define	STARTPAGE_OVERLAY_FILE_2						STARTPAGE_OVERLAY_FILE_1 + 300
#define	STARTPAGE_OVERLAY_FILE_3						STARTPAGE_OVERLAY_FILE_2 + 300
#define	STARTPAGE_OVERLAY_FILE_4						STARTPAGE_OVERLAY_FILE_3 + 300
#define	STARTPAGE_OVERLAY_FILE_5						STARTPAGE_OVERLAY_FILE_4 + 300
#define	PAGE_SIZE_OF_OVERLAY_FILE			300
#else
#define	STARTPAGE_OVERLAY_FILE_1				STARTPAGE_CONVERT_TABLE_CP950 + 154
#define	STARTPAGE_OVERLAY_FILE_2				STARTPAGE_CONVERT_TABLE_CP950 + 154
#define	STARTPAGE_OVERLAY_FILE_3				STARTPAGE_CONVERT_TABLE_CP950 + 154
#define	STARTPAGE_OVERLAY_FILE_4				STARTPAGE_CONVERT_TABLE_CP950 + 154
#define	STARTPAGE_OVERLAY_FILE_5				STARTPAGE_CONVERT_TABLE_CP950 + 154
#define	PAGE_SIZE_OF_OVERLAY_FILE			0
#endif

#define ENDPAGE_CONVERT_TABLE							STARTPAGE_OVERLAY_FILE_5 + PAGE_SIZE_OF_OVERLAY_FILE
//---------------------------------------------------------------------------------------------
//	Audio UI File size					233		[ 233 * 512 = 119296 ]	// RealSize 119117 Bytes
//	Audio UI AREA						391		[ 233 + 158 		 ]
//---------------------------------------------------------------------------------------------

#define INFO_AUTO_CONNECTION				ENDPAGE_CONVERT_TABLE
#define INFO_AUTO_CONNECTION_SIZE		8   // 1Kbyte

#define INFO_NET_CONFIG					(INFO_AUTO_CONNECTION + INFO_AUTO_CONNECTION_SIZE)
#define INFO_NET_CONFIG_SIZE				4

#ifdef BRWS_READ_NAND_INCLUDE
#define STARTPAGE_FILENAMEBUFFER_HIDDEN				(INFO_NET_CONFIG + INFO_NET_CONFIG_SIZE)
#define FILENAMEBUFFER_ONE_HIDDEN_SIZE					(( 2*1024*1024)/512) // 6Mbyte
#ifdef MULTI_DB_INCLUDE
#define FILENAMEBUFFER_TOTAL_HIDDEN_COUNT					(3) // 6Mbyte
#else
#define FILENAMEBUFFER_TOTAL_HIDDEN_COUNT					(1) // 6Mbyte
#endif
#define FILENAMEBUFFER_TOTAL_HIDDEN_SIZE					(FILENAMEBUFFER_ONE_HIDDEN_SIZE*FILENAMEBUFFER_TOTAL_HIDDEN_COUNT) // 6Mbyte
#else
#define STARTPAGE_FILENAMEBUFFER_HIDDEN				(INFO_NET_CONFIG + INFO_NET_CONFIG_SIZE)
#define FILENAMEBUFFER_TOTAL_HIDDEN_SIZE					(0)
#endif

#define STARTPAGE_USER_HIDDEN				(STARTPAGE_FILENAMEBUFFER_HIDDEN + FILENAMEBUFFER_TOTAL_HIDDEN_SIZE)

#ifdef TCCUI_ENGINE
// This area is used to store HDFile.tcc //
#define USER_HIDDEN_SIZE        (65536*2)   // 64MByte
#else
#define USER_HIDDEN_SIZE        0
#endif

#endif

#if defined (TCC801X)
#define NAND_HIDDEN_DEFAULT_TOTAL_PAGESIZE		(STARTPAGE_USER_HIDDEN + USER_HIDDEN_SIZE -1)
#else
#define NAND_HIDDEN_DEFAULT_TOTAL_PAGESIZE			4096
#define	MAX_ROMSIZE_NAND							0x200000
#endif

#define NAND_HIDDEN_DRIVE_DEFAULT_TOTAL_SECTOR_SIZE	((2*1024*1024UL)/512UL)

#define TCC_NAND_TRACE_ALL           0x0F
#define TCC_NAND_TRACE_DRV_WRITE     0x01
#define TCC_NAND_TRACE_DRV_READ      0x02
#define TCC_NAND_TRACE_DRV_ALL       0x03
#define TCC_NAND_TRACE_TNFTL_ALL     0x07
#define TCC_NAND_TRACE_OFF           0x00

//	FTLBlk_Composite_Num
#define	NAND_FTL_BLK_COMPOSITE_1WAY			1	// Normal: 1
#define	NAND_FTL_BLK_COMPOSITE_2WAY			2	// Multi-Plane: 2
#define	NAND_FTL_BLK_COMPOSITE_4WAY			4	// Multi-Plane + InterLeave: 4

// Low-Level Format
typedef enum {
	NAND_DRV_LOWFORMAT_TYPE_DEFAULT = 0,
	NAND_DRV_LOWFORMAT_TYPE_DEBUG_LEVEL_ERASE_ONLY = 1,
	NAND_DRV_LOWFORMAT_TYPE_DEBUG_LEVEL_V1_ROAREA = 2,
	NAND_DRV_LOWFORMAT_TYPE_DEBUG_LEVEL_V1_TOTALAREA = 3,
	NAND_DRV_LOWFORMAT_TYPE_DEBUG_LEVEL_V2_ROAREA = 4,
	NAND_DRV_LOWFORMAT_TYPE_DEBUG_LEVEL_V2_TOTALAREA = 5
} NAND_DRV_FORMAT_TYPE_T;

typedef enum {
	NAND_DRV_LOWFORMAT_TYPE_LEVEL_V1 = 0,
	NAND_DRV_LOWFORMAT_TYPE_LEVEL_V2 = 1
} NAND_DRV_FORMAT_DEPTH;

//*****************************************************************************
//*
//*
//*                       [ INTERNAL STRUCT DEFINE ]
//*
//*
//*****************************************************************************
typedef struct __tag_NAND_DrvInfo
{
	unsigned short int			DrvStatus;			// Status of Drive
	
	unsigned long int			TotalDiskSector;	// Total Disk Sector
	unsigned short int			Cylinder;
	unsigned short int			Head;
	unsigned char				Sector;

	TNFTL_DRVINFO				*NFTLDrvInfo;		// NFTL Driver
} NAND_DRVINFO;

#if defined( TNFTL_V7_INCLUDE )
//================================================
// Header + Partition Table = 1Block Size
//================================================
#define	ROM_HEADER_INFO_SIZE			512			

typedef struct __tag_NAND_PART_INFO
{
	U32				BlockUpperLimit;				//Physical Upper Block Limit
	U32				BlockLowerLimit;				//Physical Lower Block Limit
	U32				TotalBlockNum;
	U32				PartitionBlockMark;
	U32				PartitionSectorSize;
	U32				PartitionWCacheNum;
	U32				PartitionCRC;
	char			PartitionName[16];
} NAND_PART_INFO;

typedef struct __tag_NAND_GANG_PART_INFO
{
	U32				BlockUpperLimit;				//Physical Upper Block Limit
	U32				BlockLowerLimit;				//Physical Lower Block Limit
	U32				TotalBlockNum;
	U8				PartitionName[4];
} NAND_GANG_PART_INFO;

typedef struct __tag_NAND_GANG_PART_NAME
{
	char						PriPartitionName[16];
	char						ExtPartitionName[12][16];
} NAND_GANG_PART_NAME;

typedef struct __tag_ROM_PARTITION_INFO
{
	NAND_GANG_PART_INFO		NAND_Gang_Partition[16];
	U8						NAND_Partition_Num;
	NAND_PART_INFO			Partition[13];			// ROArea 2(1st Boot, 2nd Boot ), RWArea 1( Data, Hidden, Multi Hidden~)
} ROM_PARTITION_INFO;

typedef struct __tag_ROM_HEADER_INFO
{
	U32					PartitionTableOffset;
	U32					BinaryImageOffset;
	U8					HeaderSignature[24];
	U32					ImageVersion;
	// Target NAND Info
	U8					NAND_DRIVER_VERSION[5];
	U8					NAND_DEVICE_CODE[6];
	U8 					NAND_MEDIA_NUM;
	U16  				PBpV;						// Physical all Block Number
	U16  				FBpV;						// FLT all Block Number	
	U16			  		PpB;						// Page Number Per Block
	U16  				PageSize;					// Page Size
	U16	  				SpareSize;					// Spare Size
	// FTL Info
	U8					FTLBlk_Composite_Num;		// Normal NAND : 1, MP : 2, MP+IL: 4
	U32					FTLPpB;						// FTL Pages Per Block	(PhyPpB * FTL
	U32					PhyPpB;						// NAND Pages Per Block
	U32					RemapPageWeight[4];
	U8					ExtInterleaveUsable;
	U8					ECCType;
} ROM_HEADER_INFO;

typedef	struct	__tag_TNFTL_ExtPartitionSizeInfo
{
	unsigned int				ExtendedPartitionNum;
	unsigned int				ExtPartitionSize[12];
	#if defined(__USE_64_BIT_MTD__)
	unsigned long long			ROAreaSize;
	#else
	unsigned int				ROAreaSize;
	#endif
	unsigned int				ROAreaPartNum;
} TNFTL_EXT_PART_INFO;

typedef enum {
	NAND_PROGRESS_TYPE_FORMAT_START,
	NAND_PROGRESS_TYPE_FORMAT_PROCESS
} NAND_PROGRESS_TYPE_T;

typedef void (*NAND_PROGRESS_HANDLER_T) (NAND_PROGRESS_TYPE_T eType, int iPercent);
#else
typedef	struct	__tag_TNFTL_HiddenSizeInfo
{
	unsigned int				HiddenPageSize;	// Default Hidden Area Pages
	unsigned int				MultiHiddenAreaNum;		// Multi Hidden Num	
	unsigned int 				MultiHiddenSize[8];	// Multi Hidden Configuration
	unsigned int				ROAreaSize;
} TNFTL_HIDDEN_INFO;
#endif					

#if defined( TNFTL_V7_INCLUDE )
//*****************************************************************************
//*
//*
//*                       [ EXTERNAL VARIABLE DEFINE ]
//*
//*
//*****************************************************************************
extern NAND_DRVINFO				gNAND_DrvInfo[MAX_NAND_DRIVE];
extern TNFTL_DRVINFO			gTNFTL_DrvInfo[MAX_NAND_DRIVE];

extern TNFTL_EXT_PART_INFO		gTNFTL_ExtPartitionInfo[MAX_NAND_DRIVE];
extern NAND_IO_DEVINFO* 		gLBA_DevInfo[4];
#else
extern NAND_DRVINFO				gNAND_DrvInfo[MAX_NAND_DRIVE];
extern TNFTL_HIDDEN_INFO		gTNFTL_HiddenInfo[MAX_NAND_DRIVE];
extern NAND_IO_DEVINFO*  		gLBA_DevInfo[4];
#endif

#if defined(_LINUX_)
#if !defined(USE_V_ADDRESS)
extern unsigned char 			gNAND_PageBuffer[TNFTL_MAX_SUPPORT_NAND_IO_PAGE_SIZE + TNFTL_MAX_SUPPORT_NAND_IO_SPARE_SIZE] __attribute__((aligned(8)));
#endif
#endif

//*****************************************************************************
//*
//*
//*                       [ EXTERNAL FUCTIONS DEFINE ]
//*
//*
//*****************************************************************************
extern void				NAND_Init( void );
extern void 			NAND_InitHiddenInfo( void );
extern void 			NAND_SetFlagOfChangeAreaSize( unsigned char On_Off );
extern int				NAND_InitForDump( void );
extern int				NAND_LowFormat( NAND_DRV_FORMAT_TYPE_T type, NAND_PROGRESS_HANDLER_T pNAND_ProgressHandler );

extern int				NAND_DumpBootArea( U16 nImageNo, U8 *nReadBuffer, U32 *rRomSize, U32 *rGmcStatus, U32 *rExtStatus, U32 *rBootStatus, U32 *rRomCrc );
extern int 				NAND_RecoveryBootArea( U16 nImageNo );	

extern NAND_ERROR		NAND_InitDrive( int nDrvNo );
extern int				NAND_Clean_Hidden( unsigned long ulIndex );

extern int 				NAND_MTD_UpdateInit( unsigned int ImageSize );
extern int 				NAND_MTD_Update( unsigned int PageOffset, unsigned char *DataBuffer );

extern int				NAND_ReadSector( int nDrvNo, U32 LBA, U32 nSecSize, void* nReadBuffer );
extern int				NAND_WriteSector( int nDrvNo, U32 LBA, U32 nSecSize, void* nWriteBuffer );
extern int				NAND_HDReadPage( U32 nHDPageAddr, U32 nPageSize, void* nReadBuffer );
extern int				NAND_HDWritePage( U32 nHDPageAddr, U32 nPageSize, void* nWriteBuffer );
extern int 				NAND_PhyReadPage( U32 nBlkAddr, U16 nPageAddr, U16 nCSorder, void* nReadBuffer );
extern int				NAND_Ioctl( int function, void *param );

extern int				NAND_ReadMultiSectorStart( U32 LBA, U32 nSecSize );
extern int				NAND_ReadMultiSectorStop( void );
extern int				NAND_WriteMultiSectorStart( U32 LBA, U32 nSecSize );
extern int				NAND_WriteMultiSectorStop( void );
extern int				NAND_HDClearPages( U32 nHDStPageAddr, U32 nHDEdPageAddr );

extern int 				NAND_ReadSectorIRQ( int nDrvNo, ndd_work_info* nand_work_info );
extern int 				NAND_WriteSectorIRQ( int nDrvNo, ndd_work_info* nand_work_info );
extern int				NAND_HDReadSector( int nDrvNo, U32 LBA, U32 nSecSize, void* nReadBuffer );
extern int				NAND_HDWriteSector( int nDrvNo, U32 LBA, U32 nSecSize, void* nWriteBuffer );
extern int				NAND_HDIoctl( int function, void *param );

extern U16				NAND_GetSerialNumber( U8* rSerialNumber, U16 nSize );
extern U16				NAND_GetUniqueID( U8* rSerialNumber, U16 nSize );
extern TNFTL_ERROR		NAND_SetUartDebug( unsigned int on_off );

extern void				NAND_Suspend( void );
extern void				NAND_Resume( void );

extern int				NAND_MTD_Init( NAND_IO_DEVINFO *nDevInfo, U32* rMTDStBlk, U32* rMTDEdBlk, U32 nErase );
extern int				NAND_MTD_SetBadBlkTable( NAND_IO_DEVINFO *nDevInfo );
extern int				NAND_MTD_GetPhyBlkAddr( NAND_IO_DEVINFO *nDevInfo, U32 nMTDBlkAddr, U32 *rMTDPhyBlkAddr, U32 *rChipNo );
extern int 				NAND_MTD_ReadPage( U32 nPageAddr, U8* nPageBuffer );

extern U32 				NAND_MTD_SetupMTDReservedBlock( unsigned int nPBpV, unsigned int nBBpZ, U32 nMTDBlockNum );
extern int				NAND_GetGoldenBadMarkofPBlock(unsigned int nChipNo, unsigned int nPhyBlkAddr);
extern NAND_IO_ERROR 	NAND_SetGoldenBadMarkofPBlock( NAND_IO_DEVINFO *nDevInfo );
extern unsigned long	NAND_GetROAreaSize(void);

#endif

