/** @LS1046aPrePiSd.c
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
#include <Library/Sdxc.h>
#include <Library/IoLib.h>
#include <Library/SerialPortLib.h>
#include <LS1046aRdb.h>


EFI_STATUS UefiFdSdToDdr(
	UINTN	Lba,
	UINTN  FdSize,
	UINTN	DdrDestAddress
)
{
  EFI_STATUS Status;

  Status = SdxcBootInit(FALSE);
  if (Status != 0) {
    DEBUG((EFI_D_ERROR,"Failed to init SD\n"));
    return Status;
  }

  //Copy from SD to DDR
  return SdxcBootRead((VOID*)DdrDestAddress, Lba, FdSize);  	
}


VOID
LoadImageToDdr (
  IN  UINTN  UefiMemoryBase
  )
{ 
  //ARM_MEMORY_REGION_DESCRIPTOR  MemoryTable[5];
  VOID	(*PrePiStart)(VOID);

  DEBUG((EFI_D_RELEASE, "Loading secondary firmware from SD.....\n"));
  if (UefiFdSdToDdr(FixedPcdGet32(PcdFdSdxcLba), FixedPcdGet32(PcdFdSize), UefiMemoryBase)) {
    DEBUG((EFI_D_ERROR, "Failed to load secondary boot firmware....\n"));
    ASSERT(0);
  }

  DEBUG((EFI_D_RELEASE, "Starting secondary boot firmware....\n"));
  PrePiStart = (VOID (*)())UefiMemoryBase;
  PrePiStart();

  ASSERT(0);
}
