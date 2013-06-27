/***************************************************************************************
*	FileName    : fwdn_protocol_v7.c
*	Description : FWDN Protocol Version 7. Functions
****************************************************************************************
*
*	TCC Board Support Package
*	Copyright (c) Telechips, Inc.
*	ALL RIGHTS RESERVED
*
****************************************************************************************/
#if defined(PLATFORM_TCC92XX) || defined(PLATFORM_TCC93XX) || defined(PLATFORM_TCC88XX) || defined(PLATFORM_TCC892X)
#if defined(FWDN_V7)

#if defined(_LINUX_)
#include <usb/usb_defs.h>
#include <fwdn/fwdn_protocol_v7.h>
#include <fwdn/fwdn_drv_v7.h>
#include <usb/vtc.h>
#elif defined(_WINCE_)
#include "..\USB\usb_defs.h"
#include "fwdn_protocol_v7.h"
#include "fwdn_drv_v7.h"
#include "..\USB\vtc.h"

#include "memory.h"
#else
#include "IO_TCCXXX.h"
#include "usb_defs.h"
#include "fwdn_protocol_v7.h"
#include "fwdn_drv_v7.h"
#include "vtc.h"
#endif

#define	FWDN_PROT_VERSION			0x07000003


//==============================================================
//
//		Global Variables
//
//==============================================================
#define	USB_MAX_TRANSFER_BUFFER_SIZE	0x8000					// actually, 0xFFFF is maximum but not good for memory alignment

//==============================================================
//
//		External Variables
//
//==============================================================
extern unsigned int FWUG_VERSION;
extern const unsigned char TNFTL_Library_Version[];

//=====================================================================
//
//		Extern Functions
//
//=====================================================================
//unsigned USB_JumpUSB(void);
//unsigned char* FWUG_V4_TellLibraryVersion( void );
#if !defined(_WINCE_)
extern void memset(void *pvDest, char cChar, unsigned long iCount);
#endif

//==============================================================
//
//		External Variables
//
//==============================================================
static unsigned short gFwdn_CmdType;
static FWDN_COMM_T gFwdn_CommType;



static void FWDN_PROT_Response_Reset(FWDN_RESPONSE_T *pFwdnRsp)
{
	memset(pFwdnRsp,0,sizeof(FWDN_RESPONSE_T));
	pFwdnRsp->Signature = FWDN_RESPONSE_SIGNATURE;
	pFwdnRsp->CmdType = gFwdn_CmdType;
}

void FWDN_PROT_SetComm(FWDN_COMM_T commType)
{
	gFwdn_CommType = commType;
}

void FWDN_PROT_Response(FWDN_RESPONSE_T *pFwdnRsp)
{
#if defined(FEATURE_FWDN_COMM_VTC)
	if(gFwdn_CommType==FWDN_COMM_VTC)
	{
		VTC_SendData((void*)pFwdnRsp, sizeof(FWDN_RESPONSE_T));
		return;
	}
#endif
}

unsigned int FWDN_PROT_Data_Send(void *pBuf, unsigned int length)
{
#if defined(FEATURE_FWDN_COMM_VTC)
	if(gFwdn_CommType==FWDN_COMM_VTC)
		return VTC_SendData(pBuf,length);
#endif

	return 0;
}

unsigned int FWDN_PROT_Data_Receive_GetPointer(void **ppBuf)
{
#if defined(FEATURE_FWDN_COMM_VTC)
	if(gFwdn_CommType==FWDN_COMM_VTC)
		return VTC_ReceiveData_GetPointer(ppBuf);
#endif

	return 0;
}

unsigned int FWDN_PROT_Data_Receive(void *pBuf, unsigned int length)
{
#if defined(FEATURE_FWDN_COMM_VTC)
	if(gFwdn_CommType==FWDN_COMM_VTC)
		return VTC_ReceiveData(pBuf,length);
#endif

	return 0;
}

unsigned int FWDN_PROT_CheckIsReceivedData(void)
{
#if defined(FEATURE_FWDN_COMM_VTC)
	if(gFwdn_CommType==FWDN_COMM_VTC)
		return VTC_CheckIsReceivedData();
#endif

	return 0;
}

