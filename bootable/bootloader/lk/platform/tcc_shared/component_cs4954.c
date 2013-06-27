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

#include <debug.h>
#include <platform/reg_physical.h>
#include <platform/globals.h>
#include <lcd.h>
#include <i2c.h>
#include <pca953x.h>

#if (defined(DEFAULT_DISPLAY_COMPONENT) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)) && defined(COMPONENT_CHIP_CS4954)

static unsigned char cs4954_slave_addr = 0x00;

#if defined(BOARD_TCC930X_STB_DEMO) || defined(BOARD_TCC880X_STB_DEMO)
static unsigned char cs4954_i2c_ch = I2C_CORE0_CH1;
#elif defined(_TCC9300_)
static unsigned char cs4954_i2c_ch = I2C_CORE2_CH0;
#else
static unsigned char cs4954_i2c_ch = I2C_CH0;
#endif

void cs4954_i2c_write(unsigned char reg, unsigned char val)
{
	unsigned char bytes;
	unsigned char data[2];
	int ret;

	bytes = 2;
	data[0] = reg;
	data[1] = val;

	ret = i2c_xfer(cs4954_slave_addr, bytes, data, 0, 0, cs4954_i2c_ch);
	//dprintf(INFO, "%s, ret=%d\n", __func__, ret);
}

void cs4954_i2c_read(unsigned char reg, unsigned char *val)
{
	unsigned char bytes;
	
	i2c_xfer(cs4954_slave_addr, 1, &reg, 1, val, cs4954_i2c_ch);
}

void cs4954_reset(void)
{
#if defined(_TCC9300_)
	unsigned char i2cData_data[3] = {0,0,0};	/*{CMD,PORT0,PORT1}*/
	
	/* Reset: GPIO EXPEND U3_P12 */
	i2cData_data[0] = PCA9539_OUTPUT_0;
	i2cData_data[1] = 0;
	i2cData_data[2] = 0;
	i2c_xfer(SLAVE_ADDR_U3, 3, i2cData_data, 0, 0, I2C_CORE1_CH0);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
	i2cData_data[2] = LCD_RST;
	i2c_xfer(SLAVE_ADDR_U3, 3, i2cData_data, 0, 0, I2C_CORE1_CH0);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
#else
	PGPIO pGPIO = (PGPIO)&HwGPIO_BASE;

	/* Reset: GPIO_D22 */
	BITSET(pGPIO->GPDDAT, Hw22);
	BITSET(pGPIO->GPDEN, Hw22);
	BITCLR(pGPIO->GPDFN2, Hw28-Hw24);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
	BITCLR(pGPIO->GPDDAT, Hw22);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
	BITSET(pGPIO->GPDDAT, Hw22);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
#endif
}

void cs4954_set_addr(unsigned char addr)
{
	unsigned char bytes;
	unsigned char data[2];
	unsigned char ucSlaveAddr = 0x00;
	int ret;

	bytes = 2;
	data[0] = 0x0F;
	data[1] = addr;
	
	ret = i2c_xfer(ucSlaveAddr, bytes, data, 0, 0, cs4954_i2c_ch);
	dprintf(INFO, "cs4954_set_addr, ret=%d\n", ret);

{
	unsigned char value;
	cs4954_i2c_read(0x0F, &value);
	dprintf(INFO, "cs4954_set_addr, address=%d\n", value);
}
}

