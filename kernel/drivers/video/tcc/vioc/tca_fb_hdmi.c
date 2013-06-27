
/****************************************************************************
 *   FileName    : tca_fb_hdmi.c
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
/*****************************************************************************
*
* Header Files Include
*
******************************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/clk.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/mach/map.h>
#include <asm/mach-types.h>
#ifdef CONFIG_PM
#include <linux/pm.h>
#endif
#include <linux/cpufreq.h>
#include <mach/bsp.h>
#include <mach/tca_ckc.h>

#include <mach/vioc_plugin_tcc892x.h>
#include <mach/tccfb_ioctrl.h>

#include <mach/tca_fb_hdmi.h>
#include <mach/tca_fb_output.h>

#include <mach/vioc_outcfg.h>
#include <mach/vioc_rdma.h>
#include <mach/vioc_wdma.h>
#include <mach/vioc_wmix.h>
#include <mach/vioc_disp.h>
#include <mach/vioc_global.h>
#include <mach/vioc_config.h>
#include <mach/vioc_scaler.h>
#include <mach/tccfb_address.h>

#define RDMA_UVI_MAX_WIDTH             3072
extern void tccxxx_GetAddress(unsigned char format, unsigned int base_Yaddr, unsigned int src_imgx, unsigned int  src_imgy,
									unsigned int start_x, unsigned int start_y, unsigned int* Y, unsigned int* U,unsigned int* V);

#if 0
#define dprintk(msg...)	 { printk( "tca_hdmi: " msg); }
#else
#define dprintk(msg...)	 
#endif

extern struct display_platform_data tcc_display_data;

void TCC_HDMI_LCDC_Timing(char hdmi_lcdc, struct lcdc_timimg_parms_t *mode)
{
	unsigned int width, height;
	VIOC_DISP *pDISP;
	VIOC_WMIX * pWMIX;	
	stLCDCTR pCtrlParam;
	stLTIMING HDMI_TIMEp;
	printk(" ##### %s  hdmi_lcdc=%d\n",__func__,hdmi_lcdc);

	if(hdmi_lcdc)	{
		pDISP = (VIOC_DISP *)tcc_p2v(HwVIOC_DISP1);
		pWMIX =(VIOC_WMIX *)tcc_p2v(HwVIOC_WMIX1); 
	}
	else		{
		pDISP = (VIOC_DISP *)tcc_p2v(HwVIOC_DISP0);
		pWMIX =(VIOC_WMIX *)tcc_p2v(HwVIOC_WMIX0); 
	}


	width = mode->lpc + 1;
	height = mode->flc + 1;
	HDMI_TIMEp.lpw= mode->lpw;
	HDMI_TIMEp.lpc= mode->lpc + 1;
	HDMI_TIMEp.lswc= mode->lswc+ 1;
	HDMI_TIMEp.lewc= mode->lewc+ 1;
	
	HDMI_TIMEp.vdb = mode->vdb;
	HDMI_TIMEp.vdf = mode->vdf;
	HDMI_TIMEp.fpw = mode->fpw;
	HDMI_TIMEp.flc = mode->flc;
	HDMI_TIMEp.fswc = mode->fswc;
	HDMI_TIMEp.fewc = mode->fewc;
	HDMI_TIMEp.fpw2 = mode->fpw2;
	HDMI_TIMEp.flc2 = mode->flc2;
	HDMI_TIMEp.fswc2 = mode->fswc2;
	HDMI_TIMEp.fewc2 = mode->fewc2;
	
	VIOC_DISP_SetTimingParam(pDISP, &HDMI_TIMEp);

	memset(&pCtrlParam, NULL, sizeof(pCtrlParam));
	pCtrlParam.id= mode->id;
	pCtrlParam.iv= mode->iv;
	pCtrlParam.ih= mode->ih;
	pCtrlParam.ip= mode->ip;
	pCtrlParam.clen = 0;


	pCtrlParam.r2y = 0;
	pCtrlParam.pxdw = 12; //RGB888
	pCtrlParam.dp = mode->dp;
	pCtrlParam.ni = mode->ni;
	if( mode->ni == 0)
		pCtrlParam.advi = 1;
	pCtrlParam.tv = mode->tv;
	pCtrlParam.opt = 0;
	pCtrlParam.stn = 0;
	pCtrlParam.evsel = 0;
	pCtrlParam.ovp = 0;
	
	VIOC_DISP_SetControlConfigure(pDISP, &pCtrlParam);

	VIOC_DISP_SetSize (pDISP, width, height);
	VIOC_DISP_SetBGColor(pDISP, 0, 0 , 0);

	#if defined(CONFIG_SOC_TCC8925S)
		#if defined(CONFIG_STB_BOARD_DONGLE)
		VIOC_WMIX_SetOverlayPriority(pWMIX, 18);
		#else
		VIOC_WMIX_SetOverlayPriority(pWMIX, 24);
		#endif /* CONFIG_STB_BOARD_DONGLE */
	#endif
	VIOC_WMIX_SetBGColor(pWMIX, 0x00, 0x00, 0x00, 0xff);
	VIOC_WMIX_SetSize(pWMIX, width, height);
	VIOC_WMIX_SetUpdate (pWMIX);
	
	VIOC_OUTCFG_SetOutConfig(VIOC_OUTCFG_HDMI, hdmi_lcdc);
	
}

