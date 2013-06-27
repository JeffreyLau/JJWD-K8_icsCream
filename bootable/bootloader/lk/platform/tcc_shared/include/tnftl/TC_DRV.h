/****************************************************************************
 *   FileName    : TC_DRV.h
 *   Description :
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/

#ifndef	__TC_DRV_H__
#define	__TC_DRV_H__

#ifndef	__IO_TCCXXX_H
#if defined(_LINUX_)
#include <tnftl/IO_TCCXXX.h>
#elif defined(_WINCE_)
#include "IO_TCCXXX.h"
#endif
#endif

//#include "TCCresource.h"

/*==============================================================================
				General DMA
   ==============================================================================*/

#define	DRV_GDMA_MAX_HANDLE		27

// GDMA Driver Function Definition
enum
{
	DRV_GDMA_FUNC_INIT,				// Driver Initialization : Argument = None
	DRV_GDMA_FUNC_PROCESS_INTERRUPT,	// Process DMA Interrupt : Argument = None

	DRV_GDMA_FUNC_OPEN,				// Open Handle : Argument = (Channel Number, *sDRV_GDMA)
	DRV_GDMA_FUNC_INSTALL_HANDLER,	// Install DMA Interrupt Handler : Argument = (Channel, NewHandler, [* OldHandler])
	DRV_GDMA_FUNC_UNINSTALL_HANDLER,	// UnInstall DMA Interrupt Handler : Argument = (Channel)

	DRV_GDMA_FUNC_SETCFG,				// Configuration for DMA : Argument = (Handle, ConfigValue, ReqSel)
	DRV_GDMA_FUNC_PARSECFG,			// Parse for DMA Configuration : parameter should be set ahead in structure : Argument = (Handle)
	DRV_GDMA_FUNC_GETHwDMA,			// Get HwDMA Base Address : Argument = (Handle, *sHwDMA)
	DRV_GDMA_FUNC_ISACTIVE,			// Check Activated State : Argument = (Handle)
	DRV_GDMA_FUNC_SETREG,				// Setup DMA Register : Argument = (Handle, SrcBase, SrcInc, SrcMask, DstBase, DstInc, DstMask, DataSize)
	DRV_GDMA_FUNC_START,				// Start DMA Transfer : Argument = (Handle, SrcBase, SrcInc, SrcMask, DstBase, DstInc, DstMask, DataSize)
	DRV_GDMA_FUNC_WAITDONE,			// Wait DMA Transfer Done : Argument = (Handle, [TimeOut Value], [TimeDelay(), TimeDelayFactor])
	DRV_GDMA_FUNC_WAITDONE_STOP,		// Wait DMA Transfer Done & Stop DMA : Argument = (Handle, [TimeOut Value], [TimeDelay(), TimeDelayFactor])
	DRV_GDMA_FUNC_PAUSE,				// Pause DMA Operation : Argument = (Handle)
	DRV_GDMA_FUNC_CONTINUE,			// Continue DMA Operation : Argument = (Handle)
	DRV_GDMA_FUNC_STOP,				// Stop DMA Operation -> This channel goes to IDLE state : Argument = (Handle)
	DRV_GDMA_FUNC_CLOSE,				// Close DMA Channel from User : Argument = (Handle)
	DRV_GDMA_FUNC_MAX
};

// GDMA Driver Status Definition
enum
{
	DRV_GDMA_STATUS_INVALID,	// Not Opened
	DRV_GDMA_STATUS_IDLE,		// Openad but Not Activated
	DRV_GDMA_STATUS_PAUSE,	// Opened & Activated & Pause
	DRV_GDMA_STATUS_ACTIVE,	// Opened & Activated
	DRV_GDMA_STATUS_MAX
};

// GDMA Driver Data Structure
typedef struct
{
	U32			CHCFG;
	U32			REQSEL;

	U8			CHSTS;
	U8			BufShiftFactor;
	U32			HwCHCTRL;
	U32			HwREQSEL;
	PGDMACTRL	pHwDMA;
} sDRV_GDMA;

// GDMA Driver Return Value Definition
#define	DRV_GDMA_ERROR_YES	1
#define	DRV_GDMA_ERROR_NO		0
#define	DRV_GDMA_ERROR_OK		0
enum
{
	// Error for Driver State
	DRV_GDMA_ERROR_NOTINIT	= (int)0xF0040000,	// Non-init Function come before init function
	DRV_GDMA_ERROR_DUPINIT,					// Init function called more than twice

	// Error for Input Parameter
	DRV_GDMA_ERROR_INVALID_FUNC,				// Function Code is out of range
	DRV_GDMA_ERROR_INVALID_ARGUMENT,			// Number of Argument is out of range
	DRV_GDMA_ERROR_INVALID_HANDLE,			// Handle value is out of range
	DRV_GDMA_ERROR_INVALID_CH,				// Channel value is out of range

	// Error for Resource Availability
	DRV_GDMA_ERROR_NOTAVAILABLE_HANDLE,		// No available handle
	DRV_GDMA_ERROR_NOTAVAILABLE_CH,			// No available channel

	// Error for Operating
	DRV_GDMA_ERROR_TIMEOUT,					// Time Out
	DRV_GDMA_ERROR_NOT_READY,
	DRV_GDMA_ERROR_NOT_ACTIVE,
	DRV_GDMA_ERROR_NOT_PAUSE,

	// Error for Illegal State
	DRV_GDMA_ERROR_INTERNAL,					// Internal Bug

	DRV_GDMA_ERROR_MAX
};

// GDMA Driver Channel Configuration Definition
#define	DRV_GDMA_CFG_SyncHwReq				Hw13
#define	DRV_GDMA_CFG_ASyncHwReq				0
#define	DRV_GDMA_CFG_AckAtWrite				Hw12		// HwCHCTRL_HRD
#define	DRV_GDMA_CFG_AckAtRead					0
#define	DRV_GDMA_CFG_LockTransfer				Hw11		// HwCHCTRL_LOCK
#define	DRV_GDMA_CFG_NoArbitration				Hw10		// HwCHCTRL_BST
#define	DRV_GDMA_CFG_Arbitration					0
#define	DRV_GDMA_CFG_StartBy_SHIFT				8
#define	DRV_GDMA_CFG_StartBy(X)					((X)<<DRV_GDMA_CFG_StartBy_SHIFT)
#define	DRV_GDMA_CFG_StartByHwReqEdge			DRV_GDMA_CFG_StartBy(0)
#define	DRV_GDMA_CFG_StartByHwReqLevel			DRV_GDMA_CFG_StartBy(3)
#define	DRV_GDMA_CFG_StartBySwReq				DRV_GDMA_CFG_StartBy(2)
#define	DRV_GDMA_CFG_StartBy_MASK				DRV_GDMA_CFG_StartBy(3)
#define	DRV_GDMA_CFG_ReadBW_8Bit				(0*Hw6)
#define	DRV_GDMA_CFG_ReadBW_16Bit				(1*Hw6)
#define	DRV_GDMA_CFG_ReadBW_32Bit				(2*Hw6)
#define	DRV_GDMA_CFG_WriteBW_8Bit				(0*Hw4)
#define	DRV_GDMA_CFG_WriteBW_16Bit				(1*Hw4)
#define	DRV_GDMA_CFG_WriteBW_32Bit				(2*Hw4)
#define	DRV_GDMA_CFG_InterruptEnable				Hw2			// HwCHCTRL_IEN
#define	DRV_GDMA_CFG_RepeatEver				Hw1			// HwCHCTRL_REP

#define	DRV_GDMA_CFG_1HopUnit_SHIFT			14
#define	DRV_GDMA_CFG_1HopUnit(X)				((X)<<DRV_GDMA_CFG_1HopUnit_SHIFT)
#define	DRV_GDMA_CFG_1HopUnit_1Byte			DRV_GDMA_CFG_1HopUnit(0)
#define	DRV_GDMA_CFG_1HopUnit_2Byte			DRV_GDMA_CFG_1HopUnit(1)
#define	DRV_GDMA_CFG_1HopUnit_4Byte			DRV_GDMA_CFG_1HopUnit(2)
#define	DRV_GDMA_CFG_1HopUnit_8Byte			DRV_GDMA_CFG_1HopUnit(3)
#define	DRV_GDMA_CFG_1HopUnit_16Byte			DRV_GDMA_CFG_1HopUnit(4)
#define	DRV_GDMA_CFG_1HopUnit_32Byte			DRV_GDMA_CFG_1HopUnit(5)
#define	DRV_GDMA_CFG_1HopUnit_MASK			DRV_GDMA_CFG_1HopUnit(7)
#define	DRV_GDMA_CFG_1HopUnit_Auto				Hw17

