/*
 * OTG device I/O driver source code
 *
 * Copyright (c) 2010 Telechips, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "usb/usb_defs.h"
#include "usb/otgdev_io.h"
#include <platform/iomap.h>
#include <platform/reg_physical.h>
#include "usb/otgregs.h"
#if !defined(_LINUX_)
#include "memory.h"
#endif

/* For Signature */
#define OTGDEV_IO_SIGNATURE				'O','T','G','D','E','V','_','I','O','_'
#define OTGDEV_IO_VERSION				'V','2','.','0','0','1'
static const unsigned char OTGDEV_IO_C_Version[] = {SIGBYAHONG, OTGDEV_IO_SIGNATURE, SIGN_OS ,SIGN_CHIPSET, OTGDEV_IO_VERSION, NULL};


#define	OTGDEV_IO_TIMEOUT_RECV						0x4FFFF
#define	OTGDEV_IO_TIMEOUT_SEND						0x4FFFF

static USBDEVICE_IO_DRIVER_T s_OTGDEV_IO_Driver;


#if defined(_LINUX_)
extern void	memcpy(void *pvDest, const void *pvSrc, unsigned long iCount);
#endif

#define XFER_STATE_IDLE					0
#define XFER_STATE_WORKING				1
#define XFER_STATE_WAIT_POP				2

#define QUEUE_STATE_IDLE				0
#define QUEUE_STATE_ACTIVATE				1
#define QUEUE_STATE_DONE				2

#define QUEUE_MAX					16
#define QUEUE_EP0_SIZE					128
#if defined(SLAVE_MODE_INCLUDE)
#define QUEUE_TOTAL_SIZE				(128*1024)
//unsigned int gOTGDEV_IO_EP0QueueBuff[QUEUE_EP0_SIZE/4];
unsigned int gOTGDEV_IO_QueueBuff[QUEUE_TOTAL_SIZE/4];
#endif
typedef struct {
	USBDEV_QUEUE_T queue[QUEUE_MAX];
	unsigned char bQueueNum;
	volatile unsigned char iFirstQ;
	volatile unsigned char iLastQ;
	volatile unsigned char xferState;
	unsigned int MPS;
} EP_IN_T;
static EP_IN_T gOTGDEV_IO_EpIn[16];
typedef struct {
	USBDEV_QUEUE_T queue[QUEUE_MAX];
	unsigned char bQueueNum;
	volatile unsigned char iFirstQ;
	volatile unsigned char iLastQ;
	volatile unsigned char iPrevFirstQ;
	unsigned int MPS;
	unsigned int xferLeftLength;
	volatile unsigned char xferState;
	unsigned char bZeroPacket;
	#if defined(SLAVE_MODE_INCLUDE)
	unsigned int xferWaitPopSize;
	#endif
} EP_OUT_T;
EP_OUT_T gOTGDEV_IO_EpOut[16];

#define OTGDEV_IO_Lock()			BITCLR(USB20OTG->GAHBCFG,GAHBCFG_GlblIntrMsk_UNMASK)
#define OTGDEV_IO_Unlock()			BITSET(USB20OTG->GAHBCFG,GAHBCFG_GlblIntrMsk_UNMASK)

static unsigned short gOTGDEV_IO_IN_EP_UnstallEnable;
static unsigned short gOTGDEV_IO_OUT_EP_UnstallEnable;

//#if defined(DMA_MODE_INCLUDE)
static unsigned char gOTGDEV_IO_EP_OUT_QueuedFlag_for_IsReady[16];
//#endif

static USBDEV_ERROR_T sOTGDEV_IO_LastError = USBDEV_ERROR_FAILURE;

void _loc_memcpy(void *dst, void *src, unsigned long cnt)
{
#if defined(_LINUX_)
	if(((unsigned int)dst)&0x3 || ((unsigned int)src)&0x3)
	{
		char *p_bDst = (char*)dst;
		char *p_bSrc = (char*)src;
		while(cnt--)
			*p_bDst++ = *p_bSrc++;
	}
	else
	{
		memcpy(dst,src,cnt);
	}
#else
	memcpy(dst,src,cnt);
#endif
}

static void _loc_SetLastError(USBDEV_ERROR_T error)
{
	sOTGDEV_IO_LastError = error;
}

static unsigned int OTGDEV_IO_IsConnected(void)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	return (USB20OTG->GOTGCTL&GOTGCTL_BSesVld) ? TRUE : FALSE;
}

static void OTGDEV_IO_Init(void)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;

	USBDEV_DEBUG("OTGDEV_IO_Init\n");

	USB20OTG->DCTL = 0;
	#if defined(SLAVE_MODE_INCLUDE)
	USB20OTG->DIEPMSK = DIEPMSK_EPDisbldMsk_UNMASK|DIEPMSK_XferComplMsk_UNMASK;
	USB20OTG->DOEPMSK = DOEPMSK_SetUPMsk_UNMASK|DOEPMSK_XferComplMsk_UNMASK;
	// Unmask core interrupt
	USB20OTG->GINTMSK = GINTMSK_OEPIntMsk_UNMASK|GINTMSK_IEPIntMsk_UNMASK|GINTMSK_EnumDoneMsk_UNMASK|GINTMSK_USBRstMsk_UNMASK|GINTMSK_RxFLvlMsk_UNMASK;
	#elif defined(DMA_MODE_INCLUDE)
	USB20OTG->DIEPMSK = DIEPMSK_EPDisbldMsk_UNMASK|DIEPMSK_XferComplMsk_UNMASK;
	USB20OTG->DOEPMSK = DOEPMSK_SetUPMsk_UNMASK|DOEPMSK_XferComplMsk_UNMASK;
	//HwUSB20OTG->DCFG = DCFG_PerFrInt(0)|DCFG_NZStsOUTHShk_STALL|DCFG_DevSpd_HS;
	USB20OTG->GINTMSK = GINTMSK_OEPIntMsk_UNMASK|GINTMSK_IEPIntMsk_UNMASK|GINTMSK_EnumDoneMsk_UNMASK|GINTMSK_USBRstMsk_UNMASK;
	//HwUSB20OTG->GINTMSK = GINTMSK_USBRstMsk_UNMASK|GINTMSK_EnumDoneMsk_UNMASK|GINTMSK_ErlySuspMsk_UNMASK|GINTMSK_USBSuspMsk_UNMASK|GINTMSK_SofMsk_UNMASK;
 	#endif
}

#define MAX_OUT_EPn_PACKET_SIZE			512
#define MAX_IN_EP0_PACKET_SIZE			64
#define MAX_IN_EPn_PACKET_SIZE			512
#define SUPPORTED_CONTROL_EP_NUMBER		1
#define OUT_EP_NUMBER					16
#define GRXFSIZ_COMMON					( ( 4*( (MAX_OUT_EPn_PACKET_SIZE/4) + 1 ) ) + (4*SUPPORTED_CONTROL_EP_NUMBER) + 6 + (2*OUT_EP_NUMBER) + 1 )
#define GNPTXFSIZ_COMMON				(1*(MAX_IN_EP0_PACKET_SIZE/4))
#define DIEPTXFn_COMMON					(4*(MAX_IN_EPn_PACKET_SIZE/4))
#define IN_EP_NUMBER_COMMON				6
static void _loc_DynamicFIFOAllocation(void)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	unsigned int i,fifoAddress;

	if (USB20OTG->DIEPCTL0&DIEPCTL0_EPEna)
		BITSET(USB20OTG->DIEPCTL0,DIEPCTL0_EPDis);
	BITSET(USB20OTG->DIEPCTL0,DIEPCTL0_SNAK);
	// N/A
	//if(USB20OTG->DOEPCTL0&DOEPCTL0_EPEna)
	//	BITSET(USB20OTG->DOEPCTL0,DOEPCTL0_EPDis);
	BITSET(USB20OTG->DOEPCTL0,DOEPCTL0_SNAK);

	for (i=0; i<15; i++) {
		if (USB20OTG->DEVINENDPT[i][DIEPCTL_INDEX]&DIEPCTL_EPEna)
			BITSET(USB20OTG->DEVINENDPT[i][DIEPCTL_INDEX],DIEPCTL_EPDis);
		BITSET(USB20OTG->DEVINENDPT[i][DIEPCTL_INDEX],DIEPCTL_SNAK);

		if (USB20OTG->DEVOUTENDPT[i][DOEPCTL_INDEX]&DOEPCTL_EPEna)
			BITSET(USB20OTG->DEVOUTENDPT[i][DOEPCTL_INDEX],DOEPCTL_EPDis);
		BITSET(USB20OTG->DEVOUTENDPT[i][DOEPCTL_INDEX],DOEPCTL_SNAK);
	}

	switch(USBDEV_GetCurrentClass())
	{
		//case USBDEV_CLASS_MSC:
		//	break;
		//case USBDEV_CLASS_CDC:
		//	break;
		default:
			USB20OTG->GRXFSIZ = GRXFSIZ_COMMON;
			fifoAddress = GRXFSIZ_COMMON;
			USB20OTG->GNPTXFSIZ = GNPTXFSIZ_INEPTxF0Dep(GNPTXFSIZ_COMMON)|GNPTXFSIZ_INEPTxF0StAddr(fifoAddress);
			fifoAddress += GNPTXFSIZ_COMMON;
			for( i = 0 ; i < IN_EP_NUMBER_COMMON ; i++ ) {
				USB20OTG->DIEPTXFn[i] = DIEPTXF_INEPnTxFDep(DIEPTXFn_COMMON)|DIEPTXF_INEPnTxFStAddr(fifoAddress);
				fifoAddress += DIEPTXFn_COMMON;
			}
			break;
	}

	if( fifoAddress > 4160 )
	{
		USBDEV_ERROR("FIFO allocation overflow is occurred!!! (size=%d / 4160)",fifoAddress);
		while(1);	// FIFO allocation overflow is occurred!!!
	}
	else
	{
		USBDEV_DEBUG("FIFO allocation. (size=%d / 4160)",fifoAddress);
	}

	// Reset - set and flush All TxFIFO, flush RxFIFO
	USB20OTG->GRSTCTL |= GRSTCTL_TxFNum(0x10)|GRSTCTL_TxFFlsh|GRSTCTL_RxFFlsh;
	do
	{
		if( (USB20OTG->GRSTCTL&(GRSTCTL_TxFFlsh|GRSTCTL_RxFFlsh)) == 0 )
			break;
	} while(1);
}

