/*
 * Copyright (c) 2010 Telechips, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <debug.h>
#include <stdlib.h>
#include <string.h>
#include <reg.h>
#include <platform/iomap.h>
#include <dev/fbcon.h>

#include <platform/reg_physical.h>
#include <lcd.h>
#include <platform/irqs.h>
#include <platform/globals.h>
#include <i2c.h>
#include <dev/gpio.h>
#include <platform/gpio.h>
#include <platform/physical.h>
#include <tcc_lcd.h>
#include <tca_ddic_interface.h>


#define LCDC_FB_BPP		32


#ifdef DISPLAY_SPLASH_SCREEN_DIRECT
// Define boot image infomation 
#ifdef _LCD_32BPP_
 #include <lcd/logo_24bit.h>
#else
 #include <lcd/logo.h>
#endif

#define LCDC_FB_WIDTH	480
#define LCDC_FB_HEIGHT	272

static struct fbcon_config fb_cfg = {
	.height		= LCDC_FB_HEIGHT,
	.width		= LCDC_FB_WIDTH,
	.format		= FB_FORMAT_RGB565,
	.bpp		= LCDC_FB_BPP,
	.update_start	= NULL,
	.update_done	= NULL,
	.base = telechips_logo
};

static struct fbcon_config fb0_cfg = {
	.height		= LCDC_FB_HEIGHT,
	.width		= LCDC_FB_WIDTH,
	.format		= FB_FORMAT_RGB565,
	.bpp		= LCDC_FB_BPP,
	.update_start	= NULL,
	.update_done	= NULL,
	.base = telechips_logo
};

static struct fbcon_config fb1_cfg = {
	.height		= LCDC_FB_HEIGHT,
	.width		= LCDC_FB_WIDTH,
	.format		= FB_FORMAT_RGB565,
	.bpp		= LCDC_FB_BPP,
	.update_start	= NULL,
	.update_done	= NULL,
	.base = telechips_logo
};
#else
static struct fbcon_config fb_cfg;
static struct fbcon_config fb0_cfg;
static struct fbcon_config fb1_cfg;
#endif//


enum
{
	LCDC_COMPOSITE_NTSC,
	LCDC_COMPOSITE_PAL,
	LCDC_COMPOSITE_MAX
};

enum
{
	LCDC_COMPONENT_480I_NTSC,
	LCDC_COMPONENT_576I_PAL,
	LCDC_COMPONENT_720P,
	LCDC_COMPONENT_1080I,
	LCDC_COMPONENT_MAX
};

//#define _LCD_32BPP_
//#define LCDC0_USE
#ifdef LCDC0_USE
#define LCD_LCDC_NUM 	0
#else
#define LCD_LCDC_NUM 	1
#endif//

//  U1_1:LCD_PWRCTL 	C29:LCD_DISP  A4 : LCD_BL_EN, U3_12: LCD_RESET
#if defined (_MOBIS_)
#define GPIO_LCD_ON                     ( GPIO_PORTG|2)// using LVDS_PWREN
#define GPIO_LCD_BL                     ( GPIO_PORTG|17)// using LVDS_BLEN
#define GPIO_LCD_DISPLAY        ( GPIO_PORTE|23) // using LVDS_PWM
#define GPIO_LCD_RESET          ( GPIO_PORTG|18) // using LVDS_LEDEN

#elif defined (_M801_88_)
#define GPIO_LCD_ON			( GPIO_PORTG|2)
#define GPIO_LCD_BL			( GPIO_PORTA |4)
#define GPIO_LCD_DISPLAY	( GPIO_NC )
#define GPIO_LCD_RESET		( GPIO_PORTC|29)
#elif defined(_TCC8800ST_)
#define GPIO_LCD_ON			( GPIO_NC )
#define GPIO_LCD_BL			( GPIO_NC )
#define GPIO_LCD_DISPLAY	( GPIO_NC )
#define GPIO_LCD_RESET		( GPIO_NC )
#elif defined (_M803_)
#define GPIO_LCD_ON                     ( GPIO_PORTG|2)// using LVDS_PWREN
#define GPIO_LCD_BL                     ( GPIO_PORTC|29)// using LVDS_BLEN
#define GPIO_LCD_DISPLAY        ( GPIO_PORTA|4) // using LVDS_PWM
#define GPIO_LCD_RESET          ( GPIO_PORTC|24) // using LVDS_LEDEN
#elif defined (ED090NA)
#define GPIO_LCD_ON			( GPIO_EXT1|1)
#define GPIO_LCD_BL			( GPIO_PORTA |4)
#define GPIO_LCD_DISPLAY	( GPIO_PORTC|29)
#define GPIO_LCD_RESET		( GPIO_PORTC|24)
#else
#define GPIO_LCD_ON			( GPIO_EXT1|1)
#define GPIO_LCD_BL			( GPIO_PORTA |4)
#define GPIO_LCD_DISPLAY	( GPIO_PORTC|29)
#define GPIO_LCD_RESET		( GPIO_NC )
#endif//

struct atag_tcc_entry
{
	char output;
	char resolution;
	char hdmi_resolution;
	char composite_resolution;
	char component_resolution;
	char hdmi_mode;
};

extern struct atag_tcc_entry tcc_param;

static void lcdc_set_logo(unsigned char lcdc_num, unsigned lcd_wd, unsigned lcd_ht, struct fbcon_config *fb_con)
{
	struct tcc_lcdc_image_update Image_info;
	
	#ifdef DISPLAY_SPLASH_SCREEN_DIRECT
		fb_con->stride = fb_con->width;
	#else
		fb_con->width = lcd_wd;
		fb_con->height = lcd_ht;
		fb_con->stride = fb_con->width;
		fb_con->bpp = LCDC_FB_BPP;
		fb_con->format = FB_FORMAT_RGB565;

		fb_con->base = dma_alloc(4096, fb_con->width * fb_con->height * (fb_con->bpp/8));

		if (fb_con->base == NULL)
		  dprintf(INFO, "lcdc: framebuffer alloc failed!\n");
	#endif//

	dprintf(INFO, "fb_cfg base:%p xres:%d yres:%d bpp:%d \n", fb_con->base, fb_con->width, fb_con->height, fb_con->bpp);

	Image_info.addr0 = fb_con->base;
	Image_info.Lcdc_layer = 2;
	Image_info.enable = 1;

	Image_info.Frame_width = Image_info.Image_width = fb_con->width;
	Image_info.Frame_height = Image_info.Image_height = fb_con->height;

	if(Image_info.Image_width > lcd_wd)
		Image_info.Image_width = lcd_wd;

	if(lcd_wd > Image_info.Frame_width)
		Image_info.offset_x = (lcd_wd - Image_info.Frame_width)/2;
	else
		Image_info.offset_x =  0;

	if(lcd_ht > Image_info.Frame_height)
		Image_info.offset_y = (lcd_ht - Image_info.Frame_height)/2;
	else
		Image_info.offset_y = 0;
	
	#ifdef _LCD_32BPP_
	Image_info.fmt = TCC_LCDC_IMG_FMT_RGB888;
	#else
	Image_info.fmt = TCC_LCDC_IMG_FMT_RGB565;
	#endif

	tcclcd_image_ch_set(lcdc_num, &Image_info);

	mdelay(1);
}

#if defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
static int hdmi_detect(void)
{
	#if 0 //TBD
	PGPIO pGPIO = (PGPIO)&HwGPIO_BASE;

	BITCLR(pGPIO->GPAEN, Hw14);
	BITCLR(pGPIO->GPAFN1, Hw28-Hw24);

	if(ISSET(pGPIO->GPADAT, Hw14))
	{
		printf("HDMI is detected!!\n");
		return true;
	}
	else
	{
		printf("HDMI is not detected!!\n");
		return false;
	}
	#else
		return true;
	#endif
}

static int composite_detect(void)
{
	int ret = true;

	#if defined(BOARD_TCC880X_STB_DEMO) && defined(DISPLAY_STB_AUTO_DETECT)
		gpio_config(GPIO_PORTF|27, GPIO_FN0 | GPIO_INPUT); /* CVBS_DET */
		ret = gpio_get(GPIO_PORTF|27)? false : true;
	#endif

	return ret;
}

