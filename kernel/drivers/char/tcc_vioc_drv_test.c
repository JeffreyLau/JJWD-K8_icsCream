/*
 * drivers/char/Tcc83xx_scaler.c
 *
 * Copyright (C) 2004 Texas Instruments, Inc. 
 *
 * Video-for-Linux (Version 2) graphic capture driver for
 * the OMAP H2 and H3 camera controller.
 *
 * Adapted from omap24xx driver written by Andy Lowe (source@mvista.com)
 * Copyright (C) 2003-2004 MontaVista Software, Inc.
 * 
 * This package is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
 * 
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED 
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. 
 *
 * History:
 *   27/03/05   Vladimir Barinov - Added support for power management
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/mach/map.h>
#include <mach/bsp.h>
#if defined(CONFIG_ARCH_TCC892X)
#include <mach/irqs.h>
#include <mach/vioc_ireq.h>
#include <mach/vioc_rdma.h>
#include <mach/vioc_wdma.h>
#include <mach/vioc_wmix.h>
#include <mach/vioc_config.h>
#include <mach/vioc_plugin_tcc892x.h>
#include <mach/vioc_scaler.h>
#include <mach/vioc_wmix.h>
#include <mach/vioc_api.h>
#endif // CONFIG_ARCH_TCC892X
#include <linux/poll.h>
#include "tcc_vioc_drv_test.h"
#include <mach/tcc_vioc_drv_ioctrl.h>


#define VIOC_DRV_DEBUG 			0
#define dprintk(msg...) 		if(VIOC_DRV_DEBUG) { 	printk("VIOC_DRV:  " msg); 	}

volatile PVIOC_RDMA 	pVIOC_RDMA1_Base;
volatile PVIOC_RDMA 	pVIOC_RDMA2_Base;
volatile PVIOC_SC 		pVIOC_SCALER_Base;
volatile PVIOC_WMIX 	pVIOC_WMIX_Base;
volatile PVIOC_WDMA 	pVIOC_WDMA_Base;

typedef struct _INTERRUPT_DATA_T {
	// wait for poll  
	wait_queue_head_t 	poll_wq;
	spinlock_t 			poll_lock;
	unsigned int 			poll_count;

	// wait for ioctl command
	wait_queue_head_t 	cmd_wq;
	spinlock_t 			cmd_lock;
	unsigned int 			cmd_count;

	struct mutex 			io_mutex;
	unsigned char 		block_operating;
	unsigned char 		block_waiting;
	unsigned char 		irq_reged;
	unsigned int 			dev_opened;
} INTERRUPT_DATA_T;
static INTERRUPT_DATA_T vioc_drv_data;

VIOC_DRV_START_INFO_Type vioc_drv_start_info;

#define DEVICE_NAME 			"vioc_drv_test"
#define MAJOR_ID 				220
#define MINOR_ID 				1

static struct clk *vioc_drv_clk;

extern void tccxxx_GetAddress(unsigned char format, unsigned int base_Yaddr, unsigned int src_imgx, unsigned int  src_imgy,
								unsigned int start_x, unsigned int start_y, unsigned int* Y, unsigned int* U, unsigned int* V);
extern int range_is_allowed(unsigned long pfn, unsigned long size);

static int tccxxx_vioc_drv_mmap(struct file *file, struct vm_area_struct *vma)
{
	if(range_is_allowed(vma->vm_pgoff, vma->vm_end - vma->vm_start) < 0) {
		printk(KERN_ERR	"vioc_drv:  this address is not allowed(1). \n");
		return -EAGAIN;
	}

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	if(remap_pfn_range(vma,vma->vm_start, vma->vm_pgoff , vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
		printk(KERN_ERR	"vioc_drv:  this address is not allowed(2). \n");
		return -EAGAIN;
	}

	vma->vm_ops 	=  	NULL;
	vma->vm_flags 	|= 	VM_IO;
	vma->vm_flags 	|= 	VM_RESERVED;

	return 0;
}

static irqreturn_t tccxxx_vioc_drv_handler(int irq, void *client_data)
{  	
	INTERRUPT_DATA_T *vioc_drv_int_data = client_data;

	if(pVIOC_WDMA_Base->uIRQSTS.nREG & VIOC_WDMA_IREQ_EOFR_MASK) {
		dprintk("WDMA Interrupt is VIOC_WDMA_IREQ_EOFR_MASK. \n");
		pVIOC_WDMA_Base->uIRQSTS.nREG = 0xFFFFFFFF;   // wdma status register all clear.
	}

	if(vioc_drv_int_data->block_operating >= 1) {
		vioc_drv_int_data->block_operating = 0;
	}
		
	wake_up_interruptible(&vioc_drv_int_data->poll_wq);

	if(vioc_drv_int_data->block_waiting) {
		wake_up_interruptible(&vioc_drv_int_data->cmd_wq);
	}

	return IRQ_HANDLED;
}

char tccxxx_vioc_drv_initialize(void)
{
	int ret = 0;
	volatile PVIOC_IREQ_CONFIG pVIOCIregConfig = (volatile PVIOC_IREQ_CONFIG)tcc_p2v((unsigned int)HwVIOC_IREQ);

	// set to RDMA1
	if(vioc_drv_start_info.rdma1 < 0xFF)
		pVIOC_RDMA1_Base = (volatile PVIOC_RDMA)tcc_p2v((unsigned int)BASE_ADDR_VIOC + 0x400 + (0x100 * vioc_drv_start_info.rdma1));
	else
		pVIOC_RDMA1_Base = NULL;

	// set to RDMA2
	if(vioc_drv_start_info.rdma2 < 0xFF)
		pVIOC_RDMA2_Base = (volatile PVIOC_RDMA)tcc_p2v((unsigned int)BASE_ADDR_VIOC + 0x400 + (0x100 * vioc_drv_start_info.rdma2));
	else
		pVIOC_RDMA2_Base = NULL;

	// set to SC
	if(vioc_drv_start_info.scaler < 0xFF)
		pVIOC_SCALER_Base = (volatile PVIOC_RDMA)tcc_p2v((unsigned int)BASE_ADDR_VIOC + 0x2000 + (0x100 * vioc_drv_start_info.scaler));
	else
		pVIOC_SCALER_Base = NULL;

	// set to WMIX
	if(vioc_drv_start_info.wmixer < 0xFF)
		pVIOC_WMIX_Base = (volatile PVIOC_WMIX)tcc_p2v((unsigned int)BASE_ADDR_VIOC + 0x1800 + (0x100 * vioc_drv_start_info.wmixer));
	else
		pVIOC_WMIX_Base = NULL;

	// set to WDMA
	if(vioc_drv_start_info.wdma < 0xFF)
		pVIOC_WDMA_Base = (volatile PVIOC_WDMA)tcc_p2v((unsigned int)BASE_ADDR_VIOC + 0x2800 + (0x100 * vioc_drv_start_info.wdma));
	else
		pVIOC_WDMA_Base = NULL;

	// reset to vioc component
	if(pVIOC_WDMA_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[1], (0x1<<vioc_drv_start_info.wdma), (0x1<<vioc_drv_start_info.wdma)); 			// wdma reset

	if(pVIOC_WMIX_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[1], (0x1<<(9+vioc_drv_start_info.wmixer)), (0x1<<(9+vioc_drv_start_info.wmixer))); 	// wmix reset

	if(pVIOC_SCALER_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[0], (0x1<<(28+vioc_drv_start_info.scaler)), (0x1<<(28+vioc_drv_start_info.scaler))); 	// scaler reset

	if(pVIOC_RDMA1_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[0], (0x1<<vioc_drv_start_info.rdma1), (0x1<<vioc_drv_start_info.rdma1)); 			// rdma1 reset

	if(pVIOC_RDMA2_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[0], (0x1<<vioc_drv_start_info.rdma2), (0x1<<vioc_drv_start_info.rdma2)); 			// rdma2 reset

	if(pVIOC_RDMA2_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[0], (0x1<<vioc_drv_start_info.rdma2), (0x0<<vioc_drv_start_info.rdma2)); 			// rdma2 reset

	if(pVIOC_RDMA1_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[0], (0x1<<vioc_drv_start_info.rdma1), (0x0<<vioc_drv_start_info.rdma1)); 			// rdma reset

	if(pVIOC_SCALER_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[0], (0x1<<(28+vioc_drv_start_info.scaler)), (0x0<<(28+vioc_drv_start_info.scaler))); 	// scaler reset

	if(pVIOC_WMIX_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[1], (0x1<<(9+vioc_drv_start_info.wmixer)), (0x0<<(9+vioc_drv_start_info.wmixer))); 	// wmix reset

	if(pVIOC_WDMA_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[1], (0x1<<vioc_drv_start_info.wdma), (0x0<<vioc_drv_start_info.wdma)); 			// wdma reset

	VIOC_WDMA_SetIreqMask(pVIOC_WDMA_Base, VIOC_WDMA_IREQ_EOFR_MASK, 0x0);
	ret = request_irq((INT_VIOC_WD0 + vioc_drv_start_info.wdma), tccxxx_vioc_drv_handler, IRQF_SHARED, "vioc_drv", &vioc_drv_data);

	return ret;
}

char tccxxx_vioc_drv_finalize(void)
{
	int ret = 0;
	volatile PVIOC_IREQ_CONFIG pVIOCIregConfig = (volatile PVIOC_IREQ_CONFIG)tcc_p2v((unsigned int)HwVIOC_IREQ);

	if(pVIOC_RDMA1_Base != NULL)
		pVIOC_RDMA1_Base = NULL;

	if(pVIOC_RDMA2_Base != NULL)
		pVIOC_RDMA2_Base = NULL;

	if(pVIOC_SCALER_Base != NULL)
		pVIOC_SCALER_Base = NULL;

	if(pVIOC_WMIX_Base != NULL)
		pVIOC_WMIX_Base = NULL;

	if(pVIOC_WDMA_Base != NULL)
		pVIOC_WDMA_Base = NULL;

	if(pVIOC_WDMA_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[1], (0x1<<vioc_drv_start_info.wdma), (0x1<<vioc_drv_start_info.wdma)); 			// wdma reset

	if(pVIOC_WMIX_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[1], (0x1<<(9+vioc_drv_start_info.wmixer)), (0x1<<(9+vioc_drv_start_info.wmixer))); 	// wmix reset

	if(pVIOC_SCALER_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[0], (0x1<<(28+vioc_drv_start_info.scaler)), (0x1<<(28+vioc_drv_start_info.scaler))); 	// scaler reset

	if(pVIOC_RDMA1_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[0], (0x1<<vioc_drv_start_info.rdma1), (0x1<<vioc_drv_start_info.rdma1)); 			// rdma1 reset

	if(pVIOC_RDMA2_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[0], (0x1<<vioc_drv_start_info.rdma2), (0x1<<vioc_drv_start_info.rdma2)); 			// rdma2 reset

	if(pVIOC_RDMA2_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[0], (0x1<<vioc_drv_start_info.rdma2), (0x0<<vioc_drv_start_info.rdma2)); 			// rdma2 reset

	if(pVIOC_RDMA1_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[0], (0x1<<vioc_drv_start_info.rdma1), (0x0<<vioc_drv_start_info.rdma1)); 			// rdma reset

	if(pVIOC_SCALER_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[0], (0x1<<(28+vioc_drv_start_info.scaler)), (0x0<<(28+vioc_drv_start_info.scaler))); 	// scaler reset

	if(pVIOC_WMIX_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[1], (0x1<<(9+vioc_drv_start_info.wmixer)), (0x0<<(9+vioc_drv_start_info.wmixer))); 	// wmix reset

	if(pVIOC_WDMA_Base != NULL)
		BITCSET(pVIOCIregConfig->uSOFTRESET.nREG[1], (0x1<<vioc_drv_start_info.wdma), (0x0<<vioc_drv_start_info.wdma)); 			// wdma reset

	free_irq((INT_VIOC_WD0 + vioc_drv_start_info.wdma), &vioc_drv_data);

	return ret;
}

char tccxxx_vioc_drv_scaling(VIOC_DRV_SCALING_INFO_Type *scale_info)
{
	int ret = 0;
	VIOC_SCALER_INFO_Type pScalerInfo;
	unsigned int pSrcBase0 = 0, pSrcBase1 = 0, pSrcBase2 = 0;
	unsigned int pDstBase0 = 0, pDstBase1 = 0, pDstBase2 = 0;
	unsigned int crop_width;

	dprintk("%s \n", __func__);
	dprintk("Src:  addr:0x%x, 0x%x, 0x%x,  fmt:%d. \n", scale_info->src_y_addr, scale_info->src_u_addr, scale_info->src_v_addr, scale_info->src_fmt);
	dprintk("Dst:  addr:0x%x, 0x%x, 0x%x,  fmt:%d. \n", scale_info->dst_y_addr, scale_info->dst_u_addr, scale_info->dst_v_addr, scale_info->dst_fmt);
	dprintk("Size : src_w:%d, src_h:%d, dst_w:%d, dst_h:%d. \n", scale_info->src_img_width, scale_info->src_img_height, 		\
														   scale_info->dst_img_width, scale_info->dst_img_height);

	pSrcBase0 = (unsigned int)scale_info->src_y_addr;
	pSrcBase1 = (unsigned int)scale_info->src_u_addr;
	pSrcBase2 = (unsigned int)scale_info->src_v_addr;
	if((scale_info->src_fmt > SC_IMG_FMT_ARGB6666) && (pSrcBase1 == NULL || pSrcBase2 == NULL)) { // address limitation!!
		dprintk("%s():  src addr is not allocate. \n");
		crop_width 				= scale_info->src_win_right - scale_info->src_win_left;
		scale_info->src_win_left 	= (scale_info->src_win_left>>3)<<3;
		scale_info->src_win_right 	= scale_info->src_win_left + crop_width;
		tccxxx_GetAddress(scale_info->src_fmt, (unsigned int)scale_info->src_y_addr, 			\
									scale_info->src_img_width, scale_info->src_img_height, 	\
									scale_info->src_win_left, scale_info->src_win_top, 		\
									&pSrcBase0, &pSrcBase1, &pSrcBase2);
	}

	pDstBase0 = (unsigned int)scale_info->dst_y_addr;
	pDstBase1 = (unsigned int)scale_info->dst_u_addr;
	pDstBase2 = (unsigned int)scale_info->dst_v_addr;
	if((scale_info->dst_fmt > SC_IMG_FMT_ARGB6666) && (pDstBase1 == NULL || pDstBase2 == NULL)) { // address limitation!!
		dprintk("%s():  dst addr is not allocate. \n");
		crop_width 				= scale_info->dst_win_right - scale_info->dst_win_left;
		scale_info->dst_win_left  	= (scale_info->dst_win_left>>3)<<3;
		scale_info->dst_win_right 	= scale_info->dst_win_left + crop_width;
		tccxxx_GetAddress(scale_info->dst_fmt, (unsigned int)scale_info->dst_y_addr, 			\
									scale_info->dst_img_width, scale_info->dst_img_height, 	\
									scale_info->dst_win_left, scale_info->dst_win_top, 		\
									&pDstBase0, &pDstBase1, &pDstBase2);
	}

	spin_lock_irq(&vioc_drv_data.cmd_lock);

	// set to VRDMA switch path
	if(vioc_drv_start_info.rdma1 == VIOC_SC_RDMA_12) {
		VIOC_CONFIG_RDMA12PathCtrl(0 /* RDMA12 */);
	}

	// set to VRDMA
	#if defined(CONFIG_MACH_TCC8920ST)
		VIOC_RDMA_SetImageAlphaSelect(pVIOC_RDMA1_Base, 1);
		VIOC_RDMA_SetImageAlphaEnable(pVIOC_RDMA1_Base, 1);
	#else
		VIOC_RDMA_SetImageAlphaEnable(pVIOC_RDMA1_Base, 1);
	#endif

	VIOC_RDMA_SetImageFormat(pVIOC_RDMA1_Base, scale_info->src_fmt);
	VIOC_RDMA_SetImageSize(pVIOC_RDMA1_Base, (scale_info->src_win_right - scale_info->src_win_left), 		\
								(scale_info->src_win_bottom - scale_info->src_win_top));
	VIOC_RDMA_SetImageOffset(pVIOC_RDMA1_Base, scale_info->src_fmt, scale_info->src_img_width);
	VIOC_RDMA_SetImageBase(pVIOC_RDMA1_Base, pSrcBase0, pSrcBase1, pSrcBase2);

	// set to Scaler
	pScalerInfo.BYPASS 			= FALSE /* 0 */;
	pScalerInfo.DST_WIDTH 		= (scale_info->dst_win_right - scale_info->dst_win_left);
	pScalerInfo.DST_HEIGHT 		= (scale_info->dst_win_bottom - scale_info->dst_win_top);
	pScalerInfo.OUTPUT_POS_X 		= 0;
	pScalerInfo.OUTPUT_POS_Y 		= 0;
	pScalerInfo.OUTPUT_WIDTH 		= pScalerInfo.DST_WIDTH;
	pScalerInfo.OUTPUT_HEIGHT 	= pScalerInfo.DST_HEIGHT;
	
	VIOC_API_SCALER_SetConfig(vioc_drv_start_info.scaler, &pScalerInfo);
	VIOC_API_SCALER_SetPlugIn(vioc_drv_start_info.scaler, vioc_drv_start_info.rdma1);
	VIOC_API_SCALER_SetUpdate(vioc_drv_start_info.scaler);
	VIOC_RDMA_SetImageEnable(pVIOC_RDMA1_Base); // SoC guide info.

	// set to WMIX
	VIOC_CONFIG_WMIXPath(vioc_drv_start_info.wmixer, 0 /* bypass path */);
	VIOC_WMIX_SetSize(pVIOC_WMIX_Base, pScalerInfo.DST_WIDTH, pScalerInfo.DST_HEIGHT);	
	VIOC_WMIX_SetUpdate(pVIOC_WMIX_Base);

	// set to VWRMA
	VIOC_WDMA_SetImageFormat(pVIOC_WDMA_Base, scale_info->dst_fmt);
	VIOC_WDMA_SetImageSize(pVIOC_WDMA_Base, pScalerInfo.DST_WIDTH, pScalerInfo.DST_HEIGHT);
	VIOC_WDMA_SetImageOffset(pVIOC_WDMA_Base, scale_info->dst_fmt, scale_info->dst_img_width);
	VIOC_WDMA_SetImageBase(pVIOC_WDMA_Base, pDstBase0, pDstBase1, pDstBase2);
	if((scale_info->src_fmt < SC_IMG_FMT_FCDEC) && (scale_info->dst_fmt > SC_IMG_FMT_FCDEC)) {
		VIOC_WDMA_SetImageR2YEnable(pVIOC_WDMA_Base, 1);
	}
	VIOC_WDMA_SetImageEnable(pVIOC_WDMA_Base, 0 /* OFF */);
	pVIOC_WDMA_Base->uIRQSTS.nREG = 0xFFFFFFFF; // wdma status register all clear.

	spin_unlock_irq(&vioc_drv_data.cmd_lock);

	if(scale_info->rsp_type == VIOC_DRV_POLLING) {
		ret = wait_event_interruptible_timeout(vioc_drv_data.poll_wq, vioc_drv_data.block_operating == 0, msecs_to_jiffies(500));
		if(ret <= 0) {
			 vioc_drv_data.block_operating = 0;
			printk("vioc_drv time out: %d, Line: %d. \n", ret, __LINE__);
		}
	}

	return ret;
}
EXPORT_SYMBOL(tccxxx_vioc_drv_scaling);

