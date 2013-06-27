/**************************************************************************************
 *
 * TELECHIPS Co.
 * 
 * 6th floor Corad Bldg 1000-12 Daechi-dong, KangNam-ku, Seoul, Korea
 * ====================================================================================
 *
 * Name:        Hyun-Chul Hong
 * Phone:       82-2-3443-6792
 * E-mail:      hchong@telechips.com
 *
 * FILE NAME:   TNFTL.H
 *
 * DESCRIPTION:
 *       This is a Header File for TNFTL [ Telechips Nand Flash Translation Layer ]
 *
 *
 * FILE HISTORY:
 * 	Date: 2005.04.19	Start source coding		By Hyunchul Hong
 *
 **************************************************************************************/
#ifndef __TNFTL_H
#define __TNFTL_H

#if defined(_LINUX_)
#include <tnftl/nand_io_v7.h>
#elif defined(_WINCE_)
#include "nand_io_v7.h"
#else
#include "nand_io_v7.h"
#endif

#if defined(TCC92XX) || defined(TCC89XX) || defined(TCC93XX) || defined(TCC88XX) || defined(TCC801X) || defined(TCC892X)
#if defined(NAND_BOOT_REV)
#define TNFTL_V7_INCLUDE
#else
#define TNFTL_V6_INCLUDE
#endif
#elif defined(TCC82XX)||defined(TCC83XX)||defined(TCC79X) || defined(TCC81XX) || defined(TCC80XX)
#define TNFTL_V5_INCLUDE
#else
#define TNFTL_V4_INCLUDE
#endif

//*****************************************************************************
//*
//*
//*                       [ General DEFINE & TYPEDEF ]
//*
//*
//*****************************************************************************
#ifndef NOT_USED
#define NOT_USED				0
#endif
#ifndef USED
#define	USED					1
#endif
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
#ifndef NORMAL
#define NORMAL           		0
#endif
#ifndef STRICT
#define STRICT            		1
#endif
#ifndef NULL
#define NULL					0
#endif
#ifndef TNFTL_U16_NULL
#define TNFTL_U16_NULL			0xFFFF
#endif
#ifndef TNFTL_U32_NULL
#define TNFTL_U32_NULL			0xFFFFFFFF
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

#ifndef TNFTL_EXTENDED_PARTITION_MAX_NUM
#define TNFTL_EXTENDED_PARTITION_MAX_NUM	4
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
	ERR_TNFTL_NO_EXIST_NAND_FLASH									= 0xA200000,
	ERR_TNFTL_NO_BMP												= 0xA200001,
	ERR_TNFTL_NOT_READY_IO											= 0xA200002,
	ERR_TNFTL_NOT_READY_AREA										= 0xA200003,
	ERR_TNFTL_NOT_EXIST_NANDFLASH									= 0xA200004,
	ERR_TNFTL_NOT_EXIST_MEDIA_NUM_INFO								= 0xA200005,
	ERR_TNFTL_NOT_HAVE_ENOUGH_ZONE_BLOCK							= 0xA200006,
	ERR_TNFTL_NOT_EXIST_HIDDEN_HEADBLOCK							= 0xA200007,
	ERR_TNFTL_NOT_EXIST_NANDBOOT_AREA								= 0xA200008,
	ERR_TNFTL_NOT_HIDDEN_WRITE_CACHE_BLOCK							= 0xA200009,
	ERR_TNFTL_NOT_EXIST_DATA_IN_WRITE_CACHE_PAGE_BLOCK				= 0xA20000A,
	ERR_TNFTL_NOT_FRESH_NANDFLASH									= 0xA20000B,
	ERR_TNFTL_NOT_EXIST_FTL_BAD_BLOCK_INFO							= 0xA20000C,
	ERR_TNFTL_FAILED_CHECK_BMP										= 0xA20000D,
	ERR_TNFTL_FAILED_GET_REAL_PAGE_ADDR								= 0xA20000E,
	ERR_TNFTL_FAILED_ALLOCATE_MEM_AND_LINK_DRIVER					= 0xA20000F,
	ERR_TNFTL_FAILED_GOLDEN_PAGE									= 0xA200010,
	ERR_TNFTL_FAILED_GET_SHIFT_FACTOR_FOR_MULTIPLY					= 0xA200011,
	ERR_TNFTL_FAILED_GET_EMPTY_RAW_BLOCK							= 0xA200012,
	ERR_TNFTL_FAILED_MAKE_BMP										= 0xA200013,
	ERR_TNFTL_FAILED_GET_MEDIA_UID									= 0xA200014,
	ERR_TNFTL_FAILED_GET_MEDIA_SID									= 0xA200015,
	ERR_TNFTL_FAILED_GET_PBLOCK_STATUS_ON_BMP						= 0xA200016,
	ERR_TNFTL_FAILED_GET_NANDBOOT_AREA								= 0xA200017,
	ERR_TNFTL_FAILED_MAKE_HIDDEN_LBT_BLOCK							= 0xA200018,
	ERR_TNFTL_FAILED_GET_EMPTY_HIDDEN_BLOCK							= 0xA200019,
	ERR_TNFTL_FAILED_MAKE_HIDDEN_AREA								= 0xA20001A,
	ERR_TNFTL_FAILED_GET_FTL_PAGE_ADDRESS_OF_SECTOR_ADDRESS			= 0xA20001B,
	ERR_TNFTL_FAILED_GET_HIDDEN_WRITE_CACHE_NUMBER					= 0xA20001C,
	ERR_TNFTL_FAILED_GET_HIDDEN_WRITE_MAINPB_INFO					= 0xA20001D,
	ERR_TNFTL_FAILED_GET_EMPTY_FTL_BLOCK_FOR_WRITE_CACHE			= 0xA20001E,
	ERR_TNFTL_FAILED_GET_EMPTY_FTL_BLOCK							= 0xA20001F,
	ERR_TNFTL_FAILED_GET_EMPTY_PHYSICAL_BLOCK						= 0xA200020,
	ERR_TNFTL_FAILED_GET_LPTSCAN_BUFFER								= 0xA200021,
	ERR_TNFTL_TRY_ERASE_0TH_BLOCK									= 0xA200022,
	ERR_TNFTL_OVERSIZE_ZONES										= 0xA200023,
	ERR_TNFTL_WRONG_PARAMETER										= 0xA200024,
	ERR_TNFTL_WCACHE_FORCE_FLUSH									= 0xA200025,
	ERR_TNFTL_PBLOCK_IS_BAD_BLOCK									= 0xA200026,
	ERR_TNFTL_LPT_CORRUPTED_SEG_NO									= 0xA200027,
	ERR_TNFTL_CORRUPTED_HIDDEN_AREA									= 0xA200028,
	ERR_TNFTL_SET_OVER_HIDDEN_PAGE_SIZE								= 0xA200029,
	ERR_TNFTL_OVERSIZE_MULTIHIDDEN_NUM								= 0xA20002A,
	ERR_TNFTL_HAVE_MANY_BAD_BLOCKS_IN_ZONE							= 0xA20002B,
	ERR_TNFTL_MEDIA_HAVE_OVERSIZE_BAD_BLOCKS						= 0xA20002C,
	ERR_TNFTL_IMPOSSIBLE_TO_SET_SIZE_OF_DATA_AREA					= 0xA20002D,
	ERR_TNFTL_NANDBOOT_AREA_OVER_SIZE								= 0xA20002E,
	ERR_TNFTL_HIDDEN_AREA_OVER_SIZE									= 0xA20002F,
	ERR_TNFTL_DEBUG_WRONG_PARAMETER									= 0xA200030,
	ERR_TNFTL_DEBUG_WRONG_COMMAND_PARAMETERS						= 0xA200031,
	ERR_TNFTL_DEBUG_WRONG_RECEIVED_SECTOR_DATA						= 0xA200032,
	ERR_TNFTL_DEBUG_WRONG_SEND_SECTOR_DATA							= 0xA200033,
	ERR_TNFTL_WRONG_MATCH_UID_NBAREA								= 0xA200034,
	ERR_TNFTL_OVERSIZE_PPAGE_NUM									= 0xA200035,
	ERR_TNFTL_OVERSIZE_MTD_AREA_SIZE								= 0xA200036,
	ERR_TNFTL_DETECT_BMP_MARKER_CORRUPTION							= 0xA200037
} TNFTL_ERROR;

