#include <config.h>
#include <platform/globals.h>
#include <platform/reg_physical.h>

void clock_init(void)
{
	tca_ckc_setfbusctrl( CLKCTRL0,  ENABLE, 0, 8000000, DIRECTPLL5);	/*FBUS_CPU      800 MHz*/	// 1.2V
	tca_ckc_setfbusctrl( CLKCTRL1,  ENABLE, 0, 2500000, DIRECTPLL1);	/*FBUS_DDI      290 MHz*/
	tca_ckc_setfbusctrl( CLKCTRL2,  ENABLE, 0, 4000000, DIRECTPLL3);	/*FBUS_MEM      320 MHz */
	tca_ckc_setfbusctrl( CLKCTRL3, DISABLE, 0, 2500000, DIRECTPLL1);	/*FBUS_GRP      320 MHz */
	tca_ckc_setfbusctrl( CLKCTRL4,  ENABLE, 0, 1680000, DIRECTPLL3);	/*FBUS_IOB      190 MHz */
	tca_ckc_setfbusctrl( CLKCTRL5, DISABLE, 0, 2970000, DIRECTPLL0);	/*FBUS_VBUS     300 MHz */
	tca_ckc_setfbusctrl( CLKCTRL6, DISABLE, 0, 2500000, DIRECTPLL1);	/*FBUS_VCODEC   290 MHz */
	tca_ckc_setfbusctrl( CLKCTRL7,  ENABLE, 0, 2000000, DIRECTPLL5);	/*FBUS_SMU      200 MHz */
	tca_ckc_setfbusctrl( CLKCTRL8,  ENABLE, 0, 2160000, DIRECTPLL2);	/*FBUS_HSIO     240 MHz */
	tca_ckc_setfbusctrl( CLKCTRL9, DISABLE, 0, 2970000, DIRECTPLL0);	/*CAMBUS        330 MHz */
}

void clock_init_early(void)
{
	tca_ckc_init();

	tca_ckc_setpll(5940000, 0);
	tca_ckc_setpll(5000000, 1);
	tca_ckc_setpll(6000000, 2);
	tca_ckc_setpll(6720000, 3);
//	tca_ckc_setpll(6000000, 4);
//	tca_ckc_setpll(6480000, 5);

//	tca_ckc_setfbusctrl( CLKCTRL0,  ENABLE, 0, 8000000, DIRECTPLL5);	/*FBUS_CPU      800 MHz*/	// 1.2V
	tca_ckc_setfbusctrl( CLKCTRL1,  ENABLE, 0, 2500000, DIRECTPLL1);	/*FBUS_DDI      290 MHz*/
//	tca_ckc_setfbusctrl( CLKCTRL2,  ENABLE, 0, 3200000, DIRECTPLL3);	/*FBUS_MEM      320 MHz */
	tca_ckc_setfbusctrl( CLKCTRL3, DISABLE, 0, 2500000, DIRECTPLL1);	/*FBUS_GRP      320 MHz */
	tca_ckc_setfbusctrl( CLKCTRL4,  ENABLE, 0, 1680000, DIRECTPLL3);	/*FBUS_IOB      190 MHz */
	tca_ckc_setfbusctrl( CLKCTRL5, DISABLE, 0, 2970000, DIRECTPLL0);	/*FBUS_VBUS     300 MHz */
	tca_ckc_setfbusctrl( CLKCTRL6, DISABLE, 0, 2500000, DIRECTPLL1);	/*FBUS_VCODEC   290 MHz */
	tca_ckc_setfbusctrl( CLKCTRL7,  ENABLE, 0, 2000000, DIRECTPLL5);	/*FBUS_SMU      200 MHz */
	tca_ckc_setfbusctrl( CLKCTRL8,  ENABLE, 0, 2160000, DIRECTPLL2);	/*FBUS_HSIO     240 MHz */
	tca_ckc_setfbusctrl( CLKCTRL9, DISABLE, 0, 2970000, DIRECTPLL0);	/*CAMBUS        330 MHz */

	// init Peri. Clock
	tca_ckc_setperi(PERI_TCX       , DISABLE,  120000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_TCT       ,  ENABLE,  120000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_TCZ       ,  ENABLE,  120000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_LCD0      , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_LCD1      ,  ENABLE,  960000,PCDIRECTPLL3);
	tca_ckc_setperi(PERI_LCDSI0    , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_LCDSI1    , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_HDMIA     , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_DSI       , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_RESERVED0 , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_HDMI      , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_USB11H    , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_SDMMC0    , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_MSTICK    , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_I2C0      ,  ENABLE,   40000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_UART0     ,  ENABLE,  480000,PCDIRECTPLL3);
	tca_ckc_setperi(PERI_UART1     , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_UART2     , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_UART3     , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_UART4     , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_UART5     , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_GPSB0     , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_GPSB1     , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_GPSB2     , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_GPSB3     , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_GPSB4     , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_GPSB5     , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_ADC       ,  ENABLE,  120000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_SPDIF     , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_EHI0      , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_EHI1      , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_AUD       , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_PDM       , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_SDMMC1    , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_SDMMC2    , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_SDMMC3    , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_DAI0      , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_DAI1      , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_DAI2      , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_I2C1      ,  ENABLE,   40000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_SATA_REF0 , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_SATA_REF1 , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_SATAH0    , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_SATAH1    , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_USB20H    , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_GMAC      , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_CIFMC     , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_CIFSC     , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_ISPJ      , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_ISPS      , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_FILTER    , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_RESERVED1 , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_RESERVED2 , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_RESERVED3 , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_OUT0      , DISABLE,   10000, PCDIRECTXIN);
	tca_ckc_setperi(PERI_OUT1      , DISABLE,   10000, PCDIRECTXIN);
}
