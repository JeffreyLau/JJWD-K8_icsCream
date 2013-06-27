
#include <config.h>

#include <dev/gpio.h>
#include <platform/gpio.h>
#include <platform/iomap.h>
#include <platform/reg_physical.h>
#include <tnftl/IO_TCCXXX.h>
#include <platform/adc.h>
#include <platform/globals.h>

//#define ADC_DELAY			HwADCDLY_SEL
#define MASK_ADCDATA10(n)   ( (n)&0x3FF )
#define MASK_ADCDATA12(n)   ( (n)&0xFFF )


volatile PTSADC pTSADC;
unsigned short uADC_DATA[8];

#define ASM_NOP {					\
	__asm__ __volatile__ ("nop");	\
	__asm__ __volatile__ ("nop");	\
	__asm__ __volatile__ ("nop");	\
	__asm__ __volatile__ ("nop");	\
	__asm__ __volatile__ ("nop");	\
	__asm__ __volatile__ ("nop");	\
}
static void adc_delay(int delay)
{
	int i;
	for (i=0 ; i<delay ; i++)
		ASM_NOP;
}

static unsigned int adc_powerdown(void)
{
	unsigned int ret = 0;

#if defined(_TCC9200S_)
	return 0;
#endif

#if defined(PLATFORM_TCC892X)
//    pTSADC->CLR_INT_EOC.bREG.CLR = Hw0;
    pTSADC->ADCUPDN.bREG.DOWN = Hw0;
    pTSADC->ADCTSC.nREG |= ADCTSC_WAIT_PENDOWN;

    pTSADC->ADCCON.bREG.STBY = 1;
#else
	pTSADC->ADCCLRINT = Hw0;
	pTSADC->ADCCLRUPDN = Hw0;
	pTSADC->ADCTSC |= ADCTSC_WAIT_PENDOWN;
	BITSET(pTSADC->ADCCON, STDBM_STANDBY);
#endif
	return ret;
}

static unsigned int adc_powerup(void)
{
	unsigned int ret = 0;

#if defined(_TCC9200S_)
	return 0;
#endif

#if defined(PLATFORM_TCC892X)
	pTSADC->ADCCON.bREG.STBY = 0;
#else
	BITCLR(pTSADC->ADCCON, STDBM_STANDBY); // Because STDBM_NORMAL is [ 0<<2 ]
#endif
	return ret;
}

void adc_init_early(void)
{
	volatile PGPIO		pGPIO;
	volatile PIOBUSCFG	pIOBUS;
	volatile PPMU		pPMU;

#if defined(_TCC9200S_)
	return;
#endif

	pGPIO 	= (PGPIO) TCC_GPIO_BASE;
	pIOBUS	= (PIOBUSCFG) TCC_IOBUSCFG_BASE;
	pPMU	= (PPMU) TCC_PMU_BASE;
	pTSADC	= (PTSADC) TCC_TSADC_BASE;

#if defined(PLATFORM_TCC92XX)
	BITCSET(pGPIO->GPEFN3, 0xFFFF0000, Hw28|Hw24|Hw20|Hw16 );  // Function ADCIN
	BITCLR(pGPIO->GPEEN, 0xFF000000); // Port as Input
	BITSET(pGPIO->GPECD1, 0xFFFF0000); // Strength Max
	BITSET(pIOBUS->HCLKEN0, HwIOBUSCFG_TSADC);
#elif defined(PLATFORM_TCC93XX)
	#if defined(BOARD_TCC930X_CM)
    BITCSET(pGPIO->GPGFN2 , 0x0F000000, 0x01000000);  // Function AIN[4] key
    BITCLR(pGPIO->GPGEN, Hw22); // Port as Input
    #else
    BITCSET(pGPIO->GPEFN3 , 0xFFFF0000, Hw30|Hw26|Hw22|Hw18 );  // Function AIN[0-3]
    BITCLR(pGPIO->GPEEN, Hw32-Hw28);// 0xF0000000); // Port as Input
	BITCLR(pIOBUS->HCLKMASK1, Hw5);
	#endif
#elif defined(PLATFORM_TCC88XX)
	tca_ckc_setiopwdn(RB_TSADCCONTROLLER,0);
#elif defined(PLATFORM_TCC892X)
	//tca_ckc_setiopwdn(RB_TSADCCONTROLLER,0);
#endif

	adc_delay(50);
#if defined(PLATFORM_TCC892X)
	pPMU->PMU_TSADC.bREG.PWREN = 1;
	pPMU->PMU_TSADC.bREG.STOP = 0;
	pPMU->PMU_ISOL.bREG.TSADC = 0;


    pTSADC->ADCDLY.nREG = ADC_DELAY(50);
    pTSADC->ADCCON.nREG = RESSEL_12BIT | PRESCALER_EN | PRESCALER_VAL(11)|Hw2;
    pTSADC->ADCTSC.nREG = ADCTSC_WAIT_PENDOWN;
    pTSADC->CLR_INT_EOC.nREG = CLEAR_ADC_INT;
    pTSADC->ADCUPDN.nREG = CLEAR_ADCWK_INT;
#else
	pTSADC->ADCDLY = ADC_DELAY(50);
	pTSADC->ADCTSC = 0x50;
	pTSADC->ADCCON = RESSEL_10BIT| PRESCALER_EN | PRESCALER_VAL(12);	//12


#endif

#if defined(PLATFORM_TCC92XX)
	BITCLR(pPMU->CONTROL, HwPMU_CONTROL_ASTM);
	BITSET(pPMU->CONTROL, HwPMU_CONTROL_AISO);
	BITSET(pPMU->CONTROL, HwPMU_CONTROL_APEN);
#elif defined(PLATFORM_TCC93XX)
	BITCLR(pPMU->CONTROL, HwPMU_CONTROL_ASTM);
	BITSET(pPMU->CONTROL, HwPMU_CONTROL_AISO);
	BITSET(pPMU->CONTROL, HwPMU_CONTROL_APEN);
#elif defined(PLATFORM_TCC88XX)
	BITSET(pPMU->CONTROL, Hw16);
	BITCLR(pPMU->CONTROL, Hw17);
	BITCLR(pPMU->CONTROL, Hw18);
#endif
//	BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_TSADC);
//	adc_powerup();

#if defined(TARGET_TCC8920_EVM)
	gpio_config(TCC_GPADC(4), GPIO_FN0|GPIO_INPUT);
	gpio_config(TCC_GPADC(5), GPIO_FN0|GPIO_INPUT);
#elif defined(TARGET_M805_892X_EVM)
	gpio_config(TCC_GPADC(2), GPIO_FN0|GPIO_INPUT);
	gpio_config(TCC_GPADC(3), GPIO_FN0|GPIO_INPUT);
#endif

}

