#  @file
#
#  Based on BeagleBoardPkg/Library/BeagleBoardLib/BeagleBoardHelper.S 
#
#  Copyright (c) 2012-2013, ARM Limited. All rights reserved.
#  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.
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

#include <AsmMacroIoLib.h>
#include <AsmMacroIoLibV8.h>
#include <AutoGen.h>
#include <LS2088aRdb.h>

/* GICv3 specific constant */

 // Base Address
.equ NXP_GICD_BASE_ADDR,     0x06000000
.equ GICR_BASE_ADDR,     0x06100000

 // register offsets

/* Distributor Registers */
#define NXP_GICD_CTLR		0x0000
#define NXP_GICD_TYPER		0x0004
#define NXP_GICD_IIDR		0x0008
#define NXP_GICD_STATUSR		0x0010
#define NXP_GICD_SETSPI_NSR		0x0040
#define NXP_GICD_CLRSPI_NSR		0x0048
#define NXP_GICD_SETSPI_SR		0x0050
#define NXP_GICD_CLRSPI_SR		0x0058
#define NXP_GICD_SEIR		0x0068
#define NXP_GICD_IGROUPRn		0x0080
#define NXP_GICD_ISENABLERn		0x0100
#define NXP_GICD_ICENABLERn		0x0180
#define NXP_GICD_ISPENDRn		0x0200
#define NXP_GICD_ICPENDRn		0x0280
#define NXP_GICD_ISACTIVERn		0x0300
#define NXP_GICD_ICACTIVERn		0x0380
#define NXP_GICD_IPRIORITYRn	0x0400
#define NXP_GICD_ITARGETSRn		0x0800
#define NXP_GICD_ICFGR		0x0c00
#define NXP_GICD_IGROUPMODRn	0x0d00
#define NXP_GICD_NSACRn		0x0e00
#define NXP_GICD_SGIR		0x0f00
#define NXP_GICD_CPENDSGIRn		0x0f10
#define NXP_GICD_SPENDSGIRn		0x0f20
#define NXP_GICD_IROUTERn		0x6000

.text
.align 2

GCC_ASM_EXPORT(ArmPlatformIsPrimaryCore)
GCC_ASM_EXPORT(ArmPlatformGetPrimaryCoreMpId)
GCC_ASM_EXPORT(ArmPlatformPeiBootAction)

GCC_ASM_IMPORT(ArmReadMpidr)
GCC_ASM_IMPORT(_gPcd_FixedAtBuild_PcdArmPrimaryCore)
GCC_ASM_IMPORT(_gPcd_FixedAtBuild_PcdArmPrimaryCoreMask)
GCC_ASM_IMPORT(_gPcd_FixedAtBuild_PcdCoreCount)
GCC_ASM_IMPORT(Ccn504AddMastersToDvm)
GCC_ASM_IMPORT(Ccn504SetQos)

//DramInitAddr:	.8byte ASM_PFX(DramInit)
//UINTN
//ArmPlatformIsPrimaryCore (
//  IN UINTN MpId
//  );
ASM_PFX(ArmPlatformIsPrimaryCore):
	and x1, x0, #3
	mov	x0, #0
  cbnz x1, 1f
	mov x0, #1
1:
  ret

