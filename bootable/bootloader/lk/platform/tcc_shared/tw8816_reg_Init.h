#ifndef __TCA_TW8816_REG_INIT_H__
#define __TCA_TW8816_REG_INIT_H__



#define TW8816_I2C_ADDR		0x8A



unsigned char TW8816_Init_Data[][2] = {


	{0xFF, 0x00}, 	/* Index register page selection  0 = 0x000~0x0FE, 1= 0x100 ~ 0x1FE */
		
	{0x06, 0x80},		/* RESET : 0x00 , Analog Control Register */
	{0x02, 0x40},		/* RESET : 0x40 , Input Format - Input crystal clock frequency is 27MHz.  */
	{0x04, 0x00},		/* RESET : 0x00,  HSYNC Delay Control, Color killer time constant 0:Faster */

	{0x06, 0x00},		/* RESET : 0x00 , Analog Control Register */

/*==============================================================*/
/*		Decoder cropping setting	*/
/*===============================================================*/
#if 1
	{0x07, 0x13},		
	
	{0x08, 0x00},		/* VDELAY_LO = 0*/
	{0x09, 0xE0},		/* VACTIVE_LO = 0xE0, V=0x1E0*/	
	{0x0A, 0x00},		/* HDELAY_LO = 0*/	
	{0x0B, 0x20},		/* HACTIVE_LO = 0x20*/	

#else
	{0x07, 0x02},		/* RESET : 0x12,  Cropping Register, High(CROP_HI), VDELAY_HI =0, VACTIVE_HI=0, HDELAY_HI=0, HACTIVE_HI=2 */	
 	{0x08, 0x0C},		/* RESET : 0x12,	Vertical Delay Register, Low,			VDELAY_LO 	= 0x15 	*/
 	{0x09, 0xF8},		/* RESET : 0x20,	Vertical Active Register, Low,			VACTIVE_LO 	= 0xF0 	*/	
 	{0x0A, 0x0F},		/* RESET : 0x10,	Horizontal Delay Register, Low 		HDELAY_LO 	= 0x1B	*/	
 	{0x0B, 0xBD},		/* RESET : 0xD0,  Horizontal Active Register, Low,		HACTIVE_LO	= 0xD0	*/	
#endif
/*================================================================*/

	{0x0C, 0xCC},		/* RESET : 0xCC,  Control Register I (CNTRL1),	*/
	{0x0D, 0x15},		/* RESET : 0x15,  CC Control,		*/
	
	{0x10, 0xE5}, 	/* RESET : 0x00,  BRIGHTNESS Control Register (BRIGHT),	*/
	{0x11, 0x7F}, 	/* RESET : 0x5C,  CONTRAST Control Register (CONTRAST),	*/
	{0x12, 0x63},		/* RESET : 0x11,  SHARPNESS Control Register I (SHARPNESS),	*/
	{0x13, 0x9A},		/* RESET : 0x80,  Chroma (U) Gain Register (SAT_U),	*/
	{0x14, 0x9A},		/* RESET : 0x80,  Chroma (V) Gain Register (SAT_V),	*/
	{0x15, 0x00},		/* RESET : 0x00,  Hue Control Register (HUE),			*/
	
	{0x17, 0x31},		/* RESET : 0x30,  Vertical Peaking Control I,			*/
	{0x18, 0x44},		/* RESET : 0x44,  Coring Control Register (CORING),	*/
	{0x1C, 0x27},		/* RESET : 0x07,  Standard Selection (SDT),			*/
	{0x1D, 0x7F},		/* RESET : 0x07,  Standard Selection (SDT),			*/
	{0x1E, 0x08},		/* RESET : 0x7F,  Standard Recognition (SDTR),			*/
	{0x1F, 0x00},		/* RESET : 0x08,  Component Video Format (CVFMT),			*/
	{0x1F, 0x00},		/* RESET : 0x00,  Test Control Register (TEST),			*/
	
	{0x20, 0x50},		/* RESET : 0x50,  Clamping Gain (CLMPG),			*/
	{0x21, 0x54},		/* RESET : 0x42,  Individual AGC Gain (IAGC)			*/
	{0x22, 0xFD},		/* RESET : 0xF0,  AGC Gain (AGCGAIN)			*/
	{0x23, 0xF0},		/* RESET : 0xD8,  White Peak Threshold (PEAKWT)			*/
	{0x24, 0xB8},		/* RESET : 0xBC,  Clamp level (CLMPL)			*/
	{0x25, 0xBB},		/* RESET : 0xB8,  Sync Amplitude (SYNCT)			*/
	{0x26, 0x44},		/* RESET : 0x44,  Sync Miss Count Register (MISSCNT)		*/
	{0x27, 0x3B},		/* RESET : 0x2A,  Clamp Position Register (PCLAMP)		*/
	{0x28, 0x00},		/* RESET : 0x00,  Vertical Control I	*/
	{0x29, 0x00},		/* RESET : 0x00,  Vertical Control II	*/
	{0x2A, 0x78},		/* RESET : 0x78,  Color Killer Level Control	*/
	{0x2B, 0x44},		/* RESET : 0x44,  Comb Filter Control	*/
	{0x2C, 0x30},		/* RESET : 0x30,  Luma Delay and HFilter Control*/
	{0x2D, 0x14},		/* RESET : 0x14,  Miscellaneous Control Register I (MISC1)*/
	{0x2E, 0xAB},		/* RESET : 0xA5,  Miscellaneous Control Register II (MISC2)*/
	{0x2F, 0xE6},		/* RESET : 0xE0,  Miscellaneous Control III (MISC3)*/
	//{0x2F, 0x00},	//hws
	{0x33, 0x35},		/* RESET : 0x05,  CLAMP MODE(CLMD) */
	{0x34, 0x1A},		/* RESET : 0x1A,  ID Detection Control (NSEN/SSEN/PSEN/WKTH)*/
	{0x35, 0x03},		/* RESET : 0x00,  Clamp Control (CLCNTL)*/

	{0x38, 0x8E},		/* RESET : 0x80,  Anti-Aliasing Filter and Decoder Control*/
	//{0x38, 0x00},	//hws

/*==============================================================*/
/*		Flat Panel Display Registers		*/
/*===============================================================*/
/*==============================================================*/
/*		Scalar input control			*/
/*===============================================================*/



/*===============================================*/
/*		WVGA, 800X480  24bit LCD, TM070RDH11					*/

/*		VSYNC = LOW, HSYNC = LOW, DE = HIGH, PCLK=Faling 			*/	
/*		HSYNC Period = 928,			
		HSYNC Low width = 48
		Horizontal Display Period = 800
		Horizontal Back porch = 88
		Horizontal Front porch = 40

		VSYNC(Frame) Period = 525
		VSYNC Low width = 3
		Vertical Display Period = 480
		Vertical Back porch = 32
		Vertical Front porch = 13

/*=================================================*/
	
	{0x40, 0x07},		/* RESET : 0x00,	Scaler Input Control Register (1) , 	DE_POL = 0, HS_POL=1, VS_POL=1, CK_POL=1 */	
	{0x41, 0x28},		/* RESET : 0x20,	Scaler Input Control Register (2), 	HA is defined by individual video source, */	
	{0x42, 0x04},		/* RESET : 0x04,	Scaler Input Control Register (3), 	DTVD[23:16] = R, DTVD[15:8] = G, DTVD[7:0] = B */
	{0x43, 0x00},		/* RESET : 0x20,	Scaler Input Control Register (4), 	PCK 12mA*/
	{0x44, 0x0E},		/* RESET : 0x08,	Scaler Input Control Register (5), 	Normal mode, RGB, DTV input port for YUV format */
	
	{0x45, 0x54},		/* RESET : 0x54,	 Scaler Input Control Register (6), */

	{0x46, 0x20},		/* RESET : 0x20,	Csync and vs Offset */

	{0x49, 0x30}, 	/* RESET : 0x20,	Input H-active High */	
	{0x47, 0x00},		/* RESET : 0x00,	Input H-active Start Low */
	{0x48, 0x20},		/* RESET : 0xCF,	Input H-active End Low*/	

	{0x4D, 0x10}, 	/* RESET : 0x30,	Input V-active Length and Start High  */
	{0x4A, 0x00},		/* RESET : 0x13,	Input V-active Start Low for Odd Filed  */
	{0x4B, 0x00},		/* RESET : 0x13,	Input V-active Start Low for Even Filed  */
	{0x4C, 0xE0},		/* RESET : 0x00,	Input V-active Length Low */
	
	/*===============================================*/

	{0x4E, 0x70}, 		/* RESET : 0x00,	GPIO Selection */
	{0x4F, 0x10}, 		/* RESET : 0x00,	GPIO 1 */
	{0x50, 0x00}, 		/* RESET : 0x00,	GPIO 2 */


/*====================================================*/
/*	Zoom Control Registers								*/

/*	X_SCALE_UP = 65536 * 800/800 = 0x10000							*/
/*	X_SCALE_DOWN = 128 * 800/800 = 0x80								*/
/*	Y_SCALE_UP/DOWN = 65536 * 480/480 = 0x10000						*/
	    
/*====================================================*/

	
//	{0x63, 0x15},		/* RESET : 0x00,	Zoom Control Register ,Y_SCALE_UP/DOWN_HI=1,  ,X_SCALE_UP_HI=1 , the Horizontal and Vertical scale up factors has no effects*/
	{0x63, 0x10},	
//	{0x60, 0x00},		/* RESET : 0xB4,	Zoom Control Register, 		X_SCALE_UP_MID = 0x00*/
//	{0x68, 0x00},		/* RESET : 0x00,	Zoom Control Register,		X_SCALE_UP_LO = 0x00*/	
//	{0x69, 0x00},		/* RESET : 0x00,	Zoom Control Register ,		X_SCALE_UP_LO = 0x00*/
	
//	{0x61, 0x80},		/* RESET : 0x80,	Zoom Control Register, 		X_SCALE_DOWN_LO = 0x80*/	
//	{0x64, 0x00},		/* RESET : 0x00,	Zoom Control Register , 	Horizontal (X-Direction) Scale Up Offset*/
//	{0x66, 0x00},		/* RESET : 0x00,	Zoom Control Register ,		Horizontal non-display pixel number applied to both left and right sides.*/

	
//	{0x62, 0x00},		/* RESET : 0x50,	Zoom Control Register,		Y_SCALE_UP/DOWN_MID=0x00*/
//	{0x6A, 0x00},		/* RESET : 0x00,	Zoom Control Register 		Y_SCALE_UP/DOWN_LOW=0x00*/
//	{0x65, 0x00},		/* RESET : 0x80,	Zoom Control Register, 		Vertical (Y-Direction) Scale Up Offset for Odd field */
//	{0x6B, 0x00},		/* RESET : 0x00,	Zoom Control Register,		Vertical (Y-Direction) Scale Up Offset for Even field*/

/*==================================================*/
/*	 Image Adjustment Registers
/*==================================================*/	
	{0x70, 0x60},		/* RESET : 0x20,	Image Adjustment, 	Select the 1st set, R/G/B Contrast and R Brightness , Hue Adjustment for Digital Input Port.*/

/**********************************/
/* Red, Green, Blue Contrast adjustment */
/**********************************/
	
	{0x71, 0x80},		/* RESET : 0x80,	Image Adjustment , 	Red (or Y) Contrast Adjustment for all input sources*/
	{0x72, 0x80},		/* RESET : 0x80,	Image Adjustment , 	Green (or Cb) Contrast Adjustment for all input sources*/
	{0x73, 0x80},		/* RESET : 0x80,	Image Adjustment ,	Blue (or Cr) Contrast Adjustment for all input sources*/
	
/**********************************/
/* Red, Green, Blue Brightness adjustment */
/**********************************/	
	{0x74, 0x80},		/* RESET : 0x80,	Image Adjustment ,	Red (or Y) Brightness Adjustment for all input sources*/
	{0x75, 0x80},		/* RESET : 0x80,	Image Adjustment,		Green Brightness Adjustment for all input sources */
	{0x76, 0x80},		/* RESET : 0x80,	Image Adjustment,		Blue Brightness Adjustment for all input sources*/


	{0x77, 0x00},		/* RESET : 0x3F,	Image Adjustment,		Coring function for peaking control = 0,Peaking adjustment=0 .*/
	{0x78, 0x00},		/* RESET : 0x0A,	Image Adjustment , 	Sharpness frequency = high freq, Sharpness adjustment = 0*/
/*==================================================*/
/*	Black/White Stretch Adjustment Registers
/*==================================================*/

	{0x7C, 0x0C},		/* RESET : 0x10,	Black/White Stretch Adjustment */
	{0x7D, 0x08},		/* RESET : 0x08,	Black/White Stretch Adjustment,		Y Min/Max detection window start line, lower 8 bits ( total 10 bits). */
	{0x7E, 0xF6},		/* RESET : 0xF6,	Black/White Stretch Adjustment,		Y Min/Max detection window line end, lower 8 bits ( total 10 bits).*/
	{0x7F, 0x08},		/* RESET : 0x08,	Black/White Stretch Adjustment,		Y Min/Max detection window line end, upper 2 bits.Y Min/Max detection window start line, upper 2 bits.,*/	
	{0x80, 0x10},		/* RESET : 0x10,	Black/White Stretch Adjustment,		BWHDLY, Y Min/Max detection window H margin from Start/End pixel of HACTIVE.*/	
	{0x81, 0x0D},		/* RESET : 0x0D,	Black/White Stretch Adjustment,		Y Min/Max Horizontal filter gain.*/	
	{0x82, 0x03},		/* RESET : 0x03,	Black/White Stretch Adjustment,		Y Min/Max Vertical filter gain.*/	
	{0x83, 0x00},		/* RESET : 0x00,	Black/White Stretch Adjustment,		Minimum required Y difference for BW stretch. */	

	{0x84, 0x42},		/* RESET : 0x67,	Black/White Stretch Adjustment,		Tilt point for black stretch.*/	
	{0x85, 0xAE},		/* RESET : 0x94,	Black/White Stretch Adjustment ,		Tilt point for white stretch.*/
	{0x86, 0x20},		/* RESET : 0x18,	Black/White Stretch Adjustment,		Maximum Ymin for Black stretch. If Ymin is bigger than this value */	
	{0x87, 0xFF},		/* RESET : 0xE8,	Black/White Stretch Adjustment,		Minimum Ymax for White stretch.*/
	{0x88, 0xCA},		/* RESET : 0xC0,	Black/White Stretch Adjustment,		Adjust White stretch gain for smoother transient = 1,Adjust Black stretch gain for smoother transient=1*/
	{0x89, 0x40},		/* RESET : 0x02,	Black/White Stretch Adjustment,		Black/White Stretch Field recursive filter gain. */


/*==================================================*/
/*	PANEL CONTROL
/*==================================================*/
/*===============================================*/
/*		WVGA, 800X480  24bit LCD, TM070RDH11					*/

/*		VSYNC = LOW, HSYNC = LOW, DE = HIGH, PCLK=Faling 			*/	
/*		HSYNC Period = 928,			
		HSYNC Low width = 48
		Horizontal Display Period = 800
		Horizontal Back porch = 88
		Horizontal Front porch = 40

		VSYNC(Frame) Period = 525
		VSYNC Low width = 3
		Vertical Display Period = 480
		Vertical Back porch = 32
		Vertical Front porch = 13

/*=================================================*/


/*====================================================*/
/*	TM070RDH11-, 800x 480 X 24bit LCD
/*====================================================*/
/*		VSYNC = LOW, HSYNC = LOW, DE = HIGH, PCLK=Faling 					*/	
/*	FPHS PERIOD(HSYNC  period)				=928(0x3A0) 					*/
/*	FPHS ACTIVE(HSYNC  Low width) 				=48(0x30) 					*/
/*	FPHS H_BACK_PORCH(Horizontal Back porch) 	=88(0x58), 88-48=40(0x28)		*/ 
/*	FPDE_ACTIVE(Horizontal display period) 		=800(0x320)					*/

/*	FPVS_PERIOD(VSYNC period)				= 525(0x200)					*/
/*	FPVS_ACTIVE(VSYNC  Low width) 			=3(0x03)					*/
/*	FPVS V_BACK_PORCH(Verical Back porch) 		=32(0x20), 30-3=27(0x1B)		*/ 
/*	FP_V_ACTIVE(Horizontal display period) 		=480(0x1E0)					*/
/*====================================================*/

	{0xB0, 0x51},		/* RW - RESET : 0x40,	Panel control Register ,FPDE = 1,FPHS = 0,FPVS = 1 ,FPCLK = 0 */
	
	{0xB1, 0x00},		/* RW - RESET : 0x00,	Panel control Register ,No delay time inserted , TCON Mode off*/
	
	
	{0xB6, 0x33},		/* RW - RESET : 0x41,	Panel control Register,FPDE Horizontal Active Length=0x03 . High, FPHS Period_ High = 0x03 */
	

	{0xB2, 0xA0},		/* RW - RESET : 0x0E,	Panel control Register ,FPHS Period - Low Byte  , HSYNC Period = 928 ( 0x3A0)*/
	{0xB3, 0x30},		/* RW - RESET : 0x10,	Panel control Register , FPHS Active Pulse Width , HSYNC Low width = 48(0x30)*/
	{0xB4, 0x28},		/* RW - RESET : 0x1B,	Panel control Register Flat Panel Horizontal Back Porch Width, Horizontal Back porch = 88-48=40(0x28)*/
	{0xB5, 0x20},		/* RW - RESET : 0x00,	Panel control Register,FPDE Horizontal Active Length,  Horizontal display period = 800(0x320)*/	
	

	{0xBB, 0x12},		/* RW - RESET : 0x33,	Panel control Register,Flat Panel Vertical Active Length - High=0x01 , FPVS Period-High =0x02 */
	
	{0xB7, 0x0D},		/* RW - RESET : 0x26,	Panel control Register ,FPVS Period - Low Byte , VSYNC(Frame) Period = 525(0x20D)	*/	
	{0xB8, 0x03},		/* RW - RESET : 0x06,	Panel control Register ,FPVS Active Pulse Width , VSYNC Low width = 3(0x03)	*/	
	{0xB9, 0x1B},		/* RW - RESET : 0x1F,	Panel control Register ,Flat Panel Vertical Back Porch Width , Vertical Back porch = 30-3=27(0x1B))*/		
	{0xBA, 0xE0},		/* RW - RESET : 0x00,	Panel control Register ,Flat Panel Vertical Active Length - Low Byte , Vertical Display Period = 480(0x1E0)	*/	
	
	
	{0xBC, 0x00},		/* RW - RESET : 0x00,	Panel control Register */	
	{0xBD, 0x00},		/* RW - RESET : 0x08,	Panel control Register , Output Vsync delay from Input Vsync*/	

	{0xBE, 0x12},		/* RW - RESET : 0x00,	Panel control Register ,	Auto calculation enable, PWM Active LOW*/	
	
	{0xBF, 0x00},		/* RW - RESET : 0x00,	Panel control Register */	


/*================================================*/
	
	{0xC0, 0x00},		/* RESET : 0x00,	Panel control Register */	
	{0xC1, 0x00},		/* RESET : 0x00,	Panel control Register */
	{0xC2, 0x00},		/* RESET : 0x00,	Panel control Register */
	{0xC3, 0x00},		/* RESET : 0x00,	Panel control Register */
	{0xC4, 0x40},		/* RESET : 0x40,	Panel control Register ,		White LED PWM= 210.9KHZ, PWM COUNTER = 0x40*/
	{0xC6, 0x00},		/* RESET : 0x00,	Panel control Register */
	
/*==================================================*/
/*	LADC and MCU Control Registers	*/
/*==================================================*/	
	
	{0xC7, 0x00},		/* RESET : 0x00,	MCU Wake Up Control */
	{0xC8, 0x00},		/* RESET : 0x00,	MCU Debug and LADC Control Register */
  
/*==================================================*/
/*	Status and Interrupt Registers	*/
/*==================================================*/
	{0xD2, 0xFF},		/* RESET : 0xFF,	IRQ Mask Register */
	{0xD3, 0x07},		/* RESET : 0x00,	IRQ Enable Register*/
/*==================================================*/
/*	Power Management Registers	*/
/*==================================================*/
	
	{0xD4, 0x00},		/* RESET : 0x00,	 Power Management Register,	MSB of an internal 23 bit divide down counter.*/
	{0xD5, 0x3F},		/* RESET : 0x00,	 Power Management Register,	PWR_STATE = ON, Manual power sequencing control.auto power sequencing.Power state steering=ON*/
	{0xD6, 0x00},		/* RESET : 0x00,	 Power Management Register,	Timer Counts for Suspend State to Standby State Transition,	Timer Counts for On State to Suspend State Transition*/
	{0xD7, 0x00},		/* RESET : 0x00,	 Power Management Register,	Timer Counts for Power Off State to Standby State Transition,	Timer Counts for Standby State to Power Off State Transition*/
	{0xD8, 0x00},		/* RESET : 0x00,	 Power Management Register,	Timer Counts for Standby State to Suspend Sate Transition,	Timer Counts for Suspend to On State Transition*/


/*==================================================*/
/*	Color Enhancement	*/
/*==================================================*/

	{0xDA, 0x9F},		/* RESET : 0x3D,	 Color Enhancement Center 0 Registers*/
	{0xDB, 0x39},		/* RESET : 0xC3,	 Color Enhancement Center 1 Registers*/
	{0xDC, 0xF0},		/* RESET : 0xFC,	 Color Enhancement Center 2 Registers*/
	{0xDD, 0x76},		/* RESET : 0x00,	 Color Enhancement 0 Control Registers*/
	{0xDE, 0xF0},		/* RESET : 0x00,	 Color Enhancement 1 Control Registers*/
	{0xDF, 0xF0},		/* RESET : 0x00,	 Color Enhancement 2 Control Registers*/
	{0xE0, 0x00},		/* RESET : 0x00,	 Etc*/
	{0xF0, 0x00},		/* RESET : 0x00,	 Gamma Control Registers*/
	{0xF1, 0x00},		/* RESET : 0x00,	 Gamma Start Address Registers*/	
	{0xF4, 0x16},		/* RESET : 0x16,	 SSPLL*/
	{0xF6, 0x00},   /* RESET : 0x00,	 DAC,	Dac R Channel Gain*/    	
	{0xF7, 0x00},		/* RESET : 0x00,	 DAC,	Dac G Channel Gain*/       
	{0xF8, 0x00},		/* RESET : 0x00,	 DAC ,	Dac B Channel Gain*/   
#if 0

/*==================================================*/
/*	Spread Spectum Synthesizer Control Registers						*/
/*==================================================*/

/*==================================================*/
/*	FPCLK Control														*/
/*==================================================*/


/*===============================================*/
/*		WVGA, 800X480  24bit LCD, TM070RDH11	-40MHz	- max clock dot clock		*/
/*====================================================*/
/*	PLL Oscillation frequency = 108MHz * FPLL / 2 ^ 17 / 2^ POST					*/
/*	POST =0 																*/	
/*	FPLL = (40 * 2^17(131072) / 2^1(2) )/108 = 24272.592592592592592592592592593 = 5ED0 */
/*	DEVpp = 0 %																*/


	{0xF9, 0x00},		/* RESET : 0x00,	 SSPLL Control Registers,	FPLL[19:16]	*/
	{0xFA, 0x5E}, 	/* RESET : 0x40,	 SSPLL Control Registers,	FPLL[15:8]	*/
	{0xFB, 0xD0}, 	/* RESET : 0x00,	 SSPLL Control Registers,	FPLL[7:0]		*/	

	{0xFC, 0x40},		/* RESET : 0x40,	 SSPLL Control Registers,	FSS[7-0] */
	{0xFD, 0x30},		/* RESET : 0x30,	 SSPLL,		*/	
	{0xFE, 0x01},		/* RESET : 0x11,	 SSPLL Control Registers,	POST, PLL post divider = 0 ,VCO=27 ~ 54MHz,Charge pump currents (uA)= 2.5*/
	//{0x182, 0x0c},	

	


/*===============================================*/

	{0xFF, 0x01}, 	/* Registers Page Selection*/
	
/*==================================================*/
/*	Memory Bist Test Registers
/*==================================================*/
	{0x30, 0xF2},		/* RESET : 0xF2,	 CCFL Control I,	LEDC output select */
	{0x31, 0xAD},		/* RESET : 0xAD,	CCFL Threshold and LEDC Control*/
	{0x32, 0x02},		/* RESET : 0x04,	CCFL Control II*/
	{0x33, 0x80},		/* RESET : 0x80,	CCFL/LEDC PWM*/
	{0x34, 0x84},		/* RESET : 0x84,	CCFL Dim Frequency*/
	{0x35, 0x00},		/* RESET : 0x00,	CCFL Dim Control*/
	{0x36, 0x20},		/* RESET : -,	PWMTOP*/
	{0x40, 0x22},		/* RESET : 0x00,	GPO,	GPO11 : LCD Reset Output,	GPIO15 : White LED PWM*/
	{0x41, 0x00},		/* RESET : 0x00,	GPO,	GPO11 : LCD Reset Output Low, GPIO15 : White LED PWM OFF*/

/*==================================================*/
/*	Timing Controller Configuration Registers
/*==================================================*/
	
	{0x76, 0x00},		/* RESET : 0x00,	GPIO Pixel Count High Register*/
	{0x77, 0x5A},		/* RESET : 0x5A,	GPIO Pixel Count Low Register*/
	{0x78, 0x00},		/* RESET : 0x00,	GPIO Line Count High Register*/
	{0x79, 0x7F},		/* RESET : 0x7F,	GPIO Line Count Low Register*/
	{0x7A, 0x01},		/* RESET : 0x01,	GPIO Frame Count Register*/
	{0x7B, 0x00},		/* RESET : 0x00,	TCON and Delta RGB Misc. Control Register*/
	{0x80, 0x20}, 		/* RESET : 0x20,	Output Mode Control Register*/	
	{0x81, 0x02},		/* RESET : 0x00,	Display Control Register,	Inversion mode selection = Line inversion*/
	{0x82, 0x02},		/* RESET : 0x05,	Display Direction Control Register,	TRUDL low, TRSPT out, TCLRL low , TCSPR out*/
	{0x83, 0x0D},		/* RESET : 0x0F,	Control Signal Polarity Selection Register,	
									Row Driver Output Enable signal=high,
									Row Driver Start Pulse signal=high,
									Column Driver Latch Pulse signal=low,
									Column Driver Start Pulse signal=high
					*/
	{0x84, 0x7F},		/* RESET : 0x00,	Control Signal Generation Method Register*/
	{0x85, 0x00},		/* RESET : 0x00,	Inversion signal operating period register*/
	{0x8A, 0x02},		/* RESET : 0x02,	Special Companies LCD Module Control Register*/
	{0x8B, 0x4D},		/* RESET : 0x4D,	TCPOLP / TCPOLN Control Registers*/
	{0x8C, 0x00},		/* RESET : 0x00,	Vertical Active Start High Register*/
	{0x8D, 0x2E},		/* RESET : 0x06,	Vertical Active Start Low Register*/
	{0x8E, 0x02},		/* RESET : 0x01,	Vertical Active End High Register*/
	{0x8F, 0x16},		/* RESET : 0xE2,	Vertical Active End Low Register*/
	


/*==================================================*/
/*	Column Driver Chip Control Signals Relative Registers
/*==================================================*/

	{0x90, 0x03},		/* RESET : 0x02,	Polarity Control High Register*/
	{0x91, 0x6C},		/* RESET : 0xD0,	Polarity Control Low Register*/
	{0x92, 0x00},		/* RESET : 0x02,	Load/Latch Pulse Start High Register*/
	{0x93, 0x05},		/* RESET : 0xD0,	Load/Latch Pulse Start Low Register*/
	{0x94, 0x03},		/* RESET : 0x00,	Load/Latch Pulse Width High Register*/
	{0x95, 0x69},		/* RESET : 0x06,	Load/Latch Pulse Width Low Register*/
	{0x9A, 0x00},		/* RESET : 0x00,	Column Driver Start Pulse High Register*/
	{0x9B, 0x42},		/* RESET : 0xC8,	Column Driver Start Pulse Low Register*/
	{0x9C, 0x00},		/* RESET : 0x00,	Column Driver Start Pulse Width High Register*/
	{0x9D, 0x01},		/* RESET : 0x01,	Column Driver Start Pulse Width Low Register*/

/*==================================================*/
/* Row Driver Chip Control Signals Relative Registers
/*==================================================*/

	{0x90, 0x03},		/* RESET : 0x00,	Clock Start Pulse High Register*/
	{0x91, 0x6C},		/* RESET : 0x00,	Clock Start Pulse Low Register*/
	{0x92, 0x00},		/* RESET : 0x02,	Clock Start Pulse Width High Register*/
	{0x93, 0x05},		/* RESET : 0x30,	Clock Start Pulse Width Low Register*/
	{0x94, 0x03},		/* RESET : 0x00,	Row Start Pulse High Register*/
	{0x95, 0x69},		/* RESET : 0x06,	Row Start Pulse Low Register*/
	{0x9A, 0x00},		/* RESET : 0x00,	Row Start Pulse Width High Register*/
	{0x9B, 0x42},		/* RESET : 0x01,	Row Start Pulse Width Low Register*/
	{0x9C, 0x00},		/* RESET : 0x00,	Row Output Enable High Register*/
	{0x9D, 0x01},		/* RESET : 0x0A,	Row Output Enable High Register*/
	{0xAE, 0x00},		/* RESET : 0x00,	Row Output Enable Width High Register*/
	{0xAF, 0x2D},		/* RESET : 0x36,	Row Output Enable Width Low Register*/
	{0xB0, 0x01},		/* RESET : 0x02,	Panel type Select Register*/


/*==================================================*/
/* Analog Sense Block Register
/*==================================================*/

	{0xB1, 0x90},		/* RESET : 0x03,	Analog Sense Block Clock generation register*/
	{0xB2, 0x87},		
	{0xB3, 0x00},

/*==================================================*/
/* ADC/LLPLL Configuration Registers
/*==================================================*/

	{0xC0, 0x25},		/* RESET : 0x00,	LLPLL Input Control Register*/
	{0xC1, 0xC7},		/* RESET : R,	LLPLL Input Detection Register*/
	{0xC2, 0x00},		/* RESET : 0x00, LLPLL Control Register*/
	{0xC3, 0x03},		/* RESET : 0x03, LLPLL Divider High Register*/
	{0xC4, 0x5A},		/* RESET : 0x5A, LLPLL Divider Low Register*/
	{0xC5, 0x00},		/* RESET : 0x00, LLPLL Clock Phase Register*/
	{0xC6, 0x20},		/* RESET : 0x20, LLPLL Loop Control Register*/
	{0xC7, 0x04},		/* RESET : 0x04, LLPLL VCO Control Register*/
	{0xC8, 0x00},		/* RESET : 0x00, LLPLL VCO Control Register*/
	{0xC9, 0x06},		/* RESET : 0x06, LLPLL Pre Coast Register*/
	{0xCA, 0x06},		/* RESET : 0x06, LLPLL Post Coast Register*/
	{0xCB, 0x30},		/* RESET : 0x30, SOG Threshold Register*/
	{0xCC, 0x00},		/* RESET : 0x00, Scaler Sync Selection Register*/
	{0xCD, 0x00},		/* RESET : 0x54, PLL Initialization Register*/
	{0xD0, 0x00},		/* RESET : 0x00, Clamp Gain Control Register*/
	{0xD1, 0xF0},		/* RESET : 0xF0, Y Channel Gain Adjust Register*/
	{0xD2, 0xF0},		/* RESET : 0xF0, C Channel Gain Adjust Register*/
	{0xD3, 0xF0},		/* RESET : 0xF0, V Channel Gain Adjust Register*/
	{0xD4, 0x00},		/* RESET : 0x00, Clamp Mode Control Register*/
	{0xD5, 0x00},		/* RESET : 0x00, Clamp Start Position Register*/
	{0xD6, 0x10},		/* RESET : 0x10, Clamp Stop Position Register*/
	{0xD7, 0x70},		/* RESET : 0x70, Clamp Master Location Register*/
	{0xD8, 0x00},		/* RESET : 0x00, ADC TEST Register*/
	{0xD9, 0x10},		/* RESET : 0x04, Y Clamp Reference Register*/
	{0xDA, 0x80},		/* RESET : 0x80, C Clamp Reference Register*/
#endif
	{0xFF, 0x00},
	{0xFF, 0xFF}
	

};


#endif


