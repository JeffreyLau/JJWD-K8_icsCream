/****************************************************************************
 *   FileName    : fwdn_NAND.c
 *   Description : NAND FLASH F/W downloader function
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#if defined(_LINUX_) || defined(_WINCE_)
//#include "globals.h"
#include <tnftl/IO_TCCXXX.h>
//#include "fwdn_protocol.h"
#if defined(FWDN_V6)
#include <fwdn/fwdn_drv_v3.h>
#else
#include <fwdn/fwdn_drv_v7.h>
#endif
#include <fwdn/fwupgrade.h>
#if defined(TNFTL_V8_INCLUDE)
#include "nand_drv_v8.h"
#else
#include <tnftl/nand_drv.h>
#endif

#if defined (NKUSE)
	#include "windows.h"
	#include "stdlib.h"
#else
	#include <fwdn/TC_File.h>
#endif
#endif

#if defined(_WINCE_)
#include <memory.h>
#endif

//=============================================================================
//*
//*
//*                           [ CONST DATA DEFINE ]
//*
//*
//=============================================================================
#undef	LPRINTF
#define	LPRINTF	(1) ? (void)0 : IO_DBG_SerialPrintf_
#ifndef min
	#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//=============================================================================
//*
//*
//*                           [ Internal Functions ]
//*
//*
//=============================================================================


//=============================================================================
//*
//*
//*                     [ EXTERN VARIABLE & FUNCTIONS DEFINE ]
//*
//*
//=============================================================================
#if defined(TNFTL_V7_INCLUDE)
extern TNFTL_ERROR				TNFTL_ConvertMPPAtoFPA( TNFTL_DRVINFO *FTLInfo, U32, U32, U32 *rPHYBlkAddr, U16 *rPHYPageAddr );
extern TNFTL_ERROR				TNFTL_ConvertMFBAtoFBA( TNFTL_DRVINFO *FTLInfo, U32, U32, U32 *);

extern void						IO_DBG_SerialPrintf_(char *format, ...);
extern void						UI_API_DrawPopUp2Line(U8 *strUpBuf, U8 *strDownBuf);
extern void						UI_API_DrawPopUp3Line(U8 * strUpBuf, U8 * strMidBuf, U8 * strDownBuf);
extern void						TC_TimeDly(unsigned int ticks);
extern void						VTC_ReportProgress(int status, int error, int size, int pointer);
char * 							FWUG_GetTempBuffer(unsigned int *uiBufSize);
extern void						FWDN_FNT_InsertSN(unsigned char *pSerialNumber);
extern unsigned int				FWDN_FNT_SetSN(unsigned char * ucTempData, unsigned int uiSNOffset);
extern unsigned int				USBCLASS_MSC_TransmitData(void * lpBuffer, unsigned int nBufferSize);

extern void						FWUG_NAND_SetFlagCopyExistingRom( U32 nFlag );
extern void						FWUG_NAND_SetFlagOfGenEncryptUID( U32 nValue );
extern void						FWUG_NAND_SetFlagOfEraseEncryptUID( U32 nValue );
extern FWUG_NAND_ERROR			FWUG_NAND_locGetStAddrFromGMC( U8 nOrder, U32* rNandBootStBlockAddr, U32* nSecureMode );
extern FWUG_NAND_ERROR			FWUG_NAND_locSetParameter( unsigned short int *rPPages, unsigned short int *rPageSize, unsigned short int *rSpareSize );
extern unsigned int				FWUG_NAND_locGetEnableMaxRomFileSize( void );
#endif

extern unsigned short int		gFWUG_NAND_GoodBlockList[];
extern const unsigned int		CRC32_TABLE[256];

extern unsigned int				g_uiFWDN_OverWriteSNFlag;
extern unsigned int				g_uiFWDN_WriteSNFlag;
extern unsigned int				gMAX_ROMSIZE;	//twkwon: Initialize????

//extern int						gSDUpgrading;

unsigned int	gFileHandle; // Getting from WinCE Application
unsigned int 	gFileSize;

/*
int FWUG_MainFunc(int iFilehandle, int iFileSize)
{
	gFileHandle = iFilehandle;
	gFileSize = iFileSize;

	FwdnWriteNandFirmware(gFileSize);

	return 0;
}

#if defined (NKUSE)
unsigned int ReadBootFromApp(unsigned char* pBuffer, unsigned int uiSize)
{
	unsigned int uiReadByte;
	ReadFile((HANDLE)gFileHandle, pBuffer, uiSize, &uiReadByte, NULL);
	return uiReadByte;
}

#endif

*/