unsigned int FWDN_PROT_CheckIsTransmitReady(void)
{
#if defined(FEATURE_FWDN_COMM_VTC)
	if(gFwdn_CommType==FWDN_COMM_VTC)
		return VTC_CheckIsTransmitReady();
#endif

	return 0;
}

static void FWDN_PROT_Data_ReceiveAway(unsigned int length)
{
	unsigned int dummy[512/4];
	while(length)
	{
		unsigned int packet;
		if(length>512)
			packet = 512;
		else
			packet = length;
		FWDN_PROT_Data_Receive(dummy,packet);
		length -= packet;
	}
}

static void FWDN_PROT_SendMessages(void)
{
	FWDN_RESPONSE_T fwdnRsp;

	unsigned int* pMsg;
	unsigned int uiMsgSize;

	while (FWDN_DRV_GetMessage(&pMsg,&uiMsgSize) == TRUE)
	{
		FWDN_PROT_Response_Reset(&fwdnRsp);
		fwdnRsp.AckType = FWDN_RSP_MSG;
		fwdnRsp.DataSize = uiMsgSize;

		FWDN_PROT_Response(&fwdnRsp);
		if(fwdnRsp.DataSize != 0)
			FWDN_PROT_Data_Send((void*)pMsg, fwdnRsp.DataSize);
	}
}

void FWDN_PROT_ResponseAck(void)
{
	FWDN_RESPONSE_T fwdnRsp;

	FWDN_PROT_SendMessages();

	FWDN_PROT_Response_Reset(&fwdnRsp);
	fwdnRsp.AckType = FWDN_RSP_ACK;

	FWDN_PROT_Response(&fwdnRsp);
}

void FWDN_PROT_ResponseNack(void)
{
	FWDN_RESPONSE_T fwdnRsp;

	FWDN_PROT_SendMessages();

	FWDN_PROT_Response_Reset(&fwdnRsp);
	fwdnRsp.AckType = FWDN_RSP_NACK;
	fwdnRsp.Param0 = FWDN_DRV_GetErrorCode();

	FWDN_PROT_Response(&fwdnRsp);
}

static void _loc_Progress_CallBack( unsigned int percent )
{
	if(FWDN_PROT_CheckIsTransmitReady() == TRUE)
	{
		FWDN_RESPONSE_T fwdnRsp;

		FWDN_PROT_SendMessages();

		FWDN_PROT_Response_Reset(&fwdnRsp);
		fwdnRsp.AckType = FWDN_RSP_NYET;
		fwdnRsp.Param0 = (unsigned long)percent;

		FWDN_PROT_Response(&fwdnRsp);
	}
}

static void FWDN_PROT_UBS_Ping(void)
{
	FWDN_PROT_ResponseAck();
}

//extern unsigned int gFWDN_RomBuffer[];
//static void FWDN_PROT_UBS_InitCodeLoad(FWDN_COMMAND_T *pFwdnCmd)
//{
//	while(pFwdnCmd->ExtraCmdSize)
//	{
//		unsigned int packetSize;
//		packetSize = (pFwdnCmd->ExtraCmdSize>USB_MAX_TRANSFER_BUFFER_SIZE)? USB_MAX_TRANSFER_BUFFER_SIZE : pFwdnCmd->ExtraCmdSize;
//		pFwdnCmd->ExtraCmdSize -= packetSize;
//		FWDN_PROT_Data_Receive((void*)gFWDN_RomBuffer, packetSize);
//	}
//	FWDN_PROT_ResponseAck();
//}

