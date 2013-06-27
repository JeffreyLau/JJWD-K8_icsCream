///////////////////
// ADCCON

#define RESSEL_10BIT			(0 << 17)
#define RESSEL_12BIT			(1 << 17)

#define ECFLG_PROCESS			(0 << 16)
#define ECFLG_END				(1 << 16)

#define PRESCALER_DIS			(0 << 15)
#define PRESCALER_EN			(1 << 15)

#define PRESCALER_VAL(n)		(((n) & 0xFF) << 7)
#define PRESCALER_MASK			(0xFF << 7)

#define SEL_MUX_AIN0			(0 << 3)
#define SEL_MUX_AIN1			(1 << 3)
#define SEL_MUX_AIN2			(2 << 3)
#define SEL_MUX_AIN3			(3 << 3)
#define SEL_MUX_AIN4			(4 << 3)
#define SEL_MUX_AIN5			(5 << 3)
#define SEL_MUX_YM				(6 << 3)
#define SEL_MUX_YP				(7 << 3)
#define SEL_MUX_XM				(8 << 3)
#define SEL_MUX_XP				(9 << 3)
#define SEL_MUX_AIN10			(10 << 3)
#define SEL_MUX_AIN11			(11 << 3)
#define SEL_MUX_AIN12			(12 << 3)
#define SEL_MUX_AIN13			(13 << 3)
#define SEL_MUX_AIN14			(14 << 3)
#define SEL_MUX_AIN15			(15 << 3)
#define SEL_MUX_MASK			(0xF << 3)
#define SEL_MUX_VAL(n)			((n & 0xF) << 0x3)

#define STDBM_NORMAL			(0<<2)
#define STDBM_STANDBY			(1<<2)
#define STDBM_MASK				(1<<2)

#define READ_START_DIS			(0<<1)
#define READ_START_EN			(1<<1)

#define ENABLE_START_DIS		(0<<0)
#define ENABLE_START_EN			(1<<0)

// ADCTSC
#define UD_SEN_UP				(1<<8)
#define UD_SEN_DOWN				(0<<8)
#define YM_SEN_DIS				(1<<7)
#define YM_SEN_EN				(0<<7)
#define YP_SEN_DIS				(1<<6)
#define YP_SEN_EN				(0<<6)
#define XM_SEN_DIS				(1<<5)
#define XM_SEN_EN				(0<<5)
#define XP_SEN_DIS				(1<<4)
#define XP_SEN_EN				(0<<4)
#define PULL_UP_DIS				(1<<3)
#define PULL_UP_EN				(0<<3)
#define AUTO_PST_EN				(1<<2)
#define AUTO_PST_DIS			(0<<2)

#define XY_PST_NOP				(0<<0)
#define XY_PST_XPOS				(1<<0)
#define XY_PST_YPOS				(2<<0)
#define XY_PST_WAITINT			(3<<0)
#define XY_PST_MASK				(3<<0)

// ADCDLY
#define ADC_CLKSRC__EXTCLK		(0<<16)
#define ADC_CLKSRC__RTCCLK		(1<<16)
#define ADC_CLKSRC_MASK			(1<<16)
#define ADC_DELAY(n)			((n+1)&0xFFFF)
#define ADC_DELAY_MASK			(0xFFFF)

// ADCDAT0
#define D_UPDOWN_DOWN			(0<<15)
#define D_UPDOWN_UP				(1<<15)
#define D_AUTO_PST_DIS			(0<<14)
#define D_AUTO_PST_EN			(1<<14)
#define D_XY_PST_NOP			(0<<12)
#define D_XY_PST_XPOS			(1<<12)
#define D_XY_PST_YPOS			(2<<12)
#define D_XY_PST_WAITINT		(3<<12)
#define D_XY_PST_MASK			(3<<12)

#define D_XPDATA_MASK10(n)		((n)&0x3FF)
#define D_YPDATA_MASK10(n)		((n)&0x3FF)

#define D_XPDATA_MASK12(n)		((n)&0xFFF)
#define D_YPDATA_MASK12(n)		((n)&0xFFF)

// ADCUPDN
#define TSC_DN_INT				(1<<1)
#define TSC_UP_INT				(1<<0)
#define TSC_UPDN_MASK			(3<<0)

// ADCCLRINT
#define CLEAR_ADC_INT			(0xFF)

// ADCCLRWK
#define CLEAR_ADCWK_INT			(0xFF)


//#define ADCTSC_WAIT_PENDOWN	(UD_SEN_DOWN	|YM_SEN_EN|YP_SEN_DIS|XM_SEN_DIS|XP_SEN_DIS|PULL_UP_EN|AUTO_PST_DIS|XY_PST_WAITINT)
#define ADCTSC_WAIT_PENDOWN	(				\
								UD_SEN_DOWN	|\
								YM_SEN_DIS	|\
								YP_SEN_DIS	|\
								XM_SEN_EN	|\
								XP_SEN_DIS	|\
								PULL_UP_EN	|\
								AUTO_PST_DIS|\
								XY_PST_WAITINT\
							)

#define ADCTSC_WAIT_PENUP	(UD_SEN_UP		|YM_SEN_EN|YP_SEN_DIS|XM_SEN_DIS|XP_SEN_DIS|PULL_UP_EN|AUTO_PST_DIS|XY_PST_WAITINT)
//#define ADCTSC_AUTO_ADC		(UD_SEN_DOWN	|YM_SEN_EN|YP_SEN_DIS|XM_SEN_DIS|XP_SEN_DIS|PULL_UP_DIS|AUTO_PST_EN|XY_PST_NOP)

//#define ADCTSC_AUTO_ADC1	(UD_SEN_DOWN	|YM_SEN_DIS|YP_SEN_EN|XM_SEN_DIS|XP_SEN_EN|PULL_UP_EN|AUTO_PST_DIS|XY_PST_XPOS)
//#define ADCTSC_AUTO_ADC2	(UD_SEN_DOWN	|YM_SEN_DIS|YP_SEN_EN|XM_SEN_DIS|XP_SEN_EN|PULL_UP_EN|AUTO_PST_DIS|XY_PST_YPOS)

//#define ADCTSC_AUTO_ADC3	(UD_SEN_DOWN	|YM_SEN_DIS|YP_SEN_EN|XM_SEN_DIS|XP_SEN_EN|PULL_UP_EN|AUTO_PST_DIS|XY_PST_NOP)

//#define ADCTSC_AUTO_ADC4 AUTO_PST_EN
//#define ADCTSC_AUTO_ADC5 AUTO_PST_DIS


enum {
	ADC_CHANNEL0 = 0,
	ADC_CHANNEL1,
	ADC_CHANNEL2,
	ADC_CHANNEL3,
	ADC_CHANNEL4,
	ADC_CHANNEL5,
	ADC_TOUCHSCREEN,
};

