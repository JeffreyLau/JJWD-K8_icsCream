/***************************************************************************************
*	FileName    : pca953x.c
*	Description : GPIO_EXPAND driver
****************************************************************************************
*
*	TCC Board Support Package
*	Copyright (c) Telechips, Inc.
*	ALL RIGHTS RESERVED
*
****************************************************************************************/

//#include <common.h>
//#include <dev/i2c.h>
#include <platform/reg_physical.h>
#include <lcd.h>
#include <i2c.h>

#if defined(_TCC9200_) || defined(_TCC9201_)
//#ifdef CONFIG_I2C
unsigned char pca9539u3_data[2];

void pca953x_init(void)
{
	unsigned int i2c_ch = I2C_CH0;
	unsigned char DestAddress;
	unsigned char i2cData_mode[3] = {0,0,0};
	unsigned char i2cData_data[3] = {0,0,0};

    /* set pca9539 U2 */
	DestAddress = 0xEE; 				// PCA9539_U2_SLAVE_ADDR
	i2cData_mode[0] = 6;				// PCA9539_DIRECTION_0
	i2cData_data[0] = 2;				// PCA9539_OUTPUT_0
	i2cData_data[1] = 0;				// init low
	i2cData_data[2] = 0;				// init low

	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);

    /* set pca9539 U3 */
	DestAddress = 0xE8; 				// PCA9539_U3_SLAVE_ADDR
	i2cData_mode[0] = 6;				// PCA9539_DIRECTION_0
	i2cData_data[0] = 2;				// PCA9539_OUTPUT_0

	pca9539u3_data[0] = 0;				// init low
	pca9539u3_data[1] = 0;				// init low

	i2cData_data[1] = pca9539u3_data[0];
	i2cData_data[2] = pca9539u3_data[1];

	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);

    /* set pca9538 U4 */
	DestAddress = 0xE0;					// PCA9538_U4_SLAVE_ADDR
	i2cData_mode[0] = 3;				// PCA9538_DIRECTION_0
	i2cData_data[0] = 1;				// PCA9538_OUTPUT_0
	i2cData_data[1] = 0;				// init low

	i2c_xfer(DestAddress, 2, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 2, i2cData_mode, 0, 0, i2c_ch);

	// (delay)
	{volatile int ttt;for(ttt=0;ttt<1000;ttt++);}

    /* set pca9539 U2 */
	DestAddress = 0xEE; 				// PCA9539_U2_SLAVE_ADDR
	i2cData_mode[0] = 6;				// PCA9539_DIRECTION_0
	i2cData_data[0] = 2;				// PCA9539_OUTPUT_0
	i2cData_data[1] = 0
					| Hw14				// MUTE_CTL
	#if defined(BOOT_USE_HDMI)
					| Hw7				// HDMI_ON
	#endif
					| 0;				// init low
	i2cData_data[2] = 0;				// init low

//@HDMI_CEC Board
#if (0)
	#if defined(BOOT_USE_LCD)
	i2cData_data[2] = Hw13>>8;			// LCD_BL_EN
	#endif
#endif


	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);

    /* set pca9539 U3 */
	DestAddress = 0xE8; 				// PCA9539_U3_SLAVE_ADDR
	i2cData_mode[0] = 6;				// PCA9539_DIRECTION_0
	i2cData_data[0] = 2;				// PCA9539_OUTPUT_0

	pca9539u3_data[0] = 0				// init low
	#if defined(BOOT_USE_LCD)
					| Hw1				// LCD_ON
	#endif
	#if defined(BOOT_USE_LVDS)
					| Hw2				// LVDSIVT High
	#endif
					| Hw3				// CAM_ON
                    | Hw4				// CODEC_ON
					| Hw5				// FMTC_ON
	//#if defined(BOOTSD_INCLUDE)
	//				| Hw6 | Hw7			// SD0_ON, SD1_ON for TRIFLASH boot -> sd lib
	//#endif
					| 0;

	pca9539u3_data[1] = 0				// init low
					| Hw1				// CAS_ON
	#if defined(BOOT_USE_LVDS)
					| Hw15 >> 8			// LVDS_LP_CTRL High
	#endif
	#if defined(BOOT_USE_HDMI)
					| Hw14 >> 8			// PWR_GP4
	#endif	
					| 0;

	i2cData_data[1] = pca9539u3_data[0];
	i2cData_data[2] = pca9539u3_data[1];

	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);

    /* set pca9538 U4 */
	DestAddress = 0xE0;					// PCA9538_U4_SLAVE_ADDR
	i2cData_mode[0] = 3;				// PCA9538_DIRECTION_0
	i2cData_data[0] = 1;				// PCA9538_OUTPUT_0
	i2cData_data[1] = 0					// init low
	#if defined(BOOT_USE_LVDS) || defined(BOOT_USE_HDMI)
					| Hw2				// HDMI_LVDS_ON
	#endif
					| 0;

	i2c_xfer(DestAddress, 2, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 2, i2cData_mode, 0, 0, i2c_ch);
}

