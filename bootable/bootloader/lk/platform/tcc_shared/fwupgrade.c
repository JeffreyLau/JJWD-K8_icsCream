/****************************************************************************
 *   FileName    : Fwupgrade.c
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#if defined(_LINUX_) || defined(_WINCE_)
#include <common.h>
#include "IO_TCCXXX.h"
#include "TC_File.h"
#include "browse.h"
//#include "fwdn_protocol.h"
#include "fwupgrade.h"
//#include "usb.h"
#include "FSAPP.h"
#include "nand_drv.h"
#endif

#if defined (NKUSE)
#include "windows.h"
#include "stdlib.h"
#elif defined (__KERNEL__)
#include <linux/string.h>
#endif

//unsigned int	gMAX_ROMSIZE;	//twkwon: Initialize????

#if 1
static const unsigned int CRC32_TABLE[256] = {
	0x00000000,	0x90910101,	0x91210201,	0x01B00300,
	0x92410401,	0x02D00500,	0x03600600,	0x93F10701,
	0x94810801,	0x04100900,	0x05A00A00,	0x95310B01,
	0x06C00C00,	0x96510D01,	0x97E10E01,	0x07700F00,
	0x99011001,	0x09901100,	0x08201200,	0x98B11301,
	0x0B401400,	0x9BD11501,	0x9A611601,	0x0AF01700,
	0x0D801800,	0x9D111901,	0x9CA11A01,	0x0C301B00,
	0x9FC11C01,	0x0F501D00,	0x0EE01E00,	0x9E711F01,
	0x82012001,	0x12902100,	0x13202200,	0x83B12301,
	0x10402400,	0x80D12501,	0x81612601,	0x11F02700,
	0x16802800,	0x86112901,	0x87A12A01,	0x17302B00,
	0x84C12C01,	0x14502D00,	0x15E02E00,	0x85712F01,
	0x1B003000,	0x8B913101,	0x8A213201,	0x1AB03300,
	0x89413401,	0x19D03500,	0x18603600,	0x88F13701,
	0x8F813801,	0x1F103900,	0x1EA03A00,	0x8E313B01,
	0x1DC03C00,	0x8D513D01,	0x8CE13E01,	0x1C703F00,
	0xB4014001,	0x24904100,	0x25204200,	0xB5B14301,
	0x26404400,	0xB6D14501,	0xB7614601,	0x27F04700,
	0x20804800,	0xB0114901,	0xB1A14A01,	0x21304B00,
	0xB2C14C01,	0x22504D00,	0x23E04E00,	0xB3714F01,
	0x2D005000,	0xBD915101,	0xBC215201,	0x2CB05300,
	0xBF415401,	0x2FD05500,	0x2E605600,	0xBEF15701,
	0xB9815801,	0x29105900,	0x28A05A00,	0xB8315B01,
	0x2BC05C00,	0xBB515D01,	0xBAE15E01,	0x2A705F00,
	0x36006000,	0xA6916101,	0xA7216201,	0x37B06300,
	0xA4416401,	0x34D06500,	0x35606600,	0xA5F16701,
	0xA2816801,	0x32106900,	0x33A06A00,	0xA3316B01,
	0x30C06C00,	0xA0516D01,	0xA1E16E01,	0x31706F00,
	0xAF017001,	0x3F907100,	0x3E207200,	0xAEB17301,
	0x3D407400,	0xADD17501,	0xAC617601,	0x3CF07700,
	0x3B807800,	0xAB117901,	0xAAA17A01,	0x3A307B00,
	0xA9C17C01,	0x39507D00,	0x38E07E00,	0xA8717F01,
	0xD8018001,	0x48908100,	0x49208200,	0xD9B18301,
	0x4A408400,	0xDAD18501,	0xDB618601,	0x4BF08700,
	0x4C808800,	0xDC118901,	0xDDA18A01,	0x4D308B00,
	0xDEC18C01,	0x4E508D00,	0x4FE08E00,	0xDF718F01,
	0x41009000,	0xD1919101,	0xD0219201,	0x40B09300,
	0xD3419401,	0x43D09500,	0x42609600,	0xD2F19701,
	0xD5819801,	0x45109900,	0x44A09A00,	0xD4319B01,
	0x47C09C00,	0xD7519D01,	0xD6E19E01,	0x46709F00,
	0x5A00A000,	0xCA91A101,	0xCB21A201,	0x5BB0A300,
	0xC841A401,	0x58D0A500,	0x5960A600,	0xC9F1A701,
	0xCE81A801,	0x5E10A900,	0x5FA0AA00,	0xCF31AB01,
	0x5CC0AC00,	0xCC51AD01,	0xCDE1AE01,	0x5D70AF00,
	0xC301B001,	0x5390B100,	0x5220B200,	0xC2B1B301,
	0x5140B400,	0xC1D1B501,	0xC061B601,	0x50F0B700,
	0x5780B800,	0xC711B901,	0xC6A1BA01,	0x5630BB00,
	0xC5C1BC01,	0x5550BD00,	0x54E0BE00,	0xC471BF01,
	0x6C00C000,	0xFC91C101,	0xFD21C201,	0x6DB0C300,
	0xFE41C401,	0x6ED0C500,	0x6F60C600,	0xFFF1C701,
	0xF881C801,	0x6810C900,	0x69A0CA00,	0xF931CB01,
	0x6AC0CC00,	0xFA51CD01,	0xFBE1CE01,	0x6B70CF00,
	0xF501D001,	0x6590D100,	0x6420D200,	0xF4B1D301,
	0x6740D400,	0xF7D1D501,	0xF661D601,	0x66F0D700,
	0x6180D800,	0xF111D901,	0xF0A1DA01,	0x6030DB00,
	0xF3C1DC01,	0x6350DD00,	0x62E0DE00,	0xF271DF01,
	0xEE01E001,	0x7E90E100,	0x7F20E200,	0xEFB1E301,
	0x7C40E400,	0xECD1E501,	0xED61E601,	0x7DF0E700,
	0x7A80E800,	0xEA11E901,	0xEBA1EA01,	0x7B30EB00,
	0xE8C1EC01,	0x7850ED00,	0x79E0EE00,	0xE971EF01,
	0x7700F000,	0xE791F101,	0xE621F201,	0x76B0F300,
	0xE541F401,	0x75D0F500,	0x7460F600,	0xE4F1F701,
	0xE381F801,	0x7310F900,	0x72A0FA00,	0xE231FB01,
	0x71C0FC00,	0xE151FD01,	0xE0E1FE01,	0x7070FF00
};
#else
extern const unsigned CRC32_TABLE[];
#endif

typedef void 	ROM_Upgrade(unsigned char *, unsigned int, unsigned int, unsigned char *); 
ROM_Upgrade   	*pROMUpgradeFunc;

extern int TCDB_SaveDBHeader(unsigned char ucAssociation, int iPartID);    
extern void ResetSystem(void);


#define		MAXBUFFSIZE 16*1024 // 

//int		gSDUpgrading = 1;

unsigned char gBuffer[MAXBUFFSIZE];

#if defined(_WINCE_) || defined(_LINUX_)
///////////////////////////////////////////////////////////////////////////////////
//
// Function    : FWUG_GetTempBuffer()
//
// Description : Get temporary buffer to store data
// 
///////////////////////////////////////////////////////////////////////////////////
char *FWUG_GetTempBuffer(unsigned int *uiBufSize)
{
#if defined (NKUSE) || defined(__KERNEL__)
	*uiBufSize = MAXBUFFSIZE;
	return gBuffer;
#else
	//if (gSDUpgrading == 1)
	{
		*uiBufSize = MAXBUFFSIZE;
		return gBuffer;
	}
	//else
	//{
	//	*uiBufSize	= FSAPP_GetMaxCopySize();
	//	return FSAPP_GetFileBuffer();
	//}
#endif
}

///////////////////////////////////////////////////////////////////////////////////
//
// Function    : FWUG_CheckBattery
//
// Description : Check Battery
// 
///////////////////////////////////////////////////////////////////////////////////
int FWUG_CheckBattery(void)
{
#ifdef BATTERY_INCLUDE
	unsigned int batteryValue = 0;
	unsigned int Emergencycount = 0;
	int i;
	
	for(i=0; i<20; i++)
	{
		batteryValue = BATTERY_GetBatteryVoltage();		
		if(batteryValue < 110)
			Emergencycount++;			
		
		if(Emergencycount > 10)
		{
			SerialWriteString("BAT too low!!");
			CAPP_ReturnSetClock();	// 20041207
			return 0;
		}
		
		TC_TimeDly(10);
	}
#endif
	return 1;
}

#if !defined(__KERNEL__)
/**************************************************************************
*  FUNCTION NAME : 
*      unsigned int		FWUG_CheckROMFileCRC
*  
*  DESCRIPTION : 
*  INPUT:
*  
*  OUTPUT:
*
*  REMARK  :	
**************************************************************************/
unsigned int	FWUG_CheckROMFileCRC(int iFilehandle, unsigned char *buf, unsigned int uiBufSize)
{
	unsigned int	i;
	unsigned int	uiROMFileSize;
	unsigned int	uiVerifyCRC;
	unsigned int	uiTempCRC;
	unsigned int	uiCRCSize;
	unsigned int	uiCnt;
	unsigned int	uiMode;

	uiROMFileSize =  0x00;
	uiVerifyCRC = 0x00;
	uiTempCRC = 0x00;	

	memset( buf, 0x00, uiBufSize );
	TC_Read( iFilehandle, buf, 32 );	//header size		

	uiTempCRC |= ( buf[27] & 0x000000FF) << 24;
	uiTempCRC |= ( buf[26] & 0x000000FF) << 16;
	uiTempCRC |= ( buf[25] & 0x000000FF) << 8;
	uiTempCRC |= ( buf[24] & 0x000000FF) ;
	
	uiROMFileSize |= ( buf[31] & 0x000000FF) << 24;
	uiROMFileSize |= ( buf[30] & 0x000000FF) << 16;
	uiROMFileSize |= ( buf[29] & 0x000000FF) << 8;
	uiROMFileSize |= ( buf[28] & 0x000000FF) ;		

	if( (!uiTempCRC) || (uiROMFileSize < (128 * 1024)))
		return -1;

	TC_Seek( iFilehandle, 0, TC_SEEK_SET );
	
	if (uiBufSize < uiROMFileSize )
	{
		uiCnt = ( uiROMFileSize + ( uiBufSize - 1)) / uiBufSize;
		for ( i = 0; i < uiCnt ; i++ )
		{
			if ( i == (uiCnt -1 ))
				uiCRCSize = ( uiROMFileSize  - uiBufSize * (uiCnt - 1));
			else
				uiCRCSize = uiBufSize;			
			
			TC_Read( iFilehandle, buf, uiCRCSize );
			
			if ( i == 0 )
				uiMode = 1;
			else
				uiMode = 2;
			
			uiVerifyCRC = CalCRC_ROMFile(buf, uiCRCSize, uiVerifyCRC, uiMode);				
			
		}
	}
	else
	{
		TC_Read( iFilehandle, buf, uiROMFileSize );

		uiMode = 1;
		uiVerifyCRC = CalCRC_ROMFile( buf, uiROMFileSize, uiVerifyCRC, uiMode);
	}

	TC_Seek( iFilehandle, 0, TC_SEEK_SET );

	if ( uiTempCRC != uiVerifyCRC ) 
		return 1;
	else
		return SUCCESS;	
}
#endif