static unsigned int tccxxx_vioc_drv_poll(struct file *filp, poll_table *wait)
{
	int ret = 0;
	INTERRUPT_DATA_T *vioc_drv_int_data = (INTERRUPT_DATA_T *)filp->private_data;

	if(vioc_drv_int_data == NULL) {
		return -EFAULT;
	}

	poll_wait(filp, &vioc_drv_int_data->poll_wq, wait);
	spin_lock_irq(&vioc_drv_int_data->poll_lock);

	if(vioc_drv_int_data->block_operating == 0) {
		ret = POLLIN|POLLRDNORM;
	}

	spin_unlock_irq(&vioc_drv_int_data->poll_lock);

	return ret;
}

long tccxxx_vioc_drv_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	VIOC_DRV_SCALING_INFO_Type vioc_drv_scaling;
	INTERRUPT_DATA_T *vioc_drv_int_data = (INTERRUPT_DATA_T *)file->private_data;

	dprintk("vioc_drv:  cmd(0x%x), block_operating(0x%x), block_waiting(0x%x), cmd_count(0x%x), poll_count(0x%x). \n", 			\
			cmd, vioc_drv_int_data->block_operating, vioc_drv_int_data->block_waiting, vioc_drv_int_data->cmd_count, vioc_drv_int_data->poll_count);
	
	switch(cmd) {
		case TCC_VIOC_DRV_IOCTRL_START:
			mutex_lock(&vioc_drv_int_data->io_mutex);

			if(copy_from_user(&vioc_drv_start_info, (VIOC_DRV_START_INFO_Type *)arg, sizeof(VIOC_DRV_START_INFO_Type))) {
				printk(KERN_ALERT "Not Supported copy_from_user(%d). \n", cmd);
				ret = -EFAULT;
			}

			ret = tccxxx_vioc_drv_initialize();

			mutex_unlock(&vioc_drv_int_data->io_mutex);
			return ret;

		case TCC_VIOC_DRV_IOCTRL_END:
			mutex_lock(&vioc_drv_int_data->io_mutex);

			ret = tccxxx_vioc_drv_finalize();

			mutex_unlock(&vioc_drv_int_data->io_mutex);
			return ret;

		case TCC_VIOC_DRV_IOCTRL_SCALING:
			mutex_lock(&vioc_drv_int_data->io_mutex);

			if(vioc_drv_int_data->block_operating) {
				vioc_drv_int_data->block_waiting = 1;
				ret = wait_event_interruptible_timeout(vioc_drv_int_data->cmd_wq, vioc_drv_int_data->block_operating == 0, msecs_to_jiffies(200));
				if(ret <= 0) {
					vioc_drv_int_data->block_operating = 0;
					printk("[%d]: vioc_drv 0 timed_out block_operation:%d!! cmd_count:%d \n", ret, vioc_drv_int_data->block_waiting, vioc_drv_int_data->cmd_count);
				}
				ret = 0;
			}

			if(copy_from_user(&vioc_drv_scaling, (VIOC_DRV_SCALING_INFO_Type *)arg, sizeof(VIOC_DRV_SCALING_INFO_Type))) {
				printk(KERN_ALERT "Not Supported copy_from_user(%d). \n", cmd);
				ret = -EFAULT;
			}
			
			if(ret >= 0) {
				if(vioc_drv_int_data->block_operating >= 1) {
					printk("vioc_drv:  block_operating(%d) - block_waiting(%d) - cmd_count(%d) - poll_count(%d)!!!\n", 		\
								vioc_drv_int_data->block_operating, vioc_drv_int_data->block_waiting, vioc_drv_int_data->cmd_count, vioc_drv_int_data->poll_count);
				}

				vioc_drv_int_data->block_waiting = 0;
				vioc_drv_int_data->block_operating = 1;
				ret = tccxxx_vioc_drv_scaling(&vioc_drv_scaling);
				if(ret < 0) 	vioc_drv_int_data->block_operating = 0;
			}

			mutex_unlock(&vioc_drv_int_data->io_mutex);
			return ret;

		default:
			printk(KERN_ALERT "not supported VIOC DRV IOCTL(0x%x). \n", cmd);
			break;			
	}

	return 0;
}
EXPORT_SYMBOL(tccxxx_vioc_drv_ioctl);