void TCC_HDMI_LCDC_OutputEnable(char hdmi_lcdc, unsigned int onoff)
{

	VIOC_DISP *pDISP;

	if(hdmi_lcdc)	{
		pDISP = (VIOC_DISP *)tcc_p2v(HwVIOC_DISP1);
	}
	else		{
		pDISP = (VIOC_DISP *)tcc_p2v(HwVIOC_DISP0);
	}
	
	if(onoff)
		VIOC_DISP_TurnOn(pDISP);
	else
		VIOC_DISP_TurnOff(pDISP);
}

// 0 : 3 : layer enable/disable 

static unsigned int onthefly_using;
void TCC_HDMI_SET_OnTheFly(char hdmi_lcdc, struct tcc_lcdc_image_update *ImageInfo)
{
	
	VIOC_RDMA * pRDMABase;
	VIOC_SC *pSC;
	pSC = (VIOC_SC *)tcc_p2v(HwVIOC_SC1);
	
  #if defined(CONFIG_SOC_TCC8925S) || defined(CONFIG_ARCH_TCC893X)
  ImageInfo->Lcdc_layer = 3;
  #else
  ImageInfo->Lcdc_layer = 2;
  #endif
	if(hdmi_lcdc)
	{
		switch(ImageInfo->Lcdc_layer)
		{			
			case 0:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA04);
				break;
			case 1:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA05);
				break;
			case 2:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA06);
				break;
			case 3:
				pRDMABase = (VIOC_RDMA *)tcc_p2v(HwVIOC_RDMA07);
				break;
		}
	}
	else
	{
		switch(ImageInfo->Lcdc_layer)
		{			
			case 0:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA00);
				break;
			case 1:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA01);
				break;
			case 2:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA02);
				break;
			case 3:
				pRDMABase = (VIOC_RDMA *)tcc_p2v(HwVIOC_RDMA03);
				break;
		}		
	}
		
	if(ImageInfo->on_the_fly)
	{
		unsigned int RDMA_NUM;
		RDMA_NUM = hdmi_lcdc ? (ImageInfo->Lcdc_layer + VIOC_SC_RDMA_04) : ImageInfo->Lcdc_layer;

#if defined(CONFIG_MACH_TCC8920ST)
		if(!onthefly_using) {
			printk(" %s  scaler 1 is plug in RDMA %d \n",__func__, RDMA_NUM);
			BITSET(onthefly_using, 1 << ImageInfo->Lcdc_layer);
			VIOC_CONFIG_PlugIn (VIOC_SC1, RDMA_NUM);
		}

#else
		if(!onthefly_using) {
			dprintk(" %s  scaler 1 is plug in RDMA %d \n",__func__, RDMA_NUM);
			BITSET(onthefly_using, 1 << ImageInfo->Lcdc_layer);
			VIOC_CONFIG_PlugIn (VIOC_SC1, RDMA_NUM);
			VIOC_SC_SetBypass (pSC, OFF);
		}
#endif
	}
	else
	{
		if(ISSET(onthefly_using, 1<<ImageInfo->Lcdc_layer))
		{
			printk(" %s  scaler 1 is plug	\n",__func__);
			VIOC_RDMA_SetImageDisable(pRDMABase);
			VIOC_CONFIG_PlugOut(VIOC_SC1);
			BITCLR(onthefly_using, 1 << ImageInfo->Lcdc_layer);
		}
	}
}

#ifdef CONFIG_HDMI_DISPLAY_LASTFRAME
extern int onthefly_LastFrame;
#endif
#if 1//def CONFIG_HDMI_DISPLAY_LASTFRAME
extern unsigned int LastFrame;
extern TCC_OUTPUT_TYPE	Output_SelectMode;
#if defined(CONFIG_FB_TCC_COMPONENT)
extern void tcc_plugout_for_component(int ch_layer);
#endif
#if defined(CONFIG_FB_TCC_COMPOSITE)
extern void tcc_plugout_for_composite(int ch_layer);
#endif
#endif