typedef enum
{
	ERR_TNFTL_NOT_EXIST_MEMINIT_BLOCK								= 0x0010001,
	ERR_TNFTL_WRONG_CRC_MEMINIT_BLOCK								= 0x0010002,
	ERR_TNFTL_NOT_EXIST_MEMINIT_MIRROR_BLOCK						= 0x0010004,
	ERR_TNFTL_WRONG_CRC_MEMINIT_MIRROR_BLOCK						= 0x0010008,
	ERR_TNFTL_WRONG_CRC_GOLDENINFO									= 0x0010010,
	ERR_TNFTL_WRONG_CRC_SECOND_BOOT_CODE							= 0x0010020,	
	ERR_TNFTL_WRONG_CRC_CODE_NANDBOOT_ROMFILE						= 0x0010040	
} BOOT_AREA_CHECK_ERR;

typedef struct
{
	U32	Signature;
	U16	PageSize;
	U16	SpareSize;
	U16	Ppage;
	U16	PpB;
	U32 PpBforAddress;
	U16 EccType;
	U16 EccDataSize;
	U32	TargetAddress;	// Working Address
	U32	BootLoaderSize;
	U32 LastBlockPpB;
	U16	RomCopyNum;
	U16	RomBlockNum;
	U32	RomCRC;
	U32	BlockUppuerLimit;
	U32	BlockLowerLimit;
    #if defined(TCC93XX) || defined(TCC88XX) || defined(TCC801X) || defined(TCC892X)
    U8  CAHLD;
    U8  CAPW;
    U8  CASTP;
    U8  CmdAddrCycSetup;
    U8  RDHLD;
    U8  RDPW;
    U8  RDSTP;
    U8  ReadCycSetup;
    #endif
//---------------------------------- 44Byte
	U32	GoldenInfoCRC;
	U32 SecureData[4];

//=====================================	BootLoader Info

	U16 BL_ColCycle;
	U16 BL_RowCycle;
	U32 BL_ReadCycle;
	U32 BL_MediaType;
	U32 BL_SpareDataSize;
	U32 BL_TargetAddress;	// Working Address
	U32	BL_MemInitCodeSize;
	U32	BL_MemInitCodePpB;
	U32	BL_RomFileSize;
	U16	BL_RomCopyNum;
	U16	BL_RomBlockNum;
	U32	BL_BlockUppuerLimit;
	U32	BL_BlockLowerLimit;
	U32 BL_LastBlockPpB;
	U32 BL_BytesPerSector;
	U32	BL_MemInitCodeCRC;

	U16 NAND_Drv_Version;

} 	GOLDEN_INFO;

#define TNFTL_MAX_BLOCK_NUM_OF_NBAREA							100

//*****************************************************************************
//*
//*
//*                          [ INTERNAL DEFINATION ]
//*
//*
//*****************************************************************************
#define TNFTL_MAX_SUPPORT_DRIVER								2
#define TNFTL_MAX_SUPPORT_MULTI_NANDFLASH						4
#define TNFTL_MAX_SUPPORT_NAND_IO_PAGE_SIZE						32768		// 8192*2(Parallel)*2(MultiPlane)
#define TNFTL_MAX_SUPPORT_NAND_IO_SPARE_SIZE					2048		// 436(:=512)*4
#define TNFTL_MAX_SUPPORT_NAND_IO_SECTOR_SIZE_PER_1PAGE			32			// 16384 / 512
#define TNFTL_MAX_SUPPORT_NAND_IO_PAGE_SIZE_PER_1BLOCK			1024		// PpB * 2 (Multi-Plane) * 2 (InterLeave)
#define TNFTL_MAX_SUPPORT_MULTI_PLANE							4
#define TNFTL_MAX_SUPPORT_SHIFT_MULTI_PLANE						2
#define TNFTL_MAX_SUPPORT_BAD_BLOCKS_IN_1ZONE					20
#define TNFTL_MAX_SUPPORT_NB_AND_HD_AREA_BLOCK_SIZE				900			// 900 Blocks
#define TNFTL_MAX_VALID_BAD_BLOCK_PER_1ZONE						40			
#define TNFTL_MAX_SUPPORT_ZONE									32
#define TNFTL_MAX_SUPPORT_NAND_PPAGE_NUM						32

#define TNFTL_PLANE_0											0
#define TNFTL_PLANE_1											1
#define TNFTL_PLANE_2											2
#define TNFTL_PLANE_3											3

#define TNFTL_PHY_BLOCKS_1ZONE									1024
#define TNFTL_PHY_BLOCKS_1ZONE_SHIFT							10			// 2^10 = 1024

