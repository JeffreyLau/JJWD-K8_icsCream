/******************************************************************************
*
*  (C)Copyright All Rights Reserved by Telechips Inc.
*
*  This material is confidential and shall remain as such.
*  Any unauthorized use, distribution, reproduction is strictly prohibited.
*
*******************************************************************************
*
*  File Name   : Vioc_rdma.h
*
*  Description :
*
*******************************************************************************
*
*  yyyy/mm/dd     ver            descriptions                Author
*	---------	--------   ---------------       -----------------
*    2011/08/08     0.1            created                      hskim
*******************************************************************************/

#ifndef __VIOC_RDMA_H__
#define	__VIOC_RDMA_H__

#include <platform/reg_physical.h>
#include <platform/globals.h>

/* Interface APIs */

extern void VIOC_RDMA_SetImageConfig(VIOC_RDMA * pRDMA);
extern void VIOC_RDMA_SetImageUpdate(VIOC_RDMA *pRDMA);
extern void VIOC_RDMA_SetImageEnable(VIOC_RDMA *pRDMA);
extern void VIOC_RDMA_SetImageDisable(VIOC_RDMA *pRDMA);
extern void VIOC_RDMA_SetImageFormat(VIOC_RDMA *pRDMA, unsigned int nFormat);
extern void VIOC_RDMA_SetImageAlphaEnable(VIOC_RDMA *pRDMA, unsigned int enable);
extern void VIOC_RDMA_GetImageAlphaEnable(VIOC_RDMA *pRDMA, unsigned int *enable);
extern void VIOC_RDMA_SetImageAlphaSelect(VIOC_RDMA *pRDMA, unsigned int select);
extern void VIOC_RDMA_SetImageY2RMode(VIOC_RDMA *pRDMA, unsigned int y2r_mode);
extern void VIOC_RDMA_SetImageY2REnable(VIOC_RDMA *pRDMA, unsigned int enable);
extern void VIOC_RDMA_SetImageAlpha(VIOC_RDMA *pRDMA, unsigned int nAlpha0, unsigned int nAlpha1);
extern void VIOC_RDMA_GetImageAlpha(VIOC_RDMA *pRDMA, unsigned int *nAlpha0, unsigned int *nAlpha1);
extern void VIOC_RDMA_SetImageUVIEnable(VIOC_RDMA *pRDMA, unsigned int enable);
extern void VIOC_RDMA_SetImageSize(VIOC_RDMA *pRDMA, unsigned int sw, unsigned int sh);
extern void VIOC_RDMA_SetImageBase(VIOC_RDMA *pRDMA, unsigned int nBase0, unsigned int nBase1, unsigned int nBase2);
extern void VIOC_RDMA_SetImageSizeDownForScaler(VIOC_RDMA *pRDMA, unsigned int sw, unsigned int sh, unsigned int ratio);
extern void VIOC_RDMA_SetImageSizeDownForUpScaler(VIOC_RDMA *pRDMA, unsigned int sw, unsigned int sh, unsigned int ratio);
extern void VIOC_RDMA_SetImageOffset(VIOC_RDMA *pRDMA, unsigned int nOffset0, unsigned int nOffset1);
extern void VIOC_RDMA_SetImageScale(VIOC_RDMA *pRDMA, unsigned int scaleX, unsigned int scaleY);
extern void VIOC_RDMA_SetImageIntl (VIOC_RDMA * pRDMA, unsigned int intl_en);
extern void VIOC_RDMA_SetY2RConvertEnable(VIOC_RDMA * pRDMA, unsigned int enable);
extern void VIOC_RDMA_SetY2RConvertMode(VIOC_RDMA * pRDMA, unsigned int mode);
extern void VIOC_RDMA_SetIreqMask(VIOC_RDMA * pRDMA, unsigned int mask, unsigned int set);
extern void VIOC_RDMA_IreqHandler( unsigned int vectorID );
#endif
