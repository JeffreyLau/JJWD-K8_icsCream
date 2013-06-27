
#include <platform/globals.h>
#include <platform/reg_physical.h>
#include <platform/clock.h>
#include <i2c.h>
#include <dev/pmic.h>

#include <debug.h>

void clock_init(void)
{
#if (0)
#if defined(AXP192_PMIC)
	axp192_init(I2C_CORE0_CH0);
	axp192_set_voltage(AXP192_ID_DCDC2, 1320);	// CoreA 1200mV
#endif

	tca_ckc_setfbusctrl( FBUS_CPU,    ENABLE, 9990000);	/*FBUS_CPU      999 MHz */	// 1.32V
	tca_ckc_setfbusctrl( FBUS_MEM,    ENABLE, 4000000);	/*FBUS_MEM      400 MHz */
	tca_ckc_setfbusctrl( FBUS_IO,     ENABLE, 2500000);	/*FBUS_IOB      250 MHz */
#else
#if defined(AXP192_PMIC)
	axp192_init(I2C_CORE0_CH0);
	axp192_set_voltage(AXP192_ID_DCDC2, 1200);	// CoreA 1200mV
#endif
	tca_ckc_setfbusctrl( FBUS_CPU,    ENABLE, 8000000);	/*FBUS_CPU      800 MHz */	// 1.2V
	tca_ckc_setfbusctrl( FBUS_MEM,    ENABLE, 3200000);	/*FBUS_MEM      320 MHz */
	tca_ckc_setfbusctrl( FBUS_IO,     ENABLE, 1980000);	/*FBUS_IOB      198 MHz */
#endif
	tca_ckc_setfbusctrl( FBUS_DDI,    ENABLE, 1470000);	/*FBUS_DDI      320 MHz */	// 1.2V
	tca_ckc_setfbusctrl( FBUS_GPU,   DISABLE, 1610000);	/*FBUS_GRP      322 MHz */
	tca_ckc_setfbusctrl( FBUS_VBUS,  DISABLE, 1500000);	/*FBUS_VBUS     300 MHz */
	tca_ckc_setfbusctrl( FBUS_VCORE, DISABLE, 1450000);	/*FBUS_VCODEC   290 MHz */
	tca_ckc_setfbusctrl( FBUS_SMU,    ENABLE, 1000000);	/*FBUS_SMU      200 MHz */
	tca_ckc_setfbusctrl( FBUS_HSIO,   ENABLE, 1220000);	/*FBUS_HSIO     250 MHz */
	tca_ckc_setfbusctrl( FBUS_CAM,   DISABLE, 1650000);	/*CAMBUS        330 MHz */
}