#define TNFTL_BMP_HEADER_SIZE									4			// 4Bytes
#define TNFTL_BMP_FLAGS_IN_1BYTE								4			// Flag Size is 2Bit
#define TNFTL_BMP_FLAGS_IN_1BYTE_SHIFT							2			// Shift Factor
#define	TNFTL_BMP_START_PAGE									2
#define TNFTL_BMP_BAD_BLOCK_CACHE_SIZE							64			// 64*2 = 128Bytes
#define TNFTL_BMP_STATUS_OK										0x0000
#define TNFTL_BMP_STATUS_FAIL_NO_BMP1_SERIAL					0x0001
#define TNFTL_BMP_STATUS_FAIL_NO_BMP1_PARALLEL					0x0010
#define TNFTL_BMP_STATUS_FAIL_NO_BMP2_PARALLEL					0x0100
#define TNFTL_BMP_STATUS_FAIL_OTHER_REASON						0x1000
#define TNFTL_BMP_STATUS_GANG_WRITE_NAND						0x2000

#define TNFTL_BMP_MARKER_CHECK_MODE								0
#define TNFTL_BMP_MARKER_CORRECTION_MODE						1

#define TCC_NAND_VER_LOCATION									sizeof(GOLDEN_INFO) - sizeof(U16)
#define TCC_NAND_DRV_VERSION									7160

#define TNFTL_BSA_SCAN_BY_FACTORY_MARK							0
#define TNFTL_BSA_SCAN_BY_CHECK_PATTERN							1
#define TNFTL_BSA_GET_BY_BMP									0
#define TNFTL_BSA_GET_BY_CHECK									1
#define TNFTL_BSA_GET_BY_SPARE									2
#define TNFTL_BSA_GET_BY_FTLBBINFO								3

#define TNFTL_LPT_NOT_CHANGED									0x00
#define TNFTL_LPT_CHANGED										0x01
#define TNFTL_LPT_TABLE_VALUES_1PAGE							128
#define TNFTL_LPT_TABLE_VALUES_1PAGE_SHIFT						7
#define TNFTL_LPT_TABLE_PAGES_1BLOCK							8
#define TNFTL_LPT_TABLE_PAGES_1BLOCK_SHIFT						3
#define TNFTL_LPT_TABLE_VALUES_1BLOCK							1024
#define TNFTL_LPT_TABLE_VALUES_1BLOCK_SHIFT						10
#define TNFTL_LPT_TABLE_DATA_START_PAGE							1

#define TNFTL_MAX_SERIAL_NUMBER_SIZE							32
#define TNFTL_MAX_NANDFLASH_SID_SIZE							32
#define TNFTL_MAX_NANDFLASH_UID_SIZE							16

#define TNFTL_HIDDEN_DEFAULT_TOTAL_SECTOR_SIZE					6083
#define TNFTL_HIDDEN_DEFAULT_SPARE_BLOCK_SIZE					8
#define TNFTL_HIDDEN_BLOCK_SIZE_FOR_SYSTEM_BLOCK				3			// HEAD 1 + LBT BLOCK 2

#define TNFTL_MAX_SUPPORT_MULTI_HIDDEN_AREA_NUM					10

#define TNFTL_BAA_INVALID_BLOCK_MARK1							0xFF
#define TNFTL_BAA_INVALID_BLOCK_MARK2							0xEE
#define TNFTL_BAA_INVALID_ADDRESS								0xFFFFFE
#define TNFTL_INVALID_LOG_ADDRESS								0xFFFFFFEE

#define TNFTL_BLOCK_AREA_MASK									0xF0
#define TNFTL_BLOCK_AREA_DATA_AREA								0x10
#define TNFTL_BLOCK_AREA_HIDDEN_AREA							0x20
#define TNFTL_BLOCK_AREA_MULTI_HIDDEN_AREA0						0x30
#define TNFTL_BLOCK_AREA_MULTI_HIDDEN_AREA1						0x40
#define TNFTL_BLOCK_AREA_MULTI_HIDDEN_AREA2						0x50
#define TNFTL_BLOCK_AREA_MULTI_HIDDEN_AREA3						0x60
#define TNFTL_BLOCK_AREA_MULTI_HIDDEN_AREA4						0x70
#define TNFTL_BLOCK_AREA_MULTI_HIDDEN_AREA5						0x80
#define TNFTL_BLOCK_AREA_MULTI_HIDDEN_AREA6						0x90
#define TNFTL_BLOCK_AREA_MULTI_HIDDEN_AREA7						0xA0
#define TNFTL_BLOCK_AREA_MULTI_HIDDEN_AREA8						0xB0
#define TNFTL_BLOCK_AREA_MULTI_HIDDEN_AREA9						0xC0
#define TNFTL_BLOCK_AREA_NANDBOOT_AREA							0xE0


#define TNFTL_BLOCK_AREA_PRIMARY_PARTITION						0x10

#define TNFTL_BLOCK_AREA_EXTENDED_PARTITION_0					0x20
#define TNFTL_BLOCK_AREA_EXTENDED_PARTITION_1					0x30
#define TNFTL_BLOCK_AREA_EXTENDED_PARTITION_2					0x40
#define TNFTL_BLOCK_AREA_EXTENDED_PARTITION_3					0x50
#define TNFTL_BLOCK_AREA_EXTENDED_PARTITION_4					0x60
#define TNFTL_BLOCK_AREA_EXTENDED_PARTITION_5					0x70
#define TNFTL_BLOCK_AREA_EXTENDED_PARTITION_6					0x80
#define TNFTL_BLOCK_AREA_EXTENDED_PARTITION_7					0x90
#define TNFTL_BLOCK_AREA_EXTENDED_PARTITION_8					0xA0
#define TNFTL_BLOCK_AREA_EXTENDED_PARTITION_9					0xB0
#define TNFTL_BLOCK_AREA_EXTENDED_PARTITION_10					0xC0
#define TNFTL_BLOCK_AREA_EXTENDED_PARTITION_11					0xD0


#define TNFTL_BLOCK_TYPE_MASK									0x0F
#define TNFTL_BLOCK_TYPE_HEADER									0x01
#define TNFTL_BLOCK_TYPE_LBT									0x02
#define TNFTL_BLOCK_TYPE_DATA									0x03
#define TNFTL_BLOCK_TYPE_WRITE_CACHE_PAGE						0x05
#define TNFTL_BLOCK_TYPE_WRITE_CACHE_PAGE_2						0x06
#define TNFTL_BLOCK_TYPE_WRITE_CACHE_SUB						0x07
#define TNFTL_BLOCK_TYPE_WRITE_CACHE_PAGE_SERIAL				0x08
#define TNFTL_BLOCK_TYPE_WRITE_CACHE_PAGE_2_SERIAL				0x09


