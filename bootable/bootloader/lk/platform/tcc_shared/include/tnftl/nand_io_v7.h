 /****************************************************************************
 *   FileName    : nand_io_v7.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 * 
 ****************************************************************************/
#ifndef __NAND_IO_H
#define __NAND_IO_H

#if defined(_WINCE_)
#include <windows.h>			//for ND_TRACE(=RETAILMSG) link error
#endif

#if defined(USE_V_ADDRESS)
	#if defined(_LINUX_)
	#if defined(TCC89XX) || defined(TCC92XX)
	#define __USE_NAND_ISR__
	#elif defined(TCC93XX)
	#define __USE_NAND_ISR__
	#elif defined(TCC88XX)
	#define __USE_NAND_ISR__
	#elif defined(TCC892X)
	#define __USE_NAND_ISR__
    #endif
	#elif defined(_WINCE_)
	#define __USE_NAND_ISR__
	#endif
#endif

//#define NAND_GPIO_DEBUG

//*****************************************************************************
//*
//*
//*                       [ Specific DEFINE ]
//*
//*
//*****************************************************************************
#if defined(_LINUX_)
	#if defined(CONFIG_MACH_TCC8920ST) || (defined(TARGET_BOARD_STB) && defined(TCC892X)) || \
		defined(_M805_8925_) || defined(CONFIG_M805S_8925_0XX) || (defined(TARGET_TCC8920_EVM) && (HW_REV == 0x1006))
	#define NAND_1CS_ONLY		// For STB Board.
	#define NAND_8BIT_ONLY
	#elif (defined(TARGET_TCC8920_EVM) && (HW_REV == 0x1008))
	#define NAND_1CS_ONLY
        #elif((HW_REV == 0xA001) || (HW_REV == 0xA002))
	#define NAND_1CS_ONLY		// For STB Board.
	#define NAND_8BIT_ONLY
	#else
	#define NAND_2CS_ONLY		// For Idle CS#2~3
	#endif
//#define NAND_8BIT_ONLY
#else
//#define NAND_2CS_ONLY
//#define NAND_8BIT_ONLY
#endif

//**********************************************************************
//*		Define Dev Board
//**********************************************************************
#if defined(TCC89XX) || defined(TCC92XX)

#if  defined(TARGET_TCC9200S_EVM) || defined(CONFIG_MACH_TCC9200S)
#if defined (CONFIG_MACH_TCC8900)
#define TCC89_92_BOARD			// TCC89/91/9200 ND_RDY: GPIO_B28 - EDI - NFC_CTRL
#else
#define TCC9200S_BOARD		// TCC9200S ND_RDY: GPIO_B31
#endif
#else
#define TCC89_92_BOARD			// TCC89/91/9200 ND_RDY: GPIO_B28 - EDI - NFC_CTRL
#endif

#elif defined(TCC93XX)
	#if defined(_WINCE_)
	//#define TCC89D_93M_BOARD		// [TCC8000 DB TCC9300 MB] RDY: GPIO_B29 WP: GPIO_A19
	#define TCC93D_93M_BOARD		// [TCC9300 DB TCC9300 MB] RDY: GPIO_B29 WP: GPIO_B28
	#elif defined(_LINUX_)
	#if defined(BOARD_TCC930X_CM) || defined(CONFIG_MACH_TCC9300CM)
	#define TCC89D_93M_BOARD
	#elif defined(BOARD_TCC930X_STB_DEMO) || defined(CONFIG_MACH_TCC9300ST)
	#define TCC930X_STB_BOARD
	#else //CONFIG_MACH_TCC9300
	#define TCC93D_93M_BOARD
	#endif
	#endif
#elif defined(TCC88XX)
	#if defined(_M801_88_) || defined(CONFIG_MACH_M801_88) || defined(_M803_) || defined(CONFIG_MACH_M803)
	#define TCC8803_M801_BOARD
	#else
	#define TCC88D_88M_BOARD	
	#endif
#elif defined(TCC892X)
	#if defined(_WINCE_)
		#if defined(TCC8920_D2_08X4_SV01) || defined(TCC8920_D3_08X4_SV01)
		#define TCC8920_88M_BOARD
		#elif defined(TCC8920_D3_08X4_SV60)
		#define TCC8920_V60_88M_BOARD
		#elif defined(TCC8925_D3_08X4_2CS_SV01)
		#define TCC8925_88M_BOARD
		#else
		#define TCC892X_88M_BOARD
		#endif
	#elif defined(_LINUX_)
		#if defined(_M805_8923_) || defined(_M805_8925_) || defined(CONFIG_MACH_M805_892X)
		#define TCC892X_M805_BOARD
		#else
		#define TCC892X_88M_BOARD
		#endif
	#endif
#endif

//**********************************************************************
//*		Define Power Reset Board 
//**********************************************************************
#if defined(_M801_88_) || defined (CONFIG_MACH_M801_88)
#define PWR_RESET_M801
#elif defined(_M803_) || defined (CONFIG_MACH_M803)
#define PWR_RESET_M803
#elif defined(_M805_8923_) || defined(_M805_8925_) || defined (CONFIG_MACH_M805_892X)
#define PWR_RESET_M805
#endif

//**********************************************************************
//*		Define Power checking for nand writing protction
//**********************************************************************
#if defined(TARGET_BOARD_STB) || defined(CONFIG_MACH_TCC8920ST)
//#define PWR_CHK_NAND_WR_PROTECT
#endif

//**********************************************************************
//*		Define RDY, WP
//**********************************************************************
#ifdef TCC9200S_BOARD			// ND_RDY: B31, ND_WP: B22
#define TCC_NAND_RDY_B31
#define TCC_NAND_WP_B22
#endif

#ifdef TCC89_92_BOARD			// ND_RDY: B28, ND_WP: B31
#define TCC_NAND_RDY_B28
#define TCC_NAND_WP_B31
#endif

#ifdef TCC93D_93M_BOARD
#define TCC_NAND_RDY_B29
#define TCC_NAND_WP_A19
#endif

#ifdef TCC930X_STB_BOARD
#define TCC_NAND_RDY_B29
#define TCC_NAND_WP_B15
#define NAND_8BIT_ONLY
#endif

#ifdef TCC89D_93M_BOARD
#define TCC_NAND_RDY_B29
#define TCC_NAND_WP_B28
#endif 

#if defined(TCC88D_88M_BOARD) || defined(TCC8803_M801_BOARD)
#define TCC_NAND_RDY_B29
#define TCC_NAND_WP_B27
#endif

#if defined(TCC801X)
#define TCC_NAND_RDY_FC6_9
#define TCC_NAND_WP_PK16
#endif

#if defined(TCC892X_88M_BOARD)
#define TCC_NAND_RDY_A16

	#if (HW_REV == 0x1005) || (HW_REV == 0x1007)   // for TCC8930_D3_16X4_2CS_SV0.1 Board.
	#define TCC_NAND_WP_B8
	#elif(HW_REV == 0x1006)
	#define TCC_NAND_WP_D0
	#elif(HW_REV == 0x1008)
	#define TCC_NAND_WP_D8
        #elif(HW_REV == 0xA001)
        #define TCC_NAND_WP_E17
	#else
	#define TCC_NAND_WP_F27
	#endif
	
#endif

#if defined(TCC892X_M805_BOARD)
#define TCC_NAND_RDY_A16
	#if (HW_REV == 0x2002) || (HW_REV == 0x2003) || (HW_REV == 0x2004) || (HW_REV == 0x2005)
	#define TCC_NAND_WP_D0
	#else
	#define TCC_NAND_WP_D8
	#endif
#endif

#if defined(TCC8923_M805_BOARD)
#define TCC_NAND_RDY_A16
#define TCC_NAND_WP_D8
#endif

#ifdef TCC8920_88M_BOARD
#define TCC_NAND_RDY_A16
#define TCC_NAND_WP_F27
#endif

#ifdef TCC8925_88M_BOARD
#define TCC_NAND_RDY_A16
#define TCC_NAND_WP_D0
#define NAND_8BIT_ONLY
#endif

