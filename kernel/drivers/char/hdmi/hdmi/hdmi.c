//-------------------------------------------------------------------
// Copyright (c) Telechips, Inc.
// All right reserved.
//
//-------------------------------------------------------------------
/**
 * @file hdmi.c
 * @brief This file implements HDMI device driver.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/wait.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/platform_device.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include "regs-hdmi.h"
#include <mach/audio.h>
#include <mach/hdmi.h>
#include <asm/mach-types.h>

#include "key.h"
//#include "key-tcc.h"

#include <linux/gpio.h>
#include <mach/gpio.h>

#ifdef HDMI_TX13_REV_05
#include "vwrapper.h"
#include "regs-vwrapper.h"
#endif
#include <mach/tcc_board_hdmi.h>
#include <mach/tca_fb_output.h>

#if defined(CONFIG_ARCH_TCC892X)
#include <mach/tca_ckc.h>
#include <mach/vioc_outcfg.h>
#endif//

#define VERSION 		"1.2.1" /* Driver version number */
#define HDMI_MINOR 	240 /* Major 10, Minor 240, /dev/hdmi */


/**
 * If 'SIMPLAYHD' is 1, check Ri of 127th and 128th frame -@n
 * on 3rd authentication. And also check if Ri of 127th frame is -@n
 * different from that of 128th frame. if 'SIMPLAYHD' is 0, check only Ri -@n
 * of 128th frame.
 */
#define HDMI_DEBUG 	0
#define HDMI_DEBUG_TIME 0

#if HDMI_DEBUG
#define dprintk(args...)    printk(args)
#else
#define dprintk(args...)
#endif


#if HDMI_DEBUG_TIME
unsigned long jstart, jend;
unsigned long ji2cstart, ji2cend;
#endif


static struct clk *hdmi_clk = NULL;

/**
 * N value of ACR packet.@n
 * 4096  is the N value for 32 KHz sampling frequency @n
 * 6272  is the N value for 44.1 KHz sampling frequency @n
 * 12544 is the N value for 88.2 KHz sampling frequency @n
 * 25088 is the N value for 176.4 KHz sampling frequency @n
 * 6144  is the N value for 48 KHz sampling frequency @n
 * 12288 is the N value for 96 KHz sampling frequency @n
 * 24576 is the N value for 192 KHz sampling frequency @n
 */
static const unsigned int ACR_N_params[] =
{
    4096,
    6272,
    12544,
    25088,
    6144,
    12288,
    24576
};

static int /*__init*/ hdmi_init(void);
static void /*__init*/ hdmi_exit(void);
static int hdmi_open(struct inode *inode, struct file *file);
static int hdmi_release(struct inode *inode, struct file *file);
static irqreturn_t hdmi_handler(int irq, void *dev_id);
static ssize_t hdmi_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos);
static ssize_t hdmi_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos);
static int hdmi_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static void hdmi_avi_update_checksum(void);

static void hdmi_spd_update_checksum(void);
static int hdmi_set_spd_infoframe(struct HDMIVideoFormatCtrl VideoFormatCtrl);

void hdmi_aui_update_checksum(void);
int hdmi_set_color_space(enum ColorSpace);
int hdmi_set_color_depth(enum ColorDepth);
void hdmi_set_video_mode(struct device_video_params mode);
int hdmi_set_pixel_limit(enum PixelLimit);
int hdmi_set_hdmimode(int mode);
int hdmi_set_pixel_aspect_ratio(enum PixelAspectRatio);
int hdmi_set_audio_sample_freq(enum SamplingFreq);
int hdmi_set_audio_packet_type(enum HDMIASPType);
int hdmi_set_audio_channel_number(enum ChannelNum);
void hdmi_start(void);
void hdmi_stop(void);
void hdmi_audio_start(void);
void hdmi_audio_stop(void);
void hdmi_phy_reset(void);


#if defined(TELECHIPS)
// not use
//static struct device_lcdc_timing_params lcdc_timing_params;
void tcc_hdmi_power_on(void);
void tcc_hdmi_power_off(void);

static unsigned int gHdmiSettingFlag = 0;


/*static */char tcc_hdmi_open_num;
#endif
struct HDMIVideoParameter gHdmiVideoParms;
static unsigned int gHdmiStartFlag=0;
static unsigned int gPixelLimit=0;
static unsigned int gAudioChNum=0;
static unsigned int gSampleFreq=0;
static unsigned int gOutPacket=0;



#define	PWR_STATUS_OFF		0
#define	PWR_STATUS_ON		1
typedef struct {
	int status;
}stpwrinfo;
static stpwrinfo gHdmiPwrInfo = {PWR_STATUS_OFF};

static const struct file_operations hdmi_fops =
{
    .owner          = THIS_MODULE,
    .open           = hdmi_open,
    .release        = hdmi_release,
    .read           = hdmi_read,
    .write          = hdmi_write,
    .unlocked_ioctl = hdmi_ioctl,
};

static struct miscdevice hdmi_misc_device =
{
    HDMI_MINOR,
    "hdmi",  //"HDMI",
    &hdmi_fops,
};

static struct device *pdev_hdmi;

static int hdmi_open(struct inode *inode, struct file *file)
{
    	dprintk(KERN_INFO "%s open_num:%d\n", __FUNCTION__, tcc_hdmi_open_num);
		tcc_hdmi_open_num++;

    	return 0;
}

static int hdmi_release(struct inode *inode, struct file *file)
{
    dprintk(KERN_INFO "%s\n", __FUNCTION__);

	tcc_hdmi_open_num--;

	return 0;
}

ssize_t hdmi_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos)
{
    return 0;
}

ssize_t hdmi_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    return 0;
}

int hdmi_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch (cmd)
    {
		case HDMI_IOC_GET_PWR_STATUS:
		{
			dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_GET_PWR_STATUS : %d )\n", gHdmiPwrInfo.status);

			put_user(gHdmiPwrInfo.status,(unsigned int __user*)arg);

			break;
		}
		case HDMI_IOC_SET_PWR_CONTROL:
		{
			unsigned int cmd;


			
			if (get_user(cmd, (unsigned int __user *) arg))
				return -EFAULT;

			dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_PWR_CONTROL :  %d )\n", cmd);
			if (cmd == 0)
				tcc_hdmi_power_off();
			else
				tcc_hdmi_power_on();
			break;
		}
        case HDMI_IOC_SET_COLORSPACE:
        {
            int space;

            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_COLORSPACE)\n");

            // get arg
            if (get_user(space, (int __user *) arg))
                return -EFAULT;

            if ( !hdmi_set_color_space(space) )
            {
                dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_COLORSPACE) : Not Correct Arg = %d\n", space);
                return -EFAULT;
            }

			gHdmiVideoParms.colorSpace = space;

            break;
        }
        case HDMI_IOC_SET_COLORDEPTH:
        {
            int depth;

            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_COLORDEPTH)\n");

            // get arg
            if (get_user(depth, (int __user *) arg))
                return -EFAULT;

            if ( !hdmi_set_color_depth(depth) )
            {
                dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_COLORDEPTH) : Not Correct Arg = %d\n", depth);
                return -EFAULT;
            }

			gHdmiVideoParms.colorDepth = depth;

            break;
        }
        case HDMI_IOC_SET_HDMIMODE:
        {
            int mode;

            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_HDMIMODE)\n");

            // get arg
            if (get_user(mode, (int __user *) arg))
	            return -EFAULT;

			hdmi_set_hdmimode(mode);
			
			gHdmiVideoParms.mode = mode;

            break;
        }
        case HDMI_IOC_SET_VIDEOMODE:
        {
            struct device_video_params video_mode;
            unsigned int ret;

            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_VIDEOMODE)\n");

            // get arg
            if ( (ret = copy_from_user((void*)&video_mode,(const void*)arg,sizeof(struct device_video_params))) < 0)
            {
                return -EFAULT;
            }
