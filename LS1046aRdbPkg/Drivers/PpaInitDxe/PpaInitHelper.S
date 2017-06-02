#  @PpaInitHelper.S
#
#  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights
#  reserved.  
#
#  This program and the accompanying materials                          
#  are licensed and made available under the terms and conditions of the BSD License         
#  which accompanies this distribution.  The full text of the license may be found at        
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             
#
#
#include <AutoGen.h>

.text
.align 2

GCC_ASM_EXPORT(PpaInit)
GCC_ASM_EXPORT(El2SwitchSetup)

ASM_PFX(PpaInit):
//Push return address to the stack
//sub  sp, sp, #16
//stur  x30, [sp, #0]

//Save stack pointer for EL2
	mov x1, sp
	msr	sp_el2, x1

//Set boot loc pointer
	adr  x4, 1f
	adr  x1, ADDR_BASE_SCFG
	ldr  w2, [x1]
	mov  x1, x4
	rev  w3, w1
	str  w3, [x2, #0x604]
	lsr  x1, x4, #32
	rev  w3, w1
	str  w3, [x2, #0x600]
	
//Call PPA monitor
	br	x0	

1:
//Pop out return address from stack
//ldur x30, [sp, #0] 
//add sp, sp, #16

	// Enable GICv2 interrupts in EL2 mode
	mrs x0, hcr_el2
	orr x0, x0, #0x18
	msr hcr_el2, x0

//return 0
	mov x0, #0
	ret


ASM_PFX(El2SwitchSetup):
	mov x0, #0x5b1 // non-secure el0/el1 | hvc | 64bit el2
	msr scr_el3, x0
	msr cptr_el3, xzr    // disable coprocessor traps to el3
	mov x0, #0x33ff
	msr cptr_el2, x0  // disable coprocessor traps to el2
	// initialize sctlr_el2
	msr	sctlr_el2, xzr
	mov	x0, #0x3c9
	msr spsr_el3, x0   // el2_sp2 | d | a | i | f
	ret

ADDR_BASE_SCFG:
	.long  0x01570000

ADDR_BASE_DCFG:
	.long 0x01EE0000 

ASM_FUNCTION_REMOVE_IF_UNREFERENCED 