#ifdef TCC8920_V60_88M_BOARD
#define TCC_NAND_RDY_A16
#define TCC_NAND_WP_B8
#endif


//**********************************************************************
//*		Define IRQ
//**********************************************************************
#if defined(_WINCE_)
#define NAND_IRQ_NFC			IRQ_NFC
#else
	#if defined(TCC892X) && defined(USE_V_ADDRESS)	
	#define NAND_IRQ_NFC			INT_NFC
	#else	
	#define NAND_IRQ_NFC			IRQ_NFC
	#endif
#endif

//External interrupt 10 -[Not Use External Interrupt]  
#ifdef TCC89_92_BOARD
#define NAND_IRQ_READY			IRQ_EI10	
#else
#define NAND_IRQ_READY			IRQ_NFC
#endif

//**********************************************************************
//*		Define ECC Algorithm for ChipSet
//**********************************************************************
#if defined(TCC77X) || defined(TCC87XX) || defined(TCC82XX) || defined(TCC78X) || defined(TCC83XX) || defined(TCC79XX)
#define ECC_TYPE_RS
#elif defined(TCC81XX) || defined(TCC80XX) || defined(TCC92XX) || defined(TCC89XX) || defined(TCC93XX) || defined(TCC88XX) || defined(TCC892X) || defined(TCC801X)
#define ECC_TYPE_BCH
#endif

#if defined(USE_V_ADDRESS)
	#if defined(_LINUX_)
	#define  ND_TRACE	printk
	//#define n_printk(f, a...)    printk("[%s:%d] " f, __func__, __LINE__, ##a) 
	//#define _n_printk            printk
	#define n_printk(f, a...)
	#define _n_printk(a...)
	#elif defined(_WINCE_)
		#define  ND_TRACE(...)		RETAILMSG( 1, (L##__VA_ARGS__) )
	#endif
#else
	#if defined(_LINUX_)
		#define  ND_TRACE	printf
	#elif defined(_WINCE_)
		#define  ND_TRACE	B_RETAILMSG
	#elif defined(TCC801X)
		#define	 ND_TRACE	TccUIDebug_Print
	#else
		#define	 ND_TRACE	uart_send_printf
	#endif
#endif

//*****************************************************************************
//*
//*
//*                       [ General DEFINE & TYPEDEF ]
//*
//*
//*****************************************************************************
//===================================================================
//
//		DRIVER SIGNATURE
//
//===================================================================
#define SIGBYAHONG				'S','I','G','B','Y','A','H','O','N','G','_'

#define NAND_IO_SIGNATURE		'N','A','N','D','_','I','O','_'
#define TNFTL_SIGNATURE			'T','N','F','T','L','_'

#if defined(_WINCE_)
#define SIGN_OS					'W','I','N','C','E','_'
#elif defined(_LINUX_)
#define SIGN_OS					'L','I','N','U','X','_'
#else
#define SIGN_OS					'N','U','_'
#endif

#if defined(TCC77X)
#define SIGN_CHIPSET			'T','C','C','7','7','X','X','_'
#elif defined(TCC78X)
#define SIGN_CHIPSET			'T','C','C','7','8','X','X','_'
#elif defined(TCC79X)
#define SIGN_CHIPSET			'T','C','C','7','9','X','X','_'
#elif defined(TCC801X)
#define SIGN_CHIPSET			'T','C','C','8','0','1','X','_'
#elif defined(TCC81XX)
#define SIGN_CHIPSET			'T','C','C','8','1','X','X','_'
#elif defined(TCC82XX)
#define SIGN_CHIPSET			'T','C','C','8','2','X','X','_'
#elif defined(TCC83XX)
#define SIGN_CHIPSET			'T','C','C','8','3','X','X','_'
#elif defined(TCC87XX)
#define SIGN_CHIPSET			'T','C','C','8','7','X','X','_'
#elif defined(TCC88XX)
#define SIGN_CHIPSET			'T','C','C','8','8','X','X','_'
#elif defined(TCC89XX)
#define SIGN_CHIPSET			'T','C','C','8','9','X','X','_'
#elif defined(TCC892X)
#define SIGN_CHIPSET			'T','C','C','8','9','2','X','_'
#elif defined(TCC91XX)
#define SIGN_CHIPSET			'T','C','C','9','1','X','X','_'
#elif defined(TCC92XX)
#define SIGN_CHIPSET			'T','C','C','9','2','X','X','_'
#elif defined(TCC93XX)
#define SIGN_CHIPSET			'T','C','C','9','3','X','X','_'
#else
#error
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
#ifndef NULL
#define NULL            		0
#endif

#if defined(_LINUX_) || defined(TCC801X)
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
#ifndef _BOOL_
#define _BOOL_
typedef unsigned int			BOOL;
#endif
#else
#ifndef U8
typedef unsigned char			U8;
#endif
#ifndef U16
typedef unsigned short int		U16;
#endif
#ifndef U32
typedef unsigned int			U32;
#endif
#ifndef _BOOL_
typedef unsigned int			BOOL;
#endif
#endif

#ifndef DWORD_BYTE
typedef	union {
	unsigned long		DWORD;
	unsigned short int	WORD[2];
	unsigned char		BYTE[4];
} DWORD_BYTE;
#endif

#ifndef BITSET
#define	BITSET(X, MASK)				( (X) |= (U32)(MASK) )
#endif
#ifndef BITSCLR
#define	BITSCLR(X, SMASK, CMASK)	( (X) = ((((U32)(X)) | ((U32)(SMASK))) & ~((U32)(CMASK))) )
#endif
#ifndef BITCSET
#define	BITCSET(X, CMASK, SMASK)	( (X) = ((((U32)(X)) & ~((U32)(CMASK))) | ((U32)(SMASK))) )
#endif
#ifndef BITCLR
#define	BITCLR(X, MASK)				( (X) &= ~((U32)(MASK)) )
#endif
#ifndef BITXOR
#define	BITXOR(X, MASK)				( (X) ^= (U32)(MASK) )
#endif
#ifndef ISZERO
#define	ISZERO(X, MASK)				(  ! (((U32)(X)) & ((U32)(MASK))) )
#endif
#ifndef BYTE_OF
#define	BYTE_OF(X)					( *(volatile unsigned char *)(&(X)) )
#endif
#ifndef HWORD_OF
#define	HWORD_OF(X)					( *(volatile unsigned short *)(&(X)) )
#endif
#ifndef WORD_OF
#define	WORD_OF(X)					( *(volatile unsigned int *)(&(X)) )
#endif

#ifndef dim
#define dim(x) (sizeof(x)/sizeof(x[0]))
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
	ERR_NAND_IO_FAILED_GET_DEVICE_INFO						= 0xA100000,
	ERR_NAND_IO_FAILED_CORRECTION_SLC_ECC					= 0xA100001,
	ERR_NAND_IO_FAILED_CORRECTION_MLC_ECC					= 0xA100002,
	ERR_NAND_IO_FAILED_GET_SHIFT_FACTOR_FOR_MULTIPLY		= 0xA100003,
	ERR_NAND_IO_FAILED_GET_FACTORY_BAD_MARK_OF_PBLOCK		= 0xA100004,
	ERR_NAND_IO_READ_STATUS_GENERAL_FAIL					= 0xA100005,
	ERR_NAND_IO_TIME_OUT_READ_STATUS						= 0xA100006,
	ERR_NAND_IO_WRONG_PARAMETER								= 0xA100007,
	ERR_NAND_IO_WRONG_PARAMETER_ROW_COL_ADDRESS				= 0xA100008,
	ERR_NAND_IO_NOT_READY_DEVICE_IO							= 0xA100009,
	ERR_NAND_IO_FAILED_WRITE								= 0xA10000A,
	ERR_NAND_IO_NOT_EXIST_LBA_HEADBLOCK						= 0xA10000B,
	ERR_LBA_NOT_EXIST_MEDIA_NUM_INFO						= 0xA10000C,
	ERR_LBA_NOT_EXIST_NANDFLASH								= 0xA10000D,
	ERR_NAND_IO_FAILED_LBA_GET_MEDIA_UID					= 0xA10000E,
	ERR_NAND_IO_FAILED_LBA_PARTITION_CHANGE					= 0xA10000F,
	ERR_LBA_FAILED_CONVERT_LBA_ADDR							= 0xA100010,
	ERR_NAND_IO_FAILED_READ									= 0xA100011
} NAND_IO_ERROR;
 