#elif defined(_TCC9300_)
#if defined(BOARD_TCC930X_CM)
void pca953x_init(void)
{
	unsigned int i2c_ch = I2C_CORE2_CH0;
	unsigned char DestAddress;
	unsigned char i2cData_data[3] = {0,0,0};

// set pca9539 U2 ----------------------------------------------

	DestAddress = 0xEE;
	i2cData_data[0] = 6; // config reg. (for port direction)
	i2cData_data[1] = 0x00;
	i2cData_data[2] = 0x0F;

	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);

	i2cData_data[0] = 2; // set output port
	i2cData_data[1] = 0
	#if defined(BOOT_USE_HDMI)
					| (1<<7) // HDMI_ON
	#endif
					| 0;
	i2cData_data[2] = 0
					| (1<<6); // MUTE_CTL
	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);

// set pca9539 U3 ----------------------------------------------

	DestAddress = 0xE8;
	i2cData_data[0] = 6; // config reg. (for port direction)
	i2cData_data[1] = 0; // io 0. output all
	i2cData_data[2] = 0; // io 1. output all

	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);

	i2cData_data[0] = 2; // set output port
	i2cData_data[1] = 0
					| (1<<1) // LCD_ON
	#if defined(BOOT_USE_LVDS)
					| (1<<2) // LVDSIVT High
	#endif
					| (1<<4) // CODEC_ON
	//#if defined(BOOTSD_INCLUDE)
	//				| Hw6 | Hw7	// SD0_ON, SD1_ON for TRIFLASH boot -> sd lib
	//#endif
					| 0;

	i2cData_data[2] = 0
	#if defined(BOOT_USE_LVDS)
					| (1<<7) // LVDS_LP_CTRL High
	#endif
	#if defined(BOOT_USE_HDMI)
					| (1<<6)			// PWR_GP4
	#endif	
					| 0;
	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);

// set pca9539 U4 ----------------------------------------------

	DestAddress = 0xE0;
	i2cData_data[0] = 3; // config reg. (for port direction)
	i2cData_data[1] = 0; // io 0. output all

	i2c_xfer(DestAddress, 2, i2cData_data, 0, 0, i2c_ch);

	i2cData_data[0] = 1; // set output port
	i2cData_data[1] = 0
	#if defined(BOOT_USE_LVDS) || defined(BOOT_USE_HDMI)
					| (1<<2) // HDMI_LVDS_ON
	#endif
					| 0;
	i2c_xfer(DestAddress, 2, i2cData_data, 0, 0, i2c_ch);
}
#elif defined(BOARD_TCC930X_STB_DEMO)
void pca953x_init(void)
{
}
#else
#include <pca953x.h>

unsigned char pca9539u1_data[2];	// for sdmmc
unsigned char pca9539u3_data[2];