/**************************************************************************
*  FUNCTION NAME :
*
*      int		FwdnWriteNandFirmware(unsigned char *pucRomBuffer, unsigned uFWSize);
*
*  DESCRIPTION : Check Size, Prepare Buffer, Write ROM code into NAND Flash.
*
*  INPUT:
*			uFWSize	= Code Size (byte unit)
*
*  OUTPUT:	int - Return Type
*  			= 0 : successful
*			= other : failure
*
**************************************************************************/
#include <debug.h>

#define FWDN_BUF_SIZE		(64*1024)			// It should be larger then 2048 + 512
unsigned char	gFWDN_Buf[FWDN_BUF_SIZE]__attribute__((aligned(8)));

#if defined(TNFTL_V7_INCLUDE)
int		FwdnWriteNandFirmware( unsigned uFWSize )
{
	//#define			FWDN_WRITE_BUFFER_SIZE		(2*1024)
	#define			FWDN_WRITE_BUFFER_SIZE		(16*1024)	//twkwon

	int				i;
	unsigned int	res;
	unsigned int	uiBufSize;
	unsigned int	romFileSize;
	unsigned int	uRemainSize, uReadSize;
	unsigned int 	dwBlockOffSet, dwPageOffSet;
	unsigned int 	nStBlockOffSet, nStPageOffSet;
	unsigned char	GMC_Num;
	unsigned char	FlagofNewSizeBigger;
	#ifdef _LINUX_
	unsigned char 	nSerialNumBuf[512]__attribute__((aligned(8)));
	#else
	unsigned char 	nSerialNumBuf[512];
	#endif
	unsigned char	*buf;
	unsigned int	nSecureMode;
	unsigned long	*InitRoutine_Start 	=	CONFIG_CODE_START_ADDRESS;
	unsigned long	*InitRoutine_End	=	CONFIG_CODE_END_ADDRESS;
	unsigned long 	*FirmwareBase 		=	FIRMWARE_BASE_ADDRESS;

	unsigned long InitCode_Offset_ROM   = (*InitRoutine_Start) - (*FirmwareBase);
	unsigned long Init_Start 	 		= InitCode_Offset_ROM + ROMFILE_TEMP_BUFFER;
	unsigned long ulConfigSize 	 		= *InitRoutine_End - *InitRoutine_Start;
	romFileSize	= uFWSize;

	//===============================================================
	//	Check Size and Prepare Buffer
	//===============================================================
	if ( uFWSize > gMAX_ROMSIZE )
		return ERR_FWUG_FAIL_ROMFILESIZEBIG;

	buf = (unsigned char*)gFWDN_Buf;
	uiBufSize = FWDN_WRITE_BUFFER_SIZE;

	// NAND BOOT Only Mode Set
	#ifdef NANDBOOTONLY_MODE
	FWUG_NAND_SetEnableNandBootOnlyMode( ENABLE );
	FWUG_NAND_SetNBAreaEndPBAdd(40);
	#endif

	//===============================================================
	//	Write Code
	//===============================================================
	/* Setting Intial Parameters */

	#ifdef TNFTL_V7_INCLUDE
	FWUG_NAND_SetFlagCopyExistingRom( DISABLE );
	FWUG_NAND_SetEnableNandBootOnlyMode( ENABLE );
	FWUG_NAND_SetNBAreaEndPBAdd(TNFTL_MAX_BLOCK_NUM_OF_NBAREA);
	FWUG_NAND_PreMemInitCode( (unsigned char *)Init_Start, ulConfigSize );	//Mem Init Code Setting
	#endif

	nSecureMode	= FWUG_NAND_GetFlagOfUseSecureMode();

	/* PreProcess before write code */
	if (( res = FWUG_NAND_PreProcess( romFileSize, &FlagofNewSizeBigger )) != SUCCESS )
		return res;

	/* Write CODE Data */
	for ( i = 0 ; i < 2; ++i )
	{
		unsigned int srcAddr;
		uRemainSize = romFileSize;

		srcAddr = 0;

		if ( FlagofNewSizeBigger == TRUE )
			GMC_Num = ( i == 0 ) ? FIRST : SECOND;
		else
			GMC_Num = ( i == 0 ) ? SECOND : FIRST;

		if (( res = FWUG_NAND_WriteCodePreProcess( GMC_Num, romFileSize, &nStBlockOffSet, &nStPageOffSet, nSecureMode )) != SUCCESS )
			return res;

		while ((int)uRemainSize > 0)
		{
			unsigned int percent;

			percent = (srcAddr/(romFileSize/50));
			if( i == 1 )
				percent += 50;
			uReadSize = ( uRemainSize >= uiBufSize ) ? uiBufSize : uRemainSize;

			FWDN_DRV_FirmwareWrite_Read( srcAddr, buf, uReadSize, percent );
			#ifdef TNFTL_V7_INCLUDE
			if (( res = FWUG_NAND_WriteCodeNAND( GMC_Num, nStBlockOffSet, nStPageOffSet, (unsigned char* )(ROMFILE_TEMP_BUFFER + srcAddr), uReadSize, &dwBlockOffSet, &dwPageOffSet, nSecureMode )) != SUCCESS )
				return res;
			#else
			if (( res = FWUG_NAND_WriteCodeNAND( nStBlockOffSet, nStPageOffSet, (unsigned char* )(ROMFILE_TEMP_BUFFER + srcAddr), uReadSize, &dwBlockOffSet, &dwPageOffSet, nSecureMode )) != SUCCESS )
				return res;
			#endif

			srcAddr += uReadSize;

			nStBlockOffSet = dwBlockOffSet;
			nStPageOffSet  = dwPageOffSet;
			uRemainSize	-= uReadSize;
		}

		if (( res = FWUG_NAND_WriteCodePostProcess( GMC_Num, nStBlockOffSet, nStPageOffSet )) != SUCCESS )
			return res;
	}

	/* PostProcess after write code */
	FWDN_FNT_InsertSN( nSerialNumBuf );
	if (( res = FWUG_NAND_PostProcess( nSerialNumBuf, g_uiFWDN_WriteSNFlag, nSecureMode )) != SUCCESS )
		return res;

	return 0;
}