//*****************************************************************************
//*
//*
//*                          [ INTERNAL DEFINATION ]
//*
//*
//*****************************************************************************

/* Maker Information of supported NANDFLASH */
enum SUPPORT_MAKER_NAND
{
	SAMSUNG = 0,
	TOSHIBA,
	HYNIX,
	ST,
	MICRON,
	INTEL,
	MAX_SUPPORT_MAKER_NAND
};

enum SUPPORT_MAKER_LBA_NAND
{
	TOSHIBA_LBA = 0,
	MAX_SUPPORT_MAKER_LBA_NAND
};

enum LBA_NAND_HIDDEN_INFO
{
	//==============================================		
	// VFP Size			[  4Byte ]	0
	// MDP Size			[  4Byte ]	1
	// Hidden Size		[  4Byte ]	2
	// Multi-Hidden Num [  4Byte ]	3
	// Multi-Hidden Size[  4Byte ]	4~10
	//==============================================
	ENUM_LBA_VFP_SECTOR_SIZE = 0,
	ENUM_LBA_MDP_SECTOR_SIZE,
	ENUM_LBA_HIDDEN_SECTOR_SIZE,
	ENUM_LBA_MULTI_HIDDEN_NUM,
	ENUM_LBA_MULTI_HIDDEN_SIZE_0,
	ENUM_LBA_MULTI_HIDDEN_SIZE_1,
	ENUM_LBA_MULTI_HIDDEN_SIZE_2,
	ENUM_LBA_MULTI_HIDDEN_SIZE_3,
	ENUM_LBA_MULTI_HIDDEN_SIZE_4,
	ENUM_LBA_MULTI_HIDDEN_SIZE_5,
	ENUM_LBA_MULTI_HIDDEN_SIZE_6,
	ENUM_LBA_MULTI_HIDDEN_SIZE_7,
	ENUM_LBA_MULTI_HIDDEN_SIZE_8,
	ENUM_LBA_MULTI_HIDDEN_SIZE_9
};

#define SAMSUNG_NAND_MAKER_ID					0xEC
#define TOSHIBA_NAND_MAKER_ID					0x98
#define HYNIX_NAND_MAKER_ID						0xAD
#define ST_NAND_MAKER_ID						0x20
#define MICRON_NAND_MAKER_ID					0x2C
#define INTEL_NAND_MAKER_ID						0x89

#define NAND_IO_MAX_SUPPORT_PAGE_SIZE			16384		
#define NAND_IO_MAX_SUPPORT_SPARE_SIZE			1024		

/* Media Attribute */
#define A_08BIT									0x00000001		// 8Bit BUS
#define A_BIG									0x00000002		// Big Page Size
#define A_SMALL									0x00000004		// Small Page Size
#define A_16BIT									0x00000008		// 16Bit BUS
#define A_SLC									0x00000010		// Single Layer Cell
#define A_MLC									0x00000020		// Multi Layer Cell
#define A_MLC_8BIT								0x00000040		// 8Bit MLC ECC
#define A_MLC_12BIT								0x00000080		// 12Bit MLC ECC
#define A_MLC_16BIT								0x00000100		// 16Bit MLC ECC
#define A_MLC_24BIT								0x00000200		// 24Bit MLC ECC
#define A_MLC_40BIT								0x00002000		// 40Bit MLC ECC
#define S_READ_RETRY							0x00004000		// 40Bit MLC ECC

#define A_PARALLEL								0x00000400		// Parallel Composition
#define A_DATA_WITDH_08BIT						0x00000800		// Data 8Bit Bus
#define A_DATA_WITDH_16BIT						0x00001000		// Data 16Bit Bus

#define S_NOR									0x00010000		// NORMAL [ ReadID, Reset, Read, Page Program, Block Erase, Read Status ]
#define S_CB									0x00020000		// COPY BACK
#define S_CP									0x00040000 		// CACHE PROGRAM
#define S_MP									0x00080000		// MULTI PLANE
#define S_MP2									0x00100000		// MULTI PLANE
#define S_MCP									0x00200000		// MULTI PLANE CACHE 
#define S_IL									0x00400000		// INTER LEAVE
#define S_EIL									0x00800000		// EXTERNAL INTER LEAVE 
#define S_EFN									0x01000000		// ECC FREE NAND
#define S_NPW									0x02000000		// Not-Support PARTIAL WRITE
#define S_MP_READ								0x04000000		// MULTI PLANE READ
#define S_LBA									0x08000000		// LBA NAND
#define S_RAND_IO								0x10000000		// Random Data In/Out
#define S_EB									0x20000000		// Extend Block 

#define DISTRICT_MODE							0x40000000			
#define NON_DISTRICT_MODE						0x80000000

#define NAND_IO_NORMAL_PAGE_PROGRAM				0x00000001
#define NAND_IO_CACHE_PROGRAM_1ST				0x00000002
#define NAND_IO_CACHE_PROGRAM_2ND				0x00000004
#define NAND_IO_TWO_PLANE_PAGE_PROGAM_1ST		0x00000010
#define NAND_IO_TWO_PLANE_PAGE_PROGAM_2ND		0x00000020

#define NAND_IO_NORMAL_READ						0x00000001
#define NAND_IO_CACHE_READ						0x00000002
#define NAND_IO_TWO_PLANE_READ_1ST				0x00000010
#define NAND_IO_TWO_PLANE_READ_2ND				0x00000020
#define NAND_IO_TWO_PLANE_MID_PAGE				0x00000100
#define NAND_IO_TWO_PLANE_LAST_PAGE				0x00000200


#define NAND_IO_MAX_SUPPORT_DRIVER				2

#define NAND_IO_SERIAL_COMBINATION_MODE			0x00
#define NAND_IO_PARALLEL_COMBINATION_MODE		0x01

#if defined(NAND_1CS_ONLY)
#define NAND_IO_DRV0_START_CS					0
#define NAND_IO_DRV0_END_CS						0
#elif defined(NAND_2CS_ONLY)
#define NAND_IO_DRV0_START_CS					0
#define NAND_IO_DRV0_END_CS						1
#else
#define NAND_IO_DRV0_START_CS					0
#define NAND_IO_DRV0_END_CS						3
#endif

#define	ECC_OFF                     			0
#define	ECC_ON                      			1
#define	ECC_ON_NON_CACHE_BUF           			3

#define SLC_ECC_TYPE							0x0001			// 10 Byte
#define MLC_ECC_4BIT_TYPE						0x0002			// BCH: 7 Byte( 52 bit)	turn: 2, byte remain: 3, register mask: 0x0F
#define MLC_ECC_8BIT_TYPE						0x0004			// BCH:13 Byte (104 bit)	turn: 4, byte remain: 1, register mask: 0xFF
#define MLC_ECC_12BIT_TYPE						0x0010			// BCH:20 Byte (156 bit)	turn: 5, byte remain: 4, register mask: 0x0F
#define MLC_ECC_14BIT_TYPE						0x0020			// BCH:23 Byte (184 bit)	turn: 6, byte remain: 3, register mask: 0xFF
#define MLC_ECC_16BIT_TYPE						0x0040			// BCH:26 Byte (207 bit)turn: 7, byte remain: 2, register mask: 0x7F
#define MLC_ECC_24BIT_TYPE						0x0080			// TCC93X ( Writing After Porting )

#define NAND_IO_SPARE_SIZE_BIG					16
#define NAND_IO_SPARE_SIZE_SMALL				8

#define PAGE_ECC_OFF							0
#define PAGE_ECC_ON								1