static int component_detect(void)
{
	int ret = true;

	#if defined(BOARD_TCC880X_STB_DEMO) && defined(DISPLAY_STB_AUTO_DETECT)
		gpio_config(GPIO_PORTF|26, GPIO_FN0 | GPIO_INPUT); /* VE_DET */
		ret = gpio_get(GPIO_PORTF|26)? false : true;
	#endif

	return ret;
}
#endif

#if defined(DEFAULT_DISPLAY_COMPOSITE) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
static void composite_get_spec(unsigned char type, stCOMPOSITE_SPEC *spec)
{
	switch(type)
	{
		case LCDC_COMPOSITE_NTSC:
			spec->composite_clock = 270000;
			spec->composite_divider = 1;
			spec->composite_bus_width = 8;
			spec->composite_width = 720;
			spec->composite_height = 480;

			#if 0 // COMPOSITE_CCIR656
			spec->composite_LPW = 224 - 1; 				// line pulse width
			spec->composite_LPC = 720 * 2 - 1; 			// line pulse count (active horizontal pixel - 1)
			spec->composite_LSWC = 20 - 1;				// line start wait clock (the number of dummy pixel clock - 1)
			spec->composite_LEWC = 32 - 1;				// line end wait clock (the number of dummy pixel clock - 1)
			#else
			spec->composite_LPW = 212 - 1; 				// line pulse width
			spec->composite_LPC = 720 * 2 - 1; 			// line pulse count (active horizontal pixel - 1)
			spec->composite_LSWC = 32 - 1;				// line start wait clock (the number of dummy pixel clock - 1)
			spec->composite_LEWC = 32 - 1;				// line end wait clock (the number of dummy pixel clock - 1)
			#endif

			spec->composite_VDB = 0; 					// Back porch Vsync delay
			spec->composite_VDF = 0; 					// front porch of Vsync delay
			spec->composite_FPW1 = 1 - 1;				// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->composite_FLC1 = 480 - 1;				// frmae line count is the number of lines in each frmae on the screen
			spec->composite_FSWC1 = 37 - 1;				// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->composite_FEWC1 = 7 - 1;				// frame start wait cycle is the number of lines to insert at the begining each frame
			spec->composite_FPW2 = 1 - 1;				// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->composite_FLC2 = 480 - 1;				// frmae line count is the number of lines in each frmae on the screen
			spec->composite_FSWC2 = 38 - 1;				// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->composite_FEWC2 = 6 - 1; 				// frame start wait cycle is the number of lines to insert at the begining each frame
			break;

		case LCDC_COMPOSITE_PAL:		
			spec->composite_clock = 270000;
			spec->composite_divider = 1;
			spec->composite_bus_width = 8;
			spec->composite_width = 720;
			spec->composite_height = 576;

			spec->composite_LPW = 128 - 1; 				// line pulse width
			spec->composite_LPC = 720 * 2 - 1; 			// line pulse count (active horizontal pixel - 1)
			spec->composite_LSWC = 138 - 1;				// line start wait clock (the number of dummy pixel clock - 1)
			spec->composite_LEWC = 22 - 1;				// line end wait clock (the number of dummy pixel clock - 1)

			spec->composite_VDB = 0; 					// Back porch Vsync delay
			spec->composite_VDF = 0; 					// front porch of Vsync delay
			spec->composite_FPW1 = 1 - 1;				// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->composite_FLC1 = 576 - 1;				// frmae line count is the number of lines in each frmae on the screen
			spec->composite_FSWC1 = 43 -1;				// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->composite_FEWC1 = 5 - 1;				// frame start wait cycle is the number of lines to insert at the begining each frame
			spec->composite_FPW2 = 1 - 1;				// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->composite_FLC2 = 576 - 1;				// frmae line count is the number of lines in each frmae on the screen
			spec->composite_FSWC2 = 44 - 1;				// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->composite_FEWC2 = 4 - 1; 				// frame start wait cycle is the number of lines to insert at the begining each frame
			break;
	}
}
#endif

