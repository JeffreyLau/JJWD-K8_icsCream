/*
 * linux/arch/arm/plat-tcc/tcc_asm.S
 *
 * Author:  <linux@telechips.com>
 * Created: April 21, 2008
 * Description: LINUX POWER MANAGEMENT FUNCTIONS
 *
 * Copyright (C) 2008-2009 Telechips 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#ifndef __ALIGN
#define __ALIGN		.align 4,0x90
#define __ALIGN_STR	".align 4,0x90"
#endif


#define ALIGN __ALIGN

#ifndef ENTRY
#define ENTRY(name) \
  .globl name; \
  name:
#endif

//-------------------------------------------------------------------------------
//
//	FUNCTION :
//
//	DESCRIPTION :
//
//-------------------------------------------------------------------------------
ENTRY(IO_ARM_ChangeStackSRAM)

	mov		r0, r13
	ldr		r13, =0x10000800 //;SHUTDOWN_STACK_ADDR+SHUTDOWN_STACK_SIZE
	mov		pc, lr


//-------------------------------------------------------------------------------
//
//	FUNCTION :
//
//	DESCRIPTION :
//
//-------------------------------------------------------------------------------  
ENTRY(IO_ARM_RestoreStackSRAM)

    mov		r13, r0    
    mov		pc, lr