//----------------------------------------------------------------------
// TNFTL R/W Area Spare Date
//----------------------------------------------------------------------
// Block Area & Type		1Byte :  0xFF
// Block Parameter			2Byte
// Logical Block Address	3Byte :  0x0 ~ 0xFFFFFE
//----------------------------------------------------------------------
// Spare Data Total 		6Byte
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// TNFTL R/O Area Spare Date
//----------------------------------------------------------------------
// Block Area & Type		1Byte :  0xFF
// Rom Image Num			2Byte
// Rom Image LBA			3Byte :  0x0 ~ 0xFFFFFE
//----------------------------------------------------------------------
// Spare Data Total 		6Byte
//----------------------------------------------------------------------


// 512 Page + 16 Spare : SLC / 4 Bit ECC
typedef struct {
	U8		SpareData0[4];		//  4 Bytes Spare data
	U8		Temp;				//  1 Bytes Temp
	U8		BadMark;			//  1 Bytes bad mark
	U8		SpareData1[2];		//  2 Bytes Spare data
	U8		PageECC[8];			//  8 Bytes Spare ECC --> 4Bit ECC
} SLC_512Page_4BitECC_Spare;

// 2048 Page + 64 Spare : 4 Bit ECC
typedef struct {
	U8		BadMark;			//  1 Bytes bad mark
	U8		Temp;				//  1 Bytes Temp		--> Parallel Bad Mark reserved
	U8 		SpareData[14];		// 14 bytes Spare data
	U8		PageECC[4][8];		// 32 Bytes Spare ECC --> 4Bit ECC
	U8		SpareECC[8];		//  8 Byte
} MLC_2KPage_4BitECC_Spare;

// 4096 Page + 128 Spare : 4 Bit ECC
typedef struct {
	U8		BadMark;			//  1 Bytes bad mark
	U8		Temp;				//  1 Bytes Temp		--> Parallel Bad Mark reserved
	U8 		SpareData[14];		// 14 bytes Spare data
	U8		PageECC[8][8];		// 64 Bytes Spare ECC --> 4Bit ECC
	U8		SpareECC[8];		//  8 Byte
} MLC_4KPage_4BitECC_Spare;

// 4096 Page + 218 Spare : 8/12 Bit ECC
typedef struct {
	U8		BadMark;			//  1 Bytes bad mark
	U8		Temp;				//  1 Bytes Temp		--> Parallel Bad Mark reserved
	U8 		SpareData[14];		// 14 bytes Spare data
	U8		PageECC[8][20];		//160 Bytes Spare ECC --> 4Bit ECC
	U8		SpareECC[20];		// 20 Byte
} MLC_4KPage_8_12BitECC_Spare;

// 8192 Page + 436 Spare : 16 Bit ECC
typedef struct {
	U8		BadMark;			//  1 Bytes bad mark
	U8		Temp;				//  1 Bytes Temp
	U8 		SpareData[10];		// 10 bytes Spare data
	U8 		PageECC[16][26];	//416 Bytes Page ECC
	U8		SpareECC[8];		//  8 Bytes Spare ECC --> 4 bit ECC
} MLC_4KPage_16BitECC_Spare;


#define TYPE_SPARE_FOR_512_16_NANDFLASH			0
#define TYPE_SPARE_FOR_2048_64_NANDFLASH		1
#define TYPE_SPARE_FOR_4096_128_NANDFLASH		2
#define TYPE_SPARE_FOR_4096_218_NANDFLASH		3

#ifdef ECC_TYPE_RS
#define TYPE_ECC_FOR_1BIT_SLC_NANDFLASH			MLC_ECC_4BIT_TYPE
#define TYPE_ECC_FOR_4BIT_MLC_NANDFLASH			MLC_ECC_4BIT_TYPE
#define TYPE_ECC_FOR_8BIT_MLC_NANDFLASH			MLC_ECC_8BIT_TYPE
#define ECC_SHIFT_DATASIZE						16
#else
#define TYPE_ECC_FOR_1BIT_SLC_NANDFLASH			SLC_ECC_TYPE
#define TYPE_ECC_FOR_4BIT_MLC_NANDFLASH			MLC_ECC_4BIT_TYPE
#define TYPE_ECC_FOR_8BIT_MLC_NANDFLASH			MLC_ECC_12BIT_TYPE
#define TYPE_ECC_FOR_12BIT_MLC_NANDFLASH		MLC_ECC_12BIT_TYPE
#define TYPE_ECC_FOR_14BIT_MLC_NANDFLASH		MLC_ECC_14BIT_TYPE
#define TYPE_ECC_FOR_16BIT_MLC_NANDFLASH		MLC_ECC_16BIT_TYPE
#define TYPE_ECC_FOR_24BIT_MLC_NANDFLASH		MLC_ECC_24BIT_TYPE

#if defined(TCC89XX) || defined(TCC92XX)
#define ECC_SHIFT_DATASIZE						4
#elif defined(TCC93XX) || defined(TCC88XX) || defined(TCC801X) || defined(TCC892X)
#define ECC_SHIFT_DATASIZE						16
#endif 
#endif

#define TNFTL_READ_SPARE_ON						0
#define TNFTL_READ_SPARE_OFF					1

#define NAND_MCU_ACCESS							0
#define NAND_DMA_ACCESS							1

#define NAND_NORMAL_BUFFER						0
#define NAND_SERIAL_CHAIN_BUFFER				1


#define ECC_DECODE								0
#define ECC_ENCODE								1
#define	INTER_LEAVE_OFF                			0
#define	INTER_LEAVE_ON                			1
#define MULTI_PLANE_NORMAL_PAGE					0
#define MULTI_PLANE_START_PAGE					1
#define MULTI_PLANE_MID_PAGE					2
#define MULTI_PLANE_LAST_PAGE					3
#define MULTI_PLANE_STUFF_PAGE					4
#define MULTI_PLANE_GOOD_BLOCK					0
#define MULTI_PLANE_BAD_BLOCK					1

#define NAND_IO_BSA_OK							0x0000
#define NAND_IO_BSA_BAD_SERIAL					0x0001
#define NAND_IO_BSA_BAD_BLOCK1_PARALLEL			0x0010
#define NAND_IO_BSA_BAD_BLOCK2_PARALLEL			0x0100
#define NAND_IO_BSA_FAIL_OTHER_REASON			0x1000

#define NAND_IO_STATUS_FAIL_CS0_SERIAL			0x0001
#define NAND_IO_STATUS_FAIL_CS0_PARALLEL		0x0010
#define NAND_IO_STATUS_FAIL_CS1_PARALLEL		0x0100

#define NAND_IO_LOG_COLUMN_IN_PAGE				0
#define NAND_IO_LOG_COLUMN_IN_SPARE				1

/* LBA NAND DEFINATION : [STATUS READ 2] */
#define LBA_MAX_SUPPORT_MULTI_NANDFLASH			4
#define NAND_LBA_PNP							0x00
#define NAND_LBA_BCM							0x02
#define NAND_LBA_VFP							0x04
#define NAND_LBA_MDP							0x06

#define NAND_LBA_DATA_AREA						0x16
#define NAND_LBA_HIDDEN_AREA					0x26
#define NAND_LBA_MULTI_HIDDEN_AREA_0			0x36
#define NAND_LBA_MULTI_HIDDEN_AREA_1			0x46
#define NAND_LBA_MULTI_HIDDEN_AREA_2			0x56

#define NAND_POWER_SAVE_ENABLE					0x01
#define NAND_HIGH_SPEED_ENABLE					0x08

#define NAND_ADDRESS_OUT_OF_RANGE_ERROR			0x10
#define NAND_COMMAND_PARAMETER_ERROR			0x40

/* LBA NAND Transfer Protocol 1 */
#define NAND_PROT1_512x1						0x01
#define NAND_PROT1_512x4						0x02
#define NAND_PROT1_512x8						0x04

#define NAND_PROT1_528x1						0x21
#define NAND_PROT1_528x4						0x22
#define NAND_PROT1_528x8						0x24

#define NAND_PROT1_SPARE_INCLUDE				0x20
#define NAND_PROT1_SECTOR_COUNT_MASK			0x07

#define NAND_PROT1_NoCRC_NoECC					0x00
#define NAND_PROT1_CRC							0x40
#define NAND_PROT1_ECC_CHECK_ONLY				0x80
#define NAND_PROT1_ECC_CORRECT					0xC0