#define TNFTL_BLOCK_TYPE_NB_MASTER								0x01
#define TNFTL_BLOCK_TYPE_NB_MASTER_BACKUP						0x02
#define TNFTL_BLOCK_TYPE_NB_DATA								0x03
#define TNFTL_BLOCK_TYPE_NB_BOOTLOADER							0x04
#define TNFTL_BLOCK_TYPE_NB_MEMINIT_BACKUP						0x05

#define TNFTL_WRITE_CACHE_PAGE_OFFSET							0x01

#define TNFTL_READ_SECTOR_FROM_NULL_BLOCK						0x00
#define TNFTL_READ_SECTOR_FROM_MAIN_BLOCK						0x01
#define TNFTL_READ_SECTOR_FROM_WCACHE_SUB_BLOCK					0x02
#define TNFTL_READ_SECTOR_FROM_WCACHE_PAGE_BLOCK				0x03
#define TNFTL_WRITE_SECTOR_TO_MAIN_BLOCK						0x04
#define TNFTL_WRITE_SECTOR_TO_WCACHE_SUB_BLOCK					0x05
#define TNFTL_WRITE_SECTOR_TO_WCACHE_PAGE_BLOCK					0x06

#define TNFTL_FORMAT_AREA_QUICK_LEVEL							0x00
#define TNFTL_FORMAT_AREA_LOW_LEVEL								0x01

typedef void (*TNFTL_CALLBACK_HANDLER) ( U16 nDrvNo, U16 nStatusCode, U32 wParam );
#define TNFTL_CALLBACK_LCD_MAKE_HIDDEN_PAGE_START				0x00
#define TNFTL_CALLBACK_LCD_MAKE_HIDDEN_PAGE_PROCESS				0x01
#define TNFTL_CALLBACK_LCD_MAKE_HIDDEN_PAGE_STOP				0x02
#define TNFTL_CALLBACK_LCD_FORMAT_START							0x03
#define TNFTL_CALLBACK_LCD_FORMAT_PROCESS						0x04
#define TNFTL_CALLBACK_LCD_FORMAT_STOP							0x05
#define TNFTL_CALLBACK_LCD_BMP_UPDATE_START						0x06
#define TNFTL_CALLBACK_LCD_BMP_UPDATE_PROCESS					0x07
#define TNFTL_CALLBACK_LCD_BMP_UPDATE_STOP						0x08

#define PHY_BAD_BLOCK_MASK										0xFF000000
#define PHY_BAD_BLOCK_0											0x01000000
#define PHY_BAD_BLOCK_1											0x02000000
#define PHY_BAD_BLOCK_2											0x04000000
#define PHY_BAD_BLOCK_3											0x08000000
#define PHY_BAD_BLOCK_4											0x10000000
#define PHY_BAD_BLOCK_5											0x20000000
#define PHY_BAD_BLOCK_6											0x40000000
#define PHY_BAD_BLOCK_7											0x80000000

#define TCC_NAND_RESERVED_BADBLOCK_ALL_ON						0x00000FF
#define TCC_NAND_RESERVED_BADBLOCK_ALL_OFF						0x0000000
#define TCC_NAND_RESERVED_BADBLOCK_MP_ON     					0x0000001
#define TCC_NAND_RESERVED_BADBLOCK_MP_OFF    					0x0000000
#define TCC_NAND_RESERVED_BADBLOCK_MCP_ON     					0x0000002
#define TCC_NAND_RESERVED_BADBLOCK_MCP_OFF    					0x0000000
#define TCC_NAND_RESERVED_BADBLOCK_IL_ON     					0x0000004
#define TCC_NAND_RESERVED_BADBLOCK_IL_OFF    					0x0000000
#define TCC_NAND_RESERVED_BADBLOCK_EIL_ON     					0x0000008
#define TCC_NAND_RESERVED_BADBLOCK_EIL_OFF    					0x0000000
#define TCC_NAND_RESERVED_BADBLOCK_PARALLEL_ON     				0x0000010
#define TCC_NAND_RESERVED_BADBLOCK_PARALLEL_OFF     			0x0000000

#define WCACHE_NORMAL_USED_MODE									0x00
#define WCACHE_FORCE_SETUP_MODE									0x01

#define	WCACHE_SERIAL_PAGE_MODE									0x00
#define	WCACHE_NORMAL_PAGE_MODE									0x01

#define RCACHE_HIT_SLOT_ENTRY									0x00
#define RCACHE_NOHIT_SLOT_ENTRY									0x01
#define RCACHE_MAKE_NEW_SLOT_ENTRY								0x02

#define	TNFTL_SECURE_SIZE										0x20000			// 128KBytes

#define TNFTL_ROAREA_NEW_TYPE									0x00
#define TNFTL_ROAREA_OLD_TYPE									0x01

#if defined(_WINCE_)
#define	NAND_MTD_PARTITION_NUM						0
#elif defined(_LINUX_)
#define	NAND_MTD_PARTITION_NUM						10
#else
#define	NAND_MTD_PARTITION_NUM						0
#endif

#define __USE_64_BIT_MTD__							

//*****************************************************************************
//*
//*
//*                       [ INTERNAL STRUCT DEFINE ]
//*
//*
//*****************************************************************************
typedef	void (*TNFTL_FUNC1)(void);
typedef	void (*TNFTL_FUNC2)(unsigned int, unsigned int);
typedef	void (*TNFTL_FUNC3)(unsigned int);

typedef unsigned long int		TNFTL_LPT_TABLE_VALUE;
typedef unsigned long int		TNFTL_LPT_BLOCK;
typedef unsigned long int		TNFTL_RCACHE_SLOT;

typedef struct __tag_TNFTL_INIT_INFO
{
	U8	SerialNumber[64];
	U8	BMPSignature[4];
	U32	ExtendedPartitionNo; 		// Extended Partition Number
	U32	ExtendedPartitionSectorSize[10];
	#if defined(__USE_64_BIT_MTD__)
	U64 ROAreaSize;
	#else
	U32	ROAreaSize;
	#endif
	U32 ROAreaStPB;
} TNFTL_INIT_INFO;

