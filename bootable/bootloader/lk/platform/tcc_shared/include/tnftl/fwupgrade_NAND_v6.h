/****************************************************************************
 *   FileName    : fwupgrade_NAND.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#ifndef _FWUPGRADE_NAND_H_
#define _FWUPGRADE_NAND_H_

#if defined(NAND_BOOT_REV)
	#if defined(_LINUX_)
	#include <tnftl/tnftl_v7.h>
	#elif defined(_WINCE_)
	#include "tnftl_v7.h"
	#else
	#include "tnftl_v7.h"
	#endif
#else
    #if defined(_LINUX_)
    #include <tnftl/tnftl_v6.h>
    #elif defined(_WINCE_)
    #include "tnftl_v6.h"
    #else
    #include "tnftl_v6.h"
    #endif
#endif

//*****************************************************************************
//*
//*
//*                       [ General DEFINE & TYPEDEF ]
//*
//*
//*****************************************************************************
#ifndef ON
#define ON									0x01
#endif
#ifndef OFF
#define OFF 								0x00
#endif
#ifndef DISABLE
#define DISABLE								0
#endif
#ifndef ENABLE
#define	ENABLE								1
#endif
#ifndef FALSE
#define FALSE 	          					0
#endif
#ifndef TRUE
#define TRUE            					1
#endif

//*****************************************************************************
//*
//*
//*                       [ EXTERNAL GLOBAL VARIABLE DEFINE ]
//*
//*
//*****************************************************************************
#define	BIG										0x04
#define	SMALL									0x00
#define	PARALLEL								0x08
#define	SERIAL									0x00

#define EXTENDED_PAGE							0x80
#define NORMAL_PAGE								0x00
#define WIDE_SPARE								0x08
#define NORMAL_SPARE							0x00

#define MAX_GOOD_BLOCK_LIST_LEN					(759*2)

#if defined(TNFTL_V5_INCLUDE) || defined(TNFTL_V6_INCLUDE)
#define FWUG_NAND_IO_READPAGE					NAND_IO_ReadNBPage
#define FWUG_NAND_IO_WRITEPAGE					NAND_IO_WriteNBPage
#else
#define FWUG_NAND_IO_READPAGE					NAND_IO_ReadPage
#define FWUG_NAND_IO_WRITEPAGE					NAND_IO_WritePage
#endif

//*****************************************************************************
//*
//*
//*                         [ ERROR CODE ENUMERATION ]
//*
//*
//*****************************************************************************
#ifndef SUCCESS
#define	SUCCESS  0
#endif

typedef enum
{
	ERR_FWUG_NAND_WRONG_PARAMETER							= 0xC000000,
	ERR_FWUG_NAND_FAILED_GET_INFO_GMC						= 0xC000001,
	ERR_FWUG_NAND_FAILED_GET_SDCFG_FROM_MC					= 0xC000002,
	ERR_FWUG_NAND_FAILED_READY_NB_AREA						= 0xC000003,
	ERR_FWUG_NAND_FAILED_COPY_IMAGE							= 0xC000004,
	ERR_FWUG_NAND_FAILED_SET_START_ADDRESS_IN_GMC			= 0xC000005,
	ERR_FWUG_NAND_FAILED_WRITE_MASTER_CLUSTER				= 0xC000006,
	ERR_FWUG_NAND_FAILED_ROMFILESIZE_OVER					= 0xC000007,
	ERR_FWUG_NAND_FAILED_CHECK_CRC_ROMCODE					= 0xC000008,
	ERR_FWUG_NAND_FAILED_WRITE_CODE_DATA					= 0xC000009,
	ERR_FWUG_NAND_FAILED_WRITE_MC_DATA						= 0xC00000A,
	ERR_FWUG_NAND_FAILED_WRITE_GMC_DATA						= 0xC00000B,
	ERR_FWUG_NAND_FAILED_NO_BMP								= 0xC00000C,
	ERR_FWUG_NAND_FAILED_NOT_EXIST_NANDBOOT_AREA			= 0xC00000D,
	ERR_FWUG_NAND_FAILED_CHECK_CRC_MC						= 0xC00000E,
	ERR_FWUG_NAND_FAILED_NOT_EXIST_NANDBOOT_SIG				= 0xC00000F,
	ERR_FWUG_NAND_FAILED_NOT_EXIST_MC_DATA					= 0xC000010,
	ERR_FWUG_NAND_FAILED_MEM_INITCODESIZE_OVER				= 0xC000011,
	ERR_FWUG_NAND_FAILED_ROMBOUNDARY_OVER					= 0xC000012,
	ERR_FWUG_NAND_FAILED_TO_FIND_MEMINIT_SRC_BLOCK			= 0xC000013,
	ERR_FWUG_NAND_FAILED_TO_FIND_MEMINIT_DST_BLOCK			= 0xC000014
} FWUG_NAND_ERROR;

//*****************************************************************************
//*
//*
//*                       [ EXTERNAL FUCTIONS DEFINE ]
//*
//*
//*****************************************************************************
extern void					FWUG_NAND_SetEnableNandBootOnlyMode( U32 value );
extern void					FWUG_NAND_SetParameterForMC( U32 uiSDRAMConfig, U32 uiStExeAddr );
extern void					FWUG_NAND_SetFlagOfUseSecureMode( U32 nValue );
extern U32					FWUG_NAND_GetFlagOfUseSecureMode( void );
extern U8    				FWUG_NAND_GetCurrentNANDType( void );
extern FWUG_NAND_ERROR 		FWUG_NAND_SetNBAreaEndPBAdd( U32 nPhyBlockAddr );
extern FWUG_NAND_ERROR 		FWUG_NAND_ClearHDArea( void );
extern FWUG_NAND_ERROR		FWUG_NAND_RecoveryImageFile( U16 nImageNo  );
extern FWUG_NAND_ERROR		FWUG_NAND_PreProcess( U32 uiROMFileSize, U8* pFlagNewBig );
extern FWUG_NAND_ERROR		FWUG_NAND_PostProcess( U8* nSNBuf, U32 nSNMode, U32 nSecureMode );
extern FWUG_NAND_ERROR 		FWUG_NAND_PreMemInitCode( U8 *WriteBuf, U32 nMemInitSize );

extern FWUG_NAND_ERROR		FWUG_NAND_WriteCodePreProcess( U8 ucNum, U32 uiROMFileSize, U32 *rStBlockOffSet, U32 *rStPageOffSet, U32 nSecureMode );
#ifdef TNFTL_V7_INCLUDE
extern FWUG_NAND_ERROR		FWUG_NAND_WriteCodeNAND( U16 nRomNum, U32 nStBlockOffSet, U32 nStPageOffSet, U8 *WriteBufAddr, U32 iBufSize, U32 *rBlockOffSet, U32 *rPageOffSet, U32 nSecureMode );
#else
extern FWUG_NAND_ERROR		FWUG_NAND_WriteCodeNAND( U32 nStBlockOffSet, U32 nStPageOffSet, U8* WriteBufAddr, U32 iBufSize, U32 *rBlockOffSet, U32 *rPageOffSet, U32 nSecureMode );
#endif
extern FWUG_NAND_ERROR		FWUG_NAND_WriteCodePostProcess( U8 ucNum, U32 nStBlockOffSet, U32 nStPageOffSet );
extern FWUG_NAND_ERROR		FWUG_NAND_GetSerialNumberNAND( U8* nSID );
extern unsigned int			FWUG_NAND_CRCUpdate(unsigned int * pBuffer, unsigned int nBufferSize, unsigned int StCRCout, unsigned int nMode, unsigned int nRemainSize);

extern FWUG_NAND_ERROR		FWUG_NAND_LBA_GetSerialNumberNAND( U8* nSID );
extern void					FWUG_NAND_LBA_InitVariable(U32 nCrc128k, U32 nCrc256k);
extern FWUG_NAND_ERROR		FWUG_NAND_LBA_WriteCodePreProcess(U8 ucNum, U32 uiROMFileSize, U32 * rStBlockOffSet, U32 * rStPageOffSet, U32 nSecureMode);
extern FWUG_NAND_ERROR		FWUG_NAND_LBA_WriteCodeNAND(U32 nStBlockOffSet, U32 nStPageOffSet, U8 * WriteBufAddr, U32 iBufSize, U32 * rBlockOffSet, U32 * rPageOffSet, U32 nSecureMode);
extern FWUG_NAND_ERROR		FWUG_NAND_LBA_WriteMC(unsigned char ucNum);
extern FWUG_NAND_ERROR 	 	FWUG_NAND_LBA_WriteGMC(U8 * nSNBuf, U32 nSNMode, U32 nSecureMode);
extern FWUG_NAND_ERROR		FWUG_NAND_LBA_WriteCodetoVFP(U32 nSectorAddr, U8 * WriteBufAddr, U32 iBufSize, U32 * rSectorAddr, U32 nSecureMode);
extern FWUG_NAND_ERROR		FWUG_NAND_LBA_PostProcess(U8 nMode);

extern FWUG_NAND_ERROR		FWUG_NAND_RecoveryMeminitCode( unsigned char rSrcBlk, unsigned char rDstBlk );

#endif	// _FWUPGRADE_NAND_H_

/* end of file */

