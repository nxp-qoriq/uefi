/** @Mmc.c
  Main file of the MMC Dxe driver. The driver entrypoint is defined into this file.

  Based on Mmc implementation available in EmbeddedPkg/Universal/MmcDxe/Mmc.c

  Copyright (c) 2011-2013, ARM Limited. All rights reserved.
  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Protocol/DevicePath.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Drivers/Mmc.h>
#include <Protocol/MmcHost.h>

MMC_DEVICE_PATH gDevicePath = {
  {
    { HARDWARE_DEVICE_PATH, HW_VENDOR_DP, { (UINT8)sizeof(VENDOR_DEVICE_PATH), (UINT8)((sizeof(VENDOR_DEVICE_PATH)) >> 8) } },
    EFI_CALLER_ID_GUID
  },
  { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE,
	  { sizeof (EFI_DEVICE_PATH_PROTOCOL), 0}
  }
};

EFI_BLOCK_IO_MEDIA mMmcMediaTemplate = {
  SIGNATURE_32('m','m','c','o'),            // MediaId
  TRUE,                                     // RemovableMedia
  FALSE,                                    // MediaPresent
  FALSE,                                    // LogicalPartition
  FALSE,                                    // ReadOnly
  FALSE,                                    // WriteCaching
  512,                                      // BlockSize
  4,                                        // IoAlign
  0,                                        // Pad
  0                                         // LastBlock
};

MMC_HOST_INSTANCE       *gMmcHostInstance = NULL;

/**
  Event triggered by the timer to check if any cards have been removed
  or if new ones have been plugged in
**/

EFI_EVENT gCheckCardsEvent;

/**
  Initialize the MMC Host Pool to support multiple MMC devices
**/
VOID
InitializeMmcHostPool (
  VOID
  )
{
  InitializeListHead (&mMmcHostPool);
}

/**
  Insert a new Mmc Host controller to the pool
**/
VOID
InsertMmcHost (
  IN MMC_HOST_INSTANCE      *MmcHostInstance
  )
{
  InsertTailList (&mMmcHostPool, &(MmcHostInstance->Link));
}

VOID
CreateMmcHostProtocol (
  IN  EFI_MMC_HOST_PROTOCOL *MmcHost
  )
{
  MmcHost->Revision = MMC_HOST_PROTOCOL_REVISION;
  MmcHost->IsCardPresent = MMCIsCardPresent;
  MmcHost->IsReadOnly = MMCIsReadOnly;
  MmcHost->BuildDevicePath = MMCBuildDevicePath;
  MmcHost->NotifyState = MMCNotifyState;
  MmcHost->SendCommand = MMCSendCommand;
  MmcHost->ReceiveResponse = MMCReceiveResponse;
  MmcHost->ReadBlockData = MMCReadBlockData;
  MmcHost->WriteBlockData = MMCWriteBlockData;
}

EFI_STATUS
CreateMmcHostInstance (
  IN EFI_MMC_HOST_PROTOCOL* MmcHost
  )
{
  EFI_DEVICE_PATH_PROTOCOL    *NewDevicePathNode = NULL;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  EFI_STATUS 			   Status;

  if (gMmcHostInstance == NULL) {
    gMmcHostInstance = AllocateZeroPool (sizeof (MMC_HOST_INSTANCE));
    if (gMmcHostInstance == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    gMmcHostInstance->Signature = MMC_HOST_INSTANCE_SIGNATURE;
    gMmcHostInstance->BlockIo.Revision = EFI_BLOCK_IO_INTERFACE_REVISION;
    gMmcHostInstance->BlockIo.Reset = MmcReset;
    gMmcHostInstance->BlockIo.ReadBlocks = MmcReadBlocks;
    gMmcHostInstance->BlockIo.WriteBlocks = MmcWriteBlocks;
    gMmcHostInstance->BlockIo.FlushBlocks = MmcFlushBlocks;

    gMmcHostInstance->MmcHost = MmcHost;
  }

  gMmcHostInstance->State = MmcHwInitializationState;

  gMmcHostInstance->BlockIo.Media = AllocateCopyPool (sizeof(EFI_BLOCK_IO_MEDIA), &mMmcMediaTemplate);
  if (gMmcHostInstance->BlockIo.Media == NULL) {
    goto FREE_INSTANCE;
  }

  // Create DevicePath for the new MMC Host
  Status = MmcHost->BuildDevicePath (MmcHost, &NewDevicePathNode);
  if (EFI_ERROR (Status)) {
    goto FREE_MEDIA;
  }

  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) AllocatePool (END_DEVICE_PATH_LENGTH);
  if (DevicePath == NULL) {
    goto FREE_MEDIA;
  }

  SetDevicePathEndNode (DevicePath);
  //gMmcHostInstance->DevicePath = AppendDevicePathNode (DevicePath, NewDevicePathNode);
  gMmcHostInstance->DevicePath = &gDevicePath;

  return EFI_SUCCESS;

FREE_MEDIA:
  FreePool(gMmcHostInstance->BlockIo.Media);

FREE_INSTANCE:
  FreePool(gMmcHostInstance);
  gMmcHostInstance = NULL;

  return EFI_OUT_OF_RESOURCES;
}