#define	DRV_GDMA_CFG_BufNum_SHIFT				18
#define	DRV_GDMA_CFG_BufNum(X)				((X)<<DRV_GDMA_CFG_BufNum_SHIFT)
#define	DRV_GDMA_CFG_BufNum_1					DRV_GDMA_CFG_BufNum(0)		// Single Buffering (DMA Interrupt Occurs after entire buffer is processed)
#define	DRV_GDMA_CFG_BufNum_2					DRV_GDMA_CFG_BufNum(1)		// Double Buffering (DMA Interrupt Occurs after Half of buffer is processed)
#define	DRV_GDMA_CFG_BufNum_4					DRV_GDMA_CFG_BufNum(2)		// Quad Buffering (DMA Interrupt Occurs after 1/4 of buffer is processed)
#define	DRV_GDMA_CFG_BufNum_8					DRV_GDMA_CFG_BufNum(3)
#define	DRV_GDMA_CFG_BufNum_16				DRV_GDMA_CFG_BufNum(4)
#define	DRV_GDMA_CFG_BufNum_32				DRV_GDMA_CFG_BufNum(5)
#define	DRV_GDMA_CFG_BufNum_64				DRV_GDMA_CFG_BufNum(6)
#define	DRV_GDMA_CFG_BufNum_128				DRV_GDMA_CFG_BufNum(7)
#define	DRV_GDMA_CFG_BufNum_256				DRV_GDMA_CFG_BufNum(8)
#define	DRV_GDMA_CFG_BufNum_512				DRV_GDMA_CFG_BufNum(9)
#define	DRV_GDMA_CFG_BufNum_1024				DRV_GDMA_CFG_BufNum(10)
#define	DRV_GDMA_CFG_BufNum_2048				DRV_GDMA_CFG_BufNum(11)
#define	DRV_GDMA_CFG_BufNum_4096				DRV_GDMA_CFG_BufNum(12)
#define	DRV_GDMA_CFG_BufNum_8192				DRV_GDMA_CFG_BufNum(13)
#define	DRV_GDMA_CFG_BufNum_16384				DRV_GDMA_CFG_BufNum(14)
#define	DRV_GDMA_CFG_BufNum_32768				DRV_GDMA_CFG_BufNum(15)
#define	DRV_GDMA_CFG_BufNum_MASK				DRV_GDMA_CFG_BufNum(15)

// GDMA Driver H/W Request Selection Definition
#define	DRV_GDMA_HwREQ_ECC					Hw0
#define	DRV_GDMA_HwREQ_NFC					Hw1
#define	DRV_GDMA_HwREQ_DAI_TX					Hw2
#define	DRV_GDMA_HwREQ_DAI_RX					Hw3
#define	DRV_GDMA_HwREQ_CDIF_RX				Hw4
#define	DRV_GDMA_HwREQ_UART0_TX				Hw5
#define	DRV_GDMA_HwREQ_UART0_RX				Hw6
#define	DRV_GDMA_HwREQ_UART1_TX				Hw7
#define	DRV_GDMA_HwREQ_UART1_RX				Hw8
#define	DRV_GDMA_HwREQ_UART2_TX				Hw9
#define	DRV_GDMA_HwREQ_UART2_RX				Hw10
#define	DRV_GDMA_HwREQ_UART3_TX				Hw11
#define	DRV_GDMA_HwREQ_UART3_RX				Hw12

extern S32	DRV_GDMA(U32 Func, U32 ArgN, ...);

/*==============================================================================
				Timer
   ==============================================================================*/

#define	DRV_TMR_MAX_HANDLE						10
enum
{

	DRV_TMR_FUNC_INIT,
	DRV_TMR_FUNC_OPEN,
	DRV_TMR_FUNC_SETCFG,
	DRV_TMR_FUNC_CLOSE,
	DRV_TMR_FUNC_START,
	DRV_TMR_FUNC_ENABLE,
	DRV_TMR_FUNC_DISABLE,

	DRV_TMR_FUNC_MAX
};


// GDMA Driver Return Value Definition
#define	DRV_TMR_ERROR_YES	1
#define	DRV_TMR_ERROR_NO		0
#define	DRV_TMR_ERROR_OK		0

enum
{
	// Error for Driver State
	DRV_TMR_ERROR_NOTINIT	= (int)0xF0040000,	// Non-init Function come before init function
	DRV_TMR_ERROR_DUPINIT,					// Init function called more than twice

	// Error for Input Parameter
	DRV_TMR_ERROR_INVALID_FUNC,				// Function Code is out of range
	DRV_TMR_ERROR_INVALID_ARGUMENT,			// Number of Argument is out of range
	DRV_TMR_ERROR_INVALID_HANDLE,			// Handle value is out of range
	DRV_TMR_ERROR_INVALID_CH,				// Channel value is out of range

	// Error for Resource Availability
	DRV_TMR_ERROR_NOTAVAILABLE_HANDLE,		// No available handle
	DRV_TMR_ERROR_NOTAVAILABLE_CH,			// No available channel

	// Error for Operating
	DRV_TMR_ERROR_TIMEOUT,					// Time Out
	DRV_TMR_ERROR_NOT_READY,
	DRV_TMR_ERROR_NOT_ACTIVE,
	DRV_TMR_ERROR_NOT_PAUSE,

	// Error for Illegal State
	DRV_TMR_ERROR_INTERNAL,					// Internal Bug

	DRV_TMR_ERROR_MAX
};

// GDMA Driver Status Definition
enum
{
	DRV_TMR_STATUS_INVALID,	// Not Opened
	DRV_TMR_STATUS_IDLE,		// Openad but Not Activated
	DRV_TMR_STATUS_PAUSE,	// Opened & Activated & Pause
	DRV_TMR_STATUS_ACTIVE,	// Opened & Activated
	DRV_TMR_STATUS_MAX
};

#if 0
typedef struct
{
	U32		CHTCFG;
	U32		CHTREF;
	U32		CHTMREF;
	U8		CHSTS;

	U32		HwCHTCFG;
	U32		HwCHTREF;
	U32		HwCHTMREF;

	PTIMERN	pHwTMR;
}sDRV_TMR;
#endif

#define		TIMER_INT_ENABLE		1
#define		TIMER_INT_DISABLE		0


#define	DRV_TMR_CFG_ContinuousMode			0
#define	DRV_TMR_CFG_OneTimeMode				Hw9
#define	DRV_TMR_CFG_TCKDIVIDE2				0*Hw4
#define	DRV_TMR_CFG_TCKDIVIDE4				1*Hw4
#define	DRV_TMR_CFG_TCKDIVIDE8				2*Hw4
#define	DRV_TMR_CFG_TCKDIVIDE16				3*Hw4
#define	DRV_TMR_CFG_TCKDIVIDE32				4*Hw4
#define	DRV_TMR_CFG_TCKDIVIDE1024			5*Hw4
#define	DRV_TMR_CFG_TCKDIVIDE4096			6*Hw4
#define	DRV_TMR_CFG_TCKEXTERNAL				7*Hw4
#define	DRV_TMR_CFG_InterruptEnable			Hw3
#define	DRV_TMR_CFG_InterruptDisable			0
#define	DRV_TMR_CFG_PWMEnable				Hw2
#define	DRV_TMR_CFG_PWMDisable				0
#define	DRV_TMR_CFG_TCNTnSartCountTREFn		Hw1
#define	DRV_TMR_CFG_TCNTnStartCountZero		0
#define	DRV_TMR_CFG_TimerEnable				Hw0
#define	DRV_TMR_CFG_TimerDisable				0

extern S32	DRV_TIMER(U32 Func, U32 ArgN, ...);

/*==============================================================================
				UART
   ==============================================================================*/

#define	DRV_UART_MAX_HANDLE		10

// UART Driver Function Definition
enum
{
	DRV_UART_FUNC_INIT,					// UART Driver Initialization : Argument = None
	DRV_UART_FUNC_PROCESS_INTERRUPT,	// Process UART Interrupt : Argument = [Channel]

