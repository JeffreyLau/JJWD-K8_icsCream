/* /tcc_lcd.h
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

#ifndef __TCC_LCD_H
#define __TCC_LCD_H


#define ATAG_TCC_PANEL	0x54434364 /* TCCd */

enum {
	PANEL_ID_LMS350DF01,
	PANEL_ID_LMS480KF01,
	PANEL_ID_DX08D11VM0AAA,
	PANEL_ID_LB070WV6,
	PANEL_ID_CLAA104XA01CW,
	PANEL_ID_HT121WX2,
	PANEL_ID_TD043MGEB1,
	PANEL_ID_AT070TN93,
	PANEL_ID_TD070RDH11,
	PANEL_ID_N101L6,
	PANEL_ID_TW8816,
	PANEL_ID_CLAA102NA0DCW,
	PANEL_ID_ED090NA,
	PANEL_ID_KR080PA2S,
	PANEL_ID_CLAA070NP01,
	PANEL_ID_HV070WSA,
	PANEL_ID_HDMI
};

#define MAX_BACKLIGTH 		255
#define DEFAULT_BACKLIGTH 	150
#define GPIO_EXT1		0xFFFF0001
#define GPIO_NC			0xFFFFFFFF


#define ID_INVERT	0x01 	// Invered Data Enable(ACBIS pin)  anctive Low
#define IV_INVERT	0x02	// Invered Vertical sync  anctive Low
#define IH_INVERT	0x04	// Invered Horizontal sync	 anctive Low
#define IP_INVERT	0x08	// Invered Pixel Clock : anctive Low


struct lcd_platform_data {
// power control pin
	unsigned power_on;
	unsigned display_on;
	unsigned bl_on;
	unsigned reset;

	unsigned lcdc_num;
};

struct lcd_panel {
	const char *name;
	const char *manufacturer;

	struct lcd_platform_data dev;

	int id;			/* panel ID */
	int xres;		/* x resolution in pixels */
	int yres;		/* y resolution in pixels */
	int width;		/* display width in mm */
	int height;		/* display height in mm */
	int bpp;		/* bits per pixels */

	int clk_freq;
	int clk_div;
	int bus_width;
	int lpw;		/* line pulse width */
	int lpc;		/* line pulse count */
	int lswc;		/* line start wait clock */
	int lewc;		/* line end wait clock */
	int vdb;		/* back porch vsync delay */
	int vdf;		/* front porch vsync delay */
	int fpw1;		/* frame pulse width 1 */
	int flc1;		/* frame line count 1 */
	int fswc1;		/* frame start wait cycle 1 */
	int fewc1;		/* frame end wait cycle 1 */
	int fpw2;		/* frame pulse width 2 */
	int flc2;		/* frame line count 2 */
	int fswc2;		/* frame start wait cycle 2 */
	int fewc2;		/* frame end wait cycle 2 */
	int sync_invert;

	int (*init)(struct lcd_panel *panel);
	int (*set_power)(struct lcd_panel *panel, int on);
	int (*set_backlight_level)(struct lcd_panel *panel, int level);
};

typedef struct  
{
	unsigned lcdc_num;
	unsigned onoff;
	unsigned GammaRed[16];
	unsigned GammaGreen[16];
	unsigned GammaBlue[16];
}lcdc_gamma_params;

struct lcd_panel *tccfb_get_panel(void);


extern void DEV_Lcd_PortSet(unsigned int bit_per_pixel);

extern void LCDC_IO_Set (char lcdctrl_num, unsigned bit_per_pixel);

extern void LCDC_IO_Disable (char lcdctrl_num, unsigned bit_per_pixel);

extern void LCDC_IO_SetCurrent(char lcdctrl_num, unsigned bit_per_pixel);

extern void lcdc_initialize(char lcdctrl_num, struct lcd_panel *lcd_spec);

extern void ddic_setdefault(void);

extern void tcclcd_gpio_config(unsigned n, unsigned flags);

extern void tcclcd_gpio_set_value(unsigned n, unsigned on);

typedef enum{
	TCC_LCDC_IMG_FMT_1BPP,
	TCC_LCDC_IMG_FMT_2BPP,
	TCC_LCDC_IMG_FMT_4BPP,
	TCC_LCDC_IMG_FMT_8BPP,
	TCC_LCDC_IMG_FMT_RGB332 = 8,
	TCC_LCDC_IMG_FMT_RGB444 = 9,
	TCC_LCDC_IMG_FMT_RGB565 = 10,
	TCC_LCDC_IMG_FMT_RGB555 = 11,
	TCC_LCDC_IMG_FMT_RGB888 = 12,
	TCC_LCDC_IMG_FMT_RGB666 = 13,
	TCC_LCDC_IMG_FMT_RGB888_3	= 14,		/* RGB888 - 3bytes aligned - B1[31:24],R0[23:16],G0[15:8],B0[7:0] newly supported : 3 bytes format*/
	TCC_LCDC_IMG_FMT_ARGB6666_3 = 15,		/* ARGB6666 - 3bytes aligned - A[23:18],R[17:12],G[11:6],B[5:0]newly supported : 3 bytes format */
	TCC_LCDC_IMG_FMT_COMP = 16,				// 4bytes
	TCC_LCDC_IMG_FMT_DECOMP	= (TCC_LCDC_IMG_FMT_COMP),
	TCC_LCDC_IMG_FMT_444SEP = 21,
	TCC_LCDC_IMG_FMT_UYVY = 22,		/* 2bytes	: YCbCr 4:2:2 Sequential format LSB [Y/U/Y/V] MSB : newly supported : 2 bytes format*/
	TCC_LCDC_IMG_FMT_VYUY = 23,		/* 2bytes	: YCbCr 4:2:2 Sequential format LSB [Y/V/Y/U] MSB : newly supported : 2 bytes format*/

	TCC_LCDC_IMG_FMT_YUV420SP = 24,	
	TCC_LCDC_IMG_FMT_YUV422SP = 25, 
	TCC_LCDC_IMG_FMT_YUYV = 26, 
	TCC_LCDC_IMG_FMT_YVYU = 27,
	
	TCC_LCDC_IMG_FMT_YUV420ITL0 = 28, 
	TCC_LCDC_IMG_FMT_YUV420ITL1 = 29, 
	TCC_LCDC_IMG_FMT_YUV422ITL0 = 30, 
	TCC_LCDC_IMG_FMT_YUV422ITL1 = 31, 
	TCC_LCDC_IMG_FMT_MAX
}TCC_LCDC_IMG_FMT_TYPE;

struct tcc_lcdc_image_update
{
	unsigned int Lcdc_layer;
	unsigned int enable;
	unsigned int Frame_width;
	unsigned int Frame_height;

	unsigned int Image_width;
	unsigned int Image_height;
	unsigned int offset_x; //position
	unsigned int offset_y; 	//position
	unsigned int addr0;
	unsigned int addr1;
	unsigned int addr2;
	TCC_LCDC_IMG_FMT_TYPE fmt;	//TCC_LCDC_IMG_FMT_TYPE
};

extern void tcclcd_image_ch_set(char lcdctrl_num, struct tcc_lcdc_image_update *info);

extern void mdelay(unsigned int ms);
extern void lcd_delay_us(unsigned int us);

#endif