/* LBA NAND Transfer Protocol 2 */
#define NAND_PROT2_READ_TYPE_A					0x00
#define NAND_PROT2_READ_TYPE_B					0x02
#define NAND_PROT2_READ_TYPE_C					0x03

#define NAND_PROT2_WRITE_TYPE_A					0x00
#define NAND_PROT2_WRITE_TYPE_B					0x04

#define NAND_LBA_SYS_SECTION					2
#define	NAND_LBA_MAX_NANDFLASH_UID_SIZE			16
#define NAND_LBA_MAX_SUPPORT_MHD_AREA_NUM		10

typedef void (*NAND_LBA_CALLBACK_HANDLER) ( U16 nDrvNo, U16 nStatusCode, U32 wParam );
#define NAND_LBA_CALLBACK_LCD_FORMAT_PROCESS	0x04

#define NAND_IO_WRITE_MODE						0
#define NAND_IO_READ_MODE						1

#define NAND_IO_WRITE_NORMAL_MODE				0
#define NAND_IO_WRITE_CACHE_MODE				1
#define NAND_IO_WRITE_COPYBACK_MODE				2
#define NAND_IO_READ_NORAML_MODE				3
#define NAND_IO_ERASE_MODE						4
#define NAND_IO_STATUS_MODE						5

#define NAND_IO_NFC_BUS							0
#define NAND_IO_MEM_BUS							1

#define NAND_IO_DATA_WITDH_8BIT					0
#define NAND_IO_DATA_WITDH_16BIT				1

#if defined(_LINUX_) || defined(_WINCE_)
#if defined(USE_V_ADDRESS)
#define NAND_P2V(x)								tcc_p2v(x)
#else
#define NAND_P2V(x) 							((volatile unsigned long)(&(x)))
#endif
#endif 

#if defined(_WINCE_)
#define NAND_IO_USE_DMA_ACCESS
#elif defined(_LINUX_)
	#if defined(TCC89XX) || defined(TCC92XX)
	#ifdef KERNEL_DRIVER
	#define NAND_IO_USE_DMA_ACCESS
	#else
	#define NAND_IO_USE_DMA_ACCESS
	#endif
	#elif defined(TCC93XX) || defined(TCC88XX) || defined(TCC892X)
	#ifdef KERNEL_DRIVER
	#define NAND_IO_USE_DMA_ACCESS	
	#else
	#define NAND_IO_USE_DMA_ACCESS
	#endif
	#endif 
#else
	#if defined(TCC801X)
	#define NAND_IO_USE_MCU_ACCESS
	#else
    #ifdef _BOOT_LOADER_
	#define NAND_IO_USE_DMA_ACCESS
    #else
	#define NAND_IO_USE_DMA_ACCESS
	#endif
	#endif
#endif

#ifdef NAND_IO_USE_DMA_ACCESS
//#define NAND_IO_USE_DMA_DOUBLE_BUF
//#define NAND_IO_USE_DMA_DOUBLE_BUF_WRITE
#define NAND_IO_MTD_USE_DMA_DOUBLE_BUF
#define NAND_IO_MTD_USE_DMA_DOUBLE_BUF_WRITE
#endif

#if !defined(TCC89XX)
#define NO_PRE_ENCODING
#endif

#if defined(TCC88XX) || defined(TCC892X)
//#define 	USE_NDMA_WRITE_MTD
//#define		USE_NDMA_READ_MTD

	#if defined(USE_NDMA_WRITE_MTD) && defined(USE_NDMA_READ_MTD)
	//#define			CHANGED_STRUCTURE
	#endif
	
#endif

#if defined(_LINUX_) || defined(_WINCE_)
#if defined(TCC89XX) || defined(TCC92XX)
#define	NAND_IO_HwCMD_PA							*(volatile unsigned long*)0xF05B0000		//( gNAND_IO_pHwND->CMD )
#define	NAND_IO_HwLADR_PA							*(volatile unsigned long*)0xF05B0004		//( gNAND_IO_pHwND->LADR )
#define	NAND_IO_HwDATA_PA							*(volatile unsigned long*)0xF05B0010		//( gNAND_IO_pHwND->WDATA.D32 )
#define	NAND_IO_HwLDATA_PA							*(volatile unsigned long*)0xF05B0020		//( gNAND_IO_pHwND->LDATA )
#define	NAND_IO_HwSDATA_PA							*(volatile unsigned long*)0xF05B0040		//(( gNAND_IO_DataBusType == NAND_IO_MEM_BUS ) ? gNAND_IO_pHwND->WDATA.D8 : gNAND_IO_pHwND->SDATA.D32 )
#define	NAND_IO_HwSADR_PA							*(volatile unsigned long*)0xF05B000C		//( gNAND_IO_pHwND->SADR )
#elif defined(TCC88XX)								//TCC88X NFC BASE ADDR: 0xF05B0000
#define	NAND_IO_HwCMD_PA							*(volatile unsigned long*)0xF05B0000		//( gNAND_IO_pHwND->CMD )
#define	NAND_IO_HwLADR_PA							*(volatile unsigned long*)0xF05B0004		//( gNAND_IO_pHwND->LADR )
#define	NAND_IO_HwDATA_PA							*(volatile unsigned long*)0xF05B0010		//( gNAND_IO_pHwND->WDATA.D32 )
#define	NAND_IO_HwLDATA_PA							*(volatile unsigned long*)0xF05B0020		//( gNAND_IO_pHwND->LDATA )
#define	NAND_IO_HwSDATA_PA							*(volatile unsigned long*)0xF05B000C		//(( gNAND_IO_DataBusType == NAND_IO_MEM_BUS ) ? gNAND_IO_pHwND->WDATA.D8 : gNAND_IO_pHwND->SDATA.D32 )
#define	NAND_IO_HwSADR_PA							*(volatile unsigned long*)0xF05B0008		//( gNAND_IO_pHwND->SADR )
#elif defined(TCC93XX)
	#if defined(USE_V_ADDRESS)
	#define	NAND_IO_HwCMD_PA							*(volatile unsigned long*)0xF0050000		//( gNAND_IO_pHwND->CMD )
	#define	NAND_IO_HwLADR_PA							*(volatile unsigned long*)0xF0050004		//( gNAND_IO_pHwND->LADR )
	#define	NAND_IO_HwDATA_PA							*(volatile unsigned long*)0xF0050010		//( gNAND_IO_pHwND->WDATA.D32 )
		#if defined(NAND_IO_USE_DMA_ACCESS)
		#define	NAND_IO_HwLDATA_PA							*(volatile unsigned long*)0xB0050020		//( gNAND_IO_pHwND->LDATA )
		#else
		#define	NAND_IO_HwLDATA_PA							*(volatile unsigned long*)0xF0050020		//( gNAND_IO_pHwND->LDATA )
		#endif 
	#define	NAND_IO_HwSDATA_PA							*(volatile unsigned long*)0xF005000C		//(( gNAND_IO_DataBusType == NAND_IO_MEM_BUS ) ? gNAND_IO_pHwND->WDATA.D8 : gNAND_IO_pHwND->SDATA.D32 )
	#define	NAND_IO_HwSADR_PA							*(volatile unsigned long*)0xF0050008		//( gNAND_IO_pHwND->SADR )
	#else
	#define	NAND_IO_HwCMD_PA							*(volatile unsigned long*)0xB0050000		//( gNAND_IO_pHwND->CMD )
	#define	NAND_IO_HwLADR_PA							*(volatile unsigned long*)0xB0050004		//( gNAND_IO_pHwND->LADR )
	#define	NAND_IO_HwDATA_PA							*(volatile unsigned long*)0xB0050010		//( gNAND_IO_pHwND->WDATA.D32 )
	#define	NAND_IO_HwLDATA_PA							*(volatile unsigned long*)0xB0050020		//( gNAND_IO_pHwND->LDATA )
	#define	NAND_IO_HwSDATA_PA							*(volatile unsigned long*)0xB005000C		//(( gNAND_IO_DataBusType == NAND_IO_MEM_BUS ) ? gNAND_IO_pHwND->WDATA.D8 : gNAND_IO_pHwND->SDATA.D32 )
	#define	NAND_IO_HwSADR_PA							*(volatile unsigned long*)0xB0050008		//( gNAND_IO_pHwND->SADR )
	#endif 