static void _loc_ResetQueue(void)
{
	int i,j;

	for(i=0;i<16;i++)
	{
		gOTGDEV_IO_EpIn[i].iFirstQ = 0;
		gOTGDEV_IO_EpIn[i].iLastQ = 0;
		gOTGDEV_IO_EpIn[i].xferState = XFER_STATE_IDLE;

		gOTGDEV_IO_EpOut[i].iFirstQ = 0;
		gOTGDEV_IO_EpOut[i].iLastQ = 0;
		gOTGDEV_IO_EpOut[i].iPrevFirstQ = gOTGDEV_IO_EpOut[i].bQueueNum;
		gOTGDEV_IO_EpOut[i].xferLeftLength = 0;
		gOTGDEV_IO_EpOut[i].bZeroPacket = 0;
		gOTGDEV_IO_EpOut[i].xferState = XFER_STATE_IDLE;

		for(j=0;j<QUEUE_MAX;j++)
		{
			gOTGDEV_IO_EpIn[i].queue[j].state = QUEUE_STATE_IDLE;
			gOTGDEV_IO_EpIn[i].queue[j].actualLen = 0;
			gOTGDEV_IO_EpOut[i].queue[j].state = QUEUE_STATE_IDLE;
			gOTGDEV_IO_EpOut[i].queue[j].actualLen = 0;
		}

		gOTGDEV_IO_EP_OUT_QueuedFlag_for_IsReady[i] = FALSE;
	}
}

static void *_loc_GetQueueBuffer(unsigned int *pSize)
{
	void *pQueueBuff = NULL;

#if defined(SLAVE_MODE_INCLUDE)
	pQueueBuff = (void*)gOTGDEV_IO_QueueBuff;
	*pSize = QUEUE_TOTAL_SIZE;
#elif defined(DMA_MODE_INCLUDE)
	pQueueBuff = dma_alloc(4096, 4096*2);
	*pSize = 4096*2;
#endif

	return pQueueBuff;
}

static int OTGDEV_IO_AllocQueue(PUSBDEV_QUEUE_ALLOC_T pQueueAlloc)
{
	unsigned int i;
	unsigned char *pQueueBuff, *pQueueBuffBase;
	unsigned int uiMaxBuffQueueSize;

	pQueueBuff = (unsigned char*)_loc_GetQueueBuffer(&uiMaxBuffQueueSize);
	pQueueBuffBase = pQueueBuff;

	// Queue allocation for EP0
	gOTGDEV_IO_EpIn[0].bQueueNum = 1;
	gOTGDEV_IO_EpIn[0].queue[0].buff = pQueueBuff;
	gOTGDEV_IO_EpIn[0].queue[0].buffSize = QUEUE_EP0_SIZE;

	gOTGDEV_IO_EpOut[0].bQueueNum = 1;
	gOTGDEV_IO_EpOut[0].queue[0].buff = pQueueBuff;
	gOTGDEV_IO_EpOut[0].queue[0].buffSize = QUEUE_EP0_SIZE;
	pQueueBuff = &pQueueBuff[QUEUE_EP0_SIZE];

	for( i=1 ; i<16 ; i++ )
	{
		gOTGDEV_IO_EpIn[i].bQueueNum = 0;
		gOTGDEV_IO_EpOut[i].bQueueNum = 0;
	}

	while(pQueueAlloc)
	{
		unsigned char ep_index = pQueueAlloc->bEpAddress&0x7F;
		if(pQueueAlloc->bQueueNum > QUEUE_MAX)
		{
			_loc_SetLastError(USBDEV_ERROR_QUEUE_ALLOC_QUEUE_NUM);
			return FALSE;
		}

		if(pQueueAlloc->bEpAddress&0x80/*IN*/)
		{
			gOTGDEV_IO_EpIn[ep_index].bQueueNum = pQueueAlloc->bQueueNum;
			for(i=0 ; i<gOTGDEV_IO_EpIn[ep_index].bQueueNum ; i++)
			{
				gOTGDEV_IO_EpIn[ep_index].queue[i].buff = pQueueBuff;
				gOTGDEV_IO_EpIn[ep_index].queue[i].buffSize = pQueueAlloc->uiQueueBuffSize;
				pQueueBuff = &pQueueBuff[pQueueAlloc->uiQueueBuffSize];
			}
		}
		else
		{
			gOTGDEV_IO_EpOut[ep_index].bQueueNum = pQueueAlloc->bQueueNum;
			for(i=0 ; i<gOTGDEV_IO_EpOut[ep_index].bQueueNum ; i++)
			{
				gOTGDEV_IO_EpOut[ep_index].queue[i].buff = pQueueBuff;
				gOTGDEV_IO_EpOut[ep_index].queue[i].buffSize = pQueueAlloc->uiQueueBuffSize;
				pQueueBuff = &pQueueBuff[pQueueAlloc->uiQueueBuffSize];
			}
		}

		pQueueAlloc = pQueueAlloc->pNext;
	}

	if(((unsigned int)pQueueBuff-(unsigned int)pQueueBuffBase) > uiMaxBuffQueueSize)
	{
		_loc_SetLastError(USBDEV_ERROR_QUEUE_ALLOC_BUFFER_SIZE);
		return FALSE;
	}

	return TRUE;
}

static void OTGDEV_IO_Reset(void)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	unsigned int i;//j,fifoAddress;

	USBDEV_DEBUG("OTGDEV_IO_Reset\n");

	USB20OTG->DCTL = 0;
	USB20OTG->DCFG = 0;

	_loc_DynamicFIFOAllocation();

	// clear end-point interrupt
	USB20OTG->DIEPINT0 = USB20OTG->DIEPINT0;
	for (i=0; i<15; i++)
		USB20OTG->DEVINENDPT[i][DIEPINT_INDEX] = USB20OTG->DEVINENDPT[i][DIEPINT_INDEX];
	USB20OTG->DOEPINT0 = USB20OTG->DOEPINT0;
	for (i=0; i<15; i++)
		USB20OTG->DEVOUTENDPT[i][DOEPINT_INDEX] = USB20OTG->DEVOUTENDPT[i][DOEPINT_INDEX];

	_loc_ResetQueue();

#if defined(SLAVE_MODE_INCLUDE)
	USB20OTG->DAINTMSK = DAINTMSK_OutEPInt(0);
 	// Program endpoint-specific registers for control OUT endpoint 0
	USB20OTG->DOEPTSIZ0 = DOEPTSIZ0_SUPCnt(3)|DOEPTSIZ0_PktCnt(1)|DOEPTSIZ0_XferSize(8);
	gOTGDEV_IO_EpOut[0].xferState = XFER_STATE_WORKING;

	USB20OTG->DIEPEMPMSK = 0;
#elif defined(DMA_MODE_INCLUDE)
	USB20OTG->DAINTMSK = DAINTMSK_OutEPInt(0);
 	// Program endpoint-specific registers for control OUT endpoint 0
	USB20OTG->DOEPTSIZ0 = DOEPTSIZ0_SUPCnt(3)|DOEPTSIZ0_PktCnt(1)|DOEPTSIZ0_XferSize(8);
	USB20OTG->DOEPDMA0 = (unsigned int)gOTGDEV_IO_EpOut[0].queue[0].buff;
	gOTGDEV_IO_EpOut[0].xferState = XFER_STATE_WORKING;
#endif

	gOTGDEV_IO_IN_EP_UnstallEnable = 0xFFFF;
	gOTGDEV_IO_OUT_EP_UnstallEnable = 0xFFFF;
}

static void OTGDEV_IO_EnumDone(void)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	unsigned int enumSpeed;	// 0:high speed(30/60MHz), 1:full speed(30/60MHz), 2:low speed(6MHz), 3: full speed(48MHz)
	enumSpeed = DSTS_EnumSpd_Read(USB20OTG->DSTS);
	if( enumSpeed == 0 )
	{
		s_OTGDEV_IO_Driver.currentSpeed = USBDEV_HIGH_SPEED;
	}
	else
	{
		s_OTGDEV_IO_Driver.currentSpeed = USBDEV_FULL_SPEED;
	}
	#if defined(SLAVE_MODE_INCLUDE)
	// EPOUT0 Control
	USB20OTG->DOEPCTL0 = DOEPCTL0_EPEna|DOEPCTL0_CNAK|DOEPCTL0_EPType|DOEPCTL0_MPS_64BYTES;

	// EPIN0 Control
	USB20OTG->DIEPCTL0 = DIEPCTL0_TxFNum(0)|DIEPCTL0_EPType|DIEPCTL0_USBActEP_MASK|DIEPCTL0_MPS_64BYTES;

	#elif defined(DMA_MODE_INCLUDE)
	// EPOUT0 Control
	USB20OTG->DOEPCTL0 = DOEPCTL0_EPEna|DOEPCTL0_CNAK|DOEPCTL0_EPType|DOEPCTL0_MPS_64BYTES;

	// EPIN0 Control
	USB20OTG->DIEPCTL0 = DIEPCTL0_TxFNum(0)|DIEPCTL0_EPType|DIEPCTL0_USBActEP_MASK|DIEPCTL0_MPS_64BYTES;
	#endif
}

static unsigned int _loc_WaitForSet(volatile unsigned int *pReg, unsigned int mask)
{
	while( !( (*pReg) & mask ) )
	{
		if( USBDEV_IsConnected() == FALSE )
			return FALSE;
	}
	return TRUE;
}

static unsigned int _loc_WaitForClear(volatile unsigned int *pReg, unsigned int mask)
{
	while( (*pReg) & mask )
	{
		if( USBDEV_IsConnected() == FALSE )
			return FALSE;
	}
	return TRUE;
}

