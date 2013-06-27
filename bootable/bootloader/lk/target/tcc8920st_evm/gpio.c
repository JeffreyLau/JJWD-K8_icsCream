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

#include <dev/gpio.h>
#include <platform/gpio.h>
#include <platform/reg_physical.h>

struct gpio_cfg {
	unsigned port;
	unsigned function;
	unsigned direction;
	unsigned pull;
};

struct gpio_cfg TCC8920_GPA_init_tbl[] = {
	{ TCC_GPA(0)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPA(1)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPA(2)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPA(3)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPA(4)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPA(5)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPA(6)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPA(7)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPA(8)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPA(9)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPA(10) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPA(11) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPA(12) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPA(13) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPA(14) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPA(15) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPA(16) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE }
};

struct gpio_cfg TCC8920_GPB_init_tbl[] = {
	{ TCC_GPB(0)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(1)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(2)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(3)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(4)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(5)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(6)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE }, //DRAMPWR_ON // not used.
	{ TCC_GPB(7)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(8)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE }, //SD30 Test
	{ TCC_GPB(9)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(10) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(11) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(12) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(13) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(14) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(15) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(16) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(17) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(18) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
#if defined(TARGET_BOARD_STB)
	{ TCC_GPB(19) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_HIGH, GPIO_PULLDISABLE },
	{ TCC_GPB(20) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(21) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_HIGH, GPIO_PULLDISABLE },
#else
	{ TCC_GPB(19) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(20) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(21) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
#endif
	{ TCC_GPB(22) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(23) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(24) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(25) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(26) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(27) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(28) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPB(29) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE }
};

struct gpio_cfg TCC8920_GPC_init_tbl[] = {
	{ TCC_GPC(0)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(1)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(2)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(3)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(4)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(5)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(6)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(7)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(8)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(9)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(10) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(11) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(12) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(13) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(14) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(15) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(16) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(17) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
#if defined(TARGET_BOARD_STB)
	{ TCC_GPC(18) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE }, //PMIC_RST#
#else
	{ TCC_GPC(18) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_HIGH, GPIO_PULLDISABLE }, //PMIC_RST#
#endif
	{ TCC_GPC(19) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(20) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(21) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(22) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_HIGH, GPIO_PULLDISABLE }, //MEMQZQ_ON
	{ TCC_GPC(23) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(24) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(25) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(26) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(27) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(28) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPC(29) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE }
};

struct gpio_cfg TCC8920_GPD_init_tbl[] = {
	{ TCC_GPD(0)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPD(1)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPD(2)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPD(3)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPD(4)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPD(5)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPD(6)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPD(7)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPD(8)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE }, //RTC_PMWKUP
	{ TCC_GPD(9)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPD(10) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPD(11) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPD(12) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
#if defined(TARGET_TCC8925_HDB892F)
	{ TCC_GPD(13) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_HIGH , GPIO_PULLDISABLE }, //For FWDN
#else
	{ TCC_GPD(13) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
#endif /* TARGET_TCC8925_HDB892F */
	{ TCC_GPD(14) ,GPIO_FN0 , GPIO_INPUT           , GPIO_PULLDISABLE }, //KEY_IRQ to wake up from sleep..
	{ TCC_GPD(15) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPD(16) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPD(17) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPD(18) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPD(19) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPD(20) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPD(21) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE }
};

struct gpio_cfg TCC8920_GPE_init_tbl[] = {
	{ TCC_GPE(0)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(1)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(2)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(3)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(4)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(5)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(6)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(7)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(8)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(9)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(10) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(11) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(12) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(13) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(14) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(15) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(16) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(17) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(18) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(19) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(20) ,GPIO_FN0 , GPIO_INPUT , GPIO_PULLDISABLE },
	{ TCC_GPE(21) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(22) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(23) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(24) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(25) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(26) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(27) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(28) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(29) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(30) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPE(31) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE }
};

struct gpio_cfg TCC8920_GPF_init_tbl[] = {
	{ TCC_GPF(0)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(1)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(2)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(3)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(4)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(5)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(6)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(7)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
#if defined(TARGET_TCC8925_HDB892F)
	{ TCC_GPF(8)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_HIGH , GPIO_PULLDISABLE }, //SLEEP_MODE_CTRL
	{ TCC_GPF(9)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_HIGH , GPIO_PULLDISABLE }, //VDDQ_MEM_ON
#else
	{ TCC_GPF(8)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(9)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
#endif
	{ TCC_GPF(10) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(11) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(12) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
#if defined(TARGET_BOARD_STB)
	{ TCC_GPF(13) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_HIGH, GPIO_PULLDISABLE },
#else
	{ TCC_GPF(13) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
#endif
	{ TCC_GPF(14) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(15) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(16) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_HIGH, GPIO_PULLDISABLE }, //PWR_SHDN
	{ TCC_GPF(17) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(18) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(19) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(20) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(21) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(22) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(23) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(24) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(25) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(26) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(27) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(28) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_HIGH, GPIO_PULLDISABLE }, //RST#_IRQ to prevent NAND crash
	{ TCC_GPF(29) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPF(30) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE }
};

struct gpio_cfg TCC8920_GPG_init_tbl[] = {
	{ TCC_GPG(0)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(1)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(2)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(3)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(4)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(5)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(6)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(7)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(8)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(9)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(10) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(11) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(12) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(13) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(14) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(15) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(16) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(17) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(18) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPG(19) ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE }
};

struct gpio_cfg TCC8920_GPHDMI_init_tbl[] = {
	{ TCC_GPHDMI(0)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
#if defined(TARGET_BOARD_STB)
	{ TCC_GPHDMI(1)  ,GPIO_FN0 , GPIO_INPUT , GPIO_PULLDISABLE },
#else
	{ TCC_GPHDMI(1)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
#endif
	{ TCC_GPHDMI(2)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPHDMI(3)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE }
};

struct gpio_cfg TCC8920_GPADC_init_tbl[] = {
	{ TCC_GPADC(2)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPADC(3)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPADC(4)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE },
	{ TCC_GPADC(5)  ,GPIO_FN0 , GPIO_OUTPUT|GPIO_LOW , GPIO_PULLDISABLE }
};


void gpio_init_early(void)
{
	unsigned int i;

	for(i=0 ; i<(sizeof(TCC8920_GPA_init_tbl)/sizeof(TCC8920_GPA_init_tbl[0])) ; i++)
		gpio_config(TCC8920_GPA_init_tbl[i].port, TCC8920_GPA_init_tbl[i].function|TCC8920_GPA_init_tbl[i].direction|TCC8920_GPA_init_tbl[i].pull);

	for(i=0 ; i<(sizeof(TCC8920_GPB_init_tbl)/sizeof(TCC8920_GPB_init_tbl[0])) ; i++)
		gpio_config(TCC8920_GPB_init_tbl[i].port, TCC8920_GPB_init_tbl[i].function|TCC8920_GPB_init_tbl[i].direction|TCC8920_GPB_init_tbl[i].pull);

	for(i=0 ; i<(sizeof(TCC8920_GPC_init_tbl)/sizeof(TCC8920_GPC_init_tbl[0])) ; i++)
		gpio_config(TCC8920_GPC_init_tbl[i].port, TCC8920_GPC_init_tbl[i].function|TCC8920_GPC_init_tbl[i].direction|TCC8920_GPC_init_tbl[i].pull);

	for(i=0 ; i<(sizeof(TCC8920_GPD_init_tbl)/sizeof(TCC8920_GPD_init_tbl[0])) ; i++)
		gpio_config(TCC8920_GPD_init_tbl[i].port, TCC8920_GPD_init_tbl[i].function|TCC8920_GPD_init_tbl[i].direction|TCC8920_GPD_init_tbl[i].pull);

	for(i=0 ; i<(sizeof(TCC8920_GPE_init_tbl)/sizeof(TCC8920_GPE_init_tbl[0])) ; i++)
		gpio_config(TCC8920_GPE_init_tbl[i].port, TCC8920_GPE_init_tbl[i].function|TCC8920_GPE_init_tbl[i].direction|TCC8920_GPE_init_tbl[i].pull);

	for(i=0 ; i<(sizeof(TCC8920_GPF_init_tbl)/sizeof(TCC8920_GPF_init_tbl[0])) ; i++)
		gpio_config(TCC8920_GPF_init_tbl[i].port, TCC8920_GPF_init_tbl[i].function|TCC8920_GPF_init_tbl[i].direction|TCC8920_GPF_init_tbl[i].pull);

	for(i=0 ; i<(sizeof(TCC8920_GPG_init_tbl)/sizeof(TCC8920_GPG_init_tbl[0])) ; i++)
		gpio_config(TCC8920_GPG_init_tbl[i].port, TCC8920_GPG_init_tbl[i].function|TCC8920_GPG_init_tbl[i].direction|TCC8920_GPG_init_tbl[i].pull);

	for(i=0 ; i<(sizeof(TCC8920_GPHDMI_init_tbl)/sizeof(TCC8920_GPHDMI_init_tbl[0])) ; i++)
		gpio_config(TCC8920_GPHDMI_init_tbl[i].port, TCC8920_GPHDMI_init_tbl[i].function|TCC8920_GPHDMI_init_tbl[i].direction|TCC8920_GPHDMI_init_tbl[i].pull);

	for(i=0 ; i<(sizeof(TCC8920_GPADC_init_tbl)/sizeof(TCC8920_GPADC_init_tbl[0])) ; i++)
		gpio_config(TCC8920_GPADC_init_tbl[i].port, TCC8920_GPADC_init_tbl[i].function|TCC8920_GPADC_init_tbl[i].direction|TCC8920_GPADC_init_tbl[i].pull);

}
