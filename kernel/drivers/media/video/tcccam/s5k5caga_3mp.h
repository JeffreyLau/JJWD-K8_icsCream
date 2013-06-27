/*
 * drivers/media/video/tcccam/s5k5caga_3mp.h
 *
 * Register definitions for the s5k5caga CameraChip.
 *
 * Author: Byunghoon Moon (joonimbc@telechips.com)
 *
 * Copyright (C) 2011 Telechips, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License 
 * version 2. This program is licensed "as is" without any warranty of any 
 * kind, whether express or implied.
 */
#include <mach/globals.h>
#include "sensor_if.h"

#ifndef s5k5caga_H
#define s5k5caga_H

#define S5K5CAGA_REG_TERM 				0x0000	/* terminating list entry for reg */
#define S5K5CAGA_VAL_TERM 				0x0000	/* terminating list entry for val */

#define ENABLE_BURST_MODE
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
extern void sensor_info_init_s5k5caga(TCC_SENSOR_INFO_TYPE *sensor_info);
extern void sensor_init_fnc_s5k5caga(SENSOR_FUNC_TYPE *sensor_func);
#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
/* The s5k5caga I2C sensor chip has a fixed slave address of 0x7A. */
#define SENSOR_I2C_ADDR		0x78
#define SENSOR_FRAMERATE	15

//CLOCK
#if defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
#if defined(CONFIG_USE_ISP)
#define CAM_POLARITY_VSYNC	0   // 1: low active,	0: high active	
#define CAM_POLARITY_HSYNC	0   // 1: low active,	0: high active	
#define CAM_POLARITY_PCLK	0   // 1: positive edge	0: negative edge
#endif //CONFIG_USE_ISP
#define CKC_CAMERA_MCLK				240000
#define CKC_CAMERA_MCLK_SRC			PCDIRECTPLL2
#define CKC_CAMERA_SCLK				1440000			
#define CKC_CAMERA_SCLK_SRC			PCDIRECTPLL2
#elif defined(CONFIG_ARCH_TCC92XX)
#define CKC_CAMERA_MCLK				240000
#define CKC_CAMERA_MCLK_SRC			PCDIRECTPLL2
#define CKC_CAMERA_SCLK				1440000
#define CKC_CAMERA_SCLK_SRC			PCDIRECTPLL2
#elif defined(CONFIG_ARCH_TCC93XX)
#if defined(CONFIG_USE_ISP)
#define CAM_POLARITY_VSYNC	0   // 1: low active,	0: high active	
#define CAM_POLARITY_HSYNC	0   // 1: low active,	0: high active	
#define CAM_POLARITY_PCLK	0   // 1: positive edge	0: negative edge
#endif //CONFIG_USE_ISP
#define CKC_CAMERA_MCLK				240000
#define CKC_CAMERA_MCLK_SRC			PCDIRECTPLL1
#define CKC_CAMERA_SCLK				1680000			// max 20 fps : 76.8Mhz(pclk)/144Mhz(PLL2), max 30 fps : 84Mhz(pclk)/168Mhz(PLL1)
#define CKC_CAMERA_SCLK_SRC			PCDIRECTPLL1
#else
// todo : 
#endif

#define FRAMESKIP_COUNT_FOR_CAPTURE 1

// ZOOM Setting!!
#define PRV_W			640
#define PRV_H			480
#define PRV_ZOFFX		8
#define PRV_ZOFFY		6

#define CAP_W			2048
#define CAP_H			1536
#define CAP_ZOFFX		32
#define CAP_ZOFFY		24

#define CAM_2XMAX_ZOOM_STEP 	15
#define CAM_CAPCHG_WIDTH  		640

#ifdef CONFIG_USE_ISP
#define CAM_MAX_ZOOM_STEP    CAM_2XMAX_ZOOM_STEP+1
#endif

enum sensor_reg_width {
         WORD_LEN,
         BYTE_LEN,
         MS_DELAY
};

struct sensor_reg {
	unsigned short reg;
	unsigned short val;
	enum sensor_reg_width width;
};

struct sensor_serial_reg {
	unsigned short reg;
	enum sensor_reg_width width;
};

struct capture_size {
	unsigned long width;
	unsigned long height;
};

extern struct capture_size sensor_sizes[];

extern void sensor_init_fnc(SENSOR_FUNC_TYPE *sensor_func);
#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
#endif /* S5K5CAGA_H */

