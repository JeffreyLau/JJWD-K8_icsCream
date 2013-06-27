

//#include <common.h>
//#include <ckc.h>
#include <i2c.h>
#include <platform/reg_physical.h>
#include <platform/globals.h>
#include <lcd.h>

#if defined(DEFAULT_DISPLAY_HDMI) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)

#include "include/hdmi/regs-hdmi.h"
#include "include/hdmi/hdmi.h"
#include "include/hdmi/hdmi_audio.h"

#define HDMI_DEBUG	1

#if HDMI_DEBUG
#define DPRINTF	printf
#else
#define DPRINTF
#endif

/*
static struct spdif_struct spdif_struct;
*/

//==============================================================================
//                 Section x - HDMI PHY Setting
//==============================================================================

#define PHY_I2C_ADDRESS             0x70
#define PHY_REG_MODE_SET_DONE       0x28
#define PHY_CONFIG_START_OFFSET     0x01

/**
 * PHY register setting values for each Pixel clock and Bit depth (8, 10, 12 bit).\n
 * @see  Setting values are came from L6LP_HDMI_v1p3_TX_PHY_RegisterMap_REV0.90.doc document.
 */
#if defined(_TCC9200_) 
static const unsigned char phy_config[][3][40] = {
	// for internal PLL
	// freq = 25.200 MHz
	{
		{ 0x85, 0x88, 0x20, 0x50, 0x00, 0x0C, 0x00, 0x06, 0xD8, 0xC2, 0xA4, 0x46, 0x08, 0x0A, 0xBB, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x0C, 0x03, 0x23, 0x38, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x24, 0x96, 0x6D, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x20, 0x50, 0x00, 0x0C, 0x00, 0x06, 0xD8, 0xC2, 0xA4, 0x56, 0x08, 0x0A, 0xBB, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x0C, 0x04, 0x23, 0x46, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x24, 0x96, 0x57, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x20, 0x50, 0x00, 0x0C, 0x00, 0x06, 0xD8, 0xC2, 0xA4, 0x56, 0x08, 0x0A, 0xBB, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x08, 0x02, 0x23, 0x2A, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x24, 0x16, 0x92, 0x02, 0x00, 0x80 },
	},
	// freq = 25.175 MHz
	{
		{ 0x85, 0x88, 0x20, 0x50, 0x00, 0x0C, 0x00, 0x06, 0xD8, 0xC2, 0xA4, 0x46, 0x07, 0x0A, 0xBB, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x0C, 0x03, 0x23, 0x38, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x24, 0x96, 0x6D, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x20, 0x50, 0x00, 0x0C, 0x00, 0x06, 0xD8, 0xC2, 0xA4, 0x56, 0x07, 0x0A, 0xBB, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x0C, 0x04, 0x23, 0x46, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x24, 0x96, 0x57, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x20, 0x50, 0x00, 0x0C, 0x00, 0x06, 0xD8, 0xC2, 0xA4, 0x56, 0x07, 0x0A, 0xBB, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x08, 0x02, 0x23, 0x2A, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x24, 0x56, 0x92, 0x02, 0x00, 0x80 },
	},
	// freq = 27 MHz
	{
		{ 0x85, 0x88, 0x00, 0xA0, 0x34, 0x07, 0x0D, 0x0B, 0xD8, 0xC2, 0xA4, 0x06, 0xD8, 0x5B, 0xE2, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x1C, 0x03, 0x23, 0x3C, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xFF, 0x55, 0x66, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x34, 0x07, 0x0D, 0x0B, 0xD8, 0xC2, 0xA4, 0x16, 0xD8, 0x5B, 0xE2, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x1C, 0x04, 0x23, 0x4B, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xFF, 0xD5, 0x51, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x34, 0x07, 0x0D, 0x0B, 0xD8, 0xC2, 0xA4, 0x16, 0xD8, 0x5B, 0xE2, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x18, 0x02, 0x23, 0x2D, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xFF, 0x55, 0x88, 0x02, 0x00, 0x80 },
	},
	// freq = 27.027 MHz
	{
		{ 0x85, 0x88, 0x00, 0xA0, 0x34, 0x07, 0x0D, 0x0B, 0xD8, 0xC2, 0xA4, 0x46, 0x28, 0x4D, 0xE3, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x1C, 0x03, 0x23, 0x3C, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xFE, 0x15, 0x66, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x34, 0x07, 0x0D, 0x0B, 0xD8, 0xC2, 0xA4, 0x56, 0x28, 0x4D, 0xE3, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x1C, 0x04, 0x23, 0x4B, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xFE, 0x95, 0x51, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x34, 0x07, 0x0D, 0x0B, 0xD8, 0xC2, 0xA4, 0x56, 0x28, 0x4D, 0xE3, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x18, 0x02, 0x23, 0x2D, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xFE, 0x55, 0x88, 0x02, 0x00, 0x80 },
	},
	// freq = 54 MHz
	{
		{ 0x85, 0x88, 0x00, 0xA0, 0x34, 0x07, 0x0D, 0x07, 0xD8, 0xC2, 0xA4, 0x06, 0xD8, 0x5B, 0xE2, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x18, 0x03, 0x23, 0x3C, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xFF, 0x54, 0x66, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x34, 0x07, 0x0D, 0x07, 0xD8, 0xC2, 0xA4, 0x16, 0xD8, 0x5B, 0xE2, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x18, 0x04, 0x23, 0x4B, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xFF, 0xD4, 0x51, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x34, 0x07, 0x0D, 0x07, 0xD8, 0xC2, 0xA4, 0x16, 0xD8, 0x5B, 0xE2, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x14, 0x02, 0x23, 0x2D, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xFF, 0x54, 0x88, 0x02, 0x00, 0x80 },
	},
	// freq = 54.054 MHz
	{
		{ 0x85, 0x88, 0x00, 0xA0, 0x34, 0x07, 0x0D, 0x07, 0xD8, 0xC2, 0xA4, 0x46, 0x28, 0x4D, 0xE3, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x18, 0x03, 0x23, 0x3C, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xFF, 0x14, 0x66, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x34, 0x07, 0x0D, 0x07, 0xD8, 0xC2, 0xA4, 0x56, 0x28, 0x4D, 0xE3, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x18, 0x04, 0x23, 0x4B, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xFF, 0x94, 0x51, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x34, 0x07, 0x0D, 0x07, 0xD8, 0xC2, 0xA4, 0x56, 0x28, 0x4D, 0xE3, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x14, 0x02, 0x23, 0x2D, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xFF, 0x54, 0x88, 0x02, 0x00, 0x80 },
	},
	// freq = 74.25 MHz
	{
		{ 0x85, 0x88, 0x00, 0xA0, 0x2C, 0x07, 0x00, 0x06, 0xD8, 0xC2, 0xA4, 0x06, 0xD8, 0x5B, 0x22, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x18, 0x03, 0xA3, 0x52, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xB9, 0x54, 0x4A, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x2C, 0x07, 0x00, 0x06, 0xD8, 0xC2, 0xA4, 0x16, 0xD8, 0x5B, 0x22, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x14, 0x01, 0xA3, 0x33, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xB9, 0x14, 0x77, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x2C, 0x07, 0x00, 0x06, 0xD8, 0xC2, 0xA4, 0x16, 0xD8, 0x5B, 0x22, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x14, 0x02, 0x23, 0x3E, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xB9, 0x14, 0x63, 0x02, 0x00, 0x80 },
	},
	// freq = 74.176 MHz
	{
		{ 0x85, 0x88, 0x00, 0xA0, 0x2C, 0x07, 0x00, 0x06, 0xD8, 0xC2, 0xA4, 0x46, 0xD8, 0x5B, 0x23, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x18, 0x03, 0xA3, 0x52, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xB9, 0x54, 0x4A, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x2C, 0x07, 0x00, 0x06, 0xD8, 0xC2, 0xA4, 0x56, 0xD8, 0x5B, 0x23, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x14, 0x01, 0xA3, 0x33, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xB9, 0x14, 0x77, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x2C, 0x07, 0x00, 0x06, 0xD8, 0xC2, 0xA4, 0x56, 0xD8, 0x5B, 0x23, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x14, 0x02, 0xE3, 0x3D, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xB9, 0x54, 0x63, 0x02, 0x00, 0x80 },
	},
	// freq = 148.5 MHz
	{
		{ 0x85, 0x88, 0x00, 0xA0, 0x2C, 0x07, 0x00, 0x02, 0xD8, 0xC2, 0xA4, 0x06, 0xD8, 0x5B, 0x22, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x14, 0x03, 0xA3, 0x52, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x5C, 0x54, 0x4A, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x2C, 0x07, 0x00, 0x02, 0xD8, 0xC2, 0xA4, 0x16, 0xD8, 0x5B, 0x22, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x10, 0x01, 0xA3, 0x33, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x5C, 0x14, 0x77, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x2C, 0x07, 0x00, 0x02, 0xD8, 0xC2, 0xA4, 0x16, 0xD8, 0x5B, 0x22, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x10, 0x02, 0x23, 0x3E, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x5C, 0x14, 0x63, 0x02, 0x00, 0x80 },
	},
	// freq = 148.352 MHz
	{
		{ 0x85, 0x88, 0x00, 0xA0, 0x2C, 0x07, 0x00, 0x02, 0xD8, 0xC2, 0xA4, 0x46, 0xD8, 0x5B, 0x23, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x14, 0x03, 0xA3, 0x52, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x5C, 0x54, 0x4A, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x2C, 0x07, 0x00, 0x02, 0xD8, 0xC2, 0xA4, 0x56, 0xD8, 0x5B, 0x23, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x10, 0x01, 0xA3, 0x33, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x5C, 0x14, 0x77, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x2C, 0x07, 0x00, 0x02, 0xD8, 0xC2, 0xA4, 0x56, 0xD8, 0x5B, 0x23, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x10, 0x02, 0xE3, 0x3D, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x5C, 0x54, 0x63, 0x02, 0x00, 0x80 },
	},
	// freq = 108.108 MHz
	{
		{ 0x85, 0x88, 0x00, 0xA0, 0x34, 0x07, 0x0D, 0x03, 0xD8, 0xC2, 0xA4, 0x46, 0x28, 0x4D, 0xE3, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x14, 0x03, 0x23, 0x3C, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x7F, 0x14, 0x66, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x34, 0x07, 0x0D, 0x03, 0xD8, 0xC2, 0xA4, 0x56, 0x28, 0x4D, 0xE3, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x14, 0x04, 0x23, 0x4B, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x7F, 0x94, 0x51, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x34, 0x07, 0x0D, 0x03, 0xD8, 0xC2, 0xA4, 0x56, 0x28, 0x4D, 0xE3, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x10, 0x02, 0x23, 0x2D, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x7F, 0x54, 0x88, 0x02, 0x00, 0x80 },
	},
	// freq = 72 MHz
	{
		{ 0x85, 0x88, 0x00, 0xA0, 0x30, 0x0B, 0x00, 0x02, 0xD8, 0xC2, 0xA4, 0x06, 0xD8, 0x5B, 0x82, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x08, 0x03, 0x23, 0x50, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xBF, 0x94, 0x4C, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x30, 0x0B, 0x00, 0x02, 0xD8, 0xC2, 0xA4, 0x16, 0xD8, 0x5B, 0x82, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x04, 0x01, 0x23, 0x32, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xBF, 0xD4, 0x7A, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x30, 0x0B, 0x00, 0x02, 0xD8, 0xC2, 0xA4, 0x16, 0xD8, 0x5B, 0x82, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x04, 0x02, 0x23, 0x3C, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xBF, 0x54, 0x66, 0x02, 0x00, 0x80 },
	},
	// freq = 25 MHz
	{
		{ 0x85, 0x88, 0x00, 0x30, 0x00, 0x0C, 0x00, 0x06, 0xD8, 0xC2, 0xA4, 0x06, 0xD8, 0x5B, 0xB2, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x0C, 0x03, 0xA3, 0x37, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x28, 0x56, 0x6E, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0x30, 0x00, 0x0C, 0x00, 0x06, 0xD8, 0xC2, 0xA4, 0x16, 0xD8, 0x5B, 0xB2, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x0C, 0x04, 0xA3, 0x45, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x28, 0x56, 0x58, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0x30, 0x00, 0x0C, 0x00, 0x06, 0xD8, 0xC2, 0xA4, 0x16, 0xD8, 0x5B, 0xB2, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x08, 0x02, 0xE3, 0x29, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x28, 0x56, 0x93, 0x02, 0x00, 0x80 },
	},
	// freq = 65 MHz
	{
		{ 0x85, 0x88, 0x40, 0xA0, 0x82, 0x0B, 0x00, 0x02, 0xD8, 0xC2, 0xA4, 0x06, 0xD8, 0x5B, 0x12, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x08, 0x03, 0x63, 0x48, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xD4, 0xD4, 0x54, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x40, 0xA0, 0x82, 0x0B, 0x00, 0x02, 0xD8, 0xC2, 0xA4, 0x16, 0xD8, 0x5B, 0x12, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x04, 0x01, 0x63, 0x2D, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xD4, 0xD4, 0x87, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x40, 0xA0, 0x82, 0x0B, 0x00, 0x02, 0xD8, 0xC2, 0xA4, 0x16, 0xD8, 0x5B, 0x12, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x04, 0x02, 0x63, 0x36, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0xD4, 0x54, 0x71, 0x02, 0x00, 0x80 },
	},
	// freq = 108 MHz
	{
		{ 0x85, 0x88, 0x00, 0xA0, 0x34, 0x07, 0x0D, 0x03, 0xD8, 0xC2, 0xA4, 0x06, 0xD8, 0x5B, 0xE2, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x14, 0x03, 0x23, 0x3C, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x7F, 0x54, 0x66, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x34, 0x07, 0x0D, 0x03, 0xD8, 0xC2, 0xA4, 0x16, 0xD8, 0x5B, 0xE2, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x14, 0x04, 0x23, 0x4B, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x7F, 0xD4, 0x51, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0xA0, 0x34, 0x07, 0x0D, 0x03, 0xD8, 0xC2, 0xA4, 0x16, 0xD8, 0x5B, 0xE2, 0x84, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x10, 0x02, 0x23, 0x2D, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x7F, 0x54, 0x88, 0x02, 0x00, 0x80 },
	},
	// freq = 162 MHz
	{
		{ 0x85, 0x88, 0x00, 0x10, 0x00, 0x06, 0x00, 0x00, 0xD8, 0xC2, 0xA4, 0x06, 0xD8, 0x5B, 0xF2, 0x83, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x10, 0x00, 0x23, 0x2D, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x54, 0x54, 0x88, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0x10, 0x00, 0x06, 0x00, 0x00, 0xD8, 0xC2, 0xA4, 0x16, 0xD8, 0x5B, 0xF2, 0x83, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x10, 0x01, 0x63, 0x38, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x54, 0x14, 0x6D, 0x02, 0x00, 0x80 },
		{ 0x85, 0x88, 0x00, 0x10, 0x00, 0x06, 0x00, 0x00, 0xD8, 0xC2, 0xA4, 0x16, 0xD8, 0x5B, 0xF2, 0x83, 0x03, 0x82, 0x55/*0x04*/, 0xE0, 0x00, 0x10, 0x02, 0xA3, 0x43, 0x38, 0x40, 0x88, 0x7C, 0x09, 0x13, 0x06, 0x00, 0x54, 0x54, 0xD4, 0x5A, 0x02, 0x00, 0x80 },
	}
};
#elif defined(_TCC9300_) || defined(_TCC8800_) || defined(_TCC8800ST_)
static const unsigned char phy_config[][3][31] = {// TCC9300 HDMI PHY Setting
        //25.200
    {
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x50, 0x11, 0x54, 0x58, 0x20, 0x5B, 0xA8, 0x84, 0x00, 0x00, 0x38, 0x00, 0x08, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0x11, 0x27, 0x00, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x50, 0x11, 0x54, 0x58, 0x21, 0x5B, 0xD2, 0x84, 0x00, 0x00, 0x38, 0x00, 0xB8, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xDA, 0x26, 0x00, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x10, 0x02, 0x54, 0x58, 0x20, 0x5B, 0xA8, 0x84, 0x00, 0x00, 0x38, 0x00, 0xA4, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xB6, 0x26, 0x00, 0x00, 0x00, 0x80},

    },
        //25.175
    {
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0xFC, 0x48, 0x6B, 0x50, 0x11, 0x54, 0x58, 0x20, 0x5B, 0xA8, 0x84, 0x00, 0x00, 0x38, 0x00, 0x08, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0x12, 0x27, 0x00, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0xFB, 0x48, 0x6B, 0x50, 0x11, 0x54, 0x58, 0x21, 0x5B, 0xD2, 0x84, 0x00, 0x00, 0x38, 0x00, 0xB8, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xDB, 0x26, 0x00, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0xFC, 0x48, 0x6B, 0x10, 0x02, 0x54, 0x58, 0x20, 0x5B, 0xA8, 0x84, 0x00, 0x00, 0x38, 0x00, 0xA4, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xB6, 0x26, 0x00, 0x00, 0x00, 0x80},

    },
        //27.000
    {
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x10, 0x02, 0x54, 0x38, 0x20, 0x5B, 0x78, 0x84, 0x00, 0x00, 0x38, 0x00, 0x08, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xFF, 0x26, 0x00, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x10, 0x02, 0x54, 0x38, 0x21, 0x5B, 0x96, 0x84, 0x00, 0x00, 0x38, 0x00, 0xB8, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xCC, 0x26, 0x00, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x10, 0x02, 0x54, 0x48, 0x21, 0x5B, 0xB4, 0x84, 0x00, 0x00, 0x38, 0x00, 0xA4, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xAA, 0x26, 0x00, 0x00, 0x00, 0x80},
    },
        //27.027
    {
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x02, 0x32, 0x6B, 0x10, 0x02, 0x54, 0x38, 0x20, 0x5B, 0x78, 0x84, 0x00, 0x00, 0x38, 0x00, 0x08, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xFF, 0x26, 0x00, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x02, 0x28, 0x6B, 0x10, 0x02, 0x54, 0x38, 0x21, 0x5B, 0x96, 0x84, 0x00, 0x00, 0x38, 0x00, 0xB8, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xCC, 0x26, 0x00, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x03, 0x32, 0x6B, 0x10, 0x02, 0x54, 0x48, 0x21, 0x5B, 0xB4, 0x84, 0x00, 0x00, 0x38, 0x00, 0xA4, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xA9, 0x26, 0x00, 0x00, 0x00, 0x80},
    },
        //54.000
    {
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x10, 0x01, 0x54, 0x38, 0x20, 0x5B, 0x78, 0x84, 0x00, 0x00, 0x38, 0x00, 0x08, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xFF, 0x26, 0x01, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x10, 0x01, 0x54, 0x38, 0x21, 0x5B, 0x96, 0x84, 0x00, 0x00, 0x38, 0x00, 0xF8, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xCC, 0x26, 0x01, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x10, 0x01, 0x54, 0x48, 0x21, 0x5B, 0xB4, 0x84, 0x00, 0x00, 0x38, 0x00, 0xE4, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xAA, 0x26, 0x01, 0x00, 0x00, 0x80},
    },
        //54.054
    {
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x02, 0x32, 0x6B, 0x10, 0x01, 0x54, 0x38, 0x20, 0x5B, 0x78, 0x84, 0x00, 0x00, 0x38, 0x00, 0x08, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xFF, 0x26, 0x01, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x02, 0x28, 0x6B, 0x10, 0x01, 0x54, 0x38, 0x21, 0x5B, 0x96, 0x84, 0x00, 0x00, 0x38, 0x00, 0xF8, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xCC, 0x26, 0x01, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x03, 0x32, 0x6B, 0x10, 0x01, 0x54, 0x48, 0x21, 0x5B, 0xB4, 0x84, 0x00, 0x00, 0x38, 0x00, 0xE4, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xA9, 0x26, 0x01, 0x00, 0x00, 0x80},
    },
        //74.250
    {
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x10, 0x01, 0x54, 0x98, 0x21, 0x5B, 0xA5, 0x84, 0x00, 0x20, 0x38, 0x00, 0x08, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xB9, 0x26, 0x01, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x30, 0x40, 0x6B, 0x10, 0x01, 0x54, 0x28, 0x32, 0x5B, 0xCE, 0x84, 0x00, 0x00, 0x38, 0x00, 0xF8, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0x94, 0x26, 0x01, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x20, 0x40, 0x6B, 0x10, 0x01, 0x54, 0x98, 0x32, 0x5B, 0xF8, 0x84, 0x00, 0x00, 0x38, 0x00, 0xE4, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xF7, 0x26, 0x02, 0x00, 0x00, 0x80},
    },
        //74.176
    {
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x56, 0x5B, 0x6B, 0x10, 0x01, 0x54, 0x18, 0x25, 0x5B, 0xA5, 0x84, 0x00, 0x00, 0x38, 0x00, 0x08, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xB9, 0x26, 0x01, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x3E, 0x5B, 0x6B, 0x10, 0x01, 0x54, 0x28, 0x32, 0x5B, 0xCE, 0x84, 0x00, 0x00, 0x38, 0x00, 0xF8, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0x94, 0x26, 0x01, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x26, 0x5B, 0x6B, 0x10, 0x01, 0x54, 0x98, 0x32, 0x5B, 0xF7, 0x84, 0x00, 0x00, 0x38, 0x00, 0xE4, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xF7, 0x26, 0x02, 0x00, 0x00, 0x80},
    },
        //148.500
    {
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x18, 0x01, 0x54, 0x98, 0x21, 0x5B, 0xA5, 0x84, 0x00, 0x20, 0x38, 0x00, 0x08, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xB9, 0x26, 0x02, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x30, 0x40, 0x6B, 0x18, 0x01, 0x54, 0x28, 0x32, 0x5B, 0xCE, 0x84, 0x00, 0x00, 0x38, 0x00, 0xF8, 0x10, 0xE0, /*0x22, 0x40,*/ 0x22,0x40, 0x94, 0x26, 0x02, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x20, 0x40, 0x6B, 0x18, 0x01, 0x54, 0x98, 0x32, 0x5B, 0xF8, 0x84, 0x00, 0x00, 0x38, 0x00, 0xE4, 0x10, 0xE0, /*0x22, 0x40,*/ 0x77,0x44, 0x7B, 0x26, 0x02, 0x00, 0x00, 0x80},
    },
        //148.352
    {
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x56, 0x5B, 0x6B, 0x18, 0x01, 0x54, 0x18, 0x25, 0x5B, 0xA5, 0x84, 0x00, 0x00, 0x38, 0x00, 0x08, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xB9, 0x26, 0x02, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x3E, 0x5B, 0x6B, 0x18, 0x01, 0x54, 0x28, 0x32, 0x5B, 0xCE, 0x84, 0x00, 0x00, 0x38, 0x00, 0xF8, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0x94, 0x26, 0x02, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x26, 0x5B, 0x6B, 0x18, 0x01, 0x54, 0x98, 0x32, 0x5B, 0xF7, 0x84, 0x00, 0x00, 0x38, 0x00, 0xE4, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0x7B, 0x26, 0x02, 0x00, 0x00, 0x80},
    },
        //108.108
    {
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x02, 0x32, 0x6B, 0x18, 0x01, 0x54, 0x38, 0x20, 0x5B, 0x78, 0x84, 0x00, 0x00, 0x38, 0x00, 0x08, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xFF, 0x26, 0x02, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x02, 0x28, 0x6B, 0x18, 0x01, 0x54, 0x38, 0x21, 0x5B, 0x96, 0x84, 0x00, 0x00, 0x38, 0x00, 0xF8, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xCC, 0x26, 0x02, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x06, 0x64, 0x6B, 0x18, 0x01, 0x54, 0x48, 0x21, 0x5B, 0xB4, 0x84, 0x00, 0x00, 0x38, 0x00, 0xE4, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xA9, 0x26, 0x02, 0x00, 0x00, 0x80},
    },
        //72.000
    {
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x18, 0x01, 0x54, 0x68, 0x21, 0x5B, 0x50, 0x84, 0x00, 0x21, 0x38, 0x00, 0x08, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xBF, 0x26, 0x01, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x18, 0x01, 0x54, 0x88, 0x21, 0x5B, 0x64, 0x84, 0x00, 0x21, 0x38, 0x00, 0xF8, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0x99, 0x26, 0x01, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x18, 0x01, 0x54, 0x38, 0x20, 0x5B, 0x78, 0x84, 0x00, 0x00, 0x38, 0x00, 0xE4, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xFF, 0x26, 0x02, 0x00, 0x00, 0x80},
    },
        //25.000
    {
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0xE8, 0x36, 0x6B, 0x50, 0x11, 0x54, 0x58, 0x20, 0x5B, 0xA7, 0x84, 0x00, 0x00, 0x38, 0x00, 0x08, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0x13, 0x27, 0x00, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0xE2, 0x36, 0x6B, 0x50, 0x11, 0x54, 0x58, 0x21, 0x5B, 0xD0, 0x84, 0x00, 0x00, 0x38, 0x00, 0xB8, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xDC, 0x26, 0x00, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0xE8, 0x36, 0x6B, 0x10, 0x02, 0x54, 0x58, 0x20, 0x5B, 0xA7, 0x84, 0x00, 0x00, 0x38, 0x00, 0xA4, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xB7, 0x26, 0x00, 0x00, 0x00, 0x80},
    },
        //65.000
    {
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x08, 0x36, 0x6B, 0x10, 0x01, 0x54, 0x48, 0x20, 0x5B, 0x90, 0x84, 0x00, 0x00, 0x38, 0x00, 0x08, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xD4, 0x26, 0x01, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x0A, 0x36, 0x6B, 0x10, 0x01, 0x54, 0x88, 0x22, 0x5B, 0xB5, 0x84, 0x00, 0x20, 0x38, 0x00, 0xF8, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xA9, 0x26, 0x01, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x9C, 0x0C, 0x36, 0x6B, 0x10, 0x01, 0x54, 0x48, 0x22, 0x5B, 0xD9, 0x84, 0x00, 0x00, 0x38, 0x00, 0xE4, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0x8D, 0x26, 0x01, 0x00, 0x00, 0x80},
    },
        //108.000
    {
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x18, 0x01, 0x54, 0x38, 0x20, 0x5B, 0x78, 0x84, 0x00, 0x00, 0x38, 0x00, 0x08, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xFF, 0x26, 0x02, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x18, 0x01, 0x54, 0x38, 0x21, 0x5B, 0x96, 0x84, 0x00, 0x00, 0x38, 0x00, 0xF8, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xCC, 0x26, 0x02, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x18, 0x01, 0x54, 0x48, 0x21, 0x5B, 0xB4, 0x84, 0x00, 0x00, 0x38, 0x00, 0xE4, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xAA, 0x26, 0x02, 0x00, 0x00, 0x80},
    },
        //162.000
    {
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x18, 0x01, 0x54, 0x48, 0x21, 0x5B, 0xB4, 0x84, 0x00, 0x00, 0x38, 0x00, 0x08, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0xAA, 0x26, 0x02, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x18, 0x01, 0x54, 0xD8, 0x21, 0x5B, 0xE1, 0x84, 0x00, 0x20, 0x38, 0x00, 0xF8, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0x88, 0x26, 0x02, 0x00, 0x00, 0x80},
		{0x05, 0x00, 0xD8, 0x10, 0x1C, 0x30, 0x40, 0x6B, 0x18, 0x01, 0x54, 0x78, 0x21, 0x5B, 0x0E, 0x85, 0x00, 0x00, 0x38, 0x00, 0xE4, 0x10, 0xE0, /*0x22, 0x40,*/ 0x00,0x40, 0x71, 0x26, 0x02, 0x00, 0x00, 0x80},
    }
};
#else
#error not defined chip
#endif