typedef struct __tag_TNFTL_PAGE_INFO
{
	unsigned int				PageIndex;
	unsigned long int			FTLPageAddr[TNFTL_MAX_SUPPORT_NAND_IO_PAGE_SIZE_PER_1BLOCK];
	unsigned short int			StartPPageNo[TNFTL_MAX_SUPPORT_NAND_IO_PAGE_SIZE_PER_1BLOCK];
	unsigned short int			PPageSize[TNFTL_MAX_SUPPORT_NAND_IO_PAGE_SIZE_PER_1BLOCK];
} TNFTL_PAGE_INFO;

typedef struct __tag_TNFTL_LPT
{
	unsigned short int			Usable;
	unsigned short int			Changed;
	unsigned short int			SegNo;
	unsigned short int			MaxTableBlkNums;
	unsigned short int			CurrentPlane;
	TNFTL_LPT_BLOCK				*BlkNo;
	TNFTL_LPT_TABLE_VALUE		Tables[TNFTL_LPT_TABLE_VALUES_1BLOCK];
	
} TNFTL_LPT;

typedef struct __tag_TNFTL_MAINPB_INFO
{
	unsigned short int			Used;
	unsigned long int			LBNo;
    unsigned long int			MainPBNo;
    unsigned short int			WrEndPage;
} TNFTL_MAINPB_INFO;

typedef struct __tag_TNFTL_ALIGNCACHE_INFO
{
	unsigned short int			Usable;
	unsigned short int			CurUsedSecSize;
	unsigned long int			StartSecAddr;
	unsigned long int			LimitSecSize;
	
} TNFTL_ALIGNCACHE_INFO;

typedef struct __tag_TNFTL_READ_CACHE
{	
	unsigned short int 			Used;		
	unsigned short int			OneSlotSize;		
	unsigned short int 			ShiftOneSlotSize;
	unsigned short int 			HitEntryPos;
	unsigned short int			lastSlotNo; 
	unsigned short int   		MaxSlotNums;
	unsigned long  int			MaxMemorySize;
	
	TNFTL_RCACHE_SLOT			*Entry;
} TNFTL_RCACHE;


typedef struct __tag_TNFTL_WRITE_CACHE
{
	unsigned short int			Used;
	unsigned short int			CacheNo;

	unsigned short int			PageCacheMode;
	unsigned short int			ScanStatus;
	
	unsigned long int			UsedNo;
	unsigned long int			LBNo;
	unsigned long int			MainPBNo;
	unsigned long int			SubPBNo;
	unsigned long int			PagePBNo;
	unsigned long int			PagePBNo2;

	unsigned long int			nPoint;

	unsigned short int			MainPBWrEndPage;
	unsigned short int 			SubPBWrEndPage;
	unsigned short int 			PagePBWrEndPage;

	unsigned short int 			PageLastPBPageNo;

	unsigned short int 			PagePBStatus[TNFTL_MAX_SUPPORT_NAND_IO_PAGE_SIZE_PER_1BLOCK * 2];

} TNFTL_WCACHE;

typedef struct __tag_TNFTL_RO_AREA
{
	unsigned short int			Usable;
	unsigned long int			BlkAreaCode;

	unsigned long int			StPB;							// Start Block Number [ FTL Blcok Addr ]
	unsigned long int			EdPB;							// End Block Number   [ FTL Blcok Addr ]
	unsigned long int			PBpV;							// Physical Block Size

	unsigned long int			StPBAddr;						// Start Phy Block Addr
	unsigned long int			EdPBAddr;						// End Phy Block Addr
	
} TNFTL_RO_AREA;

typedef struct __tag_TNFTL_RW_AREA
{
	unsigned short int			Usable;
	unsigned long int			BlkAreaCode;
	unsigned long int			HeaderBlkAddr;

	unsigned long int			PBpV;							// Physical Blocks
	unsigned long int			DataPBpV;						// Area Data Block Size
	unsigned long int			DataPPpV;						// Area Data Page Size
	unsigned long int			TotalSectorSize;
	unsigned short int			FlagOfChangeTotalSectorSize;
	unsigned short int 			LptNums;						// Area Needs LPT Number
	unsigned short int			WCacheNums;						// Write Cache Number
	unsigned short int			WCacheMode;
	unsigned long int			WCacheDataStartLBNo;

	unsigned short int			MainPBInfoNums;					// Main page info Number

	unsigned short int 			RCacheUsable;
	unsigned short int			DrvStatus;						// Status of Drive
	unsigned long int			TotalDiskSector;				// Total Disk Sector
	unsigned short int			Cylinder;
	unsigned short int			Head;
	unsigned char				Sector;	
	
	TNFTL_LPT					Lpt;
	TNFTL_MAINPB_INFO			*MainPBInfo;
	TNFTL_WCACHE				*WCaches;
	TNFTL_RCACHE				RCaches;

	unsigned char				*ReadCacheBuffer;
} TNFTL_RW_AREA;