static void _loc_Disable_IN_EP(unsigned int nEndPoint)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;

	if( nEndPoint < 16 )
	{
		volatile unsigned int *pDIEPINT = (volatile unsigned int*)((unsigned int)&USB20OTG->DIEPINT0 + 0x20*nEndPoint);
		volatile unsigned int *pDIEPCTL = (volatile unsigned int*)((unsigned int)&USB20OTG->DIEPCTL0 + 0x20*nEndPoint);

		BITSET(*pDIEPINT, DIEPINT_INEPNakEff);	// clear
		BITSET(*pDIEPCTL, DIEPCTL_SNAK);
		if( _loc_WaitForSet(pDIEPINT, DIEPINT_INEPNakEff) == FALSE )
			return;

		if( *pDIEPCTL & DIEPCTL_EPEna )
		{
			BITSET(*pDIEPINT, DIEPINT_EPDisabld);	// clear
			BITSET(*pDIEPCTL, DIEPCTL_EPDis|DIEPCTL_SNAK);
			if( _loc_WaitForSet(pDIEPINT, DIEPINT_EPDisabld) == FALSE )
				return;

			// flush end-point
			GRSTCTL_TxFNum_Write(USB20OTG->GRSTCTL,nEndPoint);
			BITSET(USB20OTG->GRSTCTL, GRSTCTL_TxFFlsh);
			if( _loc_WaitForClear(&USB20OTG->GRSTCTL, GRSTCTL_TxFFlsh) == FALSE )
				return;
		}
	}
}

static void _loc_Disable_OUT_EP(unsigned int nEndPoint)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;

	if( nEndPoint < 16 )
	{
		volatile unsigned int *pDOEPINT = (volatile unsigned int*)((unsigned int)&USB20OTG->DOEPINT0 + 0x20*nEndPoint);
		volatile unsigned int *pDOEPCTL = (volatile unsigned int*)((unsigned int)&USB20OTG->DOEPCTL0 + 0x20*nEndPoint);

		BITSET(USB20OTG->DCTL, DCTL_SGOUTNak);
		if( _loc_WaitForSet(&USB20OTG->GINTSTS,GINTSTS_GOUTNakEff) == FALSE )
			return;

		if( *pDOEPCTL & DOEPCTL_EPEna )
		{
			BITSET(*pDOEPINT, DOEPINT_EPDisabld);	// clear
			BITSET(*pDOEPCTL, DOEPCTL_EPDis|DOEPCTL_SNAK);
			if( _loc_WaitForSet(pDOEPINT, DOEPINT_EPDisabld) == FALSE )
				return;
		}

		BITCLR(USB20OTG->DCTL, DCTL_CGOUTNak);
	}
	//if( 0 < nEndPoint && nEndPoint < 16 )
	//{
	//	BITSET(USB20OTG->GINTSTS, GINTSTS_GOUTNakEff);
	//	BITSET(USB20OTG->DCTL, DCTL_SGOUTNak);
	//	if( _loc_WaitForSet(&USB20OTG->GINTSTS,GINTSTS_GOUTNakEff) == FALSE )
	//		return;
	//
	//	if( USB20OTG->DEVOUTENDPT[nEndPoint-1][DOEPCTL_INDEX] & DOEPCTL_EPEna )
	//	{
	//		BITSET(USB20OTG->DEVOUTENDPT[nEndPoint-1][DOEPINT_INDEX], DOEPINT_EPDisabld);	// clear
	//		BITSET(USB20OTG->DEVOUTENDPT[nEndPoint-1][DOEPCTL_INDEX], DOEPCTL_EPDis|DOEPCTL_SNAK);
	//		if( _loc_WaitForSet(&USB20OTG->DEVINENDPT[nEndPoint-1][DOEPINT_INDEX], DOEPINT_EPDisabld) == FALSE )
	//			return;
	//	}
	//
	//	BITCLR(USB20OTG->DCTL, DCTL_SGOUTNak);
	//}
}

static void OTGDEV_IO_EP_IN_StartTransfer(unsigned int nEndPoint)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	volatile unsigned int *pDIEPTSIZ,*pDIEPCTL;
	unsigned int PktCnt,XferSize,maxPacketSize;
	unsigned char iFirstQ = gOTGDEV_IO_EpIn[nEndPoint].iFirstQ;
	PUSBDEV_QUEUE_T pFirstQ = &gOTGDEV_IO_EpIn[nEndPoint].queue[iFirstQ];

	if(nEndPoint==0)
	{
		pDIEPTSIZ = &USB20OTG->DIEPTSIZ0;
		pDIEPCTL = &USB20OTG->DIEPCTL0;
		BITCLR(*pDIEPTSIZ,DIEPTSIZ0_PktCnt(0x3)|DIEPTSIZ0_XferSize(0x7F));
	}
	else
	{
		pDIEPTSIZ = &USB20OTG->DEVINENDPT[nEndPoint-1][DIEPTSIZ_INDEX];
		pDIEPCTL = &USB20OTG->DEVINENDPT[nEndPoint-1][DIEPCTL_INDEX];
		BITCLR(*pDIEPTSIZ,DIEPTSIZ_PktCnt(0x3FF)|DIEPTSIZ_XferSize(0x7FFFF));
	}

	if(pFirstQ->xferLen==0)
	{
		XferSize=0;
		PktCnt=1;
	}
	else
	{
		if(nEndPoint==0)
		{
			maxPacketSize = 64;
			#if defined(SLAVE_MODE_INCLUDE)
			XferSize = min(0x7F, pFirstQ->xferLen - pFirstQ->actualLen);
			#elif defined(DMA_MODE_INCLUDE)
			XferSize = min(0x7F, min(pFirstQ->buffSize,pFirstQ->xferLen - pFirstQ->actualLen));
			#endif
			PktCnt=(XferSize+maxPacketSize-1)/maxPacketSize;
			if( PktCnt > 3 )
			{
				XferSize = maxPacketSize*3;
				PktCnt = 3;
			}
		}
		else
		{
			maxPacketSize = DOEPCTL_MPS_Read(USB20OTG->DEVINENDPT[nEndPoint-1][DIEPCTL_INDEX]);
			#if defined(SLAVE_MODE_INCLUDE)
			XferSize = min(0x7FFFF, pFirstQ->xferLen - pFirstQ->actualLen);
			#elif defined(DMA_MODE_INCLUDE)
			XferSize = min(0x7FFFF, min(pFirstQ->buffSize,pFirstQ->xferLen - pFirstQ->actualLen));
			#endif
			PktCnt=(XferSize+maxPacketSize-1)/maxPacketSize;
			if( PktCnt > 0x3FF )
			{
				XferSize = maxPacketSize*0x3FF;
				PktCnt = 0x3FF;
			}
		}
	}

	USBDEV_DEBUG("OTGDEV_IO EP IN %d StartTransfer PktCnt=%d, XferSize=%d\n",nEndPoint,PktCnt,XferSize);

	BITSET(*pDIEPTSIZ,DIEPTSIZ_PktCnt(PktCnt)|DIEPTSIZ_XferSize(XferSize));
	#if defined(DMA_MODE_INCLUDE)
	if( nEndPoint == 0 )
	{
		USB20OTG->DIEPDMA0 = (unsigned int)gOTGDEV_IO_EpIn[0].queue[0].buff;
		//_loc_memcpy(pEpIn->dmaAddress, pEpIn->xferBuff, XferSize);
	}
	else
	{
		USB20OTG->DEVINENDPT[nEndPoint-1][DIEPDMA_INDEX] = (unsigned int)pFirstQ->buff;
		//_loc_memcpy(pEpIn->dmaAddress, pEpIn->xferBuff, XferSize);
	}
	//pEpIn->xferBuff = (void*)( (unsigned int)pEpIn->xferBuff + XferSize );
	pFirstQ->actualLen += XferSize;
	#endif
	BITSET(*pDIEPCTL,DIEPCTL_EPEna|DIEPCTL_CNAK);

	#if defined(SLAVE_MODE_INCLUDE)
	if(XferSize>0)
	{
		/* Enable the Tx FIFO Empty Interrupt for this EP */
		BITSET(USB20OTG->DIEPEMPMSK,DIEPEMPMSK_InEpTxfEmpMsk(nEndPoint));
	}
	#endif
}

static unsigned int OTGDEV_IO_EP_IN_StartNextTransfer(unsigned int nEndPoint)
{
	unsigned char iFirstQ = gOTGDEV_IO_EpIn[nEndPoint].iFirstQ;
	unsigned char iFirstQ_Next = ((iFirstQ+1) >= gOTGDEV_IO_EpIn[nEndPoint].bQueueNum) ? 0 : (iFirstQ+1);

	if( iFirstQ != gOTGDEV_IO_EpIn[nEndPoint].iLastQ )
	{
		PUSBDEV_QUEUE_T pFirstQ_Next = &gOTGDEV_IO_EpIn[nEndPoint].queue[iFirstQ_Next];
		gOTGDEV_IO_EpIn[nEndPoint].iFirstQ = iFirstQ_Next;
		USBDEV_DEBUG("EP IN iFirstQ changed %d",iFirstQ_Next);
		if( pFirstQ_Next->state == QUEUE_STATE_ACTIVATE )
		{
			USBDEV_DEBUG("EP_IN_StartNextTransfer");
			OTGDEV_IO_EP_IN_StartTransfer(nEndPoint);
			return TRUE;
		}
	}

	return FALSE;
}

static void OTGDEV_IO_EP_OUT_StartTransfer(unsigned int nEndPoint)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	//#if defined(SLAVE_MODE_INCLUDE)
	//HwUSB20OTG->DEVOUTENDPT[nEndPoint-1][DOEPCTL_INDEX] |= DOEPCTL_CNAK;
	//BITSET(HwUSB20OTG->GINTMSK, GINTMSK_RxFLvlMsk_UNMASK);
	//#elif defined(DMA_MODE_INCLUDE)
	unsigned int PktCnt,XferSize;
	unsigned char iLastQ = gOTGDEV_IO_EpOut[nEndPoint].iLastQ;
	PUSBDEV_QUEUE_T pLastQ = &gOTGDEV_IO_EpOut[nEndPoint].queue[iLastQ];

	XferSize = pLastQ->xferLen;
	PktCnt = (XferSize+gOTGDEV_IO_EpOut[nEndPoint].MPS-1)/gOTGDEV_IO_EpOut[nEndPoint].MPS;

	// Program endpoint-specific registers for OUT endpoint
	USB20OTG->DEVOUTENDPT[nEndPoint-1][DOEPTSIZ_INDEX] = DOEPTSIZ_PktCnt(PktCnt)|DOEPTSIZ_XferSize(XferSize);
	#if defined(DMA_MODE_INCLUDE)
	USB20OTG->DEVOUTENDPT[nEndPoint-1][DOEPDMA_INDEX] = (unsigned int)pLastQ->buff;
	#endif
	USB20OTG->DEVOUTENDPT[nEndPoint-1][DOEPCTL_INDEX] |= DOEPCTL_EPEna|DOEPCTL_CNAK;
	//#endif

	USBDEV_DEBUG("Tgr EP OUT pkt=%d size=%d\n",PktCnt,XferSize);
}

