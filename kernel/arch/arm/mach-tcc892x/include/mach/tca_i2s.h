/*
 * arch/arm/mach-tcc88xx/include/mach/tca_i2s.h
 *
 */

#include <mach/bsp.h>

#define BASE_ADDR_PIC       HwPIC_BASE

#if defined(CONFIG_M805S_8923_0XA) || defined(CONFIG_M805S_8925_0XX)
#undef CONFIG_MACH_M805_892X
#endif

#if defined(CONFIG_MACH_M805_892X)
#define BASE_ADDR_ADMA      HwAUDIO1_ADMA_BASE
#define BASE_ADDR_DAI       HwAUDIO1_DAI_BASE
#define BASE_ADDR_SPDIFTX   HwAUDIO1_SPDIFTX_BASE

#define CLK_NAME_ADMA       "adma1"
#define CLK_NAME_DAI        "dai1"
#define CLK_NAME_SPDIF      "spdif1"

#elif defined(CONFIG_SOC_TCC8925S) // Planet 20121025 SPDIF exists only on channel01
#define	BASE_ADDR_ADMA0      HwAUDIO0_ADMA_BASE
#define BASE_ADDR_DAI0       HwAUDIO0_DAI_BASE
#define BASE_ADDR_SPDIFTX0   HwAUDIO1_SPDIFTX_BASE	// SPDIF0 does not exist.

#define CLK_NAME_ADMA0       "adma0"
#define CLK_NAME_DAI0        "dai0"
#define CLK_NAME_SPDIF0      "spdif1"	// SPDIF0 does not exist.

#define	BASE_ADDR_ADMA1      HwAUDIO1_ADMA_BASE
#define BASE_ADDR_DAI1       HwAUDIO1_DAI_BASE
#define BASE_ADDR_SPDIFTX1   HwAUDIO1_SPDIFTX_BASE

#define CLK_NAME_ADMA1       "adma1"
#define CLK_NAME_DAI1        "dai1"
#define CLK_NAME_SPDIF1      "spdif1"

#else
#if defined(CONFIG_ARCH_TCC892X)
#define	BASE_ADDR_ADMA0      HwAUDIO0_ADMA_BASE
#define BASE_ADDR_DAI0       HwAUDIO0_DAI_BASE
#define BASE_ADDR_SPDIFTX0   HwAUDIO0_SPDIFTX_BASE

#define CLK_NAME_ADMA0       "adma0"
#define CLK_NAME_DAI0        "dai0"
#define CLK_NAME_SPDIF0      "spdif0"

#define	BASE_ADDR_ADMA1      HwAUDIO1_ADMA_BASE
#define BASE_ADDR_DAI1       HwAUDIO1_DAI_BASE
#define BASE_ADDR_SPDIFTX1   HwAUDIO1_SPDIFTX_BASE

#define CLK_NAME_ADMA1       "adma1"
#define CLK_NAME_DAI1        "dai1"
#define CLK_NAME_SPDIF1      "spdif1"

#else
#define	BASE_ADDR_ADMA      HwAUDIO0_ADMA_BASE
#define BASE_ADDR_DAI       HwAUDIO0_DAI_BASE
#define BASE_ADDR_SPDIFTX   HwAUDIO0_SPDIFTX_BASE

#define CLK_NAME_ADMA       "adma0"
#define CLK_NAME_DAI        "dai0"
#define CLK_NAME_SPDIF      "spdif0"
#endif
#endif

