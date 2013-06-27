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

#ifndef __PLATFORM_GPIO_H
#define __PLATFORM_GPIO_H

#define GPIO_REGMASK    0x000001E0
#define GPIO_REG_SHIFT  5
#define GPIO_BITMASK    0x0000001F

#define GPIO_PORTA      (0 << GPIO_REG_SHIFT)
#define GPIO_PORTB      (1 << GPIO_REG_SHIFT)
#define GPIO_PORTC      (2 << GPIO_REG_SHIFT)
#define GPIO_PORTD      (3 << GPIO_REG_SHIFT)
#define GPIO_PORTE      (4 << GPIO_REG_SHIFT)
#define GPIO_PORTF      (5 << GPIO_REG_SHIFT)
#define GPIO_PORTG      (6 << GPIO_REG_SHIFT)
#define GPIO_PORTHDMI   (7 << GPIO_REG_SHIFT)
#define GPIO_PORTADC    (8 << GPIO_REG_SHIFT)

#define TCC_GPA(x)    (GPIO_PORTA | (x))
#define TCC_GPB(x)    (GPIO_PORTB | (x))
#define TCC_GPC(x)    (GPIO_PORTC | (x))
#define TCC_GPD(x)    (GPIO_PORTD | (x))
#define TCC_GPE(x)    (GPIO_PORTE | (x))
#define TCC_GPF(x)    (GPIO_PORTF | (x))
#define TCC_GPG(x)    (GPIO_PORTG | (x))
#define TCC_GPHDMI(x) (GPIO_PORTHDMI | (x))
#define TCC_GPADC(x)  (GPIO_PORTADC | (x))

#define GPIO_FN_BITMASK	0xF0000000
#define GPIO_FN_SHIFT	28
#define GPIO_FN(x)    (((x) + 1) << GPIO_FN_SHIFT)
#define GPIO_FN0      (1  << GPIO_FN_SHIFT)
#define GPIO_FN1      (2  << GPIO_FN_SHIFT)
#define GPIO_FN2      (3  << GPIO_FN_SHIFT)
#define GPIO_FN3      (4  << GPIO_FN_SHIFT)
#define GPIO_FN4      (5  << GPIO_FN_SHIFT)
#define GPIO_FN5      (6  << GPIO_FN_SHIFT)
#define GPIO_FN6      (7  << GPIO_FN_SHIFT)
#define GPIO_FN7      (8  << GPIO_FN_SHIFT)
#define GPIO_FN8      (9  << GPIO_FN_SHIFT)
#define GPIO_FN9      (10 << GPIO_FN_SHIFT)
#define GPIO_FN10     (11 << GPIO_FN_SHIFT)
#define GPIO_FN11     (12 << GPIO_FN_SHIFT)
#define GPIO_FN12     (13 << GPIO_FN_SHIFT)
#define GPIO_FN13     (14 << GPIO_FN_SHIFT)

#define GPIO_CD_BITMASK	0x0F000000
#define GPIO_CD_SHIFT	24
#define GPIO_CD(x)    (((x) + 1) << GPIO_CD_SHIFT)
#define GPIO_CD0      (1 << GPIO_CD_SHIFT)
#define GPIO_CD1      (2 << GPIO_CD_SHIFT)
#define GPIO_CD2      (3 << GPIO_CD_SHIFT)
#define GPIO_CD3      (4 << GPIO_CD_SHIFT)

void gpio_init_early(void);

#endif