#ifdef HDMI_TX13_REV_05
            video_wrapper_set_mode(video_mode);
#endif

            hdmi_set_video_mode(video_mode);

            break;
        }
		
        case HDMI_IOC_SET_VIDEOFORMAT_INFO:
        {
            enum VideoFormat video_format;
            unsigned int ret;

            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_VIDEOFORMAT_INFO)\n");

            // get arg
            if ( (ret = copy_from_user((void*)&video_format,(const void*)arg,sizeof(enum VideoFormat))) < 0)
            {
                return -EFAULT;
            }

			gHdmiVideoParms.resolution = video_format;

            break;
        }

        case HDMI_IOC_GET_VIDEOCONFIG:
        {
            int ret;

            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_GET_VIDEOCONFIG)\n");

            // copy to user
            if ( (ret = copy_to_user((void*)arg,(const void*)&gHdmiVideoParms,sizeof(struct HDMIVideoParameter))) < 0)
                return -EFAULT;

            break;
        }

        case HDMI_IOC_GET_HDMISTART_STATUS:
        {
            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_GET_HDMISTART_STATUS)\n");

            put_user(gHdmiStartFlag,(unsigned int __user*)arg);

            break;
        }

#if defined(TELECHIPS)
		case HDMI_IOC_SET_LCDC_TIMING:
		{

			struct device_lcdc_timing_params lcdc_mode;
            unsigned int ret;

            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_LCDC_TIMING)\n");

            // get arg
			if ( (ret = copy_from_user((void*)&lcdc_mode,(const void*)arg,sizeof(struct device_lcdc_timing_params))) < 0)	{
			    return -EFAULT;
			}

			#if 0	// Not use sys3
			#if 1
            hdmi_set_lcdc_timing(lcdc_mode);
			#else
			memcpy(&lcdc_timing_params, &lcdc_mode, sizeof(struct device_lcdc_timing_params));
			#endif
			#endif//
            break;
		}
#endif /*TELECHIPS*/

        case HDMI_IOC_SET_BLUESCREEN:
        {
            unsigned char val,reg;

            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_BLUESCREEN)\n");

            // get arg
            if (get_user(val, (unsigned char __user *) arg))
                return -EFAULT;

            reg = readb(HDMI_CON_0);
            if (val) // if on
            {
                writeb(reg|HDMI_BLUE_SCR_ENABLE,HDMI_CON_0);
            }
            else // if off
            {
                writeb(reg &~HDMI_BLUE_SCR_ENABLE,HDMI_CON_0);
            }

            break;
        }
        case HDMI_IOC_SET_PIXEL_LIMIT:
        {
            int val;

            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_PIXEL_LIMIT)\n");

            // get arg
            if (get_user(val, (int __user *) arg))
                return -EFAULT;

            if (!hdmi_set_pixel_limit(val))
            {
                dprintk(KERN_INFO "Not available Arg\n");
                return -EFAULT;
            }

			gPixelLimit = val;

            break;
        }
        case HDMI_IOC_GET_PIXEL_LIMIT:
        {
            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_GET_PIXEL_LIMIT)\n");

            // put to user
            if (put_user(gPixelLimit, (int __user *) arg))
                return -EFAULT;

            break;
        }

        case HDMI_IOC_SET_PIXEL_ASPECT_RATIO:
        {
            int val;
            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_PIXEL_ASPECT_RATIO)\n");
            if (get_user(val, (int __user *) arg))
                return -EFAULT;

            if (!hdmi_set_pixel_aspect_ratio(val))
            {
                dprintk(KERN_INFO "Not available Arg\n");
                return -EFAULT;
            }

			gHdmiVideoParms.pixelAspectRatio = val;
			
            break;
        }
        case HDMI_IOC_SET_AVMUTE:
        {
            unsigned char val,reg;

            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_AVMUTE)\n");

            // get arg
            if (get_user(val, (unsigned int __user *) arg))
                return -EFAULT;

            reg = readb(HDMI_MODE_SEL) & HDMI_MODE_SEL_HDMI;
            if (reg)
            {
                if (val)
                {
                    // set AV Mute
                    writeb(GCP_AVMUTE_ON,HDMI_GCP_BYTE1);
                    writeb(GCP_TRANSMIT_EVERY_VSYNC,HDMI_GCP_CON);
                }
                else
                {
                    // clear AV Mute
                    writeb(GCP_AVMUTE_OFF, HDMI_GCP_BYTE1);
                    writeb(GCP_TRANSMIT_EVERY_VSYNC,HDMI_GCP_CON);
                }
            }

            break;
        }
        case HDMI_IOC_SET_AUDIOPACKETTYPE:
        {
            int val;

            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_AUDIOPACKETTYPE)\n");

            // get arg
            if (get_user(val, (int __user *) arg))
                return -EFAULT;

            if (!hdmi_set_audio_packet_type(val))
            {
                dprintk(KERN_INFO "Not available Arg\n");
                return -EFAULT;
            }

			gOutPacket = val;

            break;
        }
        case HDMI_IOC_GET_AUDIOPACKETTYPE:
        {
            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_GET_AUDIOPACKETTYPE)\n");

			put_user(gOutPacket,(unsigned int __user*)arg);

            break;
        }

        case HDMI_IOC_SET_AUDIOSAMPLEFREQ:
        {
            int val;
//            unsigned char reg = readb(HDMI_CON_0);
            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_AUDIOSAMPLEFREQ)\n");

            // get arg
            if (get_user(val, (int __user *) arg))
                return -EFAULT;

            if ( !hdmi_set_audio_sample_freq(val) )
            {
                dprintk(KERN_INFO "Not available Arg\n");
                return -EFAULT;
            }
#if 0
            // set audio enable
            writeb(reg|HDMI_ASP_ENABLE ,HDMI_CON_0);
#endif /* 0 */

			gSampleFreq = val;
			
            break;
        }
        case HDMI_IOC_GET_AUDIOSAMPLEFREQ:
        {
            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_GET_AUDIOSAMPLEFREQ)\n");

			put_user(gSampleFreq,(unsigned int __user*)arg);
			
            break;
        }

        case HDMI_IOC_SET_AUDIOCHANNEL:
        {
            int val;

            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_AUDIOCHANNEL)\n");

            // get arg
            if (get_user(val, (int __user *) arg))
                return -EFAULT;

            if (!hdmi_set_audio_channel_number(val))
            {
                dprintk(KERN_INFO "Not available Arg\n");
                return -EFAULT;
            }

			gAudioChNum = val;

            break;
        }
        case HDMI_IOC_GET_AUDIOCHANNEL:
        {
            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_GET_AUDIOCHANNEL)\n");

			put_user(gAudioChNum,(unsigned int __user*)arg);			

            break;
        }

        case HDMI_IOC_SET_SPEAKER_ALLOCATION:
        {
            unsigned int val;

            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_SPEAKER_ALLOCATION)\n");

            // get arg
            if (get_user(val, (unsigned int __user *) arg))
                return -EFAULT;

            writeb(val,HDMI_AUI_BYTE4);

            break;
        }
        case HDMI_IOC_GET_SPEAKER_ALLOCATION:
        {
            unsigned int val;

            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_GET_SPEAKER_ALLOCATION)\n");

            val = readb(HDMI_AUI_BYTE4);

            put_user(val,(unsigned int __user*)arg);

            break;
        }
		
        case HDMI_IOC_GET_PHYREADY:
        {
            unsigned char phy_status;

            //dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_GET_PHYREADY)\n");

            phy_status = readb(HDMI_PHY_STATUS);

            put_user(phy_status,(unsigned char __user*)arg);

            break;
        }

		case HDMI_IOC_SET_PHYRESET:
		{
			dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_PHYRESET)\n");
			hdmi_phy_reset();
			break;
		}
		
        case HDMI_IOC_START_HDMI:
        {
            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_START_HDMI)\n");