typedef struct __tag_TNFTL_DRVINFO
{
	TNFTL_RO_AREA				NBArea;							// NANDBOOT Area
	TNFTL_RW_AREA				PriPartition;					// Primary Partition
	TNFTL_RW_AREA				*ExtPartition;					// Extended Partition[n]

	TNFTL_ALIGNCACHE_INFO		AlignCache;

	unsigned short int 			ExtendedPartitionNo;			// Extended Partition Number
	unsigned char	 			FlagOfChangePartition;

	
	unsigned char				NANDType;
	unsigned short int			IoStatus;
	unsigned short int			IoDrvNo;
	unsigned short int			IoStCS;
	unsigned short int			IoEdCS;
	unsigned short int			IoBMPFlag;						// Flag of BMP
	unsigned long int			DrvReturnValue;


	
	unsigned long int	  		PBpV;							// Physical all Blocks
	unsigned short int			BBpZ;							// Bad Blocks per 1ZONE
	unsigned short int  		PpB;							// Pages Per Block
	unsigned short int			PageSize;						// PageSize
	unsigned short int			SpareSize;						// SpareSize
	unsigned short int			PPages;							// Total Partial Pages [ 512+16Bytes ]
	unsigned short int  		ShiftPpB;						// Shift Factor of Pages Per Block
	unsigned short int			ShiftPPages;					// Shift Factor of Total Partial Pages

	unsigned long int			RwAreaStBlk;					// RW Area Start Block Addr
	unsigned long int			RwAreaEdBlk;					// RW Area End Block Addr
	#if defined(__USE_64_BIT_MTD__)
	unsigned long long			ROAreaSize;						// RO Area Size
	#else
	unsigned long int			ROAreaSize;						// RO Area Size
	#endif
	unsigned long int			ROAreaPartNum;					// RO Area Partnum
	unsigned long int			ROAreaStPB;
	unsigned long int			ROAreaBlkNum;
	unsigned short int			RWAreaFormatFlag;

	unsigned short int  		MPlanePpB;						// FTLInfo->PpB >> FTLInfo->ShiftMPlane
	unsigned short int  		ILMPlanePpB;					// FTLInfo->PpB >> 2
	unsigned short int			MPlane;							// Multi Plane Num
	unsigned short int			ShiftMPlane;					// Shift Factor of Multi Plane
	unsigned long int 			RoundMPlane;
	unsigned long int 			RemainMPlane;
	unsigned long int 			LPTBufferSize;
	unsigned short int			LPTNumsOfArea[1 + TNFTL_EXTENDED_PARTITION_MAX_NUM];

	unsigned short int			LPT_Values_1Page;
	unsigned short int			LPT_Values_1Page_Shift;
	unsigned short int			LPT_Pages_1Block;
	unsigned short int			LPT_Pages_1Block_Shift;
	unsigned short int			LPT_Block_PPage;

	unsigned short int			FTLBadBlockStatus;
	unsigned long int			FTLBadBlockNum;
	unsigned long int			FTLBadBlock[TNFTL_MAX_VALID_BAD_BLOCK_PER_1ZONE * TNFTL_MAX_SUPPORT_ZONE];

	unsigned short int			MediaNums;
	unsigned short int			ShiftMediaNums;
	unsigned short int			MediaBadBlockNum[TNFTL_MAX_SUPPORT_MULTI_NANDFLASH];

	unsigned short int			ECCStatus;

	unsigned char				MediaSizeOfSerialID;
	unsigned char				MediaSizeOfUniqueID;
	unsigned char				MediaSerialID[TNFTL_MAX_NANDFLASH_SID_SIZE];
	unsigned char				MediaUniqueID[TNFTL_MAX_NANDFLASH_UID_SIZE];
	unsigned char				MediaRawUniqueID[TNFTL_MAX_NANDFLASH_UID_SIZE];
	unsigned long int			MediaTotalFTLPageAddr[TNFTL_MAX_SUPPORT_MULTI_NANDFLASH];
	unsigned long int			MediaTotalFTLBlkAddr[TNFTL_MAX_SUPPORT_MULTI_NANDFLASH];
	unsigned short int			MediaRatioPSize[TNFTL_MAX_SUPPORT_MULTI_NANDFLASH];
	unsigned short int			MediaRatioBSize[TNFTL_MAX_SUPPORT_MULTI_NANDFLASH];
	unsigned short int			MediaRatioShiftPSize[TNFTL_MAX_SUPPORT_MULTI_NANDFLASH];
	unsigned short int			MediaRatioShiftBSize[TNFTL_MAX_SUPPORT_MULTI_NANDFLASH];
	unsigned long int 			StPBForGetEmptyPB[TNFTL_MAX_SUPPORT_MULTI_PLANE];
	NAND_IO_DEVINFO				MediaDevInfo[TNFTL_MAX_SUPPORT_MULTI_NANDFLASH];

	TNFTL_PAGE_INFO				FTLPageInfo;
	unsigned char				*FTLPageBuffer;
	unsigned char				*AlignCacheBuffer;
	unsigned int				*LPTBuffer;
	
} TNFTL_DRVINFO;

typedef struct __tag_Tcc_MTD_ReservedArea{
	unsigned int				PartBlkNum;		// Partition Block Number
	unsigned int 				BadBlkNum;		// Bad Block Number in Partition
	unsigned int 				AccBadBlkNum;	// Acc Bad Block Number upper Partition
} TNFTL_MTDBadBlkInfo;

//*****************************************************************************
//*
//*
//*                       [ EXTERNAL VARIABLE DEFINE ]
//*
//*
//*****************************************************************************



//*****************************************************************************
//*
//*
//*                       [ EXTERNAL FUCTIONS DEFINE ]
//*
//*
//*****************************************************************************
extern void						TNFTL_Set_RawColumnAddr(NAND_IO_DEVINFO *pDevInfo, unsigned int uiRawAddr, unsigned int uiColumnAddr);
extern void						TNFTL_Set_ColumnAddr(unsigned int uiColumnAddr);
extern unsigned int				TNFTL_Get_RandOffset(void);
extern void						TNFTL_Set_RandOffset(unsigned int uiRandOffset);
extern char						TNFTL_Randomizer_Enable(char fFlag);
extern void						TNFTL_Randomize(void *pvDst, const void *pvSrc, unsigned int uiLength);

extern TNFTL_ERROR				TNFTL_AllocMemAndLinkForDriver( U16 nDrvNo, TNFTL_DRVINFO *nMDrvInfo, TNFTL_DRVINFO **nPDrvInfo );
extern TNFTL_ERROR				TNFTL_AllocMemAndLinkForLPT( U16 nDrvNo, TNFTL_RW_AREA *nArea, TNFTL_LPT_BLOCK *nLptTableBlk, U32 nLptTableBlkSize );
extern TNFTL_ERROR				TNFTL_AllocMemAndLinkForWCACHE( U16 nDrvNo, TNFTL_RW_AREA *nArea, TNFTL_WCACHE *nWCaches, U32 nWCacheNum );
extern TNFTL_ERROR 				TNFTL_AllocMemAndLinkForRCACHE( U16 nDrvNo, TNFTL_RW_AREA *nArea, TNFTL_RCACHE_SLOT *nRCacheSlots );

extern TNFTL_ERROR 				TNFTL_AllocMemAndLinkForMainPBInfo( U16 nDrvNo, TNFTL_RW_AREA *nArea, TNFTL_MAINPB_INFO *nMainPBInfo, U32 nMainPBInfoNum );
extern TNFTL_ERROR				TNFTL_AllocMemAndLinkForPageBuffer( U16 nDrvNo, U8 *nPageBuffer );
extern TNFTL_ERROR 				TNFTL_AllocMemAndLinkForRCacheBuffer( U16 nDrvNo, TNFTL_RW_AREA *nArea, U8 *nPageBuffer, U32 nBufferSize );
extern TNFTL_ERROR 				TNFTL_AllocMemAndLinkForAlignCacheBuffer( U16 nDrvNo, U8 *nPageBuffer, U32 nBufferSize );