/*
void sTorm_delay_us(unsigned int us)
{
	volatile unsigned int i;
	while(us--)
	{
	//	for(i=0 ; i<20 ; i++) __asm__ __volatile__ ("nop");
		for(i=0 ; i<40 ; i++) __asm__ __volatile__ ("nop");		// <<!!
	}
}

void sTorm_delay_ms(unsigned int ms)
{
	sTorm_delay_us(1000*ms);
}
*/

/**
 * Write data though I2C. For more information of I2C, refer I2C Spec.
 * @param   devAddr    [in]    Device address
 * @param   offset  [in]    Byte offset
 * @param   size    [in]    Size of data
 * @param   buffer  [out]   Pointer to buffer
 * @return  If succeed in writing, return 1;Otherwise, return 0
 */
int PHYWrite(unsigned char devAddr, unsigned char offset, unsigned char size, unsigned char* buffer)
{
	unsigned char temp_mem[64];
    unsigned char* temp;
	int i;
    //int bytes;
    int retval = 0;

//TODO: debugging only
#if 0
    printf("------------------------------------------------\n");
    printf("%s(addr=0x%x, offset=0x%x, size=%d, buffer)\n", __FUNCTION__, devAddr, offset, size);

    i=0;
    do {
        if (i%8) {
            printf("0x%x ", buffer[i]);
        } else {
//TODO: BUG!!!!!!!!!!!!!!!!!!!
//????????
            printf("\n0x%x : 0x%x ", i, buffer[i]);
        }
        i++;
    } while (i<size);
    printf("\n");

    printf("------------------------------------------------\n");
#endif

    // allocate temporary buffer
	//temp = (unsigned char*) malloc((size+1)*sizeof(unsigned char));
	temp = &temp_mem[0];
    if (!temp)
    {
        printf("not enough resources at %s\n", __FUNCTION__);
        return retval;
    }

    temp[0] = offset;

	#if 0
    memcpy(&temp[1], &buffer[0], size);
	#else
	for (i=0; i<size; i++)
		temp[i+1] = buffer[i];
	#endif

    // write temp buffer
    #if !defined (PLATFORM_TCC892X)
	i2c_xfer(devAddr, size+1, temp, 0, 0, SMU_I2C_CH1);
	#endif

    retval = 1;

//exit:
    // free temp buffer
    //if (temp) free(temp);

    return retval;
}



/**
 * This function configures PHY registers via I2C.
 *
 * @param    clk    [in]    Pixel clock value.\n
 *                ex : PIXEL_FREQ_25_200, PIXEL_FREQ_74_250...
 * @param    depth    [in]    Color bit depth.\n
 *                 One of (HDMI_CD_24, HDMI_CD_30, HDMI_CD_36).
 * @return    1=Success, 0=Fail
 */