int tccxxx_vioc_drv_release(struct inode *inode, struct file *filp)
{
	int ret = 0;
	dprintk("vioc_drv_release IN:  %d'th, block(%d/%d), cmd(%d), irq(%d). \n", vioc_drv_data.dev_opened, 		\
			vioc_drv_data.block_operating, vioc_drv_data.block_waiting, vioc_drv_data.cmd_count, vioc_drv_data.irq_reged);

	if(vioc_drv_data.dev_opened > 0) {
		vioc_drv_data.dev_opened--;
	}

	if(vioc_drv_data.dev_opened == 0) {
		if(vioc_drv_data.block_operating) {
			ret = wait_event_interruptible_timeout(vioc_drv_data.cmd_wq, vioc_drv_data.block_operating == 0, msecs_to_jiffies(200));
		}

		if(ret <= 0) {
 			printk("[%d]:  vioc_drv timed_out block_operation: %d, cmd_count: %d. \n", ret, vioc_drv_data.block_waiting, vioc_drv_data.cmd_count);
		}

		if(vioc_drv_data.irq_reged) {
			vioc_drv_data.irq_reged = 0;
		}

		vioc_drv_data.block_operating = vioc_drv_data.block_waiting = 0;
		vioc_drv_data.poll_count = vioc_drv_data.cmd_count = 0;
	}

	clk_disable(vioc_drv_clk);
	dprintk("vioc_drv_release OUT:  %d'th. \n", vioc_drv_data.dev_opened);

	return 0;
}
EXPORT_SYMBOL(tccxxx_vioc_drv_release);

