/** @LS1046aPrePiOcram.c
#
#  Driver for installing BlockIo protocol over IFC NAND
#
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
**/

#include <Library/ArmLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/SerialPortLib.h>
#include <LS1046aRdb.h>
#include <Ddr.h>

UINTN mGlobalVariableBase = 0;

VOID
LoadImageToDdr(
  IN   UINTN  UefiMemoryBase
  );

VOID CEntryPoint(
		UINTN	UefiMemoryBase
		)
{ 
  // Data Cache enabled on Primary core when MMU is enabled.
  ArmDisableDataCache ();
  // Invalidate Data cache
  ArmInvalidateDataCache ();
  // Invalidate instruction cache
  ArmInvalidateInstructionCache ();
  // Enable Instruction Caches on all cores.
  ArmEnableInstructionCache ();

  DramInit();
  SerialPortInitialize();

  DEBUG((EFI_D_RELEASE, "\nUEFI primary boot firmware (built at %a on %a)\n", __TIME__, __DATE__));

  if((PcdGet32(PcdBootMode) == NAND_BOOT) || (PcdGet32(PcdBootMode) == SD_BOOT)) {
	LoadImageToDdr(UefiMemoryBase);
  }  else {
	DEBUG((EFI_D_ERROR, "Unsupported boot mode\n"));
	ASSERT(0);
  }
}