int PHYConfig(const enum PixelFreq clk, const enum ColorDepth depth)
{
    int index, freq;
    int size;
    unsigned char *buffer;
    unsigned char reg;

    // get depth index
    switch (depth)
    {
        case HDMI_CD_24:
            index = 0;
            break;
        case HDMI_CD_30:
            index = 1;
            break;
        case HDMI_CD_36:
            index = 2;
            break;
        default:
            DPRINTF("not available depth arg = %d\n", (int)depth);
            return 0;
    }

    // get clk freq index
    switch (clk)
    {
        case PIXEL_FREQ_25_200 :
            freq = 0;
            break;
        case PIXEL_FREQ_25_175 :
            freq = 1;
            break;
        case PIXEL_FREQ_27 :
            freq = 2;
            break;
        case PIXEL_FREQ_27_027 :
            freq = 3;
            break;
        case PIXEL_FREQ_54 :
            freq = 4;
            break;
        case PIXEL_FREQ_54_054 :
            freq = 5;
            break;
        case PIXEL_FREQ_74_250 :
            freq = 6;
            break;
        case PIXEL_FREQ_74_176 :
            freq = 7;
            break;
        case PIXEL_FREQ_148_500 :
            freq = 8;
            break;
        case PIXEL_FREQ_148_352 :
            freq = 9;
            break;
        case PIXEL_FREQ_108_108 :
            freq = 10;
            break;
        case PIXEL_FREQ_72 :
            freq = 11;
            break;
        case PIXEL_FREQ_25 :
            freq = 12;
            break;
        case PIXEL_FREQ_65 :
            freq = 13;
            break;
        case PIXEL_FREQ_108 :
            freq = 14;
            break;
        case PIXEL_FREQ_162 :
            freq = 15;
            break;
        default:
            DPRINTF("not availlable clk arg = %d\n",(int)clk);
            return 0;
    }

    // start to reconfig after that phy_ready goes down
    reg = 0x00;
    if (!PHYWrite(PHY_I2C_ADDRESS,PHY_REG_MODE_SET_DONE, 1, &reg))
    {
        DPRINTF("fail to write reconfig 0x02%x\n",PHY_REG_MODE_SET_DONE);
        return 0;
    }

	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}

    size = sizeof(phy_config[freq][index]) / sizeof(phy_config[freq][index][0]);
    buffer = (unsigned char *) phy_config[freq][index];

    if (!PHYWrite(PHY_I2C_ADDRESS, PHY_CONFIG_START_OFFSET, size, buffer))
    {
        return 0;
    }

    //for ( idx = 0 ; idx < 1000 ; idx ++ );            // delay

    return 1;
}


//==============================================================================
//                 Section x - HDMI Link / LCDC Timming
//==============================================================================

//! Structure for video timing parameters
static const struct hdmi_video_params
{
    /** [H Blank] */
    unsigned int    HBlank;

    /** [V Blank] */
    unsigned int    VBlank;

    /**
     * [H Total : V Total] @n
     * For more information, refer HDMI register map.
     */
    unsigned int    HVLine;

    /**
     * [H Sync polarity : H Sync end point : H Sync start point]@n
     * For more information, refer HDMI register map.
     */
    unsigned int    HSYNCGEN;

    /**
     * [V Sync start line num + V Sync end line num] @n
     * For more information, refer HDMI register map.
     */
    unsigned int    VSYNCGEN;

    /** CEA VIC */
    unsigned char   AVI_VIC;
    /** CEA VIC for 16:9 pixel ratio */
    unsigned char   AVI_VIC_16_9;

    /** 0 - progresive, 1 - interlaced */
    unsigned char   interlaced;

    /** Pixel repetition if double, set 1 */
    unsigned char   repetition;

    /** V Sync polarity */
    unsigned char   polarity;

    /**
     * In case of interlaced mode, @n
     * [end point of bottom field's active region : start point of that]@n
     * For more information, refer HDMI register map.
     */
    unsigned int    VBLANK_F;

    /**
     * In case of interlaced mode, @n
     * [start line of bottom field's V Sync : end line of that]@n
     * For more information, refer HDMI register map.
     */
    unsigned int    VSYNCGEN2;

    /**
     * In case of interlaced mode, @n
     * [start transition point of bottom field's V Sync : end transition of that]@n
     * For more information, refer HDMI register map.
     */
    unsigned int    VSYNCGEN3;

    /** Pixel frequency */
    enum PixelFreq PixelClock; // pixel clock
} HDMIVideoParams[] =
{

  //{ 0x140, 0x13326, 0x540326, 0x1070A0, 0x2008 , 1 , 1 , 0, 0, 0, 0       , 0       , 0       , PIXEL_FREQ_65, },  // v1024x768p_60Hz
  //{ 0x140, 0x13326, 0x540326, 0x1701C , 0x3009 , 1 , 1 , 0, 0, 0, 0       , 0       , 0       , PIXEL_FREQ_65, },  // v1024x768p_60Hz
    { 0xA0 , 0x16A0D, 0x32020D, 0x11B80E, 0xA00C , 1 , 1 , 0, 0, 1, 0       , 0       , 0       , PIXEL_FREQ_25_200,  },  // v640x480p_60Hz
    { 0x8A , 0x16A0D, 0x35A20D, 0x11300E, 0x900F , 2 , 3 , 0, 0, 1, 0       , 0       , 0       , PIXEL_FREQ_27_027,  },  // v720x480p_60Hz
    { 0x172, 0xF2EE , 0x6722EE, 0x2506C , 0x500A , 4 , 4 , 0, 0, 0, 0       , 0       , 0       , PIXEL_FREQ_74_250,  },  // v1280x720p_60Hz
    { 0x118, 0xB232 , 0x898465, 0x20856 , 0x2007 , 5 , 5 , 1, 0, 0, 0x232A49, 0x234239, 0x4A44A4, PIXEL_FREQ_74_250,  },  // v1920x1080i_60Hz
    { 0x114, 0xB106 , 0x6B420D, 0x128024, 0x4007 , 6 , 7 , 1, 1, 1, 0x10691D, 0x10A10D, 0x380380, PIXEL_FREQ_27_027,  },  // v720x480i_60Hz
    { 0x114, 0xB106 , 0x6B4106, 0x128024, 0x4007 , 8 , 9 , 0, 1, 1, 0       , 0       , 0       , PIXEL_FREQ_27_027,  },  // v720x240p_60Hz
    { 0x228, 0xB106 , 0xD6820D, 0x15084A, 0x4007 , 10, 11, 1, 1, 1, 0x10691D, 0x10A10D, 0x700700, PIXEL_FREQ_54_054,  },  // v2880x480i_60Hz
    { 0x228, 0xB106 , 0x6B4106, 0x15084A, 0x4007 , 12, 13, 0, 1, 1, 0       , 0       , 0       , PIXEL_FREQ_54_054,  },  // v2880x240p_60Hz
    { 0x114, 0x16A0D, 0x6B420D, 0x12681E, 0x900F , 14, 15, 0, 1, 1, 0       , 0       , 0       , PIXEL_FREQ_54_054,  },  // v1440x480p_60Hz
    { 0x118, 0x16C65, 0x898465, 0x20856 , 0x4009 , 16, 16, 0, 0, 0, 0       , 0       , 0       , PIXEL_FREQ_148_500, },  // v1920x1080p_60Hz
    { 0x90 , 0x18A71, 0x360271, 0x11280A, 0x500A , 17, 18, 0, 0, 1, 0       , 0       , 0       , PIXEL_FREQ_27,      },  // v720x576p_50Hz
    { 0x2BC, 0xF2EE , 0x7BC2EE, 0x779B6 , 0x500A , 19, 19, 0, 0, 0, 0       , 0       , 0       , PIXEL_FREQ_74_250,  },  // v1280x720p_50Hz
    { 0x2D0, 0xB232 , 0xA50465, 0x8EA0E , 0x2007 , 20, 20, 1, 0, 0, 0x232A49, 0x234239, 0x738738, PIXEL_FREQ_74_250,  },  // v1920x1080i_50Hz
    { 0x120, 0xC138 , 0x6C0271, 0x125016, 0x2005 , 21, 22, 1, 1, 1, 0x138951, 0x13A13D, 0x378378, PIXEL_FREQ_27,      },  // v720x576i_50Hz
    { 0x120, 0xC138 , 0x6C0138, 0x125016, 0x3006 , 23, 24, 0, 1, 1, 0       , 0       , 0       , PIXEL_FREQ_27,      },  // v720x288p_50Hz
    { 0x240, 0xC138 , 0xD80271, 0x14A82E, 0x2005 , 25, 26, 1, 1, 1, 0x138951, 0x13A13D, 0x6F06F0, PIXEL_FREQ_54,      },  // v2880x576i_50Hz
    { 0x240, 0xC138 , 0xD80138, 0x14A82E, 0x2005 , 27, 28, 0, 1, 1, 0       , 0       , 0       , PIXEL_FREQ_54,      },  // v2880x288p_50Hz
    { 0x120, 0x18A71, 0x6C0271, 0x125816, 0x500A , 29, 30, 0, 1, 1, 0       , 0       , 0       , PIXEL_FREQ_54,      },  // v1440x576p_50Hz
    { 0x2D0, 0x16C65, 0xA50465, 0x8EA0E , 0x4009 , 31, 31, 0, 0, 0, 0       , 0       , 0       , PIXEL_FREQ_148_500, },  // v1920x1080p_50Hz
    { 0x33E, 0x16C65, 0xABE465, 0xAA27C , 0x4009 , 32, 32, 0, 0, 0, 0       , 0       , 0       , PIXEL_FREQ_74_250,  },  // v1920x1080p_24Hz
    { 0x2D0, 0x16C65, 0xA50465, 0x8EA0E , 0x4009 , 33, 33, 0, 0, 0, 0       , 0       , 0       , PIXEL_FREQ_74_250,  },  // v1920x1080p_25Hz
    { 0x2D0, 0x16C65, 0xA50465, 0x8EA0E , 0x4009 , 34, 34, 0, 0, 0, 0       , 0       , 0       , PIXEL_FREQ_74_250,  },  // v1920x1080p_30Hz
    { 0x228, 0x16A0D, 0xD6820D, 0x14D83E, 0x900F , 35, 36, 0, 0, 1, 0       , 0       , 0       , PIXEL_FREQ_108_108, },  // v2880x480p_60Hz
    { 0x240, 0x18A71, 0xD80271, 0x14B82E, 0x500A , 37, 38, 0, 1, 1, 0       , 0       , 0       , PIXEL_FREQ_108,     },  // v2880x576p_50Hz
    { 0x180, 0x2AA71, 0x9004E2, 0x3181E , 0x1701C, 39, 39, 0, 0, 0, 0x2712C6, 0x28728F, 0x4a44a4, PIXEL_FREQ_72,      },  // v1920x1080i_50Hz(1250)
    { 0x2D0, 0xB232 , 0xA50465, 0x8EA0E , 0x2007 , 40, 40, 1, 0, 0, 0x232A49, 0x234239, 0x738738, PIXEL_FREQ_148_500, },  // v1920x1080i_100Hz
    { 0x2BC, 0xF2EE , 0x7BC2EE, 0x779B6 , 0x500A , 41, 41, 0, 0, 0, 0       , 0       , 0       , PIXEL_FREQ_148_500, },  // v1280x720p_100Hz
    { 0x90 , 0x18A71, 0x360271, 0x11280A, 0x500A , 42, 43, 0, 0, 1, 0       , 0       , 0       , PIXEL_FREQ_54,      },  // v720x576p_100Hz
    { 0x120, 0xC138 , 0x6C0271, 0x125016, 0x2005 , 44, 45, 1, 1, 1, 0x138951, 0x13A13D, 0x378378, PIXEL_FREQ_54,      },  // v720x576i_100Hz
    { 0x118, 0xB232 , 0x898465, 0x20856 , 0x2007 , 46, 46, 1, 0, 0, 0x232A49, 0x234239, 0x4A44A4, PIXEL_FREQ_148_500, },  // v1920x1080i_120Hz
    { 0x172, 0xF2EE , 0x6722EE, 0x2506C , 0x500A , 47, 47, 0, 0, 0, 0       , 0       , 0       , PIXEL_FREQ_148_500, },  // v1280x720p_120Hz
    { 0x8A , 0x16A0D, 0x35A20D, 0x11300E, 0x900F , 48, 49, 0, 0, 1, 0       , 0       , 0       , PIXEL_FREQ_54_054,  },  // v720x480p_120Hz
    { 0x114, 0xB106 , 0x6B420D, 0x128024, 0x4007 , 50, 51, 1, 1, 1, 0x10691D, 0x10A10D, 0x380380, PIXEL_FREQ_54_054,  },  // v720x480i_120Hz
    { 0x90 , 0x18A71, 0x360271, 0x11280A, 0x500A , 52, 53, 0, 0, 1, 0       , 0       , 0       , PIXEL_FREQ_108,     },  // v720x576p_200Hz
    { 0x120, 0xC138 , 0x6C0271, 0x125016, 0x2005 , 54, 55, 1, 1, 1, 0x138951, 0x13A13D, 0x378378, PIXEL_FREQ_108,     },  // v720x576i_200Hz
    { 0x8A , 0x16A0D, 0x35A20D, 0x11300E, 0x900F , 56, 57, 0, 0, 1, 0       , 0       , 0       , PIXEL_FREQ_108_108, },  // v720x480p_240Hz
    { 0x114, 0xB106 , 0x6B420D, 0x128024, 0x4007 , 58, 59, 1, 1, 1, 0x10691D, 0x10A10D, 0x380380, PIXEL_FREQ_108_108, },  // v720x480i_240Hz
};


typedef struct _lcdc_timimg_parms_t {

	unsigned int id;
	unsigned int iv;
	unsigned int ih;
	unsigned int ip;
	unsigned int dp;
	unsigned int ni;
	unsigned int tv;
	unsigned int tft;
	unsigned int stn;

	//LHTIME1
	unsigned int lpw;
	unsigned int lpc;
	//LHTIME2
	unsigned int lswc;
	unsigned int lewc;
	//LVTIME1
	unsigned int vdb;
	unsigned int vdf;
	unsigned int fpw;
	unsigned int flc;
	//LVTIME2
	unsigned int fswc;
	unsigned int fewc;
	//LVTIME3
	unsigned int fpw2;
	unsigned int flc2;
	//LVTIME4
	unsigned int fswc2;
	unsigned int fewc2;

}lcdc_timimg_parms_t;

#if (0)
#define DEFAULT_HDMI_LCDC_TIMING	{ 0,  1,  1,  0,  0,  1,  0,  1,  0,  61 ,    639 ,   59 ,     37 ,    0,  0,  5,  479 ,   29,  8,   5,   479 ,    29,  8, /*640x480p @ 60Hz       25.200MHz   */ }
#else
#define DEFAULT_HDMI_LCDC_TIMING	{ 0,  0,  0,  0,  0,  1,  0,  1,  0,  43 ,    1919,   147,     87 ,    0,  0,  4,  1079,   35,  3,   4,   1079,    35,  3, /*1920x1080p @ 60Hz     148.5MHz    */ }
#endif

