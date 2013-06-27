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
#include <platform/gpio.h>
#include <lcd.h>
#include <i2c.h>
#include <pca953x.h>

#if (defined(DEFAULT_DISPLAY_COMPONENT) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)) && defined(COMPONENT_CHIP_THS8200)

static unsigned char ths8200_slave_addr = 0x40;

#if defined(TARGET_TCC8920_EVM) || defined(TARGET_TCC8920ST_EVM)
static unsigned char ths8200_i2c_ch = I2C_CH_MASTER0;
#elif defined(BOARD_TCC930X_STB_DEMO) || defined(BOARD_TCC880X_STB_DEMO)
static unsigned char ths8200_i2c_ch = I2C_CORE0_CH1;
#elif defined(_TCC9300_)
static unsigned char ths8200_i2c_ch = I2C_CORE2_CH0;
#else
static unsigned char ths8200_i2c_ch = I2C_CH0;
#endif

int ths8200_i2c_write(unsigned char reg, unsigned char val)
{
	unsigned char bytes;
	unsigned char data[2];
	int ret;

	bytes = 2;
	data[0] = reg;
	data[1] = val;

	ret = i2c_xfer(ths8200_slave_addr, bytes, data, 0, 0, ths8200_i2c_ch);
	//dprintf(INFO, "%s, reg=0x%02x, val==0x%02x, ret=%d, ch=%d\n", __func__, reg, val, ret, ths8200_i2c_ch);

	return ret;
}

int ths8200_i2c_read(unsigned char reg, unsigned char *val)
{
	i2c_xfer(ths8200_slave_addr, 1, &reg, 1, val, ths8200_i2c_ch);

	return 0;
}

