/****************************************************************************
 *   FileName    : sd_regs.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#ifndef _SD_REGS_H
#define _SD_REGS_H

#ifndef BITCLR
	#define	BITSET(X, MASK)				( (X) |= (unsigned)(MASK) )
	#define	BITSCLR(X, SMASK, CMASK)	( (X) = ((((unsigned)(X)) | ((unsigned)(SMASK))) & ~((unsigned)(CMASK))) )
	#define	BITCSET(X, CMASK, SMASK)	( (X) = ((((unsigned)(X)) & ~((unsigned)(CMASK))) | ((unsigned)(SMASK))) )
	#define	BITCLR(X, MASK)				( (X) &= ~((unsigned)(MASK)) )
	#define	ISONE(X, MASK)				( (unsigned)(X) & ((unsigned)(MASK)) )
	#define	ISZERO(X, MASK)				(  ! (((unsigned)(X)) & ((unsigned)(MASK))) )
#endif

/************************************************************************
*   Bit Field Definition
************************************************************************/
#ifndef Hw0
#define Hw37                                    (1LL << 37)
#define Hw36                                    (1LL << 36)
#define Hw35                                    (1LL << 35)
#define Hw34                                    (1LL << 34)
#define Hw33                                    (1LL << 33)
#define Hw32                                    (1LL << 32)
#define Hw31                                    0x80000000
#define Hw30                                    0x40000000
#define Hw29                                    0x20000000
#define Hw28                                    0x10000000
#define Hw27                                    0x08000000
#define Hw26                                    0x04000000
#define Hw25                                    0x02000000
#define Hw24                                    0x01000000
#define Hw23                                    0x00800000
#define Hw22                                    0x00400000
#define Hw21                                    0x00200000
#define Hw20                                    0x00100000
#define Hw19                                    0x00080000
#define Hw18                                    0x00040000
#define Hw17                                    0x00020000
#define Hw16                                    0x00010000
#define Hw15                                    0x00008000
#define Hw14                                    0x00004000
#define Hw13                                    0x00002000
#define Hw12                                    0x00001000
#define Hw11                                    0x00000800
#define Hw10                                    0x00000400
#define Hw9                                     0x00000200
#define Hw8                                     0x00000100
#define Hw7                                     0x00000080
#define Hw6                                     0x00000040
#define Hw5                                     0x00000020
#define Hw4                                     0x00000010
#define Hw3                                     0x00000008
#define Hw2                                     0x00000004
#define Hw1                                     0x00000002
#define Hw0                                     0x00000001
#define HwZERO                                  0x00000000
#endif

#define	BLOCKLEN	512

#define	CMD0		0		// Go Idle State
#define	CMD1		1		// Send Operating Condition
#define	CMD2		2		// All Send CID
#define	CMD3		3		// Set RCA(Relative Card Address)
#define	CMD4		4		// Set DSR(Driver Stage/Strength Register)
#define	CMD6		6		// Switch Mode
#define	CMD7		7		// Select/Deselect Card (Go Transfer State or Go Standby state if RCA == 0)
#define	CMD8		8		// Send Extended CSD (Card Specifiec Data)
#define	CMD9		9		// Send CSD (Card Specific Data)
#define	CMD10		10		// Send CID
#define	CMD11		11		// Stream Read (Read Data Until Stop)
#define	CMD12		12		// Stop
#define	CMD13		13		// Send Status
#define	CMD15		15		// Go Inactive State

#define	CMD16		16		// Set Block Length
#define	CMD17		17		// Block Read
#define	CMD18		18		// Multiple Block Read

#define	CMD20		20		// Stream Write (Write Data Until Stop)

#define	CMD23		23		// Set Block Count
#define	CMD24		24		// Block Write
#define	CMD25		25		// Multiple Block Write
#define	CMD26		26		// Write CID (OTP type, Normally Reserved by Manufacturer)
#define	CMD27		27		// Write CSD (OTP type, Normally Reserved by Manufacturer)

#define	CMD28		28		// Set Write Protect
#define	CMD29		29		// Clear Write Protect
#define	CMD30		30		// Send Write Protect

#define	CMD32		32
#define	CMD33		33
#define	CMD35		35		// Set Start of Erase Group
#define	CMD36		36		// Set End of Erase Group
#define	CMD38		38		// Start Erase

#define	CMD39		39		// FAST IO (Read/Write Card Register)
#define	CMD40		40		// Go IRQ State

#define	CMD42		42		// Lock, Unlock

#define	CMD55		55		// ACMD Start
#define	CMD56		56		// General Purpose Read or Write Command

#define	ACMD6		6
#define	ACMD13		13
#define	ACMD18		18
#define	ACMD22		22
#define	ACMD23		23
#define	ACMD25		25
#define	ACMD26		26
#define	ACMD38		38
#define	ACMD41		41
#define	ACMD42		42
#define	ACMD43		43
#define	ACMD49		49
#define	ACMD51		51

#define	NoRsp		0
#define	RspType1	1
#define	RspType1b	2
#define	RspType2	3
#define	RspType3	4
#define	RspType4	5
#define	RspType5	6
#define	RspType6	7
#define	RspType7	8

#define COMMAND_RTYPE_MASK					(3)
#define COMMAND_RTYPE_NO_RESPONSE			(0)
#define COMMAND_RTYPE_R2					(1)
#define COMMAND_RTYPE_R1_R3_R4_R5_R7		(2)
#define COMMAND_RTYPE_R1b_R5b				(3)