static unsigned int OTGDEV_IO_EP_OUT_StartNextTransfer(unsigned int nEndPoint)
{
	unsigned char iLastQ = gOTGDEV_IO_EpOut[nEndPoint].iLastQ;
	PUSBDEV_QUEUE_T pLastQ = &gOTGDEV_IO_EpOut[nEndPoint].queue[iLastQ];

	if( gOTGDEV_IO_EpOut[nEndPoint].xferState != XFER_STATE_WORKING
		&& pLastQ->state == QUEUE_STATE_IDLE && (gOTGDEV_IO_EpOut[nEndPoint].xferLeftLength||gOTGDEV_IO_EpOut[nEndPoint].bZeroPacket) )
	{
		pLastQ->xferLen = min(gOTGDEV_IO_EpOut[nEndPoint].xferLeftLength,pLastQ->buffSize);
		if(!pLastQ->xferLen)
			gOTGDEV_IO_EpOut[nEndPoint].bZeroPacket = 0;
		gOTGDEV_IO_EpOut[nEndPoint].xferLeftLength -= pLastQ->xferLen;
		pLastQ->state = QUEUE_STATE_ACTIVATE;
		gOTGDEV_IO_EpOut[nEndPoint].xferState = XFER_STATE_WORKING;
		USBDEV_DEBUG("StartNextTransfer iLastQ(%d): ",iLastQ);
		OTGDEV_IO_EP_OUT_StartTransfer(nEndPoint);
		return TRUE;
	}

	return FALSE;
}

static unsigned int OTGDEV_IO_EP0_Read(void *pBuffer, unsigned int bufLenInByte)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	unsigned int nTotalRead=0;

	#if defined(SLAVE_MODE_INCLUDE)
	while(bufLenInByte>0)
	{
		unsigned int readSize = min(64,bufLenInByte);
		USB20OTG->DOEPTSIZ0 = DOEPTSIZ0_SUPCnt(3)|DOEPTSIZ0_PktCnt(1)|DOEPTSIZ0_XferSize(readSize);
		USB20OTG->DOEPCTL0 |= DOEPCTL0_EPEna|DOEPCTL0_CNAK;

		{
			volatile unsigned int GRXSTSP;
			volatile unsigned int GRXSTSR;
			PGRXSTSP_DEV_T pGRXSTSR;
			//unsigned int PktSts = pGRXSTSR->PktSts;
			unsigned int DWCnt;
			unsigned int i=0;
			unsigned int readSize;

			while(!(USB20OTG->DOEPINT0 & DOEPINT_XferCompl))
			{
				if(USB20OTG->GINTSTS&GINTSTS_RxFLvl_MASK)
				{
					GRXSTSR = USB20OTG->GRXSTSR;
					pGRXSTSR = (PGRXSTSP_DEV_T)&GRXSTSR;
					//if( pGRXSTSR->EPNum == 0 && pGRXSTSR->PktSts == 2/*OUT data packet received*/ )
					//	break;
					if( pGRXSTSR->BCnt )
					{
						if( pGRXSTSR->EPNum == 0 && pGRXSTSR->PktSts == 2/*OUT data packet received*/ )
						{
							unsigned int buff[64/4];
							DWCnt = (pGRXSTSR->BCnt+3)>>2;
							GRXSTSP = USB20OTG->GRXSTSP;	// pop for read from FIFO
							while( DWCnt-- > 0 )
							{
								if( i < (64/4) )
									buff[i++] = USB20OTG->DFIFOENDPT[0][0];
								else
								{
									volatile unsigned int dump = USB20OTG->DFIFOENDPT[0][0];
								}
							}
							readSize = min(bufLenInByte, pGRXSTSR->BCnt);
							_loc_memcpy(pBuffer,buff,readSize);
							bufLenInByte -= readSize;
							nTotalRead += readSize;
							pBuffer = (void*)((unsigned int)pBuffer + readSize);
							break;
						}
						else
							return nTotalRead;
					}

					GRXSTSP = USB20OTG->GRXSTSP;	// pop for read from FIFO

					//GRXSTSR = USB20OTG->GRXSTSR;
					//pGRXSTSR = (PGRXSTSP_DEV_T)&GRXSTSR;
				}

				if( USBDEV_IsConnected()==FALSE)
					return nTotalRead;
			}

			//DWCnt = (pGRXSTSR->BCnt+3)>>2;

			//if( pGRXSTSR->EPNum == 0 && pGRXSTSR->PktSts == 2/*OUT data packet received*/ )
			//{
			//	unsigned int buff[64/4];
			//	GRXSTSP = HwUSB20OTG->GRXSTSP;	// pop for read from FIFO
			//	while( DWCnt-- > 0 )
			//	{
			//		if( i < (64/4) )
			//			buff[i++] = HwUSB20OTG->DFIFOENDPT[0][0];
			//		else
			//		{
			//			volatile unsigned int dump = HwUSB20OTG->DFIFOENDPT[0][0];
			//		}
			//	}
			//	readSize = min(bufLenInByte, pGRXSTSR->BCnt);
			//	_loc_memcpy(pBuffer,buff,readSize);
			//	bufLenInByte -= readSize;
			//	nTotalRead += readSize;
			//	pBuffer = (void*)((unsigned int)pBuffer + readSize);
			//	USB20OTG->DOEPCTL0 |= DOEPCTL0_CNAK;
			//	USBDEV_DEBUG("EP0Read BCnt %d, PktSts %d",pGRXSTSR->BCnt,pGRXSTSR->PktSts);
			//}
			//else
			//{
			//	USBDEV_ERROR("EP0_Read error! EPNum %d, BCnt %d, PktSts %d bufLenInByte %d nTotalRead %d",pGRXSTSR->EPNum,pGRXSTSR->BCnt,pGRXSTSR->PktSts,bufLenInByte,nTotalRead);
			//	return nTotalRead;
			//}
		}
	}
	#elif defined(DMA_MODE_INCLUDE)
	while(bufLenInByte>0)
	{
		unsigned int readSize = min(64,bufLenInByte);
		USB20OTG->DOEPTSIZ0 = DOEPTSIZ0_SUPCnt(3)|DOEPTSIZ0_PktCnt(1)|DOEPTSIZ0_XferSize(readSize);
		USB20OTG->DOEPDMA0 = (unsigned int)gOTGDEV_IO_EpOut[0].queue[0].buff;
		USB20OTG->DOEPCTL0 |= DOEPCTL0_EPEna|DOEPCTL0_CNAK;

		while( !(HwUSB20OTG->DOEPINT0 & DOEPINT_XferCompl) )
		{
			if( USBDEV_IsConnected()==FALSE)
				return nTotalRead;
		}
		USB20OTG->DOEPINT0 = DOEPINT_XferCompl;
		_loc_memcpy(pBuffer,gOTGDEV_IO_EpOut[0].queue[0].buff,readSize);
		bufLenInByte -= readSize;
		nTotalRead += readSize;
		pBuffer = (void*)((unsigned int)pBuffer + readSize);
	}
	#endif

	return nTotalRead;
}

static unsigned int OTGDEV_IO_EP0_Write(void *pBuffer, unsigned int bufLenInByte)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	#if defined(SLAVE_MODE_INCLUDE)
	unsigned int writePacketSize, DWCnt, i;
	unsigned int writeLength=0;
	unsigned int packetBuf[16];

	do
	{
		unsigned int freeFIFOSize, writePacketCount;
		while( USB20OTG->DIEPTSIZ0&(DIEPTSIZ0_PktCnt_MASK|DIEPTSIZ0_XferSize_MASK) )
		{
			if( USBDEV_IsConnected()==FALSE)
				return writeLength;
		}

		//writePacketSize = min(64,bufLenInByte);
		freeFIFOSize = USB20OTG->DTXFSTS0<<2;
		writePacketSize = min(freeFIFOSize,bufLenInByte);
		if( writePacketSize == 0 )
			writePacketCount = 1;
		else
			writePacketCount = (writePacketSize+64-1)/64;

		bufLenInByte -= writePacketSize;

		USB20OTG->DIEPTSIZ0 = DIEPTSIZ0_PktCnt(writePacketCount)|DIEPTSIZ0_XferSize(writePacketSize);
		_loc_memcpy(packetBuf, pBuffer, writePacketSize);
		USB20OTG->DIEPCTL0 |= DIEPCTL0_EPEna|DIEPCTL0_CNAK;
		DWCnt = (writePacketSize+3)>>2;
		for( i=0 ; i<DWCnt ; i++ )
			USB20OTG->DFIFOENDPT[0][0] = packetBuf[i];

 		pBuffer = (void*)((unsigned int)pBuffer + writePacketSize);
		writeLength += writePacketSize;
	}while( bufLenInByte > 0 );

	return writeLength;
	#elif defined(DMA_MODE_INCLUDE)
	unsigned int writePacketSize,PktCnt;
	unsigned int writeLength=0;

	do
	{
		while( USB20OTG->DIEPTSIZ0&(DIEPTSIZ0_PktCnt_MASK|DIEPTSIZ0_XferSize_MASK) )
		{
			if( USBDEV_IsConnected()==FALSE)
				return writeLength;
		}

		writePacketSize = min(gOTGDEV_IO_EpIn[0].queue[0].buffSize,bufLenInByte);
		if(writePacketSize)
			PktCnt = (writePacketSize+64-1)>>6/*/64*/;
		else
			PktCnt = 1;
		bufLenInByte -= writePacketSize;
		_loc_memcpy(gOTGDEV_IO_EpIn[0].queue[0].buff, pBuffer, writePacketSize);
		USB20OTG->DIEPTSIZ0 = DIEPTSIZ0_PktCnt(PktCnt)|DIEPTSIZ0_XferSize(writePacketSize);
		USB20OTG->DIEPDMA0 = (unsigned int)gOTGDEV_IO_EpIn[0].queue[0].buff;
		USB20OTG->DIEPCTL0 |= DIEPCTL0_EPEna|DIEPCTL0_CNAK;

 		pBuffer = (void*)((unsigned int)pBuffer + writePacketSize);
		writeLength += writePacketSize;
	} while( bufLenInByte > 0 );

	return writeLength;
	#endif
}