#if defined(DEFAULT_DISPLAY_COMPONENT) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
static void component_get_spec(unsigned char type, stCOMPONENT_SPEC *spec)
{
	switch(type)
	{
		case LCDC_COMPONENT_480I_NTSC:
			spec->component_clock = 270000;
			spec->component_divider = 1;
			spec->component_bus_width = 8;
			spec->component_width = 720;
			spec->component_height = 480;
			spec->component_LPW = 128 - 1; 					// line pulse width
			spec->component_LPC = 720 * 2 - 1; 				// line pulse count (active horizontal pixel - 1)
			spec->component_LSWC = 116 - 1;					// line start wait clock (the number of dummy pixel clock - 1)
			spec->component_LEWC = 32 - 1;					// line end wait clock (the number of dummy pixel clock - 1)

			spec->component_VDB = 0; 						// Back porch Vsync delay
			spec->component_VDF = 0; 						// front porch of Vsync delay

			spec->component_FPW1 = 6 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->component_FLC1 = 480 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->component_FSWC1 = 30 - 1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->component_FEWC1 = 9 - 1;					// frame start wait cycle is the number of lines to insert at the begining each frame
			spec->component_FPW2 = 6 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->component_FLC2 = 480 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->component_FSWC2 = 31 - 1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->component_FEWC2 = 8 - 1; 					// frame start wait cycle is the number of lines to insert at the begining each frame
			break;
				
		case LCDC_COMPONENT_576I_PAL:
			spec->component_clock = 270000;
			spec->component_divider = 1;
			spec->component_bus_width = 8;
			spec->component_width = 720;
			spec->component_height = 576;
			spec->component_LPW = 128 - 1; 					// line pulse width
			spec->component_LPC = 720 * 2 - 1; 				// line pulse count (active horizontal pixel - 1)
			spec->component_LSWC = 136 - 1;					// line start wait clock (the number of dummy pixel clock - 1)
			spec->component_LEWC = 24 - 1;					// line end wait clock (the number of dummy pixel clock - 1)

			spec->component_VDB = 0; 						// Back porch Vsync delay
			spec->component_VDF = 0; 						// front porch of Vsync delay
				
			spec->component_FPW1 = 5 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->component_FLC1 = 576 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->component_FSWC1 = 39 - 1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->component_FEWC1 = 5 - 1;					// frame start wait cycle is the number of lines to insert at the begining each frame
			spec->component_FPW2 = 5 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->component_FLC2 = 576 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->component_FSWC2 = 40 - 1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->component_FEWC2 = 4 - 1; 					// frame start wait cycle is the number of lines to insert at the begining each frame
			break;

		case LCDC_COMPONENT_720P:
			spec->component_clock = 742500;
			spec->component_divider = 1;
			spec->component_bus_width = 24;
			spec->component_width = 1280;
			spec->component_height = 720;
			#if 1
			spec->component_LPW = 24 - 1; 					// line pulse width
			spec->component_LPC = 1280 - 1; 				// line pulse count (active horizontal pixel - 1)
			spec->component_LSWC = 345 - 1;					// line start wait clock (the number of dummy pixel clock - 1)
			spec->component_LEWC = 1 - 1;					// line end wait clock (the number of dummy pixel clock - 1)
			#else
			spec->component_LPW = 40 - 1; 					// line pulse width
			spec->component_LPC = 1280 - 1; 				// line pulse count (active horizontal pixel - 1)
			spec->component_LSWC = 329 - 1;					// line start wait clock (the number of dummy pixel clock - 1)
			spec->component_LEWC = 1 - 1;					// line end wait clock (the number of dummy pixel clock - 1)
			#endif

			spec->component_VDB = 0; 						// Back porch Vsync delay
			spec->component_VDF = 0; 						// front porch of Vsync delay
				
			#if 1
			spec->component_FPW1 = 3 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->component_FLC1 = 720 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->component_FSWC1 = 26 - 1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->component_FEWC1 = 1 - 1;					// frame start wait cycle is the number of lines to insert at the begining each frame
			spec->component_FPW2 = 3 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->component_FLC2 = 720 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->component_FSWC2 = 26 - 1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->component_FEWC2 = 1 - 1; 					// frame start wait cycle is the number of lines to insert at the begining each frame
			#else
			spec->component_FPW1 = 6 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->component_FLC1 = 720 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->component_FSWC1 = 20 - 1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->component_FEWC1 = 4 - 1;					// frame start wait cycle is the number of lines to insert at the begining each frame
			spec->component_FPW2 = 6 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->component_FLC2 = 720 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->component_FSWC2 = 20 - 1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->component_FEWC2 = 4 - 1; 					// frame start wait cycle is the number of lines to insert at the begining each frame
			#endif
			break;

		case LCDC_COMPONENT_1080I:
			spec->component_clock = 742500;
			spec->component_divider = 1;
			spec->component_bus_width = 24;
			spec->component_width = 1920;
			spec->component_height = 1080;
			spec->component_LPW = 24 - 1; 					// line pulse width
			spec->component_LPC = 1920 - 1; 				// line pulse count (active horizontal pixel - 1)
			spec->component_LSWC = 254 - 1;					// line start wait clock (the number of dummy pixel clock - 1)
			spec->component_LEWC = 2 - 1;					// line end wait clock (the number of dummy pixel clock - 1)

			spec->component_VDB = 0; 						// Back porch Vsync delay
			spec->component_VDF = 0; 						// front porch of Vsync delay
				
			spec->component_FPW1 = 5*2 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->component_FLC1 = 540*2 - 1;				// frmae line count is the number of lines in each frmae on the screen
			spec->component_FSWC1 = 15*2 - 1;				// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->component_FEWC1 = 2.5*2 - 1;				// frame start wait cycle is the number of lines to insert at the begining each frame
			spec->component_FPW2 = 5*2 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->component_FLC2 = 540*2 - 1;				// frmae line count is the number of lines in each frmae on the screen
			spec->component_FSWC2 = 15.5*2 - 1;				// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->component_FEWC2 = 2*2 - 1; 				// frame start wait cycle is the number of lines to insert at the begining each frame
			break;
	}
}
#endif

