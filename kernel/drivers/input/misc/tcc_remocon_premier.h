/*
 * IR driver for remote controller : tcc_remocon_premier.h
 *
 * Copyright (C) 2010 Telechips, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __TCC_REMOCON_PREMIER_H__
#define __TCC_REMOCON_PREMIER_H__

//*******************************************
//	Remote controller define
//*******************************************

#define IR_SIGNAL_MAXIMUN_BIT_COUNT	32
#define IR_FIFO_READ_COUNT			17
#define IR_BUFFER_BIT_SHIFT			16
#define IR_ID_MASK				0x0000FFFF
#define IR_CODE_MASK			0xFFFF0000

//Low Bit
#define LOW_MIN			0x0700
#define LOW_MAX			0x0FFF
//High Bit
#define HIGH_MIN		0x1800
#define HIGH_MAX		0x1E00
//Start Bit
#define START_MIN		0x4000
#define START_MAX		0x7000
//Repeat Bit
#define REPEAT_MIN		0x2000
#define REPEAT_MAX		0x2800

#define LOW_MIN_TCC892X		LOW_MIN * 2
#define LOW_MAX_TCC892X		LOW_MAX * 2
#define HIGH_MIN_TCC892X	HIGH_MIN * 2
#define HIGH_MAX_TCC892X	HIGH_MAX * 2
#define START_MIN_TCC892X	START_MIN * 2
#define START_MAX_TCC892X	START_MAX * 2
#define REPEAT_MIN_TCC892X	REPEAT_MIN * 2
#define REPEAT_MAX_TCC892X	REPEAT_MAX * 2


/*****************************************************************************
*
* IR remote controller scancode
*
******************************************************************************/
#define REM_ID_ERROR		0xFF

#define REMOCON_ID			0xFF00	//vendor ID, prodcut ID
#define REMOCON_REPEAT		0xaaaaaaac

#define SCAN_PWR			0xA659
#define SCAN_MUTE			0xE619

#define SCAN_EPG			0xB24D
#define SCAN_INFO			0xF10E
#define SCAN_TTX			0xF20D
#define SCAN_AUDIO			0xAE51
#define SCAN_PVR			0xED12
#define SCAN_SUBTITLE		0xEE11

#define SCAN_MENU			0xBA45
#define SCAN_CANCEL			0xFA05

#define SCAN_UP				0xF906
#define SCAN_DOWN			0xE916
#define SCAN_LEFT			0xA55A
#define SCAN_RIGHT			0xE41B
#define SCAN_ENTER			0xE51A	//center, enter

#define SCAN_FAVOURITE		0xAA55
#define SCAN_TV_RADIO		0xEA15
#define SCAN_RECALL			0xEC13
#define SCAN_GOTO			0xE817

#define SCAN_NUM_1			0xAD52
#define SCAN_NUM_2			0xAF50
#define SCAN_NUM_3			0xEF10
#define SCAN_NUM_4			0xA956
#define SCAN_NUM_5			0xAB54
#define SCAN_NUM_6			0xEB14
#define SCAN_NUM_7			0xB14E
#define SCAN_NUM_8			0xB34C
#define SCAN_NUM_9			0xF30C
#define SCAN_NUM_0			0xF00F

#define SCAN_REV			0xB54A
#define SCAN_FWD			0xB748
#define SCAN_PREV			0xF708
#define SCAN_NEXT			0xF40B

#define SCAN_PLAY			0xB946
#define SCAN_PAUSE			0xBB44
#define SCAN_STOP			0xFB04
#define SCAN_REPEAT			0xF807

#define SCAN_RED            0xBD42
#define SCAN_GREEN          0xBF40
#define SCAN_YELLOW         0xFF00
#define SCAN_BLUE           0xFC03

/*****************************************************************************
*
* External Variables
*
******************************************************************************/
static SCANCODE_MAPPING key_mapping[] = 
{
	{SCAN_PWR, 			REM_POWER},
	{SCAN_MUTE, 		REM_VOLUME_MUTE},

	{SCAN_EPG, 			REM_GUIDE},
	{SCAN_INFO, 		REM_INFO},
	{SCAN_TTX,			REM_WINDOW},
	{SCAN_AUDIO, 		REM_LANGUAGE_SWITCH},
	{SCAN_PVR,			REM_MEDIA_RECORD},
	{SCAN_SUBTITLE,		REM_CAPTIONS},

	{SCAN_MENU,			REM_MENU},
	{SCAN_CANCEL,		REM_BACK},

#if 1
	{SCAN_UP,			REM_DPAD_UP},
	{SCAN_DOWN, 		REM_DPAD_DOWN}, 
#else
	{SCAN_UP,			REM_PAGE_UP},
	{SCAN_DN,			REM_PAGE_DOWN},
#endif
#if 1
	{SCAN_LEFT, 		REM_DPAD_LEFT},
	{SCAN_RIGHT,		REM_DPAD_RIGHT},
#else
	{SCAN_LEFT,			REM_VOLUME_UP},
	{SCAN_RIGHT,		REM_VOLUME_DOWN},
#endif
#if 0
	{SCAN_ENTER,		REM_DPAD_CENTER},
#else
	{SCAN_ENTER,		REM_ENTER},
#endif

	{SCAN_FAVOURITE,	REM_BOOKMARK},
	{SCAN_TV_RADIO,		REM_TV},
	{SCAN_RECALL,		REM_FUNCTION},
	{SCAN_GOTO,			REM_HOME},

	{SCAN_NUM_1,		REM_1},
	{SCAN_NUM_2,		REM_2},
	{SCAN_NUM_3,		REM_3},
	{SCAN_NUM_4,		REM_4},
	{SCAN_NUM_5,		REM_5},
	{SCAN_NUM_6,		REM_6},
	{SCAN_NUM_7,		REM_7},
	{SCAN_NUM_8,		REM_8},
	{SCAN_NUM_9,		REM_9},
	{SCAN_NUM_0,		REM_0},

	{SCAN_REV,			REM_MEDIA_REWIND},
	{SCAN_FWD,			REM_MEDIA_FAST_FORWARD},
	{SCAN_PREV,			REM_MEDIA_PREVIOUS},
	{SCAN_NEXT,			REM_MEDIA_NEXT},

	{SCAN_STOP, 		REM_MEDIA_STOP},
	{SCAN_PLAY,			REM_MEDIA_PLAY},
	{SCAN_PAUSE,		REM_MEDIA_PAUSE},
	{SCAN_REPEAT,		REM_FUNCTION_F2},

	{SCAN_RED, 			REM_PROG_RED},
	{SCAN_GREEN, 		REM_PROG_GREEN},
	{SCAN_YELLOW,		REM_PROG_YELLOW},
	{SCAN_BLUE,			REM_PROG_BLUE},
};

#endif	// __TCC_REMOCON_PREMIER_H__