static void OTGDEV_IO_EP_Stall(unsigned char epAddress)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	unsigned int nEndPoint = (unsigned int)(epAddress & 0x0F);
	volatile unsigned int *pDEPCTL;
	if( epAddress & 0x80 )
	{
		// IN
		pDEPCTL = &USB20OTG->DIEPCTL0;
	}
	else
	{
		// OUT
		pDEPCTL = &USB20OTG->DOEPCTL0;
	}
	pDEPCTL = (volatile unsigned int*)( (unsigned int)pDEPCTL + 0x20*nEndPoint );

	if( epAddress & 0x80 )
	{
		// IN
		if(*pDEPCTL&DIEPCTL_EPEna)
		{
			// stall: in ep%d disabling
			BITSET(*pDEPCTL,DIEPCTL_EPDis);
		}
	}
	USBDEV_DEBUG("stall! ep%x",epAddress);
	BITSET(*pDEPCTL,Hw21);	// stall
}

static void OTGDEV_IO_EP_Unstall(unsigned char epAddress)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	unsigned int nEndPoint = (unsigned int)(epAddress & 0x0F);
	volatile unsigned int *pDEPCTL;
	unsigned int flagUnstall = FALSE;
	if( epAddress & 0x80 )
	{
		// IN
		if( gOTGDEV_IO_IN_EP_UnstallEnable & (1<<nEndPoint) )
		{
			flagUnstall = TRUE;
			pDEPCTL = &USB20OTG->DIEPCTL0;
		}
	}
	else
	{
		// OUT
		if( gOTGDEV_IO_OUT_EP_UnstallEnable & (1<<nEndPoint) )
		{
			flagUnstall = TRUE;
			pDEPCTL = &USB20OTG->DOEPCTL0;
		}
	}
	if( flagUnstall )
	{
		pDEPCTL = (volatile unsigned int*)( (unsigned int)pDEPCTL + 0x20*nEndPoint );
		USBDEV_DEBUG("unstall! ep%x",epAddress);
		BITSET(*pDEPCTL, Hw28);	// Set "PID 0" for interrupt/bulk, Set "even frame" for isochronous
		BITCLR(*pDEPCTL, Hw21);	// unstall
	}
	else
	{
		USBDEV_DEBUG("unstall disabled! ep%x",epAddress);
	}
}

static void OTGDEV_IO_EP_UnstallEnable(unsigned char epAddress, unsigned int enable)
{
	unsigned int nEndPoint = (unsigned int)(epAddress & 0x0F);
	unsigned short *pUnStallEnable;
	if( epAddress & 0x80 )
	{
		// IN
		pUnStallEnable = &gOTGDEV_IO_IN_EP_UnstallEnable;
	}
	else
	{
		// OUT
		pUnStallEnable = &gOTGDEV_IO_OUT_EP_UnstallEnable;
	}

	if( enable )
	{
		BITSET(*pUnStallEnable,1<<nEndPoint);
	}
	else
	{
		BITCLR(*pUnStallEnable,1<<nEndPoint);
	}
}

static unsigned int OTGDEV_IO_EP_IsStalled(unsigned char epAddress)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	unsigned int nEndPoint = (unsigned int)(epAddress & 0x0F);
	volatile unsigned int *pDEPCTL;
	if( epAddress & 0x80 )
	{
		// IN
		pDEPCTL = &USB20OTG->DIEPCTL0;
	}
	else
	{
		// OUT
		pDEPCTL = &USB20OTG->DOEPCTL0;
	}
	pDEPCTL = (volatile unsigned int*)( (unsigned int)pDEPCTL + 0x20*nEndPoint );
	if( (*pDEPCTL&Hw21) != 0 )	// stall
		return TRUE;
	else
		return FALSE;
}

static void OTGDEV_IO_EP_Flush(USBDEV_ENDPOINT_T *pEP)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;

	if(pEP->isIn)
	{
		// Flush TxFIFO
		USB20OTG->GRSTCTL |= GRSTCTL_TxFNum(pEP->index)|GRSTCTL_TxFFlsh;
		do
		{
			if( (USB20OTG->GRSTCTL&GRSTCTL_TxFFlsh) == 0 )
				break;
		} while(1);
	}
	else
	{
		// Flush RxFIFO
		USB20OTG->GRSTCTL |= GRSTCTL_RxFFlsh;
		do
		{
			if( (USB20OTG->GRSTCTL&GRSTCTL_RxFFlsh) == 0 )
				break;
		} while(1);
	}
}

static void OTGDEV_IO_SetAddress(unsigned short address)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	BITCSET(USB20OTG->DCFG, DCFG_DevAddr_MASK, DCFG_DevAddr(address));
}

static void OTGDEV_IO_TestMode(unsigned char value)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;

	// Sending status transaction of CONTROL TRANSFER for SetFeature
	OTGDEV_IO_EP0_Write(NULL,0);

	// It must wait the transfer complete
	while( USB20OTG->DIEPTSIZ0&(DIEPTSIZ0_PktCnt_MASK|DIEPTSIZ0_XferSize_MASK) )
	{
		if( USBDEV_IsConnected()==FALSE)
			return;
	}

	// And then, it can send the test packet
	if( 0 < value && value < 6 )
	{
		BITCSET(USB20OTG->DCTL, DCTL_TstCtl_MASK, DCTL_TstCtl(value));
	}
	else
	{
		// Reserved
		BITCLR(USB20OTG->DCTL, DCTL_TstCtl_MASK);
	}
}

#if defined(SLAVE_MODE_INCLUDE)
//#define FIFO_WRITE_TEST

//Bruce, 090507, 이 함수는 LISR에서 호출되며,
//Stack을 너무 많이 사용하여 Kernel의 Interrupt nesting을 위한 Buffer overflow가 발생한다.
//그래서 Global Variable로 선언한다.
static unsigned int packetBuf[DIEPTXFn_COMMON];

static unsigned int OTGDEV_IO_WriteFIFO(unsigned int nEndPoint)
{
	//Bruce, 090507, 이 함수는 LISR에서 호출되며,
	//Stack을 너무 많이 사용하여 Kernel의 Interrupt nesting을 위한 Buffer overflow가 발생한다.
	//그래서 Global Variable로 선언한다.
	//unsigned int packetBuf[DIEPTXFn_COMMON];

	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	void *pTxBuffer;
	volatile unsigned int *pTxPtr;
	unsigned int i,freeFIFOSize,remainedSize,maxPacketSize,transferSize,nTotalSize=0;
	unsigned char iFirstQ = gOTGDEV_IO_EpIn[nEndPoint].iFirstQ;
	PUSBDEV_QUEUE_T pFirstQ = &gOTGDEV_IO_EpIn[nEndPoint].queue[iFirstQ];

	if( nEndPoint == 0 )
		maxPacketSize = 64;
	else
		maxPacketSize = DOEPCTL_MPS_Read(USB20OTG->DEVINENDPT[nEndPoint-1][DIEPCTL_INDEX]);

	#ifndef FIFO_WRITE_TEST
	for(;;)
	#endif
	{
		pTxBuffer = (void*)&pFirstQ->buff[pFirstQ->actualLen];
		if( nEndPoint == 0 )
		{
			remainedSize = DIEPTSIZ0_XferSize_Read(USB20OTG->DIEPTSIZ0);
			freeFIFOSize = USB20OTG->DTXFSTS0<<2;
		}
		else
		{
			remainedSize = DIEPTSIZ_XferSize_Read(USB20OTG->DEVINENDPT[nEndPoint-1][DIEPTSIZ_INDEX]);
			freeFIFOSize = USB20OTG->DEVINENDPT[nEndPoint-1][DTXFSTS_INDEX]<<2;
		}

		#ifndef FIFO_WRITE_TEST
		transferSize = min(maxPacketSize,remainedSize);
		#else
		transferSize = min(freeFIFOSize,remainedSize);
		#endif

		if( freeFIFOSize < transferSize || transferSize == 0 )
			#ifndef FIFO_WRITE_TEST
			break;
			#else
			return nTotalSize;
			#endif

		if( ISZERO(pTxBuffer, 0x3) )
		{
			pTxPtr = (unsigned int*)pTxBuffer;
		}
		else
		{
			_loc_memcpy((void*)packetBuf,pTxBuffer,transferSize);
			pTxPtr = packetBuf;
		}

		for( i = 0 ; i < transferSize ; i+=4 )
		{
			USB20OTG->DFIFOENDPT[nEndPoint][0] = *pTxPtr++;
		}

		pFirstQ->actualLen += transferSize;
		nTotalSize += transferSize;
	}

	return nTotalSize;
}
#endif