#if defined(DEFAULT_DISPLAY_HDMI) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
static void lcdc_io_init_hdmi(unsigned char lcdc_num)
{
	uint width, height;
	unsigned int lclk, lcd_div;
	unsigned int lcd_reg = 0;

	PDDICONFIG		pDDICfg 	= (PDDICONFIG)HwDDI_CONFIG_BASE;
	PLCDC			pLCDC		= (PLCDC)HwLCDC1_BASE;
	PLCDC_CHANNEL 	pLCDC_CH	= (PLCDC_CHANNEL)&pLCDC->LI2C;
	PNTSCPAL 		pTVE		= (PNTSCPAL)HwTVE_BASE;


	struct fbcon_config *fb_con;
	
	#ifndef DEFAULT_DISPLAY_OUTPUT_DUAL
		pTVE->DACPD = 1;
	#endif

	printf("%s\n", __func__);
		
	fb_con = &fb_cfg;
			 	
	width = gRefHdmiVideoModeList[HDMI_VIDEO_MODE_TYPE].width;
	height = gRefHdmiVideoModeList[HDMI_VIDEO_MODE_TYPE].height;

	if(lcdc_num)
	{
		pLCDC = (LCDC *)HwLCDC1_BASE;
		pLCDC_CH = (LCDC_CHANNEL *)&pLCDC->LI2C;

		BITSET(pDDICfg->SWRESET, Hw3);
		BITCLR(pDDICfg->SWRESET, Hw3);
		BITSET(pDDICfg->HDMI_CTRL, Hw15); // LCDC1

		#if defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
			fb_con = &fb1_cfg;
		#endif
	}
	else
	{
		pLCDC = (LCDC *)HwLCDC0_BASE;
		pLCDC_CH = (LCDC_CHANNEL *)&pLCDC->LI0C;

		BITSET(pDDICfg->SWRESET, Hw2);
		BITCLR(pDDICfg->SWRESET, Hw2);
		BITCLR(pDDICfg->HDMI_CTRL, Hw15); // LCDC0

		#if defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
			fb_con = &fb0_cfg;
		#endif
	}

	/* Set PWROFF Register - Video DAC */
	tca_ckc_setpmupwroff(PMU_VIDEODAC, DISABLE);

	ddic_setdefault();

	// dai_init();
	disp_init_hdmi();

	lcdc_set_logo(lcdc_num, width, height, fb_con);
	
	/* Update LCDC screen - UPD Bit */
	BITCSET(pLCDC->LI2C, Hw16, Hw16);

}
#endif //#if defined(DEFAULT_DISPLAY_HDMI) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)

