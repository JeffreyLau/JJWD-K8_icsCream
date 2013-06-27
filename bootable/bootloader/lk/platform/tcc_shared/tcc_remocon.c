/*
 * IR driver for remote controller : tcc_remocon.c
 *
 * Copyright (C) 2010 Telechips, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

#include <platform/reg_physical.h>
#include <platform/globals.h>
#include <platform/interrupts.h>
#include <dev/gpio.h>
#include <platform/gpio.h>

#include "tcc_remocon.h"

#define DEVICE_NAME	"tcc-remote"

#define TCC_IRVERSION	0.001

#define IR_IRQ			42

#define KEY_RELEASED	0
#define KEY_PRESSED		1

#if 0
#define dbg printf
#else /* no debug */
#define dbg(x...) do {} while(0)
#endif

/*****************************************************************************
*
* structures
*
******************************************************************************/
struct tcc_remote
{
	char old_key;		//Previous KeyCode
	int status;		//Key Status
};

typedef struct
{
	unsigned int repeatCount;
	unsigned int BitCnt;	//Data Bit Count
	unsigned long long Buf;	//Data Buffer
	unsigned int Id;	//Remocon ID
	unsigned int Code;	//Parsing Return Value
	unsigned int Stat;	//Remocon Status
}REM_DATA;

static int startbit_time;
static int prev_startbit_time;

static struct tcc_remote *rem;
static REM_DATA Rem;

static int fb_power_state = 0;

/*****************************************************************************
* Function Name : static void Init_IR_Port(void);
* Description : IR port register init
* Arguments :
******************************************************************************/
static void Init_IR_Port(void)
{
#if defined(CONFIG_ARCH_TCC892X)
	gpio_config(TCC_GPG(17), GPIO_FN7|GPIO_OUTPUT|GPIO_LOW);
#else
	PGPIO pGpioA = (volatile PGPIO)tcc_p2v(HwGPIOA_BASE);
	BITCSET(pGpioA->GPAFN0, (Hw20 | Hw21 | Hw22 | Hw23), Hw20);	//GPIO_A5
#endif
}

/*****************************************************************************
* Function Name : static void remocon_irq_init(void);
* Description : IRQ register init
* Arguments :
******************************************************************************/
static void remocon_irq_init(void)
{
	volatile PPIC pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);

	#if defined(CONFIG_ARCH_TCC892X)
		BITSET(pPIC->IEN1.nREG, Hw10);
		BITSET(pPIC->SEL1.nREG, Hw10);
		BITSET(pPIC->INTMSK1.nREG, Hw10);
		BITCLR(pPIC->POL1.nREG, Hw10);
		BITSET(pPIC->MODE1.nREG, Hw10);
		BITCLR(pPIC->MODEA1.nREG, Hw10);
	#else
	BITSET(pPIC->IEN1, HwINT1_RMT);
	BITSET(pPIC->SEL1, HwINT1_RMT);
	BITSET(pPIC->INTMSK1, HwINT1_RMT);
	BITCLR(pPIC->POL1, HwINT1_RMT);
	BITSET(pPIC->MODE1, HwINT1_RMT);
	BITCLR(pPIC->MODEA1, HwINT1_RMT);
	#endif
}

/*****************************************************************************
* Function Name : static int tca_rem_getkeycodebyscancode(unsigned short kc);
* Description : transform ScanCode into KeyCode.
* Arguments : kc - scancode
******************************************************************************/
static int tca_rem_getkeycodebyscancode(unsigned short kc)
{
	int i;
	for (i = 0;i < sizeof(key_mapping)/sizeof(SCANCODE_MAPPING);i++)
		if (kc == key_mapping[i].rcode) 
			return key_mapping[i].vkcode;
	return -1;
}

/*****************************************************************************
* Function Name : static void tca_rem_sendremocondata(unsigned int key_type);
* Description : key event is generated.
* Arguments : Data - key type(new or repeat)
******************************************************************************/
static void tca_rem_sendremocondata(unsigned int key_type)
{
	int nRem;

	switch(key_type) {
	case NEW_KEY:
		nRem = tca_rem_getkeycodebyscancode(Rem.Code);
		if(nRem == -1)
			return;
		if(rem->status==KEY_PRESSED) {
			dbg("############### nRem=%d released\n",rem->old_key);
			rem->status=KEY_RELEASED;
		}
		if(nRem==REM_POWER)
			fb_power_state = 1;
		dbg("############### nRem=%d pressed\n", nRem);
		rem->status = KEY_PRESSED;
		rem->old_key = nRem;
		break;
	case REPEAT_KEY:
		if(fb_power_state==0 || !isRepeatableKey(rem->old_key))
			return;
		if(rem->status==KEY_PRESSED && Rem.repeatCount>1) {
			dbg("############### nRem=%d repeat=%d\n",rem->old_key, Rem.repeatCount);
		}
		Rem.repeatCount++;
		break;
	case REPEAT_START:
		if(fb_power_state==0 || !isRepeatableKey(rem->old_key))
			return;
		Rem.repeatCount = 0;
	}
}