#ifdef HDMI_TX13_REV_05
            video_wrapper_enable(1);
#endif

            hdmi_start();

            break;
        }
        case HDMI_IOC_STOP_HDMI:
        {
            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_STOP_HDMI)\n");

			hdmi_stop();

            break;
        }
        case HDMI_IOC_SET_AUDIO_ENABLE:
        {
            unsigned char enable;
            unsigned char reg, mode;

            dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_AUDIO_ENABLE)\n");

            // get arg
            if (get_user(enable, (int __user *) arg))
                return -EFAULT;

			// check HDMI mode
			mode = readb(HDMI_MODE_SEL) & HDMI_MODE_SEL_HDMI;

            reg = readb(HDMI_CON_0);
            // enable audio output
            if ( enable && mode )
            {
#if (1) && defined(TELECHIPS)
                hdmi_aui_update_checksum();
                writeb(TRANSMIT_EVERY_VSYNC,HDMI_AUI_CON);
	        //  writeb(TRANSMIT_ONCE,HDMI_AUI_CON);
				writeb(ACR_MEASURED_CTS_MODE,HDMI_ACR_CON);
#endif
                writeb(reg|HDMI_ASP_ENABLE,HDMI_CON_0);

				dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_AUDIO_ENABLE) : enable\n");
            }
            else // disable encryption
            {
	            writeb(reg& ~HDMI_ASP_ENABLE,HDMI_CON_0);

#if (1) && defined(TELECHIPS)
                writeb(DO_NOT_TRANSMIT,HDMI_AUI_CON);
                writeb(DO_NOT_TRANSMIT,HDMI_ACR_CON);
#endif
				dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_SET_AUDIO_ENABLE) : disable\n");
            }

            break;
        }
        case HDMI_IOC_RESET_AUISAMPLEFREQ:
        {
            unsigned char reg = readb(HDMI_AUI_BYTE2) & ~HDMI_AUI_SF_MASK;
            writeb(reg, HDMI_AUI_BYTE2);
            break;
        }
		case HDMI_IOC_VIDEO_FORMAT_CONTROL:
		{
            struct HDMIVideoFormatCtrl video_format_ctrl;
            int ret;

            //dprintk(KERN_INFO "HDMI: ioctl(HDMI_IOC_VIDEO_FORMAT_CONTROL)\n");

            // get size arg;
            if ( (ret = copy_from_user((void*)&video_format_ctrl,(const void*)arg,sizeof(video_format_ctrl))) < 0)
                return -EFAULT;

			hdmi_set_spd_infoframe( video_format_ctrl );

			writeb(TRANSMIT_EVERY_VSYNC,HDMI_SPD_CON);

			break;
		}

        default:
            return -EINVAL;
    }

    return 0;
}

/**
 * HDCP IRQ handler. @n
 * If HDCP IRQ occurs, set hdcp_event and wake up the waitqueue.
 */
static irqreturn_t hdmi_handler(int irq, void *dev_id)
{
    unsigned char flag, status;
    unsigned int event = 0;
	unsigned int tmp_event = 0;

    // check HDCP INT
    flag = readb(HDMI_SS_INTC_FLAG);


	dprintk(KERN_INFO "%s : flag = %d\n", __func__, flag);
//    if ( !(flag & (1<<HDMI_IRQ_HDCP)))
//        return IRQ_NONE;
	if (flag & (1<<HDMI_IRQ_HDCP))
	{

	}
	else
	{
	        return IRQ_NONE;
	}

    return IRQ_HANDLED;
}

int hdmi_set_setting_flag(unsigned int hdmi_setting)
{
	gHdmiSettingFlag = hdmi_setting;

	return 1;
}

int hdmi_get_setting_flag(unsigned int *hdmi_setting)
{
	*hdmi_setting = gHdmiSettingFlag;

	return 1;
}



void tcc_usleep(unsigned int delay)
{
	if(delay < 1000)
		udelay(delay);
	else
		msleep(delay/1000);
}

// onoff : 1 - power down
void tcc_ddi_pwdn_hdmi(char onoff)
{
	unsigned int  regl;

	// HDMI Power-on
	regl = readl(DDICFG_PWDN);
    	#if defined (CONFIG_ARCH_TCC92XX) ||defined (CONFIG_ARCH_TCC93XX) || defined (CONFIG_ARCH_TCC88XX)	
		if(onoff)
			writel(regl | PWDN_HDMI, DDICFG_PWDN);	
		else
			writel(regl & ~PWDN_HDMI, DDICFG_PWDN);	
	#else
		if(onoff)
			writel(regl & ~PWDN_HDMI, DDICFG_PWDN);	
		else
			writel(regl | PWDN_HDMI, DDICFG_PWDN);	
	#endif//
}

// onoff : 1 -reset
void tcc_ddi_swreset_hdmi(char onoff)
{
	unsigned int  regl;

	regl = readl(DDICFG_SWRESET);

    	#if defined (CONFIG_ARCH_TCC92XX) ||defined (CONFIG_ARCH_TCC93XX) || defined (CONFIG_ARCH_TCC88XX)	
	if(onoff)
		writel(regl | SWRESET_HDMI, DDICFG_SWRESET);
	else
		writel(regl & ~SWRESET_HDMI, DDICFG_SWRESET);
	#else
	if(onoff)
		writel(regl & ~SWRESET_HDMI, DDICFG_SWRESET);
	else
		writel(regl | SWRESET_HDMI, DDICFG_SWRESET);	
	#endif//
}