void ths8200_reset(void)
{
#if defined(BOARD_TCC930X_STB_DEMO)
	PGPIO pGPIO = (PGPIO)&HwGPIO_BASE;

	/* Reset: GPIO_D5 */
	BITSET(pGPIO->GPDDAT, Hw5);
	BITSET(pGPIO->GPDEN, Hw5);
	BITCLR(pGPIO->GPDFN0, Hw24-Hw20);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
	BITCLR(pGPIO->GPDDAT, Hw5);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
	BITSET(pGPIO->GPDDAT, Hw5);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
#elif defined(_TCC9300_)
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
#elif defined(BOARD_TCC880X_STB_DEMO)
	PGPIO pGPIO = (PGPIO)HwGPIO_BASE;

	/* Reset: GPIO_C29 */
	BITSET(pGPIO->GPCDAT, Hw29);
	BITSET(pGPIO->GPCEN, Hw29);
	BITCLR(pGPIO->GPCFN3, Hw24-Hw20);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
	BITCLR(pGPIO->GPCDAT, Hw29);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
	BITSET(pGPIO->GPCDAT, Hw29);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
#elif defined(TARGET_TCC8920_EVM) || defined(TARGET_TCC8920ST_EVM)
	gpio_set(TCC_GPB(28), 1);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
	gpio_set(TCC_GPB(28), 0);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
	gpio_set(TCC_GPB(28), 1);
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

static struct ths8200_std_info ths8200_video_480i_info[] =
{   /*480I*/
};

static struct ths8200_std_info ths8200_video_480p_info[] =
{   /*480P*/
};

static struct ths8200_std_info ths8200_video_720p_info[] =
{   /*720p*/
	 {THS8200_DTG2_CNTL, THS8200_DTG2_CNTL_720P_DEFAULT},
	 {THS8200_DTG1_SPEC_A, THS8200_DTG1_SPEC_A_720P_DEFAULT},
	 {THS8200_DTG1_SPEC_B, THS8200_DTG1_SPEC_B_720P_DEFAULT},
	 {THS8200_DTG1_SPEC_C, THS8200_DTG1_SPEC_C_720P_DEFAULT},
	 {THS8200_DTG1_SPEC_D_LSB, THS8200_DTG1_SPEC_D_LSB_720P_DEFAULT},
	 {THS8200_DTG1_SPEC_E_LSB, THS8200_DTG1_SPEC_E_LSB_720P_DEFAULT},
	 {THS8200_DTG1_SPEC_DEH_MSB, THS8200_DTG1_SPEC_DEH_MSB_720P_DEFAULT},
	 {THS8200_DTG1_SPEC_K_LSB, THS8200_DTG1_SPEC_K_LSB_720P_DEFAULT},
	 {THS8200_DTG1_TOT_PIXELS_MSB, THS8200_DTG1_TOT_PIXELS_MSB_720P_DEFAULT},
	 {THS8200_DTG1_TOT_PIXELS_LSB, THS8200_DTG1_TOT_PIXELS_LSB_720P_DEFAULT},
	 {THS8200_DTG1_MODE, THS8200_DTG1_MODE_720P_DEFAULT},
	 {THS8200_DTG1_FRAME_FIELD_SZ_MSB, THS8200_DTG1_FRAME_FIELD_SZ_MSB_720P_DEFAULT},
	 {THS8200_DTG1_FRAME_SZ_LSB, THS8200_DTG1_FRAME_SZ_LSB_720P_DEFAULT},
	 {THS8200_DTG1_FIELD_SZ_LSB, THS8200_DTG1_FIELD_SZ_LSB_720P_DEFAULT},
	 {THS8200_DTG2_HS_IN_DLY_LSB, THS8200_DTG2_HS_IN_DLY_LSB_720P_DEFAULT},
	 {THS8200_DTG2_VS_IN_DLY_MSB, THS8200_DTG2_VS_IN_DLY_MSB_720P_DEFAULT},
	 {THS8200_DTG2_VS_IN_DLY_LSB, THS8200_DTG2_VS_IN_DLY_LSB_720P_DEFAULT},
	 {0, 0},
};

static struct ths8200_std_info ths8200_video_1080i_info[] =
{        /*1080I*/
	 {THS8200_TST_CNTL1, THS8200_TST_CNTL1_1080I_DEFAULT},
	 {THS8200_TST_CNTL2, THS8200_TST_CNTL2_1080I_DEFAULT},
	 {THS8200_CSM_GY_CNTL_MULT_MSB, THS8200_CSM_GY_CNTL_MULT_MSB_1080I_DEFAULT},
	 {THS8200_DTG2_CNTL, THS8200_DTG2_CNTL_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_A, THS8200_DTG1_SPEC_A_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_B, THS8200_DTG1_SPEC_B_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_C, THS8200_DTG1_SPEC_C_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_D1, THS8200_DTG1_SPEC_D1_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_D_LSB, THS8200_DTG1_SPEC_D_LSB_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_E_LSB, THS8200_DTG1_SPEC_E_LSB_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_DEH_MSB, THS8200_DTG1_SPEC_DEH_MSB_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_K_LSB, THS8200_DTG1_SPEC_K_LSB_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_G_LSB, THS8200_DTG1_SPEC_G_LSB_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_G_MSB, THS8200_DTG1_SPEC_G_MSB_1080I_DEFAULT},
	 {THS8200_DTG1_TOT_PIXELS_MSB, THS8200_DTG1_TOT_PIXELS_MSB_1080I_DEFAULT},
	 {THS8200_DTG1_TOT_PIXELS_LSB, THS8200_DTG1_TOT_PIXELS_LSB_1080I_DEFAULT},
	 {THS8200_DTG1_MODE, THS8200_DTG1_MODE_1080I_DEFAULT},
	 {THS8200_DTG1_FRAME_FIELD_SZ_MSB, THS8200_DTG1_FRAME_FIELD_SZ_MSB_1080I_DEFAULT},
	 {THS8200_DTG1_FRAME_SZ_LSB, THS8200_DTG1_FRAME_SZ_LSB_1080I_DEFAULT},
	 {THS8200_DTG1_FIELD_SZ_LSB, THS8200_DTG1_FIELD_SZ_LSB_1080I_DEFAULT},
	 {THS8200_DTG2_HLENGTH_LSB, THS8200_DTG2_HLENGTH_LSB_1080I_DEFAULT},
	 {THS8200_DTG2_HLENGTH_LSB_HDLY_MSB, THS8200_DTG2_HLENGTH_LSB_HDLY_MSB_1080I_DEFAULT},
	 {THS8200_DTG2_HLENGTH_HDLY_LSB, THS8200_DTG2_HLENGTH_HDLY_LSB_1080I_DEFAULT},
	 {THS8200_DTG2_VLENGTH1_LSB, THS8200_DTG2_VLENGTH1_LSB_1080I_DEFAULT},
	 {THS8200_DTG2_VLENGTH1_MSB_VDLY1_MSB, THS8200_DTG2_VLENGTH1_MSB_VDLY1_MSB_1080I_DEFAULT},
	 {THS8200_DTG2_VDLY1_LSB, THS8200_DTG2_VDLY1_LSB_1080I_DEFAULT},
	 {THS8200_DTG2_VLENGTH2_LSB, THS8200_DTG2_VLENGTH2_LSB_1080I_DEFAULT},
	 {THS8200_DTG2_VDLY2_LSB, THS8200_DTG2_VDLY2_LSB_1080I_DEFAULT},
	 {THS8200_DTG2_VLENGTH2_MSB_VDLY2_MSB, THS8200_DTG2_VLENGTH2_MSB_VDLY2_MSB_1080I_DEFAULT},
	 {THS8200_DTG2_VDLY1_LSB, THS8200_DTG2_VDLY1_LSB_1080I_DEFAULT},
	 {THS8200_DTG2_HS_IN_DLY_LSB, THS8200_DTG2_HS_IN_DLY_LSB_1080I_DEFAULT},
	 {THS8200_DTG2_VS_IN_DLY_MSB, THS8200_DTG2_VS_IN_DLY_MSB_1080I_DEFAULT},
	 {THS8200_DTG2_VS_IN_DLY_LSB, THS8200_DTG2_VS_IN_DLY_LSB_1080I_DEFAULT},
	 {0, 0}
};

static int ths8200_soft_reset()
{
	int err = 0;
	unsigned char val;

	dprintf(INFO, "%s\n", __func__);

	err |= ths8200_i2c_read(THS8200_CHIP_CTL, &val);

	/* reset consists of toggling the reset bit from low to high */

	val &= 0xfe;
	err |= ths8200_i2c_write(THS8200_CHIP_CTL, val);

	val |= 0x1;
	err |= ths8200_i2c_write(THS8200_CHIP_CTL, val);

	return err;
}

void ths8200_set_mode(int mode)
{
	dprintf(INFO, "%s : mode=%d\n", __func__, mode);

	int err = 0,i;
	unsigned char reg, val;

    static struct ths8200_std_info *std_info;

	switch(mode)
	{
		case COMPONENT_MODE_480I:
	        std_info = ths8200_video_480i_info;
	        break;

		case COMPONENT_MODE_480P:
	        std_info = ths8200_video_480i_info;
			break;

	    case COMPONENT_MODE_720P:
	        std_info = ths8200_video_720p_info;
	        break;

	    case COMPONENT_MODE_1080I:
	        std_info = ths8200_video_1080i_info;
	        break;

	    default:
	        std_info = ths8200_video_720p_info;
	        break;
	}

    ths8200_soft_reset();

	#if 0
    for (i = THS8200_CSC_R11; i <= THS8200_CSC_OFFS3; i++) {
	    /* reset color space conversion registers */
	    err |= ths8200_i2c_write(i, 0x0);
    }

    /* CSC bypassed and Under overflow protection ON */
    err |= ths8200_i2c_write(THS8200_CSC_OFFS3, ((THS8200_CSC_BYPASS << THS8200_CSC_BYPASS_SHIFT) | THS8200_CSC_UOF_CNTL));

	/* 20bit YCbCr 4:2:2 input data format */
    err |= ths8200_i2c_write(THS8200_DATA_CNTL,THS8200_DATA_CNTL_MODE_20BIT_YCBCR);
	#else
    /* set color space conversion registers */
    err |= ths8200_i2c_write(THS8200_CSC_R11, 0x00);
    err |= ths8200_i2c_write(THS8200_CSC_R12, 0xDA);
    err |= ths8200_i2c_write(THS8200_CSC_R21, 0x80);
    err |= ths8200_i2c_write(THS8200_CSC_R22, 0x78);
    err |= ths8200_i2c_write(THS8200_CSC_R31, 0x02);
    err |= ths8200_i2c_write(THS8200_CSC_R32, 0x0C);
    err |= ths8200_i2c_write(THS8200_CSC_G11, 0x02);
    err |= ths8200_i2c_write(THS8200_CSC_G12, 0xDC);
    err |= ths8200_i2c_write(THS8200_CSC_G21, 0x81);
    err |= ths8200_i2c_write(THS8200_CSC_G22, 0x94);
    err |= ths8200_i2c_write(THS8200_CSC_G31, 0x81);
    err |= ths8200_i2c_write(THS8200_CSC_G32, 0xDC);
    err |= ths8200_i2c_write(THS8200_CSC_B11, 0x00);
    err |= ths8200_i2c_write(THS8200_CSC_B12, 0x4A);
    err |= ths8200_i2c_write(THS8200_CSC_B21, 0x02);
    err |= ths8200_i2c_write(THS8200_CSC_B22, 0x0C);
    err |= ths8200_i2c_write(THS8200_CSC_B31, 0x80);
    err |= ths8200_i2c_write(THS8200_CSC_B32, 0x30);
    err |= ths8200_i2c_write(THS8200_CSC_OFFS1, 0x00);
    err |= ths8200_i2c_write(THS8200_CSC_OFFS12, 0x08);
    err |= ths8200_i2c_write(THS8200_CSC_OFFS23, 0x02);
    err |= ths8200_i2c_write(THS8200_CSC_OFFS3, 0x00);

	/* 30bit RGB 4:4:4 input data format */
    err |= ths8200_i2c_write(THS8200_DATA_CNTL,THS8200_DATA_CNTL_MODE_30BIT_YCBCR);
	#endif

    err |= ths8200_i2c_write(THS8200_DTG1_Y_SYNC1_LSB,THS8200_DTG1_CBCR_SYNC1_LSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_Y_SYNC2_LSB,THS8200_DTG1_Y_SYNC2_LSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_Y_SYNC3_LSB,THS8200_DTG1_Y_SYNC3_LSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_CBCR_SYNC1_LSB,THS8200_DTG1_CBCR_SYNC1_LSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_CBCR_SYNC2_LSB,THS8200_DTG1_CBCR_SYNC2_LSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_CBCR_SYNC3_LSB,THS8200_DTG1_CBCR_SYNC3_LSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_Y_SYNC_MSB,THS8200_DTG1_Y_SYNC_MSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_CBCR_SYNC_MSB,THS8200_DTG1_CBCR_SYNC_MSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_SPEC_H_LSB,THS8200_DTG1_SPEC_H_LSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_SPEC_K_MSB,THS8200_DTG1_SPEC_K_MSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_FLD_FLIP_LINECNT_MSB,THS8200_DTG1_FLD_FLIP_LINECNT_MSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_LINECNT_LSB,THS8200_DTG1_LINECNT_LSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG2_HS_IN_DLY_MSB,THS8200_DTG2_HS_IN_DLY_MSB_DEFAULT);

    do {
	    reg = std_info->reg;
	    val = std_info->val;

	    err |= ths8200_i2c_write(reg, val);

		if (err < 0)
		{
		    dprintf(INFO, "Set mode i2c write error\n");
			break;
	    }

        std_info++;
	}while (std_info->reg);

    if (err < 0)
    {
		dprintf(INFO, "Set standard failed\n");
		return err;
	}

	ths8200_soft_reset();

	#if 0 /* this is for testing */
	{
		for(i=0; i<0x90; i++)
		{
			err = ths8200_i2c_read(i, &val);

			if(i%8 == 0)
				dprintf(INFO, "\n0x%02x : ", i);
			dprintf(INFO, "0x%02x ", val);

			//dprintf(INFO, "0x%02x : 0x%02x, err=%d\n", i, val, err);
		}
		dprintf(INFO, "\n\n");
	}
	#endif

	return;
}

void ths8200_enable(int mode)
{
	dprintf(INFO, "%s : mode=%d\n", __func__, mode);

	/* Reset the device */
	ths8200_reset();

	/* Set the mode */
	ths8200_set_mode(mode);
}

#endif //#if (defined(DEFAULT_DISPLAY_COMPONENT) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)) && defined(COMPONENT_CHIP_THS8200)

