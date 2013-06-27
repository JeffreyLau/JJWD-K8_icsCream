
//-------------------------------------------------------------------
// Copyright (c) Telechips, Inc.
// All right reserved.
//
//-------------------------------------------------------------------
/**
 * @file hdmi.h
 * @brief Definition for hdmi/video
 *
 */

#ifndef	_HDMI_H_
#define	_HDMI_H_

#include <lcd.h>
#if defined(DEFAULT_DISPLAY_HDMI) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)

#ifndef TELECHIPS
	#define TELECHIPS
#endif


#ifndef __HDMI_VIDEO_VIDEOFORMAT__
#define __HDMI_VIDEO_VIDEOFORMAT__
/**
 * @enum VideoFormat
 * Video format
 */
enum VideoFormat
{
    /** 640x480p\@60Hz */
    v640x480p_60Hz = 0,
    /** 720x480p\@60Hz */
    v720x480p_60Hz,
    /** 1280x700p\@60Hz */
    v1280x720p_60Hz,
    /** 1920x1080i\@60Hz */
    v1920x1080i_60Hz,
    /** 720x480i\@60Hz */
    v720x480i_60Hz,
    /** 720x240p\@60Hz */
    v720x240p_60Hz,
    /** 2880x480i\@60Hz */
    v2880x480i_60Hz,
    /** 2880x240p\@60Hz */
    v2880x240p_60Hz,
    /** 1440x480p\@60Hz */
    v1440x480p_60Hz,
    /** 1920x1080p\@60Hz */
    v1920x1080p_60Hz,
    /** 720x576p\@60Hz */
    v720x576p_50Hz,
    /** 1280x720p\@50Hz */
    v1280x720p_50Hz,
    /** 1920x1080i\@50Hz (V total = 1125) */
    v1920x1080i_50Hz,
    /** 720x576i\@50Hz */
    v720x576i_50Hz,
    /** 720x288p\@50Hz */
    v720x288p_50Hz,
    /** 2880x576i\@50Hz */
    v2880x576i_50Hz,
    /** 2880x288p\@50Hz */
    v2880x288p_50Hz,
    /** 1440x576p\@50Hz */
    v1440x576p_50Hz,
    /** 1920x1080p\@50Hz */
    v1920x1080p_50Hz,
    /** 1920x1080p\@24Hz */
    v1920x1080p_24Hz,
    /** 1920x1080p\@25Hz */
    v1920x1080p_25Hz,
    /** 1920x1080p\@30Hz */
    v1920x1080p_30Hz,
    /** 2880x480p\@60Hz */
    v2880x480p_60Hz,
    /** 2880x576p\@60Hz */
    v2880x576p_50Hz,
    /** 1920x1080i\@50Hz (V total = 1250) */
    v1920x1080i_50Hz_1250,
    /** 1920x1080i\@100Hz */
    v1920x1080i_100Hz,
    /** 1280x720p\@100Hz */
    v1280x720p_100Hz,
    /** 720x576p\@100Hz */
    v720x576p_100Hz,
    /** 720x576i\@100Hz */
    v720x576i_100Hz,
    /** 1920x1080i\@120Hz */
    v1920x1080i_120Hz,
    /** 1280x720p\@120Hz */
    v1280x720p_120Hz,
    /** 720x480p\@120Hz */
    v720x480p_120Hz,
    /** 720x480i\@120Hz */
    v720x480i_120Hz,
    /** 720x576p\@200Hz */
    v720x576p_200Hz,
    /** 720x576i\@200Hz */
    v720x576i_200Hz,
    /** 720x480p\@240Hz */
    v720x480p_240Hz,
    /** 720x480i\@240Hz */
    v720x480i_240Hz,
#if defined(TELECHIPS)
	max_video_formats,
#endif
};
#endif /* __HDMI_VIDEO_VIDEOFORMAT__ */
#ifndef __HDMI_VIDEO_COLORSPACE__
#define __HDMI_VIDEO_COLORSPACE__
/**
 * @enum ColorSpace
 * Color space of video stream.
 */