static const lcdc_timimg_parms_t LCDCTimimgParams[] = 
{
#if 0                                              /*  ID  IV  IH  IP  DP  NI  TV TFT STN  LPW       LPC   LSWC     LEWC   VDB VDF FPW    FLC  FSWC FEWC FPW2   FLC2  FSWC2 FEWC2 */
  /* v1024x768p_60Hz        PIXEL_FREQ_65    ,  */    { 0,  1,  1,  0,  0,  1,  0,  1,  0,  135,    1023,   159,     23 ,    0,  0,  5,  767 ,   28,  2,   5,   767 ,    28,  2, /*1024x768p @ 60Hz       65.000MHz   */ },
#else
  /* v640x480p_60Hz         PIXEL_FREQ_25_200,  */    { 0,  1,  1,  0,  0,  1,  0,  1,  0,  61 ,    639 ,   59 ,     37 ,    0,  0,  5,  479 ,   29,  8,   5,   479 ,    29,  8, /*640x480p @ 60Hz       25.200MHz   */ },
#endif
  /* v720x480p_60Hz         PIXEL_FREQ_27_027,  */    { 0,  1,  1,  0,  0,  1,  0,  1,  0,  61 ,    719 ,   59 ,     15 ,    0,  0,  5,  479 ,   29,  8,   5,   479 ,    29,  8, /*720x480p @ 60Hz       27.027MHz   */ },
  /* v1280x720p_60Hz        PIXEL_FREQ_74_250,  */    { 0,  0,  0,  0,  0,  1,  0,  1,  0,  39 ,    1279,   219,     109,    0,  0,  4,  719 ,   19,  4,   4,   719 ,    19,  4, /*1280x720p @ 60Hz      74.25MHz    */ },
  /* v1920x1080i_60Hz       PIXEL_FREQ_74_250,  */    { 0,  0,  0,  0,  0,  0,  1,  0,  0,  43 ,    1919,   147,     87 ,    0,  0,  9,  1079,   29,  3,   9,   1079,    31,  3, /*1920x1080i @ 60Hz     74.25MHz    */ },
  /* v720x480i_60Hz         PIXEL_FREQ_27_027,  */    { 0,  1,  1,  0,  1,  0,  1,  0,  0,  123,    1439,   113,     37 ,    0,  0,  5,  479 ,   29,  7,   5,   479 ,    31,  7, /*720x480i @ 60Hz       27.027MHz   */ },
  /* v720x240p_60Hz         PIXEL_FREQ_27_027,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v2880x480i_60Hz        PIXEL_FREQ_54_054,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v2880x240p_60Hz        PIXEL_FREQ_54_054,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1440x480p_60Hz        PIXEL_FREQ_54_054,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080p_60Hz       PIXEL_FREQ_148_500, */    { 0,  0,  0,  0,  0,  1,  0,  1,  0,  43 ,    1919,   147,     87 ,    0,  0,  4,  1079,   35,  3,   4,   1079,    35,  3, /*1920x1080p @ 60Hz     148.5MHz    */ },
  /* v720x576p_50Hz         PIXEL_FREQ_27,      */    { 0,  1,  1,  0,  0,  1,  0,  1,  0,  63 ,    719 ,   67 ,     11 ,    0,  0,  4,  575 ,   38,  4,   4,   575 ,    38,  4, /*720x576p @ 50Hz       27MHz       */ },
  /* v1280x720p_50Hz        PIXEL_FREQ_74_250,  */    { 0,  0,  0,  0,  0,  1,  0,  1,  0,  39 ,    1279,   219,     439,    0,  0,  4,  719 ,   19,  4,   4,   719 ,    19,  4, /*1280x720p @ 50Hz      74.25MHz    */ },
  /* v1920x1080i_50Hz       PIXEL_FREQ_74_250,  */    { 0,  0,  0,  0,  0,  0,  1,  0,  0,  43 ,    1919,   174,     500,    0,  0,  9,  1079,   29,  3,   9,   1079,    31,  3, /*1920x1080i @ 50Hz     74.25MHz    */ },
  /* v720x576i_50Hz         PIXEL_FREQ_27,      */    { 0,  1,  1,  0,  1,  0,  1,  0,  0,  125,    1439,   137,     23 ,    0,  0,  5,  575 ,   37,  3,   5,   575 ,    39,  3, /*720x576i @ 50Hz       27MHz       */ },
  /* v720x288p_50Hz         PIXEL_FREQ_27,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v2880x576i_50Hz        PIXEL_FREQ_54,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v2880x288p_50Hz        PIXEL_FREQ_54,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1440x576p_50Hz        PIXEL_FREQ_54,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080p_50Hz       PIXEL_FREQ_148_500, */    { 0,  0,  0,  0,  0,  1,  0,  1,  0,  43 ,    1919,   174,     500,    0,  0,  4,  1079,   35,  3,   4,   1079,    35,  3, /*1920x1080p @ 50Hz     148.5MHz    */ },
  /* v1920x1080p_24Hz       PIXEL_FREQ_74_250,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080p_25Hz       PIXEL_FREQ_74_250,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080p_30Hz       PIXEL_FREQ_74_250,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v2880x480p_60Hz        PIXEL_FREQ_108_108, */    DEFAULT_HDMI_LCDC_TIMING,
  /* v2880x576p_50Hz        PIXEL_FREQ_108,     */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080i_50Hz(1250) PIXEL_FREQ_72,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080i_100Hz      PIXEL_FREQ_148_500, */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1280x720p_100Hz       PIXEL_FREQ_148_500, */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x576p_100Hz        PIXEL_FREQ_54,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x576i_100Hz        PIXEL_FREQ_54,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080i_120Hz      PIXEL_FREQ_148_500, */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1280x720p_120Hz       PIXEL_FREQ_148_500, */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x480p_120Hz        PIXEL_FREQ_54_054,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x480i_120Hz        PIXEL_FREQ_54_054,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x576p_200Hz        PIXEL_FREQ_108,     */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x576i_200Hz        PIXEL_FREQ_108,     */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x480p_240Hz        PIXEL_FREQ_108_108, */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x480i_240Hz        PIXEL_FREQ_108_108, */    DEFAULT_HDMI_LCDC_TIMING,
};

/**
 * N value of ACR packet.@n
 * 4096  is the N value for 32 KHz sampling frequency @n
 * 6272  is the N value for 44.1 KHz sampling frequency @n
 * 12544 is the N value for 88.2 KHz sampling frequency @n
 * 25088 is the N value for 176.4 KHz sampling frequency @n
 * 6144  is the N value for 48 KHz sampling frequency @n
 * 12288 is the N value for 96 KHz sampling frequency @n
 * 24576 is the N value for 192 KHz sampling frequency @n
 */
static const unsigned int ACR_N_params[] =
{
    4096,
    6272,
    12544,
    25088,
    6144,
    12288,
    24576
};

void hdmi_init (void)
{
	unsigned char reg;
	unsigned int regl;
	unsigned char phy_status;
	unsigned int phy_chk_cnt = 0;
	
	#if 0
	tcc_pca953x_setup(PCA9539_U3_SLAVE_ADDR, Hw14 /*PWRGP4      */, OUTPUT, HIGH, SET_DIRECTION|SET_VALUE);
	tcc_pca953x_setup(PCA9539_U2_SLAVE_ADDR, Hw7  /*HDMI_ON     */, OUTPUT, HIGH, SET_DIRECTION|SET_VALUE);
	tcc_pca953x_setup(PCA9538_U4_SLAVE_ADDR, Hw2  /*HDMI_LVDS_ON*/, OUTPUT, HIGH, SET_DIRECTION|SET_VALUE); // for HPD 5V
	--> pca953x_init()
	#endif

    #if !defined(_TCC9300_) && !defined(_TCC8800_) && !defined(_TCC8800ST_)
	tca_ckc_setiopwdn(RB_SPDIFTXCONTROLLER, 0);
	tca_ckc_setperi(PERI_SPDIF, ENABLE, ((44100 * 512) / 100));
	#endif

	{volatile int ttt;for(ttt=0;ttt<0x5000;ttt++);}

    #if defined(_TCC9300_) || defined(_TCC8800_) || defined(_TCC8800ST_)
	tca_ckc_setperi(PERI_HDMI, ENABLE, 10000);
	#endif
	
	// disable HDMI PHY Power-off
	regl = readl(PMU_PWROFF);
	#if defined(_TCC9300_) || defined(_TCC8800_) || defined(_TCC8800ST_)
	writel(regl | PWROFF_HDMIPHY, PMU_PWROFF);
	#else
	writel(regl & ~PWROFF_HDMIPHY, PMU_PWROFF);
	#endif
	//tca_ckc_setpmupwroff(PMU_HDMIPHY, DISABLE);

	// disable HDMI Power-down
	regl = readl(DDICFG_PWDN);
	writel(regl & ~PWDN_HDMI, DDICFG_PWDN);
	//tca_ckc_setddipwdn(DDIPWDN_HDMI, DISABLE);

	// swreset DDI_BUS HDMI
	regl = readl(DDICFG_SWRESET);
	writel(regl | SWRESET_HDMI, DDICFG_SWRESET);
	{volatile int ttt;for(ttt=0;ttt<0x100;ttt++);}
	writel(regl & ~SWRESET_HDMI, DDICFG_SWRESET);
	
	// enable DDI_BUS HDMI CLK
	//regl = readl(DDICFG_HDMICTRL);
	//writel(regl | HDMICTRL_HDMI_ENABLE, DDICFG_HDMICTRL);

	// HDMI PHY Reset
	hdmi_phy_reset();

	// HDMI SPDIF Reset
	regl = readl(DDICFG_HDMICTRL);
	writel(regl | HDMICTRL_RESET_SPDIF, DDICFG_HDMICTRL);
	{volatile int ttt;for(ttt=0;ttt<0x5000;ttt++);}
	writel(regl & ~HDMICTRL_RESET_SPDIF, DDICFG_HDMICTRL);

	// HDMI TMDS Reset
	regl = readl(DDICFG_HDMICTRL);
	writel(regl | HDMICTRL_RESET_TMDS, DDICFG_HDMICTRL);
	{volatile int ttt;for(ttt=0;ttt<0x5000;ttt++);}
	writel(regl & ~HDMICTRL_RESET_TMDS, DDICFG_HDMICTRL);

	// swreset DDI_BUS HDMI
	regl = readl(DDICFG_SWRESET);
	writel(regl | SWRESET_HDMI, DDICFG_SWRESET);
	{volatile int ttt;for(ttt=0;ttt<0x100;ttt++);}
	writel(regl & ~SWRESET_HDMI, DDICFG_SWRESET);
	
	// enable DDI_BUS HDMI CLK
	regl = readl(DDICFG_HDMICTRL);
	writel(regl | HDMICTRL_HDMI_ENABLE, DDICFG_HDMICTRL);
			
	{volatile int ttt;for(ttt=0;ttt<0x5000;ttt++);}

    // disable HDCP INT
    reg = readb(HDMI_SS_INTC_CON);
    writeb(reg & ~(1<<HDMI_IRQ_HDCP), HDMI_SS_INTC_CON);
	// disable SPDIF INT
    reg = readb(HDMI_SS_INTC_CON);
    writeb(reg & ~(1<<HDMI_IRQ_SPDIF), HDMI_SS_INTC_CON);
}

/**
 * Set checksum in Audio InfoFrame Packet. @n
 * Calculate a checksum and set it in packet.
 */
void hdmi_aui_update_checksum(void)
{
    unsigned char index, checksum;

    checksum = AUI_HEADER;
    for (index = 0; index < AUI_PACKET_BYTE_LENGTH; index++)
    {
#if 1
        // when write this byte(PB5), HW shift 3 bit to right direction.
        // to compensate it, when read it, SW should shift 3 bit to left.
        if (index == 4)
            checksum += (readb(HDMI_AUI_BYTE1 + 4*index)<<3);
        else
            checksum += readb(HDMI_AUI_BYTE1 + 4*index);
#else
        checksum += readb(HDMI_AUI_BYTE1 + 4*index);
#endif
    }
    writeb(~checksum+1,HDMI_AUI_CHECK_SUM);
}

/**
 * Set checksum in AVI InfoFrame Packet. @n
 * Calculate a checksum and set it in packet.
 */
void hdmi_avi_update_checksum(void)
{
    unsigned char index, checksum;

    checksum = AVI_HEADER;
    for (index = 0; index < AVI_PACKET_BYTE_LENGTH; index++)
    {
        checksum += readb(HDMI_AVI_BYTE1 + 4*index);
    }
    writeb(~checksum+1,HDMI_AVI_CHECK_SUM);
}

/**
 * Set HDMI/DVI mode
 * @param   mode   [in] HDMI/DVI mode
 * @return  If argument is invalid, return 0;Otherwise return 1.
 */
int hdmi_set_hdmi_mode(int mode)
{
	int ret = 1;

	switch(mode)
	{
		case HDMI:
	        writeb(HDMI_MODE_SEL_HDMI,HDMI_MODE_SEL);
	        writeb(HDMICON2_HDMI,HDMI_CON_2);
			break;
		case DVI:
	        writeb(HDMI_MODE_SEL_DVI,HDMI_MODE_SEL);
	        writeb(HDMICON2_DVI,HDMI_CON_2);
			break;
		default:
			ret = 0;
			break;
	}

	return ret;
}

// [2]
void hdmi_set_lcdc_timing(struct device_lcdc_timing_params mode)
{
    volatile unsigned int regl;
    unsigned int val;
	unsigned int lcdc_id;
	unsigned int cnt=0;

#if defined(_TCC9300_)
	PCKC	pCKC = (CKC *)&HwCLK_BASE;
#else
	PCKC	pCKC = (CKC *)HwCKC_BASE;
#endif
	PLCDC	pLCDC;

	int is_lcdc_en = 0;
	
	// check hdmi connect path
	regl = readl(DDICFG_HDMICTRL);
	val = (regl & HDMICTRL_PATH_LCDC1) >> 15;
	if (val)	// lcdc1
	{
		lcdc_id = 1;
	#if defined(_TCC9300_)
		pLCDC = (LCDC *)&HwLCDC1_BASE;
	#else
		pLCDC = (LCDC *)HwLCDC1_BASE;
	#endif

		// disable lcdc
		regl = readl(&pLCDC->LCTRL);
//		writel(regl & ~(0x00000001), &pLCDC->LCTRL);
		if (regl & 0x00000001)
		{
			is_lcdc_en = 1;
			writel(regl & ~(0x00000001), &pLCDC->LCTRL);

			while(pLCDC->LSTATUS & 0x40000000)	// BUSY	Hw30
			{
			//	DPRINTF("LCDC%d-LCTRL[%x]STATUS[%x]\n", lcdc_id, pLCDC->LCTRL, pLCDC->LSTATUS);
				{volatile int ttt;for(ttt=0;ttt<0x5000;ttt++);}//udelay(1000*1);

				if (pLCDC->LCTRL & 0x00000001)
				{
			//		DPRINTF(">>lcdc%d enabled\n", lcdc_id);
					break;
				}

				if (cnt++ == 30)
				{
			//		DPRINTF("lcdc%d busy!>\n", lcdc_id);
					break;
				}
			}
		}
		
		#if defined(_TCC9300_) || defined(_TCC8800_) || defined(_TCC8800ST_)
			regl = readl(&pCKC->PCLK_LCD1);
			writel(regl & ~(Hw29), &pCKC->PCLK_LCD1);	// EN: 0

			// set lcdc1 clock
			regl = readl(&pCKC->PCLK_LCD1);
			regl &= ~(0x1f000fff);
			writel(regl, &pCKC->PCLK_LCD1);	// EN : 0, init.
			regl |= 12*Hw24;				// SEL: HDMI PCLK(Variable)
			regl |= 0;						// DIV: 0
			writel(regl, &pCKC->PCLK_LCD1);
			regl |= Hw29;					// EN : 1
			writel(regl, &pCKC->PCLK_LCD1);
		#else
			regl = readl(&pCKC->PCLK_LCD1);
			writel(regl & ~(Hw28), &pCKC->PCLK_LCD1);	// EN: 0

			// set lcdc1 clock
			regl = readl(&pCKC->PCLK_LCD1);
			regl &= ~(0x0f000fff);
			writel(regl, &pCKC->PCLK_LCD1);	// EN : 0, init.
			regl |= 12*Hw24;				// SEL: HDMI PCLK(Variable)
			regl |= 0;						// DIV: 0
			writel(regl, &pCKC->PCLK_LCD1);
			regl |= Hw28;					// EN : 1
			writel(regl, &pCKC->PCLK_LCD1);
		#endif
	}
	else		// lcdc0
	{
		lcdc_id = 0;
	#if defined(_TCC9300_)
		pLCDC = (LCDC *)&HwLCDC0_BASE;
	#else
		pLCDC = (LCDC *)HwLCDC0_BASE;
	#endif

		// disable lcdc
		regl = readl(&pLCDC->LCTRL);
//		writel(regl & ~(0x00000001), &pLCDC->LCTRL);
		if (regl & 0x00000001)
		{
			is_lcdc_en = 1;
			writel(regl & ~(0x00000001), &pLCDC->LCTRL);

			while(pLCDC->LSTATUS & 0x40000000)	// BUSY	Hw30
			{
			//	DPRINTF("LCDC%d-LCTRL[%x]STATUS[%x]\n", lcdc_id, pLCDC->LCTRL, pLCDC->LSTATUS);
				{volatile int ttt;for(ttt=0;ttt<0x5000;ttt++);}//udelay(1000*1);

				if (pLCDC->LCTRL & 0x00000001)
				{
			//		DPRINTF(">>lcdc%d enabled\n", lcdc_id);
					break;
				}

				if (cnt++ == 30)
				{
			//		DPRINTF("lcdc%d busy!>\n", lcdc_id);
					break;
				}
			}
		}

		#if defined(_TCC9300_) || defined(_TCC8800_) || defined(_TCC8800ST_)
			regl = readl(&pCKC->PCLK_LCD0);
			writel(regl & ~(Hw29), &pCKC->PCLK_LCD0);	// EN: 0

			// set lcdc1 clock
			regl = readl(&pCKC->PCLK_LCD0);
			regl &= ~(0x1f000fff);
			writel(regl, &pCKC->PCLK_LCD0);	// EN : 0, init.
			regl |= 12*Hw24;				// SEL: HDMI PCLK(Variable)
			regl |= 0;						// DIV: 0
			writel(regl, &pCKC->PCLK_LCD0);
			regl |= Hw29;					// EN : 1
			writel(regl, &pCKC->PCLK_LCD0);
		#else
			regl = readl(&pCKC->PCLK_LCD0);
			writel(regl & ~(Hw28), &pCKC->PCLK_LCD0);	// EN: 0

			// set lcdc1 clock
			regl = readl(&pCKC->PCLK_LCD0);
			regl &= ~(0x0f000fff);
			writel(regl, &pCKC->PCLK_LCD0);	// EN : 0, init.
			regl |= 12*Hw24;				// SEL: HDMI PCLK(Variable)
			regl |= 0;						// DIV: 0
			writel(regl, &pCKC->PCLK_LCD0);
			regl |= Hw28;					// EN : 1
			writel(regl, &pCKC->PCLK_LCD0);
		#endif
	}

	// set lctrl
	regl = readl(&pLCDC->LCTRL);
	regl &= ~(0x0000f3e0);
	regl |=((mode.id  & 0x1) <<15)
		 | ((mode.iv  & 0x1) <<14)
		 | ((mode.ih  & 0x1) <<13)
		 | ((mode.ip  & 0x1) <<12)
		 | ((mode.dp  & 0x1) << 9)
		 | ((mode.ni  & 0x1) << 8)
		 | ((mode.tv  & 0x1) << 7)
		 | ((mode.tft & 0x1) << 6)
		 | ((mode.stn & 0x1) << 5);
	regl = (regl & ~(0xf<<16)) | ((12)<<16);	//pxdw: 12 - rgb888
	writel(regl, &pLCDC->LCTRL);

	// set clkdiv
	writel(0x80000000, &pLCDC->LCLKDIV);
	
	// set timing
	writel((mode.lpw  <<16) | (mode.lpc  ), &pLCDC->LHTIME1);
	writel((mode.lswc <<16) | (mode.lewc ), &pLCDC->LHTIME2);
	writel((mode.fpw  <<16) | (mode.flc  ), &pLCDC->LVTIME1);
	writel((mode.fswc <<16) | (mode.fewc ), &pLCDC->LVTIME2);
	writel((mode.fpw2 <<16) | (mode.flc2 ), &pLCDC->LVTIME3);
	writel((mode.fswc2<<16) | (mode.fewc2), &pLCDC->LVTIME4);

	regl = readl(&pLCDC->LVTIME1);
	writel(regl | (mode.vdb  <<27) | (mode.vdf  <<22), &pLCDC->LVTIME1);

	// set lcdc.lds
    regl = ((mode.flc+1)<<16) | ((mode.dp==1)?((mode.lpc+1)/2):(mode.lpc+1));
	writel(regl, &pLCDC->LDS);

	// set lcdc.img dma interlace mode
	if (mode.ni)	// non-interlace mode (progressive mode)
	{
		regl = readl(&pLCDC->LI0C);
		writel(regl & ~(0x80000000), &pLCDC->LI0C);
		if(lcdc_id)
		{
			regl = readl(&pLCDC->LI1C);
			writel(regl & ~(0x80000000), &pLCDC->LI1C);
			regl = readl(&pLCDC->LI2C);
			writel(regl & ~(0x80000000), &pLCDC->LI2C);
		}
	}
	else			// interlace mode
	{
		regl = readl(&pLCDC->LI0C);
		writel(regl | (0x80000000), &pLCDC->LI0C);
		if(lcdc_id)
		{
			regl = readl(&pLCDC->LI1C);
			writel(regl | (0x80000000), &pLCDC->LI1C);
			regl = readl(&pLCDC->LI2C);
			writel(regl | (0x80000000), &pLCDC->LI2C);
		}
	}

	// enable lcdc
	if (is_lcdc_en)
	{
		regl = readl(&pLCDC->LCTRL);
		writel(regl | 0x00000001, &pLCDC->LCTRL);
	}

    return;
}