///////////////////////////////////////////////////////////////////////////////////
//
// Function    : FWUG_MainFunc
//
// Description : Main function to upgrade Firmware
// 
///////////////////////////////////////////////////////////////////////////////////
#if !defined(__KERNEL__) && !defined(ANDROID_BOOT) && !defined(ANDROID_KERNEL)
int FWUG_MainFunc(int hFile, int iFileSize)
#else
/* if Linux Kernel */
int FWUG_MainFunc(char *rom_buf, unsigned int uiROMFileSize)
#endif
{
	unsigned int	iRev;	
	unsigned int   	uiBufSize;
	unsigned char 	*buf;	

	unsigned int	i;
	unsigned int	uiRomBaseAddr, uiMenInitStart, uiMenInitEnd;
	unsigned int	uRemainSize, uReadSize;
	unsigned int 	dwBlockOffSet, dwPageOffSet;
	unsigned int 	nStBlockOffSet, nStPageOffSet;
	unsigned char	GMC_Num;
	unsigned char	FlagofNewSizeBigger;
	unsigned int	nSecureMode = DISABLE;

	#ifdef TRIFLASH_BOOT_INCLUDE
	int					res;
	ioctl_diskinfo_t 	disk_info;
	ioctl_diskrwpage_t	bootCodeWrite;
	#endif
	
#if !defined(__KERNEL__) && !defined(ANDROID_BOOT) && !defined(ANDROID_KERNEL)		// Case WINCE 
	unsigned int	uiROMFileSize;
	unsigned int iFilehandle;
	#if defined(NKUSE)
	iFilehandle = TC_Open((char*)hFile, NULL, NULL, NULL);
	#else
    iFilehandle = hFile;
	#endif
#else
	unsigned int	rom_offset;	// Case LINUX 
#endif

	//===============================================================
	//	Initialize Variable 
	//===============================================================
	uiRomBaseAddr = 0;
	uiMenInitStart = 0;
	uiMenInitEnd = 0;

	//===============================================================
	//	Check condition for FW Upgrade
	//===============================================================
	if((buf = FWUG_GetTempBuffer(&uiBufSize))==0)
	{
		return ERR_FWUG_NOT_EXISTMEMORY;
	}

#if !defined(__KERNEL__) && !defined(ANDROID_BOOT) && !defined(ANDROID_KERNEL)	// Case WINCE 
/* if Linux Kernel, already crc checked in tccbox */
	iRev = FWUG_CheckROMFileCRC( iFilehandle, buf, uiBufSize );
	if ( iRev != SUCCESS )
	{
		return -1;
	}		
#endif

	if(!FWUG_CheckBattery())
	{
		return ERR_FWUG_FAIL_BATCHECK;
	}

#ifdef NAND_BOOT_INCLUDE
#ifdef TNFTL_V7_INCLUDE
	//===============================================================
	//	UPGRADE - NANDFLASH
	//===============================================================
	/* Setting Intial Parameters */
#if !defined(__KERNEL__) && !defined(ANDROID_BOOT) && !defined(ANDROID_KERNEL)		// Case WINCE 
	uiROMFileSize = TC_Length(iFilehandle);
#endif 

	/********************************************/
	/* Get Kernel Base Address to TcBoot 		*/
	/********************************************/
#if !defined(__KERNEL__) && !defined(ANDROID_BOOT) && !defined(ANDROID_KERNEL)		// Case WINCE 
	TC_Seek( iFilehandle, ROM_FIRMWARE_BASE_OFFSET, TC_SEEK_SET );
	TC_Read( iFilehandle, buf, 4 );
#else							// Case LINUX 
	rom_offset = ROM_FIRMWARE_BASE_OFFSET;
	memcpy(buf, rom_buf + rom_offset, 4 );
#endif 
	uiRomBaseAddr |= ( buf[3] & 0x000000FF) << 24;
	uiRomBaseAddr |= ( buf[2] & 0x000000FF) << 16;
	uiRomBaseAddr |= ( buf[1] & 0x000000FF) << 8;
	uiRomBaseAddr |= ( buf[0] & 0x000000FF);	

	/********************************************/
	/* Get MemInitCode Start Address  to TcBoot */
	/********************************************/
#if !defined(__KERNEL__) && !defined(ANDROID_BOOT) && !defined(ANDROID_KERNEL)		// Case WINCE 
	TC_Seek( iFilehandle, ROM_CONFIG_CODE_START_OFFSET, TC_SEEK_SET );
	TC_Read( iFilehandle, buf, 4 );
#else							// Case LINUX 
	rom_offset = ROM_CONFIG_CODE_START_OFFSET;
	memcpy(buf, rom_buf + rom_offset, 4 );
#endif 
	uiMenInitStart |= ( buf[3] & 0x000000FF) << 24;
	uiMenInitStart |= ( buf[2] & 0x000000FF) << 16;
	uiMenInitStart |= ( buf[1] & 0x000000FF) << 8;
	uiMenInitStart |= ( buf[0] & 0x000000FF);	

	/********************************************/
	/* Get MemInitCode Start Address  to TcBoot */
	/********************************************/
#if !defined(__KERNEL__) && !defined(ANDROID_BOOT) && !defined(ANDROID_KERNEL)		// Case WINCE 
	TC_Seek( iFilehandle, ROM_CONFIG_CODE_END_OFFSET, TC_SEEK_SET );
	TC_Read( iFilehandle, buf, 4 );
#else							// Case LINUX 
	rom_offset = ROM_CONFIG_CODE_END_OFFSET;
	memcpy(buf, rom_buf + rom_offset, 4 );
#endif 
	uiMenInitEnd |= ( buf[3] & 0x000000FF) << 24;
	uiMenInitEnd |= ( buf[2] & 0x000000FF) << 16;
	uiMenInitEnd |= ( buf[1] & 0x000000FF) << 8;
	uiMenInitEnd |= ( buf[0] & 0x000000FF);

#if !defined(__KERNEL__) && !defined(ANDROID_BOOT) && !defined(ANDROID_KERNEL)		// Case WINCE 
	TC_Seek( iFilehandle, (uiMenInitStart - uiRomBaseAddr), TC_SEEK_SET );
	TC_Read( iFilehandle, buf, ( uiMenInitEnd - uiMenInitStart ) );
#else
	rom_offset = (uiMenInitStart - uiRomBaseAddr);
	memcpy(buf, rom_buf + rom_offset, ( uiMenInitEnd - uiMenInitStart ) );
#endif 
	FWUG_NAND_PreMemInitCode( buf, ( uiMenInitEnd - uiMenInitStart ) );
#endif
#endif

	#ifdef NAND_BOOT_INCLUDE
	#ifdef TNFTL_V7_INCLUDE
	FWUG_NAND_SetEnableNandBootOnlyMode( ENABLE );
	FWUG_NAND_SetNBAreaEndPBAdd(TNFTL_MAX_BLOCK_NUM_OF_NBAREA);		// Physical Block Num
	#endif

	#if defined(FWUG_V2_INCLUDE)
	nSecureMode		= FWUG_NAND_GetFlagOfUseSecureMode();
	#endif

	/* PreProcess before write code */
	if (( iRev = FWUG_NAND_PreProcess( uiROMFileSize, &FlagofNewSizeBigger )) != SUCCESS )
	{
		goto FWUG_FAIL;
	}
	#elif defined(TRIFLASH_BOOT_INCLUDE)
	// TRIFLASH...WHAT TO DO?
	if((res=DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_INITIALIZE, NULL)) < 0)
		return res;

	if((res=DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_GET_DISKINFO, (void *)&disk_info)) < 0)
		return res;

	if(disk_info.sector_size != 512)
		return (-1);
	#endif

	/* Write CODE Data */
	#ifdef NAND_BOOT_INCLUDE
	for ( i = 0 ; i < 2; ++i )
	#endif
	{
		uRemainSize = uiROMFileSize;
#if !defined(__KERNEL__) && !defined(ANDROID_BOOT) && !defined(ANDROID_KERNEL)
		TC_Seek( iFilehandle, 0, TC_SEEK_SET );
#else
		rom_offset = 0;
#endif

		if ( FlagofNewSizeBigger == TRUE )
			GMC_Num = ( i == 0 ) ? FIRST : SECOND;
		else
			GMC_Num = ( i == 0 ) ? SECOND : FIRST;
		#ifdef NAND_BOOT_INCLUDE
		#if defined(FWUG_V1_INCLUDE)
		if (( iRev = FWUG_NAND_WriteCodePreProcess( GMC_Num, uiROMFileSize, &nStBlockOffSet, &nStPageOffSet )) != SUCCESS )
		#elif defined(FWUG_V2_INCLUDE)
		if (( iRev = FWUG_NAND_WriteCodePreProcess( GMC_Num, uiROMFileSize, &nStBlockOffSet, &nStPageOffSet, nSecureMode )) != SUCCESS )
		#endif
		{
			goto FWUG_FAIL;
		}
		#elif defined(TRIFLASH_BOOT_INCLUDE)
		// TRIFLASH...INITIALIZE?
		bootCodeWrite.buff			= buf;
		bootCodeWrite.start_page	= SDMMC_GetROMAddr();
		#endif

		while ((int)uRemainSize > 0)
		{
			uReadSize = ( uRemainSize >= uiBufSize ) ? uiBufSize : uRemainSize;
#if !defined(__KERNEL__) && !defined(ANDROID_BOOT) && !defined(ANDROID_KERNEL)
			uReadSize = TC_Read( iFilehandle, buf, uReadSize );
#else
			memcpy(buf, rom_buf + rom_offset, uReadSize);
			rom_offset += uReadSize;
#endif
			
			#ifdef NAND_BOOT_INCLUDE
#ifdef TNFTL_V7_INCLUDE
			if (( iRev = FWUG_NAND_WriteCodeNAND( GMC_Num, nStBlockOffSet, nStPageOffSet, buf, uReadSize, &dwBlockOffSet, &dwPageOffSet, nSecureMode )) != SUCCESS )
#else
			if (( iRev = FWUG_NAND_WriteCodeNAND( nStBlockOffSet, nStPageOffSet, buf, uReadSize, &dwBlockOffSet, &dwPageOffSet, nSecureMode )) != SUCCESS )
#endif
			{
				goto FWUG_FAIL;
			}

			nStBlockOffSet = dwBlockOffSet;
			nStPageOffSet  = dwPageOffSet;
			#elif defined(TRIFLASH_BOOT_INCLUDE)
			// TRIFLASH...WRITE?
			{
				bootCodeWrite.rw_size	= (uReadSize+511) / 512;
				res = DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_BOOTCODE_WRITE_PAGE, (void *) & bootCodeWrite);
				if ( res < 0)		// there is error on writing F/W
				{
					//LPRINTF("Fail to Write Image\n");
					return ERR_FWUG_FAIL_CODEWRITETFLASH;
				}	
			}
			bootCodeWrite.start_page = bootCodeWrite.start_page + bootCodeWrite.rw_size;
			#endif
			
			uRemainSize	-= uReadSize;
		}

		#ifdef NAND_BOOT_INCLUDE
		if (( iRev = FWUG_NAND_WriteCodePostProcess( GMC_Num, nStBlockOffSet, nStPageOffSet )) != SUCCESS )
		{
			goto FWUG_FAIL;
		}
		#elif defined(TRIFLASH_BOOT_INCLUDE)
		// TRIFLASH... WHAT TO DO?
		#endif
	}

	#ifdef NAND_BOOT_INCLUDE
	/* PostProcess after write code */
	if (( iRev = FWUG_NAND_PostProcess((void*)0, TRUE, nSecureMode )) != SUCCESS )
	{
		goto FWUG_FAIL;
	}
	#elif defined(TRIFLASH_BOOT_INCLUDE)
	// TRIFLASH... WHAT TO DO?
	#endif

	//===============================================================
	//	CLOSE
	//===============================================================