static void OTGDEV_IO_EP_IN_Isr(unsigned int nEndPoint)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	unsigned int DIEPINT_masked;
	unsigned int DIEPINT_mask;
	volatile unsigned int *pDIEPINT;
	if( nEndPoint==0 )
	{
		pDIEPINT = &USB20OTG->DIEPINT0;
	}
	else
	{
		pDIEPINT = &USB20OTG->DEVINENDPT[nEndPoint-1][DIEPINT_INDEX];
	}

	DIEPINT_mask = USB20OTG->DIEPMSK;
	#if defined(SLAVE_MODE_INCLUDE)
	DIEPINT_mask |= (USB20OTG->DIEPEMPMSK>>nEndPoint&0x1)<<7;
	#endif
	DIEPINT_masked = (*pDIEPINT)&DIEPINT_mask;

	// Transfer Complete
	if(DIEPINT_masked&DIEPINT_XferCompl)
	{
		unsigned char iFirstQ = gOTGDEV_IO_EpIn[nEndPoint].iFirstQ;
		PUSBDEV_QUEUE_T pFirstQ = &gOTGDEV_IO_EpIn[nEndPoint].queue[iFirstQ];

		#if defined(SLAVE_MODE_INCLUDE)
		/* Disable the Tx FIFO Empty Interrupt for this EP */
		BITCLR(USB20OTG->DIEPEMPMSK,DIEPEMPMSK_InEpTxfEmpMsk(nEndPoint));

		if( pFirstQ->actualLen >= pFirstQ->xferLen )
		{
			pFirstQ->state = QUEUE_STATE_DONE;
			if( OTGDEV_IO_EP_IN_StartNextTransfer(nEndPoint) == FALSE )
				gOTGDEV_IO_EpIn[nEndPoint].xferState = XFER_STATE_IDLE;
		}
		else
		{
			OTGDEV_IO_EP_IN_StartTransfer(nEndPoint);
		}
		#elif defined(DMA_MODE_INCLUDE)
		pFirstQ->state = QUEUE_STATE_DONE;
		if( OTGDEV_IO_EP_IN_StartNextTransfer(nEndPoint) == FALSE )
			gOTGDEV_IO_EpIn[nEndPoint].xferState = XFER_STATE_IDLE;
		#endif

		*pDIEPINT = DIEPINT_XferCompl;	// clear interrupt
	}

	// Endpoint Disabled
	if(DIEPINT_masked&DIEPINT_EPDisabld)
	{
		// flush end point
		GRSTCTL_TxFNum_Write(USB20OTG->GRSTCTL,nEndPoint);
		BITSET(USB20OTG->GRSTCTL, GRSTCTL_TxFFlsh);

		*pDIEPINT = DIEPINT_EPDisabld;	// clear interrupt
	}

	// IN Token received with TX FIFO Empty
	//if(DIEPINT_masked&DIEPINT_INTknTXFEmp)
	//{
	//	*pDIEPINT = DIEPINT_INTknTXFEmp;
	//}

	#if defined(SLAVE_MODE_INCLUDE)
	// TX FIFO Empty
	if(DIEPINT_masked&DIEPINT_TxFEmp_MASK)
	{
		if( OTGDEV_IO_WriteFIFO(nEndPoint) == 0 )
		{
			/* Disable the Tx FIFO Empty Interrupt for this EP */
			BITCLR(USB20OTG->DIEPEMPMSK,DIEPEMPMSK_InEpTxfEmpMsk(nEndPoint));
		}
	}
	#endif
}

//#if defined(DMA_MODE_INCLUDE)
static void OTGDEV_IO_EP_OUT_Isr(unsigned int nEndPoint)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	unsigned int DOEPINT_masked;
	unsigned int DOEPINT_mask;
	volatile unsigned int *pDOEPINT;
	if( nEndPoint==0 )
	{
		pDOEPINT = &USB20OTG->DOEPINT0;
	}
	else
	{
		pDOEPINT = &USB20OTG->DEVOUTENDPT[nEndPoint-1][DOEPINT_INDEX];
	}

	DOEPINT_mask = USB20OTG->DOEPMSK;
	DOEPINT_masked = (*pDOEPINT)&DOEPINT_mask;

	// Transfer Complete
	if(DOEPINT_masked&DOEPINT_XferCompl)
	{
		unsigned char iLastQ = gOTGDEV_IO_EpOut[nEndPoint].iLastQ;
		unsigned char iLastQ_Next = ((iLastQ+1) >= gOTGDEV_IO_EpOut[nEndPoint].bQueueNum)? 0 : (iLastQ+1);
		PUSBDEV_QUEUE_T pLastQ = &gOTGDEV_IO_EpOut[nEndPoint].queue[iLastQ];

		#ifdef DMA_MODE_INCLUDE
		pLastQ->actualLen = pLastQ->xferLen
						- DOEPTSIZ_XferSize_Read(HwUSB20OTG->DEVOUTENDPT[nEndPoint-1][DOEPTSIZ_INDEX]);
		#endif
		pLastQ->state = QUEUE_STATE_DONE;
		gOTGDEV_IO_EpOut[nEndPoint].xferState = XFER_STATE_IDLE;
		gOTGDEV_IO_EpOut[nEndPoint].iLastQ = iLastQ_Next;
		USBDEV_DEBUG("EP OUT Isr");
		OTGDEV_IO_EP_OUT_StartNextTransfer(nEndPoint);

		*pDOEPINT = DOEPINT_XferCompl;	// clear interrupt
	}
}
//#endif

static void OTGDEV_IO_Isr(void)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	#if defined(SLAVE_MODE_INCLUDE)
	unsigned int gintsts = USB20OTG->GINTSTS;
	if( gintsts & GINTSTS_USBRst )
	{
		USBDEV_Reset();
		USB20OTG->GINTSTS = GINTSTS_USBRst;
	}
	if( gintsts & GINTSTS_EnumDone )
	{
		OTGDEV_IO_EnumDone();
		USB20OTG->GINTSTS = GINTSTS_EnumDone;
	}
	if( gintsts & GINTSTS_RxFLvl_MASK )
	{
		volatile unsigned int GRXSTSR = USB20OTG->GRXSTSR;
		PGRXSTSP_DEV_T pGRXSTSR = (PGRXSTSP_DEV_T)&GRXSTSR;
		//unsigned int PktSts = pGRXSTSR->PktSts;
		unsigned int DWCnt = (pGRXSTSR->BCnt+3)>>2;
		unsigned int EPNum = pGRXSTSR->EPNum;
		//unsigned int i=0;

		// Token
		if( DWCnt==0 )
		{
			volatile unsigned int GRXSTSP = USB20OTG->GRXSTSP;	// pop for read from FIFO
			BITSET(USB20OTG->GINTMSK,GINTMSK_RxFLvlMsk_UNMASK);
			return;
		}

		if( EPNum == 0 )
		{
			volatile unsigned int GRXSTSP = USB20OTG->GRXSTSP;	// pop for read from FIFO
			unsigned int actualLen = gOTGDEV_IO_EpOut[0].queue[0].actualLen;
			unsigned int *pBuff = (unsigned int*)&gOTGDEV_IO_EpOut[0].queue[0].buff[actualLen];
			while( DWCnt-- > 0 )
			{
				if( actualLen < gOTGDEV_IO_EpOut[0].queue[0].buffSize )
				{
					*pBuff++ = USB20OTG->DFIFOENDPT[0][0];
					actualLen += 4;
				}
				else
				{
					volatile unsigned int dump = USB20OTG->DFIFOENDPT[0][0];
				}
			}
			USBDEV_DEBUG("ISR EP0 BCnt %d, PktSts %d",pGRXSTSR->BCnt,pGRXSTSR->PktSts);
		}
		else
		{
			if( pGRXSTSR->BCnt != 0 && pGRXSTSR->PktSts == 2 )
			{
				unsigned char iLastQ = gOTGDEV_IO_EpOut[EPNum].iLastQ;
				PUSBDEV_QUEUE_T pLastQ = &gOTGDEV_IO_EpOut[EPNum].queue[iLastQ];
				USBDEV_DEBUG("isr:rx:ep%d iLastQ=%d\n",EPNum,iLastQ);
				if( pLastQ->state == QUEUE_STATE_ACTIVATE )
				{
					volatile unsigned int GRXSTSP = USB20OTG->GRXSTSP;	// pop for read from FIFO
					unsigned int *pBuffer = (unsigned int*)&(pLastQ->buff[pLastQ->actualLen]);
					USBDEV_DEBUG("queued:");
					// lastQueue.buff must aligned to 32-bit word, size have to be multiple of 4
					if( pLastQ->buffSize-pLastQ->actualLen >= (DWCnt<<2) && pBuffer!=NULL )
					{
						while( DWCnt-- > 0 )
						{
							*pBuffer++ = USB20OTG->DFIFOENDPT[EPNum][0];
						}
						pLastQ->actualLen += pGRXSTSR->BCnt;
					}
					else
					{
						while( DWCnt-- > 0 )
						{
							volatile unsigned int dump = USB20OTG->DFIFOENDPT[EPNum][0];
						}
					}
					//if( pLastQ->actualLen >= pLastQ->xferLen || pGRXSTSR->BCnt != gOTGDEV_IO_EpOut[EPNum].MPS )
					//{
					//	unsigned char iLast_Next;
					//
					//	USBDEV_DEBUG(" done");
					//
					//	pLastQ->state = QUEUE_STATE_DONE;
					//	gOTGDEV_IO_EpOut[EPNum].xferState = XFER_STATE_IDLE;
					//
					//	iLast_Next = (gOTGDEV_IO_EpOut[EPNum].iLastQ + 1) & QUEUE_INDEX_MASK;
					//	gOTGDEV_IO_EpOut[EPNum].iLastQ = iLast_Next;
					//
					//	USBDEV_DEBUG("  iLast_Next=%d\n",iLast_Next);
					//
					//	OTGDEV_IO_EP_OUT_StartNextTransfer(EPNum);
					//}
					//else
					//	USB20OTG->DEVOUTENDPT[EPNum-1][DOEPCTL_INDEX] |= DOEPCTL_CNAK;
				}
				else
				{
					USBDEV_ERROR("transfer failure");
					//USBDEV_DEBUG("xfer state wait");
					//gOTGDEV_IO_EpOut[EPNum].xferWaitPopSize = pGRXSTSR->BCnt;
					//gOTGDEV_IO_EpOut[EPNum].xferState = XFER_STATE_WAIT_POP;
					//BITCLR(USB20OTG->GINTMSK, GINTMSK_RxFLvlMsk_UNMASK);
				}
			}
		}
		//USB20OTG->GINTSTS = GINTSTS_RxFLvl_MASK;
	}
	if( gintsts & GINTSTS_OEPInt_MASK )
	{
		unsigned int i;
		unsigned int OUT_EpInt = DAINT_OutEPInt_Read(USB20OTG->DAINT);
		if( OUT_EpInt & (1<<0) )
		{
			if( USB20OTG->DOEPINT0 & DOEPINT_SetUP )
			{
				USBDEV_DEBUG("OEPInt SetUP");
				USBDEV_EP0_Setup((USB_DEVICE_REQUEST_T*)gOTGDEV_IO_EpOut[0].queue[0].buff);

				USB20OTG->DOEPINT0 = DOEPINT_SetUP;
			}
			if( USB20OTG->DOEPINT0 & DOEPINT_XferCompl )
			{
				USBDEV_DEBUG("OEPInt XferCompl");
				USB20OTG->DOEPINT0 = DOEPINT_XferCompl;
			}
			USB20OTG->DOEPTSIZ0 = DOEPTSIZ0_SUPCnt(3)|DOEPTSIZ0_PktCnt(1)|DOEPTSIZ0_XferSize(8);
			USB20OTG->DOEPCTL0 |= DOEPCTL0_EPEna|DOEPCTL0_CNAK;
		}
		for(i=1;i<16;i++)
		{
			volatile unsigned int DAINT_masked = USB20OTG->DAINT & USB20OTG->DAINTMSK;
			if( DAINT_masked & (0x10000<<i) )
			{
				OTGDEV_IO_EP_OUT_Isr(i);
			}
		}
		USB20OTG->GINTSTS = GINTSTS_OEPInt_MASK;
	}
	if( gintsts & GINTSTS_IEPInt_MASK )
	{
		unsigned int i;

		for(i=0;i<16;i++)
		{
			volatile unsigned int DAINT_masked = USB20OTG->DAINT & USB20OTG->DAINTMSK;
			if( DAINT_masked & (1<<i) )
			{
				OTGDEV_IO_EP_IN_Isr(i);
			}
		}
	}
	#elif defined(DMA_MODE_INCLUDE)
	unsigned int gintsts = USB20OTG->GINTSTS;
	if( gintsts & GINTSTS_USBRst )
	{
		USBDEV_Reset();
		USB20OTG->GINTSTS = GINTSTS_USBRst;
	}
	if( gintsts & GINTSTS_EnumDone )
	{
		OTGDEV_IO_EnumDone();
		USB20OTG->GINTSTS = GINTSTS_EnumDone;
	}
	if( gintsts & GINTSTS_OEPInt_MASK )
	{
		unsigned int i;
		unsigned int OUT_EpInt = DAINT_OutEPInt_Read(HwUSB20OTG->DAINT);
		if( OUT_EpInt & (1<<0) )
		{
			if( USB20OTG->DOEPINT0 & DOEPINT_SetUP )
			{
				USBDEV_EP0_Setup((USB_DEVICE_REQUEST_T*)gOTGDEV_IO_EpOut[0].queue[0].buff);

				USB20OTG->DOEPINT0 = DOEPINT_SetUP;
			}
			if( USB20OTG->DOEPINT0 & DOEPINT_XferCompl )
			{
				USB20OTG->DOEPINT0 = DOEPINT_XferCompl;
			}

			USB20OTG->DOEPTSIZ0 = DOEPTSIZ0_SUPCnt(3)|DOEPTSIZ0_PktCnt(1)|DOEPTSIZ0_XferSize(8);
			USB20OTG->DOEPDMA0 = (unsigned int)gOTGDEV_IO_EpOut[0].queue[0].buff;
			USB20OTG->DOEPCTL0 |= DOEPCTL0_EPEna|DOEPCTL0_CNAK;
		}
		for(i=1;i<16;i++)
		{
			volatile unsigned int DAINT_masked = USB20OTG->DAINT & USB20OTG->DAINTMSK;
			if( DAINT_masked & (0x10000<<i) )
			{
				OTGDEV_IO_EP_OUT_Isr(i);
			}
		}
		USB20OTG->GINTSTS = GINTSTS_OEPInt_MASK;
	}
	if( gintsts & GINTSTS_IEPInt_MASK )
	{
		unsigned int i;

		for(i=0;i<16;i++)
		{
			volatile unsigned int DAINT_masked = USB20OTG->DAINT & USB20OTG->DAINTMSK;
			if( DAINT_masked & (1<<i) )
			{
				OTGDEV_IO_EP_IN_Isr(i);
			}
		}
	}
	#endif
}

