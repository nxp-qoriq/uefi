/** @IfcNorLib.c

 Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

 **/

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/NorFlashPlatformLib.h>
#include <Library/FslIfc.h>

UINT32 NorCs;

/*
  Tune IFC NOR timings.
*/
VOID
IfcNorSetMemctlRegs (
  VOID
  )
{
  MmioWrite32((UINTN) &(FSL_IFC_REGS_BASE)->cspr_cs[NorCs].cspr_ext, FSL_IFC_NOR_CSPR_EXT);

  MmioWrite32((UINTN) &(FSL_IFC_REGS_BASE)->ftim_cs[NorCs].ftim[FSL_IFC_FTIM0], FSL_IFC_NOR_FTIM0);
  MmioWrite32((UINTN) &(FSL_IFC_REGS_BASE)->ftim_cs[NorCs].ftim[FSL_IFC_FTIM1], FSL_IFC_NOR_FTIM1);
  MmioWrite32((UINTN) &(FSL_IFC_REGS_BASE)->ftim_cs[NorCs].ftim[FSL_IFC_FTIM2], FSL_IFC_NOR_FTIM2);
  MmioWrite32((UINTN) &(FSL_IFC_REGS_BASE)->ftim_cs[NorCs].ftim[FSL_IFC_FTIM3], FSL_IFC_NOR_FTIM3);

  MmioWrite32((UINTN) &(FSL_IFC_REGS_BASE)->cspr_cs[NorCs].cspr, FSL_IFC_NOR_CSPR0);
  MmioWrite32((UINTN) &(FSL_IFC_REGS_BASE)->amask_cs[NorCs].amask, FSL_IFC_NOR_AMASK0);
  MmioWrite32((UINTN) &(FSL_IFC_REGS_BASE)->csor_cs[NorCs].csor, FSL_IFC_NOR_CSOR0);
}

/*
  Init IFC NOR.
*/
VOID
IfcNorInit (
  VOID
  )
{
	if(PcdGet32(PcdBootMode) == NOR_BOOT)
		NorCs = FSL_IFC_CS0;
	else
		NorCs = FSL_IFC_CS1;
  // Tune IFC NOR timings.
  IfcNorSetMemctlRegs ();
}

