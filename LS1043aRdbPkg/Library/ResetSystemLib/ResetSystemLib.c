/** ResetSystemLib.c
  Do a generic Cold Reset for LS1043a

  Based on Reset system library implementation in
  BeagleBoardPkg/Library/ResetSystemLib/ResetSystemLib.c
  
  Copyright (c) 2008 - 2010, Apple Inc. All rights reserved.<BR>
  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.
  
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <PiDxe.h>
#include <Uefi.h>

#include <IndustryStandard/ArmStdSmc.h>
#include <Library/ArmLib.h>
#include <Library/ArmSmcLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <LS1043aRdb.h>

STATIC EFI_EVENT              mResetVirtualAddrChangeEvent;
STATIC UINTN                  mResetBaseAddress;

VOID
ShutdownEfi (
  VOID
  )
{
  EFI_STATUS              Status;
  UINTN                   MemoryMapSize;
  EFI_MEMORY_DESCRIPTOR   *MemoryMap;
  UINTN                   MapKey;
  UINTN                   DescriptorSize;
  UINT32                   DescriptorVersion;
  UINTN                   Pages;

  MemoryMap = NULL;
  MemoryMapSize = 0;
  do {
    Status = gBS->GetMemoryMap (
                    &MemoryMapSize,
                    MemoryMap,
                    &MapKey,
                    &DescriptorSize,
                    &DescriptorVersion
                    );
    if (Status == EFI_BUFFER_TOO_SMALL) {

      Pages = EFI_SIZE_TO_PAGES (MemoryMapSize) + 1;
      MemoryMap = AllocatePages (Pages);
    
      //
      // Get System MemoryMap
      //
      Status = gBS->GetMemoryMap (
                      &MemoryMapSize,
                      MemoryMap,
                      &MapKey,
                      &DescriptorSize,
                      &DescriptorVersion
                      );
      // Don't do anything between the GetMemoryMap() and ExitBootServices()
      if (!EFI_ERROR (Status)) {
        Status = gBS->ExitBootServices (gImageHandle, MapKey);
        if (EFI_ERROR (Status)) {
          FreePages (MemoryMap, Pages);
          MemoryMap = NULL;
          MemoryMapSize = 0;
        }
      }
    }
  } while (EFI_ERROR (Status));

  //Clean and invalidate caches.
  WriteBackInvalidateDataCache();
  InvalidateInstructionCache();

  //Turning off Caches and MMU
  ArmDisableDataCache ();
  ArmDisableInstructionCache ();
  ArmDisableMmu ();
}

typedef
VOID
(EFIAPI *CALL_STUB)(
  VOID
);


/**
  Resets the entire platform.

  @param  ResetType             The type of reset to perform.
  @param  ResetStatus           The status code for the reset.
  @param  DataSize              The size, in bytes, of WatchdogData.
  @param  ResetData             For a ResetType of EfiResetCold, EfiResetWarm, or
                                EfiResetShutdown the data buffer starts with a Null-terminated
                                Unicode string, optionally followed by additional binary data.

**/
EFI_STATUS
EFIAPI
LibResetSystem (
  IN EFI_RESET_TYPE   ResetType,
  IN EFI_STATUS       ResetStatus,
  IN UINTN            DataSize,
  IN CHAR16           *ResetData OPTIONAL
  )
{
  ARM_SMC_ARGS ArmSmcArgs;
  
  if (ResetData != NULL) {
    DEBUG((EFI_D_ERROR, "%s", ResetData));
  }

  switch (ResetType) {
  case EfiResetPlatformSpecific:
  case EfiResetWarm:
    // Map a warm reset into a cold reset
  case EfiResetCold:
    // Send a PSCI 0.2 SYSTEM_RESET command
    ArmSmcArgs.Arg0 = ARM_SMC_ID_PSCI_SYSTEM_RESET;
    break;
  case EfiResetShutdown:
    // Send a PSCI 0.2 SYSTEM_OFF command
    ArmSmcArgs.Arg0 = ARM_SMC_ID_PSCI_SYSTEM_OFF;
    break;
  default:
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  ArmCallSmc (&ArmSmcArgs);

  // We should never be here
  DEBUG ((EFI_D_VERBOSE, "%a: PSCI failed in performing %d\n",
                            __FUNCTION__, ArmSmcArgs.Arg0));
  CpuDeadLoop ();

  return EFI_UNSUPPORTED;
}
  
/**
  Fixup internal data so that EFI can be call in virtual mode.
  Call the passed in Child Notify event and convert any pointers in
  lib to virtual mode.

  @param[in]    Event   The Event that is being processed
  @param[in]    Context Event Context
**/
VOID
EFIAPI
LibResetVirtualNotifyEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  //
  // Only needed if you are going to support the OS calling RTC functions in virtual mode.
  // You will need to call EfiConvertPointer (). To convert any stored physical addresses
  // to virtual address. After the OS transistions to calling in virtual mode, all future
  // runtime calls will be made in virtual mode.
  //
  EfiConvertPointer (0x0, (VOID**)&mResetBaseAddress);
  return;
}  

/**
  Initialize any infrastructure required for LibResetSystem () to function.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
LibInitializeResetSystem (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
	
  //
  // Do some initialization
  //
  EFI_STATUS    Status;
  
  mResetBaseAddress = RESET_BASE_ADDR;
  UINTN mResetSize = RESET_SIZE;
  
  // Declare the controller as EFI_MEMORY_RUNTIME
  Status = gDS->AddMemorySpace (
                  EfiGcdMemoryTypeMemoryMappedIo,
                  mResetBaseAddress, mResetSize,
                  EFI_MEMORY_UC | EFI_MEMORY_RUNTIME
                  );
  if (EFI_ERROR (Status))
    return Status;

  Status = gDS->SetMemorySpaceAttributes (mResetBaseAddress, mResetSize, EFI_MEMORY_UC | EFI_MEMORY_RUNTIME);
  if (EFI_ERROR (Status))
    return Status;

  //
  // Register for the virtual address change event
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  LibResetVirtualNotifyEvent,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mResetVirtualAddrChangeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;

}

