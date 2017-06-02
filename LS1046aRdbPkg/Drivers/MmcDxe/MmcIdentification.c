/** @MmcIdentification.c

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/TimerLib.h>
#include <Drivers/Mmc.h>


STATIC
EFI_STATUS
EFIAPI
MmcIdentificationMode (
  IN MMC_HOST_INSTANCE     *MmcHostInstance
  )
{
  EFI_STATUS              Status;
  EFI_MMC_HOST_PROTOCOL   *MmcHost;

  MmcHost = MmcHostInstance->MmcHost;

  if (MmcHost == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  // We can get into this function if we restart the identification mode
  if (MmcHostInstance->State == MmcHwInitializationState) {
    // Initialize the MMC Host HW
    Status = MmcNotifyState (MmcHostInstance, MmcHwInitializationState);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "MmcIdentificationMode() : Error MmcHwInitializationState, Status=%r.\n", Status));
      return Status;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
InitializeMmcDevice (
  IN  MMC_HOST_INSTANCE  *MmcHostInstance
  )
{
  EFI_STATUS              Status;

  Status = MmcIdentificationMode (MmcHostInstance);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "InitializeMmcDevice(): Error in Identification Mode, Status=%r\n", Status));
    return Status;
  }

  Status = MmcNotifyState (MmcHostInstance, MmcTransferState);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "InitializeMmcDevice(): Error MmcTransferState, Status=%r\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}