void pca953x_init(void)
{
	unsigned int i2c_ch = I2C_CORE1_CH0;
	unsigned char DestAddress;
	unsigned char i2cData_mode[3] = {0,0,0};	/*{CMD,PORT0,PORT1}*/
	unsigned char i2cData_data[3] = {0,0,0};	/*{CMD,PORT0,PORT1}*/

	#if 0 //Don't need this initilization
    /* init pca9539 U1 */
	DestAddress = SLAVE_ADDR_U1;
	i2cData_mode[0] = PCA9539_DIRECTION_0;
	i2cData_mode[1] = OUTPUT_ALL;
	i2cData_mode[2] = OUTPUT_ALL;
	i2cData_data[0] = PCA9539_OUTPUT_0;
	i2cData_data[1] = 0;
	i2cData_data[2] = 0;
	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);

    /* init pca9539 U2 */
	DestAddress = SLAVE_ADDR_U2;
	i2cData_mode[0] = PCA9539_DIRECTION_0;
	i2cData_mode[1] = OUTPUT_ALL;
	i2cData_mode[2] = OUTPUT_ALL;
	i2cData_data[0] = PCA9539_OUTPUT_0;
	i2cData_data[1] = 0;
	i2cData_data[2] = 0;
	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);

    /* init pca9539 U3 */
	DestAddress = SLAVE_ADDR_U3;
	i2cData_mode[0] = PCA9539_DIRECTION_0;
	i2cData_mode[1] = OUTPUT_ALL;
	i2cData_mode[2] = OUTPUT_ALL;
	i2cData_data[0] = PCA9539_OUTPUT_0;
	i2cData_data[1] = 0;
	i2cData_data[2] = 0;
	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);

    /* init pca9539 U14 */
	DestAddress = SLAVE_ADDR_U14;
	i2cData_mode[0] = PCA9538_DIRECTION_0;
	i2cData_mode[1] = OUTPUT_ALL;
	i2cData_data[0] = PCA9538_OUTPUT_0;
	i2cData_data[1] = 0;
	i2c_xfer(DestAddress, 2, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 2, i2cData_mode, 0, 0, i2c_ch);

	/* DELAY */
	{volatile int ttt;for(ttt=0;ttt<1000;ttt++);}
	#endif

    /* set pca9539 U1 */
	DestAddress = SLAVE_ADDR_U1;
	i2cData_mode[0] = PCA9539_DIRECTION_0;
	i2cData_mode[1] = OUTPUT_ALL;
	i2cData_mode[2] = OUTPUT_ALL;
	i2cData_data[0] = PCA9539_OUTPUT_0;
	i2cData_data[1] = 0
					| LCD_ON
					| CODEC_ON
					| EXT_CODEC_ON
					| 0;
	i2cData_data[2] = 0
                    | DXB_ON
                    | CAS_ON
                    | FMTC_ON
                    | CAM1_ON
                    | CAM2_ON
                    | 0;
	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);

	pca9539u1_data[0] = i2cData_data[1];	// backup
	pca9539u1_data[1] = i2cData_data[2];

    /* set pca9539 U2 */
	DestAddress = SLAVE_ADDR_U2;
	i2cData_mode[0] = PCA9539_DIRECTION_0;
	i2cData_mode[1] = OUTPUT_ALL;
	i2cData_mode[2] = OUTPUT_ALL;
	i2cData_data[0] = PCA9539_OUTPUT_0;
	i2cData_data[1] = 0
//                    | CAS_RST
                      | 0;
	i2cData_data[2] = 0;
	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);

    /* set pca9539 U3 port 0*/
	DestAddress = SLAVE_ADDR_U3;
	i2cData_mode[0] = PCA9539_DIRECTION_0;
	i2cData_mode[1] = OUTPUT_ALL;
	i2cData_mode[2] = OUTPUT_ALL;
	i2cData_data[0] = PCA9539_OUTPUT_0;
	i2cData_data[1] = 0;
	i2cData_data[2] = LCD_RST;
	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);


#if (0) //NC
    /* set pca9539 U14 */
	DestAddress = SLAVE_ADDR_U14;
	i2cData_mode[0] = PCA9538_DIRECTION_0;
	i2cData_mode[1] = OUTPUT_ALL;
	i2cData_data[0] = PCA9538_OUTPUT_0;
	i2cData_data[1] = 0;
	i2c_xfer(DestAddress, 2, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 2, i2cData_mode, 0, 0, i2c_ch);
#endif
}
#endif

#elif defined(_TCC8800_)
#include <pca953x.h>