void adc_init(void)
{
}

#include <debug.h>

unsigned long adc_read(int channel)
{
	unsigned long	ret = 0;
	unsigned int	uiCh = 0;

	int test_routine;

#if defined(_TCC9200S_)
	return 0;
#endif

    switch(channel)
    {
#if !defined(PLATFORM_TCC892X)
        case ADC_CHANNEL0:
            uiCh = SEL_MUX_AIN0;
            break;

        case ADC_CHANNEL1:
            uiCh = SEL_MUX_AIN1;
            break;
#endif

        case ADC_CHANNEL2:
            uiCh = SEL_MUX_AIN2;
            break;

        case ADC_CHANNEL3:
            uiCh = SEL_MUX_AIN3;
            break;

        case ADC_CHANNEL4:
            uiCh = SEL_MUX_AIN4;
            break;

        case ADC_CHANNEL5:
            uiCh = SEL_MUX_AIN5;
            break;

#if 0
        case ADC_CHANNEL6:
            uiCh = SEL_MUX_YM;
            break;

        case ADC_CHANNEL7:
            uiCh = SEL_MUX_YP;
            break;

        case ADC_CHANNEL8:
            uiCh = SEL_MUX_XM;
            break;

        case ADC_CHANNEL9:
            uiCh = SEL_MUX_XP;
            break;
#endif



    }
#if defined(PLATFORM_TCC892X)

	BITCLR(pTSADC->ADCCON.nREG, Hw2);//wakeup


	if(channel <= ADC_CHANNEL5)
	{
		BITCLR(pTSADC->ADCCON.nREG ,SEL_MUX_MASK );
		adc_delay(1);
	}


	BITSET(pTSADC->ADCCON.nREG ,(uiCh|ENABLE_START_EN) );
    while (pTSADC->ADCCON.nREG & ENABLE_START_EN)
    {	// Wait for Start Bit Cleared
		adc_delay(1);
    }
    while (!(pTSADC->ADCCON.nREG & ECFLG_END))
    {	// Wait for ADC Conversion Ended
		adc_delay(1);
   }

    ret = MASK_ADCDATA12( (pTSADC->ADCDAT0.nREG) );

    BITCSET(pTSADC->ADCCON.nREG, SEL_MUX_MASK ,ENABLE_START_EN);
    BITSET(pTSADC->ADCCON.nREG, Hw2);
#else
	if(channel != ADC_TOUCHSCREEN)
	{
		BITCLR(pTSADC->ADCCON ,SEL_MUX_MASK );
		adc_delay(5);
	}

	BITSET(pTSADC->ADCCON ,(uiCh|ENABLE_START_EN) );

	while (pTSADC->ADCCON & ENABLE_START_EN)
	{   // Wait for Start Bit Cleared
		if(channel != ADC_TOUCHSCREEN)
		{
			adc_delay(5);
		}
		else
		{
			adc_delay(10);
		}
	}

	ret = MASK_ADCDATA12( (pTSADC->ADCDAT0) );

	BITCSET(pTSADC->ADCCON, SEL_MUX_MASK ,ENABLE_START_EN);
#endif
    return ret;
}