#define HOSTCTRL_SD8						(1<<5)
#define HOSTCTRL_HS							(1<<2)
#define HOSTCTRL_SD4						(1<<1)
#define HOSTCTRL_LED						(1)

#define SW_RESET_ALL						(1)
#define SW_RESET_CMD						(1<<1)
#define SW_RESET_DAT						(1<<2)

#define INTERNAL_CLOCK_ENABLE				(1<<0)
#define INTERNAL_CLOCK_STABLE				(1<<1)
#define SD_CLOCK_ENABLE						(1<<2)

#define NORMAL_INT_CDONE					(1)
#define NORMAL_INT_TDONE					(1<<1)
#define NORMAL_INT_BLKGAP					(1<<2)
#define NORMAL_INT_DMA						(1<<3)
#define NORMAL_INT_WRRDY					(1<<4)
#define NORMAL_INT_RDRDY					(1<<5)
#define NORMAL_INT_CDIN						(1<<6)
#define NORMAL_INT_CDOUT					(1<<7)
#define NORMAL_INT_CDINT					(1<<8)
#define NORMAL_INT_ERR						(1<<15)

#define	HwSD_TCMD_CMDIDX(X)				((X)*Hw24)						// Command Index
#define	HwSD_TCMD_CMDIDX_MASK			HwSD_TCMD_CMDIDX(63)
#define	HwSD_TCMD_CTYPE(X)					((X)*Hw22)						// Suspend, Resume, Abort, Normal
#define	HwSD_TCMD_CTYPE_ABORT				HwSD_TCMD_CTYPE(3)
#define	HwSD_TCMD_CTYPE_RESUME			HwSD_TCMD_CTYPE(2)
#define	HwSD_TCMD_CTYPE_SUSPEND			HwSD_TCMD_CTYPE(1)
#define	HwSD_TCMD_CTYPE_NORMAL			HwSD_TCMD_CTYPE(0)
#define	HwSD_TCMD_CTYPE_MASK				HwSD_TCMD_CTYPE(3)
#define	HwSD_TCMD_DATSEL_PRESENT			Hw21							// Data is present and shall be transferred
#define	HwSD_TCMD_DATSEL_NODATA			HwZERO
#define	HwSD_TCMD_CICHK_EN				Hw20							// Check the Index field in the response
#define	HwSD_TCMD_CRCHK_EN				Hw19							// Check the CRC field in the response
#define	HwSD_TCMD_RTYPE_HC				Hw18							// Response with 48 bytes (HC)
#define	HwSD_TCMD_RTYPE(X)				((X)*Hw16)						// Response Type
#define	HwSD_TCMD_RTYPE_MASK			HwSD_TCMD_RTYPE(3)				// Response Type
#define	HwSD_TCMD_RTYPE_NORESP			HwSD_TCMD_RTYPE(0)				// No Response
#define	HwSD_TCMD_RTYPE_R136			HwSD_TCMD_RTYPE(1)				// Response with 136 bytes
#define	HwSD_TCMD_RTYPE_R48				HwSD_TCMD_RTYPE(2)				// Response with 48 bytes
#define	HwSD_TCMD_RTYPE_R48CHK			HwSD_TCMD_RTYPE(3)				// Response with 48 bytes - check Busy after response
#define	HwSD_TCMD_SPI_MODE				Hw7								// SPI Mode
#define	HwSD_TCMD_SD_MODE				HwZERO							// SD Mode
#define	HwSD_TCMD_ATACMD					Hw6								// CE-ATA Mode (Device will send Command Completion Signal)
#define	HwSD_TCMD_MS_MULTI				Hw5								// Multiple Block Mode
#define	HwSD_TCMD_MS_SINGLE				HwZERO							// Single Block Mode
#define	HwSD_TCMD_DIR_READ				Hw4								// Read Mode (Transfer from Card to Host)
#define	HwSD_TCMD_DIR_WRITE				HwZERO							// Write Mode (Transfer from Host to Card)
#define	HwSD_TCMD_ACMD12_EN				Hw2								// CMD12 is issued automatically after last block transfer is completed.
#define	HwSD_TCMD_BCNTEN_EN				Hw1								// Enable Block Count Register (BCNT). only relevant for multiple block transfers
#define	HwSD_TCMD_DMAEN_EN				Hw0								// DMA Enabled (can be enabled only if CA register tells it support DMA)


