/*
 * Copyright (C) 2010 Telechips, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <debug.h>
#include <kernel/thread.h>
#include <platform/debug.h>
#include <dev/fbcon.h>
#include <dev/uart.h>
#include <dev/adc.h>
#include <platform/gpio.h>
#include <reg.h>
#include <platform/iomap.h>

#define BATTVOLSAMPLE 		16
#define ACSAMPLE 8
#if defined(AXP192_PMIC)
#define LOW_BATTERY_LEVEL 	3035
#else
#define LOW_BATTERY_LEVEL 	2180
#endif
#define AC_ADC_LEVEL           1500

#if defined(_M805_8925_)
#define BAT_ADC_CHANNEL	3
#define AC_ADC_CHANNEL		2
#else
#define BAT_ADC_CHANNEL	2
#define AC_ADC_CHANNEL		3
#endif
int check_low_battery(void)
{
	unsigned long adcValue;
	unsigned long BattVoltage;
	int i;
	volatile int count = 0;

	adcValue = 0;

#if defined(AXP192_PMIC)
	while(!axp192_battery_voltage()) {
		count++;
		if(count > 5000) break;
	}
#endif

	for (i=0; i<BATTVOLSAMPLE; i++)
#if defined(AXP192_PMIC)
		adcValue += axp192_battery_voltage();
#else
		adcValue += adc_read(BAT_ADC_CHANNEL);
#endif


	if( adcValue )
	    BattVoltage = adcValue/BATTVOLSAMPLE;
	else
	    BattVoltage = 0;

	dprintf(INFO, "BATTERY LEVEL %d \n", BattVoltage);

	if( BattVoltage < 1000 )
		return 0;

	if( BattVoltage < LOW_BATTERY_LEVEL ) {
		return 1;
	}

	return 0;

}



void powercon_disable(void)
{
#if defined(AXP192_PMIC)
	axp192_power_off();
#elif defined(RN5T614_PMIC)
	rn5t614_power_off();
#endif
	gpio_set(TCC_GPF(28), 0);  // PWR
	gpio_set(TCC_GPE(7), 0);  // SHDN
}


int ac_charger_detect(void)
{
	unsigned long adcValue;
	unsigned long AcVoltage;
	int i;

	adcValue = 0;


#if defined(AXP192_PMIC)
	if(axp192_acin_detect()) return 1;
	else return 0;
#else
	for (i=0; i<ACSAMPLE; i++)
		adcValue += adc_read(AC_ADC_CHANNEL);


	if( adcValue )
	    AcVoltage = adcValue/ACSAMPLE;
	else
	    AcVoltage = 0;


	if( AcVoltage > AC_ADC_LEVEL ) {
		return 1;
	}
#endif

	return 0;


}


//TODO:
#define INT_EI0				3

int usb_detect(void)
{
/*
	BITCSET(HwGPIO->EINTSEL0, (Hw6-Hw0), 62);

	if( HwPIC->STS0 & (1<<INT_EI0) )
		return 1;
*/

	return 0;
}