// onoff : 1 -power on
void tcc_ddi_hdmi_ctrl(unsigned int index, char onoff)
{
	unsigned int regl;

	regl = readl(DDICFG_HDMICTRL);
	
	switch(index)
	{
		case HDMICTRL_RESET_HDMI:
		{
		    	#if 1//defined (CONFIG_ARCH_TCC92XX) ||defined (CONFIG_ARCH_TCC93XX) || defined (CONFIG_ARCH_TCC88XX)			
				if(onoff)
					writel(regl & ~HDMICTRL_RESET_HDMI, DDICFG_HDMICTRL);
				else
					writel(regl | HDMICTRL_RESET_HDMI, DDICFG_HDMICTRL);
			#else
				if(onoff)
					writel(regl | HDMICTRL_RESET_HDMI, DDICFG_HDMICTRL);
				else
					writel(regl & ~HDMICTRL_RESET_HDMI, DDICFG_HDMICTRL);
			#endif//			

		}
		break;

		case HDMICTRL_RESET_SPDIF:
		{
		    	#if 1//defined (CONFIG_ARCH_TCC92XX) ||defined (CONFIG_ARCH_TCC93XX) || defined (CONFIG_ARCH_TCC88XX)			
				if(onoff)
					writel(regl & ~HDMICTRL_RESET_SPDIF, DDICFG_HDMICTRL);
				else
					writel(regl | HDMICTRL_RESET_SPDIF, DDICFG_HDMICTRL);
			#else
				if(onoff)
					writel(regl | HDMICTRL_RESET_SPDIF, DDICFG_HDMICTRL);
				else
					writel(regl & ~HDMICTRL_RESET_SPDIF, DDICFG_HDMICTRL);
			#endif//		
		}
		break;
		
		case HDMICTRL_RESET_TMDS:
		{
		    	#if 1//defined (CONFIG_ARCH_TCC92XX) ||defined (CONFIG_ARCH_TCC93XX) || defined (CONFIG_ARCH_TCC88XX)			
				if(onoff)
					writel(regl & ~HDMICTRL_RESET_TMDS, DDICFG_HDMICTRL);
				else
					writel(regl | HDMICTRL_RESET_TMDS, DDICFG_HDMICTRL);
			#else
				if(onoff)
					writel(regl | HDMICTRL_RESET_TMDS, DDICFG_HDMICTRL);
				else
					writel(regl & ~HDMICTRL_RESET_TMDS, DDICFG_HDMICTRL);
			#endif//		
		}
		break;
		
		case HDMICTRL_HDMI_ENABLE:
		{
			if(onoff)
			{
				writel(regl | HDMICTRL_HDMI_ENABLE, DDICFG_HDMICTRL);
				
			}		
			else
			{
				writel(regl & ~HDMICTRL_HDMI_ENABLE, DDICFG_HDMICTRL);
			}


		}
		break;
	}
}
void tcc_hdmi_power_on(void)
{
	unsigned int  regl;

	struct tcc_hdmi_platform_data *hdmi_dev;

	dprintk(KERN_INFO "%s\n", __FUNCTION__);

	hdmi_dev = (struct tcc_hdmi_platform_data *)pdev_hdmi->platform_data;

	if(hdmi_dev->set_power != NULL)
		hdmi_dev->set_power(pdev_hdmi, TCC_HDMI_PWR_ON);

	if (hdmi_clk && (gHdmiPwrInfo.status == PWR_STATUS_OFF))
		clk_enable(hdmi_clk);

	tcc_usleep(100);
	#if defined(CONFIG_ARCH_TCC892X) //юс╫ц
	{
		PCKC				pCKC ;
		pCKC = (CKC *)tcc_p2v(HwCKC_BASE);

		if(hdmi_dev->hdmi_lcdc_num)
			pCKC->PCLKCTRL05.nREG = 0x2C000000;
		else
			pCKC->PCLKCTRL03.nREG = 0x2C000000;
	}
	#endif

	if (hdmi_clk)
		clk_set_rate(hdmi_clk, 1*1000*1000);
	
    #if defined (CONFIG_ARCH_TCC92XX)
	regl = readl(PMU_PWROFF);
	writel(regl & ~PWROFF_HDMIPHY, PMU_PWROFF);
    #elif defined (CONFIG_ARCH_TCC93XX) || defined (CONFIG_ARCH_TCC88XX)
	regl = readl(PMU_PWROFF);
	writel(regl | PWROFF_HDMIPHY, PMU_PWROFF);
	tcc_usleep(100);
	writel(regl & ~PWROFF_HDMIPHY, PMU_PWROFF);
	tcc_usleep(100);
	writel(regl | PWROFF_HDMIPHY, PMU_PWROFF);
    #elif defined(CONFIG_ARCH_TCC892X)
	tca_ckc_setippwdn(PMU_ISOL_HDMI, 1); // power down
	tcc_usleep(100);
	tca_ckc_setippwdn(PMU_ISOL_HDMI, 0); // power up
    #endif

	// HDMI Power-on
	tcc_ddi_pwdn_hdmi(0);
	
	// swreset DDI_BUS HDMI
	tcc_ddi_swreset_hdmi(1);
	{volatile int ttt;for(ttt=0;ttt<0x100;ttt++);}
	tcc_ddi_swreset_hdmi(0);


	// enable DDI_BUS HDMI CLK
	tcc_ddi_hdmi_ctrl(HDMICTRL_HDMI_ENABLE, 1);


	// HDMI PHY Reset
	hdmi_phy_reset();

	// HDMI SPDIF Reset
	tcc_ddi_hdmi_ctrl(HDMICTRL_RESET_SPDIF, 0);
	tcc_usleep(1);
	tcc_ddi_hdmi_ctrl(HDMICTRL_RESET_SPDIF, 1);

	// HDMI TMDS Reset
	tcc_ddi_hdmi_ctrl(HDMICTRL_RESET_TMDS, 0);
	tcc_usleep(1);
	tcc_ddi_hdmi_ctrl(HDMICTRL_RESET_TMDS, 1);


	// swreset DDI_BUS HDMI
	tcc_ddi_swreset_hdmi(1);
	tcc_usleep(1);
	tcc_ddi_swreset_hdmi(0);


	#if  defined (CONFIG_ARCH_TCC88XX )  || defined( CONFIG_ARCH_TCC92XX) || defined (CONFIG_ARCH_TCC93XX)
		// enable DDI_BUS HDMI CLK
		regl = readl(DDICFG_HDMICTRL);

		if(hdmi_dev->hdmi_lcdc_num)
			writel((regl&0xFFFF7FFF)  |HDMICTRL_PATH_LCDC1 ,DDICFG_HDMICTRL);
		else	
			writel((regl&0xFFFF7FFF)  |HDMICTRL_PATH_LCDC0 ,DDICFG_HDMICTRL);

	#elif defined(CONFIG_ARCH_TCC892X)
		if(hdmi_dev->hdmi_lcdc_num)
			VIOC_OUTCFG_SetOutConfig(VIOC_OUTCFG_HDMI, 1);
		else
			VIOC_OUTCFG_SetOutConfig(VIOC_OUTCFG_HDMI, 0);
	#endif//

	tcc_ddi_hdmi_ctrl(HDMICTRL_HDMI_ENABLE, 1);

	memset(&gHdmiVideoParms, 0, sizeof(struct HDMIVideoParameter));


	// default video mode setting.
	memset(&gHdmiVideoParms, 0, sizeof(struct HDMIVideoParameter));

	gHdmiVideoParms.mode = DVI;
	gHdmiVideoParms.resolution = max_video_formats;
	gHdmiVideoParms.colorSpace = HDMI_CS_RGB;
	gHdmiVideoParms.colorDepth = HDMI_CD_24;
	gHdmiVideoParms.colorimetry = HDMI_COLORIMETRY_NO_DATA;
	gHdmiVideoParms.pixelAspectRatio = HDMI_PIXEL_RATIO_4_3;

	hdmi_set_hdmimode(gHdmiVideoParms.mode);
	hdmi_set_color_space(gHdmiVideoParms.colorSpace);
	hdmi_set_color_depth(gHdmiVideoParms.colorDepth);
	hdmi_set_pixel_aspect_ratio(gHdmiVideoParms.pixelAspectRatio);

	gHdmiPwrInfo.status = PWR_STATUS_ON;

	// disable HDCP INT
	regl = readb(HDMI_SS_INTC_CON);
	writeb(regl & ~(1<<HDMI_IRQ_HDCP), HDMI_SS_INTC_CON);

	// disable SPDIF INT
	regl = readb(HDMI_SS_INTC_CON);
	writeb(regl & ~(1<<HDMI_IRQ_SPDIF), HDMI_SS_INTC_CON);

}