#define	HwSD_CTRL_WKOUT_EN				Hw26							// Enable Wakeup event on Card Removal
#define	HwSD_CTRL_WKIN_EN					Hw25							// Enable Wakeup event on Card Insertion
#define	HwSD_CTRL_WKINT_EN					Hw24							// Enable Wakeup event on Card Interrupt
#define	HwSD_CTRL_BGINT_EN					Hw19							// Enable interrupt detection at the block gap for multiple block transfer (valid only 4bit SDIO mode)
#define	HwSD_CTRL_RDWAIT_EN				Hw18							// Enable use of read-wait protocol to stop read data using DAT2 line
#define	HwSD_CTRL_CONREQ_EN				Hw17							// Restart a transaction that was stopped by BGSTOP request
#define	HwSD_CTRL_BGSTOP_EN				Hw16							// Executing transaction stops at the next block gap for non-DMA, SDMA, ADMA transfers.
#define	HwSD_CTRL_VOLTSEL(X)				((X)*Hw9)						// Selects voltage level for the SD Card
#define	HwSD_CTRL_VOLTSEL_V33				HwSD_CTRL_VOLTSEL(7)			// 3.3V Flattop
#define	HwSD_CTRL_VOLTSEL_V30				HwSD_CTRL_VOLTSEL(6)			// 3.0V (typical)
#define	HwSD_CTRL_VOLTSEL_V18				HwSD_CTRL_VOLTSEL(5)			// 1.8V (typical)
#define	HwSD_CTRL_VOLTSEL_MASK			HwSD_CTRL_VOLTSEL(7)
#define	HwSD_CTRL_POW_ON					Hw8								// SD Bus Power On (VOLTSEL should be set ahead)
#define	HwSD_CTRL_DETSEL_TST				Hw7								// Card detect test level is selected (test purpose only)
#define	HwSD_CTRL_DETSEL_SDCD				HwZERO							// SDCD# line is selected (normal case)
#define	HwSD_CTRL_SD8_EN					Hw5								// SD 8bit mode is selected
#define	HwSD_CTRL_SELDMA(X)				((X)*Hw3)						// SD DMA Selection
#define	HwSD_CTRL_SELDMA_SDMA				HwSD_CTRL_SELDMA(0)			// SDMA is selected
#define	HwSD_CTRL_SELDMA_ADMA1			HwSD_CTRL_SELDMA(1)			// ADMA1 (32bit address) is selected
#define	HwSD_CTRL_SELDMA_ADMA2_32			HwSD_CTRL_SELDMA(2)			// ADMA2 (32bit address) is selected
#define	HwSD_CTRL_SELDMA_ADMA2_64			HwSD_CTRL_SELDMA(3)			// ADMA2 (64bit address) is selected
#define	HwSD_CTRL_SELDMA_MASK				HwSD_CTRL_SELDMA(3)
#define	HwSD_CTRL_HS_EN					Hw2								// Enable High Speed
#define	HwSD_CTRL_SD4_EN					Hw1								// SD 4bit mode is selected
#define	HwSD_CTRL_SD1_EN					HwZERO							// SD 1bit mode is selected
#define	HwSD_CTRL_LED_ON					Hw0								// LED ON

#define	HwSD_CLK_RSTDAT_EN				Hw26							// SW Reset for DAT line
													// DATA, STATE(RDEN, WREN, RDACT, WRACT, DATACT, NODAT) are cleared
													// CTRL(CONREQ, BGSTOP), STS(RDRDY,WRRDY,BLKGAP,TDONE) are cleared
#define	HwSD_CLK_RSTCMD_EN				Hw25							// SW Reset for CMD line
													// STATE(NOCMD), STS(CDONE) are cleared
#define	HwSD_CLK_RSTALL_EN					Hw24							// SW Reset for All (except Card Detection Circuit)
#define	HwSD_CLK_TIMEOUT(X)				((X)*Hw16)						// Time Out Selection
#define	HwSD_CLK_TIMEOUT_TM8K				HwSD_CLK_TIMEOUT(0)			// Timeout = TMCLK * 8K
#define	HwSD_CLK_TIMEOUT_TM16K			HwSD_CLK_TIMEOUT(1)			// Timeout = TMCLK * 16K
#define	HwSD_CLK_TIMEOUT_TM32K			HwSD_CLK_TIMEOUT(2)			// Timeout = TMCLK * 32K
#define	HwSD_CLK_TIMEOUT_TM64K			HwSD_CLK_TIMEOUT(3)			// Timeout = TMCLK * 64K
#define	HwSD_CLK_TIMEOUT_TM128K			HwSD_CLK_TIMEOUT(4)			// Timeout = TMCLK * 128K
#define	HwSD_CLK_TIMEOUT_TM256K			HwSD_CLK_TIMEOUT(5)			// Timeout = TMCLK * 256K
#define	HwSD_CLK_TIMEOUT_TM512K			HwSD_CLK_TIMEOUT(6)			// Timeout = TMCLK * 512K
#define	HwSD_CLK_TIMEOUT_TM1M				HwSD_CLK_TIMEOUT(7)			// Timeout = TMCLK * 1M
#define	HwSD_CLK_TIMEOUT_TM2M				HwSD_CLK_TIMEOUT(8)			// Timeout = TMCLK * 2M
#define	HwSD_CLK_TIMEOUT_TM4M				HwSD_CLK_TIMEOUT(9)			// Timeout = TMCLK * 4M
#define	HwSD_CLK_TIMEOUT_TM8M				HwSD_CLK_TIMEOUT(10)			// Timeout = TMCLK * 8M
#define	HwSD_CLK_TIMEOUT_TM16M			HwSD_CLK_TIMEOUT(11)			// Timeout = TMCLK * 16M
#define	HwSD_CLK_TIMEOUT_TM32M			HwSD_CLK_TIMEOUT(12)			// Timeout = TMCLK * 32M
#define	HwSD_CLK_TIMEOUT_TM64M			HwSD_CLK_TIMEOUT(13)			// Timeout = TMCLK * 64M
#define	HwSD_CLK_TIMEOUT_TM128M			HwSD_CLK_TIMEOUT(14)			// Timeout = TMCLK * 128M
#define	HwSD_CLK_TIMEOUT_MASK				HwSD_CLK_TIMEOUT(15)
#define	HwSD_CLK_SDCLKSEL(X)				((X)*Hw8)						// SDCLK Frequency Selection
#define	HwSD_CLK_SDCLKSEL_D256			HwSD_CLK_SDCLKSEL(128)		// SDCLK = base clock / 256
#define	HwSD_CLK_SDCLKSEL_D128			HwSD_CLK_SDCLKSEL(64)			// SDCLK = base clock / 128
#define	HwSD_CLK_SDCLKSEL_D64				HwSD_CLK_SDCLKSEL(32)			// SDCLK = base clock / 64
#define	HwSD_CLK_SDCLKSEL_D32				HwSD_CLK_SDCLKSEL(16)			// SDCLK = base clock / 32
#define	HwSD_CLK_SDCLKSEL_D16				HwSD_CLK_SDCLKSEL(8)			// SDCLK = base clock / 16
#define	HwSD_CLK_SDCLKSEL_D8				HwSD_CLK_SDCLKSEL(4)			// SDCLK = base clock / 8
#define	HwSD_CLK_SDCLKSEL_D4				HwSD_CLK_SDCLKSEL(2)			// SDCLK = base clock / 4
#define	HwSD_CLK_SDCLKSEL_D2				HwSD_CLK_SDCLKSEL(1)			// SDCLK = base clock / 2
#define	HwSD_CLK_SDCLKSEL_D1				HwSD_CLK_SDCLKSEL(0)			// SDCLK = base clock (10MHz ~ 63MHz)
#define	HwSD_CLK_SDCLKSEL_MASK			HwSD_CLK_SDCLKSEL(255)
#define	HwSD_CLK_SCKEN_EN					Hw2								// SDCLK Enable
#define	HwSD_CLK_CLKRDY_STABLE			Hw1								// R, Internal base clock is stable
#define	HwSD_CLK_CLKEN_EN					Hw0								// Internal base clock Enable