#elif defined(TCC892X)
#define	NAND_IO_HwCMD_PA							*(volatile unsigned long*)0x76600000		//( gNAND_IO_pHwND->CMD )
#define	NAND_IO_HwLADR_PA							*(volatile unsigned long*)0x76600004		//( gNAND_IO_pHwND->LADR )
#define	NAND_IO_HwSADR_PA							*(volatile unsigned long*)0x76600008		//( gNAND_IO_pHwND->SADR )
#define	NAND_IO_HwSDATA_PA							*(volatile unsigned long*)0x7660000C		//(( gNAND_IO_DataBusType == NAND_IO_MEM_BUS ) ? gNAND_IO_pHwND->WDATA.D8 : gNAND_IO_pHwND->SDATA.D32 )
#define	NAND_IO_HwDATA_PA							*(volatile unsigned long*)0x76600010		//( gNAND_IO_pHwND->WDATA.D32 )
#define	NAND_IO_HwLDATA_PA							*(volatile unsigned long*)0x76600020		//( gNAND_IO_pHwND->LDATA )
#endif
#else //NU
#if defined(TCC801X)
#define	NAND_IO_HwCMD_PA							*(volatile unsigned long*)0x8000D000		//( gNAND_IO_pHwND->CMD )
#define	NAND_IO_HwLADR_PA							*(volatile unsigned long*)0x8000D004		//( gNAND_IO_pHwND->LADR )
#define	NAND_IO_HwDATA_PA							*(volatile unsigned long*)0x8000D010		//( gNAND_IO_pHwND->WDATA.D32 )
#define	NAND_IO_HwLDATA_PA							*(volatile unsigned long*)0x8000D020		//( gNAND_IO_pHwND->LDATA )
#define	NAND_IO_HwSDATA_PA							*(volatile unsigned long*)0x8000D00C		//(( gNAND_IO_DataBusType == NAND_IO_MEM_BUS ) ? gNAND_IO_pHwND->WDATA.D8 : gNAND_IO_pHwND->SDATA.D32 )
#define	NAND_IO_HwSADR_PA							*(volatile unsigned long*)0x8000D008		//( gNAND_IO_pHwND->SADR )
#endif	//#if defined(TCC88XX)
#endif

#if defined(TCC_NAND_WP_B22)
#define	NAND_IO_NFC_nWP							HwGPIOB->GPDAT
#define	NAND_IO_NFC_nWPBit						Hw22
#elif defined(TCC_NAND_WP_B31)
#define	NAND_IO_NFC_nWP							HwGPIOB->GPDAT
#define	NAND_IO_NFC_nWPBit						Hw31
#elif defined(TCC_NAND_WP_B28)
#define	NAND_IO_NFC_nWP							HwGPIOB->GPDAT
#define	NAND_IO_NFC_nWPBit						Hw28
#elif defined(TCC_NAND_WP_B15)
#define	NAND_IO_NFC_nWP							HwGPIOB->GPDAT
#define	NAND_IO_NFC_nWPBit						Hw15
#elif defined(TCC_NAND_WP_B27)
#define	NAND_IO_NFC_nWP							HwGPIOB->GPDAT
#define	NAND_IO_NFC_nWPBit						Hw27
#elif defined(TCC_NAND_WP_A19)					
#define	NAND_IO_NFC_nWP							HwGPIOA->GPDAT
#define	NAND_IO_NFC_nWPBit						Hw19
#elif defined(TCC_NAND_WP_E17)
#define	NAND_IO_NFC_nWP							HwGPIOE->GPDAT
#define	NAND_IO_NFC_nWPBit						Hw17
#elif defined(TCC_NAND_WP_F27)
#define	NAND_IO_NFC_nWP							HwGPIOF->GPDAT
#define	NAND_IO_NFC_nWPBit						Hw27
#elif defined(TCC_NAND_WP_B8)
#define	NAND_IO_NFC_nWP							HwGPIOB->GPDAT
#define	NAND_IO_NFC_nWPBit						Hw8
#elif defined(TCC_NAND_WP_D0)
#define	NAND_IO_NFC_nWP							HwGPIOD->GPDAT
#define	NAND_IO_NFC_nWPBit						Hw0
#elif defined(TCC_NAND_WP_D8)
#define	NAND_IO_NFC_nWP							HwGPIOD->GPDAT
#define	NAND_IO_NFC_nWPBit						Hw8
#elif defined(TCC_NAND_WP_PK16)
#define	NAND_IO_NFC_nWP							HwGPIOPK_DAT
#define	NAND_IO_NFC_nWPBit						Hw16
#endif

#define NAND_IO_STATUS_ENABLE					0x0001
#define NAND_IO_STATUS_INTERLEAVING_MASK		0x00F0
#define NAND_IO_STATUS_INTERLEAVING_CHIP1		0x0010
#define NAND_IO_STATUS_INTERLEAVING_CHIP2		0x0020
#define NAND_IO_STATUS_INTERLEAVING_CHIP3		0x0040
#define NAND_IO_STATUS_INTERLEAVING_CHIP4		0x0080

#define NAND_IO_PORT_NAND_ON_CS0				0x0001
#define NAND_IO_PORT_NAND_ON_CS1				0x0002
#define NAND_IO_PORT_NAND_ON_CS2				0x0004
#define NAND_IO_PORT_NAND_ON_CS3				0x0008
#define NAND_IO_PORT_DATA_WITDH_8BIT			0x0010
#define NAND_IO_PORT_DATA_WITDH_16BIT			0x0020

#define NAND_IO_DISTRICT_0						0x0001
#define NAND_IO_DISTRICT_1						0x0002

#define NAND_IO_DMA_WRITE						0x0001
#define NAND_IO_DMA_READ						0x0002

#if defined(TCC89XX) || defined(TCC92XX)
#define NAND_SB_BOOT_PAGE_SIZE					260
#define NAND_SB_BOOT_PAGE_ECC_SIZE				288
#elif defined(TCC93XX) || defined(TCC88XX) || defined(TCC801X) || defined(TCC892X)
#define NAND_SB_BOOT_PAGE_SIZE					260
#define NAND_SB_BOOT_PAGE_ECC_SIZE				304
#endif 

#define YAFFS_TAG_SIZE							20

//*****************************************************************************
//*
//*
//*                       [ INTERNAL STRUCT DEFINE ]
//*
//*
//*****************************************************************************
#define GMC_NAND_NOMAL_BOOT_TYPE				0x54435342		// TCSB
#define GMC_NAND_SECURE_BOOT_TYPE				0x54435353		// TCSS

typedef struct	__tag_NAND_GOLDEN_INFO
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
} 	NAND_GOLDEN_INFO;

typedef struct	__tag_NAND_BOOTLOADER_INFO
{
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
} 	NAND_BOOTLOADER_INFO;

typedef struct __tag_NAND_IO_ECCSizeInfo
{
	unsigned int				EncodeFlag;
	unsigned int				DecodeFlag;
	unsigned int				ErrorNum;
	unsigned char				EccDataSize;
	unsigned char				*All_FF_512_ECC_Code;
} NAND_IO_ECC_INFO;

typedef struct __tag_NAND_IO_WriteStatus
{
	unsigned char				ChipNo;
	unsigned char				BlockStatus;
	unsigned int				ErrorPHYPageAddr;
} NAND_IO_WRITESTATUS;

typedef struct __tag_NAND_IO_BadBlockAddr
{
	unsigned char				BlockStatus[4];
	unsigned int				BadBlkPHYAddr[4];
} NAND_IO_BADBLOCK;

typedef struct __tag_NAND_IO_Cycle
{
	unsigned char				STP;
	unsigned char				PW;
	unsigned char				HLD;
	unsigned int				RegValue;
} NAND_IO_CYCLE;

