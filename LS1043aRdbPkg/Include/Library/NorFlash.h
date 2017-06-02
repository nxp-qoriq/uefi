/** @file  NorFlashDxe.h

  Based on NOR flash access APIs used in ArmPlatformPkg/Drivers/NorFlashDxe/NorFlashDxe.h
  
  Copyright (c) 2011 - 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __NOR_FLASH_DXE_H__
#define __NOR_FLASH_DXE_H__


#include <Base.h>
#include <PiDxe.h>

#include <Guid/EventGroup.h>

#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo.h>
#include <Protocol/FirmwareVolumeBlock.h>

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/NorFlashLib.h>
#include <Library/FslIfc.h>

#define NOR_FLASH_ERASE_RETRY                     10

#define GET_NOR_BLOCK_ADDRESS(BaseAddr,Lba,LbaSize) ( BaseAddr + (UINTN)((Lba) * LbaSize) )

#define NOR_FLASH_SIGNATURE                       SIGNATURE_32('n', 'o', 'r', '0')
#define INSTANCE_FROM_FVB_THIS(a)                 CR(a, NOR_FLASH_INSTANCE, FvbProtocol, NOR_FLASH_SIGNATURE)
#define INSTANCE_FROM_BLKIO_THIS(a)               CR(a, NOR_FLASH_INSTANCE, BlockIoProtocol, NOR_FLASH_SIGNATURE)
#define INSTANCE_FROM_DISKIO_THIS(a)              CR(a, NOR_FLASH_INSTANCE, DiskIoProtocol, NOR_FLASH_SIGNATURE)

typedef struct _NOR_FLASH_INSTANCE                NOR_FLASH_INSTANCE;

typedef EFI_STATUS (*NOR_FLASH_INITIALIZE)        (NOR_FLASH_INSTANCE* Instance);

typedef struct {
  VENDOR_DEVICE_PATH                  Vendor;
  EFI_DEVICE_PATH_PROTOCOL            End;
} NOR_FLASH_DEVICE_PATH;

struct _NOR_FLASH_INSTANCE {
  UINT32                              Signature;
  EFI_HANDLE                          Handle;

  BOOLEAN                             Initialized;
  NOR_FLASH_INITIALIZE                Initialize;

  UINTN                               DeviceBaseAddress;
  UINTN                               RegionBaseAddress;
  UINTN                               Size;
  EFI_LBA                             StartLba;

  EFI_BLOCK_IO_PROTOCOL               BlockIoProtocol;
  EFI_BLOCK_IO_MEDIA                  Media;

  BOOLEAN                             SupportFvb;
  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL FvbProtocol;
  VOID*                               ShadowBuffer;

  NOR_FLASH_DEVICE_PATH               DevicePath;
};

//
// NorFlashLib.c related
//

EFI_STATUS
NorFlashPlatformWriteBuffer (
  IN NOR_FLASH_INSTANCE     *Instance,
  IN EFI_LBA                Lba,
  IN        UINTN           Offset,
  IN OUT    UINTN           *NumBytes,
  IN        UINT8           *Buffer
  );

EFI_STATUS
NorFlashPlatformEraseSector (
  IN NOR_FLASH_INSTANCE     *Instance,
  IN UINTN                  SectorAddress
  );

EFI_STATUS
NorFlashPlatformEraseChip (
  IN NOR_FLASH_INSTANCE     *Instance
  );

EFI_STATUS
NorFlashPlatformGetDevices (
  OUT NOR_FLASH_DESCRIPTION   **NorFlashDevices,
  OUT UINT32                  *Count
  );

EFI_STATUS
NorFlashPlatformFlashGetAttributes (
  OUT NOR_FLASH_DESCRIPTION  *NorFlashDevices,
  IN UINT32                  Count
  );

EFI_STATUS
NorFlashPlatformControllerInitialization (
  VOID
  );

EFI_STATUS
NorFlashPlatformRead (
  IN NOR_FLASH_INSTANCE     *Instance,
  IN EFI_LBA              Lba,
  IN UINTN                Offset,
  IN UINTN                BufferSizeInBytes,
  OUT UINT8                *Buffer
  );

EFI_STATUS
NorFlashPlatformReset (
  IN NOR_FLASH_INSTANCE     *Instance
  );

extern CONST EFI_GUID* CONST          mNorFlashVariableGuid;

//
// NorFlashFvbDxe.c
//

EFI_STATUS
EFIAPI
NorFlashFvbInitialize (
  IN NOR_FLASH_INSTANCE*                            Instance
  );

EFI_STATUS
EFIAPI
FvbGetAttributes(
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL     *This,
  OUT       EFI_FVB_ATTRIBUTES_2                    *Attributes
  );

EFI_STATUS
EFIAPI
FvbSetAttributes(
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL     *This,
  IN OUT    EFI_FVB_ATTRIBUTES_2                    *Attributes
  );

EFI_STATUS
EFIAPI
FvbGetPhysicalAddress(
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL     *This,
  OUT       EFI_PHYSICAL_ADDRESS                    *Address
  );

EFI_STATUS
EFIAPI
FvbGetBlockSize(
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL     *This,
  IN EFI_LBA              Lba,
  OUT       UINTN                                   *BlockSize,
  OUT       UINTN                                   *NumberOfBlocks
  );

EFI_STATUS
EFIAPI
FvbRead(
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL     *This,
  IN EFI_LBA              Lba,
  IN UINTN                Offset,
  IN OUT    UINTN                                   *NumBytes,
  IN OUT    UINT8                                   *Buffer
  );

EFI_STATUS
EFIAPI
FvbWrite(
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL     *This,
  IN        EFI_LBA               Lba,
  IN        UINTN                 Offset,
  IN OUT    UINTN                *NumBytes,
  IN        UINT8                *Buffer
  );

EFI_STATUS
EFIAPI
FvbEraseBlocks(
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL     *This,
  ...
  );
//
// NorFlashBlockIoDxe.c
//

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.Reset
//
EFI_STATUS
EFIAPI
NorFlashBlockIoReset (
  IN EFI_BLOCK_IO_PROTOCOL    *This,
  IN BOOLEAN                  ExtendedVerification
  );

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.ReadBlocks
//
EFI_STATUS
EFIAPI
NorFlashBlockIoReadBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL   *This,
  IN  UINT32                  MediaId,
  IN  EFI_LBA                 Lba,
  IN  UINTN                   BufferSizeInBytes,
  OUT VOID                    *Buffer
);

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.WriteBlocks
//
EFI_STATUS
EFIAPI
NorFlashBlockIoWriteBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL   *This,
  IN  UINT32                  MediaId,
  IN  EFI_LBA                 Lba,
  IN  UINTN                   BufferSizeInBytes,
  IN  VOID                    *Buffer
);

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.FlushBlocks
//
EFI_STATUS
EFIAPI
NorFlashBlockIoFlushBlocks (
  IN EFI_BLOCK_IO_PROTOCOL    *This
);

#endif /* __NOR_FLASH_DXE_H__ */
