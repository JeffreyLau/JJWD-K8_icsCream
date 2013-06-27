/***************************************************************************************
*	FileName    : lcd driver.h
****************************************************************************************
* LMS350DF01
****************************************************************************************/
#if defined(DEFAULT_DISPLAY_COMPOSITE) || defined(DEFAULT_DISPLAY_OUTPUT_DUAL)

//#define COMPOSITE_720_480
#define COMPOSITE_720_576

/* Composite MAX Specification */
#define COMPOSITE_WIDTH_MAX				720
#define COMPOSITE_HEIGHT_MAX			576
#define COMPOSITE_BPP_MAX				32

typedef struct{
	unsigned int composite_clock; 		// pixel clock
	unsigned int composite_divider; 	// pixel clock
	unsigned int composite_bus_width;	// data bus width
	unsigned int composite_width;		// lcd width
	unsigned int composite_height;		// lcd height
	unsigned int composite_LPW; 		// line pulse width
	unsigned int composite_LPC; 		// line pulse count (active horizontal pixel - 1)
	unsigned int composite_LSWC;		// line start wait clock (the number of dummy pixel clock - 1)
	unsigned int composite_LEWC;		// line end wait clock (the number of dummy pixel clock - 1)
	unsigned int composite_VDB; 		// Back porch Vsync delay
	unsigned int composite_VDF; 		// front porch of Vsync delay
	unsigned int composite_FPW1;		// TFT/TV : Frame pulse width is the pulse width of frmae clock
	unsigned int composite_FLC1;		// frmae line count is the number of lines in each frmae on the screen
	unsigned int composite_FSWC1;		// frmae start wait cycle is the number of lines to insert at the end each frame
	unsigned int composite_FEWC1;		// frame start wait cycle is the number of lines to insert at the begining each frame
	unsigned int composite_FPW2;		// TFT/TV : Frame pulse width is the pulse width of frmae clock
	unsigned int composite_FLC2;		// frmae line count is the number of lines in each frmae on the screen
	unsigned int composite_FSWC2;		// frmae start wait cycle is the number of lines to insert at the end each frame
	unsigned int composite_FEWC2; 		// frame start wait cycle is the number of lines to insert at the begining each frame
}stCOMPOSITE_SPEC;

#endif	/*DEFAULT_DISPLAY_COMPOSITE*/