void tcc_hdmi_power_off(void)
{
	unsigned int  regl;
	struct tcc_hdmi_platform_data *hdmi_dev;

	dprintk(KERN_INFO "%s\n", __FUNCTION__);

	writeb(0x7F,HDCP_RI_COMPARE_1);


	// HDMI PHY Reset
	tcc_ddi_hdmi_ctrl(HDMICTRL_RESET_HDMI, 0);
	tcc_usleep(1);
	tcc_ddi_hdmi_ctrl(HDMICTRL_RESET_HDMI, 1);

	// HDMI SPDIF Reset
	tcc_ddi_hdmi_ctrl(HDMICTRL_RESET_SPDIF, 0);
	tcc_usleep(1);
	tcc_ddi_hdmi_ctrl(HDMICTRL_RESET_SPDIF, 1);

	// HDMI TMDS Reset
	tcc_ddi_hdmi_ctrl(HDMICTRL_RESET_TMDS, 0);
	tcc_usleep(1);
	tcc_ddi_hdmi_ctrl(HDMICTRL_RESET_TMDS, 1);

	// swreset DDI_BUS HDMI
	tcc_ddi_swreset_hdmi(1);
	tcc_usleep(1);
	tcc_ddi_swreset_hdmi(0);

	// disable DDI_BUS HDMI CLK
	tcc_ddi_hdmi_ctrl(HDMICTRL_HDMI_ENABLE, 0);

	// ddi hdmi power down
	tcc_ddi_pwdn_hdmi(1);

	// enable HDMI PHY Power-off
	#if defined (CONFIG_ARCH_TCC92X)
	regl = readl(PMU_PWROFF);
	writel(regl | PWROFF_HDMIPHY, PMU_PWROFF);
      #elif defined (CONFIG_ARCH_TCC93XX) || defined (CONFIG_ARCH_TCC88XX)
	regl = readl(PMU_PWROFF);
	writel(regl & ~PWROFF_HDMIPHY, PMU_PWROFF);
	#elif defined(CONFIG_ARCH_TCC892X)
	tca_ckc_setippwdn(PMU_ISOL_HDMI, 1); // power up
	#endif
	
	// gpio power on
	tcc_usleep(100);

	// enable HDMI Power-down
	if (hdmi_clk && (gHdmiPwrInfo.status == PWR_STATUS_ON))
		clk_disable(hdmi_clk);
	
	hdmi_dev = (struct tcc_hdmi_platform_data *)pdev_hdmi->platform_data;

	if(hdmi_dev->set_power != NULL)
		hdmi_dev->set_power(pdev_hdmi, TCC_HDMI_PWR_OFF);

	gHdmiPwrInfo.status = PWR_STATUS_OFF;
	memset(&gHdmiVideoParms, 0, sizeof(struct HDMIVideoParameter));
 }

extern unsigned int tca_get_output_lcdc_num(viod);

static int hdmi_probe(struct platform_device *pdev)
{
 	unsigned char reg;
	struct tcc_hdmi_platform_data *hdmi_dev;

	pdev_hdmi = &pdev->dev;
	
	hdmi_dev = (struct tcc_hdmi_platform_data *)pdev_hdmi->platform_data;
	hdmi_dev->hdmi_lcdc_num = tca_get_output_lcdc_num();
	
	if (hdmi_clk == NULL) {
		hdmi_clk = clk_get(0, "hdmi");
		if (IS_ERR(hdmi_clk)) {
			printk(KERN_WARNING "HDMI: failed to get hdmi clock\n");
			hdmi_clk = NULL;
			return -ENODEV;
		}
	}

	if (hdmi_clk)
		clk_enable(hdmi_clk);

    dprintk(KERN_INFO "%s\n", __FUNCTION__);

    if (!machine_is_hdmidp())
        return -ENODEV;

    printk(KERN_INFO "HDMI Driver ver. %s (built %s %s)\n", VERSION, __DATE__, __TIME__);


    if (misc_register(&hdmi_misc_device))
    {
        dprintk(KERN_WARNING "HDMI: Couldn't register device 10, %d.\n", HDMI_MINOR);
        return -EBUSY;
    }

    // disable HDCP INT
    reg = readb(HDMI_SS_INTC_CON);
    writeb(reg & ~(1<<HDMI_IRQ_HDCP), HDMI_SS_INTC_CON);

    if (request_irq(IRQ_HDMI, hdmi_handler, IRQF_SHARED, "hdmi", hdmi_handler))
    {
        dprintk(KERN_WARNING "HDMI: IRQ %d is not free.\n", IRQ_HDMI);
        misc_deregister(&hdmi_misc_device);
        return -EIO;
    }

	#ifndef CONFIG_OUTPUT_SKIP_KERNEL_LOGO
		if (hdmi_clk)
			clk_disable(hdmi_clk);


		if(hdmi_dev->set_power)
		{
			hdmi_dev->set_power(pdev_hdmi, TCC_HDMI_PWR_INIT);
			hdmi_dev->set_power(pdev_hdmi, TCC_HDMI_PWR_ON);
			hdmi_dev->set_power(pdev_hdmi, TCC_HDMI_PWR_OFF);
		}
		else
		{
			printk("no have hdmi power control");
		}
	#endif

	return 0;
}

static int hdmi_remove(struct platform_device *pdev)
{
	unsigned char reg;

	dprintk(KERN_INFO "%s\n", __FUNCTION__);

    // disable HDCP INT
    reg = readb(HDMI_SS_INTC_CON);
    writeb(reg & ~(1<<HDMI_IRQ_HDCP), HDMI_SS_INTC_CON);

    // disable hdmi
    reg = readb(HDMI_CON_0);
    writeb(reg & ~HDMI_SYS_ENABLE,HDMI_CON_0);


    free_irq(IRQ_HDMI, hdmi_handler);
	gHdmiPwrInfo.status = PWR_STATUS_OFF;
	
    misc_deregister(&hdmi_misc_device);

	return 0;
	
}


/**
 * Set checksum in SPD InfoFrame Packet. @n
 * Calculate a checksum and set it in packet.
 */
void hdmi_spd_update_checksum(void)
{
    unsigned char index, checksum;

    checksum = SPD_HEADER;

    for (index = 0; index < SPD_PACKET_BYTE_LENGTH; index++)
    {
        checksum += readb(HDMI_SPD_DATA1 + 4*index);
    }

    writeb(~checksum+1,HDMI_SPD_CHECK_SUM);
}

/**
 * Set checksum in Audio InfoFrame Packet. @n
 * Calculate a checksum and set it in packet.
 */
void hdmi_aui_update_checksum(void)
{
    unsigned char index, checksum;

    checksum = AUI_HEADER;
    for (index = 0; index < AUI_PACKET_BYTE_LENGTH; index++)
    {
#if 1
        // when write this byte(PB5), HW shift 3 bit to right direction.
        // to compensate it, when read it, SW should shift 3 bit to left.
        if (index == 4)
            checksum += (readb(HDMI_AUI_BYTE1 + 4*index)<<3);
        else
            checksum += readb(HDMI_AUI_BYTE1 + 4*index);
#else
        checksum += readb(HDMI_AUI_BYTE1 + 4*index);
#endif
    }
	
    writeb(~checksum+1,HDMI_AUI_CHECK_SUM);
}

/**
 * Set checksum in AVI InfoFrame Packet. @n
 * Calculate a checksum and set it in packet.
 */
void hdmi_avi_update_checksum(void)
{
    unsigned char index, checksum;

    checksum = AVI_HEADER;
    for (index = 0; index < AVI_PACKET_BYTE_LENGTH; index++)
    {
        checksum += readb(HDMI_AVI_BYTE1 + 4*index);
    }
    writeb(~checksum+1,HDMI_AVI_CHECK_SUM);
}

/**
 * Set color space in HDMI H/W. @n
 * @param   space   [in] Color space
 * @return  If argument is invalid, return 0;Otherwise return 1.
 */
int hdmi_set_color_space(enum ColorSpace space)
{
    unsigned char reg,aviYY;
    int ret = 1;

    reg = readb(HDMI_CON_0);
    aviYY = readb(HDMI_AVI_BYTE1);
    // clear fields
    writeb(aviYY & ~(AVI_CS_Y422|AVI_CS_Y444),HDMI_AVI_BYTE1);

    if (space == HDMI_CS_YCBCR422)
    {
        // set video input interface
        writeb( reg | HDMI_YCBCR422_ENABLE, HDMI_CON_0);
        // set avi
        writeb( aviYY | AVI_CS_Y422, HDMI_AVI_BYTE1);
    }
    else
    {
        // set video input interface
        writeb( reg & ~HDMI_YCBCR422_ENABLE, HDMI_CON_0);
        if (space == HDMI_CS_YCBCR444)
        {
            // set AVI packet
            writeb( aviYY | AVI_CS_Y444, HDMI_AVI_BYTE1);
        }
        // aviYY for RGB = 0, nothing to set
        else if (space != HDMI_CS_RGB)
        {
            ret = 0;
        }
    }

    return ret;
}