typedef struct __tag_NAND_IO_DeviceCode
{
	unsigned short int			Code[6];			// Factory ID code
} NAND_IO_DEVID;

typedef struct __tag_NAND_IO_LBADevInfo
{
	unsigned char				Usable;
	
	unsigned char				CurrentMode;
	unsigned long int			CurrentSectorSize;
	
	unsigned char				HighSpeedMode;
	unsigned char				PowerSaveMode;
	unsigned char				TransProtocol1;
	unsigned char				TransProtocol2;
	unsigned char				SectorCount;
	unsigned char 				DataTransferCheck;
	unsigned char 				FlagOfChangeTotalSectorSize;
	unsigned char				MediaUniqueID[NAND_LBA_MAX_NANDFLASH_UID_SIZE];

	unsigned long int			VFPSectorSize;		// NBArea
	
	unsigned long int			MDPSectorSize;		// RWArea Totla Sector Size

	unsigned long int			DTAreaSectorSize;	// RWArea
	unsigned long int			DTAreaAddrOffSet;

	unsigned long int			HDAreaSectorSize;	// RWArea
	unsigned long int			HDAreaAddrOffSet;

	unsigned long int 			MHDAreaNums;	// Multi Hidden Area Num
	unsigned long int			MHDAreaSectorSize[NAND_LBA_MAX_SUPPORT_MHD_AREA_NUM];
	unsigned long int			MHDAreaAddrOffSet[NAND_LBA_MAX_SUPPORT_MHD_AREA_NUM];
	
} NAND_IO_LBADEVINFO;

typedef struct __tag_NAND_IO_Feature
{
	NAND_IO_DEVID				DeviceID;			// Maker & Device ID Code
	unsigned short int  		PBpV;				// Physical all Block Number
	unsigned short int			BBpZ;				// Total Bad Block in one ZONE
	unsigned short int  		PpB;				// Page Number Per Block
	unsigned short int  		PageSize;			// Page Size
	unsigned short int  		SpareSize;			// Spare Size
	unsigned short int			ColCycle;			// Column Address Cycle
	unsigned short int			RowCycle;			// Row Address Cycle
	unsigned short int			WCtime;				// Write Cycle time
	unsigned short int			WriteSTP;
	unsigned short int			WriteWP;
	unsigned short int			WriteHLD;
	unsigned short int			ReadSTP;
	unsigned short int			ReadPW;
	unsigned short int			ReadHLD;
	unsigned long int			MediaType;			// Characters of NANDFLASH
} NAND_IO_FEATURE;

typedef struct __tag_NAND_IO_DevInfo
{
	NAND_IO_FEATURE				Feature;			// Feature of NANDFLASH
	unsigned short int			IoStatus;			// IO Status
	unsigned short int			ChipNo;				// ChipSelect Number
	unsigned short int			CmdMask;			// Command Mask Bit
	unsigned short int			EccType;			// Type of ECC [ SLC , MLC4 ]
	unsigned short int			BytesPerSector;
	unsigned short int			EccDataSize;
	unsigned short int			EccWholeDataSize;

	unsigned short int			PPages;				// Total Partial Page [512+16Bytes]
	unsigned short int			UsablePPages;
	unsigned short int			DistrictNum;
	unsigned short int			ShiftDistrictNum;

	unsigned short int			ExtInterleaveUsable;

	unsigned short int			ShiftPBpV;
	unsigned short int			ShiftPpB;
	unsigned short int			ShiftPageSize;
	unsigned short int			ShiftPPages;
	unsigned short int			ShiftBytesPerSector;
	unsigned short int			ShiftUsablePPages;
	
	unsigned short int			PageUniteMode;
	unsigned short int			RemapPpB;			// Physical Page Number Per FTL 1Block
	unsigned short int			RemapPageOffset;	// Next FTL Block Page Offset
	unsigned short int			RemapRatioBSize;	// Physical Block Number Per FTL 1Block
	unsigned long int			RemapRatioPSize;	// Physical Block Number Per FTL 1Block
	
	unsigned short int			ShiftRemapPpB;
	unsigned short int			ShiftRemapPageOffset;
	unsigned short int			ShiftRemapRatioBSize;
	unsigned short int			ShiftRemapRatioPSize;

	unsigned long int			RemapPageWeight[4];

	unsigned long int			PrePageAddr;
	unsigned short int			PreStartPPage;
	unsigned short int			PreChipNo;

	unsigned char				DirectRead;
	NAND_IO_WRITESTATUS			WriteStatus;
		
	NAND_IO_BADBLOCK			BadBlockInfo;
	NAND_IO_LBADEVINFO			LBAInfo;
} NAND_IO_DEVINFO;

#define MAX_NAND_WORK_SECTOR_NUM	512

typedef	struct __tag_WriteStuffInfo {
	unsigned short int		StuffInfoNum;
	unsigned short int		StartPPageNo[MAX_NAND_WORK_SECTOR_NUM];
	unsigned short int		SectorSize[MAX_NAND_WORK_SECTOR_NUM];
	unsigned char			*nStuffPageBuffer;
} ndd_stuff_info;

typedef	struct __tag_NanddWorkInfo {
	unsigned int 			SectorNum;		// Total Sector Num
	unsigned int			StartSector;
	unsigned int			BufferAddr[MAX_NAND_WORK_SECTOR_NUM];

	unsigned int			PhyPageNum;
	unsigned int			PhyPageAddr[MAX_NAND_WORK_SECTOR_NUM];
	unsigned short int		StartPPageNo[1024];
	unsigned short int		PPageSize[1024];	// TNFTL_MAX_SUPPORT_NAND_IO_PAGE_SIZE_PER_1BLOCK
	unsigned int			FTLPPage;
	unsigned char			PageWriteFunc[1024];
	ndd_stuff_info			StuffInfo;
	unsigned char			*nSpareBuffer;
	NAND_IO_DEVINFO			*nDevInfo[MAX_NAND_WORK_SECTOR_NUM];
} ndd_work_info;

typedef struct __tag_NAND_IO_MakerInfo
{
	unsigned short int			MaxSupportNAND[MAX_SUPPORT_MAKER_NAND];
	unsigned short int			MakerID[MAX_SUPPORT_MAKER_NAND];
	NAND_IO_FEATURE*			DevInfo[MAX_SUPPORT_MAKER_NAND];
} NAND_IO_MAKERINFO;


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
extern void					NAND_IO_PowerUp(void);
extern void					NAND_IO_PowerDown(void);
extern NAND_IO_ERROR		NAND_IO_CallBackChangeWCtime( unsigned short int TotalMediaNum, NAND_IO_DEVINFO *nDevInfo );
extern NAND_IO_ERROR		NAND_IO_SetCycle( NAND_IO_DEVINFO *nDevInfo );
extern NAND_IO_ERROR		NAND_IO_GetDeviceInfo( U16 nChipNo, NAND_IO_DEVINFO *nDevInfo );
extern U32					NAND_IO_GetBUSTypeOfDataIO( void );
extern void					NAND_IO_Init( void );
extern void					NAND_IO_Reset( U16 nChipNo, int nMode );
extern void					NAND_IO_ResetForReadID( U16 nChipNo, int nMode );
extern void					NAND_IO_ReadID( U16 nChipNo, NAND_IO_DEVID *nDeviceCode, int nMode );
extern void                 NAND_IO_SetEDI( void );