#if defined(DEFAULT_DISPLAY_COMPOSITE) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
static void lcdc_io_init_composite(unsigned char lcdc_num, unsigned char type)
{
	unsigned int lcd_reg = 0;
	stCOMPOSITE_SPEC spec;

	PLCDC 					pLCDC = (PLCDC)HwLCDC0_BASE;
	PLCDC_CHANNEL 			pLCDC_CH = (PLCDC_CHANNEL)&pLCDC->LI0C;
	PDDICONFIG 				pDDICfg = (PDDICONFIG)HwDDI_CONFIG_BASE;
	PNTSCPAL 				pTVE = (PNTSCPAL)HwTVE_BASE;
	PNTSCPAL_ENCODER_CTRL 	pTVE_VEN = (PNTSCPAL_ENCODER_CTRL)HwTVE_VEN_BASE;

	struct fbcon_config *fb_con;
	
	printf("%s\n", __func__);
		
	composite_get_spec(type, &spec);

	fb_con = &fb_cfg;

	if(lcdc_num)
	{
		pLCDC = (PLCDC)HwLCDC1_BASE;
		pLCDC_CH = (PLCDC_CHANNEL)&pLCDC->LI2C;
		
		BITSET(pDDICfg->SWRESET, Hw3);	// LCDC1
		BITCLR(pDDICfg->SWRESET, Hw3);	// LCDC1	
		BITSET(pDDICfg->NTSCPAL_SEL, Hw0); // LCDC1
		tca_ckc_setperi(PERI_LCD1, ENABLE, spec.composite_clock*spec.composite_divider);

		#if defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
			fb_con = &fb1_cfg;
		#endif
	}
	else
	{
		pLCDC = (PLCDC)HwLCDC0_BASE;
		pLCDC_CH = (PLCDC_CHANNEL)&pLCDC->LI0C;
		
		BITSET(pDDICfg->SWRESET, Hw2);	// LCDC0
		BITCLR(pDDICfg->SWRESET, Hw2);	// LCDC0	
		BITCLR(pDDICfg->NTSCPAL_SEL, Hw0); // LCDC0
		tca_ckc_setperi(PERI_LCD0, ENABLE, spec.composite_clock*spec.composite_divider);

		#if defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
			fb_con = &fb0_cfg;
		#endif
	}

	/* Set PWROFF Register - Video DAC */
	tca_ckc_setpmupwroff(PMU_VIDEODAC, ENABLE);
	
	printf("LCDC0 clk:%d, LCDC1 clk:%d, divide:%d\n", tca_ckc_getperi(PERI_LCD0), tca_ckc_getperi(PERI_LCD1), spec.composite_divider);
	
	BITCSET(pLCDC->LCLKDIV, 0x01FF0000, (1 << 16));
	BITCSET(pLCDC->LCLKDIV, 0x000001FF, spec.composite_divider/2);

	BITCSET(pLCDC->LHTIME1, 0x0000FFFF, spec.composite_LPC);
	BITCSET(pLCDC->LHTIME1, 0x01FF0000, (spec.composite_LPW << 16));
	BITCSET(pLCDC->LHTIME2, 0x000001FF, spec.composite_LEWC);
	BITCSET(pLCDC->LHTIME2, 0x01FF0000, (spec.composite_LSWC << 16));
	
	BITCSET(pLCDC->LVTIME1, 0x00003FFF, spec.composite_FLC1);
	BITCSET(pLCDC->LVTIME1, 0x003F0000, (spec.composite_FPW1 << 16));
	BITCSET(pLCDC->LVTIME1, 0x03C00000, (spec.composite_VDF << 22));
	BITCSET(pLCDC->LVTIME1, 0xF8000000, (spec.composite_VDB << 27));
	BITCSET(pLCDC->LVTIME2, 0x01FF0000, (spec.composite_FSWC1 << 16));
	BITCSET(pLCDC->LVTIME2, 0x000001FF, spec.composite_FEWC1);
	
	BITCSET(pLCDC->LVTIME3, 0x00003FFF, spec.composite_FLC2);
	BITCSET(pLCDC->LVTIME3, 0x003F0000, (spec.composite_FPW2 << 16));
	BITCSET(pLCDC->LVTIME4, 0x01FF0000, (spec.composite_FSWC2 << 16));
	BITCSET(pLCDC->LVTIME4, 0x000001FF, spec.composite_FEWC2);
	
	BITCSET(pLCDC->LDS, 0x0000FFFF, (spec.composite_width << 0));
	BITCSET(pLCDC->LDS, 0xFFFF0000, (spec.composite_height << 16));

	BITCSET(lcd_reg, Hw29|Hw28, Hw29|Hw28);				// R2YMD - RGB to YCbCr Conversion Option
	BITCSET(lcd_reg, Hw23, Hw23);						// CKG - Clock Gating Enable for Timing Generator
	BITCSET(lcd_reg, Hw14, Hw14);						// IV - Inverted Vertical Sync
	BITCSET(lcd_reg, Hw13, Hw13);						// IH - Inverted Horizontal Sync
	BITCSET(lcd_reg, Hw12, Hw12);						// IP - Inverted Pixel Clock
	BITCSET(lcd_reg, Hw11, Hw11);						// CLEN - Clipping Enable
	BITCSET(lcd_reg, Hw10, Hw10);						// R2Y - RGB to YCbCr Converter Enable for OUPUT
	BITCSET(lcd_reg, Hw7, Hw7);							// TV - TV mode
	BITCSET(lcd_reg, Hw3|Hw2|Hw1, Hw3|Hw1);				// OVP
#if 0 // COMPOSITE_CCIR656
	BITCSET(lcd_reg, Hw24, Hw24);						// 656 - CCIR 656 Mode
	BITCSET(lcd_reg, Hw19|Hw18|Hw17|Hw16, Hw19);		// PXDW - Pixel Data Width
	BITCSET(lcd_reg, Hw9, Hw9);							// DP - Double Pixel Data
#else
	BITCSET(lcd_reg, Hw19|Hw18|Hw17|Hw16, Hw18|Hw17);	// PXDW - Pixel Data Width
#endif

	BITCSET(pLCDC->LCTRL, 0xFFFFFFFF, lcd_reg);

	lcdc_set_logo(lcdc_num, spec.composite_width, spec.composite_height, fb_con);
	
	/* Enable LCDC */
	BITSET(pLCDC->LCTRL, Hw0);

	/* Init DDI Cache */
	ddic_setdefault();

	//Disconnect LCDC with NTSC/PAL encoder
	BITCLR(pTVE_VEN->VENCON, HwTVEVENCON_EN_EN);
		
	//Set ECMDA Register
	if(type == LCDC_COMPOSITE_NTSC)
	{
		pTVE->ECMDA  = 
			HwTVECMDA_PWDENC_PD 			|	// [7]	 Power down mode for entire digital logic of TV encoder
			HwTVECMDA_FDRST_1				|	// [6]	 Chroma is free running as compared to H-sync
			HwTVECMDA_FSCSEL_NTSC			|	// [5:4] Color subcarrier frequency is 3.57954545 MHz for NTSC
			HwTVECMDA_PEDESTAL				|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
			HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
			HwTVECMDA_IFMT_525				|	// [1]	 Output data has 525 lines
			HwTVECMDA_PHALT_NTSC			|	// [0]	 NTSC encoded chroma signal output
			0;
	}
	else
	{
		pTVE->ECMDA  = 
			HwTVECMDA_FDRST_1				|	// [6]	 Chroma is free running as compared to H-sync
			HwTVECMDA_FSCSEL_PALX			|	// [5:4] Color subcarrier frequency is 4.43361875 MHz for PAL-B,D,G,H,I,N
			HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
			HwTVECMDA_IFMT_625				|	// [1]	 Output data has 625 lines
			HwTVECMDA_PHALT_PAL 			|	// [0]	 PAL encoded chroma signal output
			0;
	}
	
	//Set DACSEL Register
	BITSET(pTVE->DACSEL, HwTVEDACSEL_DACSEL_CVBS);
	//Set DACPD Register
	BITSET(pTVE->DACPD, HwTVEDACPD_PD_EN);

	BITSET(pTVE->ICNTL, HwTVEICNTL_VSIP_HIGH);
	BITSET(pTVE->ICNTL, HwTVEICNTL_HSVSP_RISING);
	#if 0 // COMPOSITE_CCIR656
	BITCSET(pTVE->ICNTL, HwTVEICNTL_ISYNC_MASK, HwTVEICNTL_ISYNC_ESAV_F);
	#else
	BITCSET(pTVE->ICNTL, HwTVEICNTL_ISYNC_MASK, HwTVEICNTL_ISYNC_HVSI);
	#endif
		
	//Set the Vertical Offset
	BITCSET(pTVE->HVOFFST, 0x07, ((0 & 0x700)>>8));
	pTVE->HOFFST = (0 & 0xFF);
			
	//Set the Horizontal Offset
	BITCSET(pTVE->HVOFFST, 0x08, ((1 & 0x100)>>5));
	pTVE->VOFFST = (1 & 0xFF);
			
	//Set the Digital Output Format
	BITCSET(pTVE->HVOFFST, HwTVEHVOFFST_INSEL_MASK, HwTVEHVOFFST_INSEL(2));
			
	//Set HSVSO Register
	BITCSET(pTVE->HSVSO, 0x07, ((0 & 0x700)>>8));
	pTVE->HSOE = (0 & 0xFF);
	BITCSET(pTVE->HSVSO, 0x38, ((0 & 0x700)>>5));
	pTVE->HSOB = (0 & 0xFF);
	BITCSET(pTVE->HSVSO, 0x40, ((0 & 0x100)>>2));
	pTVE->VSOB = (0 & 0xFF);

	//Set VSOE Register
	BITCSET(pTVE->VSOE, 0x1F, (0 & 0x1F));
	BITCSET(pTVE->VSOE, 0xC0, (0 & 0x03)<<6);
	BITCSET(pTVE->VSOE, 0x20, (0 & 0x01)<<5);
			
	//Set the Connection Type
	BITSET(pTVE_VEN->VENCIF, HwTVEVENCIF_FMT_1);

	BITSET(pTVE_VEN->VENCON, HwTVEVENCON_EN_EN);
	BITCLR(pTVE->DACPD, HwTVEDACPD_PD_EN);
	BITCLR(pTVE->ECMDA, HwTVECMDA_PWDENC_PD);
}
#endif //defined(DEFAULT_DISPLAY_COMPOSITE) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)