enum ColorSpace
{
    /** RGB color space */
    HDMI_CS_RGB,
    /** YCbCr 4:4:4 color space */
    HDMI_CS_YCBCR444,
    /** YCbCr 4:2:2 color space */
    HDMI_CS_YCBCR422
};
#endif /* __HDMI_VIDEO_COLORSPACE__ */

#ifndef __HDMI_VIDEO_COLORDEPTH__
#define __HDMI_VIDEO_COLORDEPTH__
/**
 * @enum ColorDepth
 * Color depth per pixel of video stream
 */
enum ColorDepth
{
    /** 36 bit color depth per pixel */
    HDMI_CD_36,
    /** 30 bit color depth per pixel */
    HDMI_CD_30,
    /** 24 bit color depth per pixel */
    HDMI_CD_24
};
#endif /* __HDMI_VIDEO_COLORDEPTH__ */

#ifndef __HDMI_VIDEO_HDMIMODE__
#define __HDMI_VIDEO_HDMIMODE__
/**
 * @enum HDMIMode
 * System mode
 */
enum HDMIMode
{
    /** DVI mode */
    DVI = 0,
    /** HDMI mode */
    HDMI
};
#endif /* __HDMI_VIDEO_HDMIMODE__ */

#ifndef __HDMI_VIDEO_PIXELLIMIT__
#define __HDMI_VIDEO_PIXELLIMIT__
/**
 * @enum PixelLimit
 * Pixel limitation of video stream
 */
enum PixelLimit
{
    /** Full range */
    HDMI_FULL_RANGE,
    /** Limit range for RGB color space */
    HDMI_RGB_LIMIT_RANGE,
    /** Limit range for YCbCr color space */
    HDMI_YCBCR_LIMIT_RANGE
};
#endif /* __HDMI_VIDEO_PIXELLIMIT__ */

#ifndef __HDMI_VIDEO_COLORIMETRY__
#define __HDMI_VIDEO_COLORIMETRY__
/**
 * @enum HDMIColorimetry
 * Colorimetry of video stream
 */
enum HDMIColorimetry
{
    /** Colorimetry is not defined */
    HDMI_COLORIMETRY_NO_DATA,
    /** ITU601 colorimetry */
    HDMI_COLORIMETRY_ITU601,
    /** ITU709 colorimetry */
    HDMI_COLORIMETRY_ITU709,
    /** Extended ITU601 colorimetry */
    HDMI_COLORIMETRY_EXTENDED_xvYCC601,
    /** Extended ITU709 colorimetry */
    HDMI_COLORIMETRY_EXTENDED_xvYCC709
};
#endif /* __HDMI_VIDEO_COLORIMETRY__ */

#ifndef __HDMI_VIDEO_PIXELASPECTRATIO__
#define __HDMI_VIDEO_PIXELASPECTRATIO__
/**
 * @enum PixelAspectRatio
 * Pixel aspect ratio of video stream
 */
enum PixelAspectRatio
{
    /** 4:3 pixel ratio */
    HDMI_PIXEL_RATIO_4_3,
    /** 16:9 pixel ratio */
    HDMI_PIXEL_RATIO_16_9
};
#endif /* __HDMI_VIDEO_PIXELASPECTRATIO__ */


#ifndef __HDMI_AUDIO_AUDIOSAMPLEPACKETTYPE__
#define __HDMI_AUDIO_AUDIOSAMPLEPACKETTYPE__
/**
 * @enum HDMIASPType
 * Type of HDMI audio sample packet
 */
enum HDMIASPType
{
    /** Audio Sample Packet Type */
    HDMI_ASP,
    /** One Bit Audio Packet Type */
    HDMI_DSD,
    /** High Bit Rate Packet Type */
    HDMI_HBR,
    /** DST Packet Type */
    HDMI_DST
};
#endif /* __HDMI_AUDID_AUDIOSAMPLEPACKETTYPE__ */

