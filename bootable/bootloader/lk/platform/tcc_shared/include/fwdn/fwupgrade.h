/****************************************************************************
 *   FileName    : fwupgrade.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#ifndef _FWUPGRADE_H_
#define _FWUPGRADE_H_

#if defined(_LINUX_)
#include <common.h>
//#include <fwdn/main.h>
#elif defined(_WINCE_)
//#include "main.h"
#endif

//**********************************************************************
//*		Define FWUG Library version for ChipSet
//**********************************************************************
#if 0
#if defined(NU_FILE_INCLUDE)
	#if defined(TCC78X)
	#define FWUG_V1_INCLUDE
	#else
	#define FWUG_V2_INCLUDE
	#endif
#else	/* K-FILESYSTEM */
#define FWUG_V1_INCLUDE
#endif
#else
	#define FWUG_V2_INCLUDE		//twkwon
#endif

//*****************************************************************************
//*
//*
//*                       [ General DEFINE & TYPEDEF ]
//*
//*
//*****************************************************************************
#ifndef ON
#define ON							0x01
#endif
#ifndef OFF
#define OFF 						0x00
#endif
#ifndef DISABLE
#define DISABLE						0
#endif
#ifndef ENABLE
#define	ENABLE						1
#endif
#ifndef FALSE
#define FALSE 	          			0
#endif
#ifndef TRUE
#define	TRUE						(!FALSE)
#endif
#ifndef NULL
#define	NULL						(0)
#endif

#if 0
#ifndef U8
typedef unsigned char				U8;
#endif
#ifndef U16
typedef unsigned short int			U16;
#endif
#ifndef U32
typedef unsigned int				U32;
#endif
#endif

#ifndef GetDword
#define GetDword(x)					(((x)[0]) + ((x)[1]<<8) + ((x)[2]<<16) + ((x)[3]<<24))
#endif // GetDword


#if defined(_WINCE_)
// common
#define ROM_FIRMWARE_BASE_OFFSET				0x1060
#define ROM_CONFIG_CODE_START_OFFSET			0x1070
#define ROM_CONFIG_CODE_END_OFFSET				0x1074
// SD/MMC
#define ROM_BOOT_AREA_SIZE_OFFSET				0x1078
#define ROM_VALID_HIDDEN_COUNT					0x107C
#define ROM_HIDDEN_AREA_SIZE_OFFSET				0x1080
// NAND
#if defined(TNFTL_V8_INCLUDE)
#define ROM_TNFTL_STRUCTURE_VERSION_OFFSET		0x1078
#define ROM_BL1_NAND_OFFSET						0x107C
#else
#define ROM_BL1_NAND_OFFSET						0x1078
#endif
#elif defined(_LINUX_)
// common
#define ROM_FIRMWARE_BASE_OFFSET				0x60
#define ROM_CONFIG_CODE_START_OFFSET			0x70
#define ROM_CONFIG_CODE_END_OFFSET				0x74
// SD/MMC
#define ROM_BOOT_AREA_SIZE_OFFSET				0x78
#define ROM_VALID_HIDDEN_COUNT_OFFSET			0x7C
#define ROM_HIDDEN_AREA_SIZE_OFFSET				0x80
// NAND
#if defined(TNFTL_V8_INCLUDE)
#define ROM_TNFTL_STRUCTURE_VERSION_OFFSET		0x78
#define ROM_BL1_NAND_OFFSET						0x7C
#else
#define ROM_BL1_NAND_OFFSET						0x78
#endif
#else
#error
#endif

#if defined(NAND_BOOT_INCLUDE)
#define ROMFILE_TEMP_BUFFER KERNEL_ADDR + 0x100000
#elif defined(BOOTSD_BOOT_INCLUDE)
#define ROMFILE_TEMP_BUFFER BASE_ADDR + 0x03000000
#endif

#define CONFIG_CODE_START_ADDRESS		ROMFILE_TEMP_BUFFER + ROM_CONFIG_CODE_START_OFFSET
#define CONFIG_CODE_END_ADDRESS			ROMFILE_TEMP_BUFFER + ROM_CONFIG_CODE_END_OFFSET
#define FIRMWARE_BASE_ADDRESS			ROMFILE_TEMP_BUFFER + ROM_FIRMWARE_BASE_OFFSET

#define BOOT_AREA_SIZE_ADDRESS			ROMFILE_TEMP_BUFFER + ROM_BOOT_AREA_SIZE_OFFSET
#define HIDDEN_AREA_SIZE_ADDRESS		ROMFILE_TEMP_BUFFER + ROM_HIDDEN_AREA_SIZE_OFFSET
#define VALID_HIDDEN_COUNT_ADDRESS		ROMFILE_TEMP_BUFFER + ROM_VALID_HIDDEN_COUNT_OFFSET

