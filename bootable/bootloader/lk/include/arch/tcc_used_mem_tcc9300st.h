/****************************************************************************
 *	 TCC Version 0.6
 *	 Copyright (c) telechips, Inc.
 *	 ALL RIGHTS RESERVED
 *
****************************************************************************/

//#define USE_VIQE
//#define USE_CAMERA
#define USE_DUALOUTPUT

#if defined(TCC_EXCLUSIVE_UI_SUPPORT)
#define USE_EXCLUSIVE_UI
#endif
//#define USE_CC_UI_LAYER

/* Memory Allocations */
#define PMEM_SURF_SIZE				(0*SZ_1M)
#ifdef USE_VIQE
#define PMEM_VIQE_SIZE				(4*SZ_1M)
#else
#define PMEM_VIQE_SIZE				(0)
#endif
#ifdef USE_CAMERA
#define PMEM_CAM_SIZE				(16*SZ_1M)
#else
#define PMEM_CAM_SIZE				(2*SZ_1M)
#endif
#if defined(CONFIG_FB_TCC_OVERLAY_EXT) || defined(FB_TCC_OVERLAY_EXT)
	#if defined(CONFIG_TCC_VIDEO_DISPLAY_BY_VSYNC_INT) || defined(TCC_VIDEO_DISPLAY_BY_VSYNC_INT)
	#define OVERLAY_MEM_TOTAL_SIZE	(28*SZ_1M)
	#else
	#define OVERLAY_MEM_TOTAL_SIZE	(24*SZ_1M)
	#endif
#else
	#if defined(CONFIG_TCC_VIDEO_DISPLAY_BY_VSYNC_INT) || defined(TCC_VIDEO_DISPLAY_BY_VSYNC_INT)
	#define OVERLAY_MEM_TOTAL_SIZE	(28*SZ_1M)
	#else
	#define OVERLAY_MEM_TOTAL_SIZE	(12*SZ_1M)
	#endif
#endif
#define VIDEO_MEM_TOTAL_SIZE		(58*SZ_1M)
#if defined(USE_DUALOUTPUT)
#define FB_MEM_SIZE					(16*SZ_1M) 
#else
#define FB_MEM_SIZE					(0)
#endif
#if defined(USE_DUALOUTPUT) || defined(USE_EXCLUSIVE_UI)
#define FB_SCALE_MEM_TOTAL_SIZE		(24*SZ_1M)
#else
#define FB_SCALE_MEM_TOTAL_SIZE		(10*SZ_1M)
#endif
#if defined(USE_EXCLUSIVE_UI)
#define EXCLUSIVE_UI_MEM_TOTAL_SIZE	(48*SZ_1M) /* 16+8+8+8+8 MB */
#else
#define EXCLUSIVE_UI_MEM_TOTAL_SIZE	(0)
#endif
#if defined(USE_CC_UI_LAYER)
#define CC_UI_MEM_TOTAL_SIZE			(12*SZ_1M*2)
#else
#define CC_UI_MEM_TOTAL_SIZE			(0)
#endif

#define TOTAL_FIXED_MEM_SIZE		(PMEM_SURF_SIZE+PMEM_VIQE_SIZE+PMEM_CAM_SIZE+OVERLAY_MEM_TOTAL_SIZE+VIDEO_MEM_TOTAL_SIZE+FB_MEM_SIZE+FB_SCALE_MEM_TOTAL_SIZE+EXCLUSIVE_UI_MEM_TOTAL_SIZE+CC_UI_MEM_TOTAL_SIZE)

#define PMEM_SURF_BASE				(MEM_PHYS_OFFSET + TOTAL_SDRAM_SIZE - TOTAL_FIXED_MEM_SIZE)

#define PMEM_VIQE_BASE				(PMEM_SURF_BASE + PMEM_SURF_SIZE)
#define PMEM_CAM_BASE				(PMEM_VIQE_BASE + PMEM_VIQE_SIZE)
#define OVERLAY_MEM_BASE			(PMEM_CAM_BASE + PMEM_CAM_SIZE)
#define VIDEO_MEM_BASE				(OVERLAY_MEM_BASE + OVERLAY_MEM_TOTAL_SIZE)
#define FB_MEM_BASE					(VIDEO_MEM_BASE + VIDEO_MEM_TOTAL_SIZE)
#define FB_SCALE_MEM_BASE			(FB_MEM_BASE + FB_MEM_SIZE)
#define EXCLUSIVE_UI_MEM_BASE  		(FB_SCALE_MEM_BASE + FB_SCALE_MEM_TOTAL_SIZE)
#define CC_UI_MEM_BASE				(EXCLUSIVE_UI_MEM_BASE + EXCLUSIVE_UI_MEM_TOTAL_SIZE)