#if defined(BOARD_TCC880X_STB_DEMO)
void pca953x_init(void)
{
}
#else
void pca953x_init(void)
{
	unsigned int i2c_ch = I2C_CORE0_CH0;
	unsigned char DestAddress;
	unsigned char i2cData_mode[3] = {0,0,0};	/*{CMD,PORT0,PORT1}*/
	unsigned char i2cData_data[3] = {0,0,0};	/*{CMD,PORT0,PORT1}*/

	/* set pca9539 U1 */
	DestAddress = SLAVE_ADDR_U1;
	i2cData_mode[0] = PCA9539_DIRECTION_0;
	i2cData_mode[1] = OUTPUT_ALL;
	i2cData_mode[2] = OUTPUT_ALL;
	i2cData_data[0] = PCA9539_OUTPUT_0;
	i2cData_data[1] = 0
					| (1 << 0)	// LCD_ON
					| (1 << 1)	// CODEC_ON
					| (1 << 2)	// SD0_ON
					| (1 << 3)	// SD1_ON
					| (1 << 4)	// SD2_ON
//					| (1 << 5)	// EXT_CODEC_ON
//					| (1 << 6)	// GPS_PWREN
					| (1 << 7)	// BT_ON
					| 0;
	i2cData_data[2] = 0
					| (1 << 0)	// DXB_ON
					| (1 << 1)	// IPOD_ON
					| (1 << 2)	// CAS_ON
					| (1 << 3)	// FMTC_ON
//					| (1 << 4)	// P-CAM_PWR_ON
					| (1 << 5)	// CAM1_ON
//					| (1 << 6)	// CAM2_ON
//					| (1 << 7)	// ATAPI_ON
					| 0;
	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);

	/* set pca9539 U2 */
	DestAddress = SLAVE_ADDR_U2;
	i2cData_mode[0] = PCA9539_DIRECTION_0;
	i2cData_mode[1] = OUTPUT_ALL;
	i2cData_mode[2] = OUTPUT_ALL;
	i2cData_data[0] = PCA9539_OUTPUT_0;
	i2cData_data[1] = 0
			//		| (1 << 0)	// MUTE_CTL
			#if defined(BOOT_USE_LVDS)
					| (1 << 1)	// LVDSIVT_ON
					| (1 << 2)	// LVDS_LP_CTRL
			#else
			//		| (1 << 1)	// LVDSIVT_ON
			//		| (1 << 2)	// LVDS_LP_CTRL
			#endif//
					| (1 << 3)	// AUTH_RST
					| (1 << 4)	// BT_RST
					| (1 << 5)	// SDWF_RST
					| (1 << 6)	// CAS_RST
			//		| (1 << 7)	// CAS_GP
					| 0;
	i2cData_data[2] = 0
			//		| (1 << 0)	// DXB1_PD
			//		| (1 << 1)	// DXB2_PD
					| (1 << 2)	// PWR_CONTROL0
					| (1 << 3)	// PWR_CONTROL1
					| (1 << 4)	// HDD_RST
			//		| (1 << 5)	// OTG0_VBUS_EN
			//		| (1 << 6)	// OTG1_VBUS_EN
			//		| (1 << 7)	// HOST_VBUS_EN
					| 0;
	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);

	/* set pca9539 U3 port 0*/
	DestAddress = SLAVE_ADDR_U3;
	i2cData_mode[0] = PCA9539_DIRECTION_0;
	i2cData_mode[1] = OUTPUT_ALL;
	i2cData_mode[2] = OUTPUT_ALL;
	i2cData_data[0] = PCA9539_OUTPUT_0;
	i2cData_data[1] = 0
					| (1 << 0)	// FMT_RST
			//		| (1 << 1)	// FMT_IRQ
			//		| (1 << 2)	// BT_WAKE
			//		| (1 << 3)	// BT_HWAKE
			//		| (1 << 4)	//
					| (1 << 5)	// COMPASS_RST
			//		| (1 << 6)	// CAM_FL_EN
			//		| (1 << 7)	// CAM2_FL_EN
					| 0;
	i2cData_data[2] = 0
			//		| (1 << 0)	// CAM2_RST
			//		| (1 << 1)	// CAM2_PWDN
			
				#if (HW_REV >= 0x0600)
					| (1 << 2)	// LCD_RST
				#else
					| (1 << 2)	// LCD_RST
				#endif//
			//		| (1 << 3)	// TV_SLEEP
			//		| (1 << 4)	// ETH_ON
					| (1 << 5)	// ETH_RST
			//		| (1 << 6)	// SMART_AUX1
			//		| (1 << 7)	// SMART_AUX2
					| 0;
	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);

