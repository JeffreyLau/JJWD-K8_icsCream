/******************************************************************************
*
*  (C)Copyright All Rights Reserved by Telechips Inc.
*
*  This material is confidential and shall remain as such.
*  Any unauthorized use, distribution, reproduction is strictly prohibited.
*
*******************************************************************************
*
*  File Name   : vioc_wmix.h
*
*  Description : VIOC wmix components control module header file.
*
*******************************************************************************
*
*  yyyy/mm/dd     ver            descriptions                Author
*	---------	--------   ---------------       -----------------
*    2011/08/10     0.1            created                      hskim
*******************************************************************************/

#ifndef __VIOC_WMIX_H__
#define __VIOC_WMIX_H__
#include <platform/reg_physical.h>
#include <platform/globals.h>


typedef	struct
{
	volatile VIOC_WMIX_MACON_u	uACON;				// 0
	volatile VIOC_WMIX_MCCON_u	uCCON;				// 1
	volatile VIOC_WMIX_MROPC_u	uROPC;				// 2
	volatile VIOC_WMIX_MPAT_u 	uPAT;				// 3
} VIOC_WMIX_ALPHA;


/* Interface APIs */
extern void VIOC_WMIX_SetOverlayPriority(VIOC_WMIX *pWMIX, unsigned int nOverlayPriority);
extern void VIOC_WMIX_SetUpdate(VIOC_WMIX *pWMIX);
#if 0 //[[ hskim_20110920_BEGIN -- Not Support
extern void VIOC_WMIX_SetAlphOpt(VIOC_WMIX *pWMIX, unsigned int opt);
#endif //]] hskim_20110920_END -- Not Support
extern void VIOC_WMIX_SetSize(VIOC_WMIX *pWMIX, unsigned int nWidth, unsigned int nHeight);
extern void VIOC_WMIX_SetBGColor(VIOC_WMIX *pWMIX, unsigned int nBG0, unsigned int nBG1, unsigned int nBG2, unsigned int nBG3);
extern void VIOC_WMIX_SetPosition(VIOC_WMIX *pWMIX, unsigned int nChannel, unsigned int nX, unsigned int nY);
extern void VIOC_WMIX_SetChromaKey(VIOC_WMIX *pWMIX, unsigned int nLayer, unsigned int nKeyEn, unsigned int nKeyR, unsigned int nKeyG, unsigned int nKeyB, unsigned int nKeyMaskR, unsigned int nKeyMaskG, unsigned int nKeyMaskB);
extern void VIOC_WMIX_GetChromaKey(VIOC_WMIX *pWMIX, unsigned int nLayer, unsigned int *nKeyEn, unsigned int *nKeyR, unsigned int *nKeyG, unsigned int *nKeyB, unsigned int *nKeyMaskR, unsigned int *nKeyMaskG, unsigned int *nKeyMaskB);
extern void VIOC_WMIX_ALPHA_SetAlphaValueControl( VIOC_WMIX_ALPHA * pWALPHA, unsigned int region, unsigned int acon0, unsigned int acon1);
extern void VIOC_WMIX_ALPHA_SetColorControl( VIOC_WMIX_ALPHA * pWALPHA, unsigned int region, unsigned int ccon0, unsigned int ccon1);
extern void VIOC_WMIX_ALPHA_SetROPMode( VIOC_WMIX_ALPHA * pWALPHA, unsigned int mode);
extern void VIOC_WMIX_ALPHA_SetAlphaSelection( VIOC_WMIX_ALPHA * pWALPHA, unsigned int asel);
extern void VIOC_WMIX_ALPHA_SetAlphaValue( VIOC_WMIX_ALPHA * pWALPHA, unsigned int alpha0, unsigned int alpha1);
extern void VIOC_WMIX_ALPHA_SetROPPattern( VIOC_WMIX_ALPHA * pWALPHA, unsigned int patR, unsigned int patG, unsigned int patB);
extern void VIOC_WMIX_SetInterruptMask(VIOC_WMIX *pWMIX, unsigned int nMask, unsigned int set);
extern unsigned int VIOC_WMIX_GetStatus(VIOC_WMIX *pWMIX );
extern void VIOC_WMIX_IreqHandler( unsigned int vectorID );

#endif