#define	HwSD_STS_VENDOR3_ERR				Hw31							// Vendor specific error status3
#define	HwSD_STS_VENDOR2_ERR				Hw30							// Vendor specific error status2
#define	HwSD_STS_VENDOR1_ERR				Hw29							// Vendor specific error status1
#define	HwSD_STS_VENDOR0_ERR				Hw28							// Vendor specific error status0
#define	HwSD_STS_ADMA_ERR					Hw25							// Error detected when ADMA Transfers
#define	HwSD_STS_ACMD12_ERR				Hw24							// One of CMD12ERR register has been set to 1
#define	HwSD_STS_CLIMIT_ERR				Hw23							// Exceeding Current limit
#define	HwSD_STS_DATEND_ERR				Hw22							// 0 detected at the end bit position of read
#define	HwSD_STS_DATCRC_ERR				Hw21							// CRC error detected
#define	HwSD_STS_DATTIME_ERR				Hw20							// Data Timeout error detected
#define	HwSD_STS_CINDEX_ERR				Hw19							// Command Index error detected in the command response
#define	HwSD_STS_CMDEND_ERR				Hw18							// 0 deteced at the end bit position of a command response
#define	HwSD_STS_CMDCRC_ERR				Hw17							// Command CRC error detected
#define	HwSD_STS_CMDTIME_ERR				Hw16							// Command Timeout error detected (no response returned within 64 SDCLKs)
#define	HwSD_STS_ERR						Hw15							// Error detected (One of above flags is set)
#define	HwSD_STS_CDINT						Hw8								// Card Interrupt is generated
#define	HwSD_STS_CDOUT					Hw7								// Card Removed (STATE.CDIN changes from 1 to 0)
#define	HwSD_STS_CDIN						Hw6								// Card Inserted (STATE.CDIN changes from 0 to 1)
#define	HwSD_STS_RDRDY					Hw5								// Buffer Read Ready (STATE.RDEN changes from 0 to 1, and ready to read)
#define	HwSD_STS_WRRDY					Hw4								// Buffer Write Ready (STATE.WREN changes from 0 to 1, and ready to read)
#define	HwSD_STS_DMAINT					Hw3								// DMA Interrupt
#define	HwSD_STS_BLKGAP					Hw2								// Block Gap Event (STATE.DATACT falling in read transfer, STATE.WRACT falling in write transfer)
#define	HwSD_STS_TDONE						Hw1								// Transfer Complete
#define	HwSD_STS_CDONE					Hw0								// The end bit of the command response acquired (except Auto CMD12)


#define	HwSD_STATE_DAT7_HIGH				Hw28							// State of DAT7 line
#define	HwSD_STATE_DAT6_HIGH				Hw27							// State of DAT6 line
#define	HwSD_STATE_DAT5_HIGH				Hw26							// State of DAT5 line
#define	HwSD_STATE_DAT4_HIGH				Hw25							// State of DAT4 line
#define	HwSD_STATE_CMD_HIGH				Hw24							// State of CMD line
#define	HwSD_STATE_DAT3_HIGH				Hw23							// State of DAT3 line
#define	HwSD_STATE_DAT2_HIGH				Hw22							// State of DAT2 line
#define	HwSD_STATE_DAT1_HIGH				Hw21							// State of DAT1 line
#define	HwSD_STATE_DAT0_HIGH				Hw20							// State of DAT0 line
#define	HwSD_STATE_SDWP_HIGH				Hw19							// State of WP line
#define	HwSD_STATE_SDCD_DET				Hw18							// Inverse State of CD# line (Card Detected State)
#define	HwSD_STATE_CDST_STABLE			Hw17							// Card Detect Pin level is stable
#define	HwSD_STATE_CDIN_INSERT				Hw16							// Card has been inserted
#define	HwSD_STATE_RDEN_EN					Hw11							// Buffer Read Enable (Readable data exist in the buffer)
#define	HwSD_STATE_WREN_EN				Hw10							// Buffer Write Enable (Data can be written to the buffer)
#define	HwSD_STATE_RDACT_ACTIVE			Hw9								// Read transfer is active
#define	HwSD_STATE_WRACT_ACTIVE			Hw8								// Write transfer is active
#define	HwSD_STATE_DATACT_ACTIVE			Hw2								// DAT[7:0] line on SD bus is in use
#define	HwSD_STATE_NODAT_INHIBIT			Hw1								// DAT line or Read Transfer is active (No Command using DAT is allowed)
#define	HwSD_STATE_NOCMD_INHIBIT			Hw0								// CMD line is in use (No Command is allowed)

