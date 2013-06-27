/*
   LZ4 HC - High Compression Mode of LZ4
   Header File
   Copyright (C) 2011-2012, Yann Collet.
   BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

       * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following disclaimer
   in the documentation and/or other materials provided with the
   distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   You can contact the author at :
   - LZ4 homepage : http://fastcompression.blogspot.com/p/lz4.html
   - LZ4 source repository : http://code.google.com/p/lz4/
*/
#pragma once


#if defined (__cplusplus)
extern "C" {
#endif

//**************************************
// Basic Types
//**************************************
#ifndef BYTE
#define BYTE	uint8_t
#endif

#ifndef U16
#define U16		uint16_t
#endif

#ifndef U32
#define U32		uint32_t
#endif

#ifndef S32
#define S32		int32_t
#endif

#ifndef U64
#define U64		uint64_t
#endif

#ifndef HTYPE
#define HTYPE					const BYTE*
#endif

//**************************************
// Constants
//**************************************
#define MINMATCH 4

#define DICTIONARY_LOGSIZE 16
#define MAXD (1<<DICTIONARY_LOGSIZE)
#define MAXD_MASK ((U32)(MAXD - 1))
#define MAX_DISTANCE (MAXD - 1)

#define HASH_LOG (DICTIONARY_LOGSIZE-1)
#define HASHTABLESIZE (1 << HASH_LOG)
#define HASH_MASK (HASHTABLESIZE - 1)

#define MAX_NB_ATTEMPTS 256

#define ML_BITS  4
#define ML_MASK  (size_t)((1U<<ML_BITS)-1)
#define RUN_BITS (8-ML_BITS)
#define RUN_MASK ((1U<<RUN_BITS)-1)

#define COPYLENGTH 8
#define LASTLITERALS 5
#define MFLIMIT (COPYLENGTH+MINMATCH)
#define MINLENGTH (MFLIMIT+1)
#define OPTIMAL_ML (int)((ML_MASK-1)+MINMATCH)

typedef struct 
{
	const BYTE* base;
	HTYPE hashTable[HASHTABLESIZE];
	U16 chainTable[MAXD];
	const BYTE* nextToUpdate;
} LZ4HC_Data_Structure;


int LZ4_compressHC (const char* source, char* dest, int isize, char* hashTable);

/*
LZ4_compressHC :
	return : the number of bytes in compressed buffer dest
	note : destination buffer must be already allocated. 
		To avoid any problem, size it to handle worst cases situations (input data not compressible)
		Worst case size evaluation is provided by function LZ4_compressBound() (see "lz4.h")
*/


/* Note :
Decompression functions are provided within regular LZ4 source code (see "lz4.h") (BSD license)
*/


#if defined (__cplusplus)
}
#endif