extern TNFTL_ERROR				TNFTL_AllocMemAndLinkForMakeLPT( U16 nDrvNo, U32 *nLPTBuffer, U32 nBufferSize );
extern TNFTL_ERROR 				TNFTL_AllocMemAndLinkForExtendedPartition( U16 nDrvNo, TNFTL_RW_AREA *nArea );
extern TNFTL_ERROR				TNFTL_SetStEdOfCS( U16 nDrvNo, U16 nStartCS, U16 nEndCS );
extern TNFTL_ERROR				TNFTL_SetExtendedPartitionNums( U16 nDrvNo, U16 nExtPartitionNums );

extern TNFTL_ERROR				TNFTL_SetUseCheckPattern( unsigned int on_off, unsigned int nMode );
extern TNFTL_ERROR 				TNFTL_ScanDevice( TNFTL_DRVINFO *FTLInfo );
extern void 					TNFTL_SetNBAreaEndPBAddr( int nPhyBlockAddr );
extern void 					TNFTL_SetROAreaInfo( TNFTL_DRVINFO * FTLInfo, unsigned long long nROAreaSize, int nROAreaPartNum, int nROAreaUsable, int ROAreaType );

extern void						TNFTL_SetAreaProtectFlag( int value );
extern void						TNFTL_SetFastInitializeMode( int value );
extern void 					TNFTL_SetMTDEILFlag( unsigned int value);	

extern void						TNFTL_DEBUG_Init( void );
extern void 					TNFTL_DEBUG_Init_Function( TNFTL_FUNC1 nFUNC1, TNFTL_FUNC2 nFUNC2, TNFTL_FUNC2 nFUNC3, TNFTL_FUNC2 nFUNC4,	TNFTL_FUNC2 nFUNC5,	TNFTL_FUNC3 nFUNC6, TNFTL_FUNC1 nFUNC7,	TNFTL_FUNC1 nFUNC8);

extern TNFTL_ERROR 				TNFTL_LPTMakeBlock( TNFTL_DRVINFO *FTLInfo );
extern TNFTL_ERROR 				TNFTL_DEBUG_VTCParser( void* nVTCCmd );
extern void						TNFTL_Init( U16 nDrvNo );
extern TNFTL_ERROR				TNFTL_InitDrive( TNFTL_DRVINFO *FTLInfo );
extern TNFTL_ERROR				TNFTL_CheckIsExistBMP( TNFTL_DRVINFO *FTLInfo );
extern TNFTL_ERROR 				TNFTL_CheckBadBlockofBoundary( TNFTL_DRVINFO *FTLInfo, U16 nMediaNo, U32 nStPHYBlkAddr, U32 nEdPHYBlkAddr );
extern TNFTL_ERROR 				TNFTL_AlignCacheFlush( TNFTL_DRVINFO *FTLInfo );

extern U32						TNFTL_AREAGetTotalSectorSize( TNFTL_DRVINFO *FTLInfo, TNFTL_RW_AREA *nArea );
extern TNFTL_ERROR				TNFTL_AREAGetTotalSecAndCHS( TNFTL_DRVINFO *FTLInfo, TNFTL_RW_AREA *nArea, U32 *rTotalSec, U16 *rCylinder, U16 *rHead, U8 *rSector );
extern TNFTL_ERROR				TNFTL_AREASetTotalSectorSize( TNFTL_DRVINFO *FTLInfo, TNFTL_RW_AREA *nArea, U32 nTotalSectorSize );
extern TNFTL_ERROR				TNFTL_AREAReadSectorBy4Byte( TNFTL_DRVINFO *FTLInfo, TNFTL_RW_AREA *nArea, U32 nSectorAddr, U16 nOffset, U16 nReadSize, U8 *nReadBuffer );
extern TNFTL_ERROR				TNFTL_AREAReadSector( TNFTL_DRVINFO *FTLInfo, TNFTL_RW_AREA *nArea, U32 nSectorAddr, U32 nSecSize, U8 *nReadBuffer );
extern TNFTL_ERROR 				TNFTL_AREAReadSectorIRQ( TNFTL_DRVINFO *FTLInfo, TNFTL_RW_AREA *nArea, ndd_work_info *nWorkInfo );
extern TNFTL_ERROR				TNFTL_AREAWriteSector( TNFTL_DRVINFO *FTLInfo, TNFTL_RW_AREA *nArea, U32 nSectorAddr, U32 nSecSize, U8 *nWriteBuffer );
extern TNFTL_ERROR 				TNFTL_AREAWriteSectorIRQ( TNFTL_DRVINFO *FTLInfo, TNFTL_RW_AREA *nArea, ndd_work_info *nWorkInfo );
extern TNFTL_ERROR				TNFTL_AREAFormat( TNFTL_DRVINFO *FTLInfo, TNFTL_RW_AREA *nArea, U16 nFormatLevel );
extern TNFTL_ERROR				TNFTL_NBGetCRCOfImageFile( TNFTL_DRVINFO *FTLInfo, U32 nFlagSelfChecking, U32 *nOrgCRCcode1, U32 *nOrgCRCcode2, U32 *nRomFileSize, U32 *rRstCRCcode1, U32 *rRstCRCcode2 );
extern TNFTL_ERROR 				TNFTL_NBGetCRCValueOfImageFile( TNFTL_DRVINFO *FTLInfo, U32 *nOrgCRCcode1, U32 *nOrgCRCcode2, U32 *nRomFileSize );
extern TNFTL_ERROR				TNFTL_NBGetSizeOfImageFile( TNFTL_DRVINFO *FTLInfo, U32 *nRomFileSize ) ;
extern TNFTL_ERROR				TNFTL_NBExtractImageFile( TNFTL_DRVINFO *FTLInfo, U16 nImageNo, U8 *nReadBuffer, U32 *rRomSize, U32 *rGmcStatus, U32 *rExtStatus, U32 *rBootStatus, U32 *rRomCrc );
extern void					 	TNFTL_WCacheSetDataStartSector( TNFTL_DRVINFO *FTLInfo, TNFTL_RW_AREA *nArea, unsigned int nDataStartSector );

extern U16						TNFTL_GetSerialNumber( TNFTL_DRVINFO *FTLInfo, U8* rSerialNumber, U16 nSize );
extern void						TNFTL_SetFlagOfChangePartition( TNFTL_DRVINFO *FTLInfo, U16 on_off );
extern void						TNFTL_SetReservedBadBlockRatio( TNFTL_DRVINFO * FTLInfo, int nMode );
extern void 					TNFTL_SetReservedBadBlockMode( unsigned int nMode );