void TCC_HDMI_DISPLAY_UPDATE(char hdmi_lcdc, struct tcc_lcdc_image_update *ImageInfo)
{
	VIOC_DISP * pDISPBase;
	VIOC_WMIX * pWMIXBase;
	VIOC_RDMA * pRDMABase;
#ifdef CONFIG_HDMI_DISPLAY_LASTFRAME
	VIOC_RDMA * pRDMABase_temp;
#endif		
	
	unsigned int lcd_width = 0, lcd_height = 0, lcd_h_pos = 0, lcd_w_pos = 0, scale_x = 0, scale_y = 0;
	
	VIOC_SC *pSC;
	pSC = (VIOC_SC *)tcc_p2v(HwVIOC_SC1);
		
	#if defined(CONFIG_SOC_TCC8925S)
	ImageInfo->Lcdc_layer = 3;
	#else		
	ImageInfo->Lcdc_layer = 2;
	#endif

	dprintk("%s enable:%d, layer:%d, fmt:%d, Fw:%d, Fh:%d, Iw:%d, Ih:%d, fmt:%d onthefly:%d\n", __func__, ImageInfo->enable, ImageInfo->Lcdc_layer,
			ImageInfo->fmt,ImageInfo->Frame_width, ImageInfo->Frame_height, ImageInfo->Image_width, ImageInfo->Image_height, ImageInfo->fmt, ImageInfo->on_the_fly);
	
	if((ImageInfo->Lcdc_layer >= 4) || (ImageInfo->fmt >TCC_LCDC_IMG_FMT_MAX))
		return;


	{
		pSC = (VIOC_SC *)tcc_p2v(HwVIOC_SC1);		

		if(hdmi_lcdc)
		{
			pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP1);
			pWMIXBase = (VIOC_WMIX*)tcc_p2v(HwVIOC_WMIX1);		

			switch(ImageInfo->Lcdc_layer)
			{			
				case 0:
					pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA04);
					break;
				case 1:
					pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA05);
					break;
				case 2:
					pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA06);
					break;
				case 3:
					pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA07);
					break;
			}
#ifdef CONFIG_HDMI_DISPLAY_LASTFRAME
			pRDMABase_temp  = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA05);
#endif		
		}
		else
		{
			pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP0);
			pWMIXBase = (VIOC_WMIX*)tcc_p2v(HwVIOC_WMIX0);
			
			switch(ImageInfo->Lcdc_layer)
			{			
				case 0:
					pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA00);
					break;
				case 1:
					pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA01);
					break;
				case 2:
					pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA02);
					break;
				case 3:
					pRDMABase = (VIOC_RDMA *)tcc_p2v(HwVIOC_RDMA03);
					break;					
			}		
#ifdef CONFIG_HDMI_DISPLAY_LASTFRAME
			pRDMABase_temp  = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA01);