/*****************************************************************************
* Function Name : static unsigned int tca_rem_readcode(char ch);
* Description : Decode readCode.
* Arguments : ch - readcode, return value - key type(new or repeat)
******************************************************************************/
static unsigned int tca_rem_readcode(char ch)
{
	switch (Rem.Stat)
	{
	/* Initialize */
	case STATUS0:
		Rem.BitCnt = Rem.Buf = 0;
		Rem.Stat = STATUS1;

		if(1)//machine_is_tcc9300() || machine_is_tcc9300st() || machine_is_tcc9300cm() || machine_is_tcc8800() || machine_is_tcc8800st())
		{
			if (ch == 'S')
			{
				Rem.Stat = STATUS2;
			}
			else if (ch == 'R')
			{
				//tcnt=12.5ms
				return REPEAT_KEY;
			}
		}
		break;

	/*Start Bit */
	case  STATUS1:
		if (ch == 'S') 	//tcnt = 15ms 
		{
			//it appears to be HW bug that the 1st repeat pule is always 
			//recognized wrongly as start bit 
			if (startbit_time != prev_startbit_time)
			{
				prev_startbit_time = startbit_time;
				Rem.Code = 0;	//reset the memorized code if we get a start bit
			}
			Rem.Stat = STATUS2;
		}

		//Repeat
		else if (ch == 'R')
		{
			dbg("R: %llx\n", Rem.Code);
			//tcnt=12.5ms
			return REPEAT_KEY;
		}
		break;

	/* Data Code Check */
	case STATUS2:
		if (ch == '0')			//Bit '0' = 3.5ms
		{
			Rem.BitCnt++;
		}
		else if (ch == '1')		//Bit '1' = 5.5ms
		{
			Rem.Buf |= (((unsigned long long)1) << Rem.BitCnt);
			Rem.BitCnt++;
		}
		else if (ch == 'S')		//If Receive Start Bit, Return 0;
		{
			return 0;
		}
		else
		{
			Rem.Stat =STATUS0;
		}

		if(Rem.BitCnt == IR_SIGNAL_MAXIMUN_BIT_COUNT)
		{
			dbg("############### BUF=%llx\n", Rem.Buf);

			Rem.Id = (Rem.Buf & IR_ID_MASK);
			dbg("############### ID=%x\n", Rem.Id);

			if (Rem.Buf == REMOCON_REPEAT)
			{
				Rem.Stat = STATUS0;
				return REPEAT_START;
			}
			else if (Rem.Id == REMOCON_ID)
			{
				Rem.Code = (Rem.Buf  & IR_CODE_MASK) >> IR_BUFFER_BIT_SHIFT;
				dbg("############### CODE=%llx\n", Rem.Code);
				Rem.Stat = STATUS0;
				return NEW_KEY;
			}
			else
			{
				Rem.Stat = STATUS0;
			}
		}
		break;
	}

	return NO_KEY;
}

/*****************************************************************************
* Function Name : static int tca_rem_process(char ch);
* Description : data processing
* Arguments : ch - rawdata, return value(0: success, -1: failed)
******************************************************************************/
static int tca_rem_process(char ch)
{
	int key_type = tca_rem_readcode(ch);

	if (key_type==NO_KEY)
		return -1;

	tca_rem_sendremocondata(key_type);
	return 0;
}