void clock_init_early(void)
{
	tca_ckc_init();

	tca_ckc_setpll(5940000, 0);
	tca_ckc_setpll(5000000, 1);
	tca_ckc_setpll(4320000, 2);
	tca_ckc_setpll(6720000, 3);
//	tca_ckc_setpll(6000000, 4);
//	tca_ckc_setpll(6480000, 5);

//	tca_ckc_setfbusctrl( FBUS_CPU,    ENABLE, 4000000);	/*FBUS_CPU      400 MHz */	// 1.0V
	tca_ckc_setfbusctrl( FBUS_DDI,    ENABLE, 1470000);	/*FBUS_DDI      147 MHz */
//	tca_ckc_setfbusctrl( FBUS_MEM,    ENABLE, 1600000);	/*FBUS_MEM      160 MHz */
	tca_ckc_setfbusctrl( FBUS_GPU,   DISABLE, 1610000);	/*FBUS_GRP      161 MHz */
	tca_ckc_setfbusctrl( FBUS_IO,     ENABLE,  980000);	/*FBUS_IOB       98 MHz */
	tca_ckc_setfbusctrl( FBUS_VBUS,  DISABLE, 1500000);	/*FBUS_VBUS     150 MHz */
	tca_ckc_setfbusctrl( FBUS_VCORE, DISABLE, 1450000);	/*FBUS_VCODEC   145 MHz */
	tca_ckc_setfbusctrl( FBUS_SMU,    ENABLE, 1000000);	/*FBUS_SMU      100 MHz */
	tca_ckc_setfbusctrl( FBUS_HSIO,   ENABLE, 1220000);	/*FBUS_HSIO     122 MHz */
	tca_ckc_setfbusctrl( FBUS_CAM,   DISABLE, 1650000);	/*CAMBUS        165 MHz */

	// init Peri. Clock
	tca_ckc_setperi(PERI_TCX       , DISABLE,  120000);
	tca_ckc_setperi(PERI_TCT       ,  ENABLE,  120000);
	tca_ckc_setperi(PERI_TCZ       ,  ENABLE,  120000);
	tca_ckc_setperi(PERI_LCD0      , DISABLE,   10000);
	tca_ckc_setperi(PERI_LCD1      ,  ENABLE,  960000);
	tca_ckc_setperi(PERI_LCDSI0    , DISABLE,   10000);
	tca_ckc_setperi(PERI_LCDSI1    , DISABLE,   10000);
	tca_ckc_setperi(PERI_HDMIA     , DISABLE,   10000);
	tca_ckc_setperi(PERI_DSI       , DISABLE,   10000);
	tca_ckc_setperi(PERI_RESERVED0 , DISABLE,   10000);
	tca_ckc_setperi(PERI_HDMI      , DISABLE,   10000);
	tca_ckc_setperi(PERI_USB11H    , DISABLE,   10000);
	tca_ckc_setperi(PERI_SDMMC0    , DISABLE,   10000);
	tca_ckc_setperi(PERI_MSTICK    , DISABLE,   10000);
	tca_ckc_setperi(PERI_I2C0      ,  ENABLE,   40000);
	tca_ckc_setperi(PERI_UART0     ,  ENABLE,  480000);
	tca_ckc_setperi(PERI_UART1     , DISABLE,   10000);
	tca_ckc_setperi(PERI_UART2     , DISABLE,   10000);
	tca_ckc_setperi(PERI_UART3     , DISABLE,   10000);
	tca_ckc_setperi(PERI_UART4     , DISABLE,   10000);
	tca_ckc_setperi(PERI_UART5     , DISABLE,   10000);
	tca_ckc_setperi(PERI_GPSB0     , DISABLE,   10000);
	tca_ckc_setperi(PERI_GPSB1     , DISABLE,   10000);
	tca_ckc_setperi(PERI_GPSB2     , DISABLE,   10000);
	tca_ckc_setperi(PERI_GPSB3     , DISABLE,   10000);
	tca_ckc_setperi(PERI_GPSB4     , DISABLE,   10000);
	tca_ckc_setperi(PERI_GPSB5     , DISABLE,   10000);
	tca_ckc_setperi(PERI_ADC       ,  ENABLE,  120000);
	tca_ckc_setperi(PERI_SPDIF     , DISABLE,   10000);
	tca_ckc_setperi(PERI_EHI0      , DISABLE,   10000);
	tca_ckc_setperi(PERI_EHI1      , DISABLE,   10000);
	tca_ckc_setperi(PERI_AUD       , DISABLE,   10000);
	tca_ckc_setperi(PERI_PDM       , DISABLE,   10000);
	tca_ckc_setperi(PERI_SDMMC1    , DISABLE,   10000);
	tca_ckc_setperi(PERI_SDMMC2    , DISABLE,   10000);
	tca_ckc_setperi(PERI_SDMMC3    , DISABLE,   10000);
	tca_ckc_setperi(PERI_DAI0      , DISABLE,   10000);
	tca_ckc_setperi(PERI_DAI1      , DISABLE,   10000);
	tca_ckc_setperi(PERI_DAI2      , DISABLE,   10000);
	tca_ckc_setperi(PERI_I2C1      ,  ENABLE,   40000);
	tca_ckc_setperi(PERI_SATA_REF0 , DISABLE,   10000);
	tca_ckc_setperi(PERI_SATA_REF1 , DISABLE,   10000);
	tca_ckc_setperi(PERI_SATAH0    , DISABLE,   10000);
	tca_ckc_setperi(PERI_SATAH1    , DISABLE,   10000);
	tca_ckc_setperi(PERI_USB20H    , DISABLE,   10000);
	tca_ckc_setperi(PERI_GMAC      , DISABLE,   10000);
	tca_ckc_setperi(PERI_CIFMC     , DISABLE,   10000);
	tca_ckc_setperi(PERI_CIFSC     , DISABLE,   10000);
	tca_ckc_setperi(PERI_ISPJ      , DISABLE,   10000);
	tca_ckc_setperi(PERI_ISPS      , DISABLE,   10000);
	tca_ckc_setperi(PERI_FILTER    , DISABLE,   10000);
	tca_ckc_setperi(PERI_RESERVED1 , DISABLE,   10000);
	tca_ckc_setperi(PERI_RESERVED2 , DISABLE,   10000);
	tca_ckc_setperi(PERI_RESERVED3 , DISABLE,   10000);
	tca_ckc_setperi(PERI_OUT0      , DISABLE,   10000);
	tca_ckc_setperi(PERI_OUT1      , DISABLE,   10000);
}