//extern unsigned int *FWDN_GetRomBuffer(void);
//extern void	memcpy(void *pvDest, const void *pvSrc, unsigned long iCount);
//static void FWDN_PROT_UBS_RomLoad(FWDN_COMMAND_T *pFwdnCmd)
//{
//	unsigned int size = (unsigned int)pFwdnCmd->ExtraCmdSize;
//	unsigned int *pFWDN_RomBuffer = gFWDN_RomBuffer;
//	unsigned int offsetSignature = (unsigned int)pFwdnCmd->Param1;
//	while(size)
//	{
//		unsigned int packetSize;
//		packetSize = (size>USB_MAX_TRANSFER_BUFFER_SIZE)? USB_MAX_TRANSFER_BUFFER_SIZE : size;
//		size -= packetSize;
//		FWDN_PROT_Data_Receive((void*)pFWDN_RomBuffer, packetSize);
//		pFWDN_RomBuffer = (unsigned int*)( ((unsigned int)pFWDN_RomBuffer) + packetSize );
//	}
//	{
//		unsigned char *pcRomBuffer = (unsigned char*)gFWDN_RomBuffer;
//		pcRomBuffer[offsetSignature] = '?';
//	}
//	FWDN_PROT_ResponseAck();
//	while( FWDN_PROT_CheckIsTransmitReady() != TRUE );	// wait until that fwdn response is transfered to host.
//
//	IO_ARM_DisableIRQ();
//	FWDN_RomLoader((unsigned int*)pFwdnCmd->Param0, gFWDN_RomBuffer, (unsigned int)pFwdnCmd->ExtraCmdSize, (ENTRY_POINT_T)pFwdnCmd->Param0);
//}

static void FWDN_PROT_DEVICE_Reset(void)
{
	FWDN_DRV_Reset();
	FWDN_PROT_ResponseNack();
}

//extern char FirmwareVersion[];
static void FWDN_PROT_DEVICE_Ping(FWDN_COMMAND_T *pFwdnCmd)
{
	FWDN_RESPONSE_T fwdnRsp;
	//unsigned int bRomExist = pFwdnCmd->Param0;

	//if(FirmwareVersion[3]==':' && bRomExist==TRUE )
	//{
	//	// It's not a new bootloader. Bootloader should be reloaded.
	//	return;
	//}

	FWDN_PROT_Response_Reset(&fwdnRsp);
	fwdnRsp.AckType = FWDN_RSP_ACK;
	fwdnRsp.Param0 = FWDN_PROT_VERSION;

	FWDN_PROT_Response(&fwdnRsp);
}

static void FWDN_PROT_DEVICE_Init(FWDN_COMMAND_T *pFwdnCmd)
{
	//unsigned int bmFlag = pFwdnCmd->Param0;
	//if(FWDN_DRV_Init(bmFlag,FWDN_PROT_CallBack_From_TNFTL)==TRUE)
	//	FWDN_PROT_ResponseAck();
	//else
	//	FWDN_PROT_ResponseNack();

	unsigned long message[512/4];
	unsigned int bmFlag = (unsigned int)pFwdnCmd->Param0;
	int result;

	result = FWDN_DRV_Init(bmFlag,_loc_Progress_CallBack,(char*)message,sizeof(message));

	if(result == 0)
	{
		FWDN_PROT_ResponseAck();
	}
	else if(result == 1)	// Ask to User
	{
		
		FWDN_RESPONSE_T fwdnRsp;
		FWDN_PROT_Response_Reset(&fwdnRsp);
		fwdnRsp.AckType = FWDN_RSP_ACK;
		fwdnRsp.Param0 = 1;
		fwdnRsp.DataSize = 512;
		FWDN_PROT_Response(&fwdnRsp);
		FWDN_PROT_Data_Send(message,512);
	}
	else
	{
		FWDN_PROT_ResponseNack();
	}
}

static void FWDN_PROT_DEVICE_InfoRead(FWDN_COMMAND_T *pFwdnCmd)
{
	FWDN_RESPONSE_T fwdnRsp;

	FWDN_PROT_Response_Reset(&fwdnRsp);
	fwdnRsp.AckType = FWDN_RSP_ACK;
	fwdnRsp.DataSize = sizeof(FWDN_DEVICE_INFORMATION);
	FWDN_PROT_Response(&fwdnRsp);

	FWDN_PROT_Data_Send((void *)FWDN_DRV_GetDeviceInfo(), fwdnRsp.DataSize);
}