/*****************************************************************************
* Function Name : static void rem_getrawdata(int *rd_data);
* Description : transform IR input value into rawdata
* Arguments : rd_data - IR input value
******************************************************************************/
static void rem_getrawdata(int *rd_data)
{
	int i=0;
	int low, high;
	char low_bit='x', high_bit='x';

#if defined(CONFIG_ARCH_TCC892X)
	#define LOW_MIN_VALUE		LOW_MIN_TCC892X
	#define LOW_MAX_VALUE		LOW_MAX_TCC892X
	#define HIGH_MIN_VALUE		HIGH_MIN_TCC892X
	#define HIGH_MAX_VALUE		HIGH_MAX_TCC892X
	#define REPEAT_MIN_VALUE	REPEAT_MIN_TCC892X
	#define REPEAT_MAX_VALUE	REPEAT_MAX_TCC892X
	#define START_MIN_VALUE		START_MIN_TCC892X
	#define START_MAX_VALUE		START_MAX_TCC892X
#else
	#define LOW_MIN_VALUE		LOW_MIN
	#define LOW_MAX_VALUE		LOW_MAX
	#define HIGH_MIN_VALUE		HIGH_MIN
	#define HIGH_MAX_VALUE		HIGH_MAX
	#define REPEAT_MIN_VALUE	REPEAT_MIN
	#define REPEAT_MAX_VALUE	REPEAT_MAX
	#define START_MIN_VALUE		START_MIN
	#define START_MAX_VALUE		START_MAX
#endif

	do
	{
		low = rd_data[i] & 0xffff;
		high = (rd_data[i] >> 16) & 0xffff;

		if ((low > LOW_MIN_VALUE) && (low < LOW_MAX_VALUE))
		{		  
			low_bit='0';
		}
		else if ((low > HIGH_MIN_VALUE) && (low < HIGH_MAX_VALUE))
		{
			low_bit='1';
		}
		else if ((low > REPEAT_MIN_VALUE) && (low < REPEAT_MAX_VALUE))
		{
			low_bit='R';
		}
		else if ((low > START_MIN_VALUE) && (low < START_MAX_VALUE))
		{
			low_bit='S';
			startbit_time= rd_data[i];  //get Start bit timing  and the 1st data bit timing
		}
		else
		{
			low_bit='E';
		}

		if ((high > LOW_MIN_VALUE) && (high < LOW_MAX_VALUE))
		{
			high_bit='0';
		}
		else if ((high > HIGH_MIN_VALUE) && (high < HIGH_MAX_VALUE))
		{
			high_bit='1';
		}
		else if ((high > REPEAT_MIN_VALUE) && (high < REPEAT_MAX_VALUE))
		{
			high_bit='R';
		}
		else if ((high > START_MIN_VALUE) && (high < START_MAX_VALUE))
		{
			high_bit='S';
			startbit_time= rd_data[i];  //get Start bit timing  and the 1st data bit timing
		}
		else
		{
			high_bit='E';
		}

		if(low_bit == 'S' || high_bit=='S') {
			dbg("\n############### start\n");
		}
		//dbg("%04x|%04x => %c%c\n", low, high, low_bit, high_bit);
		dbg("%04x|%04x => %c%c\n", low, high, low_bit, high_bit);

		if (tca_rem_process(low_bit) == 0 || tca_rem_process(high_bit) == 0)
		{
			break;
		}
	} while(++i < IR_FIFO_READ_COUNT);
}

/*****************************************************************************
* Function Name : static irqreturn_t remocon_handler(int irq, void *dev);
* Description : IR interrupt event handler
* Arguments :
******************************************************************************/
static int remocon_handler(int irq, void *id, struct pt_regs *regs)
{	
	int i=0;
	PREMOTECON pRcu = (volatile PREMOTECON)tcc_p2v(HwREMOTE_BASE);
	int rd_data[IR_FIFO_READ_COUNT];

	//tcc_remocon_GetTickCount();

	Rem.Stat = STATUS0;

	#if defined(CONFIG_ARCH_TCC892X)
		pRcu->CLKDIV.nREG = BITCLR(pRcu->CLKDIV.nREG, 0xFFFFFF00);	// Set CLK_DIV as 0
	#else
	pRcu->CLKDIV = BITCLR(pRcu->CLKDIV, 0xFFFFFF00);	// Set CLK_DIV as 0
	#endif

	//udelay(150); //delay for fifo clear

	do
	{
		#if defined(CONFIG_ARCH_TCC892X)
		rd_data[i] = pRcu->RDATA.nREG;
		#else
		rd_data[i] = pRcu->RDATA;
		#endif
	}
	while (++i < IR_FIFO_READ_COUNT);

	rem_getrawdata(rd_data);

	//udelay(100); //Remocon can't be so faster!!!

	RemoconInit();

	//tcc_remocon_GetTickCount();

	return 0;
}

/*****************************************************************************
* Name :  Device register
* Description : This functions register device
* Arguments :
******************************************************************************/
int remocon_probe(void)
{
	rem = (struct tcc_remote *)malloc(sizeof(struct tcc_remote));

	Init_IR_Port();
	RemoconConfigure ();
	RemoconStatus();
	RemoconDivide();		//remocon clk divide and end_cout
	RemoconCommandOpen();				
	RemoconIntClear();
	remocon_irq_init();

	//Init IR variable
	rem->old_key = -1;
	rem->status = KEY_RELEASED;

	//Interrupt
	register_int_handler(IR_IRQ,remocon_handler, NULL);

	return 0;
}

int getRemoconState(void)
{
    static int giRemoconPowerOn = 0;
    if(giRemoconPowerOn)
        return giRemoconPowerOn;

	if(rem->old_key==REM_POWER)
    {
        giRemoconPowerOn = 1;
    	return 1; //normal booting
    }
    else if(rem->old_key==REM_0)
    {
        giRemoconPowerOn = 2;
		return 2; //fastbooting
    }
	return 0;
}

int isMagicKey(void)
{
    if(rem->old_key==REM_9)
        return 1;
    return 0;
}