// [2] A
int hdmi_set_lcdc_config(enum VideoFormat hdmi_video_format)
{
    struct device_lcdc_timing_params device;

    // set lcdc
    memcpy((void*)&device,(const void*)&(LCDCTimimgParams[hdmi_video_format]),sizeof(device));

    // set video parameters
	hdmi_set_lcdc_timing(device);

    return 1;
}



/**
 * Set pixel aspect ratio information in AVI InfoFrame
 * @param   ratio   [in] Pixel Aspect Ratio
 * @return  If argument is invalid, return 0;Otherwise return 1.
 */
int hdmi_set_pixel_aspect_ratio(enum PixelAspectRatio ratio)
{
    int ret = 1;
    unsigned char reg = AVI_FORMAT_ASPECT_AS_PICTURE;

    switch (ratio)
    {
        case HDMI_PIXEL_RATIO_16_9:
            reg |= AVI_PICTURE_ASPECT_RATIO_16_9;
            break;
        case HDMI_PIXEL_RATIO_4_3:
            reg |= AVI_PICTURE_ASPECT_RATIO_4_3;
            break;
        default:
            ret = 0;
     }
    writeb(reg,HDMI_AVI_BYTE2);
    return ret;
}


/**
 * Set video timing parameters.@n
 * @param   mode   [in] Video timing parameters
 */
void hdmi_set_video_timing(struct device_video_params mode)
{
    unsigned char reg;
    unsigned int  val;

    // set HBLANK;
    val = mode.HBlank;
    reg = val & 0xff;
    writeb(reg,HDMI_H_BLANK_0);
    reg = (val>>8) & 0xff;
    writeb(reg,HDMI_H_BLANK_1);

    // set VBlank
    val = mode.VBlank;
    reg = val & 0xff;
    writeb(reg, HDMI_V_BLANK_0);
    reg = (val>>8) & 0xff;
    writeb(reg, HDMI_V_BLANK_1);
    reg = (val>>16) & 0xff;
    writeb(reg, HDMI_V_BLANK_2);

    // set HVLine
    val = mode.HVLine;
    reg = val & 0xff;
    writeb(reg, HDMI_H_V_LINE_0);
    reg = (val>>8) & 0xff;
    writeb(reg, HDMI_H_V_LINE_1);
    reg = (val>>16) & 0xff;
    writeb(reg, HDMI_H_V_LINE_2);

    // set VSync Polarity
    writeb(mode.polarity, HDMI_VSYNC_POL);

    // set HSyncGen
    val = mode.HSYNCGEN;
    reg = val & 0xff;
    writeb(reg, HDMI_H_SYNC_GEN_0);
    reg = (val>>8) & 0xff;
    writeb(reg, HDMI_H_SYNC_GEN_1);
    reg = (val>>16) & 0xff;
    writeb(reg, HDMI_H_SYNC_GEN_2);

    // set VSyncGen1
    val = mode.VSYNCGEN;
    reg = val & 0xff;
    writeb(reg, HDMI_V_SYNC_GEN1_0);
    reg = (val>>8) & 0xff;
    writeb(reg, HDMI_V_SYNC_GEN1_1);
    reg = (val>>16) & 0xff;
    writeb(reg, HDMI_V_SYNC_GEN1_2);

    // set interlace or progresive mode
    writeb(mode.interlaced,HDMI_INT_PRO_MODE);

    if ( mode.interlaced ) // interlaced mode
    {
        // set VBlank_F
        val = mode.VBLANK_F;
        reg = val & 0xff;
        writeb(reg, HDMI_V_BLANK_F_0);
        reg = (val>>8) & 0xff;
        writeb(reg, HDMI_V_BLANK_F_1);
        reg = (val>>16) & 0xff;
        writeb(reg, HDMI_V_BLANK_F_2);

        // set VSyncGen2
        val = mode.VSYNCGEN2;
        reg = val & 0xff;
        writeb(reg, HDMI_V_SYNC_GEN2_0);
        reg = (val>>8) & 0xff;
        writeb(reg, HDMI_V_SYNC_GEN2_1);
        reg = (val>>16) & 0xff;
        writeb(reg, HDMI_V_SYNC_GEN2_2);

        // set VSyncGen3
        val = mode.VSYNCGEN3;
        reg = val & 0xff;
        writeb(reg, HDMI_V_SYNC_GEN3_0);
        reg = (val>>8) & 0xff;
        writeb(reg, HDMI_V_SYNC_GEN3_1);
        reg = (val>>16) & 0xff;
        writeb(reg, HDMI_V_SYNC_GEN3_2);
    }
    else
    {
        // set VBlank_F with default value
        writeb(0x00, HDMI_V_BLANK_F_0);
        writeb(0x00, HDMI_V_BLANK_F_1);
        writeb(0x00, HDMI_V_BLANK_F_2);

        // set VSyncGen2 with default value
        writeb(0x01, HDMI_V_SYNC_GEN2_0);
        writeb(0x10, HDMI_V_SYNC_GEN2_1);
        writeb(0x00, HDMI_V_SYNC_GEN2_2);

        // set VSyncGen3 with default value
        writeb(0x01, HDMI_V_SYNC_GEN3_0);
        writeb(0x10, HDMI_V_SYNC_GEN3_1);
        writeb(0x00, HDMI_V_SYNC_GEN3_2);
    }

    // set pixel repetition
    reg = readb(HDMI_CON_1);
    if ( mode.repetition )
    {
        // set pixel repetition
        writeb(reg|HDMICON1_DOUBLE_PIXEL_REPETITION,HDMI_CON_1);
        // set avi packet
        writeb(AVI_PIXEL_REPETITION_DOUBLE,HDMI_AVI_BYTE5);
    }
    else
    {
        // clear pixel repetition
        writeb(reg & ~(1<<1|1<<0),HDMI_CON_1);
        // set avi packet
        writeb(0x00,HDMI_AVI_BYTE5);
    }

    // set AVI packet with VIC
	reg = readb(HDMI_AVI_BYTE2);

	if (reg & (unsigned char)AVI_PICTURE_ASPECT_RATIO_4_3)
		writeb(mode.AVI_VIC,HDMI_AVI_BYTE4);
	else
		writeb(mode.AVI_VIC_16_9,HDMI_AVI_BYTE4);

    return;
}

/**
 * Set color space in HDMI H/W. @n
 * @param   space   [in] Color space
 * @return  If argument is invalid, return 0;Otherwise return 1.
 */
int hdmi_set_color_space(enum ColorSpace space)
{
    unsigned char reg,aviYY;
    int ret = 1;

    reg = readb(HDMI_CON_0);
    aviYY = readb(HDMI_AVI_BYTE1);
    // clear fields
    writeb(aviYY & ~(AVI_CS_Y422|AVI_CS_Y444),HDMI_AVI_BYTE1);
	aviYY = readb(HDMI_AVI_BYTE1);

    if (space == HDMI_CS_YCBCR422)
    {
        // set video input interface
        writeb( reg | HDMI_YCBCR422_ENABLE, HDMI_CON_0);
        // set avi
        writeb( aviYY | AVI_CS_Y422, HDMI_AVI_BYTE1);
    }
    else
    {
        // set video input interface
        writeb( reg & ~HDMI_YCBCR422_ENABLE, HDMI_CON_0);
        if (space == HDMI_CS_YCBCR444)
        {
            // set AVI packet
            writeb( aviYY | AVI_CS_Y444, HDMI_AVI_BYTE1);
        }
        // aviYY for RGB = 0, nothing to set
        else if (space != HDMI_CS_RGB)
        {
            ret = 0;
        }
    }

    return ret;
}


/**
 * Set color depth.@n
 * @param   depth   [in] Color depth of input vieo stream
 * @return  If argument is invalid, return 0;Otherwise return 1.
 */
int hdmi_set_color_depth(enum ColorDepth depth)
{
    int ret = 1;
    switch (depth)
    {
        case HDMI_CD_36:
        {
            // set GCP CD
            writeb(GCP_CD_36BPP,HDMI_GCP_BYTE2);
            // set DC_CTRL
            writeb(HDMI_DC_CTL_12,HDMI_DC_CONTROL);
            break;
        }
        case HDMI_CD_30:
        {
            // set GCP CD
            writeb(GCP_CD_30BPP,HDMI_GCP_BYTE2);
            // set DC_CTRL
            writeb(HDMI_DC_CTL_10,HDMI_DC_CONTROL);
            break;
        }
        case HDMI_CD_24:
        {
            // set GCP CD
            writeb(GCP_CD_24BPP,HDMI_GCP_BYTE2);
            // set DC_CTRL
            writeb(HDMI_DC_CTL_8,HDMI_DC_CONTROL);
            break;
        }

        default:
        {
            ret = 0;
        }
    }
    return ret;
}

/**
 * Set pixel limitation.
 * @param   limit   [in] Pixel limitation.
* @return  If argument is invalid, return 0;Otherwise return 1.
 */
int hdmi_set_pixel_limit(enum PixelLimit limit)
{
    int ret = 1;
    unsigned char reg,aviQQ;

    // clear field
    reg = readb(HDMI_CON_1);
    reg &= ~HDMICON1_LIMIT_MASK;

    aviQQ = readb(HDMI_AVI_BYTE3);
    aviQQ &= ~AVI_QUANTIZATION_MASK;

    switch (limit) // full
    {
        case HDMI_FULL_RANGE:
        {
            aviQQ |= AVI_QUANTIZATION_FULL;
            break;
        }
        case HDMI_RGB_LIMIT_RANGE:
        {
            reg |= HDMICON1_RGB_LIMIT;
            aviQQ |= AVI_QUANTIZATION_LIMITED;
            break;
        }
        case HDMI_YCBCR_LIMIT_RANGE:
        {
            reg |= HDMICON1_YCBCR_LIMIT;
            aviQQ |= AVI_QUANTIZATION_LIMITED;
            break;
        }
        default:
        {
            ret = 0;
        }
    }
    // set pixel repetition
    writeb(reg,HDMI_CON_1);
    // set avi packet body
    writeb(aviQQ,HDMI_AVI_BYTE3);

    return ret;
}

/**
 * Configurate PHY.
 * @param   hdmi_video_mode [in]    Video mode to set
 * @return  If success, return 1; Otherwise, return 0.
 */
int hdmi_set_phy_config(const struct HDMIVideoParameter * const hdmi_video_mode)
{
	unsigned char reg;
	unsigned int regl;
    unsigned char phy_ready = 0;
    int i = 0;

	// Check HDMI PHY.
	phy_ready = readb(HDMI_PHY_STATUS);
	
	if(!phy_ready)
	{
		DPRINTF("phy is not ready! reset\n");
		hdmi_phy_reset();
 	}

    if (!PHYConfig(HDMIVideoParams[hdmi_video_mode->resolution].PixelClock,
          hdmi_video_mode->colorDepth))
    {
        DPRINTF("phy config failed!\n");
        return 0;
    }

    do
    {
		phy_ready = readb(HDMI_PHY_STATUS);
        if (i++ == 1000) break;
    } while (!phy_ready);

    if (!phy_ready)
    {
        DPRINTF("phy is not ready!!!\n");
    }
    else
    {
        DPRINTF("phy configured\n");
    }

    return 1;
}

/**
 * Set requested video mode.
 * @param   hdmi_video_mode [in]   requested video mode to set
 * @return  If success, return 1; Otherwise, return 0
 */