ASM_PFX(ArmPlatformPeiBootAction):
//Push return address to the stack
sub  sp, sp, #16
stur  x30, [sp, #0]

EL1_OR_EL2_OR_EL3(x0)	
1:
2:
	ret
3:
	LoadConstantToReg(FixedPcdGet32(PcdCounterFrequency), x0)
	msr cntfrq_el0, x0 

	LoadConstantToReg(FixedPcdGet32(PcdArmErrata828024), x0)
	cbz x0, 4f

	mrs x0, S3_1_c15_c2_0 /* cpuactlr_el1 */
	/* Disable non-allocate hint of w-b-n-a memory type */
	orr x0, x0, #1 << 49
	/* Disable write streaming no L1-allocate threshold */
	orr x0, x0, #3 << 25
	/* Disable write streaming no-allocate threshold */
	orr x0, x0, #3 << 27
	msr S3_1_c15_c2_0, x0 /* cpuactlr_el1 */

4:
	LoadConstantToReg(FixedPcdGet32(PcdArmErrata826974), x0)
	cbz x0, 5f

	mrs x0, S3_1_c15_c2_0 /* cpuactlr_el1 */
	/* Disable speculative load execution ahead of a DMB */
	orr x0, x0, #1 << 59
	msr S3_1_c15_c2_0, x0 /* cpuactlr_el1 */

5:
	/* Set Wuo bit for RN-I 20 */
	ldr	x0, =CCI_AUX_CONTROL_BASE(20)
	ldr	x1, =0x00000010
	bl	Ccn504SetAux

	 /* Enable L3 cache here */
        ldr     x0, =CCI_MN_BASE_ADDR
        ldr     x1, =CCI_MN_RNF_NODEID_LIST_OFFSET
        ldr     x2, =CCI_MN_DVM_DOMAIN_CTL_SET
        bl      Ccn504AddMastersToDvmDomain

        /* Set all RN-I ports to QoS: 15 */
        ldr     x0, =CCI_S0_QOS_CNTRL_BASE(0)
        ldr     x1, =0x00FF000C
        bl      Ccn504SetQosVal
        ldr     x0, =CCI_S1_QOS_CNTRL_BASE(0)
        ldr     x1, =0x00FF000C
        bl      Ccn504SetQosVal
        ldr     x0, =CCI_S2_QOS_CNTRL_BASE(0)
        ldr     x1, =0x00FF000C
        bl      Ccn504SetQosVal

        ldr     x0, =CCI_S0_QOS_CNTRL_BASE(2)
        ldr     x1, =0x00FF000C
        bl      Ccn504SetQosVal
        ldr     x0, =CCI_S1_QOS_CNTRL_BASE(2)
        ldr     x1, =0x00FF000C
        bl      Ccn504SetQosVal
        ldr     x0, =CCI_S2_QOS_CNTRL_BASE(2)
        ldr     x1, =0x00FF000C
        bl      Ccn504SetQosVal

        ldr     x0, =CCI_S0_QOS_CNTRL_BASE(6)
        ldr     x1, =0x00FF000C
        bl      Ccn504SetQosVal
        ldr     x0, =CCI_S1_QOS_CNTRL_BASE(6)
        ldr     x1, =0x00FF000C
        bl      Ccn504SetQosVal
        ldr     x0, =CCI_S2_QOS_CNTRL_BASE(6)
        ldr     x1, =0x00FF000C
        bl      Ccn504SetQosVal

        ldr     x0, =CCI_S0_QOS_CNTRL_BASE(12)
        ldr     x1, =0x00FF000C
        bl      Ccn504SetQosVal
        ldr     x0, =CCI_S1_QOS_CNTRL_BASE(12)
        ldr     x1, =0x00FF000C
        bl      Ccn504SetQosVal
        ldr     x0, =CCI_S2_QOS_CNTRL_BASE(12)
        ldr     x1, =0x00FF000C
        bl      Ccn504SetQosVal

        ldr     x0, =CCI_S0_QOS_CNTRL_BASE(16)
        ldr     x1, =0x00FF000C
        bl      Ccn504SetQosVal
        ldr     x0, =CCI_S1_QOS_CNTRL_BASE(16)
        ldr     x1, =0x00FF000C
        bl      Ccn504SetQosVal
        ldr     x0, =CCI_S2_QOS_CNTRL_BASE(16)
	ldr     x1, =0x00FF000C
        bl      Ccn504SetQosVal

        ldr     x0, =CCI_S0_QOS_CNTRL_BASE(20)
        ldr     x1, =0x00FF000C
        bl      Ccn504SetQosVal
        ldr     x0, =CCI_S1_QOS_CNTRL_BASE(20)
        ldr     x1, =0x00FF000C
        bl      Ccn504SetQosVal
        ldr     x0, =CCI_S2_QOS_CNTRL_BASE(20)
        ldr     x1, =0x00FF000C
        bl      Ccn504SetQosVal
        
	// Configure NXP_GICD_* registers to enable SPI interrupts
	// for Grp1NS (OS, Hyp)

	// get the base address of the gic distributor
	ldr	x0, =NXP_GICD_BASE_ADDR

	mov	w9, #0x37		/* EnableGrp0 | EnableGrp1NS */
					/* EnableGrp1S | ARE_S | ARE_NS */
	str	w9, [x0, NXP_GICD_CTLR]	/* Secure NXP_GICD_CTLR */
	ldr	w9, [x0, NXP_GICD_TYPER]
	and	w10, w9, #0x1f		/* ITLinesNumber */
	cbz	w10, 7f			/* No SPIs */
	add	x11, x0, (NXP_GICD_IGROUPRn + 4)
	add	x12, x0, (NXP_GICD_IGROUPMODRn + 4)
	mov	w9, #~0
6:	str	w9, [x11], #0x4
	str	wzr, [x12], #0x4	/* Config SPIs as Group1NS */
	sub	w10, w10, #0x1
	cbnz	w10, 6b
7:
	ldur	x30, [sp, #0]
	add	sp, sp, #16
	ret

//UINTN
//ArmPlatformGetPrimaryCoreMpId (
//  VOID
//  );
ASM_PFX(ArmPlatformGetPrimaryCoreMpId):
  LoadConstantToReg (_gPcd_FixedAtBuild_PcdArmPrimaryCore, x0)
  ldrh   w0, [x0]
  ret

ASM_FUNCTION_REMOVE_IF_UNREFERENCED 