/*=============================================================================*/
#define PA_JPEG_RAW_BASE_ADDR		(VIDEO_MEM_BASE)
#define JPEG_RAW_MEM_SIZE			(0x1000000)

#define PA_JPEG_STREAM_BASE_ADDR	(VIDEO_MEM_BASE + JPEG_RAW_MEM_SIZE)
#define JPEG_STREAM_MEM_SIZE		(0x400000)
#define JPEG_TOTAL_MEM_SIZE			(JPEG_RAW_MEM_SIZE + JPEG_STREAM_MEM_SIZE)

#define TCC_JPEG_HEADER_SIZE		(0x030000) 
#define PA_JPEG_HEADER_BASE_ADDR	(VIDEO_MEM_BASE + JPEG_TOTAL_MEM_SIZE - TCC_JPEG_HEADER_SIZE)

//for Camera
#define PREVIEW_MEM_SIZE			(PMEM_CAM_SIZE)
#define CAPTURE_MEM_SIZE			(PMEM_CAM_SIZE)
#define PA_PREVIEW_BASE_ADDR		(PMEM_CAM_BASE)

//for Video
#define	VIDEO_PHY_ADDR				(VIDEO_MEM_BASE)
#define VIDEO_MEM_SIZE				(VIDEO_MEM_TOTAL_SIZE)

//for Overlay
#define OVERLAY_TEMP_MEM_SIZE		(0x200000)

#if defined(CONFIG_FB_TCC_OVERLAY_EXT) || defined(FB_TCC_OVERLAY_EXT)
#define OVERLAY_SIZE_PER_DEVICE		(OVERLAY_MEM_TOTAL_SIZE/2)
#define OVERLAY_SIZE				(OVERLAY_SIZE_PER_DEVICE - OVERLAY_TEMP_MEM_SIZE)

#define OVERLAY_PHY_ADDR		(OVERLAY_MEM_BASE)
#define OVERLAY1_PHY_ADDR		(OVERLAY_MEM_BASE + OVERLAY_SIZE)

#define TEMP_PHY_ADDR			(OVERLAY1_PHY_ADDR + OVERLAY_SIZE)
#define PRIOR_TEMP_PHY_ADDR		(TEMP_PHY_ADDR + OVERLAY_TEMP_MEM_SIZE/2)
#define TEMP1_PHY_ADDR			(PRIOR_TEMP_PHY_ADDR + OVERLAY_TEMP_MEM_SIZE/2)
#define PRIOR_TEMP1_PHY_ADDR		(TEMP1_PHY_ADDR + OVERLAY_TEMP_MEM_SIZE/2)
	
#define CAM_OVERLAY_TEMP_ADDR		(TEMP1_PHY_ADDR)
#define CAM_OVERLAY_PRIOR_TEMP_ADDR	(PRIOR_TEMP1_PHY_ADDR)
#else
#define OVERLAY_PHY_ADDR			(OVERLAY_MEM_BASE)
#define OVERLAY_SIZE				(OVERLAY_MEM_TOTAL_SIZE - OVERLAY_TEMP_MEM_SIZE)
#define TEMP_PHY_ADDR				(OVERLAY_PHY_ADDR + OVERLAY_SIZE)
#define PRIOR_TEMP_PHY_ADDR			(TEMP_PHY_ADDR + OVERLAY_TEMP_MEM_SIZE/2)

#define CAM_OVERLAY_TEMP_ADDR		(TEMP_PHY_ADDR)
#define CAM_OVERLAY_PRIOR_TEMP_ADDR	(PRIOR_TEMP_PHY_ADDR)
#endif

// for FB hdmi display total 6Mbyte
#define HDMI_DISPLAY_MAX_WIDTH		1920
#define HDMI_DISPLAY_MAX_HEIGHT		1080