	DRV_UART_FUNC_OPEN,				// Open Handle : Argument = (Channel Number, [Port Number], *sDRV_UART)
	DRV_UART_FUNC_INSTALL_HANDLER,		// Install UART Interrupt Handler : Argument = (Channel, NewHandler, [* OldHandler])
	DRV_UART_FUNC_UNINSTALL_HANDLER,	// UnInstall UART Interrupt Handler : Argument = (Channel)

	DRV_UART_FUNC_GETHwUART,			// Get HwUART Base Address : Argument = (Handle, *sHwUART)
	DRV_UART_FUNC_SETCFG,				// Configurate UART : Argument = (Handle, Fuart, Baud, Interrupt, FifoCtrl, LineCtrl, AutoFlowControl)
	DRV_UART_FUNC_SENDDATA,			// Send Data using UART Channel : Argument = (Handle, Byte)
	DRV_UART_FUNC_RCVDATA,			// Receive Data using UART Channel : Argument = (Handle, *Byte)
	DRV_UART_FUNC_OPENDMA,			// Set Registers for DMA Operation : Argument = (Handle, TriggerLevel, ParityFlag, StopBitNum, WordLength)
	DRV_UART_FUNC_CLOSEDMA,			// Set Registers for Normal Operation : Argument = (Handle)

	DRV_UART_FUNC_START,			// Start Handle : Argument = (Handle)
	DRV_UART_FUNC_STOP,				// Stop Handle : Argument = (Handle)
	DRV_UART_FUNC_CLOSE,			// Close Handle : Argument = (Handle)

	DRV_UART_FUNC_SC_OPEN,				// Open Smart Card Interface : Argument = (Handle)
	DRV_UART_FUNC_SC_CLOSE,				// Close Smart Card Interface : Argument = (Handle)
	DRV_UART_FUNC_SC_RESET,				// Reset Smart Card Interface : Argument = (Handle, pATRData, pATRLength)
	DRV_UART_FUNC_SC_SENDDATA,			// Send Data to Smart Card : Argument = (Handle, pData, Length)
	DRV_UART_FUNC_SC_RCVDATA,			// Receive Data from Smart Card : Argument = (Handle, pData, Length)

	DRV_UART_FUNC_MAX
};

// UART Driver Status Definition
enum
{
	DRV_UART_STATUS_INVALID,	// Not Opened
	DRV_UART_STATUS_IDLE,		// Openad but Not Activated
	DRV_UART_STATUS_PAUSE,		// Opened & Activated & Pause
	DRV_UART_STATUS_ACTIVE,		// Opened & Activated
	DRV_UART_STATUS_MAX
};

// UART Driver Data Structure
typedef struct
{
	U32			BAUD;		// Baud Rate
	U8			IER;		// Interrupt Enable Register
	U8			FCR;		// FIFO Control Register
	U8			LCR;		// Line Control Register
	U8			AFT;		// Auto Flow Control Trigger Level Register
	U32			Fuart;		// UART Main Clock Frequency
	U32			SCCTRL;		// UART Smart Card Control
	U32			SCCLK;		// UART Smart Card Clock

	U8			CHSTS;		// Handle Status
	U8			CHANNEL;	// Channel Number
	U8			PORT;		// Port Number
	PUART		pHwUART;	// Base Address
} sDRV_UART;

// UART Driver Buffer structure for Smart Card
#define IO_UART_RX_BUFFER_SIZE		(1<<8)
#define IO_UART_TX_BUFFER_SIZE		(1<<3)
typedef struct
{
	U16			usTXHead, usTXTail;
	U16			usRXHead, usRXTail;
	U8			sRX_Buffer[IO_UART_RX_BUFFER_SIZE];
	U8			sTX_Buffer[IO_UART_TX_BUFFER_SIZE];
} sDRV_UART_BUF;

// UART Driver Return Value Definition
#define	DRV_UART_ERROR_YES		1
#define	DRV_UART_ERROR_NO		0
#define	DRV_UART_ERROR_OK		0
enum
{
	// Error for Driver State
	DRV_UART_ERROR_NOTINIT	= (int)0xF0055000,	// Non-init Function come before init function
	DRV_UART_ERROR_DUPINIT,						// Init function called more than twice

	// Error for Input Parameter
	DRV_UART_ERROR_INVALID_FUNC,				// Function Code is out of range
	DRV_UART_ERROR_INVALID_ARGUMENT,			// Number of Argument is out of range
	DRV_UART_ERROR_INVALID_HANDLE,				// Handle value is out of range
	DRV_UART_ERROR_INVALID_CH,					// Channel value is out of range

	// Error for Resource Availability
	DRV_UART_ERROR_NOTAVAILABLE_HANDLE,			// No available handle
	DRV_UART_ERROR_NOTAVAILABLE_CH,				// No available channel

	// Error for Receiving Operation
	DRV_UART_ERROR_RCV_READY,					// Data is not ready
	DRV_UART_ERROR_RCV_FRAMING,					// Received Data doesn't have a valid stop bit
	DRV_UART_ERROR_RCV_PARITY,					// Received Data doesn't have a correct even/odd parity

	// Error for Illegal State
	DRV_UART_ERROR_INTERNAL,					// Internal Bug

	DRV_UART_ERROR_MAX
};

// UART Channel Number Definition
enum
{
	DRV_UART_CH0 = 0,
	DRV_UART_CH1,
	DRV_UART_CH2,
	DRV_UART_CH3,
	DRV_UART_CHMAX
};

// UART Port Number Definition
enum
{
	DRV_UART_PORT0 = 0,
	DRV_UART_PORT1,
	DRV_UART_PORT2,
	DRV_UART_PORT3,
	DRV_UART_PORT4,
	DRV_UART_PORT5,
	DRV_UART_PORTMAX
};

// UART Line Parity Definition
enum
{
	DRV_UART_PARITY_NONE = 0,
	DRV_UART_PARITY_ODD,
	DRV_UART_PARITY_EVEN,
	DRV_UART_PARITY_MAX
};

// UART Buffer Definition
enum
{
	DRV_UART_BUFFER_TX = 0,
	DRV_UART_BUFFER_RX,
	DRV_UART_BUFFER_TXRX,
	DRV_UART_BUFFER_MAX
};

// UART Smart Card Control Definition
enum
{
	DRV_UART_SCCTRL_OFF = 0,
	DRV_UART_SCCTRL_ON,
	DRV_UART_SCCTRL_MAX
};

// UART Interrupt Definition for Smart Card
enum
{
	DRV_UART_SCINT_DISABLE = 0,
	DRV_UART_SCINT_ENABLE,
	DRV_UART_SCINT_NOCHANGE,
	DRV_UART_SCINT_MAX
};

// UART Application Definition
enum
{
	DRV_UART_APP_DEBUG = 0,	// Debugging by using tbench
	DRV_UART_APP_BLUETOOTH,	// Bluetooth
	DRV_UART_APP_SMARTCARD,	// SmartCard Interface
	DRV_UART_APP_COMMAND,	// Command Interpreter for Module Solution
	DRV_UART_APP_MAX
};

// UART Driver Channel Configuration Definition
#define	DRV_UART_IER_EnableMSI				Hw3
#define	DRV_UART_IER_DisableMSI				0
#define	DRV_UART_IER_EnableRcvLSI			Hw2
#define	DRV_UART_IER_DisableRcvLSI			0
#define	DRV_UART_IER_EnableTXI				Hw1
#define	DRV_UART_IER_DisableTXI				0
#define	DRV_UART_IER_EnableRXI				Hw0
#define	DRV_UART_IER_DisableRXI				0