/**
 * Set color depth.@n
 * @param   depth   [in] Color depth of input vieo stream
 * @return  If argument is invalid, return 0;Otherwise return 1.
 */
int hdmi_set_color_depth(enum ColorDepth depth)
{
    int ret = 1;
    switch (depth)
    {
        case HDMI_CD_36:
        {
            // set GCP CD
            writeb(GCP_CD_36BPP,HDMI_GCP_BYTE2);
            // set DC_CTRL
            writeb(HDMI_DC_CTL_12,HDMI_DC_CONTROL);
            break;
        }
        case HDMI_CD_30:
        {
            // set GCP CD
            writeb(GCP_CD_30BPP,HDMI_GCP_BYTE2);
            // set DC_CTRL
            writeb(HDMI_DC_CTL_10,HDMI_DC_CONTROL);
            break;
        }
        case HDMI_CD_24:
        {
            // set GCP CD
            writeb(GCP_CD_24BPP,HDMI_GCP_BYTE2);
            // set DC_CTRL
            writeb(HDMI_DC_CTL_8,HDMI_DC_CONTROL);
            break;
        }

        default:
        {
            ret = 0;
        }
    }
    return ret;
}

/**
 * Set video timing parameters.@n
 * @param   mode   [in] Video timing parameters
 */
void hdmi_set_video_mode(struct device_video_params mode)
{
    unsigned char reg;
    unsigned int  val;

    // set HBLANK;
    val = mode.HBlank;
    reg = val & 0xff;
    writeb(reg,HDMI_H_BLANK_0);
    reg = (val>>8) & 0xff;
    writeb(reg,HDMI_H_BLANK_1);

    // set VBlank
    val = mode.VBlank;
    reg = val & 0xff;
    writeb(reg, HDMI_V_BLANK_0);
    reg = (val>>8) & 0xff;
    writeb(reg, HDMI_V_BLANK_1);
    reg = (val>>16) & 0xff;
    writeb(reg, HDMI_V_BLANK_2);

    // set HVLine
    val = mode.HVLine;
    reg = val & 0xff;
    writeb(reg, HDMI_H_V_LINE_0);
    reg = (val>>8) & 0xff;
    writeb(reg, HDMI_H_V_LINE_1);
    reg = (val>>16) & 0xff;
    writeb(reg, HDMI_H_V_LINE_2);

    // set VSync Polarity
    writeb(mode.polarity, HDMI_VSYNC_POL);

    // set HSyncGen
    val = mode.HSYNCGEN;
    reg = val & 0xff;
    writeb(reg, HDMI_H_SYNC_GEN_0);
    reg = (val>>8) & 0xff;
    writeb(reg, HDMI_H_SYNC_GEN_1);
    reg = (val>>16) & 0xff;
    writeb(reg, HDMI_H_SYNC_GEN_2);

    // set VSyncGen1
    val = mode.VSYNCGEN;
    reg = val & 0xff;
    writeb(reg, HDMI_V_SYNC_GEN1_0);
    reg = (val>>8) & 0xff;
    writeb(reg, HDMI_V_SYNC_GEN1_1);
    reg = (val>>16) & 0xff;
    writeb(reg, HDMI_V_SYNC_GEN1_2);

    // set interlace or progresive mode
    writeb(mode.interlaced,HDMI_INT_PRO_MODE);

    if ( mode.interlaced ) // interlaced mode
    {
        // set VBlank_F
        val = mode.VBLANK_F;
        reg = val & 0xff;
        writeb(reg, HDMI_V_BLANK_F_0);
        reg = (val>>8) & 0xff;
        writeb(reg, HDMI_V_BLANK_F_1);
        reg = (val>>16) & 0xff;
        writeb(reg, HDMI_V_BLANK_F_2);

        // set VSyncGen2
        val = mode.VSYNCGEN2;
        reg = val & 0xff;
        writeb(reg, HDMI_V_SYNC_GEN2_0);
        reg = (val>>8) & 0xff;
        writeb(reg, HDMI_V_SYNC_GEN2_1);
        reg = (val>>16) & 0xff;
        writeb(reg, HDMI_V_SYNC_GEN2_2);

        // set VSyncGen3
        val = mode.VSYNCGEN3;
        reg = val & 0xff;
        writeb(reg, HDMI_V_SYNC_GEN3_0);
        reg = (val>>8) & 0xff;
        writeb(reg, HDMI_V_SYNC_GEN3_1);
        reg = (val>>16) & 0xff;
        writeb(reg, HDMI_V_SYNC_GEN3_2);
    }
    else
    {
        // set VBlank_F with default value
        writeb(0x00, HDMI_V_BLANK_F_0);
        writeb(0x00, HDMI_V_BLANK_F_1);
        writeb(0x00, HDMI_V_BLANK_F_2);

        // set VSyncGen2 with default value
        writeb(0x01, HDMI_V_SYNC_GEN2_0);
        writeb(0x10, HDMI_V_SYNC_GEN2_1);
        writeb(0x00, HDMI_V_SYNC_GEN2_2);

        // set VSyncGen3 with default value
        writeb(0x01, HDMI_V_SYNC_GEN3_0);
        writeb(0x10, HDMI_V_SYNC_GEN3_1);
        writeb(0x00, HDMI_V_SYNC_GEN3_2);
    }

    // set pixel repetition
    reg = readb(HDMI_CON_1);
    if ( mode.repetition )
    {
        // set pixel repetition
        writeb(reg|HDMICON1_DOUBLE_PIXEL_REPETITION,HDMI_CON_1);
        // set avi packet
        writeb(AVI_PIXEL_REPETITION_DOUBLE,HDMI_AVI_BYTE5);
    }
    else
    {
        // clear pixel repetition
        writeb(reg & ~(1<<1|1<<0),HDMI_CON_1);
        // set avi packet
        writeb(0x00,HDMI_AVI_BYTE5);
    }

    // set AVI packet with VIC
	reg = readb(HDMI_AVI_BYTE2);

	if (reg & (unsigned char)AVI_PICTURE_ASPECT_RATIO_4_3)
		writeb(mode.AVI_VIC,HDMI_AVI_BYTE4);
	else
		writeb(mode.AVI_VIC_16_9,HDMI_AVI_BYTE4);
    return;
}

/**
 * Set pixel limitation.
 * @param   limit   [in] Pixel limitation.
* @return  If argument is invalid, return 0;Otherwise return 1.
 */
int hdmi_set_pixel_limit(enum PixelLimit limit)
{
    int ret = 1;
    unsigned char reg,aviQQ;

    // clear field
    reg = readb(HDMI_CON_1);
    reg &= ~HDMICON1_LIMIT_MASK;

    aviQQ = readb(HDMI_AVI_BYTE3);
    aviQQ &= ~AVI_QUANTIZATION_MASK;

    switch (limit) // full
    {
        case HDMI_FULL_RANGE:
        {
            aviQQ |= AVI_QUANTIZATION_FULL;
            break;
        }
        case HDMI_RGB_LIMIT_RANGE:
        {
            reg |= HDMICON1_RGB_LIMIT;
            aviQQ |= AVI_QUANTIZATION_LIMITED;
            break;
        }
        case HDMI_YCBCR_LIMIT_RANGE:
        {
            reg |= HDMICON1_YCBCR_LIMIT;
            aviQQ |= AVI_QUANTIZATION_LIMITED;
            break;
        }
        default:
        {
            ret = 0;
        }
    }
    // set pixel repetition
    writeb(reg,HDMI_CON_1);
    // set avi packet body
    writeb(aviQQ,HDMI_AVI_BYTE3);

    return ret;
}

