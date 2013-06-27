/*
 * Copyright (c) 2010 Telechips, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __PLATFORM_TCC88XX_NAND_H
#define __PLATFORM_TCC88XX_NAND_H

#define TCC_NAND_BASE	0xF05B0000

#define NAND_REG(off) (TCC_NAND_BASE + (off))

#define NAND_FLASH_CMD            NAND_REG(0x0000)
#define NAND_LINEAR_ADDR          NAND_REG(0x0004)
#define NAND_BLOCK_ADDR           NAND_REG(0x0008)
#define NAND_SIGNAL_ADDR          NAND_REG(0x000C)
#define NAND_SINGLE_DATA          NAND_REG(0x0040)
#define NAND_CONTROL              NAND_REG(0x0050)

/* Flash type */
#define FLASH_UNKNOWN_DEVICE        0x00
#define FLASH_NAND_DEVICE           0x01
#define FLASH_8BIT_NAND_DEVICE      0x01
#define FLASH_16BIT_NAND_DEVICE     0x02
#define FLASH_ONENAND_DEVICE        0x03

#endif /* __PLATFORM_TCC88XX_NAND_H */
