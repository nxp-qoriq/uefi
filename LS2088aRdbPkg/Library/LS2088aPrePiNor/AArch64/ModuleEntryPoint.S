//  @ModuleEntryPoint.S
//
//  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.
//
//  This program and the accompanying materials
//  are licensed and made available under the terms and conditions of the BSD License
//  which accompanies this distribution.  The full text of the license may be found at
//  http://opensource.org/licenses/bsd-license.php
//
//  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
//  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//

#include <AutoGen.h>
#include <AsmMacroIoLibV8.h>
#include <Library/PcdLib.h>
#inculde <Library/FslDdr.h>


/* TZ Protection Controller Definitions */
#define NXP_TZPC_BASE					0x02200000
#define NXP_TZPCR0SIZE_BASE				(NXP_TZPC_BASE)
#define NXP_TZPCDECPROT_0_STAT_BASE			(NXP_TZPC_BASE + 0x800)
#define NXP_TZPCDECPROT_0_SET_BASE			(NXP_TZPC_BASE + 0x804)
#define NXP_TZPCDECPROT_0_CLR_BASE			(NXP_TZPC_BASE + 0x808)
#define NXP_TZPCDECPROT_1_STAT_BASE			(NXP_TZPC_BASE + 0x80C)
#define NXP_TZPCDECPROT_1_SET_BASE			(NXP_TZPC_BASE + 0x810)
#define NXP_TZPCDECPROT_1_CLR_BASE			(NXP_TZPC_BASE + 0x814)
#define NXP_TZPCDECPROT_2_STAT_BASE			(NXP_TZPC_BASE + 0x818)
#define NXP_TZPCDECPROT_2_SET_BASE			(NXP_TZPC_BASE + 0x81C)
#define NXP_TZPCDECPROT_2_CLR_BASE			(NXP_TZPC_BASE + 0x820)

/* NXP_TZ Address Space Controller Definitions */
#define NXP_TZASC1_BASE					0x01100000	/* as per CCSR map. */
#define NXP_TZASC2_BASE					0x01110000	/* as per CCSR map. */
#define NXP_TZASC3_BASE					0x01120000	/* as per CCSR map. */
#define NXP_TZASC4_BASE					0x01130000	/* as per CCSR map. */
#define NXP_TZASC_BUILD_CONFIG_REG(x)			((NXP_TZASC1_BASE + (x * 0x10000)))
#define NXP_TZASC_ACTION_REG(x)				((NXP_TZASC1_BASE + (x * 0x10000)) + 0x004)
#define NXP_TZASC_GATE_KEEPER(x)			((NXP_TZASC1_BASE + (x * 0x10000)) + 0x008)
#define NXP_TZASC_REGION_BASE_LOW_0(x)			((NXP_TZASC1_BASE + (x * 0x10000)) + 0x100)
#define NXP_TZASC_REGION_BASE_HIGH_0(x)		((NXP_TZASC1_BASE + (x * 0x10000)) + 0x104)
#define NXP_TZASC_REGION_TOP_LOW_0(x)			((NXP_TZASC1_BASE + (x * 0x10000)) + 0x108)
#define NXP_TZASC_REGION_TOP_HIGH_0(x)			((NXP_TZASC1_BASE + (x * 0x10000)) + 0x10C)
#define NXP_TZASC_REGION_ATTRIBUTES_0(x)		((NXP_TZASC1_BASE + (x * 0x10000)) + 0x110)
#define NXP_TZASC_REGION_ID_ACCESS_0(x)		((NXP_TZASC1_BASE + (x * 0x10000)) + 0x114)


#define GUTS_ADDR					0x01E00000
#define FSL_LSCH3_SVR				(GUTS_ADDR + 0xA4)

#define SVR_DEV_LS2088             		0x8701

.text
.align 3

GCC_ASM_EXPORT(_ModuleEntryPoint)

StartupAddr:        .8byte ASM_PFX(CEntryPoint)
DramInitAddr:       .8byte ASM_PFX(DramInit)
RelocAddr:          .8byte FixedPcdGet64(PcdFdBaseAddress)

SvrDevId:
	ldr	x1, =FSL_LSCH3_SVR
	ldr	w0, [x1]
	lsr	w0, w0, #16
	ret

ASM_PFX(_ModuleEntryPoint):

	LoadConstantToReg(FixedPcdGet32(PcdArmErrata828024), x1)
	cbz x1, 1f

	mrs	x1, S3_1_c15_c2_0
	orr	x1, x1, #1 << 49
	orr	x1, x1, #3 << 25
	orr	x1, x1, #3 << 27
	msr	S3_1_c15_c2_0, x1

1:
	LoadConstantToReg(FixedPcdGet32(PcdArmErrata826974), x1)
	cbz x1, 2f

	mrs	x1, S3_1_c15_c2_0
	orr	x1, x1, #1 << 59
	msr	S3_1_c15_c2_0, x1

