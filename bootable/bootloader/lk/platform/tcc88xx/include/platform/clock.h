
#ifndef _PLATFORM_CLOCK_H_
#define _PLATFORM_CLOCK_H_

extern int change_mem_clock(unsigned int freq);
extern void tca_ckc_init(void);

extern int tca_ckc_setpll(unsigned int pll, unsigned int ch);
extern unsigned int tca_ckc_getpll(unsigned int ch);
extern unsigned int tca_ckc_getdividpll(unsigned int ch);

extern unsigned int tca_ckc_setfbusctrl(unsigned int clkname, unsigned int isenable, unsigned int freq);
extern unsigned int tca_ckc_getfbusctrl(unsigned int clkname);

extern unsigned int tca_ckc_setperi(unsigned int periname,unsigned int isenable, unsigned int freq);

extern unsigned int tca_ckc_getperi(unsigned int periname);

extern int tca_ckc_setpmupwroff( unsigned int sel , unsigned int ispwdn);
extern int tca_ckc_getpmupwroff( unsigned int sel);
extern int tca_ckc_setfbusswreset(unsigned int fbusname, unsigned int isreset);

extern int tca_ckc_setiopwdn(unsigned int sel, unsigned int ispwdn);
extern int tca_ckc_getiopwdn(unsigned int sel);
extern int tca_ckc_setioswreset(unsigned int sel, unsigned int isreset);

extern int tca_ckc_setddipwdn(unsigned int sel , unsigned int ispwdn);
extern int tca_ckc_getddipwdn(unsigned int sel);
extern int tca_ckc_setddiswreset(unsigned int sel, unsigned int isreset);

extern int tca_ckc_setgpupwdn(unsigned int sel , unsigned int ispwdn);
extern int tca_ckc_getgpupwdn(unsigned int sel);
extern int tca_ckc_setgpuswreset(unsigned int sel, unsigned int isreset);

extern int tca_ckc_setvideopwdn(unsigned int sel , unsigned int ispwdn);
extern int tca_ckc_getvideopwdn(unsigned int sel);
extern int tca_ckc_setvideoswreset(unsigned int sel, unsigned int isreset);

extern int tca_ckc_setcampwdn(unsigned int sel , unsigned int ispwdn);
extern int tca_ckc_getcampwdn(unsigned int sel);
extern int tca_ckc_setcamswreset(unsigned int sel, unsigned int isreset);

extern int tca_ckc_sethsiopwdn(unsigned int sel , unsigned int ispwdn);
extern int tca_ckc_gethsiopwdn(unsigned int sel);
extern int tca_ckc_sethsioswreset(unsigned int sel, unsigned int isreset);

extern int tca_ckc_fclk_enable(unsigned int fclk, unsigned int enable);
extern int tca_ckc_pclk_enable(unsigned int pclk, unsigned int enable);

#endif /* _PLATFORM_CLOCK_H_ */