#if defined(DEFAULT_DISPLAY_COMPONENT) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
static void lcdc_io_init_component(unsigned char lcdc_num, unsigned char type)
{
	unsigned int lcd_reg = 0;
	stCOMPONENT_SPEC spec;

	PLCDC 			pLCDC = (PLCDC)HwLCDC1_BASE;
	PLCDC_CHANNEL 	pLCDC_CH = (PLCDC_CHANNEL)&pLCDC->LI2C;
	PDDICONFIG 		pDDICfg = (PDDICONFIG)HwDDI_CONFIG_BASE;
	PNTSCPAL 		pTVE = (PNTSCPAL)HwTVE_BASE;
	PGPIO 			pGPIO = (PGPIO)HwGPIO_BASE;

	struct fbcon_config *fb_con;

	printf("%s, lcdc_num=%d, type=%d\n", __func__, lcdc_num, type);
		
	#if defined(COMPONENT_CHIP_THS8200)
		/* Driver Strength Control - LCD_CLK(GPIO_C25) */
		BITCSET(pGPIO->GPCCD1, Hw19|Hw18, Hw19|Hw18);
		#if defined(BOARD_TCC930X_STB_DEMO)
			/* THS8200 Power Control - GPIO_D6 */
			BITCLR(pGPIO->GPDFN0, Hw28-Hw24);
			BITSET(pGPIO->GPDEN, Hw6);
			BITSET(pGPIO->GPDDAT, Hw6);
		#else
			/* THS8200 Power Control - GPIO_A5 */
			BITCLR(pGPIO->GPAFN0, Hw24-Hw20);
			BITSET(pGPIO->GPAEN, Hw5);
			BITSET(pGPIO->GPADAT, Hw5);
		#endif
	#endif

	#ifndef DEFAULT_DISPLAY_OUTPUT_DUAL
		BITSET(pTVE->DACPD, HwTVEDACPD_PD_EN);
	#endif
		
	component_get_spec(type, &spec);
	
	fb_con = &fb_cfg;
			
	if(lcdc_num)
	{
		pLCDC = (PLCDC)HwLCDC1_BASE;
		pLCDC_CH = (PLCDC_CHANNEL)&pLCDC->LI2C;
		
		BITSET(pDDICfg->SWRESET, Hw3);	// LCDC1
		BITCLR(pDDICfg->SWRESET, Hw3);	// LCDC1	
		tca_ckc_setperi(PERI_LCD1, ENABLE, spec.component_clock*spec.component_divider);

		#if defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
			fb_con = &fb1_cfg;
		#endif
	}
	else
	{
		pLCDC = (PLCDC)HwLCDC0_BASE;
		pLCDC_CH = (PLCDC_CHANNEL)&pLCDC->LI0C;
		
		BITSET(pDDICfg->SWRESET, Hw2);	// LCDC0
		BITCLR(pDDICfg->SWRESET, Hw2);	// LCDC0	
		tca_ckc_setperi(PERI_LCD0, ENABLE, spec.component_clock*spec.component_divider);

		#if defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
			fb_con = &fb0_cfg;
		#endif
	}
		
	printf("LCDC0 clk:%d, LCDC1 clk:%d, PLL:%d, divide:%d\n", tca_ckc_getperi(PERI_LCD0), tca_ckc_getperi(PERI_LCD1), tca_ckc_getpll(PCDIRECTPLL0), spec.component_divider);
	
	LCDC_IO_Set(lcdc_num, spec.component_bus_width);
	
	BITCSET(pLCDC->LCLKDIV, 0x01FF0000, (0 << 16));
	BITCSET(pLCDC->LCLKDIV, 0x000001FF, spec.component_divider/2);

	BITCSET(pLCDC->LHTIME1, 0x0000FFFF, spec.component_LPC);
	BITCSET(pLCDC->LHTIME1, 0x01FF0000, (spec.component_LPW << 16));
	BITCSET(pLCDC->LHTIME2, 0x000001FF, spec.component_LEWC);
	BITCSET(pLCDC->LHTIME2, 0x01FF0000, (spec.component_LSWC << 16));
	
	BITCSET(pLCDC->LVTIME1, 0x00003FFF, spec.component_FLC1);
	BITCSET(pLCDC->LVTIME1, 0x003F0000, (spec.component_FPW1 << 16));
	BITCSET(pLCDC->LVTIME1, 0x03C00000, (spec.component_VDF << 22));
	BITCSET(pLCDC->LVTIME1, 0xF8000000, (spec.component_VDB << 27));
	BITCSET(pLCDC->LVTIME2, 0x01FF0000, (spec.component_FSWC1 << 16));
	BITCSET(pLCDC->LVTIME2, 0x000001FF, spec.component_FEWC1);
	
	BITCSET(pLCDC->LVTIME3, 0x00003FFF, spec.component_FLC2);
	BITCSET(pLCDC->LVTIME3, 0x003F0000, (spec.component_FPW2 << 16));
	BITCSET(pLCDC->LVTIME4, 0x01FF0000, (spec.component_FSWC2 << 16));
	BITCSET(pLCDC->LVTIME4, 0x000001FF, spec.component_FEWC2);
	
	BITCSET(pLCDC->LDS, 0x0000FFFF, (spec.component_width << 0));
	BITCSET(pLCDC->LDS, 0xFFFF0000, (spec.component_height << 16));

	switch(type)
	{
		case LCDC_COMPONENT_480I_NTSC:
		case LCDC_COMPONENT_576I_PAL:
			BITCSET(lcd_reg, Hw29|Hw28, Hw29|Hw28);				// R2YMD - RGB to YCbCr Conversion Option
			BITCSET(lcd_reg, Hw23, Hw23);						// CKG - Clock Gating Enable for Timing Generator
			BITCSET(lcd_reg, Hw15, Hw15);						// ID - Inverted Data Enable
			BITCSET(lcd_reg, Hw14, Hw14);						// IV - Inverted Vertical Sync
			BITCSET(lcd_reg, Hw13, Hw13);						// IH - Inverted Horizontal Sync
			BITCSET(lcd_reg, Hw12, Hw12);						// IP - Inverted Pixel Clock
			BITCSET(lcd_reg, Hw10, Hw10);						// R2Y - RGB to YCbCr Converter Enable for OUPUT
			BITCSET(lcd_reg, Hw7, Hw7);							// TV - TV mode
			BITCSET(lcd_reg, Hw3|Hw2|Hw1, Hw3|Hw1);				// OVP
			BITCSET(lcd_reg, Hw0, Hw0);							// LEN - LCD Controller Enable
			#if 0 // COMPONENT_CCIR656
			BITCSET(lcd_reg, Hw24, Hw24);						// 656 - CCIR 656 Mode
			BITCSET(lcd_reg, Hw19|Hw18|Hw17|Hw16, Hw19);		// PXDW - Pixel Data Width
			BITCSET(lcd_reg, Hw9, Hw9);							// DP - Double Pixel Data
			#else
			BITCSET(lcd_reg, Hw19|Hw18|Hw17|Hw16, Hw18|Hw17);	// PXDW - Pixel Data Width
			#endif
			break;

		case LCDC_COMPONENT_720P:
			BITCSET(lcd_reg, Hw29|Hw28, Hw29|Hw28);				// R2YMD - RGB to YCbCr Conversion Option
			BITCSET(lcd_reg, Hw23, Hw23);						// CKG - Clock Gating Enable for Timing Generator
			#if defined(BOARD_TCC930X_STB_DEMO)
//			BITCSET(lcd_reg, Hw15, Hw15);						// ID - Inverted Data Enable
			BITCSET(lcd_reg, Hw14, Hw14);						// IV - Inverted Vertical Sync
			BITCSET(lcd_reg, Hw13, Hw13);						// IH - Inverted Horizontal Sync
//			BITCSET(lcd_reg, Hw12, Hw12);						// IP - Inverted Pixel Clock
			#else
			BITCSET(lcd_reg, Hw15, Hw15);						// ID - Inverted Data Enable
			BITCSET(lcd_reg, Hw14, Hw14);						// IV - Inverted Vertical Sync
//			BITCSET(lcd_reg, Hw13, Hw13);						// IH - Inverted Horizontal Sync
			BITCSET(lcd_reg, Hw12, Hw12);						// IP - Inverted Pixel Clock
			#endif
			BITCSET(lcd_reg, Hw8, Hw8);							// NI - Non-interlace Mode
			BITCSET(lcd_reg, Hw3|Hw2|Hw1, Hw3|Hw1);				// OVP
			BITCSET(lcd_reg, Hw0, Hw0);							// LEN - LCD Controller Enable
			#if 0 // COMPONENT_16BPP_YCBCR
			BITCSET(lcd_reg, Hw10, Hw10);						// R2Y - RGB to YCbCr Converter Enable for OUPUT
			BITCSET(lcd_reg, Hw19|Hw18|Hw17|Hw16, Hw19);		// PXDW - Pixel Data Width
			#else
			BITCSET(lcd_reg, Hw19|Hw18|Hw17|Hw16, Hw19|Hw18);	// PXDW - Pixel Data Width
			#endif
			break;

		case LCDC_COMPONENT_1080I:
			BITCSET(lcd_reg, Hw29|Hw28, Hw29|Hw28);				// R2YMD - RGB to YCbCr Conversion Option
			BITCSET(lcd_reg, Hw23, Hw23);						// CKG - Clock Gating Enable for Timing Generator
			#if defined(BOARD_TCC930X_STB_DEMO)
//			BITCSET(lcd_reg, Hw15, Hw15);						// ID - Inverted Data Enable
			BITCSET(lcd_reg, Hw14, Hw14);						// IV - Inverted Vertical Sync
			BITCSET(lcd_reg, Hw13, Hw13);						// IH - Inverted Horizontal Sync
//			BITCSET(lcd_reg, Hw12, Hw12);						// IP - Inverted Pixel Clock
			#else
			BITCSET(lcd_reg, Hw15, Hw15);						// ID - Inverted Data Enable
			BITCSET(lcd_reg, Hw14, Hw14);						// IV - Inverted Vertical Sync
//			BITCSET(lcd_reg, Hw13, Hw13);						// IH - Inverted Horizontal Sync
			BITCSET(lcd_reg, Hw12, Hw12);						// IP - Inverted Pixel Clock
			#endif
			BITCSET(lcd_reg, Hw7, Hw7);							// TV
			BITCSET(lcd_reg, Hw3|Hw2|Hw1, Hw3|Hw1);				// OVP
			BITCSET(lcd_reg, Hw0, Hw0);							// LEN - LCD Controller Enable
			#if 0 // COMPONENT_16BPP_YCBCR
			BITCSET(lcd_reg, Hw10, Hw10);						// R2Y - RGB to YCbCr Converter Enable for OUPUT
			BITCSET(lcd_reg, Hw19|Hw18|Hw17|Hw16, Hw19);		// PXDW - Pixel Data Width
			#else
			BITCSET(lcd_reg, Hw19|Hw18|Hw17|Hw16, Hw19|Hw18);	// PXDW - Pixel Data Width
			#endif
			break;

		default:
			break;
	}
	
	BITCSET(pLCDC->LCTRL, 0xFFFFFFFF, lcd_reg);

	lcdc_set_logo(lcdc_num, spec.component_width, spec.component_height, fb_con);

	/* Enable LCDC */
	BITSET(pLCDC->LCTRL, Hw0);

	/* Init DDI Cache */
	ddic_setdefault();

	#if defined(BOARD_TCC880X_STB_DEMO)
		/* VE_FIELD: GPIO_C24 */
		BITCLR(pGPIO->GPCFN3, Hw4-Hw0);
		BITSET(pGPIO->GPCEN, Hw24);
		BITSET(pGPIO->GPCDAT, Hw24);
	#endif
	
	/* Enable Component Chip */
	#if defined(COMPONENT_CHIP_CS4954)
		if(type == LCDC_COMPONENT_480I_NTSC)
			cs4954_enable(COMPONENT_MODE_NTSC_M); // NTSC_M
		else
			cs4954_enable(COMPONENT_MODE_PAL_B); // PAL_B
	#elif defined(COMPONENT_CHIP_THS8200)
		if(type == LCDC_COMPONENT_720P)
			ths8200_enable(COMPONENT_MODE_720P); // 720P
		else
			ths8200_enable(COMPONENT_MODE_1080I); // 1080I
	#endif
}
#endif //defined(DEFAULT_DISPLAY_COMPONENT) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)