static void FWDN_PROT_DEVICE_SessionStart(void)
{
	if(FWDN_DRV_SessionStart()==TRUE)
	{
		FWDN_PROT_ResponseAck();
	}
	else
	{
		FWDN_PROT_ResponseNack();
	}
}

static void FWDN_PROT_DEVICE_SessionEnd(FWDN_COMMAND_T *pFwdnCmd)
{
	unsigned int bSuccess = (unsigned int)pFwdnCmd->Param0;
	if(FWDN_DRV_SessionEnd(bSuccess) == TRUE)
	{
		FWDN_PROT_ResponseAck();
	}
	else
	{
		FWDN_PROT_ResponseNack();
	}
}

static void FWDN_PROT_DEVICE_SerialNumberWrite(FWDN_COMMAND_T *pFwdnCmd)
{
	int result = FALSE;
	
	if( pFwdnCmd->ExtraCmdSize == 32 )
	{
		unsigned long serial[32/4];
		unsigned int overwrite;

		FWDN_PROT_Data_Receive(serial, pFwdnCmd->ExtraCmdSize);
		overwrite = (unsigned int)pFwdnCmd->Param0;

		result = FWDN_DRV_SerialNumberWrite((unsigned char*)serial, overwrite);
	}

	if( result == TRUE )
		FWDN_PROT_ResponseAck();
	else
		FWDN_PROT_ResponseNack();
}

static int _loc_FirmwareWrite_ReadFromUSB(unsigned char *buff, unsigned int size, unsigned int srcAddr, unsigned int percent)
{
	FWDN_RESPONSE_T fwdnRsp;

	// If caller doesn't need data, it doesn't need to respond at everytime.
	if( size == 0 && FWDN_PROT_CheckIsTransmitReady() == FALSE )
		return 0;

	FWDN_PROT_Response_Reset(&fwdnRsp);
	fwdnRsp.AckType = FWDN_RSP_NYET;
	fwdnRsp.DataSize = (unsigned long)size;
	fwdnRsp.Param0 = (unsigned long)percent;
	fwdnRsp.Param1 = (unsigned long)srcAddr;
	FWDN_PROT_Response(&fwdnRsp);

	if( FWDN_PROT_Data_Receive((void *)buff, fwdnRsp.DataSize) == fwdnRsp.DataSize )
		return 0;
	else
		return -1;
}

static void FWDN_PROT_DEVICE_FirmwareWrite(FWDN_COMMAND_T *pFwdnCmd)
{
	int				err;
	unsigned int	fwSize;

	fwSize				= (unsigned int)pFwdnCmd->Param0;

	err = FWDN_DRV_FirmwareWrite(fwSize, &_loc_FirmwareWrite_ReadFromUSB);
	if( err == 0 )
		FWDN_PROT_ResponseAck();
	else
	{
		FWDN_RESPONSE_T fwdnRsp;

		FWDN_PROT_Response_Reset(&fwdnRsp);
		fwdnRsp.AckType = FWDN_RSP_NACK;
		fwdnRsp.Param0 = (unsigned long)err;
		fwdnRsp.Param1	= (unsigned long)FWDN_DRV_FirmwareStorageID();

		FWDN_PROT_Response(&fwdnRsp);
	}
}

static unsigned char fFWDN_PROT_MessageService = FALSE;
void FWDN_PROT_SendMessage(char *szMessage)
{
	if(fFWDN_PROT_MessageService)
	{
		VTC_SendMessage(szMessage, strlen(szMessage)+1);
	}
}

static void FWDN_PROT_DEVICE_MessageService(FWDN_COMMAND_T *pFwdnCmd)
{
	fFWDN_PROT_MessageService = (unsigned char)pFwdnCmd->Param0;
	FWDN_PROT_ResponseAck();
}

