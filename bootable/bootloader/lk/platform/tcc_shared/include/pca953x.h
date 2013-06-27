/*--------------------------------------------------------
 * 
 * TCC9300_CPU_DDR2 + TCC89_91_92_DEMO Main Board
 *
 *--------------------------------------------------------
 */
 
enum pca9539_cmd
{
	PCA9539_INPUT_0		= 0,
	PCA9539_INPUT_1		= 1,
	PCA9539_OUTPUT_0	= 2,
	PCA9539_OUTPUT_1	= 3,
	PCA9539_INVERT_0	= 4,
	PCA9539_INVERT_1	= 5,
	PCA9539_DIRECTION_0	= 6,
	PCA9539_DIRECTION_1	= 7,
};

enum pca9538_cmd
{
	PCA9538_INPUT_0		= 0,
	PCA9538_OUTPUT_0	= 1,
	PCA9538_INVERT_0	= 2,
	PCA9538_DIRECTION_0	= 3,
};

#define OUTPUT_ALL		0x0

/* pca9539 address 
 * -------------------------
 * | 1| 1| 1| 0| 1|A1|A0|RW|
 * -------------------------
 */
#define SLAVE_ADDR_U1	0xE8
#define SLAVE_ADDR_U2	0xEE
#define SLAVE_ADDR_U3	0xEA
#define SLAVE_ADDR_U11	0xEC

/* pca9537 address 
 * -------------------------
 * | 1| 0| 0| 1| 0| 0| 1|RW|
 * -------------------------
 */
#define SLAVE_ADDR_U14	0x92

/* U1 */
#define LCD_ON			Hw0		/* port 0 */
#define CODEC_ON		Hw1
#define SD0_ON			Hw2
#define SD1_ON			Hw3
#define SD2_ON			Hw4
#define EXT_CODEC_ON	Hw5
#define GPS_PWREN		Hw6
#define BT_ON			Hw7

#define DXB_ON			Hw0		/* port 1 */
#define IPOD_ON			Hw1
#define CAS_ON			Hw2
#define FMTC_ON			Hw3
#define PCAM_PWR_ON		Hw4
#define CAM1_ON			Hw5
#define CAM2_ON			Hw6
#define ATAPI_ON		Hw7

/* U2 */
#define MUTE_CTL		Hw0		/* port 0 */
#define LVDSIVT_ON		Hw1
#define LVDS_LP_CTRL	Hw2
#define AUTH_RST		Hw3
#define BT_RST			Hw4
#define SDWF_RST		Hw5
#define CAS_RST			Hw6
#define CAS_GP			Hw7

#define DXB1_PD			Hw0		/* port 1 */
#define DXB2_PD			Hw1
#define PWR_CONTROL0	Hw2
#define PWR_CONTROL1	Hw3
#define HDD_RST			Hw4
#define OTG0_VBUS_EN	Hw5
#define OTG1_VBUS_EN	Hw6
#define HOST_VBUS_EN	Hw7

/* U3 */
#define FMT_RST			Hw0		/* port 0 */
#define FMT_IRQ			Hw1
#define BT_WAKE			Hw2
#define BT_HWAKE		Hw3
#define PHY2_ON			Hw4
#define COMPASS_RST		Hw5
#define CAM_FL_EN		Hw6
#define CAM2_FL_EN		Hw7

#define CAM2_RST		Hw0		/* port 1 */
#define CAM2_PWDN		Hw1
#define LCD_RST			Hw2
#define TV_SLEEP		Hw3
#define ETH_ON			Hw4
#define ETH_RST			Hw5
#define SMART_AUX1		Hw6
#define SMART_AUX2		Hw7

/* U14 */
#define SD30_POW_CTL0   Hw0
#define SD30_POW_CTL1   Hw1
#define SD30_POW_CTL2   Hw2
#define SD30_POW_CTL3   Hw3

