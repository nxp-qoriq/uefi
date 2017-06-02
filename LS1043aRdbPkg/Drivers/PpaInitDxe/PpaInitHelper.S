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
GCC_ASM_EXPORT(DCacheCleanAllLevels)

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


// VOID DCacheCleanAllLevels(VOID)
// Clean all D-cache levels

ASM_PFX(DCacheCleanAllLevels):
	MRS X0, CLIDR_EL1
	AND W3, W0, #0x07000000  // Get 2 x Level of Coherence
	LSR W3, W3, #23
	CBZ W3, Finished
	MOV W10, #0              // W10 = 2 x cache level
	MOV W8, #1               // W8 = constant 0b1
Loop1:
	ADD W2, W10, W10, LSR #1 // Calculate 3 x cache level
	LSR W1, W0, W2           // extract 3-bit cache type for this level
	AND W1, W1, #0x7
	CMP W1, #2
	B.LT Skip                // No data or unified cache at this level
	MSR CSSELR_EL1, X10      // Select this cache level
	ISB                      // Synchronize change of CSSELR
	MRS X1, CCSIDR_EL1       // Read CCSIDR
	AND W2, W1, #7           // W2 = log2(linelen)-4
	ADD W2, W2, #4           // W2 = log2(linelen)
	UBFX W4, W1, #3, #10     // W4 = max way number, right aligned
	CLZ W5, W4               /* W5 = 32-log2(ways), bit position of way in DC operand */
	LSL W9, W4, W5           /* W9 = max way number, aligned to position in DC operand */
	LSL W16, W8, W5          // W16 = amount to decrement way number per iteration
Loop2:
	UBFX W7, W1, #13, #15    // W7 = max set number, right aligned
	LSL W7, W7, W2           /* W7 = max set number, aligned to position in DC operand */
	LSL W17, W8, W2          // W17 = amount to decrement set number per iteration
Loop3:
	ORR W11, W10, W9         // W11 = combine way number and cache number...
	ORR W11, W11, W7         // ... and set number for DC operand
	DC CSW, X11              // Do data cache clean by set and way
	SUBS W7, W7, W17         // Decrement set number
	B.GE Loop3
	SUBS X9, X9, X16         // Decrement way number
	B.GE Loop2
Skip:
	ADD W10, W10, #2         // Increment 2 x cache level
	CMP W3, W10
	DSB sy                   /* Ensure completion of previous cache maintenance operation */
	B.GT Loop1
Finished:
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