static int _loc_RquestData(unsigned char *buff, unsigned int size)
{
	FWDN_RESPONSE_T fwdnRsp;
	FWDN_PROT_Response_Reset(&fwdnRsp);
	fwdnRsp.AckType = FWDN_RSP_NYET;

	fwdnRsp.DataSize = size;
	FWDN_PROT_Response(&fwdnRsp);

	if( FWDN_PROT_Data_Receive((void *)buff, fwdnRsp.DataSize) != fwdnRsp.DataSize )
		return -1;

	return 0;
}

static void _loc_Response_NotifyData(unsigned int dataSize, unsigned int param0)
{
	FWDN_RESPONSE_T fwdnRsp;

	FWDN_PROT_Response_Reset(&fwdnRsp);
	fwdnRsp.AckType = FWDN_RSP_NYET;
	fwdnRsp.DataSize = dataSize;
	fwdnRsp.Param0 = param0;

	FWDN_PROT_Response(&fwdnRsp);
}

static void _loc_Response_RequestData(unsigned int dataSize)
{
	_loc_Response_NotifyData(dataSize,0);

#if defined(FEATURE_FWDN_COMM_VTC)
	if(gFwdn_CommType==FWDN_COMM_VTC)
	{
		VTC_ReceiveData_SetSize(dataSize);
	}
#endif
}

#if 0
static void FWDN_PROT_DISK_ClearGang(void)
{
	if(FWDN_DRV_NAND_GANG_Format()==TRUE)
		FWDN_PROT_ResponseAck();
	else
		FWDN_PROT_ResponseNack();
}

static void FWDN_PROT_DISK_WriteGang(FWDN_COMMAND_T *pFwdnCmd)
{
	NAND_PART_INFO		sNandPartInfo;

	sNandPartInfo.PartitionBlockMark	= pFwdnCmd->Param0;	
	sNandPartInfo.BlockUpperLimit 		= pFwdnCmd->Param1;
	sNandPartInfo.BlockLowerLimit 		= pFwdnCmd->Param2;
	sNandPartInfo.TotalBlockNum 		= pFwdnCmd->Param3;
	sNandPartInfo.PartitionCRC			= pFwdnCmd->Param4;

	if ( FWDN_DRV_NAND_GANG_Write( &sNandPartInfo, &_loc_RquestData ) == TRUE )
		FWDN_PROT_ResponseAck();
	else
		FWDN_PROT_ResponseNack();
}
#endif

static void FWDN_PROT_AREA_Write(FWDN_COMMAND_T *pFwdnCmd)
{
	unsigned int lba = (unsigned int)pFwdnCmd->Param0;
	unsigned int nSector = (unsigned int)pFwdnCmd->Param1;
	unsigned int nRemain = (unsigned int)pFwdnCmd->Param2;
	unsigned long _buf[20/4];
	char *name = (char *)_buf;
	if( pFwdnCmd->ExtraCmdSize != 16 )
	{
		FWDN_PROT_Data_ReceiveAway(pFwdnCmd->ExtraCmdSize);
		FWDN_DRV_SetErrorCode(ERR_FWDN_DRV_WRONG_PARAMETER);
		FWDN_PROT_ResponseNack();
		return;
	}

	FWDN_PROT_Data_Receive(name, 16);
	name[16]=0;

	if( FWDN_DRV_AREA_Write( name
							,lba
							,nSector
							,nRemain
							,&_loc_Response_RequestData
							,&FWDN_PROT_Data_Receive_GetPointer) == TRUE )
		FWDN_PROT_ResponseAck();
	else
		FWDN_PROT_ResponseNack();
}

static void FWDN_PROT_AREA_Read(FWDN_COMMAND_T *pFwdnCmd)
{
	unsigned int lba = (unsigned int)pFwdnCmd->Param0;
	unsigned int nSector = (unsigned int)pFwdnCmd->Param1;
	unsigned int nRemain = (unsigned int)pFwdnCmd->Param2;
	unsigned long _buf[20/4];
	char *name = (char *)_buf;
	if( pFwdnCmd->ExtraCmdSize != 16 )
	{
		FWDN_PROT_Data_ReceiveAway(pFwdnCmd->ExtraCmdSize);
		FWDN_DRV_SetErrorCode(ERR_FWDN_DRV_WRONG_PARAMETER);
		FWDN_PROT_ResponseNack();
		return;
	}

	FWDN_PROT_Data_Receive(name, 16);
	name[16]=0;

	if( FWDN_DRV_AREA_Read( name
							,lba
							,nSector
							,nRemain
							,&_loc_Response_NotifyData
							,&FWDN_PROT_Data_Send) == TRUE )
		FWDN_PROT_ResponseAck();
	else
		FWDN_PROT_ResponseNack();
}