void cs4954_set_mode(int type)
{
	unsigned char ucRegValue;

	switch(type)
	{
	case COMPONENT_MODE_NTSC_M: // NTSC_M
		cs4954_i2c_write(0x00, 0x03); // NTSC-M ITU R.BT601
		cs4954_i2c_write(0x01, 0x12);
		cs4954_i2c_write(0x02, 0x20);
		cs4954_i2c_write(0x03, 0x00);
		cs4954_i2c_write(0x04, 0x00);
		cs4954_i2c_write(0x05, 0x1F);
		cs4954_i2c_write(0x10, 0x1C);
		cs4954_i2c_write(0x11, 0x3E);
		cs4954_i2c_write(0x12, 0xF8);
		cs4954_i2c_write(0x13, 0xE0);
		cs4954_i2c_write(0x14, 0x43);
//		cs4954_i2c_write(0x28, 0xF8); // Brightness Control: -128 ~ 127
		break;

	case COMPONENT_MODE_NTSC_M_J: // NTSC_M_J:
		cs4954_i2c_write(0x00, 0x03); // NTSC-J ITU R.BT601
		cs4954_i2c_write(0x01, 0x10);
		cs4954_i2c_write(0x02, 0x20);
		cs4954_i2c_write(0x03, 0x00);
		cs4954_i2c_write(0x04, 0x00);
		cs4954_i2c_write(0x05, 0x1F);
		cs4954_i2c_write(0x10, 0x1C);
		cs4954_i2c_write(0x11, 0x3E);
		cs4954_i2c_write(0x12, 0xF8);
		cs4954_i2c_write(0x13, 0xE0);
		cs4954_i2c_write(0x14, 0x43);
		break;

	case COMPONENT_MODE_PAL_M: // PAL_M:
		cs4954_i2c_write(0x00, 0x63); 
		cs4954_i2c_write(0x01, 0x12);
		cs4954_i2c_write(0x02, 0x20);
		cs4954_i2c_write(0x03, 0x00);
		cs4954_i2c_write(0x04, 0x00);
		cs4954_i2c_write(0x05, 0x1F);
		cs4954_i2c_write(0x10, 0x15);
		cs4954_i2c_write(0x11, 0xC7);
		cs4954_i2c_write(0x12, 0xDF);
		cs4954_i2c_write(0x13, 0xCD);
		cs4954_i2c_write(0x14, 0x43);
		break;

	case COMPONENT_MODE_PAL_N: // PAL_N
		cs4954_i2c_write(0x00, 0xa3);
		cs4954_i2c_write(0x01, 0x30);
		cs4954_i2c_write(0x02, 0x20);
		cs4954_i2c_write(0x03, 0x00);
		cs4954_i2c_write(0x04, 0x00);
		cs4954_i2c_write(0x05, 0x1F);
		cs4954_i2c_write(0x10, 0x15);
		cs4954_i2c_write(0x11, 0x96);
		cs4954_i2c_write(0x12, 0x15);
		cs4954_i2c_write(0x13, 0x13);
		cs4954_i2c_write(0x14, 0x54);
		break;

	case COMPONENT_MODE_PAL_B: // PAL_B,G,H,I
	case COMPONENT_MODE_PAL_G:
	case COMPONENT_MODE_PAL_H:
	case COMPONENT_MODE_PAL_I:
		cs4954_i2c_write(0x00, 0x43);
		cs4954_i2c_write(0x01, 0x30);
		cs4954_i2c_write(0x02, 0x20);
		cs4954_i2c_write(0x03, 0x00);
		cs4954_i2c_write(0x04, 0x00);
		cs4954_i2c_write(0x05, 0x1F);
		cs4954_i2c_write(0x10, 0x15);
		cs4954_i2c_write(0x11, 0x96);
		cs4954_i2c_write(0x12, 0x15);
		cs4954_i2c_write(0x13, 0x13);
		cs4954_i2c_write(0x14, 0x54);
		break;
	}

	#if 0 // COMPONENT_CCIR656
	cs4954_i2c_read(0x00, &ucRegValue);
	ucRegValue |= Hw3; // CCIR656
	cs4954_i2c_write(0x00, ucRegValue);
	#endif
	
	#if 0 // This is for testing
	{
		unsigned char val;
		int i;

		dprintf(INFO, "\n");
		for (i=0; i<0x80; i++) {

			cs4954_i2c_read(i, &val);

			if (i%8 == 0)
				dprintf(INFO, "\n[%04d|0x%02x]  %02x", i, i, val);
			else
				dprintf(INFO, "  %02x", val);
		}
		
		dprintf(INFO, "\n");
		dprintf(INFO, "\n");
	}
	#endif
}

void cs4954_enable(int type)
{
	/* Set the device */
	cs4954_reset();
	
	/* Set the slave address */
	//cs4954_set_addr(cs4954_slave_addr);

	/* Set the output mode */
	cs4954_set_mode(type);
}

#endif //#if (defined(DEFAULT_DISPLAY_COMPONENT) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)) && defined(COMPONENT_CHIP_CS4954)