enum
{
	FIRST = 0,
	SECOND,
	REPETITIONNUM	
};

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
	ERR_FWUG_NOT_EXISTMEMORY = 0x1000,
	ERR_FWUG_FAIL_OPENROMFILE,
	ERR_FWUG_FAIL_CLOSEROMFILE,	
	ERR_FWUG_FAIL_READROMFILE,
	ERR_FWUG_CANCEL_FWUPGRADE,
	ERR_FWUG_FAIL_BATCHECK,
	ERR_FWUG_FAIL_ROMFILESIZEBIG,
	ERR_FWUG_FAIL_GETGOODBLOCKLIST,
	ERR_FWUG_NOT_EXISTSERIALNUM,
	ERR_FWUG_FAIL_GMCDATAWRITE,
	ERR_FWUG_FAIL_GMCSPAREWRITE,
	ERR_FWUG_FAIL_CODEDATAWRITE,
	ERR_FWUG_FAIL_CODESPAREWRITE,
	ERR_FWUG_FAIL_MCDATAWRITE,
	ERR_FWUG_FAIL_MCSPAREWRITE,
	ERR_FWUG_FAIL_FWUPGRADE,
	ERR_FWUG_FAIL_CHECK_TFLASH,
	ERR_FWUG_FAIL_SCAN_TFLASH,
	ERR_FWUG_FAIL_CODEWRITETFLASH,
	ERR_FWUG_FAIL_CODEREADTFLASH,
	ERR_FWUG_FAIL_HEADERWRITETFLASH,
	ERR_FWUG_FAIL_HEADERREADTFLASH,
	ERR_FWUG_FAIL,
	ERR_FWUG_FAIL_VERIFY
} FWUG_ERROR;

typedef enum
{
	ERR_FWUG_FAIL_MEMORY_ALLOCATION = 0x20000000,
	ERR_FWUG_FAIL_NO_DEVICE,
	ERR_FWUG_FAIL_NOT_ENOUGH_SPACE,
	ERR_FWUG_FAIL_WRITE,
	ERR_FWUG_FAIL_READ,
	ERR_FWUG_FAIL_COMPARE,
	ERR_FWUG_FAIL_COMMUNICATION,
	ERR_FWUG_FAIL_NO_DRIVER
} FWUG_ERROR_T;

typedef	int (*fFWUG_ReadDATA)(unsigned int uDest, unsigned int uSize, unsigned int percent);

//*****************************************************************************
//*
//*
//*                       [ EXTERNAL FUCTIONS DEFINE ]
//*
//*
//*****************************************************************************
extern unsigned char			gFW_FirmwareDevice, gFW_SerialNumberDevice;

extern void* 					fwdn_get_buffer_for_nand_gang(void);
#if defined(TNFTL_V7_INCLUDE)
extern char						*FWUG_GetTempBuffer(unsigned int *uiBufSize);
#elif defined(TNFTL_V8_INCLUDE)
extern void *					FWUG_GetTempBuffer(unsigned int uiBufSize);
#endif
extern int						FWUG_ReadDATA(unsigned int uDest, void *uHandler, unsigned int uSize, unsigned int percent);
extern unsigned int				CalCRC_ROMFile(unsigned int *pBuffer,unsigned int size,unsigned int crcout, unsigned int mode);
extern int						FWUG_VerifySerialNumber(unsigned char* ucBuf, unsigned int uiOffset);

extern unsigned int				FWUG_CalcCrc(unsigned int *base, unsigned int length);
extern unsigned int				FWUG_CalcCrc8(unsigned char *base, unsigned int length);
extern unsigned int				FWUG_CalcCrcI(unsigned uCRCIN, unsigned *base, unsigned int length);
extern unsigned int				FWUG_CalcCrc8I(unsigned uCRCIN, unsigned char *base, unsigned int length);

/* TFLASH */
extern unsigned int				FwdnReadTriflashFirmware(unsigned int master);
extern int						FwdnWriteTriflashFirmware(unsigned uFWSize);
extern int						FwdnGetTriflashSerial(void);
extern int						FwdnSetTriflashSerial(unsigned char *ucData, unsigned int overwrite);
extern int						FwdnClearTriflashHiddenArea(void);
/* HDD */
extern int						FwdnClearHddHiddenArea(void);
/* NOR */
extern unsigned int				FwdnReadNorFlashFirmware(unsigned int master);
extern int						FwdnWriteNorFlashFirmware(unsigned uFWSize);
extern int						FwdnGetNorSerial(void);
extern int						FwdnSetNorSerial(unsigned char *ucData, unsigned int overwrite);
/* NAND */
extern unsigned int 			FwdnReadNandFirmware(unsigned int master);
#if defined(TNFTL_V7_INCLUDE)
extern int						FwdnWriteNandFirmware(unsigned uFWSize);
extern int						FwdnGetNandSerial(void);
#elif defined(TNFTL_V8_INCLUDE)
extern int						FwdnWriteNandFirmware( unsigned char *pucRomBuffer, unsigned uFWSize );
extern int						FwdnGetNandSerial(unsigned char *pucSN, unsigned int uiSize);
#endif
extern int 						FwdnSetNandSerial(unsigned char *ucData, unsigned int overwrite);
extern int						FwdnClearNandHiddenArea(unsigned int start, unsigned int pagesize);
extern void 					FWDN_FNT_VerifySN(unsigned char* ucTempData, unsigned int uiSNOffset);
/* Serial NOR */
extern int						FwdnWriteSerialNorFirmware(unsigned int uiROMFileSize);
extern int						FwdnGetSerialNorSerial(void);
extern int						FwdnSetSerialFlashSerial(unsigned char *ucData, unsigned int overwrite);
/* EEPROM */
extern int						FwdnWriteEEPROMFirmware(unsigned uFWSize);

#endif	// _FWUPGRADE_H_

/* end of file */