typedef volatile struct
{
	unsigned long	ulSDMA;				// R/W, SDMA System Address
	unsigned short	usBlockSize;
	unsigned short	usBlockCount;
	unsigned long	ulArgument;
	unsigned short	usTransferMode;
	unsigned short	usCommand;
	unsigned long	ulResponse[4];
	unsigned long	ulBufferDataPort;
	unsigned long	ulPresentState;
	unsigned char	ucHostControl;
	unsigned char	ucPowerControl;
	unsigned char	ucBlockGapControl;
	unsigned char	ucWakeupControl;
	unsigned short	usClockControl;
	unsigned char	ucTimeoutControl;
	unsigned char	ucSoftReset;
	unsigned short	usNormalIntStatus;
	unsigned short	usErrorIntStatus;
	unsigned short	usNormalIntStatusEnable;
	unsigned short	usErrorIntStatusEnable;
	unsigned short	usNormalIntSignalEnable;
	unsigned short	usErrorIntSignalEnable;
	unsigned short	usAutoCMD12ErrorStatus;
	unsigned short	usReserved0;
	unsigned long	ulCapabilities;
	unsigned long	ulReserved1;
	unsigned long	MaxCurrentCapabilities;
	// don't care remained register
} SDSLOTREG_T,*PSDSLOTREG_T;

typedef struct _tag_CMD6_STATUS_T
{
	unsigned char ucMaxCurrentConsumptionH;
	unsigned char ucMaxCurrentConsumptionL;
	unsigned char ucFunctionGroup6_H;
	unsigned char ucFunctionGroup6_L;
	unsigned char ucFunctionGroup5_H;
	unsigned char ucFunctionGroup5_L;
	unsigned char ucFunctionGroup4_H;
	unsigned char ucFunctionGroup4_L;
	unsigned char ucFunctionGroup3_H;
	unsigned char ucFunctionGroup3_L;
	unsigned char ucFunctionGroup2_H;
	unsigned char ucFunctionGroup2_L;
	unsigned char ucFunctionGroup1_H;
	unsigned char ucFunctionGroup1_L;
	unsigned char ucSelectedFunctionGroup6_5;
	unsigned char ucSelectedFunctionGroup4_3;
	unsigned char ucSelectedFunctionGroup2_1;
	unsigned char ucDataStructureVersion;
	unsigned char ucBusyGroup6_H;
	unsigned char ucBusyGroup6_L;
	unsigned char ucBusyGroup5_H;
	unsigned char ucBusyGroup5_L;
	unsigned char ucBusyGroup4_H;
	unsigned char ucBusyGroup4_L;
	unsigned char ucBusyGroup3_H;
	unsigned char ucBusyGroup3_L;
	unsigned char ucBusyGroup2_H;
	unsigned char ucBusyGroup2_L;
	unsigned char ucBusyGroup1_H;
	unsigned char ucBusyGroup1_L;
	unsigned char reserved[34];
} CMD6_STATUS_T;

typedef struct CIDREG_t
{
	int MID;		//Manufacture ID		08bits	[127:120] 
	int OID;		//Application ID		16bits	[119:104]
	int PNM_U;	//Product Name			40bits	[103:64]
	int PNM_L;
	int PRV;		//Product Version		08bits	[63:56]
	int PSN;		//Product Serial number	32bits	[55:24]
	int MDT;		//Manufacture Date		12bits	[19:8]
	int CRC;		//CRC7 CheckSum		07bits	[7:1]
} sCIDREG;