#define	DRV_UART_FCR_RxTriggerLvl_SHIFT		6
#define	DRV_UART_FCR_RxTriggerLvl(X)			((X)<<DRV_UART_FCR_RxTriggerLvl_SHIFT)
#define	DRV_UART_FCR_RxTriggerLvl_01			DRV_UART_FCR_RxTriggerLvl(0)
#define	DRV_UART_FCR_RxTriggerLvl_04			DRV_UART_FCR_RxTriggerLvl(1)
#define	DRV_UART_FCR_RxTriggerLvl_08			DRV_UART_FCR_RxTriggerLvl(2)
#define	DRV_UART_FCR_RxTriggerLvl_14			DRV_UART_FCR_RxTriggerLvl(3)
#define	DRV_UART_FCR_TxWindowLvl_SHIFT		4
#define	DRV_UART_FCR_TxWindowLvl(X)			((X)<<DRV_UART_FCR_TxWindowLvl_SHIFT)
#define	DRV_UART_FCR_TxWindowLvl_16			DRV_UART_FCR_TxWindowLvl(0)
#define	DRV_UART_FCR_TxWindowLvl_08			DRV_UART_FCR_TxWindowLvl(1)
#define	DRV_UART_FCR_TxWindowLvl_04			DRV_UART_FCR_TxWindowLvl(2)
#define	DRV_UART_FCR_TxWindowLvl_01			DRV_UART_FCR_TxWindowLvl(3)
#define	DRV_UART_FCR_EnableDMA				Hw3
#define	DRV_UART_FCR_DisableDMA				0
#define	DRV_UART_FCR_TxFifoReset			Hw2
#define	DRV_UART_FCR_RxFifoReset			Hw1
#define	DRV_UART_FCR_EnableFifo				Hw0
#define	DRV_UART_FCR_DisableFifo			0

#define	DRV_UART_LCR_AccessDivisor			Hw7
#define	DRV_UART_LCR_NotAccessDivisor		0
#define	DRV_UART_LCR_EnableBreak			Hw6
#define	DRV_UART_LCR_DisableBreak			0
#define	DRV_UART_LCR_EnableStickParity		Hw5
#define	DRV_UART_LCR_DisableStickParity		0
#define	DRV_UART_LCR_EnableEvenParity		Hw4
#define	DRV_UART_LCR_EnableOddParity		0
#define	DRV_UART_LCR_EnableParity			Hw3
#define	DRV_UART_LCR_DisableParity			0
#define	DRV_UART_LCR_OneStopBit				Hw2
#define	DRV_UART_LCR_TwoStopBit				0
#define	DRV_UART_LCR_WordLength_SHIFT		0
#define	DRV_UART_LCR_WordLength(X)			((X)<<DRV_UART_LCR_WordLength_SHIFT)
#define	DRV_UART_LCR_WordLength_5Bit		DRV_UART_LCR_WordLength(0)
#define	DRV_UART_LCR_WordLength_6Bit		DRV_UART_LCR_WordLength(1)
#define	DRV_UART_LCR_WordLength_7Bit		DRV_UART_LCR_WordLength(2)
#define	DRV_UART_LCR_WordLength_8Bit		DRV_UART_LCR_WordLength(3)

// UART SmartCard Clock Definition
#define DRV_UART_SCCLK						60000 // 6.0 MHz
// UART SmartCard Clock Conversion Factor Definition
#define DRV_UART_SCCLK_FACTOR				372
// UART SmartCard Delay Time Definition
#define DRV_UART_SC_DELAY_TIME				4000 // 4.0 ms

extern S32	DRV_UART(U32 Func, U32 ArgN, ...);

/*==============================================================================
				GPSB
  ==============================================================================*/

#define	DRV_GPSB_MAX_HANDLE		10

// GPSB Driver Function Definition
enum
{
	DRV_GPSB_FUNC_INIT,					// GPSB Driver Initialization : Argument = None
	DRV_GPSB_FUNC_PROCESS_INTERRUPT,	// Process GPSB Interrupt : Argument = None

	DRV_GPSB_FUNC_OPEN,					// Open Handle : Argument = (Channel Number, Port Number, *sDRV_GPSB)
	DRV_GPSB_FUNC_INSTALL_HANDLER,		// Install Handler : Argument = (Channel, NewHandler, [* OldHandler])
	DRV_GPSB_FUNC_UNINSTALL_HANDLER,	// Install Handler : Argument = (Channel)

	DRV_GPSB_FUNC_GETHwGPSB,			// Get HwGPSB Base Address : Argument = (Handle, *sHwGPSB)
	DRV_GPSB_FUNC_SETCFG,				// Configurate GPSB : Argument = (Handle, MODE, SCKFREQ, SWCTRL, SDOSDI)
	DRV_GPSB_FUNC_SENDDATA,				// Send Data using GPSB Channel : Argument = (Handle, Cmd, *Buffer, Length, BitWidth)
	DRV_GPSB_FUNC_RCVDATA,				// Receive Data using GPSB Channel : Argument = (Handle, Cmd, *Buffer, Length, BitWidth)
	DRV_GPSB_FUNC_UPDATEPID,			// Update PID Table = (Handle, *PIDTable)
	DRV_GPSB_FUNC_OPENDMA,				// Set Registers for DMA Operation : Argument = (Handle)
	DRV_GPSB_FUNC_CLOSEDMA,				// Set Registers for Normal Operation : Argument = (Handle)
	DRV_GPSB_FUNC_STARTDMA,				// Set Registers for Starting DMA Operation : Argument = (Handle, DMADir, PktSize, PktNum, *TxBuf, *RxBuf, FIFOThr, ContMode)
	DRV_GPSB_FUNC_STOPDMA,				// Set Registers for Stopping DMA Operation : Argument = (Handle)

	DRV_GPSB_FUNC_START,				// Start Handle : Argument = (Handle)
	DRV_GPSB_FUNC_STOP,					// Stop Handle : Argument = (Handle)
	DRV_GPSB_FUNC_CLOSE,				// Close Handle : Argument = (Handle)

	DRV_GPSB_FUNC_MAX
};

// GPSB Driver Status Definition
enum
{
	DRV_GPSB_STATUS_INVALID,	// Not Opened
	DRV_GPSB_STATUS_IDLE,		// Openad but Not Activated
	DRV_GPSB_STATUS_PAUSE,		// Opened & Activated & Pause
	DRV_GPSB_STATUS_ACTIVE,		// Opened & Activated
	DRV_GPSB_STATUS_MAX
};

// GPSB DMA Mode Information Structure
typedef struct
{
	U8			DMAMode;		// DMA Mode - Normal, MPEG2-TS
	U8			PIDMatch;		// PID Match Control - 0: Disable, 1: Enable
	U8			SyncMatch;		// Sync Byte Match Control - 0: Disable, 1: Enable
	U8			PIDNum;			// Number of PID
} sGPSB_DMA;

// GPSB Driver Data Structure
typedef struct
{
	U32			MODE;			// Mode Register
	U32			SCKFREQ;		// SCK Frequency
	U8			SWCTRL;			// Control Setting
	U8			SDOSDI;			// Output/Input Use Setting
	U8			CHSTS;			// Handle Status
	U8			CHANNEL;		// Channel Number
	U8			PORT;			// Port Number
	sGPSB_DMA	DMAMODE;		// DMA Mode Information
	PGPSB		*pHwGPSB;		// Base Address
} sDRV_GPSB;

// GPSB Driver Return Value Definition
#define	DRV_GPSB_ERROR_YES		1
#define	DRV_GPSB_ERROR_NO		0
#define	DRV_GPSB_ERROR_OK		0
enum
{
	// Error for Driver State
	DRV_GPSB_ERROR_NOTINIT	= (int)0xF0057400,	// Non-init Function come before init function
	DRV_GPSB_ERROR_DUPINIT,						// Init function called more than twice

	// Error for Input Parameter
	DRV_GPSB_ERROR_INVALID_FUNC,				// Function Code is out of range
	DRV_GPSB_ERROR_INVALID_ARGUMENT,			// Number of Argument is out of range
	DRV_GPSB_ERROR_INVALID_HANDLE,				// Handle value is out of range
	DRV_GPSB_ERROR_INVALID_CH,					// Channel value is out of range

	// Error for Resource Availability
	DRV_GPSB_ERROR_NOTAVAILABLE_HANDLE,			// No available handle
	DRV_GPSB_ERROR_NOTAVAILABLE_CH,				// No available channel

	// Error for Illegal State
	DRV_GPSB_ERROR_INTERNAL,					// Internal Bug

	DRV_GPSB_ERROR_MAX
};

// GPSB Channel Control Method Definition
enum
{
	DRV_GPSB_CTRL_HW = 0,
	DRV_GPSB_CTRL_SW,
	DRV_GPSB_CTRL_MAX
};
#define	DRV_GPSB_SwCtrlMode(X)		((X)&0x3F)

// GPSB Tx/Rx Swap Mode Definition
#define	DRV_GPSB_SWAP_TX		Hw6
#define	DRV_GPSB_SWAP_RX		Hw7