/**************************************************************************
*  FUNCTION NAME :
*
*      int	FwdnGetNandSerial(void);
*
*  DESCRIPTION : Read and Check the Serial Number stored at NAND Flash.
*                        Verification result is reflected to global structure of FWDN_DeviceInformation.
*
*  INPUT:
*			None
*
*  OUTPUT:	int - Return Type
*  			= always 0
*
**************************************************************************/
int	FwdnGetNandSerial(void)
{
	unsigned char		ucTempData[512];

	/*Get serial number*/
	memset ( ucTempData, 0xFF, 512);

	FWUG_NAND_GetSerialNumberNAND( ucTempData );

	FWDN_FNT_VerifySN( ucTempData, 0 );

	return 0;
}

/**************************************************************************
*  FUNCTION NAME :
*
*      int	FwdnClearNandHiddenArea(unsigned int start, unsigned int pagesize);
*
*  DESCRIPTION : Clear Hidden Area of NAND Flash
*
*  INPUT:
*			pagesize	= not used
*			start	= not used
*
*  OUTPUT:	int - Return Type
*  			=
*
*  REMARK:	created on 2007/8/2    17:28:20    by vizirdo
**************************************************************************/
int	FwdnClearNandHiddenArea(unsigned int start, unsigned int pagesize)
{
	start		= start;
	pagesize	= pagesize;

	return TNFTL_AREAFormat( gNAND_DrvInfo[0].NFTLDrvInfo,
					  &gNAND_DrvInfo[0].NFTLDrvInfo->ExtPartition[0],
					  0);			// Quick Level
}