extern TNFTL_ERROR				TNFTL_IOReadPageWithECC( TNFTL_DRVINFO *FTLInfo, U32 nFTLPageAddr, U16 nStartPPage, U16 nReadPPSize, U8 *nPageBuffer, U8 *nSpareBuffer, int nEccOnOff );
extern TNFTL_ERROR 				TNFTL_IOReadPhyPage( TNFTL_DRVINFO *FTLInfo, U32 nBlkAddr, U32 nPageAddr, U16 nCSorder, U8 *nReadBuffer );
//extern __inline TNFTL_ERROR		TNFTL_IOReadSpare( TNFTL_DRVINFO *FTLInfo, U32 nFTLPageAddr, U8 *nSpareBuffer );
//extern __inline TNFTL_ERROR		TNFTL_IOReadPage( TNFTL_DRVINFO *FTLInfo, U32 nFTLPageAddr, U16 nStartPPage, U16 nReadPPSize, U8 *nPageBuffer, U8 *nSpareBuffer, int nEccOnOff );
//extern __inline TNFTL_ERROR		TNFTL_IOReadPageForSignature( TNFTL_DRVINFO *FTLInfo, U32 nFTLPageAddr, U16 nStartPPage, U16 nReadPPSize, U8 *nPageBuffer, U8 *nSpareBuffer, int nEccOnOff );

//extern __inline TNFTL_ERROR		TNFTL_IOReadTwoPlanePage( TNFTL_DRVINFO *FTLInfo, U32 nFTLPageAddr, U16 nStartPPage, U16 nReadPPSize, U8 *nPageBuffer, U8 *nSpareBuffer, int nReadOption, int nEccOnOff );
//extern __inline TNFTL_ERROR		TNFTL_IOReadTwoPlaneLastPage( TNFTL_DRVINFO *FTLInfo, U32 nFTLPageAddr, U16 nStartPPage, U16 nReadPPSize, U8 *nPageBuffer, U8 *nSpareBuffer, int nEccOnOff );
//extern __inline TNFTL_ERROR		TNFTL_IOReadUserSizePage( TNFTL_DRVINFO *FTLInfo, U32 nFTLPageAddr, U16 nColumnAddr, U32 nReadSize, U8 *nReadBuffer );

//extern __inline TNFTL_ERROR		TNFTL_IOWritePageGang( TNFTL_DRVINFO *FTLInfo, U32 nFTLPageAddr, U16 nStartPPage, U16 nWritePPSize, U8 *nPageBuffer );
//extern __inline TNFTL_ERROR		TNFTL_IOWritePage( TNFTL_DRVINFO *FTLInfo, U32 nFTLPageAddr, U16 nStartPPage, U16 nWritePPSize, U8 *nPageBuffer, U8 *nSpareBuffer, int nEccOnOff );
//extern __inline TNFTL_ERROR		TNFTL_IOWriteTwoPlanePage( TNFTL_DRVINFO *FTLInfo, U32 nFTLPageAddr, U16 nStartPPage, U16 nWritePPSize, U8 *nPageBuffer, U8 *nSpareBuffer, int nEccOnOff );
//extern __inline TNFTL_ERROR		TNFTL_IOWriteTwoPlaneLastPage( TNFTL_DRVINFO *FTLInfo, U32 nFTLPageAddr, U16 nStartPPage, U16 nWritePPSize, U8 *nPageBuffer, U8 *nSpareBuffer, int LastPage, int nEccOnOff );
//extern __inline TNFTL_ERROR		TNFTL_IOWriteCachePage( TNFTL_DRVINFO *FTLInfo, U32 nFTLPageAddr, U16 nStartPPage, U16 nWritePPSize, U8 *nPageBuffer, U8 *nSpareBuffer, int nEccOnOff );
//extern __inline TNFTL_ERROR		TNFTL_IOWriteCacheLastPage( TNFTL_DRVINFO *FTLInfo, U32 nFTLPageAddr, U16 nStartPPage, U16 nWritePPSize, U8 *nPageBuffer, U8 *nSpareBuffer, int nEccOnOff );
//extern __inline TNFTL_ERROR		TNFTL_IOWriteUserSizePage( TNFTL_DRVINFO *FTLInfo, U32 nFTLPageAddr, U16 nColumnAddr, U32 nWriteSize, U8 *nWriteBuffer );
//extern __inline TNFTL_ERROR		TNFTL_IOWriteSpare( TNFTL_DRVINFO *FTLInfo, U32 nFTLPageAddr, U8 *nSpareBuffer );
//
//extern __inline TNFTL_ERROR		TNFTL_IOCopyBackPage( TNFTL_DRVINFO *FTLInfo, U32 nDesFTLPageAddr, U32 nSrcFTLPageAddr, U8 *nPageBuffer, U8 *nSpareBuffer );
//extern __inline TNFTL_ERROR		TNFTL_IOEraseBlock( TNFTL_DRVINFO *FTLInfo, U32 nFTLBlkAddr, int nFormatMode );
//extern __inline TNFTL_ERROR		TNFTL_IOCopyBackTwoPlanePage( TNFTL_DRVINFO *FTLInfo, U32 nDesFTLPageAddr, U32 nSrcFTLPageAddr, U8 *nPageBuffer, U8 *nSpareBuffer );

extern TNFTL_ERROR 				TNFTL_GoldenPageWrite( TNFTL_DRVINFO *FTLInfo, U32 nStarAddr, U32 nWriteByteSize, U8 *nPageBuffer );
extern TNFTL_ERROR 				TNFTL_GoldenPageRead( TNFTL_DRVINFO *FTLInfo, U32 nStarAddr, U32 nReadByteSize, U8 *nPageBuffer );
extern U32 						TNFTL_SizeofGoldenPage( TNFTL_DRVINFO *FTLInfo );

extern TNFTL_ERROR				TNFTL_SetCallBackHandler( TNFTL_CALLBACK_HANDLER pCallBackHandler );
extern TNFTL_ERROR				TNFTL_SetUartDebug( unsigned int on_off );
extern TNFTL_ERROR				TNFTL_BMPRefresh( TNFTL_DRVINFO *FTLInfo );

extern TNFTL_ERROR				TNFTL_SetUseAreaReadCacheMode( TNFTL_DRVINFO *FTLInfo, TNFTL_RW_AREA *nArea, unsigned int on_off );
extern TNFTL_ERROR 				TNFTL_SetUseAlignCacheMode( TNFTL_DRVINFO *FTLInfo, unsigned int on_off );

#endif

