/*
 * Copyright (c) 2011 Telechips, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <platform/globals.h>
#include <platform/reg_physical.h>
#include <platform/clock.h>

#include <i2c.h>
#if defined(AXP192_PMIC)
#include <dev/axp192.h>
#elif defined(RN5T614_PMIC)
#include <dev/rn5t614.h>
#endif

/************************************************************
* Function    : clock_init()
* Description :
*    - increase fbus clock (1.2V or higher level)
************************************************************/
void clock_init(void)
{
#if defined(AXP192_PMIC)
	axp192_init(I2C_CH_MASTER0);
	axp192_set_voltage(AXP192_ID_DCDC1, 1200);	// VDD_12D
	#if !defined(_TCC8923_0XA_)
	axp192_set_power(AXP192_ID_DCDC1, 0);
	#endif
	axp192_set_voltage(AXP192_ID_DCDC2, 1350);	// VDD_COREA
	axp192_set_voltage(AXP192_ID_DCDC3, 1800);	// VDD_18D
	axp192_set_voltage(AXP192_ID_LDO2,  3000);	// VDD_IOD0
	axp192_set_power(AXP192_ID_LDO2, 1);
	axp192_set_voltage(AXP192_ID_LDO3,  3300);	// WIFI_IOD
	axp192_set_power(AXP192_ID_LDO3, 0);
	axp192_set_voltage(AXP192_ID_LDO4,  3300);	// HDMI_OSC
	axp192_set_power(AXP192_ID_LDO4, 0);
#elif defined(RN5T614_PMIC)
	rn5t614_init(I2C_CH_MASTER0);
	rn5t614_set_voltage(RN5T614_ID_DCDC1, 1350);	// CoreA 1200mV

	#if (HW_REV == 0x2003 || HW_REV == 0x2004 || HW_REV == 0x2005)
		rn5t614_set_power(RN5T614_ID_LDO2, 0);
		rn5t614_set_power(RN5T614_ID_LDO3, 0);
		rn5t614_set_voltage(RN5T614_ID_LDO4, 3300);
		rn5t614_set_power(RN5T614_ID_LDO4, 1);
		rn5t614_set_voltage(RN5T614_ID_LDO5, 3300);
		rn5t614_set_power(RN5T614_ID_LDO5, 0);
		rn5t614_set_voltage(RN5T614_ID_LDO6, 1200);
		rn5t614_set_power(RN5T614_ID_LDO6, 0);
		rn5t614_set_voltage(RN5T614_ID_LDO7, 1200);
		rn5t614_set_power(RN5T614_ID_LDO7, 0);
		rn5t614_set_voltage(RN5T614_ID_LDO8, 3000);
		rn5t614_set_power(RN5T614_ID_LDO8, 0);
	#elif (HW_REV == 0x2002)
		rn5t614_set_power(RN5T614_ID_LDO2, 0);
		rn5t614_set_power(RN5T614_ID_LDO3, 0);
		rn5t614_set_voltage(RN5T614_ID_LDO4, 3300);
		rn5t614_set_power(RN5T614_ID_LDO4, 1);
		rn5t614_set_voltage(RN5T614_ID_LDO5, 3300);
		rn5t614_set_power(RN5T614_ID_LDO5, 0);
		rn5t614_set_voltage(RN5T614_ID_LDO6, 2600);
		rn5t614_set_power(RN5T614_ID_LDO6, 0);
		rn5t614_set_voltage(RN5T614_ID_LDO7, 3000);
		rn5t614_set_power(RN5T614_ID_LDO7, 0);
		rn5t614_set_voltage(RN5T614_ID_LDO8, 3000);
		rn5t614_set_power(RN5T614_ID_LDO8, 0);
	#else
		#error -- please select the HW_REV --
	#endif
#endif

	tca_ckc_setfbusctrl( FBUS_CPU,    ENABLE, 8120000);	/*FBUS_CPU      625 MHz */	// 1.2V
#if defined(DRAM_DDR3)
	tca_ckc_setfbusctrl( FBUS_MEM,    ENABLE, 5330000);	/*FBUS_MEM      533 MHz */
#else
	tca_ckc_setfbusctrl( FBUS_MEM,    ENABLE, 3000000);	/*FBUS_MEM      300 MHz */
#endif
	tca_ckc_setfbusctrl( FBUS_DDI,    ENABLE, 3120000);	/*FBUS_DDI      312 MHz */
	tca_ckc_setfbusctrl( FBUS_GPU,   DISABLE, 3640000);	/*FBUS_GRP      370 MHz */
	tca_ckc_setfbusctrl( FBUS_IO,     ENABLE, 1960000);	/*FBUS_IOB      196 MHz */
	tca_ckc_setfbusctrl( FBUS_VBUS,  DISABLE, 2770000);	/*FBUS_VBUS     277 MHz */
	tca_ckc_setfbusctrl( FBUS_VCORE, DISABLE, 2770000);	/*FBUS_VCODEC   277 MHz */
	tca_ckc_setfbusctrl( FBUS_HSIO,   ENABLE, 2500000);	/*FBUS_HSIO     250 MHz */
	tca_ckc_setfbusctrl( FBUS_SMU,    ENABLE, 1000000);	/*FBUS_SMU      196 MHz */
}