#endif		
		}
	}

	VIOC_DISP_GetSize(pDISPBase, &lcd_width, &lcd_height);
		
	if((!lcd_width) || (!lcd_height))
		return;
	
	if(!ImageInfo->enable)	{
		volatile PVIOC_IREQ_CONFIG pIREQConfig;
		pIREQConfig = (volatile PVIOC_IREQ_CONFIG)tcc_p2v((unsigned int)HwVIOC_IREQ);

		VIOC_RDMA_SetImageDisable(pRDMABase);

		if( hdmi_lcdc ) {
			#if defined(CONFIG_SOC_TCC8925S)
			BITCSET(pIREQConfig->uSOFTRESET.nREG[0], (0x1<<VIOC_WMIX_RDMA_07), (0x1<<VIOC_WMIX_RDMA_07)); // rdma reset
			BITCSET(pIREQConfig->uSOFTRESET.nREG[0], (0x1<<VIOC_WMIX_RDMA_07), (0x0<<VIOC_WMIX_RDMA_07)); // rdma reset
			#else
		BITCSET(pIREQConfig->uSOFTRESET.nREG[0], (0x1<<VIOC_WMIX_RDMA_06), (0x1<<VIOC_WMIX_RDMA_06)); // rdma reset
		BITCSET(pIREQConfig->uSOFTRESET.nREG[0], (0x1<<VIOC_WMIX_RDMA_06), (0x0<<VIOC_WMIX_RDMA_06)); // rdma reset
			#endif
		}
		else {
			#if defined(CONFIG_SOC_TCC8925S)
			BITCSET(pIREQConfig->uSOFTRESET.nREG[0], (0x1<<VIOC_WMIX_RDMA_03), (0x1<<VIOC_WMIX_RDMA_03)); // rdma reset
			BITCSET(pIREQConfig->uSOFTRESET.nREG[0], (0x1<<VIOC_WMIX_RDMA_03), (0x0<<VIOC_WMIX_RDMA_03)); // rdma reset
			#else
			BITCSET(pIREQConfig->uSOFTRESET.nREG[0], (0x1<<VIOC_WMIX_RDMA_02), (0x1<<VIOC_WMIX_RDMA_02)); // rdma reset
			BITCSET(pIREQConfig->uSOFTRESET.nREG[0], (0x1<<VIOC_WMIX_RDMA_02), (0x0<<VIOC_WMIX_RDMA_02)); // rdma reset
			#endif	
		}
#ifdef CONFIG_HDMI_DISPLAY_LASTFRAME
		VIOC_RDMA_SetImageDisable(pRDMABase_temp);
#endif		
		if(ISSET(onthefly_using, 1<<ImageInfo->Lcdc_layer))
		{
			VIOC_CONFIG_PlugOut(VIOC_SC1);
			BITCLR(onthefly_using, 1 << ImageInfo->Lcdc_layer);

			BITCSET(pIREQConfig->uSOFTRESET.nREG[0], (0x1<<(28+VIOC_SC1)), (0x1<<(28+VIOC_SC1))); // scaler reset
			BITCSET(pIREQConfig->uSOFTRESET.nREG[0], (0x1<<(28+VIOC_SC1)), (0x0<<(28+VIOC_SC1))); // scaler reset
		}

#if 1//def CONFIG_HDMI_DISPLAY_LASTFRAME
		if(LastFrame == 1){
			LastFrame = 0;
			//printk("LastFrame = 0 in TCC_HDMI_DISPLAY_UPDATE \n");
		}
#endif
		return;
	}	

	dprintk("%s lcdc:%d, pRDMA:0x%08x, pWMIX:0x%08x, pDISP:0x%08x, addr0:0x%08x\n", __func__, hdmi_lcdc, pRDMABase, pWMIXBase, pDISPBase, ImageInfo->addr0);
	
	if(ImageInfo->on_the_fly)
	{
		unsigned int RDMA_NUM;

	#if defined(CONFIG_SOC_TCC8925S)
		if(hdmi_lcdc)		RDMA_NUM = VIOC_SC_RDMA_07;
		else				RDMA_NUM = VIOC_SC_RDMA_03;			
	#else		
		if(hdmi_lcdc)		RDMA_NUM = VIOC_SC_RDMA_06;
		else				RDMA_NUM = VIOC_SC_RDMA_02;			
	#endif

		VIOC_SC_SetSrcSize(pSC, ImageInfo->Frame_width, ImageInfo->Frame_height);
		VIOC_SC_SetDstSize (pSC, ImageInfo->Image_width, ImageInfo->Image_height);			// set destination size in scaler
		VIOC_SC_SetOutSize (pSC, ImageInfo->Image_width, ImageInfo->Image_height);			// set output size in scaer
		
#if defined(CONFIG_MACH_TCC8920ST)
		if(ImageInfo->Frame_width==ImageInfo->Image_width && ImageInfo->Frame_height==ImageInfo->Image_height) {
			VIOC_SC_SetBypass (pSC, ON);
			dprintk(" %s  scaler 1 is plug in SetBypass ON \n",__func__);
		}else {
			VIOC_SC_SetBypass (pSC, OFF);
			dprintk(" %s  scaler 1 is plug in SetBypass OFF \n",__func__);
		}

		if(!onthefly_using)
		{ 
			VIOC_RDMA_SetImageDisable(pRDMABase);
			dprintk(" %s  scaler 1 is plug in RDMA %d \n",__func__, RDMA_NUM);
			BITSET(onthefly_using, 1 << ImageInfo->Lcdc_layer);
			VIOC_CONFIG_PlugIn (VIOC_SC1, RDMA_NUM);	
		}
#else
		if(!onthefly_using) 
		{	
			VIOC_RDMA_SetImageDisable(pRDMABase);
			dprintk(" %s  scaler 1 is plug in RDMA %d \n",__func__, RDMA_NUM);
			BITSET(onthefly_using, 1 << ImageInfo->Lcdc_layer);
			VIOC_SC_SetBypass (pSC, OFF);
			VIOC_CONFIG_PlugIn (VIOC_SC1, RDMA_NUM);
		}
#endif
	}
	else
	{
		if(ISSET(onthefly_using, 1<<ImageInfo->Lcdc_layer))
		{
			dprintk(" %s  scaler 1 is plug out \n",__func__);
			VIOC_RDMA_SetImageDisable(pRDMABase);
			VIOC_CONFIG_PlugOut(VIOC_SC1);
			BITCLR(onthefly_using, 1 << ImageInfo->Lcdc_layer);
		}
	}
	
	if(onthefly_using)
		VIOC_SC_SetUpdate (pSC);
		
	// position
	VIOC_WMIX_SetPosition(pWMIXBase, ImageInfo->Lcdc_layer, ImageInfo->offset_x, ImageInfo->offset_y);
	VIOC_WMIX_SetUpdate(pWMIXBase);	

		
	if(ImageInfo->fmt >= TCC_LCDC_IMG_FMT_UYVY && ImageInfo->fmt <= TCC_LCDC_IMG_FMT_YUV422ITL1)
	{
		VIOC_RDMA_SetImageY2REnable(pRDMABase, TRUE);
		VIOC_RDMA_SetImageY2RMode(pRDMABase, 0); /* Y2RMode Default 0 (Studio Color) */
		if( ImageInfo->Frame_width <= RDMA_UVI_MAX_WIDTH )
			VIOC_RDMA_SetImageUVIEnable(pRDMABase, TRUE);
		else
			VIOC_RDMA_SetImageUVIEnable(pRDMABase, FALSE);
	}
	
	VIOC_RDMA_SetImageOffset(pRDMABase, ImageInfo->fmt, ImageInfo->Frame_width);
	VIOC_RDMA_SetImageFormat(pRDMABase, ImageInfo->fmt);

	// scale
	//VIOC_RDMA_SetImageScale(pRDMABase, scale_x, scale_y);

	if( !( ((ImageInfo->crop_left == 0) && (ImageInfo->crop_right == ImageInfo->Frame_width)) &&  ((ImageInfo->crop_top == 0) && (ImageInfo->crop_bottom == ImageInfo->Frame_height)))  )
	{
		dprintk(" Image Crop left=[%d], right=[%d], top=[%d], bottom=[%d] \n", ImageInfo->crop_left, ImageInfo->crop_right, ImageInfo->crop_top, ImageInfo->crop_bottom);
		int addr_Y = (unsigned int)ImageInfo->addr0;
		int addr_U = (unsigned int)ImageInfo->addr1;
		int addr_V = (unsigned int)ImageInfo->addr2;
		
		tccxxx_GetAddress(ImageInfo->fmt, (unsigned int)ImageInfo->addr0, ImageInfo->Frame_width, ImageInfo->Frame_height, 		
								ImageInfo->crop_left, ImageInfo->crop_top, &addr_Y, &addr_U, &addr_V);		

		VIOC_RDMA_SetImageSize(pRDMABase, ImageInfo->crop_right - ImageInfo->crop_left, ImageInfo->crop_bottom - ImageInfo->crop_top);
		VIOC_RDMA_SetImageBase(pRDMABase, addr_Y, addr_U, addr_V);
	}
	else
	{	
		dprintk(" don't Image Crop left=[%d], right=[%d], top=[%d], bottom=[%d] \n", ImageInfo->crop_left, ImageInfo->crop_right, ImageInfo->crop_top, ImageInfo->crop_bottom);
		VIOC_RDMA_SetImageSize(pRDMABase, ImageInfo->Frame_width, ImageInfo->Frame_height);
		VIOC_RDMA_SetImageBase(pRDMABase, ImageInfo->addr0, ImageInfo->addr1, ImageInfo->addr2);		
	}
		
	VIOC_RDMA_SetImageIntl(pRDMABase, 0);

	VIOC_WMIX_SetUpdate(pWMIXBase);	
	
	VIOC_RDMA_SetImageEnable(pRDMABase);

