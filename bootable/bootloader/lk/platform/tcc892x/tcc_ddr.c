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

#include "config.h"
#include "debug.h"
#include <platform/globals.h>
#include "tnftl/IO_TCCXXX.h"
#include "tcc_ddr.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef BITCSET
#define BITCSET(X, CMASK, SMASK)	( (X) = ((((unsigned int)(X)) & ~((unsigned int)(CMASK))) | ((unsigned int)(SMASK))) )
#endif

#define addr_clk(x) (0x74000000+x)
#define addr_mem(x) (0x73000000+x)
#define time2cycle(time, tCK)		((int)((time + tCK - 1) / tCK))

#define denali_ctl(x) (*(volatile unsigned long *)addr_mem(0x500000+(x*4)))
#define denali_phy(x) (*(volatile unsigned long *)addr_mem(0x600000+(x)))
#define ddr_phy(x) (*(volatile unsigned long *)addr_mem(0x420400+(x)))


#define CKC_CHANGE_FUNC_ADDR        0x10003000
#define CKC_CHANGE_FUNC_SIZE        0x1000
#define CKC_CHANGE_ARG_BASE         (CKC_CHANGE_FUNC_ADDR + CKC_CHANGE_FUNC_SIZE)
#define CKC_CHANGE_ARG_SIZE         0x100
#define CKC_CHANGE_ARG(x)           (*(volatile unsigned long *)(CKC_CHANGE_ARG_BASE + (4*(x))))
#define CKC_CHANGE_STACK_TOP        0x10005000

typedef struct {
	unsigned int	pll;
	unsigned int	clkctrl;   
	unsigned int	freq;
} MEMCLK;

MEMCLK mem_clk_table[] =
{
#if defined(DRAM_DDR3)
	{0x00012C03, 0x00200018, 6000000},
	{0x00011303, 0x00200018, 5500000},
	{0x00010B03, 0x00200018, 5340000},
	{0x00010903, 0x00200018, 5300000},
	{0x0000FB03, 0x00200018, 4980000},
	{0x4101C203, 0x00200018, 4500000},
	{0x41019003, 0x00200018, 4000000},
	{0x41018603, 0x00200018, 3900000},
	{0x41017C03, 0x00200018, 3800000},
	{0x41017203, 0x00200018, 3700000},
#endif
	{0x41016803, 0x00200018, 3600000},
	{0x01015E03, 0x00200018, 3500000},
	{0x01015403, 0x00200018, 3400000},
	{0x01014A03, 0x00200018, 3300000},
	{0x01014003, 0x00200018, 3200000},
	{0x01013603, 0x00200018, 3100000},
	{0x01012C03, 0x00200018, 3000000},
#if defined(DRAM_DDR2)
	{0x01012203, 0x00200018, 2900000},
	{0x01011803, 0x00200018, 2800000},
	{0x01010E03, 0x00200018, 2700000},
	{0x01010403, 0x00200018, 2600000},
	{0x4201F403, 0x00200018, 2500000},
	{0x4201E003, 0x00200018, 2400000},
	{0x4201CC03, 0x00200018, 2300000},
	{0x4201B803, 0x00200018, 2200000},
	{0x4201A403, 0x00200018, 2100000},
	{0x42019003, 0x00200018, 2000000},
	{0x42017C03, 0x00200018, 1900000},
	{0x42016803, 0x00200018, 1800000},
	{0x02015403, 0x00200018, 1700000},
	{0x02014003, 0x00200018, 1600000},
	{0x02013003, 0x00200018, 1520000},
	{0x02012203, 0x00200018, 1450000},
	{0x02011A03, 0x00200018, 1410000},
	{0x02011403, 0x00200018, 1380000},
	{0x02010403, 0x00200018, 1300000},
	{0x0200FC03, 0x00200018, 1260000},
#endif
};

#pragma optimize( "g", off )
#pragma optimize( "g", on )