int tccxxx_vioc_drv_open(struct inode *inode, struct file *filp)
{	
	int ret = 0;
	dprintk("vioc_drv_open IN:  %d'th, block(%d/%d), cmd(%d), irq(%d). \n", vioc_drv_data.dev_opened, 		\
			vioc_drv_data.block_operating, vioc_drv_data.block_waiting, vioc_drv_data.cmd_count, vioc_drv_data.irq_reged);

	clk_enable(vioc_drv_clk);

	if(!vioc_drv_data.irq_reged) {
		vioc_drv_data.irq_reged = 1;
	}

	vioc_drv_data.dev_opened++;
	filp->private_data = &vioc_drv_data;
	dprintk("vioc_drv_open OUT:  %d'th. \n", vioc_drv_data.dev_opened);

	return ret;
}
EXPORT_SYMBOL(tccxxx_vioc_drv_open);


static struct file_operations tcc_vioc_drv_fops = {
	.owner 			= THIS_MODULE,
	.unlocked_ioctl 	= tccxxx_vioc_drv_ioctl,
	.mmap 			= tccxxx_vioc_drv_mmap,
	.open 			= tccxxx_vioc_drv_open,
	.release 			= tccxxx_vioc_drv_release,
	.poll 			= tccxxx_vioc_drv_poll,
};