#elif defined(TNFTL_V8_INCLUDE)

int		FwdnWriteNandFirmware( unsigned char *pucRomBuffer, unsigned uFWSize )
{
	int res = 0;

#if defined(TNFTL_V8_INCLUDE)
	res = NAND_WriteFirmware(0,pucRomBuffer,uFWSize);
#else
	{
		U32 nSecureMode;
		U8 FlagofNewSizeBigger;
		unsigned int i;
		unsigned long ulBaseAddress = GetDword(&pucRomBuffer[ROM_FIRMWARE_BASE_OFFSET]);
		unsigned long ulNandLoaderOffset = GetDword(&pucRomBuffer[ROM_BL1_NAND_OFFSET]) - ulBaseAddress;
		unsigned long ulMemInitCodeOffset = GetDword(&pucRomBuffer[ROM_CONFIG_CODE_START_OFFSET]) - ulBaseAddress;
		unsigned int uiMemInitCodeSize = GetDword(&pucRomBuffer[ROM_CONFIG_CODE_END_OFFSET]) - GetDword(&pucRomBuffer[ROM_CONFIG_CODE_START_OFFSET]);

	    //===============================================================
	    //	Check Size and Prepare Buffer
	    //===============================================================
	    if ( uFWSize > gMAX_ROMSIZE )
		    return ERR_FWUG_FAIL_ROMFILESIZEBIG;

	    // NAND BOOT Only Mode Set
	    #ifdef NANDBOOTONLY_MODE
	    FWUG_NAND_SetEnableNandBootOnlyMode( ENABLE );
	    FWUG_NAND_SetNBAreaEndPBAdd(40);
	    #endif

	    //===============================================================
	    //	Write Code
	    //===============================================================
	    /* Setting Intial Parameters */
	    FWUG_NAND_SetFlagCopyExistingRom( DISABLE );
	    FWUG_NAND_SetEnableNandBootOnlyMode( ENABLE );
	    FWUG_NAND_SetNBAreaEndPBAdd(TNFTL_MAX_BLOCK_NUM_OF_NBAREA);
	    FWUG_NAND_PreMemInitCode( &pucRomBuffer[ulMemInitCodeOffset], uiMemInitCodeSize );	//Mem Init Code Setting

	    nSecureMode	= FWUG_NAND_GetFlagOfUseSecureMode();

	    /* PreProcess before write code */
	    if (( res = FWUG_NAND_PreProcess( uFWSize, &FlagofNewSizeBigger )) != SUCCESS )
		    return res;

	    /* Write CODE Data */
	    for ( i = 0 ; i < 2; ++i )
	    {
		    unsigned int srcAddr;
		    //unsigned uRemainSize = uFWSize;
			U8 GMC_Num;
			U32 nStBlockOffSet,nStPageOffSet;

		    srcAddr = 0;

		    if ( FlagofNewSizeBigger == TRUE )
			    GMC_Num = ( i == 0 ) ? FIRST : SECOND;
		    else
			    GMC_Num = ( i == 0 ) ? SECOND : FIRST;

		    if (( res = FWUG_NAND_WriteCodePreProcess( GMC_Num, uFWSize, &nStBlockOffSet, &nStPageOffSet, nSecureMode )) != SUCCESS )
			    return res;

		    //while ((int)uRemainSize > 0)
		    {
			    //unsigned int percent;
				U32 uReadSize,dwBlockOffSet,dwPageOffSet;

			    //percent = (srcAddr/(uFWSize/50));
			    //if( i == 1 )
				//    percent += 50;
			    //uReadSize = ( uRemainSize >= uiBufSize ) ? uiBufSize : uRemainSize;
				uReadSize = uFWSize;

			    if (( res = FWUG_NAND_WriteCodeNAND( GMC_Num, nStBlockOffSet, nStPageOffSet, (unsigned char* )(ROMFILE_TEMP_BUFFER + srcAddr), uReadSize, &dwBlockOffSet, &dwPageOffSet, nSecureMode )) != SUCCESS )
				    return res;

			    srcAddr += uReadSize;

			    nStBlockOffSet = dwBlockOffSet;
			    nStPageOffSet  = dwPageOffSet;
			    //uRemainSize	-= uReadSize;
		    }

		    if (( res = FWUG_NAND_WriteCodePostProcess( GMC_Num, nStBlockOffSet, nStPageOffSet )) != SUCCESS )
			    return res;
		}

		/* PostProcess after write code */
		{
			#ifdef _LINUX_
			unsigned char 	nSerialNumBuf[512]__attribute__((aligned(8)));
			#else
			unsigned char 	nSerialNumBuf[512];
			#endif

			FWDN_FNT_InsertSN( nSerialNumBuf );
			if (( res = FWUG_NAND_PostProcess( nSerialNumBuf, g_uiFWDN_WriteSNFlag, nSecureMode )) != SUCCESS )
				return res;
		}
	}
#endif

	return res;
}

