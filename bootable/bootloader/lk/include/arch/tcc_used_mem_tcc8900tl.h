/****************************************************************************
 *	 TCC Version 0.6
 *	 Copyright (c) telechips, Inc.
 *	 ALL RIGHTS RESERVED
 *
****************************************************************************/

//#define USE_VIQE
//#define USE_CAMERA
//#define USE_CC_UI_LAYER

/* Memory Allocations */
#define PMEM_SURF_SIZE			(16*SZ_1M)
#define PMEM_OSD_SIZE			(30*SZ_1M)
#ifdef USE_VIQE
#define PMEM_VIQE_SIZE			(4*SZ_1M)
#else
#define PMEM_VIQE_SIZE			(0)
#endif
#ifdef USE_CAMERA
#define PMEM_CAM_SIZE			(8*SZ_1M)
#else
#define PMEM_CAM_SIZE			(2*SZ_1M)
#endif
#define OVERLAY_MEM_TOTAL_SIZE		(9*SZ_1M)
#define VIDEO_MEM_TOTAL_SIZE		(58*SZ_1M)

#define FB_SCALE_MEM_TOTAL_SIZE		(10*SZ_1M)

#if defined(USE_CC_UI_LAYER)
#define CC_UI_MEM_TOTAL_SIZE			(450*SZ_1K*2)	/* ((320*60*4) * 6) */
#else
#define CC_UI_MEM_TOTAL_SIZE			(0)
#endif

#define TOTAL_FIXED_MEM_SIZE		(PMEM_SURF_SIZE+PMEM_OSD_SIZE+PMEM_VIQE_SIZE+PMEM_CAM_SIZE+OVERLAY_MEM_TOTAL_SIZE+VIDEO_MEM_TOTAL_SIZE + FB_SCALE_MEM_TOTAL_SIZE + CC_UI_MEM_TOTAL_SIZE)

#if (TCC_MEM_SIZE <= 256)
#define PMEM_SURF_BASE			(MEM_PHYS_OFFSET + TOTAL_SDRAM_SIZE - TOTAL_FIXED_MEM_SIZE)
#else
#define PMEM_SURF_BASE			(MEM_PHYS_OFFSET + 0x0100000)
#endif

#define PMEM_OSD_BASE			(PMEM_SURF_BASE + PMEM_SURF_SIZE)
#define PMEM_VIQE_BASE			(PMEM_OSD_BASE + PMEM_OSD_SIZE)
#define PMEM_CAM_BASE			(PMEM_VIQE_BASE + PMEM_VIQE_SIZE)
#define OVERLAY_MEM_BASE		(PMEM_CAM_BASE + PMEM_CAM_SIZE)
#define VIDEO_MEM_BASE			(OVERLAY_MEM_BASE + OVERLAY_MEM_TOTAL_SIZE)
#define FB_SCALE_MEM_BASE		(VIDEO_MEM_BASE + VIDEO_MEM_TOTAL_SIZE)
#define CC_UI_MEM_BASE				(FB_SCALE_MEM_BASE + FB_SCALE_MEM_TOTAL_SIZE)

/*=============================================================================*/
#define PA_JPEG_RAW_BASE_ADDR		(VIDEO_MEM_BASE)
#define JPEG_RAW_MEM_SIZE		(0x1000000)

#define PA_JPEG_STREAM_BASE_ADDR	(VIDEO_MEM_BASE + JPEG_RAW_MEM_SIZE)
#define JPEG_STREAM_MEM_SIZE		(0x400000)
#define JPEG_TOTAL_MEM_SIZE		(JPEG_RAW_MEM_SIZE + JPEG_STREAM_MEM_SIZE)

#define TCC_JPEG_HEADER_SIZE		(0x030000) 
#define PA_JPEG_HEADER_BASE_ADDR	(VIDEO_MEM_BASE + JPEG_TOTAL_MEM_SIZE - TCC_JPEG_HEADER_SIZE)

//for Camera
#define PREVIEW_MEM_SIZE		(PMEM_CAM_SIZE)
#define CAPTURE_MEM_SIZE		(PMEM_CAM_SIZE)
#define PA_PREVIEW_BASE_ADDR		(PMEM_CAM_BASE)

//for Video
#define VIDEO_PHY_ADDR			(VIDEO_MEM_BASE)
#define VIDEO_MEM_SIZE			(VIDEO_MEM_TOTAL_SIZE)

//for Overlay
#define OVERLAY_TEMP_MEM_SIZE		(0x200000)

#define OVERLAY_PHY_ADDR		(OVERLAY_MEM_BASE)
#define OVERLAY_SIZE			(OVERLAY_MEM_TOTAL_SIZE - OVERLAY_TEMP_MEM_SIZE)
#define TEMP_PHY_ADDR			(OVERLAY_PHY_ADDR + OVERLAY_SIZE)
#define PRIOR_TEMP_PHY_ADDR		(TEMP_PHY_ADDR + OVERLAY_TEMP_MEM_SIZE/2)

#define CAM_OVERLAY_TEMP_ADDR		(TEMP_PHY_ADDR)
#define CAM_OVERLAY_PRIOR_TEMP_ADDR	(PRIOR_TEMP_PHY_ADDR)


// for FB hdmi display total 6Mbyte
// for FB out display total 
// scaler buffer : 1280 * 720 * 2 * 2 = 3.6M
// G2D buffer : 800 * 480 * 2 * 2 = 1.5M
// total :  LCD : 5.1M
#define FB_SCALE_MAX_WIDTH		1920
#define FB_SCALE_MAX_HEIGHT		1080


#define FB_SCALE_SIZE			(FB_SCALE_MAX_WIDTH * FB_SCALE_MAX_HEIGHT *2)
#define FB_G2D_SIZE			(800 * 480 *2)

#define FB_SCALE_ADDR0			FB_SCALE_MEM_BASE
#define FB_SCALE_ADDR1			(FB_SCALE_ADDR0 + FB_SCALE_SIZE)

#define FB_G2D_ADDR0			(FB_SCALE_ADDR1 + FB_SCALE_SIZE)
#define FB_G2D_ADDR1			(FB_G2D_ADDR0 + FB_G2D_SIZE)

#define CC_UI_ADDR					(CC_UI_MEM_BASE)
#define CC_UI_SIZE					(CC_UI_MEM_TOTAL_SIZE)