static char banner[] __initdata = KERN_INFO "TCC VIOC TEST Driver Initializing. \n";
static struct class *vioc_drv_class;

int __init 
tccxxx_vioc_drv_init(void)
{
	printk(banner);
	
	register_chrdev(MAJOR_ID, DEVICE_NAME, &tcc_vioc_drv_fops);
	vioc_drv_class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(vioc_drv_class, NULL, MKDEV(MAJOR_ID, MINOR_ID), NULL, DEVICE_NAME);

	memset(&vioc_drv_data, 0, sizeof(INTERRUPT_DATA_T));

	spin_lock_init(&vioc_drv_data.poll_lock);
	spin_lock_init(&vioc_drv_data.cmd_lock);

	mutex_init(&vioc_drv_data.io_mutex);
	
	init_waitqueue_head(&vioc_drv_data.poll_wq);
	init_waitqueue_head(&vioc_drv_data.cmd_wq);

	vioc_drv_clk = clk_get(NULL, "ddi");
	BUG_ON(vioc_drv_clk == NULL);

	return 0;
}

void __exit
tccxxx_vioc_drv_cleanup(void)
{
	unregister_chrdev(MAJOR_ID, DEVICE_NAME);
	clk_put(vioc_drv_clk);
	return;
}


MODULE_AUTHOR("Telechips.");
MODULE_DESCRIPTION("TCC VIOC TEST Driver");
MODULE_LICENSE("GPL");


arch_initcall(tccxxx_vioc_drv_init);
module_exit(tccxxx_vioc_drv_cleanup);


