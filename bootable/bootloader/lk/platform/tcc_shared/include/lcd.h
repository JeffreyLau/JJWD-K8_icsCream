/***************************************************************************************
*	FileName    : lcd.h
*	Description : lcd Functions
****************************************************************************************
*
*	TCC Board Support Package
*	Copyright (c) Telechips, Inc.
*	ALL RIGHTS RESERVED
*
****************************************************************************************/
#ifndef __LCD_H__
#define __LCD_H__


#if defined(DEFAULT_DISPLAY_LCD)
	#if defined(CLAA104XA01CW) || defined(HT121WX2) || defined(N101L6) || defined(CLAA102NA0DCW) || defined(ED090NA) || defined(CLAA070NP01)|| defined(HV070WSA)
		#define BOOT_USE_LVDS
	#else
		#define BOOT_USE_LCD
	#endif
	
#elif defined(DEFAULT_DISPLAY_HDMI)
	#include <lcd/HDMI_TCC.h>

#elif defined(DEFAULT_DISPLAY_COMPOSITE)
	#include <lcd/COMPOSITE_TCC.h>

#elif defined(DEFAULT_DISPLAY_COMPONENT)
	#if defined(COMPONENT_CHIP_CS4954)
	#include <lcd/COMPONENT_CS4954.h>
	#elif defined(COMPONENT_CHIP_THS8200)
	#include <lcd/COMPONENT_THS8200.h>
	#endif

#elif defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
	#include <lcd/HDMI_TCC.h>
	#include <lcd/COMPOSITE_TCC.h>
	#if defined(COMPONENT_CHIP_CS4954)
	#include <lcd/COMPONENT_CS4954.h>
	#elif defined(COMPONENT_CHIP_THS8200)
	#include <lcd/COMPONENT_THS8200.h>
	#endif
#endif

#if defined(DEFAULT_DISPLAY_HDMI) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
#define BOOT_USE_HDMI
#endif//


#endif	/*__LCD_H__*/