/**
 * Set pixel aspect ratio information in AVI InfoFrame
 * @param   ratio   [in] Pixel Aspect Ratio
 * @return  If argument is invalid, return 0;Otherwise return 1.
 */
int hdmi_set_pixel_aspect_ratio(enum PixelAspectRatio ratio)
{
    int ret = 1;
    unsigned char reg = AVI_FORMAT_ASPECT_AS_PICTURE;

    switch (ratio)
    {
        case HDMI_PIXEL_RATIO_16_9:
            reg |= AVI_PICTURE_ASPECT_RATIO_16_9;
            break;
        case HDMI_PIXEL_RATIO_4_3:
            reg |= AVI_PICTURE_ASPECT_RATIO_4_3;
            break;
        default:
            ret = 0;
     }
    writeb(reg,HDMI_AVI_BYTE2);
    return ret;
}

/**
 * Set HDMI/DVI mode
 * @param   mode   [in] HDMI/DVI mode
 * @return  If argument is invalid, return 0;Otherwise return 1.
 */
int hdmi_set_hdmimode(int mode)
{
	int ret = 1;

	switch(mode)
	{
		case HDMI:
	        writeb(HDMI_MODE_SEL_HDMI,HDMI_MODE_SEL);
	        writeb(HDMICON2_HDMI,HDMI_CON_2);
			break;
		case DVI:
	        writeb(HDMI_MODE_SEL_DVI,HDMI_MODE_SEL);
	        writeb(HDMICON2_DVI,HDMI_CON_2);
			break;
		default:
			ret = 0;
			break;
	}

	return ret;
}

/**
 * Set Audio Clock Recovery and Audio Infoframe packet -@n
 * based on sampling frequency.
 * @param   freq   [in] Sampling frequency
 * @return  If argument is invalid, return 0;Otherwise return 1.
 */
int hdmi_set_audio_sample_freq(enum SamplingFreq freq)
{
    unsigned char reg;
    unsigned int n;
    int ret = 1;

    // check param
    if ( freq > sizeof(ACR_N_params)/sizeof(unsigned int) || freq < 0 )
        return 0;

    // set ACR packet
    // set N value
    n = ACR_N_params[freq];
    reg = n & 0xff;
    writeb(reg,HDMI_ACR_N0);
    reg = (n>>8) & 0xff;
    writeb(reg,HDMI_ACR_N1);
    reg = (n>>16) & 0xff;
    writeb(reg,HDMI_ACR_N2);

#if 0
    // set as measure cts mode
    writeb(ACR_MEASURED_CTS_MODE,HDMI_ACR_CON);
#endif /* 0 */

    // set AUI packet
    reg = readb(HDMI_AUI_BYTE2) & ~HDMI_AUI_SF_MASK;

    switch (freq)
    {
        case SF_32KHZ:
            reg |= HDMI_AUI_SF_SF_32KHZ;
            break;

        case SF_44KHZ:
            reg |= HDMI_AUI_SF_SF_44KHZ;
            break;

        case SF_88KHZ:
            reg |= HDMI_AUI_SF_SF_88KHZ;
            break;

        case SF_176KHZ:
            reg |= HDMI_AUI_SF_SF_176KHZ;
            break;

        case SF_48KHZ:
            reg |= HDMI_AUI_SF_SF_48KHZ;
            break;

        case SF_96KHZ:
            reg |= HDMI_AUI_SF_SF_96KHZ;
            break;

        case SF_192KHZ:
            reg |= HDMI_AUI_SF_SF_192KHZ;
            break;

        default:
            ret = 0;
    }

    writeb(reg, HDMI_AUI_BYTE2);

    return ret;
}

/**
 * Set HDMI audio output packet type.
 * @param   packet   [in] Audio packet type
 * @return  If argument is invalid, return 0;Otherwise return 1.
 */
int hdmi_set_audio_packet_type(enum HDMIASPType packet)
{
    int ret = 1;
    unsigned char reg;

    reg = readb(HDMI_ASP_CON);
    reg &= ~ASP_TYPE_MASK;

    switch (packet)
    {
        case HDMI_ASP:
        {
            reg |= ASP_LPCM_TYPE;
            break;
        }
        case HDMI_DSD:
        {
            reg |= ASP_DSD_TYPE;
            break;
        }
        case HDMI_HBR:
        {
            unsigned char regb = readb(HDMI_SS_I2S_CH_ST_3) & ~I2S_CH_ST_3_SF_MASK;
            regb |= I2S_CH_ST_3_SF_768KHZ;
            writeb(regb, HDMI_SS_I2S_CH_ST_3);

#if 0
            // update CUV
            writeb(0x01, HDMI_SS_I2S_CH_ST_CON);
#endif /* 0 */

            reg |= ASP_HBR_TYPE;
            break;
        }
        case HDMI_DST:
        {
            reg |= ASP_DST_TYPE;
            break;
        }
        default:
            ret = 0;
    }
    writeb(reg,HDMI_ASP_CON);
    return ret;
}

/**
 * Set layout and sample present fields in Audio Sample Packet -@n
 * and channel number field in Audio InfoFrame packet.
 * @param   channel   [in]  Number of channels
 * @return  If argument is invalid, return 0;Otherwise return 1.
 */
int hdmi_set_audio_channel_number(enum ChannelNum channel)
{
    int ret = 1;
    unsigned char reg;
    unsigned char reg_byte4;

    reg = readb(HDMI_ASP_CON);
    // clear field
    reg &= ~(ASP_MODE_MASK|ASP_SP_MASK);

    // celar field
    reg_byte4 = 0;

    // set layout & SP_PRESENT on ASP_CON
    // set AUI Packet
    switch (channel)
    {
        case CH_2:
            reg |= (ASP_LAYOUT_0|ASP_SP_0);
            writeb(AUI_CC_2CH,HDMI_AUI_BYTE1);
            break;
        case CH_3:
            reg |= (ASP_LAYOUT_1|ASP_SP_0|ASP_SP_1);
            writeb(AUI_CC_3CH,HDMI_AUI_BYTE1);
            break;
        case CH_4:
            reg |= (ASP_LAYOUT_1|ASP_SP_0|ASP_SP_1);
            writeb(AUI_CC_4CH,HDMI_AUI_BYTE1);
            break;
        case CH_5:
            reg_byte4 = 0x0A;
            reg |= (ASP_LAYOUT_1|ASP_SP_0|ASP_SP_1|ASP_SP_2);
            writeb(AUI_CC_5CH,HDMI_AUI_BYTE1);
            break;
        case CH_6:
            reg_byte4 = 0x0A;
            reg |= (ASP_LAYOUT_1|ASP_SP_0|ASP_SP_1|ASP_SP_2);
            writeb(AUI_CC_6CH,HDMI_AUI_BYTE1);
            break;
        case CH_7:
            reg_byte4 = 0x12;
            reg |= (ASP_LAYOUT_1|ASP_SP_0|ASP_SP_1|ASP_SP_2|ASP_SP_3);
            writeb(AUI_CC_7CH,HDMI_AUI_BYTE1);
            break;
        case CH_8:
            reg_byte4 = 0x12;
            reg |= (ASP_LAYOUT_1|ASP_SP_0|ASP_SP_1|ASP_SP_2|ASP_SP_3);
            writeb(AUI_CC_8CH,HDMI_AUI_BYTE1);
            break;
        default:
            ret = 0;
    }

    writeb(reg_byte4, HDMI_AUI_BYTE4);
    writeb(reg,HDMI_ASP_CON);
    return ret;
}

