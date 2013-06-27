/****************************************************************************
 *   FileName    :  Typedefine.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/

typedef unsigned long           UNSIGNED;
typedef long                    SIGNED;
typedef unsigned char           DATA_ELEMENT;
typedef DATA_ELEMENT            OPTION;
typedef DATA_ELEMENT            BOOLEAN;
typedef int                     STATUS;
typedef unsigned char           UNSIGNED_CHAR;
#ifndef _CHAR_
#define _CHAR_
typedef char                    CHAR;
#endif
typedef unsigned int            UNSIGNED_INT;
typedef int                     INT;
typedef unsigned long *         UNSIGNED_PTR;
typedef unsigned char *         BYTE_PTR;
#define VOID void

#ifndef _INT8_
#define _INT8_
typedef char           INT8;
#endif
#ifndef _UINT8_
#define _UINT8_
typedef unsigned char  UINT8;
#endif
#ifndef _INT16_
#define _INT16_
typedef signed short   INT16;
#endif
#ifndef _UINT16_
#define _UINT16_
typedef unsigned short UINT16;
#endif
#ifndef _INT32_
#define _INT32_
typedef signed long    INT32;
#endif
#ifndef _UINT32_
#define _UINT32_
typedef unsigned long  UINT32;
#endif
