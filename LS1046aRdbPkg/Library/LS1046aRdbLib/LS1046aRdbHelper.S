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

.text
.align 2

GCC_ASM_EXPORT(ArmPlatformIsPrimaryCore)
GCC_ASM_EXPORT(ArmPlatformGetPrimaryCoreMpId)
GCC_ASM_EXPORT(ArmPlatformPeiBootAction)

GCC_ASM_IMPORT(ArmReadMpidr)
GCC_ASM_IMPORT(_gPcd_FixedAtBuild_PcdArmPrimaryCore)
GCC_ASM_IMPORT(_gPcd_FixedAtBuild_PcdArmPrimaryCoreMask)
GCC_ASM_IMPORT(_gPcd_FixedAtBuild_PcdCoreCount)


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
EL1_OR_EL2_OR_EL3(x0)	
1:
2:
	ret
3:
	LoadConstantToReg(FixedPcdGet32(PcdCounterFrequency), x0)
	msr cntfrq_el0, x0 

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