#ifndef __HDMI_DEVICE_VIDEO_PARAMS__
#define __HDMI_DEVICE_VIDEO_PARAMS__
/**
 * @struct device_video_params
 * Video timing paramters to set HDMI H/W @n
 * For more information, refer to HDMI register map doc.
 */
struct device_video_params
{
    /** [H Blank] */
    unsigned int    HBlank;

    /** [V Blank] */
    unsigned int    VBlank;

    /** [H Total : V Total] */
    unsigned int    HVLine;

    /** [H Sync polarity : H Sync end point : H Sync start point] */
    unsigned int    HSYNCGEN;

    /** [V Sync start line num + V Sync end line num] */
    unsigned int    VSYNCGEN;

    /** CEA VIC */
    unsigned char   AVI_VIC;
    /** CEA VIC for 16:9 pixel ratio */
    unsigned char   AVI_VIC_16_9;

    /** 0 - progressive, 1 - interlaced */
    unsigned char   interlaced;

    /** Pixel repetition if double, set 1 */
    unsigned char   repetition;

    /** V Sync polarity */
    unsigned char   polarity;

    /**
     * If interlaced mode, @n
     * [ end point of bottom field's active region : start point of that ]
     */
    unsigned int    VBLANK_F;

    /**
     * If interlaced mode, @n
     * [ start line of bottom field's V Sync : end line of that ]
     */
    unsigned int    VSYNCGEN2;

    /**
     * If interlaced mode, @n
     * [ start transition point of bottom field's V Sync : end transition of that ]
     */
    unsigned int    VSYNCGEN3;
};

#endif /* __HDMI_DEVICE_VIDEO_PARAMS__ */

#if defined(TELECHIPS)
#ifndef __HDMI_DEVICE_LCDC_TIMING_PARAMS__
#define __HDMI_DEVICE_LCDC_TIMING_PARAMS__
/**
 * @struct device_lcdc_timing_params
 * Video timing paramters to set LCDC H/W @n
 * For more information, refer to LCDC register map doc.
 */
typedef struct _lcdc_timimg_parms_t {

	unsigned int id;
	unsigned int iv;
	unsigned int ih;
	unsigned int ip;
	unsigned int dp;
	unsigned int ni;
	unsigned int tv;
	unsigned int tft;
	unsigned int stn;

	//LHTIME1
	unsigned int lpw;
	unsigned int lpc;
	//LHTIME2
	unsigned int lswc;
	unsigned int lewc;
	//LVTIME1
	unsigned int vdb;
	unsigned int vdf;
	unsigned int fpw;
	unsigned int flc;
	//LVTIME2
	unsigned int fswc;
	unsigned int fewc;
	//LVTIME3
	unsigned int fpw2;
	unsigned int flc2;
	//LVTIME4
	unsigned int fswc2;
	unsigned int fewc2;

}lcdc_timimg_parms_t;


#if (0)
#define DEFAULT_HDMI_LCDC_TIMING	{ 0,  1,  1,  0,  0,  1,  0,  1,  0,  61 ,    639 ,   59 ,     37 ,    0,  0,  5,  479 ,   29,  8,   5,   479 ,    29,  8, /*640x480p @ 60Hz       25.200MHz   */ }
#else
#define DEFAULT_HDMI_LCDC_TIMING	{ 0,  0,  0,  0,  0,  1,  0,  1,  0,  43 ,    1919,   147,     87 ,    0,  0,  4,  1079,   35,  3,   4,   1079,    35,  3, /*1920x1080p @ 60Hz     148.5MHz    */ }
#endif

