/*
 * Copyright (c) 2010 Telechips, Inc.
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

#ifndef _VTC_H
#define _VTC_H

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//	Functions for Client Driver
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int	VTC_CheckIsReceivedData(void);
unsigned int	VTC_CheckIsTransmitReady(void);
void			VTC_ReceiveData_SetSize( unsigned int nBufferSize );
unsigned int	VTC_ReceiveData_GetPointer(void **ppBuffer);
unsigned int	VTC_ReceiveData( void* lpBuffer, unsigned int nBufferSize );
unsigned int	VTC_SendData( const void* lpBuffer, unsigned int nBufferSize );
unsigned int	VTC_SendMessage(const void* lpBuffer, unsigned int nBufferSize);

#endif //_VTC_H
