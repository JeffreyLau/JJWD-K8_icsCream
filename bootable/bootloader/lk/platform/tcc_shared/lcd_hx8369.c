/*
 * at070tn93_lcd.c -- support for TPO AT070TN93 LCD
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

#ifdef HX8369
#include <dev/gpio.h>
#include <tca_tco.h>
#include <platform/gpio.h>
#include "IO_TCCXXX.h"


//LCD_RST
#define GPIO_B29 29

//SPI-SCL
#define GPIO_G03 3
//LCD_CLK
//#define GPIO_B00 0

//SPI-CS
#define GPIO_G04 4

//SPI-SDI
#define GPIO_C16  16
#define SPI_GPIO_CS                   TCC_GPG(GPIO_G04)
#define SPI_GPIO_CLK                 TCC_GPG(GPIO_G03)
#define SPI_GPIO_RES                 TCC_GPB(GPIO_B29)
#define SPI_GPIO_MOSI              TCC_GPC(GPIO_C16)

#define SPI_CS_VALUE(x)               tcclcd_gpio_set_value(SPI_GPIO_CS, !!x)
#define SPI_CLK_VALUE(x)             tcclcd_gpio_set_value(SPI_GPIO_CLK, !!x)
#define SPI_RES_VALUE(x)             tcclcd_gpio_set_value(SPI_GPIO_RES, !!x)
#define SPI_MOSI_VALUE(x)          tcclcd_gpio_set_value(SPI_GPIO_MOSI, !!x)



static void SPI_SendData(unsigned char i)
{  
   unsigned char n;
   
   for(n=0; n<8; n++)			
   {  
	  if(i&0x80){
	  	SPI_MOSI_VALUE(1);
	  }else{
	        SPI_MOSI_VALUE(0);
	  }
          i<<= 1;
         SPI_CLK_VALUE(0);
         SPI_CLK_VALUE(1); 	
   }
}

static void SPI_WriteComm(unsigned int i)	  //spec page 46
{
    SPI_CS_VALUE(0);
	SPI_SendData(0x20);	 //high 8bit
	SPI_SendData(i>>8);
	SPI_SendData(0x00);	//low 8bit
	SPI_SendData(i);
         SPI_CS_VALUE(1);
}

static void SPI_WriteData(unsigned int i)
{
         SPI_CS_VALUE(0);
	SPI_SendData(0x40);
	SPI_SendData(i);
         SPI_CS_VALUE(1);
}
static int hm8369_panel_init(struct lcd_panel *panel)
{
	printf("LCD_HX8369_Init enter\n");
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d  \n", __func__, 0);

	tcclcd_gpio_config(pdata->display_on, GPIO_OUTPUT);
	tcclcd_gpio_config(pdata->bl_on, GPIO_OUTPUT);
	tcclcd_gpio_config(pdata->reset, GPIO_OUTPUT);
	tcclcd_gpio_config(pdata->power_on, GPIO_OUTPUT);

	tcclcd_gpio_set_value(pdata->display_on, 0);
	tcclcd_gpio_set_value(pdata->bl_on, 0);
	tcclcd_gpio_set_value(pdata->reset, 0);
	tcclcd_gpio_set_value(pdata->power_on, 0);
	
	unsigned int spi_cs = SPI_GPIO_CS;
	unsigned int spi_scl = SPI_GPIO_CLK;	
	unsigned int spi_rst = SPI_GPIO_RES;	
	unsigned int spi_mosi = SPI_GPIO_MOSI;	
	
	tcclcd_gpio_config(spi_cs, GPIO_OUTPUT);
	tcclcd_gpio_config(spi_scl, GPIO_OUTPUT);	
	tcclcd_gpio_config(spi_rst, GPIO_OUTPUT);
	tcclcd_gpio_config(spi_mosi, GPIO_OUTPUT);	

	
//	gpio_request(spi_cs, "spi_cs");
	tcclcd_gpio_set_value(spi_cs, 1);

//	gpio_request(spi_scl, "spi_scl");
	tcclcd_gpio_set_value(spi_scl, 1);

//	gpio_request(spi_rst, "spi_rst");
	tcclcd_gpio_set_value(spi_rst, 1);

//	gpio_request(spi_mosi, "spi_mosi");
	tcclcd_gpio_set_value(spi_mosi, 1); 
	mdelay(100);

	//SPI_RES_VALUE(1) ;
	//mdelay(100);
	SPI_RES_VALUE(0) ;
    mdelay(500);
    SPI_RES_VALUE(1) ;
	mdelay(800);
	
	SPI_WriteComm(0xff00); SPI_WriteData(0x80);
	SPI_WriteComm(0xff01); SPI_WriteData(0x09); //enable EXTC
	SPI_WriteComm(0xff02); SPI_WriteData(0x01);
	SPI_WriteComm(0xff80); SPI_WriteData(0x80); //enable Orise mode
	SPI_WriteComm(0xff81); SPI_WriteData(0x09);
	SPI_WriteComm(0xff03); SPI_WriteData(0x01); //enable SPI+I2C cmd2 read
	
	//gamma DC
	SPI_WriteComm(0xc0b4); SPI_WriteData(0x50); //column inversion	
	SPI_WriteComm(0xC489); SPI_WriteData(0x08); //reg off	
	SPI_WriteComm(0xC0a3); SPI_WriteData(0x00); //pre-charge //V02			   
	SPI_WriteComm(0xC582); SPI_WriteData(0xA3); //REG-pump23
	SPI_WriteComm(0xC590); SPI_WriteData(0x96); //Pump setting (3x=D6)-->(2x=96)//v02 01/11
	SPI_WriteComm(0xC591); SPI_WriteData(0x87); //Pump setting(VGH/VGL)   
	SPI_WriteComm(0xD800); SPI_WriteData(0x73); //GVDD=4.5V  73   
	SPI_WriteComm(0xD801); SPI_WriteData(0x71); //NGVDD=4.5V 71  
	 
	//VCOMDC																 
	SPI_WriteComm(0xD900); SPI_WriteData(0x6A); // VCOMDC=	 
	mdelay(20);
	  
	// Positive
	SPI_WriteComm(0xE100); SPI_WriteData(0x09);
	SPI_WriteComm(0xE101); SPI_WriteData(0x0a);
	SPI_WriteComm(0xE102); SPI_WriteData(0x0e);
	SPI_WriteComm(0xE103); SPI_WriteData(0x0d);
	SPI_WriteComm(0xE104); SPI_WriteData(0x07);
	SPI_WriteComm(0xE105); SPI_WriteData(0x18);
	SPI_WriteComm(0xE106); SPI_WriteData(0x0d);
	SPI_WriteComm(0xE107); SPI_WriteData(0x0d);
	SPI_WriteComm(0xE108); SPI_WriteData(0x01);
	SPI_WriteComm(0xE109); SPI_WriteData(0x04);
	SPI_WriteComm(0xE10A); SPI_WriteData(0x05);
	SPI_WriteComm(0xE10B); SPI_WriteData(0x06);
	SPI_WriteComm(0xE10C); SPI_WriteData(0x0D);
	SPI_WriteComm(0xE10D); SPI_WriteData(0x22);
	SPI_WriteComm(0xE10E); SPI_WriteData(0x20);
	SPI_WriteComm(0xE10F); SPI_WriteData(0x05);
	
	// Negative
	SPI_WriteComm(0xE200); SPI_WriteData(0x09);
	SPI_WriteComm(0xE201); SPI_WriteData(0x0a);
	SPI_WriteComm(0xE202); SPI_WriteData(0x0e);
	SPI_WriteComm(0xE203); SPI_WriteData(0x0d);
	SPI_WriteComm(0xE204); SPI_WriteData(0x07);
	SPI_WriteComm(0xE205); SPI_WriteData(0x18);
	SPI_WriteComm(0xE206); SPI_WriteData(0x0d);
	SPI_WriteComm(0xE207); SPI_WriteData(0x0d);
	SPI_WriteComm(0xE208); SPI_WriteData(0x01);
	SPI_WriteComm(0xE209); SPI_WriteData(0x04);
	SPI_WriteComm(0xE20A); SPI_WriteData(0x05);
	SPI_WriteComm(0xE20B); SPI_WriteData(0x06);
	SPI_WriteComm(0xE20C); SPI_WriteData(0x0D);
	SPI_WriteComm(0xE20D); SPI_WriteData(0x22);
	SPI_WriteComm(0xE20E); SPI_WriteData(0x20);
	SPI_WriteComm(0xE20F); SPI_WriteData(0x05);
	
	SPI_WriteComm(0xC181); SPI_WriteData(0x66); 	//Frame rate 65Hz//V02	 
	//SPI_WriteComm(0xC181); SPI_WriteData(0x00); 	//Frame rate 35Hz//V02
	
	// RGB I/F setting VSYNC for OTM8018 0x0e
	SPI_WriteComm(0xC1a1); SPI_WriteData(0x08); 	//external Vsync,Hsync,DE
	
	SPI_WriteComm(0xC0a3); SPI_WriteData(0x1b); 	//pre-charge //V02
	SPI_WriteComm(0xC481); SPI_WriteData(0x83); 	//source bias //V02
	SPI_WriteComm(0xC592); SPI_WriteData(0x01); 	//Pump45
	SPI_WriteComm(0xC5B1); SPI_WriteData(0xA9); 	//DC voltage setting ;[0]GVDD output, default: 0xa8
	
	// CE8x : vst1, vst2, vst3, vst4
	SPI_WriteComm(0xCE80); SPI_WriteData(0x85); // ce81[7:0] : vst1_shift[7:0]
	SPI_WriteComm(0xCE81); SPI_WriteData(0x03); // ce82[7:0] : 0000,	vst1_width[3:0]
	SPI_WriteComm(0xCE82); SPI_WriteData(0x00); // ce83[7:0] : vst1_tchop[7:0]
	SPI_WriteComm(0xCE83); SPI_WriteData(0x84); // ce84[7:0] : vst2_shift[7:0]
	SPI_WriteComm(0xCE84); SPI_WriteData(0x03); // ce85[7:0] : 0000,	vst2_width[3:0]
	SPI_WriteComm(0xCE85); SPI_WriteData(0x00); // ce86[7:0] : vst2_tchop[7:0]																														  
	SPI_WriteComm(0xCE86); SPI_WriteData(0x83); // ce87[7:0] : vst3_shift[7:0]																														  
	SPI_WriteComm(0xCE87); SPI_WriteData(0x03); // ce88[7:0] : 0000,	vst3_width[3:0]
	SPI_WriteComm(0xCE88); SPI_WriteData(0x00); // ce89[7:0] : vst3_tchop[7:0]													   
	SPI_WriteComm(0xCE89); SPI_WriteData(0x82); // ce8a[7:0] : vst4_shift[7:0]
	SPI_WriteComm(0xCE8a); SPI_WriteData(0x03); // ce8b[7:0] : 0000,	vst4_width[3:0]
	SPI_WriteComm(0xCE8b); SPI_WriteData(0x00); // ce8c[7:0] : vst4_tchop[7:0]
	
	//CEAx : clka1, clka2
	SPI_WriteComm(0xCEa0); SPI_WriteData(0x38); // cea1[7:0] : clka1_width[3:0], clka1_shift[11:8]
	SPI_WriteComm(0xCEa1); SPI_WriteData(0x02); // cea2[7:0] : clka1_shift[7:0]
	SPI_WriteComm(0xCEa2); SPI_WriteData(0x03); // cea3[7:0] : clka1_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]												
	SPI_WriteComm(0xCEa3); SPI_WriteData(0x21); // cea4[7:0] : clka1_switch[7:0]																						
	SPI_WriteComm(0xCEa4); SPI_WriteData(0x00); // cea5[7:0] : clka1_extend[7:0]																						
	SPI_WriteComm(0xCEa5); SPI_WriteData(0x00); // cea6[7:0] : clka1_tchop[7:0] 																						
	SPI_WriteComm(0xCEa6); SPI_WriteData(0x00); // cea7[7:0] : clka1_tglue[7:0] 																						
	SPI_WriteComm(0xCEa7); SPI_WriteData(0x38); // cea8[7:0] : clka2_width[3:0], clka2_shift[11:8]																		
	SPI_WriteComm(0xCEa8); SPI_WriteData(0x01); // cea9[7:0] : clka2_shift[7:0] 																						
	SPI_WriteComm(0xCEa9); SPI_WriteData(0x03); // ceaa[7:0] : clka2_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]												
	SPI_WriteComm(0xCEaa); SPI_WriteData(0x22); // ceab[7:0] : clka2_switch[7:0]																						
	SPI_WriteComm(0xCEab); SPI_WriteData(0x00); // ceac[7:0] : clka2_extend 																							
	SPI_WriteComm(0xCEac); SPI_WriteData(0x00); // cead[7:0] : clka2_tchop																								
	SPI_WriteComm(0xCEad); SPI_WriteData(0x00); // ceae[7:0] : clka2_tglue																								
																																								
	//CEBx : clka3, clka4																																		
	SPI_WriteComm(0xCEb0); SPI_WriteData(0x38); // ceb1[7:0] : clka3_width[3:0], clka3_shift[11:8]																		
	SPI_WriteComm(0xCEb1); SPI_WriteData(0x00); // ceb2[7:0] : clka3_shift[7:0] 																						
	SPI_WriteComm(0xCEb2); SPI_WriteData(0x03); // ceb3[7:0] : clka3_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]												
	SPI_WriteComm(0xCEb3); SPI_WriteData(0x23); // ceb4[7:0] : clka3_switch[7:0]																						
	SPI_WriteComm(0xCEb4); SPI_WriteData(0x00); // ceb5[7:0] : clka3_extend[7:0]																						
	SPI_WriteComm(0xCEb5); SPI_WriteData(0x00); // ceb6[7:0] : clka3_tchop[7:0] 																						
	SPI_WriteComm(0xCEb6); SPI_WriteData(0x00); // ceb7[7:0] : clka3_tglue[7:0] 																						
	SPI_WriteComm(0xCEb7); SPI_WriteData(0x30); // ceb8[7:0] : clka4_width[3:0], clka2_shift[11:8]																		
	SPI_WriteComm(0xCEb8); SPI_WriteData(0x00); // ceb9[7:0] : clka4_shift[7:0] 																						
	SPI_WriteComm(0xCEb9); SPI_WriteData(0x03); // ceba[7:0] : clka4_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]												
	SPI_WriteComm(0xCEba); SPI_WriteData(0x24); // cebb[7:0] : clka4_switch[7:0]																						
	SPI_WriteComm(0xCEbb); SPI_WriteData(0x00); // cebc[7:0] : clka4_extend 																							
	SPI_WriteComm(0xCEbc); SPI_WriteData(0x00); // cebd[7:0] : clka4_tchop																								
	SPI_WriteComm(0xCEbd); SPI_WriteData(0x00); // cebe[7:0] : clka4_tglue																								
																																								
	//CECx : clkb1, clkb2																																		
	SPI_WriteComm(0xCEc0); SPI_WriteData(0x30); // cec1[7:0] : clkb1_width[3:0], clkb1_shift[11:8]																		
	SPI_WriteComm(0xCEc1); SPI_WriteData(0x01); // cec2[7:0] : clkb1_shift[7:0] 																						
	SPI_WriteComm(0xCEc2); SPI_WriteData(0x03); // cec3[7:0] : clkb1_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]												
	SPI_WriteComm(0xCEc3); SPI_WriteData(0x25); // cec4[7:0] : clkb1_switch[7:0]																						
	SPI_WriteComm(0xCEc4); SPI_WriteData(0x00); // cec5[7:0] : clkb1_extend[7:0]																						
	SPI_WriteComm(0xCEc5); SPI_WriteData(0x00); // cec6[7:0] : clkb1_tchop[7:0] 																						
	SPI_WriteComm(0xCEc6); SPI_WriteData(0x00); // cec7[7:0] : clkb1_tglue[7:0] 																						
	SPI_WriteComm(0xCEc7); SPI_WriteData(0x30); // cec8[7:0] : clkb2_width[3:0], clkb2_shift[11:8]																		
	SPI_WriteComm(0xCEc8); SPI_WriteData(0x02); // cec9[7:0] : clkb2_shift[7:0] 																						
	SPI_WriteComm(0xCEc9); SPI_WriteData(0x03); // ceca[7:0] : clkb2_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]												
	SPI_WriteComm(0xCEca); SPI_WriteData(0x26); // cecb[7:0] : clkb2_switch[7:0]																						
	SPI_WriteComm(0xCEcb); SPI_WriteData(0x00); // cecc[7:0] : clkb2_extend 																							
	SPI_WriteComm(0xCEcc); SPI_WriteData(0x00); // cecd[7:0] : clkb2_tchop																								
	SPI_WriteComm(0xCEcd); SPI_WriteData(0x00); // cece[7:0] : clkb2_tglue																								
																																								
	//CEDx : clkb3, clkb4																																		
	SPI_WriteComm(0xCEd0); SPI_WriteData(0x30); // ced1[7:0] : clkb3_width[3:0], clkb3_shift[11:8]																		
	SPI_WriteComm(0xCEd1); SPI_WriteData(0x03); // ced2[7:0] : clkb3_shift[7:0] 																						
	SPI_WriteComm(0xCEd2); SPI_WriteData(0x03); // ced3[7:0] : clkb3_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]												
	SPI_WriteComm(0xCEd3); SPI_WriteData(0x27); // ced4[7:0] : clkb3_switch[7:0]																						
	SPI_WriteComm(0xCEd4); SPI_WriteData(0x00); // ced5[7:0] : clkb3_extend[7:0]																						
	SPI_WriteComm(0xCEd5); SPI_WriteData(0x00); // ced6[7:0] : clkb3_tchop[7:0] 																						
	SPI_WriteComm(0xCEd6); SPI_WriteData(0x00); // ced7[7:0] : clkb3_tglue[7:0] 																						
	SPI_WriteComm(0xCEd7); SPI_WriteData(0x30); // ced8[7:0] : clkb4_width[3:0], clkb4_shift[11:8]																		
	SPI_WriteComm(0xCEd8); SPI_WriteData(0x04); // ced9[7:0] : clkb4_shift[7:0] 																						
	SPI_WriteComm(0xCEd9); SPI_WriteData(0x03); // ceda[7:0] : clkb4_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]												
	SPI_WriteComm(0xCEda); SPI_WriteData(0x28); // cedb[7:0] : clkb4_switch[7:0]																						
	SPI_WriteComm(0xCEdb); SPI_WriteData(0x00); // cedc[7:0] : clkb4_extend 																							
	SPI_WriteComm(0xCEdc); SPI_WriteData(0x00); // cedd[7:0] : clkb4_tchop																								
	SPI_WriteComm(0xCEdd); SPI_WriteData(0x00); // cede[7:0] : clkb4_tglue																								
																																								
	//CFCx :																																					
	SPI_WriteComm(0xCFc0); SPI_WriteData(0x00); // cfc1[7:0] : eclk_normal_width[7:0]																					
	SPI_WriteComm(0xCFc1); SPI_WriteData(0x00); // cfc2[7:0] : eclk_partial_width[7:0]																					
	SPI_WriteComm(0xCFc2); SPI_WriteData(0x00); // cfc3[7:0] : all_normal_tchop[7:0]																					
	SPI_WriteComm(0xCFc3); SPI_WriteData(0x00); // cfc4[7:0] : all_partial_tchop[7:0]																					
	SPI_WriteComm(0xCFc4); SPI_WriteData(0x00); // cfc5[7:0] : eclk1_follow[3:0], eclk2_follow[3:0] 																	
	SPI_WriteComm(0xCFc5); SPI_WriteData(0x00); // cfc6[7:0] : eclk3_follow[3:0], eclk4_follow[3:0] 																	
	SPI_WriteComm(0xCFc6); SPI_WriteData(0x00); // cfc7[7:0] : 00, vstmask, vendmask, 00, dir1, dir2 (0=VGL, 1=VGH) 													
	SPI_WriteComm(0xCFc7); SPI_WriteData(0x00); // cfc8[7:0] : reg_goa_gnd_opt, reg_goa_dpgm_tail_set, reg_goa_f_gating_en, reg_goa_f_odd_gating, toggle_mod1, 2, 3, 4	
	SPI_WriteComm(0xCFc8); SPI_WriteData(0x00); // cfc9[7:0] : duty_block[3:0], DGPM[3:0]																				
	SPI_WriteComm(0xCFc9); SPI_WriteData(0x00); // cfca[7:0] : reg_goa_gnd_period[7:0]																					
	
	//CFDx :
	SPI_WriteComm(0xCFd0);	SPI_WriteData(0x00);// cfd1[7:0] : 0000000, reg_goa_frame_odd_high
	// Parameter 1
	
	//--------------------------------------------------------------------------------
	//		initial setting 3 < Panel setting >
	//--------------------------------------------------------------------------------
	// cbcx
	SPI_WriteComm(0xCBc0); SPI_WriteData(0x00); //cbc1[7:0] : enmode H-byte of sig1  (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBc1); SPI_WriteData(0x00); //cbc2[7:0] : enmode H-byte of sig2  (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBc2); SPI_WriteData(0x00); //cbc3[7:0] : enmode H-byte of sig3  (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBc3); SPI_WriteData(0x00); //cbc4[7:0] : enmode H-byte of sig4  (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBc4); SPI_WriteData(0x04); //cbc5[7:0] : enmode H-byte of sig5  (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBc5); SPI_WriteData(0x04); //cbc6[7:0] : enmode H-byte of sig6  (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBc6); SPI_WriteData(0x04); //cbc7[7:0] : enmode H-byte of sig7  (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBc7); SPI_WriteData(0x04); //cbc8[7:0] : enmode H-byte of sig8  (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBc8); SPI_WriteData(0x04); //cbc9[7:0] : enmode H-byte of sig9  (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBc9); SPI_WriteData(0x04); //cbca[7:0] : enmode H-byte of sig10 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBca); SPI_WriteData(0x00); //cbcb[7:0] : enmode H-byte of sig11 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBcb); SPI_WriteData(0x00); //cbcc[7:0] : enmode H-byte of sig12 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBcc); SPI_WriteData(0x00); //cbcd[7:0] : enmode H-byte of sig13 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBcd); SPI_WriteData(0x00); //cbce[7:0] : enmode H-byte of sig14 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBce); SPI_WriteData(0x00); //cbcf[7:0] : enmode H-byte of sig15 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
																																		 
	// cbdx 																															 
	SPI_WriteComm(0xCBd0); SPI_WriteData(0x00); //cbd1[7:0] : enmode H-byte of sig16 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBd1); SPI_WriteData(0x00); //cbd2[7:0] : enmode H-byte of sig17 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBd2); SPI_WriteData(0x00); //cbd3[7:0] : enmode H-byte of sig18 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBd3); SPI_WriteData(0x00); //cbd4[7:0] : enmode H-byte of sig19 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBd4); SPI_WriteData(0x00); //cbd5[7:0] : enmode H-byte of sig20 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBd5); SPI_WriteData(0x00); //cbd6[7:0] : enmode H-byte of sig21 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBd6); SPI_WriteData(0x00); //cbd7[7:0] : enmode H-byte of sig22 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBd7); SPI_WriteData(0x00); //cbd8[7:0] : enmode H-byte of sig23 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBd8); SPI_WriteData(0x00); //cbd9[7:0] : enmode H-byte of sig24 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBd9); SPI_WriteData(0x04); //cbda[7:0] : enmode H-byte of sig25 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBda); SPI_WriteData(0x04); //cbdb[7:0] : enmode H-byte of sig26 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBdb); SPI_WriteData(0x04); //cbdc[7:0] : enmode H-byte of sig27 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBdc); SPI_WriteData(0x04); //cbdd[7:0] : enmode H-byte of sig28 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBdd); SPI_WriteData(0x04); //cbde[7:0] : enmode H-byte of sig29 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBde); SPI_WriteData(0x04); //cbdf[7:0] : enmode H-byte of sig30 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
																 
	// cbex 													 
	SPI_WriteComm(0xCBe0); SPI_WriteData(0x00); //cbe1[7:0] : enmode H-byte of sig31 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBe1); SPI_WriteData(0x00); //cbe2[7:0] : enmode H-byte of sig32 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBe2); SPI_WriteData(0x00); //cbe3[7:0] : enmode H-byte of sig33 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBe3); SPI_WriteData(0x00); //cbe4[7:0] : enmode H-byte of sig34 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBe4); SPI_WriteData(0x00); //cbe5[7:0] : enmode H-byte of sig35 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBe5); SPI_WriteData(0x00); //cbe6[7:0] : enmode H-byte of sig36 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBe6); SPI_WriteData(0x00); //cbe7[7:0] : enmode H-byte of sig37 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBe7); SPI_WriteData(0x00); //cbe8[7:0] : enmode H-byte of sig38 (pwrof_0, pwrof_1, norm, pwron_4 ) 						 
	SPI_WriteComm(0xCBe8); SPI_WriteData(0x00); //cbe9[7:0] : enmode H-byte of sig39 (pwrof_0, pwrof_1, norm, pwron_4 ) 																								 
	SPI_WriteComm(0xCBe9); SPI_WriteData(0x00); //cbea[7:0] : enmode H-byte of sig40 (pwrof_0, pwrof_1, norm, pwron_4 ) 																								 
													  
	// cc8x 										  
	SPI_WriteComm(0xCC80); SPI_WriteData(0x00); //cc81[7:0] : reg setting for signal01 selection with u2d mode									 
	SPI_WriteComm(0xCC81); SPI_WriteData(0x00); //cc82[7:0] : reg setting for signal02 selection with u2d mode									 
	SPI_WriteComm(0xCC82); SPI_WriteData(0x00); //cc83[7:0] : reg setting for signal03 selection with u2d mode									 
	SPI_WriteComm(0xCC83); SPI_WriteData(0x00); //cc84[7:0] : reg setting for signal04 selection with u2d mode									 
	SPI_WriteComm(0xCC84); SPI_WriteData(0x0C); //cc85[7:0] : reg setting for signal05 selection with u2d mode									 
	SPI_WriteComm(0xCC85); SPI_WriteData(0x0A); //cc86[7:0] : reg setting for signal06 selection with u2d mode									 
	SPI_WriteComm(0xCC86); SPI_WriteData(0x10); //cc87[7:0] : reg setting for signal07 selection with u2d mode									 
	SPI_WriteComm(0xCC87); SPI_WriteData(0x0E); //cc88[7:0] : reg setting for signal08 selection with u2d mode									 
	SPI_WriteComm(0xCC88); SPI_WriteData(0x03); //cc89[7:0] : reg setting for signal09 selection with u2d mode									 
	SPI_WriteComm(0xCC89); SPI_WriteData(0x04); //cc8a[7:0] : reg setting for signal10 selection with u2d mode									 
																																		 
	// cc9x 																															 
	SPI_WriteComm(0xCC90); SPI_WriteData(0x00); //cc91[7:0] : reg setting for signal11 selection with u2d mode									 
	SPI_WriteComm(0xCC91); SPI_WriteData(0x00); //cc92[7:0] : reg setting for signal12 selection with u2d mode									 
	SPI_WriteComm(0xCC92); SPI_WriteData(0x00); //cc93[7:0] : reg setting for signal13 selection with u2d mode									 
	SPI_WriteComm(0xCC93); SPI_WriteData(0x00); //cc94[7:0] : reg setting for signal14 selection with u2d mode									 
	SPI_WriteComm(0xCC94); SPI_WriteData(0x00); //cc95[7:0] : reg setting for signal15 selection with u2d mode									 
	SPI_WriteComm(0xCC95); SPI_WriteData(0x00); //cc96[7:0] : reg setting for signal16 selection with u2d mode									 
	SPI_WriteComm(0xCC96); SPI_WriteData(0x00); //cc97[7:0] : reg setting for signal17 selection with u2d mode									 
	SPI_WriteComm(0xCC97); SPI_WriteData(0x00); //cc98[7:0] : reg setting for signal18 selection with u2d mode									 
	SPI_WriteComm(0xCC98); SPI_WriteData(0x00); //cc99[7:0] : reg setting for signal19 selection with u2d mode									 
	SPI_WriteComm(0xCC99); SPI_WriteData(0x00); //cc9a[7:0] : reg setting for signal20 selection with u2d mode									 
	SPI_WriteComm(0xCC9a); SPI_WriteData(0x00); //cc9b[7:0] : reg setting for signal21 selection with u2d mode									 
	SPI_WriteComm(0xCC9b); SPI_WriteData(0x00); //cc9c[7:0] : reg setting for signal22 selection with u2d mode									 
	SPI_WriteComm(0xCC9c); SPI_WriteData(0x00); //cc9d[7:0] : reg setting for signal23 selection with u2d mode									 
	SPI_WriteComm(0xCC9d); SPI_WriteData(0x00); //cc9e[7:0] : reg setting for signal24 selection with u2d mode									 
	SPI_WriteComm(0xCC9e); SPI_WriteData(0x0B); //cc9f[7:0] : reg setting for signal25 selection with u2d mode									 
																																		 
	// ccax 																															 
	SPI_WriteComm(0xCCa0); SPI_WriteData(0x09); //cca1[7:0] : reg setting for signal26 selection with u2d mode									 
	SPI_WriteComm(0xCCa1); SPI_WriteData(0x0F); //cca2[7:0] : reg setting for signal27 selection with u2d mode									 
	SPI_WriteComm(0xCCa2); SPI_WriteData(0x0D); //cca3[7:0] : reg setting for signal28 selection with u2d mode									 
	SPI_WriteComm(0xCCa3); SPI_WriteData(0x01); //cca4[7:0] : reg setting for signal29 selection with u2d mode									 
	SPI_WriteComm(0xCCa4); SPI_WriteData(0x02); //cca5[7:0] : reg setting for signal20 selection with u2d mode									 
	SPI_WriteComm(0xCCa5); SPI_WriteData(0x00); //cca6[7:0] : reg setting for signal31 selection with u2d mode									 
	SPI_WriteComm(0xCCa6); SPI_WriteData(0x00); //cca7[7:0] : reg setting for signal32 selection with u2d mode									 
	SPI_WriteComm(0xCCa7); SPI_WriteData(0x00); //cca8[7:0] : reg setting for signal33 selection with u2d mode									 
	SPI_WriteComm(0xCCa8); SPI_WriteData(0x00); //cca9[7:0] : reg setting for signal34 selection with u2d mode									 
	SPI_WriteComm(0xCCa9); SPI_WriteData(0x00); //ccaa[7:0] : reg setting for signal35 selection with u2d mode									 
	SPI_WriteComm(0xCCaa); SPI_WriteData(0x00); //ccab[7:0] : reg setting for signal36 selection with u2d mode									 
	SPI_WriteComm(0xCCab); SPI_WriteData(0x00); //ccac[7:0] : reg setting for signal37 selection with u2d mode									 
	SPI_WriteComm(0xCCac); SPI_WriteData(0x00); //ccad[7:0] : reg setting for signal38 selection with u2d mode									 
	SPI_WriteComm(0xCCad); SPI_WriteData(0x00); //ccae[7:0] : reg setting for signal39 selection with u2d mode									 
	SPI_WriteComm(0xCCae); SPI_WriteData(0x00); //ccaf[7:0] : reg setting for signal40 selection with u2d mode									 
																																		 
	// ccbx 																															 
	SPI_WriteComm(0xCCb0); SPI_WriteData(0x00); //ccb1[7:0] : reg setting for signal01 selection with d2u mode									 
	SPI_WriteComm(0xCCb1); SPI_WriteData(0x00); //ccb2[7:0] : reg setting for signal02 selection with d2u mode									 
	SPI_WriteComm(0xCCb2); SPI_WriteData(0x00); //ccb3[7:0] : reg setting for signal03 selection with d2u mode									 
	SPI_WriteComm(0xCCb3); SPI_WriteData(0x00); //ccb4[7:0] : reg setting for signal04 selection with d2u mode									 
	SPI_WriteComm(0xCCb4); SPI_WriteData(0x0D); //ccb5[7:0] : reg setting for signal05 selection with d2u mode									 
	SPI_WriteComm(0xCCb5); SPI_WriteData(0x0F); //ccb6[7:0] : reg setting for signal06 selection with d2u mode									 
	SPI_WriteComm(0xCCb6); SPI_WriteData(0x09); //ccb7[7:0] : reg setting for signal07 selection with d2u mode									 
	SPI_WriteComm(0xCCb7); SPI_WriteData(0x0B); //ccb8[7:0] : reg setting for signal08 selection with d2u mode									 
	SPI_WriteComm(0xCCb8); SPI_WriteData(0x02); //ccb9[7:0] : reg setting for signal09 selection with d2u mode									 
	SPI_WriteComm(0xCCb9); SPI_WriteData(0x01); //ccba[7:0] : reg setting for signal10 selection with d2u mode									 
																																		 
	// cccx 																															 
	SPI_WriteComm(0xCCc0); SPI_WriteData(0x00); //ccc1[7:0] : reg setting for signal11 selection with d2u mode									 
	SPI_WriteComm(0xCCc1); SPI_WriteData(0x00); //ccc2[7:0] : reg setting for signal12 selection with d2u mode									 
	SPI_WriteComm(0xCCc2); SPI_WriteData(0x00); //ccc3[7:0] : reg setting for signal13 selection with d2u mode									 
	SPI_WriteComm(0xCCc3); SPI_WriteData(0x00); //ccc4[7:0] : reg setting for signal14 selection with d2u mode									 
	SPI_WriteComm(0xCCc4); SPI_WriteData(0x00); //ccc5[7:0] : reg setting for signal15 selection with d2u mode									 
	SPI_WriteComm(0xCCc5); SPI_WriteData(0x00); //ccc6[7:0] : reg setting for signal16 selection with d2u mode									 
	SPI_WriteComm(0xCCc6); SPI_WriteData(0x00); //ccc7[7:0] : reg setting for signal17 selection with d2u mode									 
	SPI_WriteComm(0xCCc7); SPI_WriteData(0x00); //ccc8[7:0] : reg setting for signal18 selection with d2u mode									 
	SPI_WriteComm(0xCCc8); SPI_WriteData(0x00); //ccc9[7:0] : reg setting for signal19 selection with d2u mode									 
	SPI_WriteComm(0xCCc9); SPI_WriteData(0x00); //ccca[7:0] : reg setting for signal20 selection with d2u mode									 
	SPI_WriteComm(0xCCca); SPI_WriteData(0x00); //cccb[7:0] : reg setting for signal21 selection with d2u mode									 
	SPI_WriteComm(0xCCcb); SPI_WriteData(0x00); //cccc[7:0] : reg setting for signal22 selection with d2u mode									 
	SPI_WriteComm(0xCCcc); SPI_WriteData(0x00); //cccd[7:0] : reg setting for signal23 selection with d2u mode									 
	SPI_WriteComm(0xCCcd); SPI_WriteData(0x00); //ccce[7:0] : reg setting for signal24 selection with d2u mode									 
	SPI_WriteComm(0xCCce); SPI_WriteData(0x0E); //cccf[7:0] : reg setting for signal25 selection with d2u mode									 
																																		 
	// ccdx 																															 
	SPI_WriteComm(0xCCd0); SPI_WriteData(0x10); //ccd1[7:0] : reg setting for signal26 selection with d2u mode									 
	SPI_WriteComm(0xCCd1); SPI_WriteData(0x0A); //ccd2[7:0] : reg setting for signal27 selection with d2u mode									 
	SPI_WriteComm(0xCCd2); SPI_WriteData(0x0C); //ccd3[7:0] : reg setting for signal28 selection with d2u mode									 
	SPI_WriteComm(0xCCd3); SPI_WriteData(0x04); //ccd4[7:0] : reg setting for signal29 selection with d2u mode									 
	SPI_WriteComm(0xCCd4); SPI_WriteData(0x03); //ccd5[7:0] : reg setting for signal30 selection with d2u mode									 
	SPI_WriteComm(0xCCd5); SPI_WriteData(0x00); //ccd6[7:0] : reg setting for signal31 selection with d2u mode									 
	SPI_WriteComm(0xCCd6); SPI_WriteData(0x00); //ccd7[7:0] : reg setting for signal32 selection with d2u mode									 
	SPI_WriteComm(0xCCd7); SPI_WriteData(0x00); //ccd8[7:0] : reg setting for signal33 selection with d2u mode									 
	SPI_WriteComm(0xCCd8); SPI_WriteData(0x00); //ccd9[7:0] : reg setting for signal34 selection with d2u mode									 
	SPI_WriteComm(0xCCd9); SPI_WriteData(0x00); //ccda[7:0] : reg setting for signal35 selection with d2u mode									 
	SPI_WriteComm(0xCCda); SPI_WriteData(0x00); //ccdb[7:0] : reg setting for signal36 selection with d2u mode									 
	SPI_WriteComm(0xCCdb); SPI_WriteData(0x00); //ccdc[7:0] : reg setting for signal37 selection with d2u mode									 
	SPI_WriteComm(0xCCdc); SPI_WriteData(0x00); //ccdd[7:0] : reg setting for signal38 selection with d2u mode									 
	SPI_WriteComm(0xCCdd); SPI_WriteData(0x00); //ccde[7:0] : reg setting for signal39 selection with d2u mode					  
	SPI_WriteComm(0xCCde); SPI_WriteData(0x00); //ccdf[7:0] : reg setting for signal40 selection with d2u mode					  
	
	///=============================
	SPI_WriteComm(0x3A00); SPI_WriteData(0x77); //24bit	//MCU 16bits D[15:0]
	SPI_WriteComm(0x1100);
	mdelay(500);	
	SPI_WriteComm(0x2900);
	mdelay(200);
	SPI_WriteComm(0x2c00); 
//	printf("LCD_HX8369_Init end\n");
//	while(1);
	return 0;

}


static int hm8369_set_power(struct lcd_panel *panel, int on)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d ~ \n", __func__, on);


	if (on) {
		tcclcd_gpio_set_value(pdata->reset, 1);
#if defined(_M801_88_)
		lcd_delay_us(1000*10);
#else
		lcd_delay_us(1000);
#endif
		tcclcd_gpio_set_value(pdata->power_on, 1);
		lcd_delay_us(1000);
		tcclcd_gpio_set_value(pdata->reset, 0);
		lcd_delay_us(1000);
		tcclcd_gpio_set_value(pdata->reset, 1);
		
		tcclcd_gpio_set_value(pdata->display_on, 1);
		mdelay(10);

		lcdc_initialize(pdata->lcdc_num, panel);

		LCDC_IO_Set(pdata->lcdc_num, panel->bus_width);

		LCDC_IO_SetCurrent(pdata->lcdc_num, panel->bus_width);

		mdelay(16);
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


static int hm8369_set_backlight_level(struct lcd_panel *panel, int level)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d\n", __func__, level);

	if (level == 0) {
		tcclcd_gpio_set_value(pdata->bl_on, 0);
	} else {
		#ifdef TCC892X
		if(HW_REV == 0x1005 || HW_REV == 0x1006 || HW_REV == 0x1007|| HW_REV == 0x1008 || HW_REV == 0x2002 || HW_REV == 0x2003 || (HW_REV == 0x2004) || (HW_REV == 0x2005) || (HW_REV == 0x2006) || (HW_REV == 0x2007) || (HW_REV == 0x2008))
			tca_tco_pwm_ctrl(0, pdata->bl_on, MAX_BACKLIGTH, level);
		else
			tca_tco_pwm_ctrl(1, pdata->bl_on, MAX_BACKLIGTH, level);
		#else
			tca_tco_pwm_ctrl(0, pdata->bl_on, MAX_BACKLIGTH, level);		
		#endif//

	}

	return 0;
}


static struct lcd_panel hm8369_panel = {
	.name		= "HX8369",
	.manufacturer	= "INNOLUX",
	.id		= PANEL_ID_HX8369,//
    .xres        = 480,
    .yres        = 800,
	.width		= 154,
	.height		= 85,
	.bpp		= 24,
	.clk_freq	= 310000,
	.clk_div	= 2,
	.bus_width	= 24,
    .lpw        = 9,
    .lpc        = 479,
    .lswc        = 19,
    .lewc        = 39,
    .vdb        =0,
    .vdf        = 0,
    .fpw1        = 1,
    .flc1        = 799,
    .fswc1        = 3,
    .fewc1        = 3,
    .fpw2        = 0,
    .flc2        = 799,
    .fswc2        = 3,
    .fewc2        = 3,
	.sync_invert	= IV_INVERT | IH_INVERT,

		.init		= hm8369_panel_init,
		.set_power  = hm8369_set_power,
		.set_backlight_level = hm8369_set_backlight_level,
};

struct lcd_panel *tccfb_get_panel(void)
{
	return &hm8369_panel;
}
#endif//AT070TN93