int hdmi_set_spd_infoframe(struct HDMIVideoFormatCtrl VideoFormatCtrl)
{
	//SPD INFOFRAME PACKET HEADER
	writeb(SPD_PACKET_TYPE,HDMI_SPD_HEADER0);
	writeb(SPD_PACKET_VERSION,HDMI_SPD_HEADER1);
	writeb(SPD_PACKET_BYTE_LENGTH,HDMI_SPD_HEADER2);

	//SPD INFOFRAME PACKET CONTENTS
	writeb(SPD_PACKET_ID0,HDMI_SPD_DATA1);
	writeb(SPD_PACKET_ID1,HDMI_SPD_DATA2);
	writeb(SPD_PACKET_ID2,HDMI_SPD_DATA3);

	switch(VideoFormatCtrl.video_format)
	{
		case HDMI_2D:
			writeb((SPD_HDMI_VIDEO_FORMAT_NONE << 5),HDMI_SPD_DATA4);
			break;
		case HDMI_VIC:
			writeb((SPD_HDMI_VIDEO_FORMAT_VIC << 5),HDMI_SPD_DATA4);
			break;
		case HDMI_3D:
			writeb((SPD_HDMI_VIDEO_FORMAT_3D << 5),HDMI_SPD_DATA4);
			break;
		default:
			break;
	}

	if(VideoFormatCtrl.video_format == HDMI_3D)
	{
		switch(VideoFormatCtrl.structure_3D)
		{
			case FRAME_PACKING:
				writeb((SPD_3D_STRUCT_FRAME_PACKING << 4),HDMI_SPD_DATA5);
				break;
			case TOP_AND_BOTTOM:
				writeb((SPD_3D_STRUCT_TOP_AND_BOTTOM << 4),HDMI_SPD_DATA5);
				break;
			case SIDE_BY_SIDE:
				writeb((SPD_3D_STRUCT_SIDE_BY_SIDE << 4),HDMI_SPD_DATA5);
				break;
		}

		if(VideoFormatCtrl.ext_data_3D)
			writeb(VideoFormatCtrl.ext_data_3D << 4,HDMI_SPD_DATA5);
	}
	else
	{
		writeb(0,HDMI_SPD_DATA5);
		writeb(0,HDMI_SPD_DATA6);
		writeb(0,HDMI_SPD_DATA7);
	}
	
	hdmi_spd_update_checksum();

	return 1;
}

/**
 * hdmi_phy_reset.
 */
void hdmi_phy_reset(void)
{
	unsigned int  regl;
	unsigned char phy_status;
	unsigned int phy_chk_cnt = 0;

	// HDMI PHY Reset
	tcc_ddi_hdmi_ctrl(HDMICTRL_RESET_HDMI, 0);
	tcc_usleep(100); // TCC93xx 25us, tcc8900 5us
	tcc_ddi_hdmi_ctrl(HDMICTRL_RESET_HDMI, 1);

	do
	{
		phy_status = readb(HDMI_PHY_STATUS);
		if(phy_chk_cnt++ == 200)
			break;

		tcc_usleep(10);
	}while(!phy_status);

	if(phy_status)
		printk(KERN_INFO "%s phy is ready : 10us * %d\n", __FUNCTION__, phy_chk_cnt);
	else
	{
		printk(KERN_INFO "%s try phy reset again \n", __FUNCTION__);

		// HDMI PHY Reset
		tcc_ddi_hdmi_ctrl(HDMICTRL_RESET_HDMI, 0);
		tcc_usleep(100); // TCC93xx 25us, tcc8900 5us
		tcc_ddi_hdmi_ctrl(HDMICTRL_RESET_HDMI, 1);

		phy_chk_cnt = 0;

		do
		{
			phy_status = readb(HDMI_PHY_STATUS);
			if(phy_chk_cnt++ == 200)
				break;

			tcc_usleep(10);
		}while(!phy_status);

		if(phy_status)
			printk(KERN_INFO "%s phy is ready : 10us * %d\n", __FUNCTION__, phy_chk_cnt);
		else
			printk(KERN_INFO "%s phy is not ready\n", __FUNCTION__);
	}
}

/**
 * Enable HDMI output.
 */
void hdmi_start(void)
{
    unsigned char reg,mode;

    // check HDMI mode
    mode = readb(HDMI_MODE_SEL) & HDMI_MODE_SEL_HDMI;
    reg = readb(HDMI_CON_0);

    // enable external vido gen.
    writeb(HDMI_EXTERNAL_VIDEO,HDMI_VIDEO_PATTERN_GEN);

    if (mode) // HDMI
    {
        // enable AVI packet: mandatory
        // update avi packet checksum
        hdmi_avi_update_checksum();

        // enable avi packet
        writeb(TRANSMIT_EVERY_VSYNC,HDMI_AVI_CON);

        // check if it is deep color mode or not
        if (readb(HDMI_DC_CONTROL))
        {
            // enable gcp
            writeb(GCP_TRANSMIT_EVERY_VSYNC,HDMI_GCP_CON);
        }
        // enable hdmi
		#if defined(TELECHIPS)
		writeb(reg|HDMI_SYS_ENABLE,HDMI_CON_0);
		#else
        writeb(reg|HDMI_SYS_ENABLE|HDMI_ENCODING_OPTION_ENABLE, HDMI_CON_0);
		#endif
    }
    else // DVI
    {
        // disable all packet
        writeb(DO_NOT_TRANSMIT,HDMI_AUI_CON);
        writeb(DO_NOT_TRANSMIT,HDMI_AVI_CON);
        writeb(DO_NOT_TRANSMIT,HDMI_GCP_CON);
		writeb(DO_NOT_TRANSMIT,HDMI_SPD_CON);

        // enable hdmi without audio
        reg &= ~HDMI_ASP_ENABLE;
		#if defined(TELECHIPS)
		writeb(reg|HDMI_SYS_ENABLE,HDMI_CON_0);
		#else
        writeb(reg|HDMI_SYS_ENABLE|HDMI_ENCODING_OPTION_ENABLE,HDMI_CON_0);
		#endif
    }

	gHdmiStartFlag = 1;

    return;
}

void hdmi_stop(void)
{
    unsigned char reg;

    reg = readb(HDMI_CON_0);
    writeb(reg & ~HDMI_SYS_ENABLE,HDMI_CON_0);
#ifdef HDMI_TX13_REV_05
    video_wrapper_enable(0);
#endif

	gHdmiStartFlag = 0;
}

int hdmi_suspend(struct platform_device *dev, pm_message_t state)
{
	gHdmiPwrInfo.status = PWR_STATUS_OFF ;
	
	printk(KERN_INFO "%s  state:%d \n", __FUNCTION__, state);

	return 0;
}

//extern unsigned int do_hibernation;
int hdmi_resume(struct platform_device *dev)
{

	//printk(KERN_INFO "%s do_hibernation:%d \n", __FUNCTION__, do_hibernation);

	return 0;
}


static struct platform_driver tcc_hdmi = {
	.probe	= hdmi_probe,
	.remove	= hdmi_remove,
	.suspend = hdmi_suspend,
	.resume = hdmi_resume,
	.driver	= {
		.name	= "tcc_hdmi",
		.owner	= THIS_MODULE,
	},
};

static __init int hdmi_init(void)
{
	return platform_driver_register(&tcc_hdmi);
}

static __exit void hdmi_exit(void)
{
	platform_driver_unregister(&tcc_hdmi);
}

module_init(hdmi_init);
module_exit(hdmi_exit);

MODULE_AUTHOR("Telechips Inc. <linux@telechips.com>");
MODULE_LICENSE("GPL");