static unsigned int OTGDEV_IO_EP_Enable(USBDEV_ENDPOINT_T *pEP, unsigned int enable)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
#if defined(SLAVE_MODE_INCLUDE)
	unsigned int DIOEPCTL;

	if( pEP->index == 0
			|| (pEP->isIn != 0 && pEP->index > IN_EP_NUMBER_COMMON) )
		return FALSE;

	if( enable )
	{
		DIOEPCTL=DIOEPCTL_SetD0PID|DIOEPCTL_USBActEP|DIOEPCTL_MPS(pEP->maximumPacketSize);

		switch( pEP->transferType )
		{
			case USBDEV_EP_TRANSFER_TYPE_CONTROL:
				DIOEPCTL |= DIOEPCTL_EPType_CONTROL;
				break;
			case USBDEV_EP_TRANSFER_TYPE_ISOCHRONOUS:
				DIOEPCTL |= DIOEPCTL_EPType_ISOCHRONOUS;
				break;
			case USBDEV_EP_TRANSFER_TYPE_BULK:
				DIOEPCTL |= DIOEPCTL_EPType_BULK;
				break;
			case USBDEV_EP_TRANSFER_TYPE_INTERRUPT:
				DIOEPCTL |= DIOEPCTL_EPType_INTERRUPT;
				break;
		}

		if( pEP->isIn )
		{
			// IN
			//HwUSB20OTG->DIEPEMPMSK |= 1<<pEP->index;
			BITSET(USB20OTG->DAINTMSK,DAINTMSK_InEPInt(pEP->index));
			USB20OTG->DEVINENDPT[pEP->index-1][DIEPCTL_INDEX] = DIOEPCTL|DIEPCTL_TxFNum(pEP->index);

			gOTGDEV_IO_EpIn[pEP->index].MPS = pEP->maximumPacketSize;

			USBDEV_DEBUG("OTGDEV_IO EP IN %d Enable MPS=%d\n",pEP->index,pEP->maximumPacketSize);
		}
		else
		{
			// OUT
			BITSET(USB20OTG->DAINTMSK,DAINTMSK_OutEPInt(pEP->index));
			USB20OTG->DEVOUTENDPT[pEP->index-1][DOEPCTL_INDEX] = DIOEPCTL/*|DOEPCTL_EPEna|DOEPCTL_CNAK*/;

			gOTGDEV_IO_EpOut[pEP->index].MPS = pEP->maximumPacketSize;

			USBDEV_DEBUG("OTGDEV_IO EP OUT %d Enable MPS=%d\n",pEP->index,pEP->maximumPacketSize);
		}

		return TRUE;
	}
	else
	{
		if( pEP->isIn )
		{
			_loc_Disable_IN_EP(pEP->index);
			BITCLR(USB20OTG->DAINTMSK,DAINTMSK_InEPInt(pEP->index));
			USBDEV_DEBUG("OTGDEV_IO EP IN %d Disable\n",pEP->index);
		}
		else
		{
			USBDEV_DEBUG("OTGDEV_IO EP IN %d Disable\n",pEP->index);
		}
	}

	return TRUE;
#elif defined(DMA_MODE_INCLUDE)
	unsigned int DIOEPCTL;

	if( pEP->index == 0 )
		return FALSE;

	if( enable )
	{
		DIOEPCTL=DIOEPCTL_SetD0PID|DIOEPCTL_USBActEP|DIOEPCTL_MPS(pEP->maximumPacketSize);

		switch( pEP->transferType )
		{
			case USBDEV_EP_TRANSFER_TYPE_CONTROL:
				DIOEPCTL |= DIOEPCTL_EPType_CONTROL;
				break;
			case USBDEV_EP_TRANSFER_TYPE_ISOCHRONOUS:
				DIOEPCTL |= DIOEPCTL_EPType_ISOCHRONOUS;
				break;
			case USBDEV_EP_TRANSFER_TYPE_BULK:
				DIOEPCTL |= DIOEPCTL_EPType_BULK;
				break;
			case USBDEV_EP_TRANSFER_TYPE_INTERRUPT:
				DIOEPCTL |= DIOEPCTL_EPType_INTERRUPT;
				break;
		}

		if( pEP->isIn )
		{
			// IN
			BITSET(USB20OTG->DAINTMSK,DAINTMSK_InEPInt(pEP->index));
			HwUSB20OTG->DEVINENDPT[pEP->index-1][DIEPCTL_INDEX] = DIOEPCTL|DIEPCTL_TxFNum(pEP->index);

			gOTGDEV_IO_EpIn[pEP->index].MPS = pEP->maximumPacketSize;

			USBDEV_DEBUG("OTGDEV_IO EP IN %d Enable MPS=%d\n",pEP->index,pEP->maximumPacketSize);
		}
		else
		{
			// OUT
			BITSET(USB20OTG->DAINTMSK,DAINTMSK_OutEPInt(pEP->index));
			USB20OTG->DEVOUTENDPT[pEP->index-1][DOEPCTL_INDEX] = DIOEPCTL;

			gOTGDEV_IO_EpOut[pEP->index].MPS = pEP->maximumPacketSize;

			USBDEV_DEBUG("OTGDEV_IO EP OUT %d Enable MPS=%d\n",pEP->index,pEP->maximumPacketSize);
		}

		return TRUE;
	}
	else
	{
		if( pEP->isIn )
		{
			_loc_Disable_IN_EP(pEP->index);
			BITCLR(USB20OTG->DAINTMSK,DAINTMSK_InEPInt(pEP->index));
			USBDEV_DEBUG("OTGDEV_IO EP IN %d Disable\n",pEP->index);
		}
		else
		{
			BITCLR(USB20OTG->DAINTMSK,DAINTMSK_OutEPInt(pEP->index));
			USBDEV_DEBUG("OTGDEV_IO EP IN %d Disable\n",pEP->index);
		}
	}

	return TRUE;
#endif
}

//unsigned int OTGDEV_IO_WaitForOutReady( USBDEV_ENDPOINT_T *pEP )
//{
//	unsigned char		cRet;
//	unsigned long int	t,k;
//	unsigned long 		lSaveESR;
//
//	HwIR = (unsigned long)pEP->index;
//
//	cRet = 0;
//
//	for ( t = 0; t < OTGDEV_IO_TIMEOUT_RECV && !cRet; t++ )
//	{
//		if ( USBDEV_IsConnected() == FALSE )
//			break;
//
//		for ( k = 0; k < 100 && !cRet; k++ )
//		{
//			lSaveESR = (HwESR&(Hw3+Hw2));
//
//			#ifdef USE_BULK_OUT_DUAL_FIFO
//			if ((lSaveESR == Hw2 ) || ( lSaveESR == Hw3 ))	// One Packet or Two Packet in FIFO
//			#else
//			if ( lSaveESR == Hw2 )							// One Packet in FIFO
//			#endif
//			{
//				cRet = 1;
//			}
//		}
//	}
//
//	return cRet;
//}