static const lcdc_timimg_parms_t LCDCTimimgParams[] = 
{
#if 0                                              /*  ID  IV  IH  IP  DP  NI  TV TFT STN  LPW       LPC   LSWC     LEWC   VDB VDF FPW    FLC  FSWC FEWC FPW2   FLC2  FSWC2 FEWC2 */
  /* v1024x768p_60Hz        PIXEL_FREQ_65    ,  */    { 0,  1,  1,  0,  0,  1,  0,  1,  0,  135,    1023,   159,     23 ,    0,  0,  5,  767 ,   28,  2,   5,   767 ,    28,  2, /*1024x768p @ 60Hz       65.000MHz   */ },
#else
  /* v640x480p_60Hz         PIXEL_FREQ_25_200,  */    { 0,  1,  1,  0,  0,  1,  0,  1,  0,  61 ,    639 ,   59 ,     37 ,    0,  0,  5,  479 ,   29,  8,   5,   479 ,    29,  8, /*640x480p @ 60Hz       25.200MHz   */ },
#endif
  /* v720x480p_60Hz         PIXEL_FREQ_27_027,  */    { 0,  1,  1,  0,  0,  1,  0,  1,  0,  61 ,    719 ,   59 ,     15 ,    0,  0,  5,  479 ,   29,  8,   5,   479 ,    29,  8, /*720x480p @ 60Hz       27.027MHz   */ },
  /* v1280x720p_60Hz        PIXEL_FREQ_74_250,  */    { 0,  0,  0,  0,  0,  1,  0,  1,  0,  39 ,    1279,   219,     109,    0,  0,  4,  719 ,   19,  4,   4,   719 ,    19,  4, /*1280x720p @ 60Hz      74.25MHz    */ },
  /* v1920x1080i_60Hz       PIXEL_FREQ_74_250,  */    { 0,  0,  0,  0,  0,  0,  1,  0,  0,  43 ,    1919,   147,     87 ,    0,  0,  9,  1079,   29,  3,   9,   1079,    31,  3, /*1920x1080i @ 60Hz     74.25MHz    */ },
  /* v720x480i_60Hz         PIXEL_FREQ_27_027,  */    { 0,  1,  1,  0,  1,  0,  1,  0,  0,  123,    1439,   113,     37 ,    0,  0,  5,  479 ,   29,  7,   5,   479 ,    31,  7, /*720x480i @ 60Hz       27.027MHz   */ },
  /* v720x240p_60Hz         PIXEL_FREQ_27_027,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v2880x480i_60Hz        PIXEL_FREQ_54_054,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v2880x240p_60Hz        PIXEL_FREQ_54_054,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1440x480p_60Hz        PIXEL_FREQ_54_054,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080p_60Hz       PIXEL_FREQ_148_500, */    { 0,  0,  0,  0,  0,  1,  0,  1,  0,  43 ,    1919,   147,     87 ,    0,  0,  4,  1079,   35,  3,   4,   1079,    35,  3, /*1920x1080p @ 60Hz     148.5MHz    */ },
  /* v720x576p_50Hz         PIXEL_FREQ_27,      */    { 0,  1,  1,  0,  0,  1,  0,  1,  0,  63 ,    719 ,   67 ,     11 ,    0,  0,  4,  575 ,   38,  4,   4,   575 ,    38,  4, /*720x576p @ 50Hz       27MHz       */ },
  /* v1280x720p_50Hz        PIXEL_FREQ_74_250,  */    { 0,  0,  0,  0,  0,  1,  0,  1,  0,  39 ,    1279,   219,     439,    0,  0,  4,  719 ,   19,  4,   4,   719 ,    19,  4, /*1280x720p @ 50Hz      74.25MHz    */ },
  /* v1920x1080i_50Hz       PIXEL_FREQ_74_250,  */    { 0,  0,  0,  0,  0,  0,  1,  0,  0,  43 ,    1919,   174,     500,    0,  0,  9,  1079,   29,  3,   9,   1079,    31,  3, /*1920x1080i @ 50Hz     74.25MHz    */ },
  /* v720x576i_50Hz         PIXEL_FREQ_27,      */    { 0,  1,  1,  0,  1,  0,  1,  0,  0,  125,    1439,   137,     23 ,    0,  0,  5,  575 ,   37,  3,   5,   575 ,    39,  3, /*720x576i @ 50Hz       27MHz       */ },
  /* v720x288p_50Hz         PIXEL_FREQ_27,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v2880x576i_50Hz        PIXEL_FREQ_54,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v2880x288p_50Hz        PIXEL_FREQ_54,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1440x576p_50Hz        PIXEL_FREQ_54,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080p_50Hz       PIXEL_FREQ_148_500, */    { 0,  0,  0,  0,  0,  1,  0,  1,  0,  43 ,    1919,   174,     500,    0,  0,  4,  1079,   35,  3,   4,   1079,    35,  3, /*1920x1080p @ 50Hz     148.5MHz    */ },
  /* v1920x1080p_24Hz       PIXEL_FREQ_74_250,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080p_25Hz       PIXEL_FREQ_74_250,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080p_30Hz       PIXEL_FREQ_74_250,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v2880x480p_60Hz        PIXEL_FREQ_108_108, */    DEFAULT_HDMI_LCDC_TIMING,
  /* v2880x576p_50Hz        PIXEL_FREQ_108,     */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080i_50Hz(1250) PIXEL_FREQ_72,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080i_100Hz      PIXEL_FREQ_148_500, */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1280x720p_100Hz       PIXEL_FREQ_148_500, */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x576p_100Hz        PIXEL_FREQ_54,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x576i_100Hz        PIXEL_FREQ_54,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080i_120Hz      PIXEL_FREQ_148_500, */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1280x720p_120Hz       PIXEL_FREQ_148_500, */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x480p_120Hz        PIXEL_FREQ_54_054,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x480i_120Hz        PIXEL_FREQ_54_054,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x576p_200Hz        PIXEL_FREQ_108,     */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x576i_200Hz        PIXEL_FREQ_108,     */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x480p_240Hz        PIXEL_FREQ_108_108, */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x480i_240Hz        PIXEL_FREQ_108_108, */    DEFAULT_HDMI_LCDC_TIMING,
};