int hdmi_set_video_mode(const struct HDMIVideoParameter * const hdmi_video_mode)
{
    struct device_video_params device;
	enum PixelLimit pxl_lmt = HDMI_FULL_RANGE;

    if (!hdmi_video_mode)
    {
        DPRINTF("bad args: hdmi_video_mode\n");
        return 0;
    }

    // parsing video parameters
    memcpy((void*)&device,(const void*)&(HDMIVideoParams[hdmi_video_mode->resolution]),sizeof(device));

	// set lcdc
	hdmi_set_lcdc_config(hdmi_video_mode->resolution);

    // set pixel aspect ratio
    // !! must be setting before 'HDMI_IOC_SET_VIDEOMODE'
    hdmi_set_pixel_aspect_ratio(hdmi_video_mode->pixelAspectRatio);

    // set video parameters
    hdmi_set_video_timing(device);

    // set video format information
	//gHdmiVideoParms.resolution = hdmi_video_mode->resolution;

    // set color space
    if ( !hdmi_set_color_space(hdmi_video_mode->colorSpace) )
    {
        DPRINTF("bad args: hdmi_video_mode->colorSpace : Not Correct Arg = %d\n", hdmi_video_mode->colorSpace);
        //return -EFAULT;
    }
	//gHdmiVideoParms.colorSpace = hdmi_video_mode->colorSpace;

    // set color depth
    if ( !hdmi_set_color_depth(hdmi_video_mode->colorDepth) )
    {
        DPRINTF("bad args: hdmi_video_mode->colorDepth : Not Correct Arg = %d\n", hdmi_video_mode->colorDepth);
        //return -EFAULT;
    }
	//gHdmiVideoParms.colorDepth = hdmi_video_mode->colorDepth;

	// set pixel limitation.
	switch(hdmi_video_mode->colorSpace) 
	{
		case HDMI_CS_RGB:		/** RGB color space */
			if (hdmi_video_mode->resolution == v640x480p_60Hz)
				pxl_lmt = HDMI_FULL_RANGE;
			else
				pxl_lmt = HDMI_RGB_LIMIT_RANGE;
			break;
		case HDMI_CS_YCBCR444:	/** YCbCr 4:4:4 color space */
		case HDMI_CS_YCBCR422:	/** YCbCr 4:2:2 color space */
			pxl_lmt = HDMI_YCBCR_LIMIT_RANGE;
			break;
	}

    if (!hdmi_set_pixel_limit(pxl_lmt))
    {
		DPRINTF("bad args: hdmi_video_mode->colorDepth : Not Correct Arg = %d\n", pxl_lmt);
        //return -EFAULT;
    }
	//gPixelLimit = pxl_lmt;

    // set phy
    if (!hdmi_set_phy_config(hdmi_video_mode))
    {
        DPRINTF("fail to config PHY!\n");
        //return 0;
    }

    return 1;
}


/**
 * Set audio input port.
 *
 * @param   port    [in]    Audio input port.
 * @return  If argument is invalid, return 0;Otherwise, return 1.
 */

//int setAudioInputPort(enum HDMIAudioPort port)
int hdmi_set_audio_inputport(enum HDMIAudioPort port)
{
#if (0) && defined(TELECHIPS)
	// just test... 32fs..
	unsigned int regl;

	regl = readl(0xf0534040);	//DAMR
	if((regl & 0xf0) != 0x80)
	{
		regl = (regl & ~0xf0) | 0x80;
		writel(regl, 0xf0534040);
	}
#endif

    switch (port)
    {
        case I2S_PORT:
        {

			//@STORM::090928
		    // disable SPDIF INT
			{
			    unsigned char reg;
			    reg = readb(HDMI_SS_INTC_CON);
			    writeb(reg & ~(1<<HDMI_IRQ_SPDIF), HDMI_SS_INTC_CON);
			}
			
            // enable audio
            //writeb(0, HDMI_SS_SPDIF_CLK_CTRL); // enable clock???
            writeb(I2S_CLK_CON_ENABLE,HDMI_SS_I2S_CLK_CON);

            // disable DSD
            writeb(I2S_DSD_CON_DISABLE, HDMI_SS_I2S_DSD_CON);

#if (0) && defined(TELECHIPS)
            // pin select for I2S
            writeb(I2S_PIN_SEL_AUDIO_1 | (I2S_PIN_SEL_AUDIO_0 << 4), HDMI_SS_I2S_PIN_SEL_0);
            writeb(I2S_PIN_SEL_AUDIO_2 | (I2S_PIN_SEL_AUDIO_3 << 4), HDMI_SS_I2S_PIN_SEL_1);
            writeb(I2S_PIN_SEL_AUDIO_4 | (I2S_PIN_SEL_AUDIO_5 << 4), HDMI_SS_I2S_PIN_SEL_2);
            writeb(I2S_PIN_SEL_AUDIO_6, HDMI_SS_I2S_PIN_SEL_3);
#endif

            // I2S control
            writeb(I2S_CON_SC_POL_FALLING | I2S_CON_CH_POL_LOW, HDMI_SS_I2S_CON_1);

            // I2S MUX Control
            writeb(I2S_IN_MUX_ENABLE | I2S_IN_MUX_CUV_ENABLE | I2S_IN_MUX_SELECT_I2S | I2S_IN_MUX_IN_ENABLE, HDMI_SS_I2S_IN_MUX_CON);

            // enable all channels
            writeb(I2S_MUX_CH_ALL_ENABLE , HDMI_SS_I2S_MUX_CH);

            // enable CUV from right and left channel
            writeb(I2S_MUX_CUV_LEFT_ENABLE| I2S_MUX_CUV_RIGHT_ENABLE , HDMI_SS_I2S_MUX_CUV);

            break;
        }
        case SPDIF_PORT:
        {
			#if 0
			
            unsigned char reg;

            // enable audio
            writeb(I2S_CLK_CON_ENABLE, HDMI_SS_I2S_CLK_CON);

            // set mux control
            writeb(I2S_IN_MUX_SELECT_SPDIF | I2S_IN_MUX_ENABLE, HDMI_SS_I2S_IN_MUX_CON);

            // enable all channels
            writeb(I2S_MUX_CH_ALL_ENABLE , HDMI_SS_I2S_MUX_CH);

            // enable CUV from right and left channel
            writeb(I2S_MUX_CUV_LEFT_ENABLE| I2S_MUX_CUV_RIGHT_ENABLE , HDMI_SS_I2S_MUX_CUV);

            writeb(0, HDMI_SS_SPDIF_CLK_CTRL); // enable clock???
            writeb(SPDIF_CLK_CTRL_ENABLE, HDMI_SS_SPDIF_CLK_CTRL);

            writeb(SPDIF_CONFIG_1_NOISE_FILTER_3_SAMPLES | SPDIF_CONFIG_1_UVCP_ENABLE | SPDIF_CONFIG_1_WORD_LENGTH_MANUAL | SPDIF_CONFIG_1_ALIGN_32BIT | SPDIF_CONFIG_1_HDMI_2_BURST, HDMI_SS_SPDIF_CONFIG_1);

            // max 24bits
            writeb(0x0b, HDMI_SS_SPDIF_USER_VALUE_1);

            if (spdif_struct.codingtype == SPDIF_NLPCM)
            {
                writeb(0xFF,HDMI_SS_SPDIF_IRQ_MASK); // enable all
            }
            else
            {
                writeb(SPDIF_BUFFER_OVERFLOW_MASK | SPDIF_PREAMBLE_DETECTED_MASK | SPDIF_STATUS_RECOVERED_MASK | SPDIF_CLK_RECOVERY_FAIL_MASK, HDMI_SS_SPDIF_IRQ_MASK);
            }
            // enable SPDIF INT
            reg = readb(HDMI_SS_INTC_CON);
            writeb(reg | (1<<HDMI_IRQ_SPDIF) | (1<<HDMI_IRQ_GLOBAL), HDMI_SS_INTC_CON);

            // start to detect signal
            writeb(SPDIF_SIGNAL_DETECT, HDMI_SS_SPDIF_OP_CTRL);
			
			#endif
			
            break;
        }
//TODO: need to check!!!
        case DSD_PORT:
        {
			#if 0
			
            // enable all channels
            writeb(I2S_MUX_CH_0_LEFT_ENABLE | I2S_MUX_CH_0_RIGHT_ENABLE |
                    I2S_MUX_CH_1_LEFT_ENABLE | I2S_MUX_CH_1_RIGHT_ENABLE |
                    I2S_MUX_CH_2_LEFT_ENABLE | I2S_MUX_CH_2_RIGHT_ENABLE , HDMI_SS_I2S_MUX_CH);

            // enable CUV from right and left channel
            writeb(~(I2S_MUX_CUV_LEFT_ENABLE| I2S_MUX_CUV_RIGHT_ENABLE) , HDMI_SS_I2S_MUX_CUV);


            // set mux control
            writeb(I2S_IN_MUX_ENABLE | I2S_IN_MUX_SELECT_DSD | I2S_IN_MUX_IN_ENABLE, HDMI_SS_I2S_IN_MUX_CON);

			#endif

            break;
        }
        default:
            return 0;
    }
    return 1;
}

/**
 * Set sampling frequency in I2S receiver.
 *
 * @param   freq    [in]   Sampling frequency.
 * @return  If argument is invalid, return 0;Otherwise, return 1.
 */
//int setCUVSampleFreq(enum SamplingFreq freq)
int hdmi_set_audio_cuv_samplefreq(enum SamplingFreq freq)
{
    int ret = 1;
    unsigned char reg = readb(HDMI_SS_I2S_CH_ST_3) & ~I2S_CH_ST_3_SF_MASK;

    switch (freq)
    {
        case SF_32KHZ:
            reg |= I2S_CH_ST_3_SF_32KHZ;
		//	tea_i2s_setclock(32000);
		//	tea_spdif_setclock(32000);
            break;
        case SF_44KHZ:
            reg |= I2S_CH_ST_3_SF_44KHZ;
		//	tea_i2s_setclock(44100);
		//	tea_spdif_setclock(44100);
            break;
        case SF_88KHZ:
            reg |= I2S_CH_ST_3_SF_88KHZ;
		//	tea_i2s_setclock(88200);
		//	tea_spdif_setclock(88200);
            break;
        case SF_176KHZ:
            reg |= I2S_CH_ST_3_SF_176KHZ;
		//	tea_i2s_setclock(176000);
		//	tea_spdif_setclock(176000);
            break;
        case SF_48KHZ:
            reg |= I2S_CH_ST_3_SF_48KHZ;
		//	tea_i2s_setclock(48000);
		//	tea_spdif_setclock(48000);
            break;
        case SF_96KHZ:
            reg |= I2S_CH_ST_3_SF_96KHZ;
		//	tea_i2s_setclock(96000);
		//	tea_spdif_setclock(96000);
            break;
        case SF_192KHZ:
            reg |= I2S_CH_ST_3_SF_192KHZ;
		//	tea_i2s_setclock(192000);
		//	tea_spdif_setclock(192000);
            break;
        default:
            ret = 0;
    }

    writeb(reg, HDMI_SS_I2S_CH_ST_3);

    return ret;
}

/**
 * Set coding type in I2S receiver.
 *
 * @param   coding    [in]   Coding type.
 * @return  If argument is invalid, return 0;Otherwise, return 1.
 */
//int setCUVCodingType(enum CUVAudioCoding coding)
int hdmi_set_audio_cuv_codingtype(enum CUVAudioCoding coding)
{
    int ret = 1;
    unsigned char reg = readb(HDMI_SS_I2S_CH_ST_0) & ~I2S_CH_ST_0_TYPE_MASK;

    switch (coding)
    {
        case CUV_LPCM:
            reg |= I2S_CH_ST_0_TYPE_LPCM;
            break;

        case CUV_NLPCM:
            reg |= I2S_CH_ST_0_TYPE_NLPCM;
            break;

        default:
            ret = 0;
    };

    writeb(reg, HDMI_SS_I2S_CH_ST_0);

    return ret;
}

/**
 * Set the number of channels in I2S receiver.
 *
 * @param   num     [in]   Number of channels.
 * @return  If argument is invalid, return 0;Otherwise, return 1.
 */
//int setCUVChannelNum(enum CUVChannelNumber num)
int hdmi_set_audio_cuv_channelnum(enum CUVChannelNumber num)
{
    int ret = 1;
    unsigned char reg = readb(HDMI_SS_I2S_CH_ST_2) & ~I2S_CH_ST_2_CHANNEL_MASK;

    switch (num)
    {
        case CUV_CH_UNDEFINED:
            reg |= I2S_CH_ST_2_CH_UNDEFINED;
            break;

        case CUV_CH_01:
            reg |= I2S_CH_ST_2_CH_01;
            break;

        case CUV_CH_02:
            reg |= I2S_CH_ST_2_CH_02;
            break;

        case CUV_CH_03:
            reg |= I2S_CH_ST_2_CH_03;
            break;

        case CUV_CH_04:
            reg |= I2S_CH_ST_2_CH_04;
            break;

        case CUV_CH_05:
            reg |= I2S_CH_ST_2_CH_05;
            break;

        case CUV_CH_06:
            reg |= I2S_CH_ST_2_CH_06;
            break;

        case CUV_CH_07:
            reg |= I2S_CH_ST_2_CH_07;
            break;

        case CUV_CH_08:
            reg |= I2S_CH_ST_2_CH_08;
            break;

        case CUV_CH_09:
            reg |= I2S_CH_ST_2_CH_09;
            break;

        case CUV_CH_10:
            reg |= I2S_CH_ST_2_CH_10;
            break;

        case CUV_CH_11:
            reg |= I2S_CH_ST_2_CH_11;
            break;

        case CUV_CH_12:
            reg |= I2S_CH_ST_2_CH_12;
            break;

        case CUV_CH_13:
            reg |= I2S_CH_ST_2_CH_13;
            break;

        case CUV_CH_14:
            reg |= I2S_CH_ST_2_CH_14;
            break;

        case CUV_CH_15:
            reg |= I2S_CH_ST_2_CH_15;
            break;

        default:
            ret = 0;
    }

    writeb(reg, HDMI_SS_I2S_CH_ST_2);

    return ret;
}

/**
 * Set word length in I2S receiver.
 *
 * @param   length    [in]   Word length.
 * @return  If argument is invalid, return 0;Otherwise, return 1.
 */
//int setCUVWordLength(enum CUVWordLength length)
int hdmi_set_audio_cuv_wordlength(enum CUVWordLength length)
{
    int ret = 1;
    unsigned char reg = readb(HDMI_SS_I2S_CH_ST_4) & ~I2S_CH_ST_4_WL_MASK;

    switch (length)
    {
        case CUV_WL_20_NOT_DEFINED:
            reg |= I2S_CH_ST_4_WL_20_NOT_DEFINED;
            break;

        case CUV_WL_20_16:
            reg |= I2S_CH_ST_4_WL_20_16;
            break;

        case CUV_WL_20_18:
            reg |= I2S_CH_ST_4_WL_20_18;
            break;

        case CUV_WL_20_19:
            reg |= I2S_CH_ST_4_WL_20_19;
            break;

        case CUV_WL_20_20:
            reg |= I2S_CH_ST_4_WL_20_20;
            break;

        case CUV_WL_20_17:
            reg |= I2S_CH_ST_4_WL_20_17;
            break;

        case CUV_WL_24_NOT_DEFINED:
            reg |= I2S_CH_ST_4_WL_24_NOT_DEFINED;
            break;

        case CUV_WL_24_20:
            reg |= I2S_CH_ST_4_WL_24_20;
            break;

        case CUV_WL_24_22:
            reg |= I2S_CH_ST_4_WL_24_22;
            break;

        case CUV_WL_24_23:
            reg |= I2S_CH_ST_4_WL_24_23;
            break;

        case CUV_WL_24_24:
            reg |= I2S_CH_ST_4_WL_24_24;
            break;

        case CUV_WL_24_21:
            reg |= I2S_CH_ST_4_WL_24_21;
            break;

        default:
            ret = 0;
    }
    writeb(reg, HDMI_SS_I2S_CH_ST_4);

    return ret;
}

/**
 * Set I2S audio paramters in I2S receiver.
 *
 * @param   i2s     [in]   I2S audio paramters.
 * @return  If argument is invalid, return 0;Otherwise, return 1.
 */
