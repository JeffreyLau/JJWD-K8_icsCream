/***************************************************************************************
*	FileName    : lcd driver.h
****************************************************************************************
* LMS350DF01
****************************************************************************************/
#if defined(DEFAULT_DISPLAY_COMPONENT) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)

//#define COMPONENT_720_480
#define COMPONENT_720_576

#define COMPONENT_WIDTH_MAX			720
#define COMPONENT_HEIGHT_MAX		576
#define COMPONENT_BPP_MAX			32

typedef enum{
	COMPONENT_MODE_NTSC_M = 0,
	COMPONENT_MODE_NTSC_M_J,
	COMPONENT_MODE_NTSC_N,
	COMPONENT_MODE_NTSC_N_J,
	COMPONENT_MODE_NTSC_443,
	COMPONENT_MODE_PAL_M,
	COMPONENT_MODE_PAL_N,
	COMPONENT_MODE_PAL_B,
	COMPONENT_MODE_PAL_G,
	COMPONENT_MODE_PAL_H,
	COMPONENT_MODE_PAL_I,
	COMPONENT_MODE_PSEUDO_NTSC,
	COMPONENT_MODE_PSEUDO_PAL,
	COMPONENT_MODE_480I,
	COMPONENT_MODE_480P,
	COMPONENT_MODE_720P,
	COMPONENT_MODE_1080I,
	COMPONENT_MODE_MAX
}COMPONENT_MODE_TYPE;

typedef struct{
	unsigned int component_clock; 		// pixel clock
	unsigned int component_divider; 	// pixel clock
	unsigned int component_bus_width;	// data bus width
	unsigned int component_width;		// lcd width
	unsigned int component_height;		// lcd height
	unsigned int component_LPW; 		// line pulse width
	unsigned int component_LPC; 		// line pulse count (active horizontal pixel - 1)
	unsigned int component_LSWC;		// line start wait clock (the number of dummy pixel clock - 1)
	unsigned int component_LEWC;		// line end wait clock (the number of dummy pixel clock - 1)
	unsigned int component_VDB; 		// Back porch Vsync delay
	unsigned int component_VDF; 		// front porch of Vsync delay
	unsigned int component_FPW1;		// TFT/TV : Frame pulse width is the pulse width of frmae clock
	unsigned int component_FLC1;		// frmae line count is the number of lines in each frmae on the screen
	unsigned int component_FSWC1;		// frmae start wait cycle is the number of lines to insert at the end each frame
	unsigned int component_FEWC1;		// frame start wait cycle is the number of lines to insert at the begining each frame
	unsigned int component_FPW2;		// TFT/TV : Frame pulse width is the pulse width of frmae clock
	unsigned int component_FLC2;		// frmae line count is the number of lines in each frmae on the screen
	unsigned int component_FSWC2;		// frmae start wait cycle is the number of lines to insert at the end each frame
	unsigned int component_FEWC2; 		// frame start wait cycle is the number of lines to insert at the begining each frame
}stCOMPONENT_SPEC;

#endif	/*DEFAULT_DISPLAY_COMPONENT*/
