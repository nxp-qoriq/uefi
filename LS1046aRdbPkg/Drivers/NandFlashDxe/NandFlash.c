/** @NandFlash.c
#
#  Driver for installing BlockIo protocol over IFC NAND
#
#  Based on NAND flash driver implementation in
#  Omap35xxPkg/Flash/Flash.c
#
#  Copyright (c) 2011-2013, ARM Limited. All rights reserved.
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


#include <Library/FslIfc.h>
#include <Library/IfcNand.h>

EFI_BLOCK_IO_MEDIA gNandFlashMedia = {
  SIGNATURE_32('n','a','n','d'),            // MediaId
  FALSE,                                    // RemovableMedia
  TRUE,                                     // MediaPresent
  FALSE,                                    // LogicalPartition
  FALSE,                                    // ReadOnly
  FALSE,                                    // WriteCaching
  0,                                        // BlockSize
  2,                                        // IoAlign
  0,                                        // Pad
  0                                         // LastBlock
};

// Device path for SemiHosting. It contains our autogened Caller ID GUID.

typedef struct {

  VENDOR_DEVICE_PATH        Guid;

  EFI_DEVICE_PATH_PROTOCOL  End;

} FLASH_DEVICE_PATH;

FLASH_DEVICE_PATH gDevicePath = {
  {
    { HARDWARE_DEVICE_PATH, HW_VENDOR_DP, { sizeof (VENDOR_DEVICE_PATH), 0 } },
    EFI_CALLER_ID_GUID
  },
  { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE,
	  { sizeof (EFI_DEVICE_PATH_PROTOCOL), 0} }
};

EFI_STATUS
EFIAPI
NandFlashReset (
  IN EFI_BLOCK_IO_PROTOCOL          *This,
  IN BOOLEAN                        ExtendedVerification
  )
{
  return IfcNandFlashReset(This, ExtendedVerification);
}

EFI_STATUS
EFIAPI
NandFlashReadBlocks (
  IN EFI_BLOCK_IO_PROTOCOL          *This,
  IN UINT32                         MediaId,
  IN EFI_LBA                        Lba,
  IN UINTN                          BufferSize,
  OUT VOID                          *Buffer
  )
{
  return IfcNandFlashReadBlocks(This, MediaId, Lba, BufferSize, Buffer);
}

EFI_STATUS
EFIAPI
NandFlashWriteBlocks (
  IN EFI_BLOCK_IO_PROTOCOL          *This,
  IN UINT32                         MediaId,
  IN EFI_LBA                        Lba,
  IN UINTN                          BufferSize,
  IN VOID                           *Buffer
  )
{
  return IfcNandFlashWriteBlocks(This, MediaId, Lba, BufferSize, Buffer);
}

EFI_STATUS
EFIAPI
NandFlashFlushBlocks (
  IN EFI_BLOCK_IO_PROTOCOL  *This
  )
{
  return IfcNandFlashFlushBlocks(This);
}

EFI_BLOCK_IO_PROTOCOL BlockIo =
{
  EFI_BLOCK_IO_INTERFACE_REVISION,  // Revision
  &gNandFlashMedia,                  // *Media
  NandFlashReset,                   // Reset
  NandFlashReadBlocks,              // ReadBlocks
  NandFlashWriteBlocks,             // WriteBlocks
  NandFlashFlushBlocks              // FlushBlocks
};

EFI_STATUS
NandFlashInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS              Status;

  Status = IfcNandFlashInit(&gNandFlashMedia);

  //
  // Install driver model protocol(s).
  //
	if(!EFI_ERROR(Status)) {
		Status = gBS->InstallMultipleProtocolInterfaces (
							 &ImageHandle,
							 &gEfiDevicePathProtocolGuid, &gDevicePath,
							 &gEfiBlockIoProtocolGuid, &BlockIo,
				 NULL
							 );
	}

  return Status;
}