//int setI2SParameter(struct I2SParameter i2s)
int hdmi_set_audio_i2s_parameter(struct I2SParameter i2s)
{
    unsigned char reg;
    // bit per channel
    switch(i2s.bpc)
    {
        case I2S_BPC_16:
            reg = I2S_CON_DATA_NUM_16;
            break;

        case I2S_BPC_20:
            reg = I2S_CON_DATA_NUM_20;
            break;

        case I2S_BPC_24:
            reg = I2S_CON_DATA_NUM_24;
            break;
        default:
            return 0;
    }

    // LR clock
    switch(i2s.clk)
    {
        case I2S_32FS:
            reg = (I2S_CON_BIT_CH_32 | I2S_CON_DATA_NUM_16);
            break;

        case I2S_48FS:
            reg |= I2S_CON_BIT_CH_48;
            break;

        case I2S_64FS:
            reg |= I2S_CON_BIT_CH_64;
            break;
        default:
            return 0;
    }

    // format
    switch(i2s.format)
    {
        case I2S_BASIC:
            reg |= I2S_CON_I2S_MODE_BASIC;
            break;

        case I2S_LEFT_JUSTIFIED:
            reg |= I2S_CON_I2S_MODE_LEFT_JUSTIFIED;
            break;

        case I2S_RIGHT_JUSTIFIED:
            reg |= I2S_CON_I2S_MODE_RIGHT_JUSTIFIED;
            break;
        default:
            return -1;
    }
    writeb(reg , HDMI_SS_I2S_CON_2);

    return 1;

}


/**
 * Set Audio Clock Recovery and Audio Infoframe packet -@n
 * based on sampling frequency.
 * @param   freq   [in] Sampling frequency
 * @return  If argument is invalid, return 0;Otherwise return 1.
 */
int hdmi_set_audio_sample_freq(enum SamplingFreq freq)
{
    unsigned char reg;
    unsigned int n;
    int ret = 1;

    // check param
    if ( freq > sizeof(ACR_N_params)/sizeof(unsigned int) || freq < 0 )
        return 0;

    // set ACR packet
    // set N value
    n = ACR_N_params[freq];
    reg = n & 0xff;
    writeb(reg,HDMI_ACR_N0);
    reg = (n>>8) & 0xff;
    writeb(reg,HDMI_ACR_N1);
    reg = (n>>16) & 0xff;
    writeb(reg,HDMI_ACR_N2);

    // set as measure cts mode
    writeb(ACR_MEASURED_CTS_MODE,HDMI_ACR_CON);

    // set AUI packet
    reg = readb(HDMI_AUI_BYTE2) & ~HDMI_AUI_SF_MASK;

    switch (freq)
    {
        case SF_32KHZ:
            reg |= HDMI_AUI_SF_SF_32KHZ;
            break;

        case SF_44KHZ:
            reg |= HDMI_AUI_SF_SF_44KHZ;
            break;

        case SF_88KHZ:
            reg |= HDMI_AUI_SF_SF_88KHZ;
            break;

        case SF_176KHZ:
            reg |= HDMI_AUI_SF_SF_176KHZ;
            break;

        case SF_48KHZ:
            reg |= HDMI_AUI_SF_SF_48KHZ;
            break;

        case SF_96KHZ:
            reg |= HDMI_AUI_SF_SF_96KHZ;
            break;

        case SF_192KHZ:
            reg |= HDMI_AUI_SF_SF_192KHZ;
            break;

        default:
            ret = 0;
    }

    writeb(reg, HDMI_AUI_BYTE2);

    return ret;
}

/**
 * Set HDMI audio output packet type.
 * @param   packet   [in] Audio packet type
 * @return  If argument is invalid, return 0;Otherwise return 1.
 */
int hdmi_set_audio_packet_type(enum HDMIASPType packet)
{
    int ret = 1;
    unsigned char reg;

    reg = readb(HDMI_ASP_CON);
    reg &= ~ASP_TYPE_MASK;

    switch (packet)
    {
        case HDMI_ASP:
        {
            reg |= ASP_LPCM_TYPE;
            break;
        }
        case HDMI_DSD:
        {
            reg |= ASP_DSD_TYPE;
            break;
        }
        case HDMI_HBR:
        {
            reg |= ASP_HBR_TYPE;
            break;
        }
        case HDMI_DST:
        {
            reg |= ASP_DST_TYPE;
            break;
        }
        default:
            ret = 0;
    }
    writeb(reg,HDMI_ASP_CON);
    return ret;
}

/**
 * Set layout and sample present fields in Audio Sample Packet -@n
 * and channel number field in Audio InfoFrame packet.
 * @param   channel   [in]  Number of channels
 * @return  If argument is invalid, return 0;Otherwise return 1.
 */
int hdmi_set_audio_channel_number(enum ChannelNum channel)
{
    int ret = 1;
    unsigned char reg;

    reg = readb(HDMI_ASP_CON);
    // clear field
    reg &= ~(ASP_MODE_MASK|ASP_SP_MASK);

    // set layout & SP_PRESENT on ASP_CON
    // set AUI Packet
    switch (channel)
    {
        case CH_2:
            reg |= (ASP_LAYOUT_0|ASP_SP_0);
            writeb(AUI_CC_2CH,HDMI_AUI_BYTE1);
            break;
        case CH_3:
            reg |= (ASP_LAYOUT_1|ASP_SP_0|ASP_SP_1);
            writeb(AUI_CC_3CH,HDMI_AUI_BYTE1);
            break;
        case CH_4:
            reg |= (ASP_LAYOUT_1|ASP_SP_0|ASP_SP_1);
            writeb(AUI_CC_4CH,HDMI_AUI_BYTE1);
            break;
        case CH_5:
            reg |= (ASP_LAYOUT_1|ASP_SP_0|ASP_SP_1|ASP_SP_2);
            writeb(AUI_CC_5CH,HDMI_AUI_BYTE1);
            break;
        case CH_6:
            reg |= (ASP_LAYOUT_1|ASP_SP_0|ASP_SP_1|ASP_SP_2);
            writeb(AUI_CC_6CH,HDMI_AUI_BYTE1);
            break;
        case CH_7:
            reg |= (ASP_LAYOUT_1|ASP_SP_0|ASP_SP_1|ASP_SP_2|ASP_SP_3);
            writeb(AUI_CC_7CH,HDMI_AUI_BYTE1);
            break;
        case CH_8:
            reg |= (ASP_LAYOUT_1|ASP_SP_0|ASP_SP_1|ASP_SP_2|ASP_SP_3);
            writeb(AUI_CC_8CH,HDMI_AUI_BYTE1);
            break;
        default:
            ret = 0;
    }
    writeb(reg,HDMI_ASP_CON);
    return ret;
}

/**
 * Set requested audio mode.
 * @param   hdmi_audio_mode [in]    Audio mode to set
 * @return If success, return 1;Otherwise, return 0
 */
int hdmi_set_audio_mode(const struct HDMIAudioParameter * const hdmi_audio_mode)
{
//	unsigned char val;
	unsigned char reg;
//	unsigned int vall;
//	unsigned int regl;
	
    // check paramter
    if (!hdmi_audio_mode)
    {
        DPRINTF("bad args: hdmi_audio_mode\n");
        return 0;
    }


	// [I2S_CLK_CON.i2s_en]
	//  You must set i2s_en, after other registers are configured. 
	//  when you want to reset the i2s, this register is 0 1. 
	// disable audio
	writeb(I2S_CLK_CON_DISABLE,HDMI_SS_I2S_CLK_CON);
	
    // setting audio input port parameters
    switch (hdmi_audio_mode->inputPort)
    {
        int wordlength,codingtype;

        case I2S_PORT:
        {
            // set CUV
            // sample freq
            #if (0)
            if (ioctl(audio_fd,AUDIO_IOC_SET_I2S_CUV_SET_SAMPLEFREQ,&hdmi_audio_mode->sampleFreq))
            {
                DPRINTF("error to set sample freq on I2S CUV\n");
                return 0;
            }
			#else
            if (!hdmi_set_audio_cuv_samplefreq(hdmi_audio_mode->sampleFreq))
            {
                DPRINTF("(hdmi_audio_mode->sampleFreq) Not Available Arg\n");
                return 0;
            }
			//gHDMIAudioParams.sampleFreq = val;
			#endif

            // channel number
            #if (0)
            if (ioctl(audio_fd,AUDIO_IOC_SET_I2S_CUV_SET_CHANNELNUMBER,&hdmi_audio_mode->channelNum))
            {
                DPRINTF("error to set channel number on I2S CUV\n");
                return 0;
            }
			#else
            if (!hdmi_set_audio_cuv_channelnum(hdmi_audio_mode->channelNum))
            {
                DPRINTF("(hdmi_audio_mode->channelNum) Not Available Arg\n");
                return 0;
            }
			//gHDMIAudioParams.channelNum = val;
			#endif

            if (hdmi_audio_mode->formatCode == LPCM_FORMAT)
            {
                codingtype = CUV_LPCM;
            }
            else
            {
                codingtype = CUV_NLPCM;
            }

			#if (0)
            if (ioctl(audio_fd,AUDIO_IOC_SET_I2S_CUV_SET_CODINGTYPE, &codingtype))
            {
                DPRINTF("error to set coding type on I2S CUV\n");
                return 0;
            }
			#else
            if (!hdmi_set_audio_cuv_codingtype(codingtype))
            {
                DPRINTF("(codingtype) Not Available Arg\n");
                return 0;
            }
			//gCodingType = val;
			#endif

			#if (0)
            if (ioctl(audio_fd,AUDIO_IOC_SET_AUDIOFORMATCODE_INFO, &hdmi_audio_mode->formatCode))
            {
                DPRINTF("error to set coding type on I2S CUV\n");
                return 0;
            }
			#else
			//gHDMIAudioParams.formatCode = hdmi_audio_mode->formatCode;
			#endif

//Check: don't care
            // word length
            if (codingtype == CUV_LPCM)
            {
                switch(hdmi_audio_mode->wordLength)
                {
                    case WORD_16:
                    {
                        wordlength = CUV_WL_20_16;
                        break;
                    }
                    case WORD_17:
                    {
                        wordlength = CUV_WL_20_17;
                        break;
                    }
                    case WORD_18:
                    {
                        wordlength = CUV_WL_20_18;
                        break;
                    }
                    case WORD_19:
                    {
                        wordlength = CUV_WL_20_19;
                        break;
                    }
                    case WORD_20:
                    {
                        wordlength = CUV_WL_24_20;
                        break;
                    }
                    case WORD_21:
                    {
                        wordlength = CUV_WL_24_21;
                        break;
                    }
                    case WORD_22:
                    {
                        wordlength = CUV_WL_24_22;
                        break;
                    }
                    case WORD_23:
                    {
                        wordlength = CUV_WL_24_23;
                        break;
                    }
                    case WORD_24:
                    {
                        wordlength = CUV_WL_24_24;
                        break;
                    }
                    default:
                    {
                        wordlength = CUV_WL_24_NOT_DEFINED;
                        break;
                    }
                } // switch

				#if (0)
                if (ioctl(audio_fd,AUDIO_IOC_SET_I2S_CUV_SET_WORDLENGTH ,&wordlength))
                {
                    DPRINTF("error to set word length on I2S CUV\n");
                    return 0;
                }
				#else
	            if (!hdmi_set_audio_cuv_wordlength(wordlength))
	            {
	                DPRINTF("(wordlength) Not Available Arg\n");
	                return 0;
	            }
				#endif

				#if (0)
                if (ioctl(audio_fd,AUDIO_IOC_SET_I2S_CUV_SET_WORDLENGTH_INFO ,&hdmi_audio_mode->wordLength))
                {
                    DPRINTF("error to set word length info on I2S CUV\n");
                    return 0;
                }
				#else
				//gLPCMWordLen = hdmi_audio_mode->wordLength;
				#endif

            } // if (LPCM)

			#if (0)
            if (ioctl(audio_fd,AUDIO_IOC_UPDATE_I2S_CUV,NULL))
            {
                DPRINTF("error to update CUV fields\n");
                return 0;
            }
			#else
			writeb(0x01, HDMI_SS_I2S_CH_ST_CON);
			#endif

			#if (0)
            if (ioctl(audio_fd,AUDIO_IOC_SET_I2S_PARAMETER,&hdmi_audio_mode->i2sParam))
            {
                DPRINTF("error to set i2s parameters\n");
                return 0;
            }
			#else
			hdmi_set_audio_i2s_parameter(hdmi_audio_mode->i2sParam);
			//memcpy(&gHDMIAudioParams.i2sParam, &i2s, sizeof(struct I2SParameter));
			#endif
			
            break;
        }
        case SPDIF_PORT:
        {
			#if 0
            if (hdmi_audio_mode->formatCode == LPCM_FORMAT)
            {
                codingtype = SPDIF_LPCM;
            }
            else
            {
                codingtype = SPDIF_NLPCM;
            }
            if (codingtype == SPDIF_NLPCM)
            {
                spdif_struct.codingtype = SPDIF_NLPCM;
            }
            else if (codingtype == SPDIF_LPCM)
            {
                spdif_struct.codingtype = SPDIF_LPCM;
            }
            else
            {
                spdif_struct.codingtype = -1;
                return 0;
            }
			#endif
			
            break;
        }
        case DSD_PORT:
        {
            //TODO: implement
            break;
        }
        default:
            DPRINTF("not available arg on input port\n");
            return 0;
    }

    // set input port
    #if (0)
    if (ioctl(audio_fd, AUDIO_IOC_SET_AUDIOINPUT ,&hdmi_audio_mode->inputPort) )
    {
        DPRINTF("error to set input port on audio\n");
        return 0;
    }
	#else
    if (!hdmi_set_audio_inputport(hdmi_audio_mode->inputPort))
    {
        DPRINTF("(hdmi_audio_mode->inputPort) Not Available Arg\n");
        return 0;
    }
	//gHDMIAudioParams.inputPort = hdmi_audio_mode->inputPort;
	#endif

    // set audio channel num on audio sample packet and audio infoframe
    #if (0)
    if (ioctl(hdmi_fd, HDMI_IOC_SET_AUDIOCHANNEL ,&hdmi_audio_mode->channelNum))
    {
        DPRINTF("error to set audio channel number on HDMI\n");
        return 0;
    }
	#else
    if (!hdmi_set_audio_channel_number(hdmi_audio_mode->channelNum))
    {
        DPRINTF("(hdmi_audio_mode->channelNum) Not available Arg\n");
        return 0;
    }
	//gAudioChNum = hdmi_audio_mode->channelNum;
	#endif

    // set audio clock recovery packet and audio infoframe sample freq
    #if (0)
    if (ioctl(hdmi_fd, HDMI_IOC_SET_AUDIOSAMPLEFREQ  ,&hdmi_audio_mode->sampleFreq))
    {
        DPRINTF("error to set audio sample freq on HDMI\n");
        return 0;
    }
	#else
    reg = readb(HDMI_CON_0);

    if ( !hdmi_set_audio_sample_freq(hdmi_audio_mode->sampleFreq) )
    {
        DPRINTF("(hdmi_audio_mode->sampleFreq) Not available Arg\n");
        return 0;
    }
    // set audio enable
    writeb(reg|HDMI_ASP_ENABLE ,HDMI_CON_0);

	//gSampleFreq = hdmi_audio_mode->sampleFreq;
	#endif

    // get hdmi audio parameters

    if (hdmi_audio_mode->outPacket == HDMI_ASP)
    {
        // reset sampling freq fields in AUI
        #if (0)
        if (ioctl(hdmi_fd, HDMI_IOC_RESET_AUISAMPLEFREQ, NULL))
        {
            DPRINTF("fail to reset sampling freq field in AUI InfoFrame\n");
            return 0;
        }
		#else
	    reg = readb(HDMI_AUI_BYTE2) & ~HDMI_AUI_SF_MASK;
	    writeb(reg, HDMI_AUI_BYTE2);
		#endif
    }

    // set audio packet type
    #if (0)
    if (ioctl(hdmi_fd, HDMI_IOC_SET_AUDIOPACKETTYPE ,&hdmi_audio_mode->outPacket))
    {
        DPRINTF("error to set audio packet type on HDMI\n");
        return 0;
    }
	#else
    if (!hdmi_set_audio_packet_type(hdmi_audio_mode->outPacket))
    {
        DPRINTF("(hdmi_audio_mode->outPacket) Not available Arg\n");
        return 0;
    }
	//gOutPacket = hdmi_audio_mode->outPacket;
	#endif

    return 1;
}