#if defined(USE_EXCLUSIVE_UI)
#define FB_G2D_WIDTH				1920
#define FB_G2D_HEIGHT				1080
#elif defined(USE_DUALOUTPUT)
#define FB_G2D_WIDTH				1280
#define FB_G2D_HEIGHT				720
#else
#define FB_G2D_WIDTH				800
#define FB_G2D_HEIGHT				480
#endif

#if defined(USE_EXCLUSIVE_UI)
#define FB_SCALE_SIZE				(HDMI_DISPLAY_MAX_WIDTH * HDMI_DISPLAY_MAX_HEIGHT *2)
#define FB_G2D_SIZE					(FB_G2D_WIDTH * FB_G2D_HEIGHT *4)

#define FB_SCALE_ADDR0				FB_SCALE_MEM_BASE
#define FB_SCALE_ADDR1				(FB_SCALE_ADDR0 + FB_SCALE_SIZE)

#define FB_G2D_ADDR0				(FB_SCALE_ADDR1 + FB_SCALE_SIZE)
#define FB_G2D_ADDR1				(FB_G2D_ADDR0 + FB_G2D_SIZE)

#define EXCLUSIVE_UI_SRC_ADDR  		EXCLUSIVE_UI_MEM_BASE
#define EXCLUSIVE_UI_SRC_SIZE		(16*SZ_1M)
#define EXCLUSIVE_UI_DST_ADDR  		(EXCLUSIVE_UI_SRC_ADDR + EXCLUSIVE_UI_SRC_SIZE)
#define EXCLUSIVE_UI_DST_SIZE		(8*SZ_1M)
#define EXCLUSIVE_UI_SCALE_ADDR		(EXCLUSIVE_UI_DST_ADDR + EXCLUSIVE_UI_DST_SIZE)
#define EXCLUSIVE_UI_SCALE_SIZE		(8*SZ_1M)
#define EXCLUSIVE_UI_G2D_ADDR		(EXCLUSIVE_UI_SCALE_ADDR + EXCLUSIVE_UI_SCALE_SIZE)
#define EXCLUSIVE_UI_G2D_SIZE		(8*SZ_1M)
#define EXCLUSIVE_UI_VIQE_ADDR		(EXCLUSIVE_UI_G2D_ADDR + EXCLUSIVE_UI_G2D_SIZE)
#define EXCLUSIVE_UI_VIQE_SIZE		(8*SZ_1M)
#elif defined(USE_DUALOUTPUT)
#define FB_SCALE_SIZE				(HDMI_DISPLAY_MAX_WIDTH * HDMI_DISPLAY_MAX_HEIGHT *2)
#define FB_G2D_SIZE					(FB_G2D_WIDTH * FB_G2D_HEIGHT *2)

#define FB_SCALE_ADDR0				FB_SCALE_MEM_BASE
#define FB_SCALE_ADDR1				(FB_SCALE_ADDR0 + FB_SCALE_SIZE)
#define FB_SCALE_ADDR2				(FB_SCALE_ADDR1 + FB_SCALE_SIZE)
#define FB_SCALE_ADDR3				(FB_SCALE_ADDR2 + FB_SCALE_SIZE)

#define FB_G2D_ADDR0				(FB_SCALE_ADDR3 + FB_SCALE_SIZE)
#define FB_G2D_ADDR1				(FB_G2D_ADDR0 + FB_G2D_SIZE)
#define FB_G2D_ADDR2				(FB_G2D_ADDR1 + FB_G2D_SIZE)
#define FB_G2D_ADDR3				(FB_G2D_ADDR2 + FB_G2D_SIZE)
#else
#define FB_SCALE_SIZE				(HDMI_DISPLAY_MAX_WIDTH * HDMI_DISPLAY_MAX_HEIGHT *2)
#define FB_G2D_SIZE					(FB_G2D_WIDTH * FB_G2D_HEIGHT *2)

#define FB_SCALE_ADDR0				FB_SCALE_MEM_BASE
#define FB_SCALE_ADDR1				(FB_SCALE_ADDR0 + FB_SCALE_SIZE)

#define FB_G2D_ADDR0				(FB_SCALE_ADDR1 + FB_SCALE_SIZE)
#define FB_G2D_ADDR1				(FB_G2D_ADDR0 + FB_G2D_SIZE)
#endif

#define CC_UI_ADDR					(CC_UI_MEM_BASE)
#define CC_UI_SIZE					(CC_UI_MEM_TOTAL_SIZE)