#if !defined(__KERNEL__) && !defined(ANDROID_BOOT) && !defined(ANDROID_KERNEL)
#if defined(NKUSE)
	TC_Close(iFilehandle);
#endif
#endif

	if (TC_ISERR(iRev))
	{
		return -1;
	}

	return SUCCESS;

FWUG_FAIL:
	return iRev;
}

#endif

/**************************************************************************
*  FUNCTION NAME : 
*      unsigned int FWUG_CalcCrc(unsigned int *base, unsigned int length, unsigned int *crctable);
*  
*  DESCRIPTION : 
*  INPUT:
*			base	= 
*			crctable	= 
*			length	= 
*  
*  OUTPUT:	int - Return Type
*  			= 
*  REMARK  :	
**************************************************************************/
unsigned int FWUG_CalcCrc(unsigned int *base, unsigned int length)
{
	unsigned int crcout = 0;
	unsigned int cnt, i, code, tmp;
	
	for(cnt=0; cnt<length; cnt++)
	{
		code = base[cnt];
		for(i=0; i<4; i++)
		{
			tmp = code^crcout;
			crcout = (crcout>>8)^CRC32_TABLE[tmp&0xFF];
			code = code >> 8;
		}
	}
	return crcout;
}

/**************************************************************************
*  FUNCTION NAME : 
*      unsigned int FWUG_CalcCrc8(unsigned char *base, unsigned int length, unsigned int *crctable);
*  
*  DESCRIPTION : 
*  INPUT:
*			base	= 
*			crctable	= 
*			length	= 
*  
*  OUTPUT:	int - Return Type
*  			= 
*  REMARK  :	
**************************************************************************/
unsigned int FWUG_CalcCrc8(unsigned char *base, unsigned int length)
{
	unsigned int crcout = 0;
	unsigned int cnt; 
	unsigned char	code, tmp;
	
	for(cnt=0; cnt<length; cnt++)
	{
		code = base[cnt];
		tmp = code^crcout;
		crcout = (crcout>>8)^CRC32_TABLE[tmp&0xFF];
	}
	return crcout;
}