static void _loc_SendStatus(unsigned int param0, unsigned int param1, unsigned int param2)
{
	FWDN_RESPONSE_T fwdnRsp;
	FWDN_PROT_Response_Reset(&fwdnRsp);
	fwdnRsp.AckType = FWDN_RSP_NYET;
	fwdnRsp.Param0 = param0;
	fwdnRsp.Param1 = param1;
	fwdnRsp.Param2 = param2;
	FWDN_PROT_Response(&fwdnRsp);
}

static void FWDN_PROT_AREA_CalcCRC(FWDN_COMMAND_T *pFwdnCmd)
{
	unsigned int lba = (unsigned int)pFwdnCmd->Param0;
	unsigned int nSector = (unsigned int)pFwdnCmd->Param1;
	unsigned int crc = (unsigned int)pFwdnCmd->Param2;
	unsigned int nRemain = (unsigned int)pFwdnCmd->Param3;
	unsigned long _buf[20/4];
	char *name = (char *)_buf;
	if( pFwdnCmd->ExtraCmdSize != 16 )
	{
		FWDN_PROT_Data_ReceiveAway(pFwdnCmd->ExtraCmdSize);
		FWDN_DRV_SetErrorCode(ERR_FWDN_DRV_WRONG_PARAMETER);
		FWDN_PROT_ResponseNack();
		return;
	}

	FWDN_PROT_Data_Receive(name, 16);
	name[16]=0;

	if( FWDN_DRV_AREA_CalcCRC( name
							,lba
							,nSector
							,nRemain
							,&crc
							,&_loc_SendStatus ) == TRUE )
	{
		FWDN_RESPONSE_T fwdnRsp;
		FWDN_PROT_Response_Reset(&fwdnRsp);
		fwdnRsp.AckType = FWDN_RSP_ACK;
		fwdnRsp.Param0 = crc;
		FWDN_PROT_Response(&fwdnRsp);
	}
	else
		FWDN_PROT_ResponseNack();
}

static void FWDN_PROT_DUMP_InfoRead(void)
{
	FWDN_RESPONSE_T fwdnRsp;
	unsigned long rsp[0x100/4];

	FWDN_PROT_Response_Reset(&fwdnRsp);
	fwdnRsp.AckType = FWDN_RSP_ACK;
	fwdnRsp.DataSize = FWDN_DRV_DUMP_InfoRead(rsp);

	FWDN_PROT_Response(&fwdnRsp);
	if( fwdnRsp.DataSize != 0 )
		FWDN_PROT_Data_Send(rsp, fwdnRsp.DataSize);
}

static void FWDN_PROT_DUMP_BlockRead(FWDN_COMMAND_T *pFwdnCmd)
{
	if( FWDN_DRV_DUMP_BlockRead(pFwdnCmd->Param0
									,pFwdnCmd->Param1
									,pFwdnCmd->Param2
									,&_loc_Response_NotifyData
									,&FWDN_PROT_Data_Send) == TRUE )
		FWDN_PROT_ResponseAck();
	else
		FWDN_PROT_ResponseNack();
}

static void FWDN_PROT_DUMP_BlockWrite(FWDN_COMMAND_T *pFwdnCmd)
{
	if( FWDN_DRV_DUMP_BlockWrite(pFwdnCmd->Param0
									,pFwdnCmd->Param1
									,pFwdnCmd->Param2
									,&_loc_Response_RequestData
									,&FWDN_PROT_Data_Receive) == TRUE )
		FWDN_PROT_ResponseAck();
	else
		FWDN_PROT_ResponseNack();
}

