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

#define CONFIG_OCRAM1_BASE_ADDR           0x10000000
#define CONFIG_OCRAM_END                  0x10020000

#define DCSR_DCFG_SBEESR2                 0x20140534
#define DCSR_DCFG_MBEESR2                 0x20140544

.text
.align 3

GCC_ASM_EXPORT(_ModuleEntryPoint)

StartupAddr:        .8byte ASM_PFX(CEntryPoint)

ClearOcramEccErr:
       /* OCRAM1/2 ECC status bit */
       mov    w1, #0x60
       ldr    x0, =DCSR_DCFG_SBEESR2
       str    w1, [x0]
       ldr    x0, =DCSR_DCFG_MBEESR2
       str    w1, [x0]
       ret

ClearOcram:
/* Clear OCRAM */
       ldr    x0, =CONFIG_OCRAM1_BASE_ADDR
       ldr    x1, =CONFIG_OCRAM_END
       mov    x2, #0
Clear:
       str    x2, [x0]
       add    x0, x0, #8
       cmp    x0, x1
       b.lo   Clear
       ret

InitOcram:
       mov    x28, x30                    /* Save LR */
       bl     ClearOcram
       bl     ClearOcramEccErr
       mov    x30, x28                     /* Restore LR */
       ret

ASM_PFX(_ModuleEntryPoint):
	bl     InitOcram

        /* Initialize the L2 RAM latency */
        mrs   x1, S3_1_c11_c0_2
        mov   x0, #0x1C7
        /* Clear L2 Tag RAM latency and L2 Data RAM latency */
        bic   x1, x1, x0
        /* Set L2 data ram latency bits [2:0] */
        orr   x1, x1, #0x2
        /* set L2 tag ram latency bits [8:6] */
        orr   x1,  x1, #0x80
        msr   S3_1_c11_c0_2, x1
        isb
	 
       LoadConstantToReg (FixedPcdGet32(PcdOcramStackBase), x0)
	mov sp, x0

	LoadConstantToReg (FixedPcdGet64(PcdFdBaseAddress), x0)
	LoadConstantToReg (FixedPcdGet32(PcdFdNorBaseAddress), x1)
	LoadConstantToReg (FixedPcdGet32(PcdPiFdSize), x5)
	add x1, x1, x5
	LoadConstantToReg (FixedPcdGet32(PcdFdSize), x2)
	ldr x4, StartupAddr
	blr x4
  