/**************************************************************************
*  FUNCTION NAME : 
*      unsigned int FWUG_CalcCrcI(unsigned uCRCIN, unsigned *base, unsigned int length, unsigned int *crctable);
*  
*  DESCRIPTION : 
*  INPUT:
*			base	= 
*			crctable	= 
*			length	= 
*  
*  OUTPUT:	int - Return Type
*  			= 
*  REMARK  :	
**************************************************************************/
unsigned int FWUG_CalcCrcI(unsigned uCRCIN, unsigned *base, unsigned int length)
{
	unsigned int crcout = uCRCIN;
	unsigned int cnt, i, code, tmp;

	for(cnt=0; cnt<length; cnt++)
	{
		code = base[cnt];
		for(i=0; i<4; i++)
		{
			tmp = code^crcout;
			crcout = (crcout>>8)^CRC32_TABLE[tmp&0xFF];
			code = code >> 8;
		}
	}
	return crcout;
}

/**************************************************************************
*  FUNCTION NAME : 
*      unsigned int FWUG_CalcCrc8I(unsigned uCRCIN, unsigned char *base, unsigned int length, unsigned int *crctable);
*  
*  DESCRIPTION : 
*  INPUT:
*			base	= 
*			crctable	= 
*			length	= 
*  
*  OUTPUT:	int - Return Type
*  			= 
*  REMARK  :	
**************************************************************************/
unsigned int FWUG_CalcCrc8I(unsigned uCRCIN, unsigned char *base, unsigned int length)
{
	unsigned int crcout = uCRCIN;
	unsigned int cnt; 
	unsigned char	code, tmp;

	for(cnt=0; cnt<length; cnt++)
	{
		code = base[cnt];
		tmp = code^crcout;
		crcout = (crcout>>8)^CRC32_TABLE[tmp&0xFF];
	}
	return crcout;
}