static void FWDN_PROT_Dump(FWDN_COMMAND_T *pFwdnCmd)
{
	unsigned int uiAddress = (unsigned int)pFwdnCmd->Param0;
	unsigned int uiSize = (unsigned int)pFwdnCmd->Param1;
	unsigned long _buf[20/4];
	char *name = (char *)_buf;
	if( pFwdnCmd->ExtraCmdSize != 16 )
	{
		FWDN_PROT_Data_ReceiveAway(pFwdnCmd->ExtraCmdSize);
		FWDN_DRV_SetErrorCode(ERR_FWDN_DRV_WRONG_PARAMETER);
		FWDN_PROT_ResponseNack();
		return;
	}

	FWDN_PROT_Data_Receive(name, 16);
	name[16]=0;

	if( FWDN_DRV_Dump( name
						,uiAddress
						,uiSize
						,&_loc_Response_NotifyData
						,&FWDN_PROT_Data_Send) == TRUE )
		FWDN_PROT_ResponseAck();
	else
		FWDN_PROT_ResponseNack();
}

static void FWDN_PROT_TOTALBIN_Read(FWDN_COMMAND_T *pFwdnCmd)
{
	if( FWDN_DRV_TOTALBIN_Read(&_loc_Response_NotifyData
								,&FWDN_PROT_Data_Send) == TRUE )
		FWDN_PROT_ResponseAck();
	else
		FWDN_PROT_ResponseNack();
}

static void FWDN_PROT_TOTALBIN_Write(FWDN_COMMAND_T *pFwdnCmd)
{
	if( FWDN_DRV_TOTALBIN_Write(&_loc_RquestData) == TRUE )
		FWDN_PROT_ResponseAck();
	else
		FWDN_PROT_ResponseNack();
}

static void FWDN_PROT_TEST(FWDN_COMMAND_T *pFwdnCmd)
{
	unsigned long _buf[512/4];
	unsigned char *dummy = (unsigned char *)_buf;
	unsigned int dataSize = pFwdnCmd->ExtraCmdSize;
	unsigned int packetSize;
	switch(pFwdnCmd->CmdType)
	{
		case FWDN_CMD_TEST_SEND:
		{
			while( dataSize > 0 )
			{
				packetSize = min(dataSize,512);
				if( FWDN_PROT_Data_Receive((void*)dummy,packetSize) != packetSize )
					break;
				dataSize -= packetSize;
			}
			break;
		}
		case FWDN_CMD_TEST_RECEIVE:
		{
			for(packetSize=0;packetSize<512;packetSize++)
				dummy[packetSize] = (unsigned char)packetSize;
			while( dataSize > 0 )
			{
				packetSize = min(dataSize,512);
				if( FWDN_PROT_Data_Send((void*)dummy,packetSize) != packetSize )
					break;
				dataSize -= packetSize;
			}
			break;
		}
	}

	if( dataSize == 0 )
		FWDN_PROT_ResponseAck();
	else
		FWDN_PROT_ResponseNack();
}

static void FWDN_PROT_UnknownCommand(FWDN_COMMAND_T *pFwdnCmd)
{
	FWDN_PROT_Data_ReceiveAway(pFwdnCmd->ExtraCmdSize);
	FWDN_DRV_SetErrorCode(ERR_FWDN_DRV_WRONG_PARAMETER);
	FWDN_DRV_AddMessage("Unknown Command!");
	FWDN_PROT_ResponseNack();
}

