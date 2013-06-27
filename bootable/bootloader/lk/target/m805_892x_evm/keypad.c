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

#include <dev/keys.h>
#include <dev/gpio_keypad.h>
#include <platform/gpio.h>

#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

static unsigned int tcc8923_col_gpios[] = {
	//TCC_GPD(12), TCC_GPD(13), TCC_GPE(12)
};

#if (HW_REV == 0x2002)
static unsigned int tcc8923_row_gpios[] = {
	TCC_GPE(19), TCC_GPE(20), TCC_GPE(21), TCC_GPE(22), TCC_GPE(23)
};
#elif (HW_REV == 0x2003 || HW_REV == 0x2004 || (HW_REV == 0x2005))
static unsigned int tcc8923_row_gpios[] = {
	TCC_GPE(21), TCC_GPE(22), TCC_GPE(23)
};
#else
static unsigned int tcc8923_row_gpios[] = {
	//TCC_GPE(26), TCC_GPE(27)
	TCC_GPD(12), TCC_GPD(13), TCC_GPE(12), TCC_GPE(26), TCC_GPE(27)
};
#endif

#define KEYMAP_INDEX(row, col) ((row)*ARRAY_SIZE(tcc8923_col_gpios) + (col))

static const unsigned short tcc8923_keymap[] = {
	[KEYMAP_INDEX(0, 0)] = KEY_MENU,
	[KEYMAP_INDEX(0, 1)] = KEY_UP,
	[KEYMAP_INDEX(0, 2)] = KEY_HOME,

	[KEYMAP_INDEX(1, 0)] = KEY_F1,
	[KEYMAP_INDEX(1, 1)] = KEY_DOWN,
	[KEYMAP_INDEX(1, 2)] = KEY_BACK,
};

static struct gpio_keypad_info tcc8923_keypad_info = {
	.keymap		= tcc8923_keymap,
	.output_gpios	= tcc8923_col_gpios,
	.input_gpios	= tcc8923_row_gpios,
	.noutputs	= ARRAY_SIZE(tcc8923_col_gpios),
	.ninputs	= ARRAY_SIZE(tcc8923_row_gpios),
	.settle_time	= 5 /* msec */,
	.poll_time	= 20 /* msec */,
	.flags		= GPIOKPF_DRIVE_INACTIVE,
};

void keypad_init(void)
{
#ifdef KEYS_USE_GPIO_KEYPAD
	gpio_keypad_init(&tcc8923_keypad_info);
#endif
}