//unsigned int OTGDEV_IO_WaitForInReady( USBDEV_ENDPOINT_T *pEP )
//{
//	unsigned char		cRet;
//	unsigned long int	t,k;
//	unsigned long 		lSaveESR;
//	unsigned int		dwConfirmCount;
//
//	HwIR = (unsigned long)pEP->index;
//
//	cRet = 0;
//	dwConfirmCount = 0;
//
//	for ( t = 0; t < OTGDEV_IO_TIMEOUT_SEND && !cRet; t++ )
//	{
//		if ( USBDEV_IsConnected() == FALSE )
//			break;
//
//		for ( k = 0; k < 100 && !cRet; k++ )
//		{
//			lSaveESR = (HwESR&(Hw3+Hw2));
//
//			#ifdef USE_BULK_IN_DUAL_FIFO
//			if ((lSaveESR == 0x00 ) || ( lSaveESR == Hw2 ))	// No Packet or One Packet in FIFO
//			#else
//			if ( lSaveESR == 0x00 )							// No Packet in FIFO
//			#endif
//			{
//				//cRet = 1;
//				++dwConfirmCount;
//			}
//			else
//			{
//				dwConfirmCount = 0;
//			}
//
//			if ( dwConfirmCount > 20 )
//			{
//				cRet = 1;
//			}
//		}
//	}
//
//	return cRet;
//}

static unsigned int OTGDEV_IO_EP_SetQueue(USBDEV_ENDPOINT_T *pEP, unsigned int size);
static unsigned int OTGDEV_IO_EP_IsReady(USBDEV_ENDPOINT_T *pEP)
{
	if( pEP->index == 0 )
		return FALSE;

	if( pEP->isIn == TRUE )
	{
		if( gOTGDEV_IO_EpIn[pEP->index].xferState == XFER_STATE_IDLE )
			return TRUE;
	}
	else
	{
		//#if defined(SLAVE_MODE_INCLUDE)
		//if( gOTGDEV_IO_EpOut[pEP->index].xferState == XFER_STATE_WAIT_POP )
		//{
		//	USBDEV_DEBUG("ready\n");
		//	return gOTGDEV_IO_EpOut[pEP->index].xferWaitPopSize;
		//}
		//#elif defined(DMA_MODE_INCLUDE)
		if( gOTGDEV_IO_EP_OUT_QueuedFlag_for_IsReady[pEP->index] == TRUE )
		{
			unsigned char iFirstQ = gOTGDEV_IO_EpOut[pEP->index].iFirstQ;
			PUSBDEV_QUEUE_T pFirstQ = &gOTGDEV_IO_EpOut[pEP->index].queue[iFirstQ];
			if( pFirstQ->state == QUEUE_STATE_DONE )
				return pFirstQ->actualLen;
		}
		else
		{
			OTGDEV_IO_EP_SetQueue(pEP,gOTGDEV_IO_EpOut[pEP->index].MPS);
			gOTGDEV_IO_EP_OUT_QueuedFlag_for_IsReady[pEP->index] = TRUE;
		}
		//#endif
	}

	return FALSE;
}

static PUSBDEV_QUEUE_T OTGDEV_IO_EP_GetQueue(USBDEV_ENDPOINT_T *pEP)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	if( pEP->isIn )
	{
		unsigned char iLastQ = gOTGDEV_IO_EpIn[pEP->index].iLastQ;
		unsigned char iLastQ_Next = ((iLastQ+1) >= gOTGDEV_IO_EpIn[pEP->index].bQueueNum)? 0 : (iLastQ+1);

		while( iLastQ_Next == gOTGDEV_IO_EpIn[pEP->index].iFirstQ )
		{
			if(USBDEV_IsConnected() == FALSE)
				return NULL;
		}

		USBDEV_DEBUG("GetQueue IN EP%d iLastQ=%d\n",pEP->index,iLastQ);

		return &gOTGDEV_IO_EpIn[pEP->index].queue[iLastQ];
	}
	else
	{
		unsigned char iFirstQ = gOTGDEV_IO_EpOut[pEP->index].iFirstQ;
		PUSBDEV_QUEUE_T pFirstQ = &gOTGDEV_IO_EpOut[pEP->index].queue[iFirstQ];
		while( pFirstQ->state == QUEUE_STATE_ACTIVATE )
		{
			if(USBDEV_IsConnected() == FALSE)
				return NULL;
		}
		if( pFirstQ->state == QUEUE_STATE_DONE )
		{
			USBDEV_DEBUG("GetQueue OUT EP%d iFirstQ=%d size=%d\n",pEP->index,iFirstQ,pFirstQ->actualLen);
			if( gOTGDEV_IO_EpOut[pEP->index].iPrevFirstQ < gOTGDEV_IO_EpOut[pEP->index].bQueueNum )
			{
				unsigned char iPrevFirstQ = gOTGDEV_IO_EpOut[pEP->index].iPrevFirstQ;
				PUSBDEV_QUEUE_T pPrevFirstQ = &gOTGDEV_IO_EpOut[pEP->index].queue[iPrevFirstQ];
				pPrevFirstQ->actualLen = 0;
				pPrevFirstQ->state = QUEUE_STATE_IDLE;
				USBDEV_DEBUG("DelQueue iPrevFirstQ=%d",iPrevFirstQ);
				OTGDEV_IO_Lock();
				OTGDEV_IO_EP_OUT_StartNextTransfer(pEP->index);
				OTGDEV_IO_Unlock();
			}
			gOTGDEV_IO_EpOut[pEP->index].iPrevFirstQ = iFirstQ;
			gOTGDEV_IO_EpOut[pEP->index].iFirstQ = ((iFirstQ+1) >= gOTGDEV_IO_EpOut[pEP->index].bQueueNum)? 0 : (iFirstQ+1);
			return pFirstQ;
		}
	}
	return NULL;
}

static unsigned int OTGDEV_IO_EP_SetQueue(USBDEV_ENDPOINT_T *pEP, unsigned int size)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	if( pEP->isIn )
	{
		unsigned char iLastQ = gOTGDEV_IO_EpIn[pEP->index].iLastQ;
		unsigned char iLastQ_Next = ((iLastQ+1) >= gOTGDEV_IO_EpIn[pEP->index].bQueueNum) ? 0 : (iLastQ+1);
		if( iLastQ_Next != gOTGDEV_IO_EpIn[pEP->index].iFirstQ )
		{
			PUSBDEV_QUEUE_T pLastQ = &gOTGDEV_IO_EpIn[pEP->index].queue[iLastQ];
			unsigned int xferLen = min(size,pLastQ->buffSize);

			OTGDEV_IO_Lock();

			pLastQ->xferLen = xferLen;
			pLastQ->actualLen = 0;
			pLastQ->state = QUEUE_STATE_ACTIVATE;
			gOTGDEV_IO_EpIn[pEP->index].iLastQ = iLastQ_Next;

			USBDEV_DEBUG("SetQueue IN EP%d xferLen=%d iLastQ_Next=%d",pEP->index,xferLen,iLastQ_Next);

			if(gOTGDEV_IO_EpIn[pEP->index].xferState != XFER_STATE_WORKING)
			{
				gOTGDEV_IO_EpIn[pEP->index].xferState = XFER_STATE_WORKING;
				USBDEV_DEBUG("  -> start transfer\n");
				OTGDEV_IO_EP_IN_StartTransfer(pEP->index);
			}

			OTGDEV_IO_Unlock();

			return xferLen;
		}
	}
	else
	{
		//#if defined(DMA_MODE_INCLUDE)
		if( gOTGDEV_IO_EP_OUT_QueuedFlag_for_IsReady[pEP->index] == TRUE )
		{
			unsigned char iFirstQ = gOTGDEV_IO_EpOut[pEP->index].iFirstQ;
			PUSBDEV_QUEUE_T pFirstQ = &gOTGDEV_IO_EpOut[pEP->index].queue[iFirstQ];

			gOTGDEV_IO_EP_OUT_QueuedFlag_for_IsReady[pEP->index] = FALSE;
			return min(pFirstQ->xferLen,size);
		}
		//#endif
		{
			OTGDEV_IO_Lock();
			gOTGDEV_IO_EpOut[pEP->index].xferLeftLength += size;
			if(!size)
				gOTGDEV_IO_EpOut[pEP->index].bZeroPacket = 1;
			OTGDEV_IO_EP_OUT_StartNextTransfer(pEP->index);
			OTGDEV_IO_Unlock();
			return size;
		}
	}
	return 0;
}

static USBDEV_ERROR_T OTGDEV_IO_GetLastError(void)
{
	return sOTGDEV_IO_LastError;
}


static USBDEVICE_IO_DRIVER_T s_OTGDEV_IO_Driver = {
	OTGDEV_IO_C_Version,
	USBDEV_FULL_SPEED,
	OTGDEV_IO_AllocQueue,
	OTGDEV_IO_IsConnected,
	OTGDEV_IO_Init,
	OTGDEV_IO_Reset,
	OTGDEV_IO_EP_Stall,
	OTGDEV_IO_EP_Unstall,
	OTGDEV_IO_EP_UnstallEnable,
	OTGDEV_IO_EP_IsStalled,
	OTGDEV_IO_EP_Flush,
	OTGDEV_IO_EP0_Read,
	OTGDEV_IO_EP0_Write,
	OTGDEV_IO_SetAddress,
	OTGDEV_IO_TestMode,
	OTGDEV_IO_Isr,
	OTGDEV_IO_EP_Enable,
	OTGDEV_IO_EP_IsReady,
	OTGDEV_IO_EP_GetQueue,
	OTGDEV_IO_EP_SetQueue,
	OTGDEV_IO_GetLastError
};

USBDEVICE_IO_DRIVER_T *OTGDEV_IO_GetDriver(void)
{
	return &s_OTGDEV_IO_Driver;
}
