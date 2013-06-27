#ifndef __FWDN_PROTOCOL_V7__
#define __FWDN_PROTOCOL_V7__

////////////////////////////////////////////////////////////////////////////
//
//				FWDN Protocol V7
//
////////////////////////////////////////////////////////////////////////////
#define FEATURE_FWDN_COMM_VTC


////////////////////////////////////////////////////////////////////////////
//		FWDN Command
////////////////////////////////////////////////////////////////////////////
#define FWDN_CMD_NONE							0x0000

#define FWDN_CMD_UBS_PING						0x0001
#define FWDN_CMD_UBS_INITCODE_LOAD				0x0002
#define FWDN_CMD_UBS_ROM_LOAD					0x0003

#define FWDN_CMD_DEVICE_RESET					0x0100
#define FWDN_CMD_DEVICE_PING					0x0101
#define FWDN_CMD_DEVICE_INIT					0x0102
#define FWDN_CMD_DEVICE_INFO_READ				0x0103
#define FWDN_CMD_DEVICE_SESSION_START			0x0104
#define FWDN_CMD_DEVICE_SESSION_END				0x0105
#define FWDN_CMD_DEVICE_SERIAL_NUMBER_WRITE		0x0106
#define FWDN_CMD_DEVICE_FIRMWARE_WRITE			0x0107
#define FWDN_CMD_DEVICE_MESSAGE_SERVICE			0x0108

#define FWDN_CMD_AREA_INFO_READ					0x0200
#define FWDN_CMD_AREA_WRITE						0x0201
#define FWDN_CMD_AREA_READ						0x0202
#define FWDN_CMD_AREA_CALC_CRC					0x0203

#define FWDN_CMD_DUMP_INFO_READ					0x0300
#define FWDN_CMD_DUMP_BLOCK_READ				0x0301
#define FWDN_CMD_DUMP_BLOCK_WRITE				0x0302
#define FWDN_CMD_DUMP							0x0303

#define FWDN_CMD_TOTALBIN_WRITE					0x0400
#define FWDN_CMD_TOTALBIN_READ					0x0401

#define FWDN_CMD_TNFTL_V5_DEBUG					0x0F00

#define	FWDN_CMD_TEST_SEND						0xEF00
#define FWDN_CMD_TEST_RECEIVE					0xEF01

/////////////////////////////////////////////////////////
//	0xF000 ~ 0xFFFF is reserved for vendor
/////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//		FWDN Response Ack Type
////////////////////////////////////////////////////////////////////////////
#define FWDN_RSP_NACK						0x00
#define FWDN_RSP_ACK						0x01
#define FWDN_RSP_NYET						0x02
#define FWDN_RSP_MSG						0x03


////////////////////////////////////////////////////////////////////////////
//		FWDN Command / Response Signatures
////////////////////////////////////////////////////////////////////////////
#define FWDN_COMMAND_SIGNATURE				0x43445746L		//"FWDC" ; FWDn Command
#define FWDN_RESPONSE_SIGNATURE				0x52445746L		//"FWDR" ; FWDn Response


#define FWDN_EXTRA_RSP_MAX_SIZE				0xFF


#if defined(_WINCE_)
#pragma pack(push, 1)
#endif

typedef struct _tag_FWDN_COMMAND_T
{
	unsigned long	Signature;
	unsigned short	CmdType;
	unsigned short	SubCmdType;
	unsigned long	ExtraCmdSize;
	unsigned long	Param0;
	unsigned long	Param1;
	unsigned long	Param2;
	unsigned long	Param3;
	unsigned long	Param4;

#if defined(_LINUX_)
} __attribute__((packed)) FWDN_COMMAND_T;
#else
} FWDN_COMMAND_T;
#endif

typedef struct _tag_FWDN_RESPONSE_T
{
	unsigned long	Signature;
	unsigned short	CmdType;
	unsigned char	AckType;
	unsigned char	reserved;
 	unsigned long	DataSize;
	unsigned long	Param0;
	unsigned long	Param1;
	unsigned long	Param2;

#if defined(_LINUX_)
} __attribute__((packed)) FWDN_RESPONSE_T;
#else
} FWDN_RESPONSE_T;
#endif

#if defined(_WINCE_)
#pragma pack(pop)
#endif

typedef enum {
	FWDN_COMM_VTC,
	FWDN_COMM_CDC
} FWDN_COMM_T;

void FWDN_PROT_SetComm(FWDN_COMM_T commType);
void FWDN_PROT_CheckCommand(void);

#endif //__FWDN_PROTOCOL_V7__