#if (HW_REV >= 0x0600)
	/* set pca9539 U4 port 0*/
	DestAddress = SLAVE_ADDR_U11;
	i2cData_mode[0] = PCA9539_DIRECTION_0;
	i2cData_mode[1] = OUTPUT_ALL;
	i2cData_mode[2] = OUTPUT_ALL;
	i2cData_data[0] = PCA9539_OUTPUT_0;
	i2cData_data[1] = 0
			//		| (1 << 0)	// V_5P0_EN
					| (1 << 1)	// OTG_EN
			//		| (1 << 2)	// HS_HOST_EN
			//		| (1 << 3)	// FS_HOST_EN
			//		| (1 << 4)	// HDMI_EN
			//		| (1 << 5)	// MIPI_EN
			//		| (1 << 6)	// SATA_EN
			#if defined(BOOT_USE_LVDS)
					| (1 << 7)	// LVDS_EN
			#else
			//		| (1 << 7)	// LVDS_EN
			#endif//
					| 0;
	i2cData_data[2] = 0
			//		| (1 << 0)	// ATV_V5P0_EN
			//		| (1 << 1)	// ATAPI_IPOD_V5P0_EN
			//		| (1 << 2)	// USB_CHARGE_CURRENT_SEL
			//		| (1 << 3)	// USB_SUSPEND
			//		| (1 << 4)	// CAHRGE_EN
			//		| (1 << 5)	// SD_3.0_PWR_EN
			//		| (1 << 6)	// SD_3.0_PWR_CTL
			//		| (1 << 7)	// CHARGE_FINISH
					| 0;
	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);
#endif
}
#endif

#elif defined(TARGET_TCC8920_EVM)
#include <pca953x.h>