static const struct hdmi_video_mode_list_t {
	enum VideoFormat 		vfmt_val;
	enum PixelAspectRatio	ratio;
	unsigned int			width;
	unsigned int			height;
	unsigned char			vfmt_str[32];
} gRefHdmiVideoModeList[15] = 
{
/* [ 0] */ { v1920x1080p_60Hz, HDMI_PIXEL_RATIO_16_9,  1920,   1080,   "1920x1080P 60Hz [16:9]"  },
/* [ 1] */ { v1920x1080p_50Hz, HDMI_PIXEL_RATIO_16_9,  1920,   1080,   "1920x1080P 50Hz [16:9]"  },
/* [ 2] */ { v1920x1080i_60Hz, HDMI_PIXEL_RATIO_16_9,  1920,   1080,   "1920x1080I 60Hz [16:9]"  },
/* [ 3] */ { v1920x1080i_50Hz, HDMI_PIXEL_RATIO_16_9,  1920,   1080,   "1920x1080I 50Hz [16:9]"  },
/* [ 4] */ { v1280x720p_60Hz , HDMI_PIXEL_RATIO_16_9,  1280,    720,   " 1280x720P 60Hz [16:9]"  },
/* [ 5] */ { v1280x720p_50Hz , HDMI_PIXEL_RATIO_16_9,  1280,    720,   " 1280x720P 50Hz [16:9]"  },
/* [ 6] */ { v720x480p_60Hz  , HDMI_PIXEL_RATIO_16_9,   720,    480,   "  720x480P 60Hz [16:9]"  },
/* [ 7] */ { v720x480p_60Hz  , HDMI_PIXEL_RATIO_4_3 ,   720,    480,   "  720x480P 60Hz [ 4:3]"  },
/* [ 8] */ { v720x480i_60Hz  , HDMI_PIXEL_RATIO_16_9,   720,    480,   "  720x480I 60Hz [16:9]"  },
/* [ 9] */ { v720x480i_60Hz  , HDMI_PIXEL_RATIO_4_3 ,   720,    480,   "  720x480I 60Hz [ 4:3]"  },
/* [10] */ { v720x576p_50Hz  , HDMI_PIXEL_RATIO_16_9,   720,    576,   "  720x576P 50Hz [16:9]"  },
/* [11] */ { v720x576p_50Hz  , HDMI_PIXEL_RATIO_4_3 ,   720,    576,   "  720x576P 50Hz [ 4:3]"  },
/* [12] */ { v720x576i_50Hz  , HDMI_PIXEL_RATIO_16_9,   720,    576,   "  720x576I 50Hz [16:9]"  },
/* [13] */ { v720x576i_50Hz  , HDMI_PIXEL_RATIO_4_3 ,   720,    576,   "  720x576I 50Hz [ 4:3]"  },
/* [14] */ { v640x480p_60Hz  , HDMI_PIXEL_RATIO_4_3 ,   640,    480,   "  640x480P 60Hz [ 4:3]"  }
};
#endif /* __HDMI_DEVICE_VIDEO_PARAMS__ */
#endif /* TELECHIPS */