void FWDN_PROT_CheckCommand(void)
{
	FWDN_COMMAND_T fwdnCmd;

	if ( FWDN_PROT_CheckIsReceivedData() == 0 )
	{
		return;
	}

	if ( FWDN_PROT_Data_Receive( (void*)(&fwdnCmd), sizeof(FWDN_COMMAND_T) ) != sizeof(FWDN_COMMAND_T) )
	{
		//FWDN_PROT_Test();
		return;
	}

	//FWDN_PROT_Test();

	FWDN_DRV_ClearErrorCode();
	FWDN_DRV_ClearMessage();

	if(fwdnCmd.SubCmdType)
	{
		FWDN_DRV_SetErrorCode(ERR_FWDN_DRV_WRONG_PARAMETER);
		FWDN_PROT_Data_ReceiveAway(fwdnCmd.ExtraCmdSize);
		FWDN_PROT_ResponseNack();
	}

	gFwdn_CmdType = fwdnCmd.CmdType;
	switch(fwdnCmd.CmdType)
	{
		case FWDN_CMD_UBS_PING:
			FWDN_PROT_UBS_Ping();
			break;
		//case FWDN_CMD_UBS_INITCODE_LOAD:
		//	FWDN_PROT_UBS_InitCodeLoad(&fwdnCmd);
		//	break;
		//case FWDN_CMD_UBS_ROM_LOAD:
		//	FWDN_PROT_UBS_RomLoad(&fwdnCmd);
		//	break;

		case FWDN_CMD_DEVICE_RESET:
			FWDN_PROT_DEVICE_Reset();
			break;
		case FWDN_CMD_DEVICE_PING:
			FWDN_PROT_DEVICE_Ping(&fwdnCmd);
			break;
		case FWDN_CMD_DEVICE_INIT:
			FWDN_PROT_DEVICE_Init(&fwdnCmd);
			break;
		case FWDN_CMD_DEVICE_INFO_READ:
			FWDN_PROT_DEVICE_InfoRead(&fwdnCmd);
			break;
		case FWDN_CMD_DEVICE_SESSION_START:
			FWDN_PROT_DEVICE_SessionStart();
			break;
		case FWDN_CMD_DEVICE_SESSION_END:
			FWDN_PROT_DEVICE_SessionEnd(&fwdnCmd);
			break;
		case FWDN_CMD_DEVICE_SERIAL_NUMBER_WRITE:
			FWDN_PROT_DEVICE_SerialNumberWrite(&fwdnCmd);
			break;
		case FWDN_CMD_DEVICE_FIRMWARE_WRITE:
			FWDN_PROT_DEVICE_FirmwareWrite(&fwdnCmd);
			break;
		case FWDN_CMD_DEVICE_MESSAGE_SERVICE:
			FWDN_PROT_DEVICE_MessageService(&fwdnCmd);
			break;

		//case FWDN_CMD_AREA_INFO_READ:
		//	break;
		case FWDN_CMD_AREA_WRITE:
			FWDN_PROT_AREA_Write(&fwdnCmd);
			break;
		case FWDN_CMD_AREA_READ:
			FWDN_PROT_AREA_Read(&fwdnCmd);
			break;
		case FWDN_CMD_AREA_CALC_CRC:
			FWDN_PROT_AREA_CalcCRC(&fwdnCmd);
			break;

		case FWDN_CMD_DUMP_INFO_READ:
			FWDN_PROT_DUMP_InfoRead();
			break;
		case FWDN_CMD_DUMP_BLOCK_READ:
			FWDN_PROT_DUMP_BlockRead(&fwdnCmd);
			break;
		case FWDN_CMD_DUMP_BLOCK_WRITE:
			FWDN_PROT_DUMP_BlockWrite(&fwdnCmd);
			break;
		case FWDN_CMD_DUMP:
			FWDN_PROT_Dump(&fwdnCmd);
			break;

		case FWDN_CMD_TOTALBIN_WRITE:
			FWDN_PROT_TOTALBIN_Write(&fwdnCmd);
			break;
		case FWDN_CMD_TOTALBIN_READ:
			FWDN_PROT_TOTALBIN_Read(&fwdnCmd);
			break;

		case FWDN_CMD_TEST_SEND:
		case FWDN_CMD_TEST_RECEIVE:
			FWDN_PROT_TEST(&fwdnCmd);
			break;

		//case FWDN_CMD_TNFTL_V5_DEBUG:
		//	TNFTL_DEBUG_VTCParser( &fwdnCmd );

		default:
			FWDN_PROT_UnknownCommand(&fwdnCmd);
			break;
	}
}

#endif //FWDN_V7

#endif
/************* end of file *************************************************************/