#ifdef CONFIG_HDMI_DISPLAY_LASTFRAME
	VIOC_RDMA_SetImageDisable(pRDMABase_temp);
	if(onthefly_LastFrame)
	{
		onthefly_LastFrame = 0;
		VIOC_CONFIG_PlugOut(VIOC_SC0);
	}
#endif

}

#ifdef CONFIG_HDMI_DISPLAY_LASTFRAME
void TCC_HDMI_LAST_FRAME_UPDATE(char hdmi_lcdc, struct tcc_lcdc_image_update *ImageInfo, int interlace_onthefly_mode)
{
	VIOC_DISP * pDISPBase;
	VIOC_WMIX * pWMIXBase;
	VIOC_RDMA * pRDMABase;
	VIOC_RDMA * pRDMABase_temp;
	
	unsigned int lcd_width = 0, lcd_height = 0;
	unsigned int old_lcdc_layer;

#if 0//!defined(CONFIG_TCC_OUTPUT_ATTACH)
	VIOC_SC *pSC;

	pSC = (VIOC_SC *)tcc_p2v(HwVIOC_SC0);
#endif

	if((ImageInfo->Lcdc_layer >= 3) || (ImageInfo->fmt >TCC_LCDC_IMG_FMT_MAX))
		return;

	if(hdmi_lcdc)
	{
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP1);
		pWMIXBase = (VIOC_WMIX*)tcc_p2v(HwVIOC_WMIX1);		

		pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA05);		
		pRDMABase_temp  = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA06);		
	}
	else
	{
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP0);
		pWMIXBase = (VIOC_WMIX*)tcc_p2v(HwVIOC_WMIX0);

		pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA01);
		pRDMABase_temp  = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA02);		
	}		
	
	ImageInfo->Lcdc_layer = 1;
	old_lcdc_layer = 2;

	VIOC_DISP_GetSize(pDISPBase, &lcd_width, &lcd_height);		
	if((!lcd_width) || (!lcd_height))
		return;

