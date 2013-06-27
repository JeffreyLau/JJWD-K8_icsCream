/****************************************************************************
 *	 TCC Version 0.6
 *	 Copyright (c) telechips, Inc.
 *	 ALL RIGHTS RESERVED
 *
****************************************************************************/

#ifndef SZ_1K
#define SZ_1K		(1024)
#endif

#ifndef SZ_1M
#define SZ_1M		(1024*1024)
#endif

#if defined(PLATFORM_TCC892X)
#define MEM_PHYS_OFFSET						(0x80000000)
#else
#define MEM_PHYS_OFFSET						(0x40000000)
#endif

#define ARRAY_MBYTE(x)		((((x) + (SZ_1M-1))>> 20) << 20)

#define TOTAL_SDRAM_SIZE					(TCC_MEM_SIZE * SZ_1M)

#if defined(PLATFORM_TCC93XX)
	#if defined(TARGET_BOARD_STB)
		#include "tcc_used_mem_tcc9300st.h"
	#elif defined(BOARD_TCC930X_CM)
		#include "tcc_used_mem_tcc9300cm.h"
	#else
		#include "tcc_used_mem_tcc93xx.h"
	#endif
#elif defined(_TCC8900_)
	#if defined(TARGET_BOARD_STB)
		#include "tcc_used_mem_tcc8900st.h"
	#elif defined(CONFIG_USE_TCC_LAYER)
		#include "tcc_used_mem_tcc8900tl.h"
	#else
		#include "tcc_used_mem_tcc8900.h"
	#endif
#elif defined(PLATFORM_TCC88XX)
	#if defined(TARGET_BOARD_STB) || defined(HDMI_1280X720)
		#include "tcc_used_mem_tcc8800st.h"
	#else
		#include "tcc_used_mem_tcc8800.h"
	#endif
#elif defined(PLATFORM_TCC92XX)
	#include "tcc_used_mem_tcc92xx.h"
#elif defined(PLATFORM_TCC892X)
	#if defined(TARGET_BOARD_STB)
		#include "tcc_used_mem_tcc8920st.h"
	#else
		#include "tcc_used_mem_tcc892x.h"
	#endif
#else
	#error No memory map is specified!
#endif