/**************************************************************************
*  FUNCTION NAME :
*
*      int	FwdnGetNandSerial(unsigned char *pucSN, unsigned int uiSize);
*
*  DESCRIPTION : Read and Check the Serial Number stored at NAND Flash.
*                        Verification result is reflected to global structure of FWDN_DeviceInformation.
*
*  INPUT:
*			None
*
*  OUTPUT:	int - Return Type
*  			= always 0
*
**************************************************************************/
int	FwdnGetNandSerial(unsigned char *pucSN, unsigned int uiSize)
{
#if defined(TNFTL_V8_INCLUDE)
	return NAND_GetSerialNumber(pucSN, uiSize);
#else
	unsigned char		ucTempData[512];

	/*Get serial number*/
	memset ( ucTempData, 0xFF, 512);

	FWUG_NAND_GetSerialNumberNAND( ucTempData );

	FWDN_FNT_VerifySN( ucTempData, 0 );

	memcpy(pucSN,ucTempData,min(uiSize,32));
	return min(uiSize,32);
#endif
}

#endif



/**************************************************************************
*  FUNCTION NAME :
*
*      int FwdnSetNandSerial(unsigned char *ucData, unsigned int overwrite);
*
*  DESCRIPTION : Set Serial Number into global structure of FWDN_DeviceInformation.
*
*  INPUT:
*			ucData	= Serial Number
*			overwrite	=	0 : check existing serial number and not overwrite if that is valid.
*						1 : overwrite serial number regardless of existing one.
*
*  OUTPUT:	int - Return Type
*  			= always 0
*
**************************************************************************/
int FwdnSetNandSerial(unsigned char *ucData, unsigned int overwrite)
{
#if defined(TNFTL_V8_INCLUDE)
	memcpy( FWDN_DeviceInformation.DevSerialNumber, ucData , 32 );

	if( overwrite )
	{
		NAND_SetSerialNumber(ucData, 32);
	}
#else
	unsigned char		ucTempData[512];

	if ( overwrite & 0x8000 )
	{
		FWUG_NAND_SetFlagOfGenEncryptUID( ENABLE );
		overwrite &= ~(0x8000);
	}
	else if ( overwrite & 0x4000 )
	{
		FWUG_NAND_SetFlagOfEraseEncryptUID( ENABLE );
		overwrite &= ~(0x4000);
	}

	memcpy( FWDN_DeviceInformation.DevSerialNumber, ucData , 32 );
	g_uiFWDN_OverWriteSNFlag	=  overwrite;
	g_uiFWDN_WriteSNFlag		= 0;

	if ( overwrite == 0 )
	{
		/*Get serial number*/
		memset( ucTempData, 0xFF, 512 );

		FWUG_NAND_GetSerialNumberNAND( ucTempData );

		FWDN_FNT_SetSN( ucTempData, 0 );
	}
#endif

	return 0;
}

/* end of file */