typedef struct CSDREG_t
{
	unsigned	char CSDS;					//CSD Structure								02bits	[127:126]
	unsigned	char SPEC_VERS;				// System Specification Version						04bits	[125:122]
	unsigned	char TAAC;					//Data Read Access Time-1						08bits	[119:112]
	unsigned	char NSAC;					//Data Read Access Time-2 in CLK cycles(NSAC*100) 	08bits	[111:104]
	unsigned	char TRAN_SPEED;				//Max Data Transfer Rate							08bits	[103:96]
	unsigned	short CCC;					//Card Command Classes							12bits	[95:84]
	unsigned	char READ_BL_LEN;			//Max Read Data Block Length						04bits	[83:80]
	unsigned	char READ_BL_PARTIAL;		//Partial blocks for read allowed						01bits	[79:79]
	unsigned	char WRITE_BL_MISALIGN;		//											01bits	[78:78]
	unsigned	char READ_BL_MISALIGN;		//											01bits	[77:77]
	unsigned	char DSR_IMP;				//											01bits	[76:76]
	unsigned	long C_SIZE;					//Device Size									12bits	[73:62]
	unsigned	char VDD_R_CURR_MIN;		//											03bits	[61:59]
	unsigned	char VDD_R_CURR_MAX;		//											03bits	[58:56]
	unsigned	char VDD_W_CURR_MIN;		//											03bits	[55:53]
	unsigned	char VDD_W_CURR_MAX;		//											03bits	[52:50]
	unsigned	char C_SIZE_MULT;			//Device Size Multiplier							03bits	[49:47]
	unsigned	char ERASE_BL_EN;			//											01bits	[46:46]
	unsigned	char SECTOR_SIZE;			//											07bits	[45:39]
	unsigned	char WP_GRP_SIZE;			//											07bits	[38:32]
	unsigned	char WP_GRP_ENABLE;			//											01bits	[31:31]
	unsigned	char R2W_FACTOR;			//Write Speed Factor								03bits	[28:26]
	unsigned	char WRITE_BL_LEN;			//											04bits	[25:22]
	unsigned	char WRITE_BL_PARTIAL;		//											01bits	[21:21]
	unsigned	char FILE_FORMAT_GRP;		//											01bits	[15:15]
	unsigned	char COPY;					//											01bits	[14:14]
	unsigned	char PERM_WRITE_PROTECT;	//											01bits	[13:13]
	unsigned	char TMP_WRITE_PROTECT;		//											01bits	[12:12]
	unsigned	char FILE_FORMAT;			//											02bits	[11:10]
	unsigned	char CRC;					//											07bits	[7:1]

	unsigned	RealTAAC;					// Taac with 2.67 us unit
	unsigned	TranFreq;					// Transfer Freq with 100 Hz unit
} sCSDREG;

struct csd_441 {	// B120040-ksjung
	unsigned char CSD_STRUCTURE;
	unsigned char SPEC_VERS;
	unsigned char TAAC;
	unsigned char NSAC;
	unsigned char TRAN_SPEED;
	unsigned char CCC;
	unsigned char READ_BL_LEN;
	unsigned char READ_BL_PARTIAL;
	unsigned char WRITE_BLK_MISALIGN;
	unsigned char READ_BLK_MISALIGN;
	unsigned char DSR_IMP;
	unsigned long C_SIZE;
	unsigned char VDD_R_CURR_MIN;
	unsigned char VDD_R_CURR_MAX;
	unsigned char VDD_W_CURR_MIN;
	unsigned char VDD_W_CURR_MAX;
	unsigned char C_SIZE_MULT;
	unsigned char ERASE_BLK_EN;
	unsigned long SECTOR_SIZE;
	unsigned long WP_GRP_SIZE;
	unsigned char WP_GRP_ENABLE;
	unsigned char ERASE_GRP_SIZE;
	unsigned char ERASE_GRP_MULT;
	unsigned char DEFAULT_ECC;
	unsigned char R2W_FACTOR;
	unsigned char WRITE_BL_LEN;
	unsigned char WRITE_BL_PARTIAL;
	unsigned char CONTENT_PROT_APP;
	unsigned char FILE_FORMAT_GRP;
	unsigned char COPY;
	unsigned char PERM_WRITE_PROTECT;
	unsigned char TMP_WRITE_PROTECT;
	unsigned char FILE_FORMAT;
	unsigned char ECC;
	unsigned char CRC;
};


typedef struct EXTCSDREG_t		//structure of Extended CSD register
{
	unsigned char Reserved133[134];
	unsigned char SecBadBlkMgmnt;		// Bad Block Management mode
	unsigned char Reserved135;
	unsigned char EnhStartAddr[4];		// 
	unsigned char EnhSizeMult[3];
	unsigned char GpSizeMult[12];
	unsigned char PartitionSetComplete;
	unsigned char PartitionAttr;
	unsigned char MaxEnhSizeMult[3];
	unsigned char PartitionSupport;
	unsigned char Reserved161;
	unsigned char RstnFunc;
	unsigned char Reserved163[5];
	unsigned char RPMBSizeMult;
	unsigned char FWConfig;
	unsigned char Reserved170;
	unsigned char UserWP;
	unsigned char Reserved172;
	unsigned char BootWP;
	unsigned char Reserved174;
	unsigned char EraseGroupDef;
	unsigned char Reserved176;
	unsigned char BootBusWidth;
	unsigned char BootConfigProt;
	unsigned char PartitionConfig;
	unsigned char Reserved180;
	unsigned char ErasedMemCont;	// initial value after erase operation
	unsigned char Reserved182;
	unsigned char BusWidth;			// bus width
	unsigned char Reserved184;
	unsigned char HSTiming;			// high speed timing
	unsigned char Reserved186;
	unsigned char PowClass;			// power class 
	unsigned char Reserved188;
	unsigned char CmdSetRev;		// command set revision
	unsigned char Reserved190;
	unsigned char CmdSet;			// currently active command in the card
	unsigned char EXTCSDRev;		// EXT_CSD revision
	unsigned char Reserved193;
	unsigned char CSDStruct;		// CSD Structure field in CSD registe
	unsigned char Reserved195;
	unsigned char CardType;			// MMC card type
	unsigned char Reserved197[3];
	unsigned char PwrCl52195;
	unsigned char PwrCl26195;
	unsigned char PwrCl52360;
	unsigned char PwrCl26360;		//supported power class by the card
	unsigned char Reserved204;
	unsigned char MinPerfR0426;		//min Read performance  at 4bit bus width & 26MHz	
	unsigned char MinPerfW0426;		//min Write performance  at 4bit bus width & 26MHz	
	unsigned char MinPerfR08260452;	//min Read performance  at 8bit bus width & 26MHz host clock or 4bit bus width & 52MHz
	unsigned char MinPerfW08260452;	//min Write performance  at 8bit bus width & 26MHz host clock or 4bit bus width & 52MHz
	unsigned char MinPerfR0852;		//min Read performance  at 8bit bus width & 52MHz host clock
	unsigned char MinPerfW0852;		//min write performance  at 8bit bus width & 52MHz host clock
	unsigned char Reserved211;
	unsigned long SecCount;		//sector count
	unsigned char Reserved216;
	unsigned char SATimeout;
	unsigned char Reserved218;
	unsigned char SCVCCQ;
	unsigned char SCVCC;
	unsigned char HcWpGrpSize;
	unsigned char RelWrSecC;
	unsigned char EraseTimeoutMult;
	unsigned char HcEraseGrpSize;
	unsigned char AccSize;
	unsigned char BootSizeMult;
	unsigned char Reserved227;
	unsigned char BootInfo;
	unsigned char SecTrimMult;
	unsigned char SecEraseMult;
	unsigned char SecFeatureSupport;
	unsigned char TrimMult;
	unsigned char Reserved233;
	unsigned char MinPerDDRR852;
	unsigned char MinPerDDRW852;
	unsigned char Reserved236[2];
	unsigned char PwrClDDR52195;
	unsigned char PwrClDDR52360;
	unsigned char Reserved240;
	unsigned char IniTimeoutAP;
	unsigned char Reserved242[262];
	unsigned char sCmdSet;			//command sets are supported by the card
	unsigned char Reserved505[7];
} sEXTCSDREG;