#ifndef __HDMI_VIDEO_PIXELFREQUENCY__
#define __HDMI_VIDEO_PIXELFREQUENCY__
/**
 * @enum PixelFreq
 * Pixel Frequency
 */
enum PixelFreq {
    /** 25.2 MHz pixel frequency */
    PIXEL_FREQ_25_200 = 2520,
    /** 25.175 MHz pixel frequency */
    PIXEL_FREQ_25_175 = 2517,
    /** 27 MHz pixel frequency */
    PIXEL_FREQ_27 = 2700,
    /** 27.027 MHz pixel frequency */
    PIXEL_FREQ_27_027 = 2702,
    /** 54 MHz pixel frequency */
    PIXEL_FREQ_54 = 5400,
    /** 54.054 MHz pixel frequency */
    PIXEL_FREQ_54_054 = 5405,
    /** 74.25 MHz pixel frequency */
    PIXEL_FREQ_74_250 = 7425,
    /** 74.176 MHz pixel frequency */
    PIXEL_FREQ_74_176 = 7417,
    /** 148.5 MHz pixel frequency */
    PIXEL_FREQ_148_500 = 14850,
    /** 148.352 MHz pixel frequency */
    PIXEL_FREQ_148_352 = 14835,
    /** 108.108 MHz pixel frequency */
    PIXEL_FREQ_108_108 = 10810,
    /** 72 MHz pixel frequency */
    PIXEL_FREQ_72 = 7200,
    /** 25 MHz pixel frequency */
    PIXEL_FREQ_25 = 2500,
    /** 65 MHz pixel frequency */
    PIXEL_FREQ_65 = 6500,
    /** 108 MHz pixel frequency */
    PIXEL_FREQ_108 = 10800,
    /** 162 MHz pixel frequency */
    PIXEL_FREQ_162 = 16200,
};
#endif /* __HDMI_VIDEO_PIXELFREQUENCY__ */


#ifndef __HDMI_VIDEO_PARAMETERS__
#define __HDMI_VIDEO_PARAMETERS__

//! Structure for HDMI video
struct HDMIVideoParameter {
    /** Video interface */
    enum HDMIMode mode;
    /** Video format */
    enum VideoFormat resolution;
    /** Color space */
    enum ColorSpace colorSpace;
    /** Color depth */
    enum ColorDepth colorDepth;
    /** Colorimetry */
    enum HDMIColorimetry colorimetry;
    /** Pixel aspect ratio */
    enum PixelAspectRatio pixelAspectRatio;
};
#endif /* __HDMI_VIDEO_PARAMETERS__ */

#endif //#if defined(DEFAULT_DISPLAY_HDMI) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)

#endif /* _VIDEO_H_ */

