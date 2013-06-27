/***************************************************************************************
*	FileName    : lcd driver.h
****************************************************************************************
* HDMI_TCC
****************************************************************************************/
#if defined(DEFAULT_DISPLAY_HDMI) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)

#define LCD_640_480

//--------------------------------------------------------------------
// [HDMI Video Mode Selection]
//--------------------------------------------------------------------
#define HDMI_VIDEO_MODE_TYPE	(14)

        /*------+------------------+-----------------------+-----------------+
        | Type  |  Video           |   Pixel               |     Screen      |
        | Index |  Format          |   Ratio               | Width  | Height |
        |-------+------------------+-----------------------+-----------------+
        |  [ 0] | v1920x1080p_60Hz | HDMI_PIXEL_RATIO_16_9 |  1920  | 1080   |
        |  [ 1] | v1920x1080p_50Hz | HDMI_PIXEL_RATIO_16_9 |  1920  | 1080   |
        |  [ 2] | v1920x1080i_60Hz | HDMI_PIXEL_RATIO_16_9 |  1920  | 1080   |
        |  [ 3] | v1920x1080i_50Hz | HDMI_PIXEL_RATIO_16_9 |  1920  | 1080   |
        |  [ 4] | v1280x720p_60Hz  | HDMI_PIXEL_RATIO_16_9 |  1280  |  720   |
        |  [ 5] | v1280x720p_50Hz  | HDMI_PIXEL_RATIO_16_9 |  1280  |  720   |
        |  [ 6] | v720x480p_60Hz   | HDMI_PIXEL_RATIO_16_9 |   720  |  480   |
        |  [ 7] | v720x480p_60Hz   | HDMI_PIXEL_RATIO_4_3  |   720  |  480   |
        |  [ 8] | v720x480i_60Hz   | HDMI_PIXEL_RATIO_16_9 |   720  |  480   |
        |  [ 9] | v720x480i_60Hz   | HDMI_PIXEL_RATIO_4_3  |   720  |  480   |
        |  [10] | v720x576p_50Hz   | HDMI_PIXEL_RATIO_16_9 |   720  |  576   |
        |  [11] | v720x576p_50Hz   | HDMI_PIXEL_RATIO_4_3  |   720  |  576   |
        |  [12] | v720x576i_50Hz   | HDMI_PIXEL_RATIO_16_9 |   720  |  576   |
        |  [13] | v720x576i_50Hz   | HDMI_PIXEL_RATIO_4_3  |   720  |  576   |
        |  [14] | v640x480p_60Hz   | HDMI_PIXEL_RATIO_4_3  |   640  |  480   |
        +-------------------------------------------------------------------*/

//--------------------------------------------------------------------
// [HDMI/DVI Mode Selection]
//--------------------------------------------------------------------
#define HDMI_MODE_TYPE		(1)		// 0 : DVI mode, 1 : HDMI mode

// Unit 100hz

#endif	/*#if defined(DEFAULT_DISPLAY_HDMI) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)*/
