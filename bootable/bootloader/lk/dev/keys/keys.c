/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
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
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
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

#include <bits.h>
#include <debug.h>
#include <string.h>
#include <dev/keys.h>
#include <dev/gpio.h>
#include <config.h>
#include <platform/reg_physical.h>
#ifdef PLATFORM_TCC
#include <platform/gpio.h>
#endif
static unsigned long key_bitmap[BITMAP_NUM_WORDS(MAX_KEYS)];

void keys_init(void)
{
	memset(key_bitmap, 0, sizeof(key_bitmap));
}

void keys_post_event(uint16_t code, int16_t value)
{
	if (code >= MAX_KEYS) {
		dprintf(INFO, "Invalid keycode posted: %d\n", code);
		return;
	}

	/* TODO: Implement an actual event queue if it becomes necessary */
	if (value)
		bitmap_set(key_bitmap, code);
	else
		bitmap_clear(key_bitmap, code);

//	dprintf(INFO, "key state change: %d %d\n", code, value);
}

#ifdef KEYS_USE_ADC_KEYPAD
struct tcc_button
{
    int				s_scancode;
    int				e_scancode;
    uint16_t		vkcode;
};
static struct tcc_button tcc_buttons[] = {
#if defined(BOARD_TCC930X_CM)
	{0x3B0, 0x3BF, KEY_MENU},
	{0x1B0, 0x1BF, KEY_CLEAR},
	{0x90 , 0x9F , KEY_UP},
	{0x2C0, 0x2CF, KEY_HOME},
	{0x110, 0x11F, KEY_BACK},
	{0x10 , 0x1F , KEY_DOWN},
#elif defined(TARGET_TCC8800_EVM)
	{0x1A0, 0x1CF, KEY_MENU},
	{0x80 , 0xAF , KEY_DOWN},
	{0x140, 0x16F, KEY_HOME},
	{0x40 , 0x6F , KEY_UP},
	{0x320, 0x35F, KEY_BACK},
#else
#if 1	// 10bits
	{0x23F, 0x248, KEY_BACK},	// VK_LWIN
	{0x11E, 0x121, KEY_7},		// VK_MENU
	{0x9A , 0x9F , KEY_DOWN},	// VK_SPACE
	{0x5A , 0x5F , KEY_UP},		// VK_UP
	{0x2BA, 0x2C1, KEY_RIGHT},	// VK_TAB
	{0x8  , 0x1E , KEY_CENTER},	// VK_ESCAPE
	{0x338, 0x340, KEY_LEFT},	// VK_RBUTTON
	{0x1B4, 0x1B9, KEY_MENU},	// VK_LEFT
	{0xEA , 0xEF , KEY_8},		// VK_RIGHT
	{0x158, 0x15C, KEY_HOME},	// VK_DOWN
	{0x384, 0x3CB, KEY_SOFT1},	// VK_RETURN
#else
	{0x55A, 0x582,  KEY_MENU},
	{0x6C2, 0x6F4,  KEY_1},
	{0x5A , 0x82 ,  KEY_2},
	{0x15E, 0x186,  KEY_3},
	{0x460, 0x492,  KEY_HOME},
	{0xE92, 0x1004, KEY_5},
	{0x398, 0x3CA,  KEY_6},
	{0xCD0, 0xD0C,  KEY_7},
	{0xADC, 0xB18,  KEY_8},
	{0x258, 0x294,  KEY_9},
	{0x8E8, 0x92E,  KEY_CLEAR},
#endif
#endif
};
#endif

int keys_get_state(uint16_t code)
{
#if defined(_TCC8800_) && ((HW_REV == 0x0602) || (HW_REV == 0x0612))
	uint16_t key_code = 0;

	if ((HwGPIOF->GPDAT&Hw6) == 0)
		key_code = KEY_HOME;
	if ((HwGPIOF->GPDAT&Hw3) == 0)
		key_code = KEY_BACK;

	if (key_code == code)
		return 1;

	return 0;
#elif defined(_TCC8800ST_)
	uint16_t key_code = 0;

	if ((HwGPIOA->GPDAT&Hw3) == 0)
		key_code = KEY_HOME;
	if ((HwGPIOA->GPDAT&Hw4) == 0)
		key_code = KEY_BACK;

	if (key_code == code)
		return 1;

	return 0;
#elif defined(_M57TE_) || defined(_M801_)
	uint16_t key_code = 0;

	if ((HwGPIOE->GPDAT&Hw27) == 0)
		key_code = KEY_HOME;
	if ((HwGPIOE->GPDAT&Hw24) == 0)
		key_code = KEY_BACK;

	if (key_code == code)
		return 1;

	return 0;
#elif defined(_M801_88_) || defined(_M803_)
	uint16_t key_code = 0;

	if ((HwGPIOF->GPDAT&Hw6) == 0)
		key_code = KEY_HOME;
	if ((HwGPIOF->GPDAT&Hw2) == 0)
		key_code = KEY_BACK;

	if (key_code == code)
		return 1;

	return 0;
#elif defined(_M805_8923_)
	uint16_t key_code = 0;

	if ((HwGPIOE->GPDAT&Hw27) == 0)
		key_code = KEY_HOME;
	if ((HwGPIOD->GPDAT&Hw12) == 0)
		key_code = KEY_BACK;

	if (key_code == code)
		return 1;

	return 0;
#elif defined(_M805_8925_)
	uint16_t key_code = 0;

	if ((HwGPIOE->GPDAT&Hw21) == 0)
		key_code = KEY_VOLUMEUP;
	else if ((HwGPIOE->GPDAT&Hw22) == 0)
		key_code = KEY_VOLUMEDOWN;
	else if ((HwGPIOE->GPDAT&Hw23) == 0)
		key_code = KEY_HOME;
#if !(HW_REV == 0x2003 || HW_REV == 0x2004 || (HW_REV == 0x2005))
	else if ((HwGPIOE->GPDAT&Hw19) == 0)
		key_code = KEY_BACK;
#endif
	if (key_code == code)
		return 1;

	return 0;
#elif defined(KEYS_USE_ADC_KEYPAD)
	unsigned long	adc_key;
	uint16_t		key_code = 0;
	int				i;

#if defined(BOARD_TCC930X_CM) || defined(PLATFORM_TCC88XX)
	adc_key = adc_read(4);
#else
	adc_key = adc_read(0);
#endif

	for (i=0 ; i<sizeof(tcc_buttons)/sizeof(struct tcc_button) ; i++)
		if (adc_key >= tcc_buttons[i].s_scancode && adc_key <= tcc_buttons[i].e_scancode) {
			key_code = tcc_buttons[i].vkcode;
			break;
		}

	if (code == key_code)
		return 1;

	return 0;

#elif defined(TARGET_TCC8925_STB_DONGLE)
	gpio_config(GPIO_PORTE|20, GPIO_INPUT | GPIO_FN0);
	printf("check: %d ", gpio_get(GPIO_PORTE|20));
	if(gpio_get(GPIO_PORTE|20) == 0){
		if (KEY_BACK == code)
		return 1;
	}
	return 0;
#else
	if (code >= MAX_KEYS) {
		dprintf(INFO, "Invalid keycode requested: %d\n", code);
		return -1;
	}
	return bitmap_test(key_bitmap, code);
#endif
}
