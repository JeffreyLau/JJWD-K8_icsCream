/*
 * Copyright (c) 2011 Telechips, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

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

extern int tca_ckc_setippwdn( unsigned int sel, unsigned int ispwdn);
extern int tca_ckc_setfbuspwdn( unsigned int fbusname, unsigned int ispwdn);
extern int tca_ckc_getfbuspwdn( unsigned int fbusname);
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

#if 0
extern int tca_ckc_setvideopwdn(unsigned int sel , unsigned int ispwdn);
extern int tca_ckc_getvideopwdn(unsigned int sel);
extern int tca_ckc_setvideoswreset(unsigned int sel, unsigned int isreset);
#endif

extern int tca_ckc_sethsiopwdn(unsigned int sel , unsigned int ispwdn);
extern int tca_ckc_gethsiopwdn(unsigned int sel);
extern int tca_ckc_sethsioswreset(unsigned int sel, unsigned int isreset);

#endif /* _PLATFORM_CLOCK_H_ */