struct ext_csd_441 {	// B120040-ksjung
	unsigned char Reserved_133[134];  // [133:0]
	unsigned char SEC_BAD_BLK_MGMNT;  // [134]      Bad Block Management Mode
	unsigned char Reserved_135;       // [135]
	unsigned long ENH_START_ADDR;     // [139:136]  Enhanced User Data Start Address
	unsigned char ENH_SIZE_MULT[3];   // [142:140]  Enhanced User Data Area Size
	unsigned char GP_SIZE_MULT[12];   // [154:143]  General Purpose Partition Size
	unsigned char PARTITION_SETTING_COMPLETED;//[155] Partitioning Setting
	unsigned char PARTITIONS_ATTRIBUTE;//[156]      Partitions Attribute
	unsigned char MAX_ENH_SIZE_MULT[3];//[159:157]  Max Enhanced Area Size
	unsigned char PARTITIONING_SUPPORT;//[160]      Partitioning Support
	unsigned char HPI_MGMT;           // [161]      HPI management
	unsigned char RST_n_FUNCTION;     // [162]      H/W Reset Function
	unsigned char BKOPS_EN;           // [163]      Enable Background Operations Handshake
	unsigned char BKOPS_START;        // [164]      Manually Start Background Operations
	unsigned char Reserved_165;       // [165]
	unsigned char WR_REL_PARAM;       // [166]      Write Reliability Parameter Register
	unsigned char WR_REL_SET;         // [167]      Write Reliability Setting Register
	unsigned char RPMB_SIZE_MULT;     // [168]      RPMB Size
	unsigned char FW_CONFIG;          // [169]      FW Configuration
	unsigned char Reserved_170;       // [170]
	unsigned char USER_WP;            // [171]      User Area Write Protection Register
	unsigned char Reserved_172;       // [172]
	unsigned char BOOT_WP;            // [173]      Boot Area Write Protection Register
	unsigned char Reserved_174;       // [174]
	unsigned char ERASE_GROUP_DEF;    // [175]      High-density Erase Group Definition
	unsigned char Reserved_176;       // [176]
	unsigned char BOOT_BUS_WIDTH;     // [177]      Boot Bus Width1
	unsigned char BOOT_CONFIG_PROT;   // [178]      Boot Config Protection
	unsigned char PARTITION_CONFIG;   // [179]      Partition Configuration
	unsigned char Reserved_180;       // [180]
	unsigned char ERASED_MEM_CONT;    // [181]      Erased Memory Content
	unsigned char Reserved_182;       // [182]
	unsigned char BUS_WIDTH;          // [183]      Bus Width Mode
	unsigned char Reserved_184;       // [184]
	unsigned char HS_TIMING;          // [185]      High-speed Interface Timing
	unsigned char Reserved_186;       // [186]
	unsigned char POWER_CLASS;        // [187]      Power Class
	unsigned char Reserved_188;       // [188]
	unsigned char CMD_SET_REV;        // [189]      Command Set Revision
	unsigned char Reserved_190;       // [190]
	unsigned char CMD_SET;            // [191]      Command Set
	unsigned char EXT_CSD_REV;        // [192]      Extended CSD Revision
	unsigned char Reserved_193;       // [193]
	unsigned char CSD_STRUCTURE;      // [194]      CSD Structure Version
	unsigned char Reserved_195;       // [195]
	unsigned char CARD_TYPE;          // [196]      Card Type
	unsigned char Reserved_197;       // [197]
	unsigned char OUT_OF_INTERRUPT_TIME;//[198]     Out-of-Interrupt Busy Timing
	unsigned char PARTITION_SWITCH_TIME;//[199]     Partition Switching Timing
	unsigned char PWR_CL_52_195;      // [200]      Power Class for 52MHz at 1.95V
	unsigned char PWR_CL_26_195;      // [201]      Power Class for 26MHz at 1.95V
	unsigned char PWR_CL_52_360;      // [202]      Power Class for 52MHz at 3.6V
	unsigned char PWR_CL_26_360;      // [203]      Power Class for 26MHz at 3.6V
	unsigned char Reserved_204;       // [204]
	unsigned char MIN_PERF_R_4_26;    // [205]      Minimum Read Performance for 4bit at 26MHz
	unsigned char MIN_PERF_W_4_26;    // [206]      Minimum Write Performance for 4bit at 26MHz
	unsigned char MIN_PERF_R_8_26_4_52; // [207]    Minimum Read Performance for 8bit at 26MHz, for 4bit at 52MHz
	unsigned char MIN_PERF_W_8_26_4_52; // [208]    Minimum Write Performance for 8bit at 26MHz, for 4bit at 52MHz
	unsigned char MIN_PERF_R_8_52;    // [209]      Minimum Read Performance for 8bit at 52MHz
	unsigned char MIN_PERF_W_8_52;    // [210]      Minimum Write Performance for 8bit at 52MHz
	unsigned char Reserved_211;       // [211]
	unsigned long SEC_COUNT;          // [215:212]  Sector Count
	unsigned char Reserved_216;       // [216]
	unsigned char S_A_TIMEOUT;        // [217]      Sleep/awake Timeout
	unsigned char Reserved_218;       // [218]
	unsigned char S_C_VCCQ;           // [219]      Sleep Current (VCCQ)
	unsigned char S_C_VCC;            // [220]      Sleep Current (VCC)
	unsigned char HC_WP_GRP_SIZE;     // [221]      High-capacity Write Protect Group Size
	unsigned char REL_WR_SEC_C;       // [222]      Reliable Write Sector Count
	unsigned char ERASE_TIMEOUT_MULT; // [223]      High-capacity Erase Timeout
	unsigned char HC_ERASE_GRP_SIZE;  // [224]      High-capacity Erase Unit Size
	unsigned char ACC_SIZE;           // [225]      Access Size
	unsigned char BOOT_SIZE_MULTI;    // [226]      Boot Partition Size
	unsigned char Reserved_227;       // [227]
	unsigned char BOOT_INFO;          // [228]      Boot Information
	unsigned char SEC_TRIM_MULT;      // [229]      Secure TRIM Multiplier
	unsigned char SEC_ERASE_MULT;     // [230]      Secure Erase Multiplier
	unsigned char SEC_FEATURE_SUPPORT;// [231]      Secure Feature Support
	unsigned char TRIM_MULT;          // [232]      TRIM Multiplier
	unsigned char Reserved_233;       // [233]
	unsigned char MIN_PERF_DDR_R_8_52;// [234]      Minimum Read Performance for 8bit at 52MHz in DDR Mode
	unsigned char MIN_PERF_DDR_W_8_52;// [235]      Minimum Write Performance for 8bit at 52MHz in DDR Mode
	unsigned char Reserved_237[2];    // [237:236]
	unsigned char PWR_CL_DDR_52_195;  // [238]      Power Class for 52MHz, DDR at 1.95v
	unsigned char PWR_CL_DDR_52_360;  // [239]      Power Class for 52MHz, DDR at 3.6v
	unsigned char Reserved_240;       // [240]
	unsigned char INI_TIMEOUT_AP;     // [241]      1st Initialization Time After Partitioning
	unsigned long BORRECTLY_PRG_SECTORS_NUM;  // [245:242]  Number of Correctly Programmed Sectors
	unsigned char BKOPS_Status;       // [246]      Background Operations Status
	unsigned char Reserved_501[255];  // [501:247]
	unsigned char BKOPS_SUPPORT;      // [502]      Background Operations Status
	unsigned char HPI_FEATURES;       // [503]      HPI features
	unsigned char S_CMD_SET;          // [504]      Supported Command Sets
	unsigned char Reserved_511[7];    // [511:505]
};