/**************************************************************************
*  FUNCTION NAME : 
*      unsigned int CalCRC_ROMFile(unsigned int *pBuffer,unsigned int size,unsigned int crcout, unsigned int mode);
*  
*  DESCRIPTION : 
*  INPUT:
*			crcout	= 
*			mode	= 
*			pBuffer	= 
*			size	= 
*  
*  OUTPUT:	int - Return Type
*  			= 
*  REMARK  :	
**************************************************************************/
unsigned int CalCRC_ROMFile(unsigned int *pBuffer,unsigned int size,unsigned int crcout, unsigned int mode)
{

	unsigned int cnt, i, code, tmp;
	unsigned int CrcRegion;


	CrcRegion = (size>>2);

	for(cnt=0; cnt<CrcRegion; cnt++)
	{
		code = pBuffer[cnt];
		if(mode==0 || mode==1)
		{
			if(cnt == 4 || cnt == 5)
			{
				continue;
			}
		}
		if(mode==1)
		{
			if(cnt==6)
			{
				code = 0x00000000;
			}
		}
		
	
		for(i=0; i<4; i++)
		{
			tmp = code^crcout;
			crcout = (crcout>>8)^CRC32_TABLE[tmp&0xFF];
			code = code >> 8;
		}
	}
	
	return crcout;
} 

/************* end of file *************************************************************/