#if 0//!defined(CONFIG_TCC_OUTPUT_ATTACH)
	if(ImageInfo->on_the_fly)
	{
		onthefly_LastFrame = 1;	

		VIOC_SC_SetSrcSize(pSC, ImageInfo->Frame_width, ImageInfo->Frame_height);
		VIOC_SC_SetDstSize (pSC, ImageInfo->Image_width, ImageInfo->Image_height);			// set destination size in scaler
		VIOC_SC_SetOutSize (pSC, ImageInfo->Image_width, ImageInfo->Image_height);			// set output size in scaer
		
		VIOC_SC_SetBypass (pSC, OFF);
		VIOC_CONFIG_PlugIn(VIOC_SC0, VIOC_SC_RDMA_05);				
		VIOC_SC_SetUpdate (pSC);
	}
#endif

	// position
	VIOC_WMIX_SetPosition(pWMIXBase, ImageInfo->Lcdc_layer, ImageInfo->offset_x, ImageInfo->offset_y);

	if(machine_is_tcc8920st()) {
		VIOC_RDMA_SetImageUVIEnable(pRDMABase, TRUE);
	}
		
	if(ImageInfo->fmt >= TCC_LCDC_IMG_FMT_UYVY && ImageInfo->fmt <= TCC_LCDC_IMG_FMT_YUV422ITL1)
	{
		VIOC_RDMA_SetImageY2REnable(pRDMABase, TRUE);
		VIOC_RDMA_SetImageY2RMode(pRDMABase, 0); /* Y2RMode Default 0 (Studio Color) */
	}
	
	VIOC_RDMA_SetImageOffset(pRDMABase, ImageInfo->fmt, ImageInfo->Frame_width);
	VIOC_RDMA_SetImageFormat(pRDMABase, ImageInfo->fmt);

	// scale
	//VIOC_RDMA_SetImageScale(pRDMABase, scale_x, scale_y);

	VIOC_RDMA_SetImageSize(pRDMABase, ImageInfo->Frame_width, ImageInfo->Frame_height);
	VIOC_RDMA_SetImageBase(pRDMABase, ImageInfo->addr0, ImageInfo->addr1, ImageInfo->addr2);		
		
	VIOC_RDMA_SetImageIntl(pRDMABase, 0);
	VIOC_WMIX_SetUpdate(pWMIXBase);	
	
	VIOC_RDMA_SetImageEnable(pRDMABase);

	if(!interlace_onthefly_mode)
	{
		VIOC_RDMA_SetImageDisable(pRDMABase_temp);			

		if(Output_SelectMode == TCC_OUTPUT_HDMI){
			if(ISSET(onthefly_using, 1<<old_lcdc_layer))
			{
				VIOC_CONFIG_PlugOut(VIOC_SC1);
				BITCLR(onthefly_using, 1 << old_lcdc_layer);
			}
		}
		else if(Output_SelectMode== TCC_OUTPUT_COMPOSITE){
			#if defined(CONFIG_FB_TCC_COMPOSITE)
				tcc_plugout_for_composite(old_lcdc_layer);
			#endif
		}
		else if(Output_SelectMode == TCC_OUTPUT_COMPONENT){
			#if defined(CONFIG_FB_TCC_COMPONENT)
				tcc_plugout_for_component(old_lcdc_layer);
			#endif
		}
	}
}
#endif