void pca953x_init(void)
{
	unsigned int i2c_ch = I2C_CH_MASTER0;
	unsigned char DestAddress;
	unsigned char i2cData_mode[3] = {0,0,0};	/*{CMD,PORT0,PORT1}*/
	unsigned char i2cData_data[3] = {0,0,0};	/*{CMD,PORT0,PORT1}*/

	/* set pca9539 U1 */
	DestAddress = SLAVE_ADDR_U1;
	i2cData_mode[0] = PCA9539_DIRECTION_0;
	i2cData_mode[1] = OUTPUT_ALL;
	i2cData_mode[2] = OUTPUT_ALL;
	i2cData_data[0] = PCA9539_OUTPUT_0;
	i2cData_data[1] = 0
					| (1 << 0)	// LCD_ON
					| (1 << 1)	// CODEC_ON
					| (1 << 2)	// SD0_ON
					| (1 << 3)	// SD1_ON
					| (1 << 4)	// SD2_ON
//					| (1 << 5)	// EXT_CODEC_ON
//					| (1 << 6)	// GPS_PWREN
					| (1 << 7)	// BT_ON
					| 0;
	i2cData_data[2] = 0
					| (1 << 0)	// DXB_ON
					| (1 << 1)	// IPOD_ON
					| (1 << 2)	// CAS_ON
					| (1 << 3)	// FMTC_ON
//					| (1 << 4)	// P-CAM_PWR_ON
					| (1 << 5)	// CAM1_ON
//					| (1 << 6)	// CAM2_ON
//					| (1 << 7)	// ATAPI_ON
					| 0;
	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);

	/* set pca9539 U2 */
	DestAddress = SLAVE_ADDR_U2;
	i2cData_mode[0] = PCA9539_DIRECTION_0;
	i2cData_mode[1] = OUTPUT_ALL;
	i2cData_mode[2] = OUTPUT_ALL;
	i2cData_data[0] = PCA9539_OUTPUT_0;
	i2cData_data[1] = 0
			//		| (1 << 0)	// MUTE_CTL
			#if defined(BOOT_USE_LVDS)
					| (1 << 1)	// LVDSIVT_ON
					| (1 << 2)	// LVDS_LP_CTRL
			#endif
					| (1 << 3)	// AUTH_RST
					| (1 << 4)	// BT_RST
					| (1 << 5)	// SDWF_RST
					| (1 << 6)	// CAS_RST
			//		| (1 << 7)	// CAS_GP
					| 0;
	i2cData_data[2] = 0
			//		| (1 << 0)	// DXB1_PD
			//		| (1 << 1)	// DXB2_PD
					| (1 << 2)	// PWR_CONTROL0
					| (1 << 3)	// PWR_CONTROL1
					| (1 << 4)	// HDD_RST
			//		| (1 << 5)	// OTG0_VBUS_EN
			//		| (1 << 6)	// OTG1_VBUS_EN
			//		| (1 << 7)	// HOST_VBUS_EN
					| 0;
	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);

	/* set pca9539 U3 port 0*/
	DestAddress = SLAVE_ADDR_U3;
	i2cData_mode[0] = PCA9539_DIRECTION_0;
	i2cData_mode[1] = OUTPUT_ALL;
	i2cData_mode[2] = OUTPUT_ALL;
	i2cData_data[0] = PCA9539_OUTPUT_0;
	i2cData_data[1] = 0
					| (1 << 0)	// FMT_RST
			//		| (1 << 1)	// FMT_IRQ
			//		| (1 << 2)	// BT_WAKE
			//		| (1 << 3)	// BT_HWAKE
			//		| (1 << 4)	//
					| (1 << 5)	// COMPASS_RST
			//		| (1 << 6)	// CAM_FL_EN
			//		| (1 << 7)	// CAM2_FL_EN
					| 0;
	i2cData_data[2] = 0
			//		| (1 << 0)	// CAM2_RST
			//		| (1 << 1)	// CAM2_PWDN
					| (1 << 2)	// LCD_RST
			//		| (1 << 3)	// TV_SLEEP
			//		| (1 << 4)	// ETH_ON
					| (1 << 5)	// ETH_RST
			//		| (1 << 6)	// SMART_AUX1
			//		| (1 << 7)	// SMART_AUX2
					| 0;
	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);

	/* set pca9539 U4 port 0*/
	DestAddress = SLAVE_ADDR_U11;
	i2cData_mode[0] = PCA9539_DIRECTION_0;
	i2cData_mode[1] = OUTPUT_ALL;
	i2cData_mode[2] = OUTPUT_ALL;
	i2cData_data[0] = PCA9539_OUTPUT_0;
	i2cData_data[1] = 0
			//		| (1 << 0)	// V_5P0_EN
					| (1 << 1)	// OTG_EN
			//		| (1 << 2)	// HS_HOST_EN
			//		| (1 << 3)	// FS_HOST_EN
			//		| (1 << 4)	// HDMI_EN
			//		| (1 << 5)	// MIPI_EN
			//		| (1 << 6)	// SATA_EN
			#if defined(BOOT_USE_LVDS)
					| (1 << 7)	// LVDS_EN
			#endif
					| 0;
	i2cData_data[2] = 0
			//		| (1 << 0)	// ATV_V5P0_EN
			//		| (1 << 1)	// ATAPI_IPOD_V5P0_EN
			//		| (1 << 2)	// USB_CHARGE_CURRENT_SEL
			//		| (1 << 3)	// USB_SUSPEND
			//		| (1 << 4)	// CAHRGE_EN
			//		| (1 << 5)	// SD_3.0_PWR_EN
			//		| (1 << 6)	// SD_3.0_PWR_CTL
			//		| (1 << 7)	// CHARGE_FINISH
					| 0;
	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);
}
#elif defined(TARGET_TCC8920ST_EVM)
#include <pca953x.h>

void pca953x_init(void)
{
}
#endif//
/************* end of file *************************************************************/
