/** @file  QspiDxe.h

  Based on NorFlashDxe.h available in
  ArmPlatformPkg/Drivers/NorFlashDxe/NorFlashDxe.h
  Copyright (c) 2011 - 2014, ARM Ltd. All rights reserved.<BR>
  Copyright 2017 NXP

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __QSPI_DXE_H__
#define __QSPI_DXE_H__


#include <Base.h>
#include <PiDxe.h>

#include <Guid/EventGroup.h>

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/Qspi.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeLib.h>

#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo.h>
#include <Protocol/FirmwareVolumeBlock.h>

#define GET_BLOCK_OFFSET(Lba) \
  ((Instance->RegionBaseAddress)-(Instance->DeviceBaseAddress)+((UINTN)((Lba) * Instance->Media.BlockSize)))
#define QSPI_FLASH_SIGNATURE                   SIGNATURE_32('q', 's', 'p', 'i')
#define INSTANCE_FROM_FVB_THIS(a)              CR(a, QSPI_FLASH_INSTANCE, FvbProtocol, QSPI_FLASH_SIGNATURE)
#define INSTANCE_FROM_BLKIO_THIS(a)            CR(a, QSPI_FLASH_INSTANCE, BlockIoProtocol, QSPI_FLASH_SIGNATURE)
#define INSTANCE_FROM_DISKIO_THIS(a)           CR(a, QSPI_FLASH_INSTANCE, DiskIoProtocol, QSPI_FLASH_SIGNATURE)

typedef struct _QSPI_FLASH_INSTANCE            QSPI_FLASH_INSTANCE;

typedef EFI_STATUS (*QSPI_FLASH_INITIALIZE)    (QSPI_FLASH_INSTANCE* Instance);

typedef struct {
  VENDOR_DEVICE_PATH                  Vendor;
  EFI_DEVICE_PATH_PROTOCOL            End;
} QSPI_FLASH_DEVICE_PATH;

struct _QSPI_FLASH_INSTANCE {
  UINT32                              Signature;
  EFI_HANDLE                          Handle;

  BOOLEAN                             Initialized;
  QSPI_FLASH_INITIALIZE               Initialize;

  UINTN                               DeviceBaseAddress;
  UINTN                               RegionBaseAddress;
  UINTN                               Size;
  EFI_LBA                             StartLba;

  EFI_BLOCK_IO_PROTOCOL               BlockIoProtocol;
  EFI_BLOCK_IO_MEDIA                  Media;

  BOOLEAN                             SupportFvb;
  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL FvbProtocol;
  VOID*                               ShadowBuffer;

  QSPI_FLASH_DEVICE_PATH              DevicePath;
};

EFI_STATUS
QspiReadCfiData (
  IN  UINTN                   DeviceBaseAddress,
  IN  UINTN                   CFI_Offset,
  IN  UINT32                  NumberOfBytes,
  OUT UINT32                  *Data
);

EFI_STATUS
QspiWriteBuffer (
  IN QSPI_FLASH_INSTANCE    *Instance,
  IN UINTN                  TargetAddress,
  IN UINTN                  BufferSizeInBytes,
  IN UINT32                 *Buffer
);

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.Reset
//
EFI_STATUS
EFIAPI
QspiBlockIoReset (
  IN EFI_BLOCK_IO_PROTOCOL    *This,
  IN BOOLEAN                  ExtendedVerification
);

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.ReadBlocks
//
EFI_STATUS
EFIAPI
QspiBlockIoReadBlocks (
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
QspiBlockIoWriteBlocks (
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
QspiBlockIoFlushBlocks (
  IN EFI_BLOCK_IO_PROTOCOL    *This
);

//
// DiskIO Protocol function EFI_DISK_IO_PROTOCOL.ReadDisk
//
EFI_STATUS
EFIAPI
QspiDiskIoReadDisk (
  IN EFI_DISK_IO_PROTOCOL         *This,
  IN UINT32                       MediaId,
  IN UINT64                       Offset,
  IN UINTN                        BufferSize,
  OUT VOID                        *Buffer
);

//
// DiskIO Protocol function EFI_DISK_IO_PROTOCOL.WriteDisk
//
EFI_STATUS
EFIAPI
QspiDiskIoWriteDisk (
  IN EFI_DISK_IO_PROTOCOL         *This,
  IN UINT32                       MediaId,
  IN UINT64                       Offset,
  IN UINTN                        BufferSize,
  IN VOID                         *Buffer
);

//
// Function Prototypes in QspiFvbDxe.c
//

EFI_STATUS
EFIAPI
QspiFvbInitialize (
  IN QSPI_FLASH_INSTANCE*                            Instance
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
  IN        EFI_LBA                                 Lba,
  OUT       UINTN                                   *BlockSize,
  OUT       UINTN                                   *NumberOfBlocks
);

EFI_STATUS
EFIAPI
FvbRead(
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL     *This,
  IN        EFI_LBA                                 Lba,
  IN        UINTN                                   Offset,
  IN OUT    UINTN                                   *NumBytes,
  IN OUT    UINT8                                   *Buffer
);

EFI_STATUS
EFIAPI
FvbWrite(
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL     *This,
  IN        EFI_LBA                                 Lba,
  IN        UINTN                                   Offset,
  IN OUT    UINTN                                   *NumBytes,
  IN        UINT8                                   *Buffer
);

EFI_STATUS
EFIAPI
FvbEraseBlocks(
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL     *This,
  ...
);

//
// Function Prototypes in QspiDxe.c
//

EFI_STATUS
QspiRead (
  IN QSPI_FLASH_INSTANCE   *Instance,
  IN EFI_LBA              Lba,
  IN UINTN                Offset,
  IN UINTN                BufferSizeInBytes,
  OUT VOID                *Buffer
);

EFI_STATUS
QspiWrite (
  IN        QSPI_FLASH_INSTANCE  *Instance,
  IN        EFI_LBA              Lba,
  IN        UINTN                Offset,
  IN OUT    UINTN                NumBytes,
  IN        UINT8                *Buffer
);

//
// Function Prototypes in QspiTest.c
//

EFI_STATUS
QspiTest (
  IN        QSPI_FLASH_INSTANCE   *Instance
);

#endif /* __QSPI_DXE_H__ */