#define	DRV_GPSB_SwapMode(X)		((X) & (DRV_GPSB_SWAP_TX | DRV_GPSB_SWAP_RX))
#define	DRV_GPSB_SwapMode_Tx(X)	(((X) & DRV_GPSB_SWAP_TX) ? 1 : 0)
#define	DRV_GPSB_SwapMode_Rx(X)	(((X) & DRV_GPSB_SWAP_RX) ? 1 : 0)

// GPSB Channel Master/Slave Definition
enum
{
	DRV_GPSB_MODE_MASTER = 0,
	DRV_GPSB_MODE_SLAVE,
	DRV_GPSB_MODE_MAX
};

// GPSB Channel I/O Definition
enum
{
	DRV_GPSB_SDOSDI_NOUSE = 0,
	DRV_GPSB_SDOSDI_SDI,
	DRV_GPSB_SDOSDI_SDO,
	DRV_GPSB_SDOSDI_ALLUSE,
	DRV_GPSB_SDOSDI_MAX
};

// GPSB Channel Number Definition
enum
{
	DRV_GPSB_CH0 = 0,
	DRV_GPSB_CH1,
 
	DRV_GPSB_CH2,
	DRV_GPSB_CH3,
 	DRV_GPSB_CH4,
	DRV_GPSB_CH5, 
	DRV_GPSB_CHMAX
};

// GPSB Port Number Definition
enum
{
	DRV_GPSB_PORT0 = 0,
	DRV_GPSB_PORT1,
	DRV_GPSB_PORT2,
 
	DRV_GPSB_PORT3,
	DRV_GPSB_PORT4,
	DRV_GPSB_PORT5,
	DRV_GPSB_PORT6,
	DRV_GPSB_PORT7,
	DRV_GPSB_PORT8,
	DRV_GPSB_PORT9,
	DRV_GPSB_PORT10,
 	DRV_GPSB_PORT11,
	DRV_GPSB_PORT12,
 
	DRV_GPSB_PORTMAX
};

// GPSB DMA Mode Definition
enum
{
	DRV_GPSB_DMAMODE_NOR = 0,
	DRV_GPSB_DMAMODE_TS,
	DRV_GPSB_DMAMODE_MAX
};

// GPSB DMA Transfer Direction Definition
enum
{
	DRV_GPSB_DMA_RECEIVE = 0,
	DRV_GPSB_DMA_TRANSMIT,
	DRV_GPSB_DMA_MAX
};

// GPSB Driver Channel Mode Definition
#define	DRV_GPSB_MODE_Divisor_SHIFT			24
#define	DRV_GPSB_MODE_Divisor(X)			((X)<<DRV_GPSB_MODE_Divisor_SHIFT)
#define	DRV_GPSB_MODE_MRecoverTime_2SCKO	Hw23
#define	DRV_GPSB_MODE_MRecoverTime_SCKO		0
#define	DRV_GPSB_MODE_MHoldTime_2SCKO		Hw22
#define	DRV_GPSB_MODE_MHoldTime_SCKO		0
#define	DRV_GPSB_MODE_MSetupTime_2SCKO		Hw21
#define	DRV_GPSB_MODE_MSetupTime_SCKO		0
#define	DRV_GPSB_MODE_CSActiveHigh			Hw20
#define	DRV_GPSB_MODE_CSActiveLow			0
#define	DRV_GPSB_MODE_CMDActiveHigh			Hw19
#define	DRV_GPSB_MODE_CMDActiveLow			0
#define	DRV_GPSB_MODE_TransRisingEdge		Hw18
#define	DRV_GPSB_MODE_TransFallingEdge		0
#define	DRV_GPSB_MODE_RcvRisingEdge			Hw17
#define	DRV_GPSB_MODE_RcvFallingEdge		0
#define	DRV_GPSB_MODE_SCKPolCtrl_High		Hw16
#define	DRV_GPSB_MODE_SCKPolCtrl_Low		0
#define	DRV_GPSB_MODE_ClearRcvFifoCount		Hw15
#define	DRV_GPSB_MODE_ClearTransFifoCount	Hw14
#define	DRV_GPSB_MODE_BitWidth_SHIFT		8
#define	DRV_GPSB_MODE_BitWidth(X)			((X)<<DRV_GPSB_MODE_BitWidth_SHIFT)
#define	DRV_GPSB_MODE_DataShiftLSBFirst		Hw7
#define	DRV_GPSB_MODE_DataShiftMSBFirst		0
#define	DRV_GPSB_MODE_EnableLoopBack		Hw6
#define	DRV_GPSB_MODE_DisableLoopBack		0
#define	DRV_GPSB_MODE_EnableContTrans		Hw4
#define	DRV_GPSB_MODE_DisableContTrans		0
#define	DRV_GPSB_MODE_EnableOperation		Hw3
#define	DRV_GPSB_MODE_DisableOperation		0
#define	DRV_GPSB_MODE_SlaveMode				Hw2
#define	DRV_GPSB_MODE_MasterMode			0
#define	DRV_GPSB_MODE_OpMode_SHIFT			0
#define	DRV_GPSB_MODE_OpMode(X)				((X)<<DRV_GPSB_MODE_OpMode_SHIFT)
#define	DRV_GPSB_MODE_OpMod_SPI				DRV_GPSB_MODE_OpMode(0)
#define	DRV_GPSB_MODE_OpMod_SSP				DRV_GPSB_MODE_OpMode(1)

// GPSB PID Table Definition
#define	DRV_GPSB_PID_MAX_NUM				32
#define	DRV_GPSB_PID_VALUE_MASK				0x1FFF

extern S32	DRV_GPSB(U32 Func, U32 ArgN, ...);


/*==============================================================================
				I2C
   ==============================================================================*/
int	DDI_I2C_Initialize(void);
int	DDI_I2C_Terminate(void);
int	DDI_I2C_Write(unsigned int uiFlag, unsigned int uiHbyte, unsigned int uiLbyte);
int	DDI_I2C_Read(unsigned int uiFlag, unsigned int uiHbyte, unsigned int uiLbyte);

#define	DRV_I2CM_MAX_HANDLE					10
#define	DRV_I2CS_MAX_HANDLE					10

#define	DRV_I2C_MASTER						1
#define	DRV_I2C_SLAVE						2

#define	DRV_I2C_MODE_8BIT					1
#define	DRV_I2C_MODE_16BIT					2

#define	DRV_IO_I2CM_CH0						0
#define	DRV_IO_I2CM_CH1						1

#define	DRV_IO_I2C_PORT0					0
#define	DRV_IO_I2C_PORT1					1

enum
{

	DRV_I2C_FUNC_INIT,
	DRV_I2C_FUNC_OPEN,
	DRV_I2C_FUNC_SETCFG,
	DRV_I2C_FUNC_INSTALL_HANDLER,
	DRV_I2C_FUNC_UNINSTALL_HANDLER,
	DRV_I2C_FUNC_PROCESS_INTERRUPT,
	DRV_I2C_FUNC_CLOSE,
	DRV_I2C_FUNC_START,
	DRV_I2C_FUNC_WRITE,
	DRV_I2C_FUNC_READ,

	DRV_I2C_FUNC_MAX
};


// I2C Driver Return Value Definition
#define	DRV_I2C_ERROR_YES						1
#define	DRV_I2C_ERROR_NO						0
#define	DRV_I2C_ERROR_OK						0

enum
{
	// Error for Driver State
	DRV_I2C_ERROR_NOTINIT	= (int)0xF0040000,	// Non-init Function come before init function
	DRV_I2C_ERROR_DUPINIT,					// Init function called more than twice

	// Error for Input Parameter
	DRV_I2C_ERROR_INVALID_FUNC,				// Function Code is out of range
	DRV_I2C_ERROR_INVALID_ARGUMENT,			// Number of Argument is out of range
	DRV_I2C_ERROR_INVALID_HANDLE,			// Handle value is out of range
	DRV_I2C_ERROR_INVALID_CH,				// Channel value is out of range

	// Error for Resource Availability
	DRV_I2C_ERROR_NOTAVAILABLE_HANDLE,		// No available handle
	DRV_I2C_ERROR_NOTAVAILABLE_CH,			// No available channel

	// Error for Operating
	DRV_I2C_ERROR_TIMEOUT,					// Time Out
	DRV_I2C_ERROR_NOT_READY,
	DRV_I2C_ERROR_NOT_ACTIVE,
	DRV_I2C_ERROR_NOT_PAUSE,