#if defined(DEFAULT_DISPLAY_LCD)
static struct lcd_panel * lcdc_io_init(unsigned char lcdc_num)
{
	struct lcd_panel *panel;
	unsigned int 	lclk, lcd_div;
	PDDICONFIG		pDDICfg 	= (DDICONFIG *)HwDDI_CONFIG_BASE;


	BITSET(pDDICfg->SWRESET, Hw3);
	BITSET(pDDICfg->SWRESET, Hw2);

	BITCLR(pDDICfg->SWRESET, Hw3);
	BITCLR(pDDICfg->SWRESET, Hw2);	


	panel = tccfb_get_panel();

	panel->dev.power_on = GPIO_LCD_ON;
	panel->dev.display_on = GPIO_LCD_DISPLAY;
	panel->dev.bl_on = GPIO_LCD_BL;
	panel->dev.reset = GPIO_LCD_RESET;

	panel->dev.lcdc_num = lcdc_num;
	
	panel->init(panel);

	if(lcdc_num)
	{
		tca_ckc_setperi(PERI_LCD1,ENABLE, panel->clk_freq * panel->clk_div);
		lclk  = tca_ckc_getperi(PERI_LCD1);
	}
	else
	{
		tca_ckc_setperi(PERI_LCD0,ENABLE, panel->clk_freq * panel->clk_div);
		lclk  = tca_ckc_getperi(PERI_LCD0);
	}
	printf("telechips tcc88xx %s lcdc:%d clk:%d set clk:%d \n", __func__, lcdc_num, panel->clk_freq, lclk);


#ifdef LCD_CPU_INTERFACE // LCD SI clock enable
	BITCLR(pDDICfg->LVDS_CTRL,  HwDDIC_PWDN_LCDSI);
	tca_ckc_setperi(PERI_LCDSI,ENABLE, panel->clk_freq);
#endif//
	ddic_setdefault();

