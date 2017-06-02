/** @file
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

#include <Chipset/AArch64.h>
#include <Library/ArmPlatformLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PrintLib.h>
#include <Library/BdsLinuxFit.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib/MemLibInternals.h>
#include <Library/FslIfc.h>
#include <LS2088aRdb.h>

extern EFI_STATUS PpaInit(UINTN *);
extern VOID InitMmu(ARM_MEMORY_REGION_DESCRIPTOR*);
extern VOID DCacheCleanAllLevels(VOID);

/**
 * Copy PPA firmware to DDR
 */
VOID
CopyPpaImage (
  const char *title,
  UINTN image_addr,
  UINTN image_size,
  UINTN *PpaRamAddr)
{
	DEBUG((EFI_D_RELEASE, "%a copied to address 0x%llx\n", title, PpaRamAddr));
  	InternalMemCopyMem((void *)PpaRamAddr, (void *)image_addr, image_size);
}

VOID
GetPpaImagefromFlash (
  UINTN *PpaRamAddr
  )
{
	EFI_STATUS Status;
	EFI_PHYSICAL_ADDRESS FitImage;
	EFI_PHYSICAL_ADDRESS PpaImage;
	INT32 CfgNodeOffset;
	INT32 NodeOffset;
	INT32 PpaImageSize;

	// Assuming that the PPA FW is present on NOR flash
	// FIXME: Add support for other flash devices.

	if(PcdGet32(PcdBootMode) != QSPI_BOOT)
	  FitImage = PcdGet64 (PcdPpaNorBaseAddr);
	else
	  FitImage = PcdGet64 (PcdPpaQspiBaseAddr);

	/* Check if we have a valid PPA firmware FIT image */
	Status = FitCheckHeader(FitImage);
	if (EFI_ERROR (Status)) {
		DEBUG((EFI_D_ERROR, "Bad FIT image header (0x%x).\n", Status));
		goto EXIT_FREE_FIT;
	}

	Status = FitGetConfNode(FitImage, (void *)(PcdGetPtr(PcdPpaFitConfiguration)), &CfgNodeOffset);
	if (EFI_ERROR (Status)) {
		DEBUG((EFI_D_ERROR, "Did not find configuration node in FIT header (0x%x).\n", Status));
		goto EXIT_FREE_FIT;
	}

	Status = FitGetNodeFromConf(FitImage, CfgNodeOffset, FIT_FIRMWARE_IMAGE, &NodeOffset);
	if (EFI_ERROR (Status)) {
		DEBUG((EFI_D_ERROR, "Did not find PPA node in FIT header (0x%x).\n", Status));
		goto EXIT_FREE_FIT;
	}

	Status = FitGetNodeData(FitImage, NodeOffset, (VOID*)&PpaImage, &PpaImageSize);
	if (EFI_ERROR (Status)) {
		DEBUG((EFI_D_ERROR, "Did not find PPA f/w in FIT image (0x%x).\n", Status));
		goto EXIT_FREE_FIT;
	}

	// PPA will be placed on DDR at address PpaRamAddr
	CopyPpaImage ("PPA Firmware", PpaImage, PpaImageSize, PpaRamAddr);

	return;

EXIT_FREE_FIT:
	// Flow should never reach here
	ASSERT (Status == EFI_SUCCESS);

	return;
}

EFI_STATUS
PpaInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
	EFI_STATUS Status;
	UINTN *PpaRamAddr;
	ARM_MEMORY_REGION_DESCRIPTOR *MemoryTable = NULL;
	UINTN PpaDdrSize = PcdGet64(PcdPpaDdrOffsetAddr);

	/* Allocate a DDR region to house the PPA firmware:
	 * Note that PPA requires a 64KB aligned start address on DDR.
	 * Zero'ize the allocated region.
	 */
	PpaRamAddr = (UINTN *)AllocateAlignedRuntimePages((PpaDdrSize / (4 * 1024)) /* no of 4 KB pages to allocate */,
							  (64 * 1024 ) /* Alignment = 64 KB*/);
	PpaRamAddr = ZeroMem (PpaRamAddr, (PpaDdrSize));

	/* Copy the PPA firmware from Flash to DDR */
	GetPpaImagefromFlash(PpaRamAddr);

	/* Clean D-cache (all levels) and Invalidate I-cache */
	DCacheCleanAllLevels ();
	ArmInvalidateInstructionCache ();

	/* Call PPA firmware */
	Status = PpaInit(PpaRamAddr);

	/* Now that we are back in EL2 mode, setup the MMU and
	 * caches again for EL2 mode
	 */
	ArmPlatformGetVirtualMemoryMap (&MemoryTable);
	InitMmu(MemoryTable);

	return Status;
}
