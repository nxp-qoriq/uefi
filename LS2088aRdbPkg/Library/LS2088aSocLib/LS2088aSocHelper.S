#  @LS2088aSocHelper.S
#
#  Copyright (c) 2016, Freescale Semiconductor, Inc. All rights
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

GCC_ASM_EXPORT(CleanAllDcacheLevels)
GCC_ASM_EXPORT(CleanDcacheRange)


//
// VOID CleanAllDcacheLevels(VOID)
//
// This function cleans and invalidates all levels of the dcache.
//
ASM_PFX(CleanAllDcacheLevels):
	mrs x0, clidr_el1
	and w3, w0, #0x07000000  // Get 2 x Level of Coherence
	lsr w3, w3, #23
	cbz w3, Finished
	mov w10, #0              // W10 = 2 x cache level
	mov w8, #1               // W8 = constant 0b1
Loop1:
	add w2, w10, w10, lsr #1 // Calculate 3 x cache level
	lsr w1, w0, w2           // extract 3-bit cache type for this level
	and w1, w1, #0x7
	cmp w1, #2
	b.lt Skip                // No data or unified cache at this level
	msr csselr_el1, x10      // Select this cache level
	isb                      // Synchronize change of CSSELR
	mrs x1, ccsidr_el1       // Read CCSIDR
	and w2, w1, #7           // W2 = log2(linelen)-4
	add w2, w2, #4           // W2 = log2(linelen)
	ubfx w4, w1, #3, #10     // W4 = max way number, right aligned
	clz w5, w4               /* W5 = 32-log2(ways), bit position of way in DC operand */
	lsl w9, w4, w5           /* W9 = max way number, aligned to position in DC operand */
	lsl w16, w8, w5          // W16 = amount to decrement way number per iteration
Loop2:
	ubfx w7, w1, #13, #15    // W7 = max set number, right aligned
	lsl w7, w7, w2           /* W7 = max set number, aligned to position in DC operand */
	lsl w17, w8, w2          // W17 = amount to decrement set number per iteration
Loop3:
	orr w11, w10, w9         // W11 = combine way number and cache number...
	orr w11, w11, w7         // ... and set number for DC operand
	dc csw, x11              // Do data cache clean by set and way
	subs w7, w7, w17         // Decrement set number
	b.ge Loop3
	subs x9, x9, x16         // Decrement way number
	b.ge Loop2
Skip:
	add w10, w10, #2         // Increment 2 x cache level
	cmp w3, w10
	dsb sy                   /* Ensure completion of previous cache maintenance operation */
	b.gt Loop1
Finished:
	ret


//
// VOID CleanDcacheRange(UINT64 StartAddr, UINT64 EndAddr)
//
// Cleans and invalidates a range of addresses from D-cache
//
// X0: start address
// X1: end address
//
ASM_PFX(CleanDcacheRange):
	mrs	x3, ctr_el0
	lsr	x3, x3, #16
	and	x3, x3, #0xf
	mov	x2, #4
	lsl	x2, x2, x3		/* cache line size */

	/* x2 <- minimal cache line size in cache system */
	sub	x3, x2, #1
	bic	x0, x0, x3
1:	dc	civac, x0	/* clean & invalidate data or unified cache */
	add	x0, x0, x2
	cmp	x0, x1
	b.lo	1b
	dsb	sy
	ret


ASM_FUNCTION_REMOVE_IF_UNREFERENCED