	// Error for Illegal State
	DRV_I2C_ERROR_INTERNAL,					// Internal Bug

	DRV_I2C_ERROR_MAX
};

// I2C Driver Status Definition
enum
{
	DRV_I2C_STATUS_INVALID,	// Not Opened
	DRV_I2C_STATUS_IDLE,		// Openad but Not Activated
	DRV_I2C_STATUS_PAUSE,	// Opened & Activated & Pause
	DRV_I2C_STATUS_ACTIVE,	// Opened & Activated
	DRV_I2C_STATUS_MAX
};

typedef struct
{
	U32			CHPRES;
	U32			CHCTRL;
	U8			CHSTS;

	U32			SlaveAddr;
	U32			PORT;
	U32			BitMode;

	U32			HWPRES;
	U32			HWCTRL;
	U32			HWSR;
	U32			HWTR;

	I2CMASTER	*pHwI2CM;
}sDRV_I2CM;

typedef struct
{
	U32			CHCTL;
	U32			CHADDR;
	U32			CHINT;

	U8			CHSTS;

	U32			HwDPORT;
	U32			HwCTL;
	U32			HwADDR;
	U32			HwINT;
	U32			HwSTAT;
	U32			HwMBF;
	U32			HwMB0;
	U32			HwMB1;

	PI2CSLAVE	pHwI2CS;
}sDRV_I2CS;

#define	DRV_I2CM_SCL_400KHz							4000
#define	DRV_I2CM_SCL_260KHz							2600
#define	DRV_I2CM_SCL_200KHz							2000
#define	DRV_I2CM_SCL_160KHz							1600
#define	DRV_I2CM_SCL_133KHz							1330
#define	DRV_I2CM_SCL_100KHz							1000

#define	DRV_I2CM_CTR_I2CCoreEnable					Hw7
#define	DRV_I2CM_CTR_I2CCoreDisable					0
#define	DRV_I2CM_CTR_I2CCoreInterruptEnable			Hw6
#define	DRV_I2CM_CTR_I2CCoreInterruptDisable			0
#define	DRV_I2CM_CTR_I2CDataWidth8Bit					0
#define	DRV_I2CM_CTR_I2CDataWidth16Bit				Hw5

#define	DRV_I2CM_CMD_STA_Enable						Hw7
#define	DRV_I2CM_CMD_STA_Disable						0
#define	DRV_I2CM_CMD_STO_Enable						Hw6
#define	DRV_I2CM_CMD_STO_Disable						0
#define	DRV_I2CM_CMD_RD_Enable						Hw5
#define	DRV_I2CM_CMD_RD_Disable						0
#define	DRV_I2CM_CMD_WR_Enable						Hw4
#define	DRV_I2CM_CMD_WR_Disable						0
#define	DRV_I2CM_CMD_ACK_Enable						Hw3
#define	DRV_I2CM_CMD_ACK_Disable						0
#define	DRV_I2CM_CMD_LATCH_Enable					1
#define	DRV_I2CM_CMD_LATCH_Disable					0

#define	DRV_I2CS_CTL_0Master_1Master				0
#define	DRV_I2CS_CTL_0Slave_1Master					Hw30
#define	DRV_I2CS_CTL_0Master_1Slave					Hw31
#define	DRV_I2CS_CTL_RXFIFOThreshold1				Hw8
#define	DRV_I2CS_CTL_FIFO_Clear						Hw2
#define	DRV_I2CS_CTL_I2CSlaveCoreEnable				Hw0
#define	DRV_I2CS_CTL_I2CSlaveCoreDisable				0

#define	DRV_I2CS_INT_STAT_R_BYTE						Hw27							// All byte of data buffer has been read by a master
#define	DRV_I2CS_INT_STAT_W_BYTE						Hw26							// All byte of data buffer has been written by a master
#define	DRV_I2CS_INT_STAT_R_BUFF						Hw25							// Data buffer has been read by a master
#define	DRV_I2CS_INT_STAT_W_BUFF					Hw24							// Data buffer has been written by a master
#define	DRV_I2CS_INT_STAT_TXUR						Hw23							// TX FIFO under run
#define	DRV_I2CS_INT_STAT_RXOR						Hw22							// RX FIFO over run
#define	DRV_I2CS_INT_STAT_TXB							Hw21							// TX bus cycle started with TX FIFO empty
#define	DRV_I2CS_INT_STAT_RXF							Hw20							// RX FIFO full
#define	DRV_I2CS_INT_STAT_TXE							Hw19							// TX FIFO empty
#define	DRV_I2CS_INT_STAT_RXNE						Hw18							// RX FIFO not empty
#define	DRV_I2CS_INT_STAT_TXL							Hw17							// TX FIFO Level (TXVC <= TXTH)
#define	DRV_I2CS_INT_STAT_RXL							Hw16							// RX FIFO Level (RXVC <= RXTH)

#define	DRV_I2CS_INT_EN_R_BYTE						Hw11							// All byte of data buffer has been read by a master
#define	DRV_I2CS_INT_EN_W_BYTE						Hw10							// All byte of data buffer has been written by a master
#define	DRV_I2CS_INT_EN_R_BUFF						Hw9								// Data buffer has been read by a master
#define	DRV_I2CS_INT_EN_W_BUFF						Hw8								// Data buffer has been written by a master
#define	DRV_I2CS_INT_EN_TXUR							Hw7								// TX FIFO under run
#define	DRV_I2CS_INT_EN_RXOR							Hw6								// RX FIFO over run
#define	DRV_I2CS_INT_EN_TXB							Hw5								// TX bus cycle started with TX FIFO empty
#define	DRV_I2CS_INT_EN_RXF							Hw4								// RX FIFO full
#define	DRV_I2CS_INT_EN_TXE							Hw3								// TX FIFO empty
#define	DRV_I2CS_INT_EN_RXNE							Hw2								// RX FIFO not empty
#define	DRV_I2CS_INT_EN_TXL							Hw1								// TX FIFO Level (TXVC <= TXTH)
#define	DRV_I2CS_INT_EN_RXL							Hw0								// RX FIFO Level (RXVC <= RXTH)

// I2C Master
extern S32	DRV_I2CM(U32 Func, U32 ArgN, ...);

// I2C Slave
extern S32	DRV_I2CS( U32 Func, U32 ArgN, ... );


/*==============================================================================
				2D DMA (TCC78x)
   ==============================================================================*/
#define	LCD_ROT0L		0
#define	LCD_ROT90L		1
#define	LCD_ROT180L		2
#define	LCD_ROT270L		3
#define	LCD_MIR_ROT0L	10
#define	LCD_MIR_ROT90L	11
#define	LCD_MIR_ROT180L	12
#define	LCD_MIR_ROT270L	13


#ifdef 	TCC79XX
/*==============================================================================
				NTSC/PAL Encoder
  ==============================================================================*/

#define	DRV_TVE_MAX_HANDLE		2

// TVE Driver Function Definition
enum
{
	DRV_TVE_FUNC_INIT,					// TVE Driver Initialization : Argument = None
	DRV_TVE_FUNC_PROCESS_INTERRUPT,		// Process TVE Interrupt : Argument = None

	DRV_TVE_FUNC_OPEN,					// Open Handle : Argument = (*sDRV_TVE)
	DRV_TVE_FUNC_GETHwTVE,				// Get HwTVE Base Address : Argument = (Handle, *sHwTVE)
	DRV_TVE_FUNC_SETCFG,				// Configurate TVE : Argument = (Handle, TVMode)

	DRV_TVE_FUNC_START,					// Start Operation : Argument = (Handle)
	DRV_TVE_FUNC_STOP,					// Stop Handle : Argument = (Handle)
	DRV_TVE_FUNC_CLOSE,					// Close Handle : Argument = (Handle)

	DRV_TVE_FUNC_ADJ, 						//Adjust Video : Argument = (Handle)

	DRV_TVE_FUNC_MAX
};

// TVE Driver Status Definition
enum
{
	DRV_TVE_STATUS_INVALID,		// Not Opened
	DRV_TVE_STATUS_IDLE,		// Openad but Not Activated
	DRV_TVE_STATUS_PAUSE,		// Opened & Activated & Pause
	DRV_TVE_STATUS_ACTIVE,		// Opened & Activated
	DRV_TVE_STATUS_MAX
};

