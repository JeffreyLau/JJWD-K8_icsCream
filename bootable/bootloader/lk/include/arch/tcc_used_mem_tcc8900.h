/****************************************************************************
 *	 TCC Version 0.6
 *	 Copyright (c) telechips, Inc.
 *	 ALL RIGHTS RESERVED
 *
****************************************************************************/

//#define USE_VIQE
//#define USE_CAMERA
//#define USE_LVDS
//#define USE_CC_UI_LAYER

/* Memory Allocations */
#define PMEM_SURF_SIZE			(0)

#ifdef USE_VIQE
#define PMEM_VIQE_SIZE			(4*SZ_1M)
#else
#define PMEM_VIQE_SIZE			(0)
#endif
#ifdef USE_CAMERA
#define PMEM_CAM_SIZE			(16*SZ_1M)
#else
#define PMEM_CAM_SIZE			(2*SZ_1M)
#endif

#if defined(USE_LVDS)
// This is for big LCD panel.
#define FB_WIDTH 		1280
#define FB_HEIGHT 		800
#define FB_MEM_SIZE		(ARRAY_MBYTE(FB_WIDTH * FB_HEIGHT * 4 * 2)) // framebuffer use defined memory area.
#else
// This is default memory map.
#define FB_WIDTH 		800
#define FB_HEIGHT 		480
	#if (TCC_MEM_SIZE > 256)
	#define FB_MEM_SIZE	(5 * SZ_1M)	//800 * 480 * 4 *3(framebuffer double buffer, vsync buffer)
	#else
	#define FB_MEM_SIZE	(0)
	#endif//

#endif//


//for Overlay			 default : 2Mbyte
#define OVERLAY_TEMP_MEM_SIZE		(ARRAY_MBYTE(FB_WIDTH * FB_HEIGHT * 2 * 2))

#if defined(CONFIG_FB_TCC_OVERLAY_EXT) || defined(FB_TCC_OVERLAY_EXT)
	#define OVERLAY_MEM_TOTAL_SIZE		((20 * SZ_1M) + (OVERLAY_TEMP_MEM_SIZE * 2))
	#else
	#define OVERLAY_MEM_TOTAL_SIZE		((8 * SZ_1M) + (OVERLAY_TEMP_MEM_SIZE * 2))
	#endif


#if (TCC_MEM_SIZE <= 256) || defined(DRAM_DDR3)
/* We restrict video resolution to 720p for 256MB or DDR3 */
#define VIDEO_MEM_TOTAL_SIZE		(54*SZ_1M)
#else
#define VIDEO_MEM_TOTAL_SIZE		(58*SZ_1M)
#endif

#define HDMI_DISPLAY_MAX_WIDTH		1280
#define HDMI_DISPLAY_MAX_HEIGHT		720
// FB scale up buffer   	default: 1920 * 1080 * 2 	= 4Mbye
#define FB_SCALE_SIZE			(ARRAY_MBYTE(HDMI_DISPLAY_MAX_WIDTH * HDMI_DISPLAY_MAX_HEIGHT * 2))

// LVDS    : 1280 * 800 * 2  	= 2Mbyte
// RGB LCD : 800 * 480 * 2 = 0.7Mbye =>1Mbyte
#define FB_G2D_SIZE				(ARRAY_MBYTE(FB_WIDTH * FB_HEIGHT * 2))
// FB  g2d converter & scaler up total buffer size  	default : (1 * 2) + (4 * 2) = 10M
#define FB_SCALE_MEM_TOTAL_SIZE	((FB_SCALE_SIZE * 2) + (FB_G2D_SIZE * 2))

#if defined(USE_CC_UI_LAYER)
#define CC_UI_MEM_TOTAL_SIZE			(450*SZ_1K*2)	/* ((320*60*4) * 6) */
#else
#define CC_UI_MEM_TOTAL_SIZE			(0)
#endif

#define TOTAL_FIXED_MEM_SIZE	(PMEM_SURF_SIZE+PMEM_VIQE_SIZE+PMEM_CAM_SIZE + FB_MEM_SIZE + OVERLAY_MEM_TOTAL_SIZE+VIDEO_MEM_TOTAL_SIZE + FB_SCALE_MEM_TOTAL_SIZE + CC_UI_MEM_TOTAL_SIZE)