extern NAND_IO_ERROR		NAND_IO_ReadSpare( NAND_IO_DEVINFO *nDevInfo, U32 nPageAddr, U8 *nSpareBuffer );
extern NAND_IO_ERROR 		NAND_IO_ReadSpareMTD( NAND_IO_DEVINFO *nDevInfo, U32 nPageAddr, U8 *nSpareBuffer );
extern NAND_IO_ERROR		NAND_IO_ReadPage( NAND_IO_DEVINFO *nDevInfo, U32 nPageAddr, U16 nStartPPage, U16 nReadPPSize, U8 *nPageBuffer, U8 *nSpareBuffer, int nEccOnOff );
extern NAND_IO_ERROR		NAND_IO_ReadPhyPage( NAND_IO_DEVINFO *nDevInfo, U32 nPageAddr, U16 nStartPPage, U16 nReadPPSize, U8 *nPageBuffer, U8 *nSpareBuffer, int nEccOnOff );
extern NAND_IO_ERROR		NAND_IO_ReadPageMTD(NAND_IO_DEVINFO * nDevInfo, U32 nPageAddr, U16 nStartPPage, U16 nReadPPSize, U8 * nPageBuffer, U8 * nSpareBuffer, int nEccOnOff);
extern NAND_IO_ERROR		NAND_IO_ReadTwoPlanePage(NAND_IO_DEVINFO * nDevInfo, U32 nPageAddr, U32 nSecondPageAddr, U16 nStartPPage, U16 nReadPPSize, U8 * nPageBuffer, U8 * nSpareBuffer, int nEccOnOff);
extern NAND_IO_ERROR		NAND_IO_ReadTwoPlaneLastPage(NAND_IO_DEVINFO * nDevInfo, U32 nPageAddr, U16 nStartPPage, U16 nReadPPSize, U8 * nPageBuffer, U8 * nSpareBuffer, int nEccOnOff);
extern NAND_IO_ERROR		NAND_IO_ReadUserSizePage( NAND_IO_DEVINFO *nDevInfo, U32 nPageAddr, U16 nColumnAddr, U32 nReadSize, U8 *nReadBuffer );

extern NAND_IO_ERROR 		NAND_IO_WriteSpare( NAND_IO_DEVINFO *nDevInfo, U32 nPageAddr, U8* nSpareBuffer, int nEccOnOff );
extern NAND_IO_ERROR		NAND_IO_WritePage( NAND_IO_DEVINFO *nDevInfo, U32 nPageAddr, U16 nStartPPage, U16 nWritePPSize, U8 *nPageBuffer, U8* nSpareBuffer, int nEccOnOff );
extern NAND_IO_ERROR 		NAND_IO_WritePageGANG( NAND_IO_DEVINFO *nDevInfo, U32 nPageAddr, U8 *nPageBuffer );
extern NAND_IO_ERROR		NAND_IO_WritePageMTD(NAND_IO_DEVINFO * nDevInfo, U32 nPageAddr, U16 nStartPPage, U16 nWritePPSize, U8 * nPageBuffer, U8 * nSpareBuffer, int nEccOnOff);
extern NAND_IO_ERROR		NAND_IO_WriteCachePage( NAND_IO_DEVINFO *nDevInfo, U32 nPageAddr, U16 nStartPPage, U16 nWritePPSize, U8 *nPageBuffer, U8* nSpareBuffer, int nEccOnOff );
extern NAND_IO_ERROR		NAND_IO_WriteTwoPlanePage( NAND_IO_DEVINFO *nDevInfo, U32 nPageAddr, U16 nStartPPage, U16 nWritePPSize, U8 *nPageBuffer, U8* nSpareBuffer, int nEccOnOff );
extern NAND_IO_ERROR		NAND_IO_WriteTwoPlaneLastPage( NAND_IO_DEVINFO *nDevInfo, U32 nPageAddr, U16 nStartPPage, U16 nWritePPSize, U8 *nPageBuffer, U8* nSpareBuffer, int LastPage, int nEccOnOff );
extern NAND_IO_ERROR		NAND_IO_WriteUserSizePage( NAND_IO_DEVINFO *nDevInfo, U32 nPageAddr, U16 nColumnAddr, U32 nWriteSize, U8 *nWriteBuffer );
extern NAND_IO_ERROR 		NAND_IO_MakeBootBinary( NAND_IO_DEVINFO *nDevInfo, U8 *nPageBuffer );

extern NAND_IO_ERROR		NAND_IO_CopyBackPage( NAND_IO_DEVINFO *nDevInfo, U32 nDesPageAddr, U32 nSrcPageAddr );
extern NAND_IO_ERROR		NAND_IO_CopyBackTwoPlanePage( NAND_IO_DEVINFO *nDevInfo, U32 nDesPageAddr, U32 nSrcPageAddr );

extern NAND_IO_ERROR		NAND_IO_EraseBlock( NAND_IO_DEVINFO *nDevInfo, U32 nBlockPageAddr, int nFormatMode );
extern NAND_IO_ERROR		NAND_IO_EraseBlockMTD( NAND_IO_DEVINFO *nDevInfo, U32 nBlockPageAddr, int nFormatMode );
extern NAND_IO_ERROR		NAND_IO_EraseBlockForTwoPlane( NAND_IO_DEVINFO *nDevInfo, U32 nBlockPageAddr, int nFormatMode );

extern NAND_IO_ERROR		NAND_IO_GetFactoryBadMarkOfPBlock( NAND_IO_DEVINFO *nDevInfo, U32 nBlockPageAddr, U32 mode );
extern NAND_IO_ERROR		NAND_IO_GetUID( NAND_IO_DEVINFO *nDevInfo, U16 *nCmd, U8 *rReadData );
extern NAND_IO_ERROR		NAND_IO_WaitBusyCheckForWriteEnd( NAND_IO_DEVINFO *nDevInfo );

extern NAND_IO_ERROR 		NAND_IO_EncDecodeBinary( unsigned int nEncDec, unsigned int nEccType, unsigned int nDataSize, U8 *nPageBuffer, U8 *nEccBuffer );
extern void 				NAND_IO_SetInterLeavePageAddr(U32 nPageAddr );
extern void 				NAND_IO_SetInterleaveStatus( NAND_IO_DEVINFO *nDevInfo, U32 nPageAddr );

extern void					NAND_IO_IRQ_Init( int mode );
extern void					NAND_IO_IRQ_Enable( unsigned int fEnable );
extern unsigned int 		NAND_IO_Module_IsInited( void );

extern NAND_IO_ERROR 		NAND_IO_IRQ_ReadPagePreProcess( NAND_IO_DEVINFO *nDevInfo, U32 nPageAddr, U16 nStartPPage, U32 *rEccBuffer );
extern void 				NAND_IO_IRQ_ReadPagePostProcess( void );
extern void 				NAND_IO_IRQ_Read512DataPreProcess( NAND_IO_DEVINFO *nDevInfo );
extern NAND_IO_ERROR 		NAND_IO_IRQ_Read512DataPostProcess( NAND_IO_DEVINFO *nDevInfo, U8* nPageBuffer, U8* nSpareBuffer );

extern NAND_IO_ERROR 		NAND_IO_IRQ_WritePagePreProcess( NAND_IO_DEVINFO *nDevInfo, U32 nPageAddr, U16 nStartPPage, U8 *nSpareBuffer, U32 *rEccBuffer, U8 nWriteMode );
extern void	 				NAND_IO_IRQ_WritePageMidProcess( NAND_IO_DEVINFO *nDevInfo, U8 nWriteMode );
extern void				 	NAND_IO_IRQ_WritePagePostProcess( void );
void 						NAND_IO_IRQ_Write512DataPreProcess( NAND_IO_DEVINFO *nDevInfo );
extern NAND_IO_ERROR 		NAND_IO_IRQ_Write512DataPostProcess( NAND_IO_DEVINFO *nDevInfo, U8* nECCBuffer );

extern NAND_IO_ERROR 		NAND_IO_IRQ_ExtInterruptClear(unsigned int irq);

extern void 				NAND_IO_IRQ_ReadyBusySet( void );
extern void 				NAND_IO_IRQ_ReadyBusyClear( void );

extern void 				NAND_IO_IRQ_SetupDMA( void *pDST, int nMode,  int nDSize  );
extern void 				NAND_IO_IRQ_SetupDMAForSpare( int nDSize );

extern void 				NAND_IO_WriteColAddr( U32 nColumnAddr, NAND_IO_DEVINFO *nDevInfo );
extern void					NAND_IO_SetCommCycleTime( void );
extern void 				NAND_IO_SetReadCycleTime(void);
extern NAND_IO_ERROR 		NAND_IO_WriteSpareData( NAND_IO_DEVINFO *nDevInfo, U8 *nSpareBuffer, int nPageEccOnOff );

#endif