EFI_STATUS DestroyMmcHostInstance (
  IN MMC_HOST_INSTANCE* MmcHostInstance
  )
{
  EFI_STATUS Status;

  // Uninstall Protocol Interfaces
  Status = gBS->UninstallMultipleProtocolInterfaces (
        MmcHostInstance->MmcHandle,
        &gEfiBlockIoProtocolGuid,&(MmcHostInstance->BlockIo),
        &gEfiDevicePathProtocolGuid,MmcHostInstance->DevicePath,
        NULL
        );
  ASSERT_EFI_ERROR (Status);

  // Free Memory allocated for the instance
  if (MmcHostInstance->BlockIo.Media) {
    FreePool(MmcHostInstance->BlockIo.Media);
  }
  FreePool (MmcHostInstance);

  return Status;
}

/**
  This function checks if the controller implement the Mmc Host and the Device Path Protocols
**/
EFI_STATUS
EFIAPI
MmcDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                      Status;
  //EFI_DEVICE_PATH_PROTOCOL        *ParentDevicePath;
  EFI_MMC_HOST_PROTOCOL           *MmcHost;
  EFI_DEV_PATH_PTR                Node;

  //
  // Check RemainingDevicePath validation
  //
  if (RemainingDevicePath != NULL) {
    //
    // Check if RemainingDevicePath is the End of Device Path Node,
    // if yes, go on checking other conditions
    //
    if (!IsDevicePathEnd (RemainingDevicePath)) {
      //
      // If RemainingDevicePath isn't the End of Device Path Node,
      // check its validation
      //
      Node.DevPath = RemainingDevicePath;
      if (Node.DevPath->Type != HARDWARE_DEVICE_PATH ||
        Node.DevPath->SubType != HW_VENDOR_DP      ||
        DevicePathNodeLength(Node.DevPath) != sizeof(VENDOR_DEVICE_PATH)) {
          return EFI_UNSUPPORTED;
      }
    }
  }

  //
  // Check if Mmc Host protocol is installed by platform
  //
  Status = gBS->OpenProtocol (
                Controller,
                &gEfiMmcHostProtocolGuid,
                (VOID **) &MmcHost,
                This->DriverBindingHandle,
                Controller,
                EFI_OPEN_PROTOCOL_BY_DRIVER
                );
  if (Status == EFI_ALREADY_STARTED) {
    return EFI_SUCCESS;
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Close the Mmc Host used to perform the supported test
  //
  gBS->CloseProtocol (
      Controller,
      &gEfiMmcHostProtocolGuid,
      This->DriverBindingHandle,
      Controller
      );

  return EFI_SUCCESS;
}