	panel->set_power(panel, 1);

	printf("%s end\n", __func__);

	return panel;
}
#endif
unsigned *target_atag_panel(unsigned *ptr)
{
	unsigned panel_id;

	struct lcd_panel *panel;

	panel = tccfb_get_panel();
	panel_id = panel->id;

	*ptr++ = 3;
	*ptr++ = ATAG_TCC_PANEL;
	*ptr++ = panel_id;
	return ptr;
}

#if defined(DEFAULT_DISPLAY_LCD)
struct fbcon_config *lcdc_init(void)
{
	#define LCDC_NUM 	1
	
	struct lcd_panel *panel_info;
	struct fbcon_config *fbcon_cfg;
	struct tcc_lcdc_image_update Image_info;

	panel_info = lcdc_io_init(LCDC_NUM);
	
	dprintf(INFO, "lcdc: panel is %d x %d %dbpp\n",	panel_info->xres, panel_info->yres, fb_cfg.bpp);

	#ifdef DISPLAY_SPLASH_SCREEN_DIRECT
	fb_cfg.stride = fb_cfg.width;
	#else
	fb_cfg.width = panel_info->xres;
	fb_cfg.height = panel_info->yres;
	fb_cfg.stride = fb_cfg.width;
	fb_cfg.bpp = LCDC_FB_BPP;
	fb_cfg.format = FB_FORMAT_RGB565;

	fb_cfg.base = dma_alloc(4096, fb_cfg.width * fb_cfg.height * (fb_cfg.bpp/8));

	if (fb_cfg.base == NULL)
	  dprintf(INFO, "lcdc: framebuffer alloc failed!\n");
	#endif//

	dprintf(INFO, "fb_cfg base:%p xres:%d yres:%d bpp:%d \n",fb_cfg.base, fb_cfg.width, fb_cfg.height, fb_cfg.bpp);

	Image_info.addr0 = fb_cfg.base;
	Image_info.Lcdc_layer = 2;
	Image_info.enable = 1;

	Image_info.Frame_width = Image_info.Image_width = fb_cfg.width;
	Image_info.Frame_height = Image_info.Image_height = fb_cfg.height;

	if(Image_info.Image_width > panel_info->xres)
		Image_info.Image_width = panel_info->xres;

	if(panel_info->xres > Image_info.Frame_width)
		Image_info.offset_x = (panel_info->xres - Image_info.Frame_width)/2;
	else
		Image_info.offset_x =  0;

	if(panel_info->yres > Image_info.Frame_height)
		Image_info.offset_y = (panel_info->yres - Image_info.Frame_height)/2;
	else
		Image_info.offset_y = 0;
	
	#ifdef _LCD_32BPP_
	Image_info.fmt = TCC_LCDC_IMG_FMT_RGB888;
	#else
	Image_info.fmt = TCC_LCDC_IMG_FMT_RGB565;
	#endif
	tcclcd_image_ch_set(LCDC_NUM, &Image_info);
	mdelay(1);
	panel_info->set_backlight_level(panel_info, MAX_BACKLIGTH);

	fbcon_cfg = &fb_cfg;

	return fbcon_cfg;
}
#else
struct fbcon_config *lcdc_init(void)
{
	struct fbcon_config *fbcon_cfg;
	unsigned char lcdc_1st;
	unsigned char lcdc_2nd;
	
	#if defined(_TCC9300_) || defined(_TCC8800_) || defined(_TCC8800ST_)
		lcdc_1st = 0; //LCD_CONTROLLER_0
		lcdc_2nd = 1; //LCD_CONTROLLER_1
	#else
		lcdc_1st = 1; //LCD_CONTROLLER_1
		lcdc_2nd = 0; //LCD_CONTROLLER_0
	#endif

	#if defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
		#if defined(DISPLAY_STB_DUAL) 
			/* 1st Output Setting : HDMI */
			lcdc_io_init_hdmi(lcdc_1st); 

			/* 2nd Output Setting : COMPOSITE or COMPONENT */
			if(tcc_param.output == 3)
				lcdc_io_init_component(lcdc_2nd, LCDC_COMPONENT_720P + tcc_param.component_resolution);
			else
				lcdc_io_init_composite(lcdc_2nd, tcc_param.composite_resolution);
		#elif defined(DISPLAY_STB_AUTO_DETECT)
			/* 1st Output Setting : HDMI */
			lcdc_io_init_hdmi(lcdc_1st);

			/* 2nd Output Setting : COMPOSITE or COMPONENT */
			if(tcc_param.output == 3)
			{
				if(component_detect() == true)
					lcdc_io_init_component(lcdc_2nd, LCDC_COMPONENT_720P + tcc_param.component_resolution);
				else if(composite_detect() == true)
					lcdc_io_init_composite(lcdc_2nd, tcc_param.composite_resolution);
			}
			else
			{
				if(composite_detect() == true)
					lcdc_io_init_composite(lcdc_2nd, tcc_param.composite_resolution);
				else if(component_detect() == true)
					lcdc_io_init_component(lcdc_2nd, LCDC_COMPONENT_720P + tcc_param.component_resolution);
			}
		#else
 			switch(tcc_param.output)
			{
				case 0: /* NONE */
				case 1: /* HDMI */
					lcdc_io_init_hdmi(lcdc_1st);
					break;

				case 2:  /* COMPOSITE */
					lcdc_io_init_composite(lcdc_2nd, tcc_param.composite_resolution);
					break;

				case 3:  /* COMPONENT */
					#if defined(COMPONENT_CHIP_CS4954)
						lcdc_io_init_component(lcdc_2nd, LCDC_COMPONENT_480I_NTSC + tcc_param.component_resolution);
			 		#else
						lcdc_io_init_component(lcdc_2nd, LCDC_COMPONENT_720P + tcc_param.component_resolution);
					#endif
					break;

				default:
					lcdc_io_init_hdmi(lcdc_1st);
					break;
			}
 		#endif
	#else
		lcdc_io_init_hdmi(lcdc_1st);
		lcdc_io_init_composite(lcdc_2nd, LCDC_COMPOSITE_NTSC);
		//lcdc_io_init_component(lcdc_2nd, LCDC_COMPONENT_720P);
	#endif

	#if defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
	fbcon_cfg = &fb0_cfg;
	#else
	fbcon_cfg = &fb_cfg;
	#endif

	return fbcon_cfg;
}
#endif