/**
 * Set speaker allocation information.
 * @param   speaker [in]    Value to set. @n
 *                          For the values, refer CEA-861 Spec.
 * @return If success, return 1;Otherwise, return 0
 */
int hdmi_set_speaker_allocation(const unsigned int speaker)
{
    writeb(speaker,HDMI_AUI_BYTE4);
    return 1;
}

/**
 * Enable/Disable blue screen mode.
 * @param   enable [in]    1 to enable blue screen mode @n
 *                         0 to disable blue screen mode
 * @return If success, return 1;Otherwise, return 0
 */
int hdmi_set_bluescreen(const unsigned char enable)
{
    unsigned char reg;

    // get arg

    reg = readb(HDMI_CON_0);
    if (enable) // if on
    {
        writeb(reg|HDMI_BLUE_SCR_ENABLE,HDMI_CON_0);
    }
    else // if off
    {
        writeb(reg &~HDMI_BLUE_SCR_ENABLE,HDMI_CON_0);
    }

    return 1;
}

/**
 * Enable/Disable audio mute.
 * @param   enable [in]    0 to enable audio mute @n
 *                         1 to disable audio mute
 * @return If success, return 1;Otherwise, return 0
 */
int hdmi_set_audio_enable(const unsigned char enable)
{
    unsigned char reg;

    DPRINTF("(audio mute) %d\n", enable);

    reg = readb(HDMI_CON_0);
    // enable audio output
    if (enable)
    {
#if (1) && defined(TELECHIPS)
        hdmi_aui_update_checksum();
        writeb(TRANSMIT_EVERY_VSYNC,HDMI_AUI_CON);
    //  writeb(TRANSMIT_ONCE,HDMI_AUI_CON);
    //  writeb(ACR_MEASURED_CTS_MODE,HDMI_ACR_CON);
#endif
        writeb(reg|HDMI_ASP_ENABLE,HDMI_CON_0);
    }
    else // disable encryption
    {
#if (1) && defined(TELECHIPS)
        writeb(DO_NOT_TRANSMIT,HDMI_AUI_CON);
        writeb(DO_NOT_TRANSMIT,HDMI_ACR_CON);
#endif
        writeb(reg& ~HDMI_ASP_ENABLE,HDMI_CON_0);
    }

    return 1;
}

/**
 * Enable/Disable audio mute.
 * @param   enable [in]    1 to enable audio mute @n
 *                         0 to disable audio mute
 * @return If success, return 1;Otherwise, return 0
 */
int hdmi_set_audio_mute(const unsigned char enable)
{
	unsigned char parm;

	if (enable)	parm = 0;
	else		parm = 1;

	hdmi_set_audio_enable(parm);

    return 1;
}

/**
 * Enable/Disable A/V mute mode.
 * @param   enable [in]    1 to enable A/V mute mode @n
 *                         0 to disable A/V mute mode
 * @return If success, return 1;Otherwise, return 0
 */
int hdmi_set_av_mute(const unsigned char enable)
{
	unsigned char reg;
	
	DPRINTF("(a/v mute) %d\n", enable);

    reg = readb(HDMI_MODE_SEL) & HDMI_MODE_SEL_HDMI;
    if (reg)
    {
        if (enable)
        {
            // set AV Mute
            writeb(GCP_AVMUTE_ON,HDMI_GCP_BYTE1);
            writeb(GCP_TRANSMIT_EVERY_VSYNC,HDMI_GCP_CON);
        }
        else
        {
            // clear AV Mute
            writeb(GCP_AVMUTE_OFF, HDMI_GCP_BYTE1);
            writeb(GCP_TRANSMIT_EVERY_VSYNC,HDMI_GCP_CON);
        }
    }

    return 1;
}

/**
 * hdmi_phy_reset.
 */
void hdmi_phy_reset(void)
{
	unsigned int  regl;
	unsigned char phy_status;
	unsigned int phy_chk_cnt = 0;

	// HDMI PHY Reset
	regl = readl(DDICFG_HDMICTRL);
	writel(regl | HDMICTRL_RESET_HDMI, DDICFG_HDMICTRL);
	{volatile int ttt;for(ttt=0;ttt<0x5000;ttt++);}
	writel(regl & ~HDMICTRL_RESET_HDMI, DDICFG_HDMICTRL);

	do
	{
		phy_status = readb(HDMI_PHY_STATUS);
		if(phy_chk_cnt++ == 200)
			break;

		{volatile int ttt;for(ttt=0;ttt<0x5000;ttt++);}
	}while(!phy_status);

	if(phy_status)
		DPRINTF("%s phy is ready\n", __func__);
	else
	{
		DPRINTF("%s phy is not ready\n", __func__);
		DPRINTF("%s try phy reset again\n", __func__);
		// HDMI PHY Reset
		regl = readl(DDICFG_HDMICTRL);
		writel(regl | HDMICTRL_RESET_HDMI, DDICFG_HDMICTRL);
		{volatile int ttt;for(ttt=0;ttt<0x5000;ttt++);}
		writel(regl & ~HDMICTRL_RESET_HDMI, DDICFG_HDMICTRL);

		phy_chk_cnt = 0;

		do
		{
			phy_status = readb(HDMI_PHY_STATUS);
			if(phy_chk_cnt++ == 200)
				break;

			{volatile int ttt;for(ttt=0;ttt<0x5000;ttt++);}
		}while(!phy_status);

		if(phy_status)
			DPRINTF("%s phy is ready\n", __func__);
		else
			DPRINTF("%s phy is not ready\n", __func__);
	}
}

/**
 * Enable HDMI output.
 */
void hdmi_start(void)
{
    unsigned char reg,mode;
	unsigned int  regl,val;
	int is_lcdc_en=1;	// enable.
	unsigned int cnt=0;
	PLCDC pLCDC;

	regl = readl(DDICFG_HDMICTRL);
	val = (regl & HDMICTRL_PATH_LCDC1) >> 15;

#if defined(_TCC9300_)
	if (val)	// lcdc1
		pLCDC = (LCDC *)&HwLCDC1_BASE;
	else
		pLCDC = (LCDC *)&HwLCDC0_BASE;
#else
	if (val)	// lcdc1
		pLCDC = (LCDC *)HwLCDC1_BASE;
	else
		pLCDC = (LCDC *)HwLCDC0_BASE;
#endif

	// disable lcdc
	regl = readl(&pLCDC->LCTRL);
//	writel(regl & ~(0x00000001), &pLCDC->LCTRL);
	if (regl & 0x00000001)
	{
		is_lcdc_en = 1;
		writel(regl & ~(0x00000001), &pLCDC->LCTRL);

		while(pLCDC->LSTATUS & 0x40000000)	// BUSY	Hw30
		{
		//	DPRINTF("LCDC-LCTRL[%x]STATUS[%x]\n", pLCDC->LCTRL, pLCDC->LSTATUS);
			{volatile int ttt;for(ttt=0;ttt<0x5000;ttt++);}//udelay(1000*1);

			if (pLCDC->LCTRL & 0x00000001)
			{
		//		DPRINTF(">>lcdc enabled\n");
				break;
			}

			if (cnt++ == 30)
			{
		//		DPRINTF("lcdc busy!\n");
				break;
			}
		}
	}

	mode = readb(HDMI_AVI_BYTE1) & (AVI_CS_Y422|AVI_CS_Y444);
	if(mode)	// YUV420 or YUV444
	{
		regl = readl(&pLCDC->LCTRL);
		regl |= Hw10;		//R2Y: enable
		regl = ((regl&(~(Hw20-Hw16))) | (8<<16));	//pxdw[19:16]	- YCbCr
		writel(regl, &pLCDC->LCTRL);
	}
	else		// RGB
	{
		regl = readl(&pLCDC->LCTRL);
		regl &= (~Hw10);	//R2Y: disable
		regl = ((regl&(~(Hw20-Hw16))) | (12<<16));	//pxdw[19:16]	- rgb888
		writel(regl, &pLCDC->LCTRL);
	}

	// enable lcdc
	if (is_lcdc_en)
	{
		regl = readl(&pLCDC->LCTRL);
		writel(regl | 0x00000001, &pLCDC->LCTRL);
	}

    // check HDMI mode
    mode = readb(HDMI_MODE_SEL) & HDMI_MODE_SEL_HDMI;
    reg = readb(HDMI_CON_0);

    // enable external vido gen.
    writeb(HDMI_EXTERNAL_VIDEO,HDMI_VIDEO_PATTERN_GEN);

    if (mode) // HDMI
    {
        // enable AVI packet: mandatory
        // update avi packet checksum
        hdmi_avi_update_checksum();
        // enable avi packet
        writeb(TRANSMIT_EVERY_VSYNC,HDMI_AVI_CON);

        // check if audio is enable
        if (readb(HDMI_ACR_CON))
        {
            // enable aui packet
			hdmi_aui_update_checksum();
			writeb(TRANSMIT_EVERY_VSYNC,HDMI_AUI_CON);
		//	writeb(TRANSMIT_ONCE,HDMI_AUI_CON);
			reg |= HDMI_ASP_ENABLE;
        }

        // check if it is deep color mode or not
        if (readb(HDMI_DC_CONTROL))
        {
            // enable gcp
            writeb(GCP_TRANSMIT_EVERY_VSYNC,HDMI_GCP_CON);
        }
        // enable hdmi
#if defined(TELECHIPS)
		writeb(reg|HDMI_SYS_ENABLE,HDMI_CON_0);
#else
        writeb(reg|HDMI_SYS_ENABLE|HDMI_ENCODING_OPTION_ENABLE,HDMI_CON_0);
#endif
    }
    else // DVI
    {
        // disable all packet
        writeb(DO_NOT_TRANSMIT,HDMI_AUI_CON);
        writeb(DO_NOT_TRANSMIT,HDMI_AVI_CON);
        writeb(DO_NOT_TRANSMIT,HDMI_GCP_CON);

        // enable hdmi without audio
        reg &= ~HDMI_ASP_ENABLE;
#if defined(TELECHIPS)
		writeb(reg|HDMI_SYS_ENABLE,HDMI_CON_0);
#else
        writeb(reg|HDMI_SYS_ENABLE|HDMI_ENCODING_OPTION_ENABLE,HDMI_CON_0);
#endif
    }

	//gHdmiStartFlag = 1;
	
    return;
}

/**
 * Disable HDMI output.
 */
void hdmi_stop(void)
{
    unsigned char reg;

    reg = readb(HDMI_CON_0);
    writeb(reg & ~HDMI_SYS_ENABLE,HDMI_CON_0);

	//gHdmiStartFlag = 0;
}


//==============================================================================
//                 Section x - Application
//==============================================================================

const struct hdmi_video_mode_list_t {
	enum VideoFormat 		vfmt_val;
	enum PixelAspectRatio	ratio;
	unsigned int			width;
	unsigned int			height;
	unsigned char			vfmt_str[32];
} gRefHdmiVideoModeList[15] = 
{
/* [ 0] */ { v1920x1080p_60Hz, HDMI_PIXEL_RATIO_16_9,  1920,   1080,   "1920x1080P 60Hz [16:9]"  },
/* [ 1] */ { v1920x1080p_50Hz, HDMI_PIXEL_RATIO_16_9,  1920,   1080,   "1920x1080P 50Hz [16:9]"  },
/* [ 2] */ { v1920x1080i_60Hz, HDMI_PIXEL_RATIO_16_9,  1920,   1080,   "1920x1080I 60Hz [16:9]"  },
/* [ 3] */ { v1920x1080i_50Hz, HDMI_PIXEL_RATIO_16_9,  1920,   1080,   "1920x1080I 50Hz [16:9]"  },
/* [ 4] */ { v1280x720p_60Hz , HDMI_PIXEL_RATIO_16_9,  1280,    720,   " 1280x720P 60Hz [16:9]"  },
/* [ 5] */ { v1280x720p_50Hz , HDMI_PIXEL_RATIO_16_9,  1280,    720,   " 1280x720P 50Hz [16:9]"  },
/* [ 6] */ { v720x480p_60Hz  , HDMI_PIXEL_RATIO_16_9,   720,    480,   "  720x480P 60Hz [16:9]"  },
/* [ 7] */ { v720x480p_60Hz  , HDMI_PIXEL_RATIO_4_3 ,   720,    480,   "  720x480P 60Hz [ 4:3]"  },
/* [ 8] */ { v720x480i_60Hz  , HDMI_PIXEL_RATIO_16_9,   720,    480,   "  720x480I 60Hz [16:9]"  },
/* [ 9] */ { v720x480i_60Hz  , HDMI_PIXEL_RATIO_4_3 ,   720,    480,   "  720x480I 60Hz [ 4:3]"  },
/* [10] */ { v720x576p_50Hz  , HDMI_PIXEL_RATIO_16_9,   720,    576,   "  720x576P 50Hz [16:9]"  },
/* [11] */ { v720x576p_50Hz  , HDMI_PIXEL_RATIO_4_3 ,   720,    576,   "  720x576P 50Hz [ 4:3]"  },
/* [12] */ { v720x576i_50Hz  , HDMI_PIXEL_RATIO_16_9,   720,    576,   "  720x576I 50Hz [16:9]"  },
/* [13] */ { v720x576i_50Hz  , HDMI_PIXEL_RATIO_4_3 ,   720,    576,   "  720x576I 50Hz [ 4:3]"  },
/* [14] */ { v640x480p_60Hz  , HDMI_PIXEL_RATIO_4_3 ,   640,    480,   "  640x480P 60Hz [ 4:3]"  }
};

void disp_init_hdmi(void)
{
	const struct HDMIVideoParameter video = {
	#if (HDMI_MODE_TYPE == 1)
	/*	video.mode 				=*/	HDMI,
	#else
	/*	video.mode 				=*/	DVI,
	#endif
	/*	video.resolution 		=*/	gRefHdmiVideoModeList[HDMI_VIDEO_MODE_TYPE].vfmt_val,
	/*	video.colorSpace		=*/	HDMI_CS_RGB,
	/*	video.colorDepth		=*/	HDMI_CD_24,
	/*	video.colorimetry		=*/	HDMI_COLORIMETRY_NO_DATA,
	/*	video.pixelAspectRatio	=*/	gRefHdmiVideoModeList[HDMI_VIDEO_MODE_TYPE].ratio,
	};

	const struct HDMIAudioParameter audio = {
	/*	audio.inputPort 		=*/	I2S_PORT,
	/*	audio.outPacket 		=*/	HDMI_ASP,
	/*	audio.formatCode		=*/	LPCM_FORMAT,
	/*	audio.channelNum		=*/	CH_2,
	/*	audio.sampleFreq		=*/	SF_44KHZ,
	/*	audio.wordLength		=*/	WORD_16,

	/*	audio.i2sParam.bpc		=*/	I2S_BPC_16,
	/*	audio.i2sParam.format	=*/	I2S_BASIC,
	/*	audio.i2sParam.clk		=*/	I2S_64FS
	};

	hdmi_init();

	hdmi_set_hdmi_mode(video.mode);

	hdmi_set_video_mode(&video);

	if (video.mode == HDMI)
		hdmi_set_audio_mode(&audio);

//	hdmi_stop();
	hdmi_start();

}

#endif	//#if defined(DEFAULT_DISPLAY_HDMI) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)