/**

**/
EFI_STATUS
EFIAPI
MmcDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
{
  EFI_MMC_HOST_PROTOCOL   *MmcHost = NULL;

  MmcHost = AllocateZeroPool (sizeof (EFI_MMC_HOST_PROTOCOL));
  if (MmcHost == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Check RemainingDevicePath validation
  //
  if (RemainingDevicePath != NULL) {
    //
    // Check if RemainingDevicePath is the End of Device Path Node,
    // if yes, return EFI_SUCCESS
    //
    if (IsDevicePathEnd (RemainingDevicePath)) {
      return EFI_SUCCESS;
    }
  }

  // Update Mmc Host Protocol Register
  CreateMmcHostProtocol(MmcHost);

  if (CreateMmcHostInstance(MmcHost) != EFI_OUT_OF_RESOURCES) {
    // Add the handle to the pool
    //InsertMmcHost (MmcHostInstance);

    gMmcHostInstance->Initialized = FALSE;

    // Detect card presence now
    CheckCardsCallback (NULL, NULL);
  }

  return EFI_SUCCESS;
}

/**

**/
EFI_STATUS
EFIAPI
MmcDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN  EFI_HANDLE                    Controller,
  IN  UINTN                         NumberOfChildren,
  IN  EFI_HANDLE                    *ChildHandleBuffer
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;

    // Close gEfiMmcHostProtocolGuid
    Status = gBS->CloseProtocol (
                Controller,
                &gEfiMmcHostProtocolGuid,(VOID **) &gMmcHostInstance->MmcHost,
                This->DriverBindingHandle
                );

    // Destroy MmcHostInstance
    DestroyMmcHostInstance (gMmcHostInstance);

  return Status;
}

VOID
EFIAPI
CheckCardsCallback (
  IN  EFI_EVENT   Event,
  IN  VOID        *Context
  )
{
    if (gMmcHostInstance->MmcHost->IsCardPresent (gMmcHostInstance->MmcHost) == !gMmcHostInstance->Initialized) {
      gMmcHostInstance->State = MmcHwInitializationState;
      gMmcHostInstance->BlockIo.Media->MediaPresent = !gMmcHostInstance->Initialized;
      if (InitializeMmcDevice (gMmcHostInstance) != EFI_SUCCESS)
        gMmcHostInstance->Initialized = FALSE;
      else
        gMmcHostInstance->Initialized = TRUE;
    }
}


EFI_DRIVER_BINDING_PROTOCOL gMmcDriverBinding = {
  MmcDriverBindingSupported,
  MmcDriverBindingStart,	//Detect card and get details
  MmcDriverBindingStop,
  0xa,
  NULL,
  NULL
};

EFI_STATUS
EFIAPI
MmcDxeInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_MMC_HOST_PROTOCOL   *MmcHost = NULL;

  Status = MmcInitialize();
  if (Status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR,"Failed to init MMC\n"));
    return Status;
  }

  MmcHost = AllocateZeroPool (sizeof (EFI_MMC_HOST_PROTOCOL));
  if (MmcHost == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

#ifdef BINDING_SUPPORTED
  //
  // Install driver model protocol(s).
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
           ImageHandle,
           SystemTable,
           &gMmcDriverBinding,
           ImageHandle,
           &gMmcComponentName,
           &gMmcComponentName2
           );
  ASSERT_EFI_ERROR (Status);

  //
  //FIXME Recieving card present even if no card is in slot, so events are getting triggered continuously.
  //

  // Use a timer to detect if a card has been plugged in or removed
  Status = gBS->CreateEvent (
                EVT_NOTIFY_SIGNAL | EVT_TIMER,
                TPL_CALLBACK,
                CheckCardsCallback,
                NULL,
                &gCheckCardsEvent);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->SetTimer(
                gCheckCardsEvent,
                TimerPeriodic,
                (UINT64)(10*1000*200)); // 200 ms
  ASSERT_EFI_ERROR (Status);
#endif
  /*FIXME Remove below code if installing Driver binding protocol */
  // Update Mmc Host Protocol Register
  CreateMmcHostProtocol(MmcHost);

  if (CreateMmcHostInstance(MmcHost) == EFI_OUT_OF_RESOURCES) {

    gMmcHostInstance->Initialized = FALSE;
    return EFI_OUT_OF_RESOURCES;
  } else {
    // Detect card presence now
    CheckCardsCallback (NULL, NULL);
    if (gMmcHostInstance->Initialized == TRUE) {
      // Publish BlockIO protocol interface
      Status = gBS->InstallMultipleProtocolInterfaces (
                &ImageHandle,
                &gEfiDevicePathProtocolGuid,gMmcHostInstance->DevicePath,
                &gEfiBlockIoProtocolGuid,&gMmcHostInstance->BlockIo,
		  &gEfiMmcHostProtocolGuid,gMmcHostInstance->MmcHost,
                NULL
                );

      gMmcHostInstance->MmcHandle = &ImageHandle;
    }
  }

  return Status;
}
