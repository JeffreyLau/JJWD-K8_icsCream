/*
 * dx08d11vm0aaa_lcd.c -- support for Hitachi DX08D11VM0AAA LCD
 *
 * Copyright (C) 2009, 2010 Telechips, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <debug.h>
#include <stdlib.h>
#include <string.h>
#include <tcc_lcd.h>

#ifdef DX08D11VM0AAA
#include <i2c.h>
#include <dev/gpio.h>

#define WAIT_FLAG	0xFFFF
#define END_FLAG	0xFFFE



unsigned short gLCDInit_Table[][2]=
{
		{0x002, 0x0000}, 
		{0x003, 0x0000},
		{0x004, 0x0000},
		{0x010, 0x0210}, 
		{0x020, 0x0000},
		{0x021, 0x2831},
		{0x022, 0x003E},
		{0x023, 0x7400},
		{0x024, 0x7400},
		{0x025, 0x6A06},
		{0x026, 0x7400},
		{0x027, 0x1C06},
		{0x028, 0x1C28},
		{0x029, 0x1C4A},
		{0x02A, 0x0666},
		{0x100, 0x0533},
		{0x101, 0x0003},
		{0x102, 0x3700},
#if 1		
		{0x300, 0x7777}, 
		{0x301, 0x0717},
		{0x302, 0xC114},
		{0x303, 0x0071}, 
		{0x304, 0x7110}, 
		{0x305, 0x01D4},
#else
		{0x300, 0x5757}, //0x7777
		{0x301, 0x0537}, //0x0717
		{0x302, 0xC114},
		{0x303, 0x0053}, //0x0071
		{0x304, 0x5130}, //0x7110
		{0x305, 0xC317}, //0x01D4
#endif
		{0x402, 0x0000},
		{0x501, 0xFFFF},
		{0x502, 0xFFFF},
		{0x503, 0xFFFF},
		{0x504, 0x00FF},
		{0x002, 0x0200},
		{WAIT_FLAG, 2},//wait 10ms
		{0x001, 0x0001},
		{WAIT_FLAG, 1},//wait 2ms
		{0x002, 0x8210},
		{WAIT_FLAG, 4},//wait 20ms
		{0x002, 0x8310},
		{WAIT_FLAG, 4},//wait 20ms
		{0x002, 0x0710},
		{WAIT_FLAG, 4},//wait 20ms
		{0x002, 0x1730},
		{WAIT_FLAG, 4},//wait 20ms
		{0x001, 0x0012},
		{0x001, 0x0032},
		{0x023, 0x0000},
		{WAIT_FLAG, 4},//wait 20ms
		{0x001, 0x0033},
		{0x023, 0x7400},
		{END_FLAG, 0x0}
};

unsigned short gLCDOff_Table[][2]=
{
		{0x203, 0x0000},
		{WAIT_FLAG, 4},//wait 20ms
		{0x001 ,0x032},
		{0x023 ,0x7400},
		{WAIT_FLAG, 4},//wait 20ms
		{0x023, 0x0000},
		{WAIT_FLAG, 4},//wait 20ms
		{0x001 ,0x0012},
		{WAIT_FLAG, 4},//wait 20ms
		{0x001 ,0x0000},
		{WAIT_FLAG, 1},//wait 1ms
		{0x002 ,0x0730},
		{0x100 ,0x0000},
		{0x002 ,0x0000},
		{WAIT_FLAG, 1},//wait 1ms
		{0x002 ,0x0001},
		{WAIT_FLAG, 1},//wait 1ms
		{END_FLAG, 0x0}
};

extern void GPSB_SETConfig(sDRV_GPSB * pDRV_GPSB, int iCH, int iPort);
extern unsigned IO_GPSB_TxRxData(unsigned uCH, void *pTxBuf, void *pRxBuf, unsigned uLength);
#define LCD_SPI_CH	DRV_GPSB_CH1
static void GPSB_Set_Data(unsigned int Reg, unsigned int Data)
{
	unsigned int value;
	volatile unsigned int gReg = 0x100000;
	volatile unsigned int gWriteData = 0x120000;

	value = gReg  | Reg;
	IO_GPSB_TxRxData(LCD_SPI_CH, &value, 0, 1);

	value = gWriteData | Data;
	IO_GPSB_TxRxData(LCD_SPI_CH, &value, 0, 1);

	lcd_delay_us(10);

}

static sDRV_GPSB gDRV_SPI_LCD =
{
	0 
	| DRV_GPSB_MODE_OpMod_SPI 			// 0 : spi
	| DRV_GPSB_MODE_MasterMode			// 0 : Master mode enable	
	| DRV_GPSB_MODE_BitWidth(23)		// 31 : Bit width selection, 32bits = 31+1
//	| DRV_GPSB_MODE_SCKPolCtrl_High		// 16
	| DRV_GPSB_MODE_EnableOperation		// 3
//	| DRV_GPSB_MODE_RcvRisingEdge		// 17
//	| DRV_GPSB_MODE_TransRisingEdge		// 18
	| DRV_GPSB_MODE_ClearRcvFifoCount	// 1 : Assert Rx FIFO Counter Clear
//	| DRV_GPSB_MODE_SCKPolCtrl_High		// 1 : SCKI inverted. for SPI timing 1, 2	
	, 60000								// Clock Divider
	, DRV_GPSB_CTRL_HW					// Hardware Control
	, DRV_GPSB_SDOSDI_SDO				// SPI Input Only
	, 0
};

static int DX08D11VM0AAA_Module_On(void)
{
	int i = 0;
	
	
	#if defined(TCC88XX)
	GPSB_SETConfig(&gDRV_SPI_LCD, LCD_SPI_CH, DRV_GPSB_PORT10);
	#else
	GPSB_SETConfig(&gDRV_SPI_LCD, LCD_SPI_CH, DRV_GPSB_PORT9);
	#endif

	while(gLCDInit_Table[i][0] != END_FLAG)
	{

		if(gLCDInit_Table[i][0] == WAIT_FLAG)
			mdelay(gLCDInit_Table[i][1]);
		else
			GPSB_Set_Data(gLCDInit_Table[i][0], gLCDInit_Table[i][1]);

		i++;			
	}
	
	GPSB_Port_disable();
	return 1;
}

static int dx08d11vm0aaa_panel_init(struct lcd_panel *panel)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d  \n", __func__, 0);

	tcclcd_gpio_config(pdata->display_on, 1);
	tcclcd_gpio_config(pdata->bl_on, 1);
	tcclcd_gpio_config(pdata->reset, 1);
	tcclcd_gpio_config(pdata->power_on, 1);

	return 0;
}

static int dx08d11vm0aaa_set_power(struct lcd_panel *panel, int on)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d \n", __func__, on);
	if (on) {
		tcclcd_gpio_set_value(pdata->power_on, 1);
		tcclcd_gpio_set_value(pdata->reset, 1);
		mdelay(10);
		DX08D11VM0AAA_Module_On();
		mdelay(10);

		lcdc_initialize(pdata->lcdc_num, panel);
		
		LCDC_IO_Set(pdata->lcdc_num, panel->bus_width);

		mdelay(16);
		
		tcclcd_gpio_set_value(pdata->display_on, 1);

	}
	else 
	{
		tcclcd_gpio_set_value(pdata->display_on, 0);
		mdelay(10);
		tcclcd_gpio_set_value(pdata->reset, 0);
		tcclcd_gpio_set_value(pdata->power_on, 0);
		
		LCDC_IO_Disable(pdata->lcdc_num, panel->bus_width);
	}

	return 0;
}


typedef struct{
	char reg;
	char value;
} BD6083GUL_CTRL_TYPE;

#define I2C_BD6083GUL_SLAVE 0xEC

BD6083GUL_CTRL_TYPE BD6083_init[] = {
	{0x00, 0x01}, {0x01, 0x0E}, {0x03, 0x63},
	{0x09, 0x00},{0x02, 0x01}
};
BD6083GUL_CTRL_TYPE BD6083_NormaleToALCD[] = {
	{0x01, 0x0F}, {0x09, 0x87},	{0x0A, 0x01},
	{0x0B, 0x01}, {0x0D, 0x13},	{0x0E, 0x63}, {0x01, 0x0F}
};
BD6083GUL_CTRL_TYPE BD6083_ALCToNormal[] = {
	{0x01, 0x0E},	{0x09, 0x00},{0x02, 0x01}
};
BD6083GUL_CTRL_TYPE BD6083_NormalToDiming[] = { {0x03, 0x04} };
BD6083GUL_CTRL_TYPE BD6083_DimingToSleep[] = { {0x02, 0x00} };
BD6083GUL_CTRL_TYPE BD6083_SleepToNomal[] = { {0x06, 0x63}, {0x02, 0x01} };
BD6083GUL_CTRL_TYPE BD6083_DimingToNomal[] = { {0x03, 0x63} };

void bl_write( BD6083GUL_CTRL_TYPE *BL_ctrl, unsigned size)
{
	unsigned i;
	char i2c_write_buf[2];


	for(i = 0; i < size; i++)
	{
		if(BL_ctrl[i].reg == 0xFF)
		{
			lcd_delay_us(BL_ctrl[i].value);
		}
		else
		{
			i2c_write_buf[0] = BL_ctrl[i].reg;
			i2c_write_buf[1] = BL_ctrl[i].value;
			#if defined(TCC88XX)
			i2c_xfer(I2C_BD6083GUL_SLAVE, 2, (unsigned char *)i2c_write_buf, 0, 0, I2C_CORE0_CH1);
			#else
			i2c_xfer(I2C_BD6083GUL_SLAVE, 2, (unsigned char *)i2c_write_buf, 0, 0, I2C_CH0);
			#endif//
		}

	}
}

static int dx08d11vm0aaa_set_backlight_level(struct lcd_panel *panel, int level)
{
	#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

	static char BD6083_state = 0;
	
	printf("%s ==> level:%d \n", __func__, level);

	if (level) {
		if (!BD6083_state) {
			bl_write(BD6083_init, ARRAY_SIZE(BD6083_init));
		}

		BD6083_NormalToDiming[0].value = level >> 1;
		bl_write(BD6083_NormalToDiming, ARRAY_SIZE(BD6083_NormalToDiming));

	} else {
		BD6083_state = 0;
		bl_write(BD6083_DimingToSleep, ARRAY_SIZE(BD6083_DimingToSleep));
	}
	return 0;
}

static struct lcd_panel dx08d11vm0aaa_panel = {
	.name		= "DX08D11VM0AAA",
	.manufacturer	= "Hitachi",
	.id			= 	PANEL_ID_DX08D11VM0AAA,
	.xres		= 480,
	.yres		= 800,
	.width		= 45,
	.height		= 76,
	.bpp		= 32,
	.clk_freq	= 230000,
	.clk_div	= 2,
	.bus_width	= 24,
	.lpw		= 4,
	.lpc		= 480,
	.lswc		= 8,
	.lewc		= 5,
	.vdb		= 0,
	.vdf		= 0,
	.fpw1		= 4,
	.flc1		= 800,
	.fswc1		= 2,
	.fewc1		= 11,
	.fpw2		= 4,
	.flc2		= 800,
	.fswc2		= 2,
	.fewc2		= 11,
	.sync_invert	= ID_INVERT | IV_INVERT | IH_INVERT | IP_INVERT,
	.init		= dx08d11vm0aaa_panel_init,
	.set_power	= dx08d11vm0aaa_set_power,
	.set_backlight_level = dx08d11vm0aaa_set_backlight_level,
};

struct lcd_panel *tccfb_get_panel(void)
{
	return &dx08d11vm0aaa_panel;
}
#endif//DX08D11VM0AAA