struct sd_status {	// B120040
	unsigned long reserved_391;
	unsigned long UHS_AU_SIZE;	
	unsigned long UHS_SPEED_GRADE;
	unsigned long ERASE_OFFSET;
	unsigned long ERASE_TIMEOUT;
	unsigned long ERASE_SIZE;
	unsigned long reserved_427;
	unsigned long AU_SIZE;
	unsigned long PERFORMANCE_MOVE;
	unsigned long SPEED_CLASS;
	unsigned long SIZE_OF_PROTECTED_AREA;
	unsigned long SD_CARD_TYPE;
	unsigned long reserved_508;
	unsigned long SECURED_MODE;
	unsigned long DAT_BUS_WIDTH;
};

typedef struct SCRREG_t
{
	int SCRS;						//SCR Structure					04bits	[63:60]
	int SD_SPEC;						//SD Memory Card-spec.Version	04bits	[59:56]
	int DATA_STAT_AFTER_ERASE;		//							01bits	[55:55]
	int SD_SECURITY;					//							03bits	[54:52]
	int SD_BUS_WIDTH;				//							04bits	[51:48]
} sSCRREG;

typedef struct SCR_Register_t
{
	unsigned int sd_bus_widths :4;
	unsigned int sd_security : 3;
	unsigned int data_stat_after_erase : 1;
	unsigned int sd_spec : 4;
	unsigned int scr_structure : 4;
} sSCR_Register;

#endif //_SD_REGS_H