// TVE Driver Data Structure
typedef struct
{
	U8				TVMODE;			// Output Mode : NTSC or PAL
	U8				CGMSCTRL;		// CGMS Control : ON or OFF
	U32				CGMSDATA;		// CGMS Data : 20 bits
	U8				CHSTS;			// Handle Status
	sHwTVE			*pHwTVE;		// Base Address
} sDRV_TVE;

typedef	volatile struct {
	unsigned VENCON;
	unsigned VENCIF;
} sHwTVECON;

// TVE Mode Control Register Definition - ECMDA, ECMDB, ECMDC Register
#define SET_TVE_MODE_PWDN_ENABLE			0x00000001
#define SET_TVE_MODE_PWDN_DISABLE			0x00000002
#define SET_TVE_MODE_FDRST_RELATIONSHIP		0x00000004
#define SET_TVE_MODE_FDRST_FREERUNNING		0x00000008
#define SET_TVE_MODE_FSCSEL_NTSC			0x00000010
#define SET_TVE_MODE_FSCSEL_PALX			0x00000020
#define SET_TVE_MODE_FSCSEL_PALM			0x00000040
#define SET_TVE_MODE_FSCSEL_PALCN			0x00000080
#define SET_TVE_MODE_PEDESTAL_ENABLE		0x00000100
#define SET_TVE_MODE_PEDESTAL_DISABLE		0x00000200
#define SET_TVE_MODE_PIXEL_601				0x00000400
#define SET_TVE_MODE_PIXEL_SQUARE			0x00000800
#define SET_TVE_MODE_OUTPUT_525LINES		0x00001000
#define SET_TVE_MODE_OUTPUT_625LINES		0x00002000
#define SET_TVE_MODE_PHALT_NTSC				0x00004000
#define SET_TVE_MODE_PHALT_PAL				0x00008000

#define SET_TVE_MODE_VBIBLANK_BLACK			0x00010000
#define SET_TVE_MODE_VBIBLANK_BYPASS		0x00020000
#define SET_TVE_MODE_CHROMABW_LOW			0x00040000
#define SET_TVE_MODE_CHROMABW_MEDIUM		0x00080000
#define SET_TVE_MODE_CHROMABW_HIGH			0x00100000
#define SET_TVE_MODE_LUMABW_LOW				0x00200000
#define SET_TVE_MODE_LUMABW_MEDIUM			0x00400000
#define SET_TVE_MODE_LUMABW_HIGH			0x00800000

#define SET_TVE_MODE_CSYNC_ENABLE			0x01000000
#define SET_TVE_MODE_CSYNC_DISABLE			0x02000000
#define SET_TVE_MODE_CSOUTPUT_CSYNC			0x04000000
#define SET_TVE_MODE_CSOUTPUT_KEYCLAMP		0x08000000
#define SET_TVE_MODE_CSOUTPUT_KEYPULSE		0x10000000
#define SET_TVE_MODE_RGBSYNC_NOSYNC			0x20000000
#define SET_TVE_MODE_RGBSYNC_RGB			0x40000000
#define SET_TVE_MODE_RGBSYNC_G				0x80000000

// TVE Clock Generator Register Definition - GLK Register
#define SET_TVE_GLK_CRYSTAL_27MHZ			0x00000001
#define SET_TVE_GLK_CRYSTAL_24MHZ			0x00000002
#define SET_TVE_GLK_RESET_ENABLE			0x00000004
#define SET_TVE_GLK_RESET_DISABLE			0x00000008
#define SET_TVE_GLK_SOURCE_INT				0x00000010
#define SET_TVE_GLK_SOURCE_RTCO				0x00000020
#define SET_TVE_GLK_SOURCE_CLKI				0x00000040
#define SET_TVE_GLK_PL_ACTIVELOW			0x00000080
#define SET_TVE_GLK_PL_ACTIVEHIGH			0x00000100

// TVE DAC Control Register Definition - DACSEL, DACPD Register
#define SET_TVE_DAC_OUTPUT_CODE0			0x00000001
#define SET_TVE_DAC_OUTPUT_CVBS				0x00000002
#define SET_TVE_DAC_PWDN_ENABLE				0x00000004
#define SET_TVE_DAC_PWDN_DISABLE			0x00000008

// TVE SYNC Control Register Definition - ICNTL Register
#define SET_TVE_SYNC_ODD_ACTIVELOW			0x00000001
#define SET_TVE_SYNC_ODD_ACTIVEHIGH			0x00000002
#define SET_TVE_SYNC_VSYNC_ACTIVELOW		0x00000004
#define SET_TVE_SYNC_VSYNC_ACTIVEHIGH		0x00000008
#define SET_TVE_SYNC_HSYNC_ACTIVELOW		0x00000010
#define SET_TVE_SYNC_HSYNC_ACTIVEHIGH		0x00000020
#define SET_TVE_SYNC_LATCH_VSFALLING		0x00000040
#define SET_TVE_SYNC_LATCH_VSRISING			0x00000080
#define SET_TVE_SYNC_VSOUTOUT_MID			0x00000100
#define SET_TVE_SYNC_VSOUTOUT_START			0x00000200
#define SET_TVE_SYNC_ISYNC_FSI				0x00000400
#define SET_TVE_SYNC_ISYNC_HVFSI			0x00000800
#define SET_TVE_SYNC_ISYNC_HVSI				0x00001000
#define SET_TVE_SYNC_ISYNC_VFSI				0x00002000
#define SET_TVE_SYNC_ISYNC_VSI				0x00004000
#define SET_TVE_SYNC_ISYNC_ESAV_L			0x00008000
#define SET_TVE_SYNC_ISYNC_ESAV_FI			0x00010000
#define SET_TVE_SYNC_ISYNC_FREE				0x00020000

// TVE VBI Control Register Definition -  VCTRL Register
#define SET_TVE_VCTRL_VBICTL_NONE			0x00000001
#define SET_TVE_VCTRL_VBICTL_10LINE			0x00000002
#define SET_TVE_VCTRL_VBICTL_1LINE			0x00000004
#define SET_TVE_VCTRL_VBICTL_2LINE			0x00000008
#define SET_TVE_VCTRL_CAP_ODD_ENABLE		0x00000010
#define SET_TVE_VCTRL_CAP_ODD_DISABLE		0x00000020
#define SET_TVE_VCTRL_CAP_EVEN_ENABLE		0x00000040
#define SET_TVE_VCTRL_CAP_EVEN_DISABLE		0x00000080
#define SET_TVE_VCTRL_CGMS_ODD_ENABLE		0x00000100
#define SET_TVE_VCTRL_CGMS_ODD_DISABLE		0x00000200
#define SET_TVE_VCTRL_CGMS_EVEN_ENABLE		0x00000400
#define SET_TVE_VCTRL_CGMS_EVEN_DISABLE		0x00000800
#define SET_TVE_VCTRL_WSCREEN_ENABLE		0x00001000
#define SET_TVE_VCTRL_WSCREEN_DISABLE		0x00002000

// TVE Connection and I/F Control Definition - VENCON, VENCIF Register
#define SET_TVE_CONIF_CON_ENABLE			0x00000001
#define SET_TVE_CONIF_CON_DISABLE			0x00000002
#define SET_TVE_CONIF_FMT_CINLSB			0x00000004
#define SET_TVE_CONIF_FMT_CINMSB			0x00000008

// TVE Setting Value Definition
enum
{
	DRV_TVE_SET_SCH = 0,
	DRV_TVE_SET_HUE,
	DRV_TVE_SET_SAT,
	DRV_TVE_SET_CONT,
	DRV_TVE_SET_BRIGHT,
	DRV_TVE_SET_FSC,
	DRV_TVE_SET_INPUT,
	DRV_TVE_SET_HOFFSET,
	DRV_TVE_SET_VOFFSET,
	DRV_TVE_SET_HSOE,
	DRV_TVE_SET_HSOB,
	DRV_TVE_SET_VSOB,
	DRV_TVE_SET_VSOE,
	DRV_TVE_SET_VSOST,
	DRV_TVE_SET_NOVRST
};

// TVE Input Format Definition
enum
{
	DRV_TVE_INPUT_BW16_27MHZ = 0,
	DRV_TVE_INPUT_BW16_13P5MHZ,
	DRV_TVE_INPUT_BW8_13P5MHZ
};