/************************************************************
* Function    : clock_init_early()
* Description :
*    - set pll/peri-clock and
*    - set fbus clock to low (1.0V level)
************************************************************/
void clock_init_early(void)
{
	tca_ckc_init();

	tca_ckc_setfbusctrl( FBUS_IO,     ENABLE,   60000);	/*FBUS_IO		6 MHz */
	tca_ckc_setfbusctrl( FBUS_SMU,    ENABLE,   60000);	/*FBUS_SMU		6 MHz */

	tca_ckc_setpll(5940000, 0);
	tca_ckc_setpll(5000000, 1);
#if defined(CONFIG_GPU_BUS_SCALING)
	tca_ckc_setpll(6720000, 2);
//	tca_ckc_setpll(7280000, 3);	// for gpu
#else
	tca_ckc_setpll(7280000, 2);
	tca_ckc_setpll(6720000, 3);
#endif
//	tca_ckc_setpll(6000000, 4);	// for memory bus
//	tca_ckc_setpll(6480000, 5);	// for cpu

//	tca_ckc_setfbusctrl( FBUS_CPU,    ENABLE, 4000000);	/*FBUS_CPU      400 MHz */	// 1.0V
//	tca_ckc_setfbusctrl( FBUS_MEM,    ENABLE, 1250000);	/*FBUS_MEM      160 MHz */
	tca_ckc_setfbusctrl( FBUS_DDI,    ENABLE, 1250000);	/*FBUS_DDI      290 MHz */
	tca_ckc_setfbusctrl( FBUS_GPU,   DISABLE, 1250000);	/*FBUS_GRP      320 MHz */
	tca_ckc_setfbusctrl( FBUS_IO,     ENABLE,  720000);	/*FBUS_IO       190 MHz */
	tca_ckc_setfbusctrl( FBUS_VBUS,  DISABLE,  990000);	/*FBUS_VBUS     300 MHz */
	tca_ckc_setfbusctrl( FBUS_VCORE, DISABLE, 1250000);	/*FBUS_VCODEC   290 MHz */
	tca_ckc_setfbusctrl( FBUS_HSIO,   ENABLE, 1080000);	/*FBUS_HSIO     240 MHz */
	tca_ckc_setfbusctrl( FBUS_SMU,    ENABLE, 1000000);	/*FBUS_SMU      200 MHz */

	// init Peri. Clock
	tca_ckc_setperi(PERI_TCX       , DISABLE,  120000);
	tca_ckc_setperi(PERI_TCT       ,  ENABLE,  120000);
	tca_ckc_setperi(PERI_TCZ       ,  ENABLE,  120000);
	tca_ckc_setperi(PERI_LCD0      , DISABLE,   10000);
	tca_ckc_setperi(PERI_LCDSI0    , DISABLE,   10000);
	tca_ckc_setperi(PERI_LCD1      ,  ENABLE,  960000);
	tca_ckc_setperi(PERI_LCDSI1    , DISABLE,   10000);
	tca_ckc_setperi(PERI_RESERVED0 , DISABLE,   10000);
	tca_ckc_setperi(PERI_LCDTIMER  , DISABLE,   10000);
	tca_ckc_setperi(PERI_JPEG      , DISABLE,   10000);
	tca_ckc_setperi(PERI_RESERVED1 , DISABLE,   10000);
	tca_ckc_setperi(PERI_RESERVED2 , DISABLE,   10000);
	tca_ckc_setperi(PERI_GMAC      , DISABLE,   10000);
	tca_ckc_setperi(PERI_USBOTG    , DISABLE,   10000);
	tca_ckc_setperi(PERI_RESERVED3 , DISABLE,   10000);
	tca_ckc_setperi(PERI_OUT0      , DISABLE,   10000);
	tca_ckc_setperi(PERI_USB20H    , DISABLE,   10000);
	tca_ckc_setperi(PERI_HDMI      , DISABLE,   10000);
	tca_ckc_setperi(PERI_HDMIA     , DISABLE,   10000);
	tca_ckc_setperi(PERI_OUT1      , DISABLE,   10000);
	tca_ckc_setperi(PERI_EHI       , DISABLE,   10000);
	tca_ckc_setperi(PERI_SDMMC0    , DISABLE,   10000);
	tca_ckc_setperi(PERI_SDMMC1    , DISABLE,   10000);
	tca_ckc_setperi(PERI_SDMMC2    , DISABLE,   10000);
	tca_ckc_setperi(PERI_SDMMC3    , DISABLE,   10000);
	tca_ckc_setperi(PERI_ADAI1     , DISABLE,   10000);
	tca_ckc_setperi(PERI_ADAM1     , DISABLE,   10000);
	tca_ckc_setperi(PERI_SPDIF1    , DISABLE,   10000);
	tca_ckc_setperi(PERI_ADAI0     , DISABLE,   10000);
	tca_ckc_setperi(PERI_ADAM0     , DISABLE,   10000);
	tca_ckc_setperi(PERI_SPDIF0    , DISABLE,   10000);
	tca_ckc_setperi(PERI_PDM       , DISABLE,   10000);
	tca_ckc_setperi(PERI_RESERVED4 , DISABLE,   10000);
	tca_ckc_setperi(PERI_ADC       ,  ENABLE,  120000);
	tca_ckc_setperi(PERI_I2C0      ,  ENABLE,   40000);
	tca_ckc_setperi(PERI_I2C1      ,  ENABLE,   40000);
	tca_ckc_setperi(PERI_I2C2      ,  ENABLE,   40000);
	tca_ckc_setperi(PERI_I2C3      ,  ENABLE,   40000);
	tca_ckc_setperi(PERI_UART0     ,  ENABLE,  480000);
	tca_ckc_setperi(PERI_UART1     , DISABLE,   10000);
	tca_ckc_setperi(PERI_UART2     , DISABLE,   10000);
	tca_ckc_setperi(PERI_UART3     , DISABLE,   10000);
	tca_ckc_setperi(PERI_UART4     , DISABLE,   10000);
	tca_ckc_setperi(PERI_UART5     , DISABLE,   10000);
	tca_ckc_setperi(PERI_UART6     , DISABLE,   10000);
	tca_ckc_setperi(PERI_UART7     , DISABLE,   10000);
	tca_ckc_setperi(PERI_GPSB0     , DISABLE,   10000);
	tca_ckc_setperi(PERI_GPSB1     , DISABLE,   10000);
	tca_ckc_setperi(PERI_GPSB2     , DISABLE,   10000);
	tca_ckc_setperi(PERI_GPSB3     , DISABLE,   10000);
	tca_ckc_setperi(PERI_GPSB4     , DISABLE,   10000);
	tca_ckc_setperi(PERI_GPSB5     , DISABLE,   10000);
}