#if (TCC_MEM_SIZE <= 256)
#define PMEM_SURF_BASE			(MEM_PHYS_OFFSET + TOTAL_SDRAM_SIZE - TOTAL_FIXED_MEM_SIZE)
#else
#define PMEM_SURF_BASE			(MEM_PHYS_OFFSET + 0x2000000)
#endif

#define PMEM_VIQE_BASE			(PMEM_SURF_BASE + PMEM_SURF_SIZE)
#define PMEM_CAM_BASE			(PMEM_VIQE_BASE + PMEM_VIQE_SIZE)
#define OVERLAY_MEM_BASE		(PMEM_CAM_BASE + PMEM_CAM_SIZE)
#define VIDEO_MEM_BASE			(OVERLAY_MEM_BASE + OVERLAY_MEM_TOTAL_SIZE)
#define FB_MEM_BASE		(VIDEO_MEM_BASE + VIDEO_MEM_TOTAL_SIZE)
#define FB_SCALE_MEM_BASE			(FB_MEM_BASE + FB_MEM_SIZE)
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
#define	VIDEO_PHY_ADDR			(VIDEO_MEM_BASE)
#define VIDEO_MEM_SIZE			(VIDEO_MEM_TOTAL_SIZE)



#if defined(CONFIG_FB_TCC_OVERLAY_EXT) || defined(FB_TCC_OVERLAY_EXT)
#define OVERLAY_SIZE_PER_DEVICE		(OVERLAY_MEM_TOTAL_SIZE/2)
#define OVERLAY_SIZE			(OVERLAY_SIZE_PER_DEVICE - OVERLAY_TEMP_MEM_SIZE)

#define OVERLAY_PHY_ADDR		(OVERLAY_MEM_BASE)
#define TEMP_PHY_ADDR			(OVERLAY_PHY_ADDR + OVERLAY_SIZE)
#define PRIOR_TEMP_PHY_ADDR		(TEMP_PHY_ADDR + OVERLAY_TEMP_MEM_SIZE/2)

#define OVERLAY1_PHY_ADDR		(OVERLAY_MEM_BASE + OVERLAY_SIZE_PER_DEVICE)
#define TEMP1_PHY_ADDR			(OVERLAY1_PHY_ADDR + OVERLAY_SIZE)
#define PRIOR_TEMP1_PHY_ADDR		(TEMP1_PHY_ADDR + OVERLAY_TEMP_MEM_SIZE/2)
	
#define CAM_OVERLAY_TEMP_ADDR		(TEMP1_PHY_ADDR)
#define CAM_OVERLAY_PRIOR_TEMP_ADDR	(PRIOR_TEMP1_PHY_ADDR)
#else
#define OVERLAY_PHY_ADDR		(OVERLAY_MEM_BASE)
#define OVERLAY_SIZE			(OVERLAY_MEM_TOTAL_SIZE - OVERLAY_TEMP_MEM_SIZE)
#define TEMP_PHY_ADDR			(OVERLAY_PHY_ADDR + OVERLAY_SIZE)
#define PRIOR_TEMP_PHY_ADDR		(TEMP_PHY_ADDR + OVERLAY_TEMP_MEM_SIZE/2)

#define CAM_OVERLAY_TEMP_ADDR		(TEMP_PHY_ADDR)
#define CAM_OVERLAY_PRIOR_TEMP_ADDR	(PRIOR_TEMP_PHY_ADDR)
#endif

#define FB_SCALE_ADDR0				FB_SCALE_MEM_BASE
#define FB_SCALE_ADDR1				(FB_SCALE_ADDR0 + FB_SCALE_SIZE)

#define FB_G2D_ADDR0				(FB_SCALE_ADDR1 + FB_SCALE_SIZE)
#define FB_G2D_ADDR1				(FB_G2D_ADDR0 + FB_G2D_SIZE)

#define CC_UI_ADDR					(CC_UI_MEM_BASE)
#define CC_UI_SIZE					(CC_UI_MEM_TOTAL_SIZE)
