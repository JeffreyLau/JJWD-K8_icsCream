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

#define REMOCON_ID			0xFE01        //vendor ID, prodcut ID
#define REMOCON_REPEAT		0xaaaaaaac

#define SCAN_POWER			0xE31C
#define SCAN_MUTE			0xF708

#define SCAN_RED            0xA956
#define SCAN_GREEN          0xA857
#define SCAN_YELLOW         0xE01F
#define SCAN_BLUE           0xA45B

#define SCAN_INFO			0xAA55
#define SCAN_GOTO			0xA15E
#define SCAN_SUBTITLE		0xAE51
#define SCAN_AUDIO			0xA659

#define SCAN_RECORD			0xA55A
#define SCAN_VIDEO_MODE		0xAD52
#define SCAN_STOP			0xA25D
#define SCAN_PLAY_PAUSE		0xA35C

#define SCAN_REWIND			0xFE01
#define SCAN_FAST_FORWARD	0xA05F
#define SCAN_PREVIOUS		0xE619
#define SCAN_NEXT			0xA758

#define SCAN_HOME			0xE718
#define SCAN_BACK			0xE817
#define SCAN_SETUP			0xB649
#define SCAN_TASK			0xF50A

#define SCAN_UP				0xE51A
#define SCAN_DOWN			0xB748
#define SCAN_LEFT			0xB847
#define SCAN_RIGHT			0xF807
#define SCAN_OK				0xF906

#define SCAN_VOLUME_UP		0xB44B
#define SCAN_VOLUME_DOWN	0xB04F

#define SCAN_PAGE_UP		0xF609
#define SCAN_PAGE_DOWN		0xFA05

#define SCAN_SEARCH			0xFC03
#define SCAN_APP			0xBD42

#define SCAN_NUM_1			0xAB54
#define SCAN_NUM_2			0xE916
#define SCAN_NUM_3			0xEA15

#define SCAN_NUM_4			0xAF50
#define SCAN_NUM_5			0xED12
#define SCAN_NUM_6			0xEE11

#define SCAN_NUM_7			0xB34C
#define SCAN_NUM_8			0xF10E
#define SCAN_NUM_9			0xF20D

#define SCAN_POUND			0xBE41
#define SCAN_NUM_0			0xF30C
#define SCAN_DELETE			0xEF10

/*****************************************************************************
*
* External Variables
*
******************************************************************************/
static SCANCODE_MAPPING key_mapping[] = 
{
	{SCAN_POWER, 		REM_POWER},
	{SCAN_MUTE, 		REM_VOLUME_MUTE},

	{SCAN_RED, 			REM_TV},
	{SCAN_GREEN, 		REM_GUIDE},
	{SCAN_YELLOW, 		REM_BOOKMARK},
	{SCAN_BLUE, 		REM_MENU},

	{SCAN_INFO, 		REM_INFO},
	{SCAN_GOTO, 		REM_WINDOW},
	{SCAN_SUBTITLE,		REM_CAPTIONS},
	{SCAN_AUDIO, 		REM_LANGUAGE_SWITCH},

	{SCAN_RECORD, 		REM_MEDIA_RECORD},
	{SCAN_VIDEO_MODE,	REM_FUNCTION},
	{SCAN_STOP, 		REM_MEDIA_STOP},
	{SCAN_PLAY_PAUSE,	REM_MEDIA_PLAY_PAUSE},

	{SCAN_REWIND,		REM_MEDIA_REWIND},
	{SCAN_FAST_FORWARD,	REM_MEDIA_FAST_FORWARD},
	{SCAN_PREVIOUS, 	REM_MEDIA_PREVIOUS},
	{SCAN_NEXT,			REM_MEDIA_NEXT},

	{SCAN_HOME, 		REM_HOME},
	{SCAN_BACK,			REM_BACK},
	{SCAN_SETUP, 		REM_SETTINGS},
	{SCAN_TASK,			REM_APP_SWITCH},

	{SCAN_UP, 			REM_DPAD_UP},
	{SCAN_DOWN,			REM_DPAD_DOWN},
	{SCAN_LEFT, 		REM_DPAD_LEFT},
	{SCAN_RIGHT,		REM_DPAD_RIGHT},
	{SCAN_OK, 			REM_ENTER},

	{SCAN_VOLUME_UP,	REM_VOLUME_UP},
	{SCAN_VOLUME_DOWN,	REM_VOLUME_DOWN},

	{SCAN_PAGE_UP, 		REM_PAGE_UP},
	{SCAN_PAGE_DOWN,	REM_PAGE_DOWN},

	{SCAN_SEARCH, 		REM_SEARCH},
	{SCAN_APP,			REM_FUNCTION_F2},

	{SCAN_NUM_1, 		REM_1},
	{SCAN_NUM_2,		REM_2},
	{SCAN_NUM_3, 		REM_3},

	{SCAN_NUM_4, 		REM_4},
	{SCAN_NUM_5,		REM_5},
	{SCAN_NUM_6, 		REM_6},

	{SCAN_NUM_7, 		REM_7},
	{SCAN_NUM_8,		REM_8},
	{SCAN_NUM_9, 		REM_9},

	{SCAN_POUND, 		REM_POUND},
	{SCAN_NUM_0,		REM_0},
	{SCAN_DELETE,		REM_DEL},
};

#endif	// __TCC_REMOCON_PREMIER_H__