#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
void TCC_HDMI_DISPLAY_PROCESS(char hdmi_lcdc, struct tcc_lcdc_image_update *ImageInfo, char force_update)
{
	unsigned int regl;
	unsigned int lcd_width = 0, lcd_height = 0, lcd_h_pos = 0, lcd_w_pos = 0, scale_x = 0, scale_y = 0;
	unsigned int img_width = 0, img_height = 0, output_width = 0, output_height = 0;
	unsigned int buffer_width = 0, y2r_option = 0, mixer_interrupt_use = 0;
	exclusive_ui_ar_params aspect_ratio;
	PLCDC pLCDC;
	PLCDC_CHANNEL pLCDC_channel;

	dprintk(" %s onoff:%d :fmt:%d Fw:%d Fh:%d Iw:%d Ih:%d \n", __func__,ImageInfo->enable,
		ImageInfo->fmt,ImageInfo->Frame_width, ImageInfo->Frame_height, ImageInfo->Image_width, ImageInfo->Image_height);

	if((ImageInfo->Lcdc_layer >= 3) || (ImageInfo->fmt >TCC_LCDC_IMG_FMT_MAX))
		return;

	if (hdmi_lcdc)	// lcdc1
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
	else
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);

	regl = pLCDC->LDS; // get LCD size 

	lcd_width = (regl & 0xFFFF);
	lcd_height = ((regl>>16) & 0xFFFF);
	
	if((!lcd_width) || (!lcd_height))
		return;

	output_width = lcd_width;
	output_height = lcd_height;

	switch(ImageInfo->Lcdc_layer)
	{
		case 0:
			pLCDC_channel = (volatile PLCDC_CHANNEL)&pLCDC->LI0C;
			break;
		case 1:
			pLCDC_channel = (volatile PLCDC_CHANNEL)&pLCDC->LI1C;
			break;
		case 2:
			pLCDC_channel = (volatile PLCDC_CHANNEL)&pLCDC->LI2C;
			break;
	}
	
	img_width = ImageInfo->Image_width;
	img_height= ImageInfo->Image_height;

	/* Aspect Ratio Conversion for DVD Contents */
	//if((img_width <= HDMI_EXCLUSIVE_UI_DVD_MAX_WD) && (img_height <= HDMI_EXCLUSIVE_UI_DVD_MAX_HT))
	{
		memset((void *)&aspect_ratio, 0x00, sizeof(exclusive_ui_ar_params));
		TCC_OUTPUT_EXCLUSIVE_UI_CfgAR(hdmi_lcdc, img_width, img_height, &aspect_ratio);
	}

	if(!ImageInfo->enable)	{
		#if defined(CONFIG_ARCH_TCC92XX)
		pLCDC_channel->LIC = 0;
		#else
		pLCDC_channel->LIC= HwLCT_RU;
		#endif//
		return;
	}

	/* Get the parameters for exclusive ui */
	TCC_OUTPUT_EXCLUSIVE_UI_GetParam(&hdmi_exclusive_ui_param);

	if(hdmi_exclusive_ui_param.enable)
	{
		/* Clear the on_the_flay for video */
		hdmi_exclusive_ui_onthefly = FALSE;

		/* Clear update flag and backup an information of the last video */
		TCC_OUTPUT_EXCLUSIVE_UI_SetImage(ImageInfo);

		/*-------------------------------------*/
		/*           INTERLACE VIDEO           */
		/* Update video data with exclusive ui */
		/*-------------------------------------*/
		if(hdmi_exclusive_ui_param.interlace)
		{
			/* Interlace BD Contents with 1920x1080 graphic plane */
			if(!TCC_OUTPUT_EXCLUSIVE_UI_ViqeMtoM(hdmi_lcdc, img_width, img_height))
			{
				if((img_width == lcd_width) && (img_height == lcd_height))
				{
					/* 1920x1080 HDMI Output */
				}
				else
				{
					/* 1280x720 HDMI Output */
				}
			}
			else
			{
				/* Disable the lcdc channel_1 */
				TCC_OUTPUT_EXCLUSIVE_UI_Direct(hdmi_lcdc, FALSE);

				/* Check the output format */
				if(ISZERO(pLCDC->LCTRL, HwLCTRL_NI))
					lcd_height = lcd_height >> 1;
			
				if(hdmi_exclusive_ui_param.clear == TRUE)
				{
					/* M2M Scaler without On_the_fly */
					if(TCC_OUTPUT_EXCLUSIVE_UI_Scaler_OnTheFly(ImageInfo, hdmi_lcdc) < 0)
						hdmi_exclusive_ui_onthefly = FALSE;
					else
						hdmi_exclusive_ui_onthefly = TRUE;

					/* Update Screen */
					TCC_HDMI_DISPLAY_UPDATE(hdmi_lcdc, ImageInfo);
				}
				else
				{
					/* VIQE and M2M Scaler with On_the_fly */
					TCC_OUTPUT_EXCLUSIVE_UI_Scaler(ImageInfo, HDMI_EXCLUSIVE_UI_SCALER1, hdmi_exclusive_ui_param.plane_width, hdmi_exclusive_ui_param.plane_height, hdmi_exclusive_ui_idx, 0);

					if(TCC_OUTPUT_EXCLUSIVE_UI_GetUpdate())
					{
						dprintk("E\n");
					}
					else
					{
						/* Overlay Mixer */
						TCC_OUTPUT_EXCLUSIVE_UI_Mixer(ImageInfo, hdmi_exclusive_ui_idx, 0);
						/* Update overlay mixer image */
						TCC_OUTPUT_EXCLUSIVE_UI_Restore_Mixer(ImageInfo, hdmi_lcdc, output_width, output_height, hdmi_exclusive_ui_idx);
						/* Set the interrupt flag */
						mixer_interrupt_use = 1;
	 				}
				}
			}
		}
		else
		/*-------------------------------------*/
		/*          PROGRESSIVE VIDEO          */
		/* Update video data with exclusive ui */
		/*-------------------------------------*/
		{
			/* 1920x1080 Progressive BD Contents */
			if( (img_width == hdmi_exclusive_ui_param.plane_width) && (img_width == lcd_width) && (img_width == TCC_FB_OUT_MAX_WIDTH) &&
				(img_height == hdmi_exclusive_ui_param.plane_height) && (img_height == lcd_height) && (img_height == TCC_FB_OUT_MAX_HEIGHT) )
			{
				/* 1920x1080 HDMI Output */
			}
			else
			{
				/* Disable the lcdc channel_1 */
				TCC_OUTPUT_EXCLUSIVE_UI_Direct(hdmi_lcdc, FALSE);

				if(hdmi_exclusive_ui_param.clear == FALSE)
				{
					/* M2M Scaler without On_the_fly */
					TCC_OUTPUT_EXCLUSIVE_UI_Scaler(ImageInfo, HDMI_EXCLUSIVE_UI_SCALER0, hdmi_exclusive_ui_param.plane_width, hdmi_exclusive_ui_param.plane_height, hdmi_exclusive_ui_idx, 0);
					/* Overlay Mixer */
					TCC_OUTPUT_EXCLUSIVE_UI_Mixer(ImageInfo, hdmi_exclusive_ui_idx, 0);
				}

			#if defined(CONFIG_TCC_VIDEO_DISPLAY_BY_VSYNC_INT)
				/* M2M Scaler without On_the_fly */
				TCC_OUTPUT_EXCLUSIVE_UI_Scaler(ImageInfo, HDMI_EXCLUSIVE_UI_SCALER0, output_width, output_height, hdmi_exclusive_ui_idx, 1);
				//hdmi_exclusive_ui_onthefly = FALSE;
			#else
				/* M2M Scaler with On_the_fly */
				if(TCC_OUTPUT_EXCLUSIVE_UI_Scaler_OnTheFly(ImageInfo, hdmi_lcdc) < 0)
					hdmi_exclusive_ui_onthefly = FALSE;
				else
					hdmi_exclusive_ui_onthefly = TRUE;
			#endif
			}

			#if 0 //shkim - on_the_fly
			/* Set the on_the_flay for android UI */
			if(hdmi_exclusive_ui_onthefly == FALSE)
				TCC_OUTPUT_EXCLUSIVE_UI_SetOnthefly(hdmi_lcdc, 1, TRUE);
			else
				TCC_OUTPUT_EXCLUSIVE_UI_SetOnthefly(hdmi_lcdc, 1, FALSE);
			#endif
		}

		hdmi_exclusive_ui_idx = !hdmi_exclusive_ui_idx;
	}

	if(force_update && !mixer_interrupt_use)
	{
		TCC_HDMI_DISPLAY_UPDATE(hdmi_lcdc, ImageInfo);
	}
}

void TCC_HDMI_DISPLAY_IMAGE(exclusive_ui_update UpdateImage)
{
	/* M2M Scaler with On_the_fly */
	if(TCC_OUTPUT_EXCLUSIVE_UI_Scaler_OnTheFly(&UpdateImage.image, UpdateImage.lcdc) < 0)
		hdmi_exclusive_ui_onthefly = FALSE;
	else
		hdmi_exclusive_ui_onthefly = TRUE;

	dprintk("%s, hdmi_exclusive_ui_onthefly=%d\n", __func__, hdmi_exclusive_ui_onthefly);
			
	/* Update Screen */
	TCC_HDMI_DISPLAY_UPDATE(UpdateImage.lcdc, &UpdateImage.image);
}
#endif