// TVE Ouput Mode Definition
enum
{
	DRV_TVE_OUTPUT_NTSC = 0,
	DRV_TVE_OUTPUT_PAL,
	DRV_TVE_OUTPUT_MAX
};

// TVE Ouput Mode Definition
enum
{
	DRV_TVE_CGMS_OFF = 0,
	DRV_TVE_CGMS_ON,
	DRV_TVE_CGMS_MAX
};

// TVE Driver Return Value Definition
#define	DRV_TVE_ERROR_YES		1
#define	DRV_TVE_ERROR_NO		0
#define	DRV_TVE_ERROR_OK		0
enum
{
	// Error for Driver State
	DRV_TVE_ERROR_NOTINIT	= (int)0xF9000000,	// Non-init Function come before init function
	DRV_TVE_ERROR_DUPINIT,						// Init function called more than twice

	// Error for Input Parameter
	DRV_TVE_ERROR_INVALID_FUNC,					// Function Code is out of range
	DRV_TVE_ERROR_INVALID_ARGUMENT,				// Number of Argument is out of range
	DRV_TVE_ERROR_INVALID_HANDLE,				// Handle value is out of range
	DRV_TVE_ERROR_INVALID_RESPONSE,				// No Response

	// Error for Resource Availability
	DRV_TVE_ERROR_NOTAVAILABLE_HANDLE,			// No available handle

	// Error for Illegal State
	DRV_TVE_ERROR_INTERNAL,						// Internal Bug

	DRV_TVE_ERROR_MAX
};

extern S32	DRV_TVE(U32 Func, U32 ArgN, ...);

/*==============================================================================
				AUDIO
   ==============================================================================*/
/*---------------------------------------------------------------------
	DDI_AUD_Set_EQ_FreqVal

Description
	Fix input equation value to global variable
Parameters
	uiFreqValue : input value
Returns
	None
 -----------------------------------------------------------------------*/
void DDI_AUD_Set_EQ_FreqVal(unsigned int uiFreqValue );

/*---------------------------------------------------------------------
	DDI_AUD_Get_EQ_FreqVal

Description
	Get input equation value to global variable
Parameters
	None
Returns
	uiFreqValue
 -----------------------------------------------------------------------*/
unsigned int DDI_AUD_Get_EQ_FreqVal(void);

/*---------------------------------------------------------------------
	UI_DRV_Get_EQ_FreqVal

Description
	Get input equation value to global variable
Parameters
	None
Returns
	uiFreqValue
 -----------------------------------------------------------------------*/
unsigned int DDI_AUD_Get_EQBandInst(void);

/**********************************************************
*
*	Function of
*
*	DDI_AUD_CopyMono
*
*	Input	: pSrc  = pointer of mono PCM data
			  size  = size of PCM data

*	Output	:
			  pLeft = pointer of left output
			  pRight = pointer of right output

*	Return	: none
*
*	Description	:
**********************************************************/
void DDI_AUD_CopyMono( short *pSrc, short *pLeft, short *pRight, unsigned int size );

/**********************************************************
*
*	Function of
*
*	DDI_AUD_CopyStereo
*
*	Input	: pSrc  = pointer of stero PCM data
			  size  = size of PCM data

*	Output	:
			  pLeft = pointer of left output
			  pRight = pointer of right output

*	Return	: none
*
*	Description	:
**********************************************************/
void DDI_AUD_CopyStereo( short *pSrc, short *pLeft, short *pRight, unsigned int size );

/**********************************************************
*
*	Function of
*
*	DDI_AUD_Terminate
*
*	Input	:
*	Output	:
*	Return	: none
*
*	Description	:
**********************************************************/
void DDI_AUD_Terminate(void);

/**********************************************************
*
*	Function of
*
*	DDI_AUD_Initialize
*
*	Input	: iSampleFreq = Sampling frequency
			  iChannel = number of audio channel
*	Output	:
*	Return	: none
*
*	Description	:
**********************************************************/
void DDI_AUD_Initialize( int iSampleFreq, int iChannel );

/**********************************************************
*
*	Function of
*
*	DDI_AUD_Output
*
*	Input	: pSrc  = pointer of PCM data
			  iSize  = size of PCM data
			  iChannel = number of audio channel

*	Output	:
*	Return	: none
*
*	Description	:
**********************************************************/
void DDI_AUD_Output( short *pSrc, int iSize, int iChannel );

/**********************************************************
*
*	Function of
*
*	DDI_AUD_MuteOn
*
*	Input	: none
*	Output	: none
*	Return	: none
*
*	Description	:
**********************************************************/
void DDI_AUD_MuteOn(void);

/**********************************************************
*
*	Function of
*
*	DDI_AUD_MuteOff
*
*	Input	: none
*	Output	: none
*	Return	: none
*
*	Description	:
**********************************************************/
void DDI_AUD_MuteOff(void);

/**********************************************************
*
*	Function of
*
*	DDI_AUD_SetVolume
*
*	Input	: cVol = value of volume level
*	Output	: none
*	Return	: none
*
*	Description	:
**********************************************************/
void DDI_AUD_SetVolume(	unsigned char cVol );

/**********************************************************
*
*	Function of
*
*	DDI_AUD_DisableEffect
*
*	Input	: none
*	Output	: none
*	Return	: none
*
*	Description	:
**********************************************************/
void DDI_AUD_DisableEffect(void);

/**********************************************************
*
*	Function of
*
*	DDI_AUD_SetEffectEQ
*
*	Input	: iSampleFreq
			  pEqValue = pointer of effect value
*	Output	: none
*	Return	: none
*
*	Description	:
**********************************************************/
void DDI_AUD_SetEffectEQ(unsigned int iSampleFreq );

/*---------------------------------------------------------------------
	DDI_AUD_ApplyEqualizerMode

Description
	Set the EQ mode
Parameters
	ucEQMode : selected mode by user
Returns
	None
 -----------------------------------------------------------------------*/
void DDI_AUD_ApplyEqualizerMode(U8	ucEQMode);

/**********************************************************
*
*	Function of
*
*	DDI_AUD_SetSrsWow
*
*	Input	: iSampleFreq
			  iSrs = value of SRS effect
			  iWow = value of WOW effect
*	Output	: none
*	Return	: none
*
*	Description	:
**********************************************************/
void DDI_AUD_SetSrsWow( int iSampleFreq, int iSrs, int iWow);


/*==============================================================================
				SMARTCARD
   ==============================================================================*/

/**********************************************************
*	int	DDI_SC_Initialize(void);
*
*	Input		:
*	Output		:
*	Return		: 0 = initialize successful
*				  negative = initialize failed.
*
*	Description	: Initialize SC library & Create SEMA.
**********************************************************/
int	DDI_SC_Initialize(void);

/**********************************************************
*	int	DDI_SC_Open(void);
*
*	Input		:
*	Output		:
*	Return		: 0 = Open successful, -1 = Open failed.
*
*	Description	: Establish New SC connection.
**********************************************************/
int	DDI_SC_Open(void);

/**********************************************************
*	int	DDI_SC_Close(void);
*
*	Input		:
*	Output		:
*	Return		: 0 = Close successful
*
*	Description	: Close SC connection.
**********************************************************/
int	DDI_SC_Close(void);

/**********************************************************
*	int	DDI_SC_Reset(void *pATR, unsigned *pATRLength);
*
*	Input		: pATR = pointer for storing ATR string
*				  pATRLength = pointer for storing ATR string length
*	Output		:
*	Return		: 0 = Reset successful
*
*	Description	: Reset SmartCard & Get ATR string.
**********************************************************/
int	DDI_SC_Reset(void *pATR, unsigned *pATRLength);

/**********************************************************
*	int	DDI_SC_SendRcv(void *pSend, unsigned uSendLength, void *pRcv, unsigned uRcvLength);
*
*	Input		: pSend = pointer of command string
*				  uSendLength = length of command string
*				  pRcv = pointer for storing response string
*				  uRcvLength = length of response string.
*	Output		:
*	Return		: 0 = Send & Receiving is successful.
*				  -1 = Response string is not good.
*
*	Description	: Send command string to SmartCard & Receive response string from SmartCard.
**********************************************************/
int	DDI_SC_SendRcv(void *pSend, unsigned uSendLength, void *pRcv, unsigned uRcvLength);

#endif

#endif

/* end of file */