/****************************************************************************************
* FUNCTION :void InitRoutine_Start(void)
* DESCRIPTION :
* ***************************************************************************************/
volatile void InitRoutine_Start(void)
{
#if defined(DRAM_DDR3)
	volatile int i;
	register unsigned int tmp;
	uint nCL, nCWL;

	#define PLL5_VALUE      0x01012C03	// PLL5 : 600MHz for CPU
#ifdef DDR3_AXI0_FIFO_1to2_ENABLE
//	#define PLL4_VALUE      0x00012C03	// PLL4 : 1200MHz for MEM
//	#define MEM_CLK         600 // = PLL4/2
//	#define PLL4_VALUE      0x00011303	// PLL4 : 1068MHz for MEM
//	#define MEM_CLK         550 // = PLL4/2
	#define PLL4_VALUE      0x01012C03	// PLL4 : 600MHz for MEM
	#define MEM_CLK         300 // = PLL4/2
#else
//	#define PLL4_VALUE      0x41016803 // PLL4 : 720MHz for MEM
//	#define MEM_CLK         360// = PLL4/2
	#define PLL4_VALUE      0x01012C03	// PLL4 : 600MHz for MEM
	#define MEM_CLK         300 // = PLL4/2
#endif
	#define DDR3_CLK        MEM_CLK
	#define tCK (1000000/DDR3_CLK)

//--------------------------------------------------------------------------
//for debugging by j-tag
	*(volatile unsigned long *)0x77000018 = 0xf0000000;

//--------------------------------------------------------------------------
//Change to config mode

//--------------------------------------------------------------------------
//Clock setting..

	//Memory BUS Configuration : MBUSCFG
	#ifdef DDR3_AXI0_FIFO_1to2_ENABLE
	*(volatile unsigned long *)addr_mem(0x810010) |= Hw15; //dclkr=1 : this is used for DDR3 only.
	#else
	*(volatile unsigned long *)addr_mem(0x810010) &= ~Hw15; //(d)not used just for ddr3 clk div
	#endif
	i=3200; while(i--);

	*(volatile unsigned long *)addr_clk(0x000000) = 0x002ffff4; //cpu bus : XIN
	*(volatile unsigned long *)addr_clk(0x000004) = 0x00200014; //mem bus : XIN/2 
	*(volatile unsigned long *)addr_clk(0x000010) = 0x00200014; //io bus : XIN/2
	*(volatile unsigned long *)addr_clk(0x000020) = 0x00200014; //smu bus : XIN/2
	*(volatile unsigned long *)addr_clk(0x000044) = PLL5_VALUE; //pll5 for cpu
	*(volatile unsigned long *)addr_clk(0x000044) = 0x80000000 | PLL5_VALUE;
	*(volatile unsigned long *)addr_clk(0x000040) = PLL4_VALUE; //pll4 for mem
	*(volatile unsigned long *)addr_clk(0x000040) = 0x80000000 | PLL4_VALUE;
	i=3200; while(i--);
	*(volatile unsigned long *)addr_clk(0x000000) = 0x002FFFF9;  // cpu
	*(volatile unsigned long *)addr_clk(0x000004) = 0x00200018;  // mem bus
	*(volatile unsigned long *)addr_clk(0x000010) = 0x00200039;  // io bus
	*(volatile unsigned long *)addr_clk(0x000020) = 0x00200039;  // smu bus
	i=3200; while(i--);

//--------------------------------------------------------------------------
// Timing Parameter

	if(tCK >= 2500){ /* 2.5ns, 400MHz */
		nCL = 6;
		nCWL = 5;
	}else if(tCK >= 1875){ // 1.875ns, 533..MHz
		nCL = 8;
		nCWL = 6;
	}else if(tCK >= 1500){ // 1.5 ns, 666..MHz
		if(DDR3_MAX_SPEED < DDR3_1600)
			nCL = 9;
		else
			nCL = 10;
		nCWL = 7;
	}else if(tCK >= 1250){ // 1.25 ns, 800MHz
		nCL = 11;
		nCWL = 8;
	}else if(tCK >= 1070){ // 1.07 ns, 933..MHz
		nCL = 13;
		nCWL = 9;
	}else if(tCK >= 935){ // 0.935 ns, 1066..MHz
		nCL = 14;
		nCWL = 10;
	}

	denali_ctl(0) = 0x20410600; //DRAM_CLASS[11:8] = 6:DDR3, 4:DDR2
	denali_ctl(2) = 0x00000006; //TINIT[23:0] = 0x4 -==> 0x6
	denali_ctl(3) = time2cycle(20000000,tCK); //TRST_PWRON = 200us, 7 //Bruce_temp.. ns ?
	denali_ctl(4) = time2cycle(50000000,tCK); //CKE_INACTIVE = 500us, 10 //Bruce_temp.. ns ?

	//TBST_INT_INTERVAL[26:24] = 0x1, WL[20:16], CASLAT_LIN[13:8], CL half-cycle increment = 0, INITAREF[3:0] = 0x8
	if(DDR3_AL == AL_DISABLED){ //nAL = 0;
		denali_ctl(5) = (0x1<<24 | (nCWL)<<16 | ((nCL<<1)|0)<<8 | 0x8);
	}
	else if(DDR3_AL == AL_CL_MINUS_ONE){ //nAL = nCL - 1;
		denali_ctl(5) = (0x1<<24 | (nCWL+nCL-1)<<16 | ((nCL<<1)|0)<<8 | 0x8);
	}	
	else if(DDR3_AL == AL_CL_MINUS_TWO){ //nAL = nCL - 2;
		denali_ctl(5) = (0x1<<24 | (nCWL+nCL-2)<<16 | ((nCL<<1)|0)<<8 | 0x8);
	}

	denali_ctl(6) = (time2cycle(DDR3_tRAS_ps,tCK)<<24 | time2cycle(DDR3_tRC_ps,tCK)<<16 | time2cycle(DDR3_tRRD_ps,tCK)<<8 | DDR3_tCCD_ck);
	denali_ctl(7) = (DDR3_tMRD_ck<<24 | time2cycle(DDR3_tRTP_ps,tCK)<<16 | time2cycle(DDR3_tRP_ps,tCK)<<8 | (time2cycle(DDR3_tWTR_ps,tCK)+1));
	denali_ctl(8) = (time2cycle(DDR3_tRAS_MAX_ps,tCK)<<8 | time2cycle(DDR3_tMOD_ps,tCK));
	denali_ctl(9) = ((time2cycle(DDR3_tCKE_ps,tCK)+1)<<8 | time2cycle(DDR3_tCKE_ps,tCK));
	denali_ctl(10) = (time2cycle(DDR3_tWR_ps,tCK)<<24 | time2cycle(DDR3_tRCD_ps,tCK)<<16 | 1<<8 | 1);
	denali_ctl(11) = (1<<24 | DDR3_tDLLK_ck<<8 | (time2cycle(DDR3_tWR_ps,tCK)+nCL));
	denali_ctl(12) = (1<<16 | time2cycle(DDR3_tFAW_ps,tCK)<<8 | 3);
	denali_ctl(13) = time2cycle(DDR3_tRP_ps,tCK)+1;
	BITCSET(denali_ctl(14), 0x03FF0100, (time2cycle(DDR3_tRFC_ps,tCK)<<16 | 1<<8));
	denali_ctl(15) = time2cycle(DDR3_tREFI_ps,tCK);
	denali_ctl(16) = (time2cycle(DDR3_tXPDLL_ps,tCK)<<16 | time2cycle(DDR3_tXP_ps,tCK)); // DDR3 Only
	denali_ctl(17) = (6<<16 | 2); //TXARD[-0] = 0x2, TXARDS[-16] = 0x6 // DDR2 only
	denali_ctl(18) = (time2cycle(DDR3_tXS_ps,tCK)<<16 | DDR3_tXSDLL_ck);
	denali_ctl(19) |= (1<<16); //ENABLE_QUICK_SREFRESH = 0x1
	BITCSET(denali_ctl(20), 0x000F0F00, time2cycle(DDR3_tCKSRX_ps,tCK)<<16 | time2cycle(DDR3_tCKSRE_ps,tCK)<<8);

//--------------------------------------------------------------------------
// MRS Setting

	// MRS0
#if (1) //Bruce_temp_mrs0 FAST_EXIT 맞나?
	tmp = DDR3_BURST_LEN | (DDR3_READ_BURST_TYPE<<3);
#else
	tmp = DDR3_BURST_LEN | (DDR3_READ_BURST_TYPE<<3) | (FAST_EXIT<<12);
#endif
	if(nCL < 5)
		tmp |= ((5-4)<<4);
	else if(nCL > 11)
		tmp |= ((11-4)<<4);
	else
		tmp |= ((nCL-4)<<4);

	if(time2cycle(DDR3_tWR_ps,tCK) <= 5) // Write Recovery for autoprecharge
		tmp |= WR_5<<9;
	else if(time2cycle(DDR3_tWR_ps,tCK) == 6)
		tmp |= WR_6<<9;
	else if(time2cycle(DDR3_tWR_ps,tCK) == 7)
		tmp |= WR_7<<9;
	else if(time2cycle(DDR3_tWR_ps,tCK) == 8)
		tmp |= WR_8<<9;
	else if((time2cycle(DDR3_tWR_ps,tCK) == 9) || (time2cycle(DDR3_tWR_ps,tCK) == 10))
		tmp |= WR_10<<9;
	else if(time2cycle(DDR3_tWR_ps,tCK) >= 11)
		tmp |= WR_12<<9;

	tmp	|= (1<<8); // DLL Reset

	denali_ctl(27) = tmp<<8;
	BITCSET(denali_ctl(30), 0x0000FFFF, tmp);

	// MRS1
	BITCSET(denali_ctl(28), 0x0000FFFF, (DDR3_AL<<3) | (DDR3_ODT<<2) | (DDR3_DIC<<1));
	BITCSET(denali_ctl(30), 0xFFFF0000, ((DDR3_AL<<3) | (DDR3_ODT<<2) | (DDR3_DIC<<1))<<16);

	// MRS2
	tmp = 0;
	if(nCWL <= 5)
		tmp |= 0;
	else if(nCWL == 6)
		tmp |= 1<<3;
	else if(nCWL == 7)
		tmp |= 2<<3;
	else if(nCWL >= 8)
		tmp |= 3<<3;

	BITCSET(denali_ctl(28), 0xFFFF0000, tmp<<16);
	BITCSET(denali_ctl(31), 0x0000FFFF, tmp);

	// MRS3
	BITCSET(denali_ctl(29), 0xFFFF0000, 0<<16);
	BITCSET(denali_ctl(32), 0x0000FFFF, 0);

//--------------------------------------------------------------------------

	//BIST Start
	BITCLR(denali_ctl(32), 1<<16); //BIST_GO = 0x0
	denali_ctl(33) = (1<<16)|(1<<8); //BIST_ADDR_CHECK[-16] = 0x1, BIST_DATA_CHECK[-8] = 0x1
	denali_ctl(34) = 0; denali_ctl(35) = 0; denali_ctl(36) = 0; denali_ctl(37) = 0;
	//BIST End

	denali_ctl(38) = (DDR3_tZQOPER_ck<<16 | DDR3_tZQINIT_ck);
	denali_ctl(39) = (0x2<<24 | DDR3_tZQCS_ck); //ZQCS, ZQ_ON_SREF_EXIT
	denali_ctl(40) = (0x1<<16 | 0x40); //ZQCS_ROTATE=0x1, REFRESH_PER_ZQ=0x40

	// DRAM Size Setting
	if(DDR3_BANK_NUM == 8) //EIGHT_BANK_MODE
		BITSET(denali_ctl(40), 0x1<<24);
	else
		BITCLR(denali_ctl(40), 0x1<<24);
	denali_ctl(41) = 0xFF<<24|DDR3_APBIT<<16|(11-DDR3_COLBITS)<<8|(16-DDR3_ROWBITS); //ADDR_PINS, COLUMN_SIZE, APREBIT, AGE_COUNT = 0xFF
	denali_ctl(42) = 0x1<<24|0x1<<16|0x1<<8|0xFF; //COMMAND_AGE_CCOUNT = 0xff, ADDR_CMP_EN = 0x1, BANK_SPLIT_EN = 0x1 PLACEMENT_EN = 0x1
	denali_ctl(43) = 0x1<<24|0x1<<16|0x1<<8|0x1; //PRIORITY_EN = 0x1, RW_SAME_EN = 0x1,SWAP_EN = 0x1, SWAP_PORT_RW_SAME_EN = 0x1
#if defined(CONFIG_DRAM_16BIT_USED)
	if(DDR3_LOGICAL_CHIP_NUM == 2)
		denali_ctl(44) = (0x1<<24|0xc<<16|0x3<<8); //REDUC[24] = DQ 0:32BIT, 1:16BIT
	else
		denali_ctl(44) = (0x1<<24|0xc<<16|0x1<<8); //REDUC[24] = DQ 0:32BIT, 1:16BIT
#else
	if(DDR3_LOGICAL_CHIP_NUM == 2)
		denali_ctl(44) = (0x0<<24|0xc<<16|0x3<<8); //REDUC[24] = DQ 0:32BIT, 1:16BIT
	else
		denali_ctl(44) = (0x0<<24|0xc<<16|0x1<<8); //REDUC[24] = DQ 0:32BIT, 1:16BIT
#endif
	denali_ctl(45) = 0x1<<24; //RESYNC_DLL_PER_AREF_EN = 0x1 // Automatic Controller-Initiated Update

	denali_ctl(47) = 0x00020000;

//--------------------------------------------------------------------------
// ODT Setting

	BITCSET(denali_ctl(64), 0x00030000, 0x0<<16); //ODT_RD_MAP_CS0
	if(DDR3_LOGICAL_CHIP_NUM == 2)
		BITCSET(denali_ctl(65), 0x00000003, 0x0<<0); //ODT_RD_MAP_CS1

	if(DDR3_PINMAP_TYPE == 0 || DDR3_PINMAP_TYPE == 1){
		BITCSET(denali_ctl(64), 0x03000000, 0x1<<24); //ODT_WR_MAP_CS0
		if(DDR3_LOGICAL_CHIP_NUM == 2)
			BITCSET(denali_ctl(65), 0x00000300, 0x1<<8); //ODT_WR_MAP_CS1
	} else {
		BITCSET(denali_ctl(64), 0x03000000, 0x3<<24); //ODT_WR_MAP_CS0
		if(DDR3_LOGICAL_CHIP_NUM == 2)
			BITCSET(denali_ctl(65), 0x00000300, 0x3<<8); //ODT_WR_MAP_CS1
	}
	BITCSET(denali_ctl(65), 0x000F0000, 0x2<<16); //ADD_ODT_CLK_R2W_SAMECS = 0x2
	denali_ctl(66) = 0x1<<24|0x1<<16|0x2<<8|0x2; //ADD_ODT_CLK_DIFFTYPE_DIFFCS = 0x2, ADD_ODT_CLK_SAMETYPE_DIFFCS = 0x2, R2R_DIFFCS_DLY = 0x1, R2W_DIFFCS_DLY = 0x1
	denali_ctl(67) = 0x2<<24|0x0<<16|0x1<<8|0x1; //W2R_DIFFCS_DLY = 0x1, W2W_DIFFCS_DLY = 0x1, R2R_SAMECS_DLY = 0x0, R2W_SAMECS_DLY = 0x2
	BITCSET(denali_ctl(68), 0x00000707, 0x0<<8|0x0); //W2R_SAMECS_DLY = 0x0, W2W_SAMECS_DLY = 0x0
	denali_ctl(72) = 0x0<<16|0x28<<8|0x19; //WLDQSEN = 0x19, WLMRD = 0x28, WRLVL_EN = 0x0
	denali_ctl(81) = 0x21<<16|0x21; //RDLVL_DELAY_0 = 0x21, RDLVL_GATE_DELAY_0 = 0x21
	denali_ctl(84) = 0x2121<<8; //RDLVL_DELAY_1 = 0x2121
	denali_ctl(88) = 0x2121; //RDLVL_DELAY_2 = 0x2121
	denali_ctl(91) = 0x2121<<8; //RDLVL_DELAY_3 = 0x2121
	denali_ctl(92) = 0xFFFF<<16; //AXI0_EN_SIZE_LT_WIDTH_INSTR = 0xffff
	denali_ctl(93) = 0x8<<8|0x8; //AXI0_R_PRIORITY = 0x8. AXI0_W_PRIORITY = 0x8
	#ifdef DDR3_AXI0_FIFO_1to2_ENABLE // AXI : MEM Rating Setting, 0 = Async, 1 = 2:1 ( Port : Core ), 2 = 1:2 ( Port : Core ), 3 = Sync.
		BITCSET(denali_ctl(93), 0x00030000, 0x3<<16); //AXI0_FIFO_TYPE_REG[17:16] = 0x3
	#else
		BITCSET(denali_ctl(93), 0x00030000, 0x1<<16); //AXI0_FIFO_TYPE_REG[17:16] = 0x1
	#endif

//--------------------------------------------------------------------------
// DFI Timing

	// tPHY_RDLAT     : DFI_RDDATA_EN <=> DFI_RDDATA_VALID 
	// tPHY_WRLAT(RO) : Write Command <=> DFI_WRDATA_EN    : WRLAT_ADJ(min. 1) + REG_DIMM_ENABLE - 1
	// tRDDATA_EN     : Read Command  <=> DFI_RDDATA_EN    : RDLAT_ADJ(min. 2) + REG_DIMM_ENABLE - 1
	// tPHY_WRDATA    : DFI_WRDATA    <=> DFI_WRDATA_EN
	BITCSET(denali_ctl(95), 0x3F000000, 0x8<<24); //TDFI_PHY_RDLAT = 0x8
	BITCLR(denali_ctl(96), 0x3<<8); //DRAM_CLK_DISABLE[9:8] = [CS1, CS0] = 0x0
	denali_ctl(97) = 0x200<<16|0x50E; //TDFI_CTRLUPD_MAX = 0x50e, TDFI_PHYUPD_TYPE0 = 0x200
	denali_ctl(98) = 0x200<<16|0x200; //TDFI_PHYUPD_TYPE1 = 0x200, TDFI_PHYUPD_TYPE2 = 0x200
	denali_ctl(99) = 0x50E<<16|0x200; //TDFI_PHYUPD_TYPE3 = 0x200, TDFI_PHYUPD_RESP = 0x50e
	denali_ctl(100) = 0x1946; //TDFI_CTRLUPD_INTERVAL = 0x1946
	if(DDR3_AL == AL_DISABLED){ //nAL = 0;
		denali_ctl(101) = (0x1<<24|0x1<<16|nCWL<<8|(nCL+5));
	}
	else if(DDR3_AL == AL_CL_MINUS_ONE){ //nAL = nCL - 1;
		denali_ctl(101) = (0x1<<24|0x1<<16|(nCWL+nCL-1)<<8|(nCL+5));
	}	
	else if(DDR3_AL == AL_CL_MINUS_TWO){ //nAL = nCL - 2;
		denali_ctl(101) = (0x1<<24|0x1<<16|(nCWL+nCL-2)<<8|(nCL+5));
	}
	denali_ctl(102) = 0x3<<24|0x8000<<8|0x1; //TDFI_DRAM_CLK_ENABLE = 0x1, DFI_WRLVL_MAX_DELAY = 0x8000, TDFI_WRLVL_EN = 0x3
	denali_ctl(103) = 0x4<<16|0x7<<8|0x3; //TDFI_WRLVL_DLL = 0x3, TDFI_WRLVL_LOAD = 0x7, TDFI_WRLVL_RESPLAT = 0x4
	denali_ctl(104) = 0xA; //TDFI_WRLVL_WW = 0xa
	denali_ctl(107) = 0x10<<16|0xFFFF; //RDLVL_MAX_DELAY = 0xffff, RDLVL_GATE_MAX_DELAY = 0x10
	denali_ctl(108) = 0x12<<24|0x7<<16|0x3<<8|0x3; //TDFI_RDLVL_EN = 0x3, TDFI_RDLVL_DLL = 0x3, TDFI_RDLVL_LOAD = 0x7, TDFI_RDLVL_RESPLAT = 0x12 
	denali_ctl(109) = 0xF; //TDFI_RDLVL_RR = 0xf
	denali_ctl(115) = 0x2<<8|0x4; //RDLVL_DQ_ZERO_COUNT = 0x4, RDLVL_GATE_DQ_ZERO_COUNT = 0x2
	denali_ctl(118) = 1; //ODT_ALT_EN = 0x1

//--------------------------------------------------------------------------
// Denali phy setting

	denali_phy(0x10) = 0x10680000; //Bruce_temp, 현재 ZQ Cal 동작 안되서 강제로 focing한 값을 사용함
	denali_phy(0x0C) = 0x01801010;
	denali_phy(0x1C) = 0x00316000|(MEMCTRL_TERM<<8)|(MEMCTRL_DDS<<4); //Bruce_temp, zq forcing valu (일단 이값을 사용해야함)

//--------------------------------------------------------------------------
// DDR phy setting

	//PHY Mode contrl register (PHYMDCFG0)
	ddr_phy(0x0000) = (0 << 21) |    //ctrl_lat                  //Bruce_temp_8920 ??
	                  (0x1f << 16) | //ctrl_wake_up
	                  (DDR3_PINMAP_TYPE << 14) |    //pin_map
	                  ((DDR3_LOGICAL_CHIP_NUM-1) << 13) | //cs2s
	                  (0 << 4) |     //mode                        //Bruce_temp_8920 ??
	                  (0 << 3) |     //c_type : denali=0, lpcon=1
	                  (1);           //dram_type : ddr2=0, ddr3=1, lpddr=2, lpddr2=3

//--------------------------------------------------------------------------
// Start denali MC init
	BITSET(denali_ctl(0), 1); //START = 1
	if(denali_ctl(46)&(1<<5))
	{
		BITSET(denali_ctl(47),(1<<5));
		while(denali_ctl(46)&(1<<5));
	}
	BITCLR(denali_phy(0x08),1<<1); //ctrl_dll_on_rst = 0
	for (i=0; i<5; i++);
	BITSET(denali_phy(0x08),1<<1); //ctrl_dll_on_rst = 1
	while(!(denali_phy(0x28)&(1<<12))); //ctrl_zq_end, ZQ End ...
	while(!(denali_phy(0x24)&(1<<10))); //ctrl_locked, DLL Locked ...
	while(!(denali_ctl(46)&0x20));

	//denali_ctl(47) |= 0x20;
	denali_phy(0x1C) = 0x00316000|(MEMCTRL_TERM<<8)|(MEMCTRL_DDS<<4);
	denali_phy(0x14) = 0x0A0A0A0A;		// Denali Phy Control Reg_02

//--------------------------------------------------------------------------

#elif defined(DRAM_DDR2)

	volatile int i;
	register unsigned int tmp;

	#define PLL5_VALUE      0x01012C03	// PLL5 : 600MHz for CPU
	//#define PLL4_VALUE      0x4201A403 //for 210 //0x01012C03	// PLL4 : 600MHz for MEM
	//#define MEM_CLK         210//300 // = PLL4/2
	#define PLL4_VALUE      0x01012C03	// PLL4 : 600MHz for MEM
	#define MEM_CLK         300 // = PLL4/2

	#define DDR2_CLK        MEM_CLK
	#define tCK (1000000/DDR2_CLK)

//--------------------------------------------------------------------------
//for debugging by j-tag
	*(volatile unsigned long *)0x77000018 = 0xf0000000;

//--------------------------------------------------------------------------
//Change to config mode

	//Miscellaneous Configuration : COMMON
	*(volatile unsigned long *)addr_mem(0x410020) &= ~Hw17; //creq2=0 : enter low power
	while((*(volatile unsigned long *)addr_mem(0x410020))&Hw24); //cack2==0 : wait for acknowledgement to request..

#if (0)
	//MEMCTRL
	//*(volatile unsigned long *)addr_mem(0x430004) |= 0x00000001; //clk_stop_en=1

	if(DDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr_mem(0x430048)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
	else//DDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr_mem(0x430048)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED

	if(DDR2_LOGICAL_CHIP_NUM == 2){
		if(DDR2_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr_mem(0x43004C)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
		else//DDR2_BANK_NUM == 4
			while (((*(volatile unsigned long *)addr_mem(0x43004C)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
	}
#endif

//--------------------------------------------------------------------------
//Clock setting..

	//Memory BUS Configuration : MBUSCFG
	*(volatile unsigned long *)addr_mem(0x810010) &= ~Hw15; //dclkr=0 : this is used for DDR3 only.
	i=3200; while(i--);

	*(volatile unsigned long *)addr_clk(0x000000) = 0x002ffff4; //cpu bus : XIN
	*(volatile unsigned long *)addr_clk(0x000004) = 0x00200014; //mem bus : XIN/2 
	*(volatile unsigned long *)addr_clk(0x000010) = 0x00200014; //io bus : XIN/2
	*(volatile unsigned long *)addr_clk(0x000020) = 0x00200014; //smu bus : XIN/2
	*(volatile unsigned long *)addr_clk(0x000044) = PLL5_VALUE; //pll5 for cpu
	*(volatile unsigned long *)addr_clk(0x000044) = 0x80000000 | PLL5_VALUE;
	*(volatile unsigned long *)addr_clk(0x000040) = PLL4_VALUE; //pll4 for mem
	*(volatile unsigned long *)addr_clk(0x000040) = 0x80000000 | PLL4_VALUE;
	i=3200; while(i--);
	*(volatile unsigned long *)addr_clk(0x000000) = 0x002FFFF9;  // cpu
	*(volatile unsigned long *)addr_clk(0x000004) = 0x00200018;  // mem bus
	*(volatile unsigned long *)addr_clk(0x000010) = 0x00200039;  // io bus
	*(volatile unsigned long *)addr_clk(0x000020) = 0x00200039;  // smu bus
	i=3200; while(i--);


//--------------------------------------------------------------------------
// Controller setting

	//PHY Mode contrl register (PHYMDCFG0)
	*(volatile unsigned long *)addr_mem(0x420400) = (0 << 21) | //ctrl_lat                  //Bruce_temp_8920 ??
	                                             (0x1f << 16) | //ctrl_wake_up
	                                             (DDR2_PINMAP_TYPE << 14) | //pin_map
	                                             ((DDR2_LOGICAL_CHIP_NUM-1) << 13) | //cs2s
	                                             (0 << 4) | //mode                        //Bruce_temp_8920 ??
	                                             (1 << 3) | //c_type : denali=0, lpcon=1
	                                             (0); //dram_type : ddr2=0, ddr3=1, lpddr=2, lpddr2=3



	//PhyZQControl
	if (DDR2_CLK >= DDR2_ODT_ENABLE_MIN_FREQ) {
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL ;
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL | Hw1 ;//zq start
	} else {
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL | Hw0;
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL | Hw1 | Hw0 ;//zq start
	}
	while (((*(volatile unsigned long *)addr_mem(0x430040)) & (0x10000)) != 0x10000);	// Wait until ZQ End

	if (DDR2_CLK >= DDR2_ODT_ENABLE_MIN_FREQ) {
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL ;
	} else {
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL | Hw0;
	}

	*(volatile unsigned long *)addr_mem(0x430018 ) = 0x0010100A; //PHY Control0
	*(volatile unsigned long *)addr_mem(0x43001C ) = 0xE0000086; //PHY Control1 // modify by crony : [31:30] : ODT Enable for Write and Read
	*(volatile unsigned long *)addr_mem(0x430020 ) = 0x00000000; //PHY Control2
	*(volatile unsigned long *)addr_mem(0x430024 ) = 0x00000000; //PHY Control3
	*(volatile unsigned long *)addr_mem(0x430018 ) = 0x0010100B; //PHY Control0

	while (((*(volatile unsigned long *)addr_mem(0x430040)) & (0x7)) != 0x7);// Wait until FLOCK == 1

	//PHY Control1
	*(volatile unsigned long *)addr_mem(0x43001C) = 0xE000008E; //resync = 1
	*(volatile unsigned long *)addr_mem(0x43001C) = 0xE0000086; //resync = 0

//--------------------------------------------------------------------------
// Memory config

	//Enable Out of order scheduling
	*(volatile unsigned long *)addr_mem(0x430000 ) = 0x30FF2018;

	//MEMCTRL
	*(volatile unsigned long *)addr_mem(0x430004 ) = (0x2 << 20) |
	                                             ((DDR2_LOGICAL_CHIP_NUM-1)<<16) |
	                                             ((DDR2_LOGICAL_DATA_BITS/16)<<12) |
	                                             (0x4 << 8) |
	                                             (0x0 << 6) |
	                                             (0x0 << 5) |
	                                             (0x0 << 4) |
	                                             (0x0 << 2) |
	                                             (0x0 << 1) |
	                                             (0x0);

	//MEMCHIP0
	*(volatile unsigned long *)addr_mem(0x430008 ) = (0x80<<24) |
	                                             ((0xFF - (DDR2_TOTAL_MB_SIZE/(DDR2_LOGICAL_CHIP_NUM*0x10)-1))<<16) |
	                                             (0x1 << 12) |
	                                             ((DDR2_COLBITS - 7)<<8) |
	                                             ((DDR2_ROWBITS - 12)<<4) |
	                                             DDR2_BANK_BITS;

	//MEMCHIP1
	if(DDR2_LOGICAL_CHIP_NUM == 2)
	*(volatile unsigned long *)addr_mem(0x43000C ) = ((0x80 + DDR2_TOTAL_MB_SIZE/(DDR2_LOGICAL_CHIP_NUM*0x10))<<24) |
		                                         ((0xFF - (DDR2_TOTAL_MB_SIZE/(DDR2_LOGICAL_CHIP_NUM*0x10)-1))<<16) |
		                                         (0x1 << 12) |
		                                         ((DDR2_COLBITS - 7)<<8) |
		                                         ((DDR2_ROWBITS - 12)<<4) |
		                                         DDR2_BANK_BITS;

//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr_mem(0x430014 ) = 0x0; //PRECONFIG
	*(volatile unsigned long *)addr_mem(0x430028 ) = 0xFFFF00FF; //PRECONFIG

//--------------------------------------------------------------------------
// Timing parameter setting.

	//T_REFI
	*(volatile unsigned long *)addr_mem(0x430030 ) = time2cycle(DDR2_tREFI_ps, tCK);

	//TROW
	*(volatile unsigned long *)addr_mem(0x430034 ) = time2cycle(DDR2_tRAS_ps, tCK); //tRAS
	*(volatile unsigned long *)addr_mem(0x430034 ) |= (time2cycle(DDR2_tRC_ps, tCK)<<6); //tRC
	*(volatile unsigned long *)addr_mem(0x430034 ) |= (DDR2_CL<<12); //tRCD
	*(volatile unsigned long *)addr_mem(0x430034 ) |= (DDR2_CL<<16); //tRP
	*(volatile unsigned long *)addr_mem(0x430034 ) |= (time2cycle(DDR2_tRRD_ps, tCK)<<20); //tRRD
	*(volatile unsigned long *)addr_mem(0x430034 ) |= (time2cycle(DDR2_tRFC_ps, tCK)<<24); //tRFC

	//TDATA
	*(volatile unsigned long *)addr_mem(0x430038 ) = DDR2_CL; //tRL
	*(volatile unsigned long *)addr_mem(0x430038 ) |= ((DDR2_CL-1)<<8); //tWL
	*(volatile unsigned long *)addr_mem(0x430038 ) |= (DDR2_CL<<16); //tCL
	tmp = time2cycle(DDR2_tRTP_ps, tCK);
	if(tmp<DDR2_tRTP_ck) tmp=DDR2_tRTP_ck;
	*(volatile unsigned long *)addr_mem(0x430038 ) |= (tmp<<20); //tRTP
	*(volatile unsigned long *)addr_mem(0x430038 ) |= (time2cycle(DDR2_tWR_ps, tCK)<<24); //tWR
	tmp = time2cycle(DDR2_tWTR_ps, tCK);
	if(tmp<DDR2_tWTR_ck) tmp=DDR2_tWTR_ck;
	*(volatile unsigned long *)addr_mem(0x430038 ) |= (tmp<<28); //tWTR

	//TPOWER
	*(volatile unsigned long *)addr_mem(0x43003C ) = DDR2_tMRD_ck; //tMRD
	*(volatile unsigned long *)addr_mem(0x43003C ) |= (DDR2_tCKE_ck<<4); //tCKE
	*(volatile unsigned long *)addr_mem(0x43003C ) |= (DDR2_tXP_ck<<8); //tXP
	*(volatile unsigned long *)addr_mem(0x43003C ) |= (DDR2_tXSR_ck<<16); //tXSR
	*(volatile unsigned long *)addr_mem(0x43003C ) |= (time2cycle(DDR2_tFAW_ps, tCK)<<24); //tFAW

//--------------------------------------------------------------------------
// MRS Setting

	//Direct Command
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x07000000;//NOP
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x01000000;//precharge all
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00020000;
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00030000;
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010000;
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00000100;
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x01000000;//precharge all
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x05000000;//AREF
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x05000000;//AREF
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x05000000;//AREF
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00000000;	// DLL reset release.
	*(volatile unsigned long *)addr_mem(0x430010 ) = (DDR2_BURST_LEN|(DDR2_READ_BURST_TYPE<<3)|(DDR2_CL<<4)|((time2cycle(DDR2_tWR_ps, tCK)-1)<<9));
	i = 100; while(i--);
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010380; // OCD Calibration default
	i = 100; while(i--);
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010000 | (DDR2_ODT<<2) | (DDR2_DIC<<1);

	if(DDR2_LOGICAL_CHIP_NUM == 2)
	{
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x07000000 | Hw20;//NOP
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x01000000 | Hw20;//precharge all
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00020000 | Hw20;
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00030000 | Hw20;
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010000 | Hw20;
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00000100 | Hw20;
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x01000000 | Hw20;//precharge all
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x05000000 | Hw20;//AREF
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x05000000 | Hw20;//AREF
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x05000000 | Hw20;//AREF
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00000000 | Hw20;	// DLL reset release.
		*(volatile unsigned long *)addr_mem(0x430010 ) = (DDR2_BURST_LEN|(DDR2_READ_BURST_TYPE<<3)|(DDR2_CL<<4)|((time2cycle(DDR2_tWR_ps, tCK)-1)<<9)) | Hw20;
		i = 100; while(i--);
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010380 | Hw20; // OCD Calibration default
		i = 100; while(i--);
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010000 | (DDR2_ODT<<2) | (DDR2_DIC<<1) | Hw20;
	}

//--------------------------------------------------------------------------

	//Miscellaneous Configuration : COMMON
	*(volatile unsigned long *)addr_mem(0x410020) |= Hw17; //creq2=1 : exit low power
	while(!((*(volatile unsigned long *)addr_mem(0x410020))&Hw24)); //cack2==1 : wait for acknowledgement to request..

	*(volatile unsigned long *)addr_mem(0x430000 ) |= 0x00000020; //CONCONTROL : aref_en=1

	if(DDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr_mem(0x430048)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
	else//DDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr_mem(0x430048)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED

	if(DDR2_LOGICAL_CHIP_NUM == 2){
		if(DDR2_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr_mem(0x43004C)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
		else//DDR2_BANK_NUM == 4
			while (((*(volatile unsigned long *)addr_mem(0x43004C)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED
	}

//--------------------------------------------------------------------------

#endif

}

/****************************************************************************************
* FUNCTION :void InitRoutine_End(void)
* DESCRIPTION :
* ***************************************************************************************/
volatile void InitRoutine_End(void)
{

}

/*===========================================================================
TYPE
===========================================================================*/
#if defined(DRAM_DDR3)
enum {
	PLL_VALUE = 0,
	CKC_CTRL_VALUE,
	CLK_RATE,
	DENALI_CTL_3,
	DENALI_CTL_4,
	DENALI_CTL_5,
	DENALI_CTL_6,
	DENALI_CTL_7,
	DENALI_CTL_8,
	DENALI_CTL_9,
	DENALI_CTL_10,
	DENALI_CTL_11,
	DENALI_CTL_12,
	DENALI_CTL_13,
	tRFC,
	DENALI_CTL_15,
	DENALI_CTL_16,
	DENALI_CTL_17,
	DENALI_CTL_18,
	tCKSRX,
	tCKSRE,
	DENALI_CTL_101,
	MR0,
	MR1,
	MR2,
	MR3
};
#elif defined(DRAM_DDR2)
enum {
	PLL_VALUE = 0,
	CKC_CTRL_VALUE,
	CLK_RATE,
	RESET,
	PMU_IDX,
	SOURCE,
	ONOFF,
	T_REFI,
	TROW,
	TDATA,
	TPOWER,
	T_WR,
	CL,
};
#endif

/*===========================================================================
FUNCTION
===========================================================================*/
static unsigned int get_cycle(unsigned int tck, unsigned int ps, unsigned int ck)
{
	unsigned int ret;

	ret = time2cycle(ps, tck);

	if(ret > ck)
		return ret;
	else
		return ck;
}

/*===========================================================================
FUNCTION
===========================================================================*/
static unsigned int get_membus_ckc(unsigned int mem_freq)
{
	unsigned i = 0;

	for (i=0 ; i<ARRAY_SIZE(mem_clk_table) ; i++) {
		if (mem_clk_table[i].freq <= mem_freq)
			break;
	}

	if (i >= ARRAY_SIZE(mem_clk_table))
		i = (ARRAY_SIZE(mem_clk_table) - 1);

	CKC_CHANGE_ARG(PLL_VALUE) = mem_clk_table[i].pll;
	CKC_CHANGE_ARG(CKC_CTRL_VALUE) = mem_clk_table[i].clkctrl;
	CKC_CHANGE_ARG(CLK_RATE) = (mem_clk_table[i].freq/10000);

	return (mem_clk_table[i].freq/10000);
}

/*===========================================================================
FUNCTION
===========================================================================*/
static void get_ddr_param(unsigned int mem_freq)
{
#if defined(DRAM_DDR3)

	unsigned tck = 0;
	unsigned nCL, nCWL;
	unsigned tmp;

	tck = (1000000/mem_freq);

	if(tck >= 2500){ /* 2.5ns, 400MHz */
		nCL = 6;
		nCWL = 5;
	}else if(tck >= 1875){ // 1.875ns, 533..MHz
		nCL = 8;
		nCWL = 6;
	}else if(tck >= 1500){ // 1.5 ns, 666..MHz
		if(DDR3_MAX_SPEED < DDR3_1600)
			nCL = 9;
		else
			nCL = 10;
		nCWL = 7;
	}else if(tck >= 1250){ // 1.25 ns, 800MHz
		nCL = 11;
		nCWL = 8;
	}else if(tck >= 1070){ // 1.07 ns, 933..MHz
		nCL = 13;
		nCWL = 9;
	}else if(tck >= 935){ // 0.935 ns, 1066..MHz
		nCL = 14;
		nCWL = 10;
	}

	CKC_CHANGE_ARG(DENALI_CTL_3) = get_cycle(tck, 20000000, 1); //TRST_PWRON = 200us, 7 //Bruce_temp.. ns ?
	CKC_CHANGE_ARG(DENALI_CTL_4) = get_cycle(tck, 50000000, 1); //CKE_INACTIVE = 500us, 10 //Bruce_temp.. ns ?

	//TBST_INT_INTERVAL[26:24] = 0x1, WL[20:16], CASLAT_LIN[13:8], CL half-cycle increment = 0, INITAREF[3:0] = 0x8
	if(DDR3_AL == AL_DISABLED){ //nAL = 0;
		CKC_CHANGE_ARG(DENALI_CTL_5) = (0x1<<24 | (nCWL)<<16 | ((nCL<<1)|0)<<8 | 0x8);
	}
	else if(DDR3_AL == AL_CL_MINUS_ONE){ //nAL = nCL - 1;
		CKC_CHANGE_ARG(DENALI_CTL_5) = (0x1<<24 | (nCWL+nCL-1)<<16 | ((nCL<<1)|0)<<8 | 0x8);
	}	
	else if(DDR3_AL == AL_CL_MINUS_TWO){ //nAL = nCL - 2;
		CKC_CHANGE_ARG(DENALI_CTL_5) = (0x1<<24 | (nCWL+nCL-2)<<16 | ((nCL<<1)|0)<<8 | 0x8);
	}

	CKC_CHANGE_ARG(DENALI_CTL_6) = (get_cycle(tck, DDR3_tRAS_ps, DDR3_tRAS_ck)<<24 | get_cycle(tck, DDR3_tRC_ps, DDR3_tRC_ck)<<16 | get_cycle(tck, DDR3_tRRD_ps, DDR3_tRRD_ck)<<8 | DDR3_tCCD_ck);
	CKC_CHANGE_ARG(DENALI_CTL_7) = (DDR3_tMRD_ck<<24 | get_cycle(tck, DDR3_tRTP_ps, DDR3_tRTP_ck)<<16 | get_cycle(tck, DDR3_tRP_ps, DDR3_tRP_ck)<<8 | (get_cycle(tck, DDR3_tWTR_ps, DDR3_tWTR_ck)+1));
	CKC_CHANGE_ARG(DENALI_CTL_8) = (get_cycle(tck, DDR3_tRAS_MAX_ps, 1)<<8 | get_cycle(tck, DDR3_tMOD_ps, DDR3_tMOD_ck));
	CKC_CHANGE_ARG(DENALI_CTL_9) = ((get_cycle(tck, DDR3_tCKE_ps, DDR3_tCKE_ck)+1)<<8 | get_cycle(tck, DDR3_tCKE_ps, DDR3_tCKE_ck));
	CKC_CHANGE_ARG(DENALI_CTL_10) = (get_cycle(tck, DDR3_tWR_ps, DDR3_tWR_ck)<<24 | get_cycle(tck, DDR3_tRCD_ps, DDR3_tRCD_ck)<<16 | 1<<8 | 1);
	CKC_CHANGE_ARG(DENALI_CTL_11) = (1<<24 | DDR3_tDLLK_ck<<8 | (get_cycle(tck, DDR3_tWR_ps, DDR3_tWR_ck)+nCL));
	CKC_CHANGE_ARG(DENALI_CTL_12) = (1<<16 | get_cycle(tck, DDR3_tFAW_ps, DDR3_tFAW_ck)<<8 | 3);
	CKC_CHANGE_ARG(DENALI_CTL_13) = get_cycle(tck, DDR3_tRP_ps, DDR3_tRP_ck)+1;
	CKC_CHANGE_ARG(tRFC) = get_cycle(tck, DDR3_tRFC_ps, 1);
	CKC_CHANGE_ARG(DENALI_CTL_15) = get_cycle(tck, DDR3_tREFI_ps, 1);
	CKC_CHANGE_ARG(DENALI_CTL_16) = (get_cycle(tck, DDR3_tXPDLL_ps, DDR3_tXPDLL_ck)<<16 | get_cycle(tck, DDR3_tXP_ps, DDR3_tXP_ck)); // DDR3 Only
	CKC_CHANGE_ARG(DENALI_CTL_17) = (6<<16 | 2); //TXARD[-0] = 0x2, TXARDS[-16] = 0x6 // DDR2 only
	CKC_CHANGE_ARG(DENALI_CTL_18) = (get_cycle(tck, DDR3_tXS_ps, DDR3_tXS_ck)<<16 | DDR3_tXSDLL_ck);
	CKC_CHANGE_ARG(tCKSRX) = get_cycle(tck, DDR3_tCKSRX_ps, DDR3_tCKSRX_ck);
	CKC_CHANGE_ARG(tCKSRE) = get_cycle(tck, DDR3_tCKSRE_ps, DDR3_tCKSRE_ck);

//--------------------------------------------------------------------------
// DFI Timing

	if(DDR3_AL == AL_DISABLED){ //nAL = 0;
		CKC_CHANGE_ARG(DENALI_CTL_101) = (0x1<<24|0x1<<16|nCWL<<8|(nCL+5));
	}
	else if(DDR3_AL == AL_CL_MINUS_ONE){ //nAL = nCL - 1;
		CKC_CHANGE_ARG(DENALI_CTL_101) = (0x1<<24|0x1<<16|(nCWL+nCL-1)<<8|(nCL+5));
	}	
	else if(DDR3_AL == AL_CL_MINUS_TWO){ //nAL = nCL - 2;
		CKC_CHANGE_ARG(DENALI_CTL_101) = (0x1<<24|0x1<<16|(nCWL+nCL-2)<<8|(nCL+5));
	}

//--------------------------------------------------------------------------
// MRS Setting

	// MRS0
#if (1) //Bruce_temp_mrs0 FAST_EXIT 맞나?
	tmp = DDR3_BURST_LEN | (DDR3_READ_BURST_TYPE<<3);
#else
	tmp = DDR3_BURST_LEN | (DDR3_READ_BURST_TYPE<<3) | (FAST_EXIT<<12);
#endif

	if(nCL < 5)
		tmp |= ((5-4)<<4);
	else if(nCL > 11)
		tmp |= ((11-4)<<4);
	else
		tmp |= ((nCL-4)<<4);

	if(get_cycle(tck, DDR3_tWR_ps, DDR3_tWR_ck) <= 5) // Write Recovery for autoprecharge
		tmp |= WR_5<<9;
	else if(get_cycle(tck, DDR3_tWR_ps, DDR3_tWR_ck) == 6)
		tmp |= WR_6<<9;
	else if(get_cycle(tck, DDR3_tWR_ps, DDR3_tWR_ck) == 7)
		tmp |= WR_7<<9;
	else if(get_cycle(tck, DDR3_tWR_ps, DDR3_tWR_ck) == 8)
		tmp |= WR_8<<9;
	else if((get_cycle(tck, DDR3_tWR_ps, DDR3_tWR_ck) == 9) || (get_cycle(tck, DDR3_tWR_ps, DDR3_tWR_ck) == 10))
		tmp |= WR_10<<9;
	else if(get_cycle(tck, DDR3_tWR_ps, DDR3_tWR_ck) >= 11)
		tmp |= WR_12<<9;

	tmp	|= (1<<8); // DLL Reset

	CKC_CHANGE_ARG(MR0) = tmp;

	// MRS1
	CKC_CHANGE_ARG(MR1) = ((DDR3_AL<<3) | (DDR3_ODT<<2) | (DDR3_DIC<<1));

	// MRS2
	tmp = 0;
	if(nCWL <= 5)
		tmp |= 0;
	else if(nCWL == 6)
		tmp |= 1<<3;
	else if(nCWL == 7)
		tmp |= 2<<3;
	else if(nCWL >= 8)
		tmp |= 3<<3;

	CKC_CHANGE_ARG(MR2) = tmp;

#elif defined(DRAM_DDR2)
	int tck = 0;
	unsigned int nRFC, nRRD, nRP, nRCD, nRC, nRAS;
	unsigned int nWTR, nWR, nRTP, nCL, nWL, nRL;
	unsigned int nFAW, nXSR, nXP, nCKE, nMRD;

	tck = 1000000/mem_freq; // tck is pico sec unit

	//nCL = nRCD = nRP
	nCL = DDR2_CL;//((DDR2_CL > 5) ? 5 : DDR2_CL);

	CKC_CHANGE_ARG(CL) = nCL;
	CKC_CHANGE_ARG(T_REFI) = get_cycle(tck, DDR2_tREFI_ps, 1);

	nRFC = get_cycle(tck, DDR2_tRFC_ps, 1);

	nRRD = get_cycle(tck, DDR2_tRRD_ps, DDR2_tRRD_ck);
	nRP  = nCL; //get_cycle(tck, DDR2_tRP_ps,  DDR2_tRP_ck);
	nRCD = nCL; //get_cycle(tck, DDR2_tRCD_ps, DDR2_tRCD_ck);
	nRC  = get_cycle(tck, DDR2_tRC_ps,  DDR2_tRC_ck);
	nRAS = get_cycle(tck, DDR2_tRAS_ps, DDR2_tRAS_ck);

	CKC_CHANGE_ARG(TROW) = (nRFC<<24)|(nRRD<<20)|(nRP<<16)|(nRCD<<12)|(nRC<<6)|(nRAS);

	nWTR = get_cycle(tck, DDR2_tWTR_ps, DDR2_tWTR_ck);
	nWR  = get_cycle(tck, DDR2_tWR_ps, DDR2_tWR_ck);
	nRTP = get_cycle(tck, DDR2_tRTP_ps, DDR2_tRTP_ck);
	nWL  = nCL -1;
	nRL  = nCL;
	CKC_CHANGE_ARG(TDATA) = (nWTR<<28)|(nWR<<24)|(nRTP<<20)|(nCL<<16)|(nWL<<8)|(nRL);

	nFAW = get_cycle(tck, DDR2_tFAW_ps, DDR2_tFAW_ck);
	nXSR = DDR2_tXSR_ck;
	nXP  = DDR2_tXP_ck;
	nCKE = DDR2_tCKE_ck;
	nMRD = DDR2_tMRD_ck;
	CKC_CHANGE_ARG(TPOWER) = (nFAW<<24)|(nXSR<<16)|(nXP<<8)|(nCKE<<4)|(nMRD);

	CKC_CHANGE_ARG(T_WR) = nWR;

	//printk("FREQ:%d, T_REFI:0x%x, TROW:0x%x, TDATA:0x%x, TPOWER:0x%x, T_WR:0x%x\n",mem_freq,CKC_CHANGE_ARG(T_REFI),CKC_CHANGE_ARG(TROW),CKC_CHANGE_ARG(TDATA),CKC_CHANGE_ARG(TPOWER),CKC_CHANGE_ARG(T_WR));
#endif
}

/*===========================================================================
FUNCTION
===========================================================================*/
void _change_mem_clock(void)
{
#if defined(DRAM_DDR3)

	volatile int i;
	volatile int tmp;

//--------------------------------------------------------------------------
// holding to access to dram

	BITSET(denali_ctl(44),0x1); //inhibit_dram_cmd=1
	while(!(denali_ctl(46)&(0x1<<16)));	// wait for inhibit_dram_cmd
	BITSET(denali_ctl(47), 0x1<<16);

//--------------------------------------------------------------------------
//enter self-refresh

	BITSET(denali_phy(0x34), 0x1<<10); //lp_ext_req=1
	while(!(denali_phy(0x34)&(0x1<<27))); //until lp_ext_ack==1
	BITCSET(denali_phy(0x34), 0x000000FF, (2<<2)|(1<<1)|(0));
	BITSET(denali_phy(0x34), 0x1<<8); //lp_ext_cmd_strb=1
	while((denali_phy(0x34)&(0x007F0000)) !=0x00450000); //until lp_ext_state==0x45 : check self-refresh state
	BITCLR(denali_phy(0x34), 0x1<<8); //lp_ext_cmd_strb=0
	BITCLR(denali_phy(0x34), 0x1<<10); //lp_ext_req=0
	while(denali_phy(0x34)&(0x1<<27)); //until lp_ext_ack==0
	BITSET(denali_ctl(96), 0x3<<8); //DRAM_CLK_DISABLE[9:8] = [CS1, CS0] = 0x3
	BITCLR(denali_ctl(0),0x1); //START[0] = 0
	BITSET(denali_phy(0x0C), 0x1<<22); //ctrl_cmosrcv[22] = 0x1
	BITCSET(denali_phy(0x0C), 0x001F0000, 0x1F<<16); //ctrl_pd[20:16] = 0x1f
	BITCSET(denali_phy(0x20), 0x000000FF, 0xF<<4|0xF); //ctrl_pulld_dq[7:4] = 0xf, ctrl_pulld_dqs[3:0] = 0xf

//--------------------------------------------------------------------------
//Clock setting..

	//Memory BUS Configuration : MBUSCFG
	#ifdef DDR3_AXI0_FIFO_1to2_ENABLE
	*(volatile unsigned long *)addr_mem(0x810010) |= Hw15; //dclkr=1 : this is used for DDR3 only.
	#else
	*(volatile unsigned long *)addr_mem(0x810010) &= ~Hw15; //(d)not used just for ddr3 clk div
	#endif
	i=50; while(i--);

#if (1)
	*(volatile unsigned long *)addr_clk(0x000004) = 0x00200014;  // mem bus
	*(volatile unsigned long *)addr_clk(0x000040) = CKC_CHANGE_ARG(PLL_VALUE) | 0x00100000; //pll4 for mem , lock_en
	*(volatile unsigned long *)addr_clk(0x000040) |= 0x80000000;
	i=20; while(i--) while((*(volatile unsigned long *)addr_clk(0x000040) & 0x00200000) == 0);
	i=100; while(i--);
	*(volatile unsigned long *)addr_clk(0x000004) = ((*(volatile unsigned long *)addr_clk(0x000004))&0xFFFFFFF0) | (CKC_CHANGE_ARG(CKC_CTRL_VALUE)&0x0000000F);
	while((*(volatile unsigned long *)addr_clk(0x000004))&0x80000000);	// CHGREQ
	*(volatile unsigned long *)addr_clk(0x000004) = ((*(volatile unsigned long *)addr_clk(0x000004))&0xFFFFFF0F) | (CKC_CHANGE_ARG(CKC_CTRL_VALUE)&0x000000F0);
	while((*(volatile unsigned long *)addr_clk(0x000004))&0x20000000);	// CFGREQ
#else
	*(volatile unsigned long *)addr_clk(0x000004) = 0x00200014;  // mem bus
	*(volatile unsigned long *)addr_clk(0x000040) = CKC_CHANGE_ARG(PLL_VALUE) | 0x00100000; //pll4 for mem , lock_en
	*(volatile unsigned long *)addr_clk(0x000040) |= 0x80000000;
	i=20; while(i--) while((*(volatile unsigned long *)addr_clk(0x000040) & 0x00200000) == 0);
	i=100; while(i--);
	*(volatile unsigned long *)addr_clk(0x000004) = CKC_CHANGE_ARG(CKC_CTRL_VALUE);  // mem bus
	i=50; while(i--);
#endif

//--------------------------------------------------------------------------
// Timing Parameter

	denali_ctl(3) = CKC_CHANGE_ARG(DENALI_CTL_3);
	denali_ctl(4) = CKC_CHANGE_ARG(DENALI_CTL_4);
	denali_ctl(5) = CKC_CHANGE_ARG(DENALI_CTL_5);
	denali_ctl(6) = CKC_CHANGE_ARG(DENALI_CTL_6);
	denali_ctl(7) = CKC_CHANGE_ARG(DENALI_CTL_7);
	denali_ctl(8) = CKC_CHANGE_ARG(DENALI_CTL_8);
	denali_ctl(9) = CKC_CHANGE_ARG(DENALI_CTL_9);
	denali_ctl(10) = CKC_CHANGE_ARG(DENALI_CTL_10);
	denali_ctl(11) = CKC_CHANGE_ARG(DENALI_CTL_11);
	denali_ctl(12) = CKC_CHANGE_ARG(DENALI_CTL_12);
	denali_ctl(13) = CKC_CHANGE_ARG(DENALI_CTL_13);
	BITCSET(denali_ctl(14), 0x03FF0100, (CKC_CHANGE_ARG(tRFC)<<16 | 1<<8));
	denali_ctl(15) = CKC_CHANGE_ARG(DENALI_CTL_15);
	denali_ctl(16) = CKC_CHANGE_ARG(DENALI_CTL_16);
	denali_ctl(17) = CKC_CHANGE_ARG(DENALI_CTL_17);
	denali_ctl(18) = CKC_CHANGE_ARG(DENALI_CTL_18);
	BITCSET(denali_ctl(20), 0x000F0F00, CKC_CHANGE_ARG(tCKSRX)<<16 | CKC_CHANGE_ARG(tCKSRE)<<8);

//--------------------------------------------------------------------------
// DFI Timing

	denali_ctl(101) = CKC_CHANGE_ARG(DENALI_CTL_101);

//--------------------------------------------------------------------------
// MRS Setting

	// MRS0
	denali_ctl(27) = CKC_CHANGE_ARG(MR0)<<8;
	BITCSET(denali_ctl(30), 0x0000FFFF, CKC_CHANGE_ARG(MR0));

	// MRS1
	BITCSET(denali_ctl(28), 0x0000FFFF, CKC_CHANGE_ARG(MR1));
	BITCSET(denali_ctl(30), 0xFFFF0000, CKC_CHANGE_ARG(MR1)<<16);

	// MRS2
	BITCSET(denali_ctl(28), 0xFFFF0000, CKC_CHANGE_ARG(MR2)<<16);
	BITCSET(denali_ctl(31), 0x0000FFFF, CKC_CHANGE_ARG(MR2));

//--------------------------------------------------------------------------
// Exit self-refresh

	BITCLR(denali_phy(0x0C), 0x1<<22); //ctrl_cmosrcv[22] = 0x0
	BITCLR(denali_phy(0x0C), 0x001F0000); //ctrl_pd[20:16] = 0x0
	BITCLR(denali_phy(0x20), 0x000000FF); //ctrl_pulld_dq[7:4] = 0x0, ctrl_pulld_dqs[3:0] = 0x0
	while(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG&(1<<8)){
		BITCLR(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG, 1<<8); //SSTL_RTO : SSTL I/O retention mode =0
	}
	*(volatile unsigned long *)addr_mem(0x810004) &= ~(1<<2); //PHY=0
	*(volatile unsigned long *)addr_mem(0x810004) |= (1<<2); //PHY=1
	while(!(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG&(1<<8))){
		BITSET(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG, 1<<8); //SSTL_RTO : SSTL I/O retention mode disable=1
	}
	BITCLR(denali_ctl(96), 0x3<<8); //DRAM_CLK_DISABLE[9:8] = [CS1, CS0] = 0x0
	BITSET(denali_ctl(0),0x1); //START[0] = 1
//	while(!(denali_ctl(46)&(0x20000)));
//	BITSET(denali_ctl(47), 0x20000);
	BITCSET(denali_ctl(20), 0xFF000000, ((2<<2)|(0<<1)|(1))<<24);
	while(!(denali_ctl(46)&(0x40)));
	BITSET(denali_ctl(47), 0x40);

//--------------------------------------------------------------------------
// MRS Write

	// MRS2
	BITCSET(denali_ctl(29), 0x0000FFFF, (denali_ctl(28)&0xFFFF0000)>>16);
	BITCSET(denali_ctl(31), 0xFFFF0000, (denali_ctl(31)&0x0000FFFF)<<16);
	denali_ctl(26) = (2<<0)|(1<<23)|(1<<24)|(1<<25); // MR Select[7-0], MR enable[23], All CS[24], Trigger[25]
	while(!(denali_ctl(46)&(0x8000)));
	BITSET(denali_ctl(47), 0x8000);

	// MRS3
	BITCSET(denali_ctl(29), 0x0000FFFF, (denali_ctl(29)&0xFFFF0000)>>16);
	BITCSET(denali_ctl(31), 0xFFFF0000, (denali_ctl(32)&0x0000FFFF)<<16);
	denali_ctl(26) = (3<<0)|(1<<23)|(1<<24)|(1<<25); // MR Select[7-0], MR enable[23], All CS[24], Trigger[25]
	while(!(denali_ctl(46)&(0x8000)));
	BITSET(denali_ctl(47), 0x8000);

	// MRS1
	BITCSET(denali_ctl(29), 0x0000FFFF, (denali_ctl(28)&0x0000FFFF)>>0);
	BITCSET(denali_ctl(31), 0xFFFF0000, (denali_ctl(30)&0xFFFF0000)<<0);
	denali_ctl(26) = (1<<0)|(1<<23)|(1<<24)|(1<<25); // MR Select[7-0], MR enable[23], All CS[24], Trigger[25]
	while(!(denali_ctl(46)&(0x8000)));
	BITSET(denali_ctl(47), 0x8000);

	// MRS0
	BITCSET(denali_ctl(29), 0x0000FFFF, (denali_ctl(27)&0x00FFFF00)>>8);
	BITCSET(denali_ctl(31), 0xFFFF0000, (denali_ctl(30)&0x0000FFFF)<<16);
	denali_ctl(26) = (0<<0)|(1<<23)|(1<<24)|(1<<25); // MR Select[7-0], MR enable[23], All CS[24], Trigger[25]
	while(!(denali_ctl(46)&(0x8000)));
	BITSET(denali_ctl(47), 0x8000);

//--------------------------------------------------------------------------

	while(!(denali_phy(0x24)&(1<<10))); //ctrl_locked, DLL Locked ...

	BITCLR(denali_ctl(40) ,0x1<<16); //ZQCS_ROTATE=0x0
	BITCSET(denali_ctl(39) ,0x3<<16, 0x2<<16); //ZQ_REQ=2 : 0x1=short calibration, 0x2=long calibration

//--------------------------------------------------------------------------
// release holding to access to dram

	i = 50;	while(i--) tmp = denali_ctl(46); // for reset DLL on DDR3
	BITCLR(denali_ctl(44),0x1); //inhibit_dram_cmd=0

//--------------------------------------------------------------------------

#elif defined(DRAM_DDR2)

	volatile int i;

//--------------------------------------------------------------------------
//Change to config mode

	//Miscellaneous Configuration : COMMON
	*(volatile unsigned long *)addr_mem(0x410020) &= ~Hw17; //creq2=0 : enter low power
	while((*(volatile unsigned long *)addr_mem(0x410020))&Hw24); //cack2==0 : wait for acknowledgement to request..

	//MEMCTRL
	*(volatile unsigned long *)addr_mem(0x430004) |= 0x00000001; //clk_stop_en=1 //Bruce_temp_8920

#if (0)
	if(DDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr_mem(0x430048)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
	else//DDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr_mem(0x430048)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED

	if(DDR2_LOGICAL_CHIP_NUM == 2){
		if(DDR2_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr_mem(0x43004C)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
		else//DDR2_BANK_NUM == 4
			while (((*(volatile unsigned long *)addr_mem(0x43004C)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
	}
#endif

//--------------------------------------------------------------------------
//Clock setting..

	//Memory BUS Configuration : MBUSCFG
	*(volatile unsigned long *)addr_mem(0x810010) &= ~Hw15; //(d)not used just for ddr3 clk div
	i=50; while(i--);

#if (1)
	*(volatile unsigned long *)addr_clk(0x000004) = 0x00200014;  // mem bus
	*(volatile unsigned long *)addr_clk(0x000040) = CKC_CHANGE_ARG(PLL_VALUE) | 0x00100000; //pll4 for mem , lock_en
	*(volatile unsigned long *)addr_clk(0x000040) |= 0x80000000;
	i=20; while(i--) while((*(volatile unsigned long *)addr_clk(0x000040) & 0x00200000) == 0);
	i=100; while(i--);
	*(volatile unsigned long *)addr_clk(0x000004) = ((*(volatile unsigned long *)addr_clk(0x000004))&0xFFFFFFF0) | (CKC_CHANGE_ARG(CKC_CTRL_VALUE)&0x0000000F);
	while((*(volatile unsigned long *)addr_clk(0x000004))&0x80000000);	// CHGREQ
	*(volatile unsigned long *)addr_clk(0x000004) = ((*(volatile unsigned long *)addr_clk(0x000004))&0xFFFFFF0F) | (CKC_CHANGE_ARG(CKC_CTRL_VALUE)&0x000000F0);
	while((*(volatile unsigned long *)addr_clk(0x000004))&0x20000000);	// CFGREQ
#else
	*(volatile unsigned long *)addr_clk(0x000004) = 0x00200014;  // mem bus
	*(volatile unsigned long *)addr_clk(0x000040) = CKC_CHANGE_ARG(PLL_VALUE) | 0x00100000; //pll4 for mem , lock_en
	*(volatile unsigned long *)addr_clk(0x000040) |= 0x80000000;
	i=100; while(i--) while((*(volatile unsigned long *)addr_clk(0x000040) & 0x00200000) == 0);
	i=100; while(i--);
	*(volatile unsigned long *)addr_clk(0x000004) = CKC_CHANGE_ARG(CKC_CTRL_VALUE);  // mem bus
	i=100; while(i--);
#endif

//--------------------------------------------------------------------------
// Controller setting

	//PHY Mode contrl register (PHYMDCFG0)
	*(volatile unsigned long *)addr_mem(0x420400) = (0 << 21) | //ctrl_lat                  //Bruce_temp_8920 ??
	                                             (0x1f << 16) | //ctrl_wake_up
	                                             (DDR2_PINMAP_TYPE << 14) | //pin_map
	                                             ((DDR2_LOGICAL_CHIP_NUM-1) << 13) | //cs2s
	                                             (0 << 4) | //mode                        //Bruce_temp_8920 ??
	                                             (1 << 3) | //c_type : denali=0, lpcon=1
	                                             (0); //dram_type : ddr2=0, ddr3=1, lpddr=2, lpddr2=3

	//PhyZQControl
	if (CKC_CHANGE_ARG(CLK_RATE) >= DDR2_ODT_ENABLE_MIN_FREQ) {
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL ;
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL | Hw1 ;//zq start
	} else {
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL | Hw0;
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL | Hw1 | Hw0 ;//zq start
	}
	while (((*(volatile unsigned long *)addr_mem(0x430040)) & (0x10000)) != 0x10000);	// Wait until ZQ End

	if (CKC_CHANGE_ARG(CLK_RATE) >= DDR2_ODT_ENABLE_MIN_FREQ) {
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL ;
	} else {
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL | Hw0;
	}

	*(volatile unsigned long *)addr_mem(0x430018 ) = 0x0010100A; //PHY Control0
	*(volatile unsigned long *)addr_mem(0x43001C ) = 0xE0000086; //PHY Control1 // modify by crony : [31:30] : ODT Enable for Write and Read
	*(volatile unsigned long *)addr_mem(0x430020 ) = 0x00000000; //PHY Control2
	*(volatile unsigned long *)addr_mem(0x430024 ) = 0x00000000; //PHY Control3
	*(volatile unsigned long *)addr_mem(0x430018 ) = 0x0010100B; //PHY Control0

	while (((*(volatile unsigned long *)addr_mem(0x430040)) & (0x7)) != 0x7);// Wait until FLOCK == 1

	//PHY Control1
	*(volatile unsigned long *)addr_mem(0x43001C) = 0xE000008E; //resync = 1
	*(volatile unsigned long *)addr_mem(0x43001C) = 0xE0000086; //resync = 0

//--------------------------------------------------------------------------
// Memory config

	//Enable Out of order scheduling
	*(volatile unsigned long *)addr_mem(0x430000 ) = 0x30FF2018;

	//MEMCTRL
	*(volatile unsigned long *)addr_mem(0x430004 ) = (0x2 << 20) |
	                                             ((DDR2_LOGICAL_CHIP_NUM-1)<<16) |
	                                             ((DDR2_LOGICAL_DATA_BITS/16)<<12) |
	                                             (0x4 << 8) |
	                                             (0x0 << 6) |
	                                             (0x0 << 5) |
	                                             (0x0 << 4) |
	                                             (0x0 << 2) |
	                                             (0x0 << 1) |
	                                             (0x0);

	//MEMCHIP0
	*(volatile unsigned long *)addr_mem(0x430008 ) = (0x80<<24) |
	                                             ((0xFF - (DDR2_TOTAL_MB_SIZE/(DDR2_LOGICAL_CHIP_NUM*0x10)-1))<<16) |
	                                             (0x1 << 12) |
	                                             ((DDR2_COLBITS - 7)<<8) |
	                                             ((DDR2_ROWBITS - 12)<<4) |
	                                             DDR2_BANK_BITS;

	//MEMCHIP1
	if(DDR2_LOGICAL_CHIP_NUM == 2)
	*(volatile unsigned long *)addr_mem(0x43000C ) = ((0x80 + DDR2_TOTAL_MB_SIZE/(DDR2_LOGICAL_CHIP_NUM*0x10))<<24) |
		                                         ((0xFF - (DDR2_TOTAL_MB_SIZE/(DDR2_LOGICAL_CHIP_NUM*0x10)-1))<<16) |
		                                         (0x1 << 12) |
		                                         ((DDR2_COLBITS - 7)<<8) |
		                                         ((DDR2_ROWBITS - 12)<<4) |
		                                         DDR2_BANK_BITS;

//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr_mem(0x430014 ) = 0x0; //PRECONFIG
	*(volatile unsigned long *)addr_mem(0x430028 ) = 0xFFFF00FF; //PRECONFIG

//--------------------------------------------------------------------------
// Timing parameter setting.

	*(volatile unsigned long *)addr_mem(0x430030 ) = CKC_CHANGE_ARG(T_REFI);
	*(volatile unsigned long *)addr_mem(0x430034 ) = CKC_CHANGE_ARG(TROW);
	*(volatile unsigned long *)addr_mem(0x430038 ) = CKC_CHANGE_ARG(TDATA);
	*(volatile unsigned long *)addr_mem(0x43003C ) = CKC_CHANGE_ARG(TPOWER);

//--------------------------------------------------------------------------
// MRS Setting

	//Direct Command
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x07000000;//NOP
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x01000000;//precharge all
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00020000;
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00030000;
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010000;
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00000100;
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x01000000;//precharge all
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x05000000;//AREF
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x05000000;//AREF
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x05000000;//AREF
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00000000;	// DLL reset release.
	*(volatile unsigned long *)addr_mem(0x430010 ) = (DDR2_BURST_LEN|(DDR2_READ_BURST_TYPE<<3)|(CKC_CHANGE_ARG(CL)<<4)|((CKC_CHANGE_ARG(T_WR)-1)<<9));	// BurstLength 4, CL , WR 
	i = 100; while(i--);
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010380; // OCD Calibration default
	i = 100; while(i--);
	if (CKC_CHANGE_ARG(CLK_RATE) >= DDR2_ODT_ENABLE_MIN_FREQ)
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010000 | (DDR2_ODT<<2) | (DDR2_DIC<<1);
	else
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010000 | (DDR2_DIC<<1);

	if(DDR2_LOGICAL_CHIP_NUM == 2)
	{
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x07000000 | Hw20;//NOP
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x01000000 | Hw20;//precharge all
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00020000 | Hw20;
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00030000 | Hw20;
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010000 | Hw20;
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00000100 | Hw20;
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x01000000 | Hw20;//precharge all
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x05000000 | Hw20;//AREF
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x05000000 | Hw20;//AREF
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x05000000 | Hw20;//AREF
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00000000 | Hw20;	// DLL reset release.
		*(volatile unsigned long *)addr_mem(0x430010 ) = (DDR2_BURST_LEN|(DDR2_READ_BURST_TYPE<<3)|(CKC_CHANGE_ARG(CL)<<4)|((CKC_CHANGE_ARG(T_WR)-1)<<9)) | Hw20;	// BurstLength 4, CL , WR 
		i = 100; while(i--);
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010380 | Hw20; // OCD Calibration default
		i = 100; while(i--);
		if (CKC_CHANGE_ARG(CLK_RATE) >= DDR2_ODT_ENABLE_MIN_FREQ)
			*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010000 | (DDR2_ODT<<2) | (DDR2_DIC<<1) | Hw20;
		else
			*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010000 | (DDR2_DIC<<1) | Hw20;
	}

//--------------------------------------------------------------------------

	//Miscellaneous Configuration : COMMON
	*(volatile unsigned long *)addr_mem(0x410020) |= Hw17; //creq2=1 : exit low power
	while(!((*(volatile unsigned long *)addr_mem(0x410020))&Hw24)); //cack2==1 : wait for acknowledgement to request..

	*(volatile unsigned long *)addr_mem(0x430000 ) |= 0x00000020; //CONCONTROL : aref_en=1

	if(DDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr_mem(0x430048)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
	else//DDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr_mem(0x430048)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED

	if(DDR2_LOGICAL_CHIP_NUM == 2){
		if(DDR2_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr_mem(0x43004C)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
		else//DDR2_BANK_NUM == 4
			while (((*(volatile unsigned long *)addr_mem(0x43004C)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED
	}

//--------------------------------------------------------------------------
#endif
}

/*===========================================================================
FUNCTION
===========================================================================*/
int change_mem_clock(unsigned int freq)
{
	int i;
	typedef void (*FuncPtr)(void);
	unsigned flags, tmp, stk;
	unsigned int   mem_freq;
	FuncPtr pFunc = (FuncPtr)(CKC_CHANGE_FUNC_ADDR);

	//Bruce_temp_8920, memcpy가 오동작한다.
	//memcpy((void *)CKC_CHANGE_FUNC_ADDR, (void*)_change_mem_clock, CKC_CHANGE_FUNC_SIZE);
	{
		unsigned src = (unsigned)_change_mem_clock;
		unsigned dest = CKC_CHANGE_FUNC_ADDR;

		for(i=0;i<CKC_CHANGE_FUNC_SIZE;i+=4)
			*(volatile unsigned long *)(dest+i) = *(volatile unsigned long *)(src+i);
	}

	mem_freq = get_membus_ckc(freq);
	dprintf (INFO, "MEM FREQ : %dMHz\n", mem_freq);
	get_ddr_param(mem_freq);

	__asm__ __volatile__( \
	"mrs	%0, cpsr\n" \
	"cpsid	i\n" \
	"mrs	%1, cpsr\n" \
	"orr	%1, %1, #128\n" \
	"msr	cpsr_c, %1" \
	: "=r" (flags), "=r" (tmp) \
	: \
	: "memory", "cc");

	//CKC_CHANGE_STACK_TOP
	__asm__ __volatile__( \
	"mov	%0, sp \n" \
	"ldr	sp, =0x10005000 \n" \
	: "=r" (stk) \
	: \
	: "memory", "cc");

	//arm_invalidate_tlb();

	{
		uint32_t val;
		__asm__ volatile("mrc	p15, 0, %0, c1, c0, 0" : "=r" (val));
		val &= ~0x1;
		__asm__ volatile("mcr	p15, 0, %0, c1, c0, 0" :: "r" (val));
	}

	pFunc();

	{
		uint32_t val;
		__asm__ volatile("mrc	p15, 0, %0, c1, c0, 0" : "=r" (val));
		val |= 0x1;
		__asm__ volatile("mcr	p15, 0, %0, c1, c0, 0" :: "r" (val));
	}

	__asm__ __volatile__( \
	"mov	sp, %0 \n" \
	: \
	: "r" (stk) \
	: "memory", "cc");

	__asm__ __volatile__( \
	"msr	cpsr_c, %0\n " \
	"cpsid	i" \
	: \
	: "r" (flags) \
	: "memory", "cc");

	return 0;
}


/************* end of file *************************************************************/