2:
	LoadConstantToReg(FixedPcdGet32(PcdArmErrata833471), x1)
	cbz x1, 3f

	mrs	x1, S3_1_c15_c2_0
	orr	x1, x1, #1 << 38
	msr	S3_1_c15_c2_0, x1

3:
	LoadConstantToReg(FixedPcdGet32(PcdArmErrata829520), x1)
	cbz x1, 4f

	mrs	x1, S3_1_c15_c2_0
	orr	x1, x1, #1 << 4
	msr	S3_1_c15_c2_0, x1

4:
	LoadConstantToReg(FixedPcdGet32(PcdArmErrata829520), x1)
	cbz x1, 5f

	mrs	x1, S3_1_c15_c2_0
	and	x1, x1, #0xE
	msr	S3_1_c15_c2_0, x1

5:

/** NXP_TZPC_BP147 configuration */
	/* Set Non Secure access for all devices protected via NXP_TZPC */
	ldr	x1, =NXP_TZPCDECPROT_0_SET_BASE /* Decode Protection-0 Set Reg */
	orr	w0, w0, #1 << 3 /* DCFG_RESET is accessible from NS world */
	str	w0, [x1]

	isb
	dsb	sy
/* NXP_TZPC_BP147 configuration ends here*/

/** NXP_TZASC_400 configuration */
	/* Set NXP_TZASC so that:
	 * a. We use only Region0 whose global secure write/read is EN
	 * b. We use only Region0 whose NSAID write/read is EN
	 *
	 * NOTE: As per the CCSR map doc, NXP_TZASC 3 and NXP_TZASC 4 are just
	 * 	 placeholders.
	 */

	bl	SvrDevId		/* get high 16 bits of SVR */
	/*
	 * Skip NXP_TZASC related operations for LS2088 and its personlaities
	 */
	ldr	x1, =SVR_DEV_LS2088
	cmp	x0, x1
	b.eq	6f

/** NXP_TZASC_1 configuration */
	ldr	x1, =NXP_TZASC_GATE_KEEPER(0)
	ldr	w0, [x1]		/* Filter 0 Gate Keeper Register */
	orr	w0, w0, #1 << 0		/* Set open_request for Filter 0 */
	str	w0, [x1]

	ldr	x1, =NXP_TZASC_REGION_ATTRIBUTES_0(0)
	ldr	w0, [x1]		/* Region-0 Attributes Register */
	orr	w0, w0, #1 << 31	/* Set Sec global write en, Bit[31] */
	orr	w0, w0, #1 << 30	/* Set Sec global read en, Bit[30] */
	str	w0, [x1]

	ldr	x1, =NXP_TZASC_REGION_ID_ACCESS_0(0)
	ldr	w0, [x1]		/* Region-0 Access Register */
	mov	w0, #0xFFFFFFFF		/* Set nsaid_wr_en and nsaid_rd_en */
	str	w0, [x1]
/* NXP_TZASC_1 configuration ends here */
/** NXP_TZASC_2 configuration */
	ldr	x1, =NXP_TZASC_GATE_KEEPER(1)
	ldr	w0, [x1]		/* Filter 0 Gate Keeper Register */
	orr	w0, w0, #1 << 0		/* Set open_request for Filter 0 */
	str	w0, [x1]

	ldr	x1, =NXP_TZASC_REGION_ATTRIBUTES_0(1)
	ldr	w0, [x1]		/* Region-1 Attributes Register */
	orr	w0, w0, #1 << 31	/* Set Sec global write en, Bit[31] */
	orr	w0, w0, #1 << 30	/* Set Sec global read en, Bit[30] */
	str	w0, [x1]

	ldr	x1, =NXP_TZASC_REGION_ID_ACCESS_0(1)
	ldr	w0, [x1]		/* Region-1 Attributes Register */
	mov	w0, #0xFFFFFFFF		/* Set nsaid_wr_en and nsaid_rd_en */
	str	w0, [x1]
/* NXP_TZASC_2 configuration ends here */
	isb
	dsb	sy
/* NXP_TZASC_400 configuration ends here*/

6:
       LoadConstantToReg (FixedPcdGet32(PcdOcramStackBase), x0)
	mov sp, x0
	LoadConstantToReg (FixedPcdGet64(PcdFdBaseAddress), x0)
	LoadConstantToReg (FixedPcdGet32(PcdFdNorBaseAddress), x1)
	LoadConstantToReg (FixedPcdGet32(PcdPiFdSize), x5)
	add x1, x1, x5
	LoadConstantToReg (FixedPcdGet32(PcdFdSize), x2)
	ldr x4, StartupAddr
	blr x4
  
