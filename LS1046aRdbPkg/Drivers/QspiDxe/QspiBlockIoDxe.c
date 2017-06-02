/** @file  QspiBlockIoDxe.c

  Copyright (c) 2011-2013, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2016-2017 NXP.
  All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD
  License which accompanies this distribution.  The full text of the license 
  may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

 **/

#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "QspiDxe.h"
//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.Reset
//
EFI_STATUS
  EFIAPI
QspiBlockIoReset (
    IN EFI_BLOCK_IO_PROTOCOL  *This,
    IN BOOLEAN                ExtendedVerification
    )
{

  DEBUG ((DEBUG_BLKIO, "QspiBlockIoReset(MediaId=0x%x)\n",
        This->Media->MediaId));

  // Nothing to do so just return without error
  return EFI_SUCCESS;
}

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
    )
{
  QSPI_FLASH_INSTANCE  *Instance;
  EFI_STATUS           Status;
  EFI_BLOCK_IO_MEDIA   *Media;
  UINTN                NumBlocks;

  if ((This == NULL) || (Buffer == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Instance = INSTANCE_FROM_BLKIO_THIS(This);
  Media = This->Media;
  NumBlocks = ((UINTN)BufferSizeInBytes) / Instance->Media.BlockSize ;

  DEBUG ((DEBUG_BLKIO, "%a(MediaId=0x%x, Lba=%ld, BufferSize=0x%x bytes (%d kB)"
        ", BufferPtr @ 0x%p)\n",__FUNCTION__, MediaId, Lba, BufferSizeInBytes,
        Buffer));

  if (!Media) {
    Status = EFI_INVALID_PARAMETER;
  } else if (!Media->MediaPresent) {
    Status = EFI_NO_MEDIA;
  } else if (Media->MediaId != MediaId) {
    Status = EFI_MEDIA_CHANGED;
  } else if ((Media->IoAlign >= 2) && 
      (((UINTN)Buffer & (Media->IoAlign - 1)) != 0)) {
    Status = EFI_INVALID_PARAMETER;
  } else if (BufferSizeInBytes == 0) {
    // Return if we have not any byte to read
    Status = EFI_SUCCESS;
  } else if ((BufferSizeInBytes % Media->BlockSize) != 0) {
    // The size of the buffer must be a multiple of the block size
    Status = EFI_BAD_BUFFER_SIZE;
  } else if ((Lba + NumBlocks - 1) > Media->LastBlock) {
    // All blocks must be within the device
    DEBUG((EFI_D_ERROR, "%a: ERROR - Read will exceed last block\n",
          __FUNCTION__));
    Status = EFI_INVALID_PARAMETER;
  } else {
    while(BufferSizeInBytes)
    {
      //Read Block
      Status = QspiRead(Instance, Lba++, 0, Media->BlockSize, Buffer);
      BufferSizeInBytes -= Media->BlockSize;
      Buffer += Media->BlockSize;
    }
  }

  return Status;
}

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
    )
{
  QSPI_FLASH_INSTANCE  *Instance;
  EFI_STATUS           Status;
  EFI_BLOCK_IO_MEDIA   *Media;
  UINTN                NumBlocks;

  if ((This == NULL) || (Buffer == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Instance = INSTANCE_FROM_BLKIO_THIS(This);
  Media = This->Media;
  NumBlocks = ((UINTN)BufferSizeInBytes) / Instance->Media.BlockSize ;

  DEBUG ((DEBUG_BLKIO, "%a(MediaId=0x%x, Lba=%ld, BufferSize=0x%x bytes (%d kB)"
        ", BufferPtr @ 0x%08x)\n",__FUNCTION__, MediaId, Lba, BufferSizeInBytes,
        Buffer));

  if (!Media) {
    Status = EFI_INVALID_PARAMETER;
  } else if( !Media->MediaPresent ) {
    Status = EFI_NO_MEDIA;
  } else if( Media->MediaId != MediaId ) {
    Status = EFI_MEDIA_CHANGED;
  } else if( Media->ReadOnly ) {
    Status = EFI_WRITE_PROTECTED;
  } else if (BufferSizeInBytes == 0) {
    // We must have some bytes to Write
    DEBUG((DEBUG_BLKIO, "%a: BufferSizeInBytes=0x%x\n", __FUNCTION__,
          BufferSizeInBytes));
    Status = EFI_BAD_BUFFER_SIZE;
  } else if ((BufferSizeInBytes % Media->BlockSize) != 0) {
    // The size of the buffer must be a multiple of the block size
    DEBUG((DEBUG_BLKIO, "%a: BlockSize in bytes =0x%x\n",__FUNCTION__,
          Media->BlockSize));
    Status = EFI_BAD_BUFFER_SIZE;
  } else if ((Lba + NumBlocks - 1) > Media->LastBlock) {
    // All blocks must be within the device
    DEBUG((EFI_D_ERROR, "%a: ERROR - Write will exceed last block\n",
          __FUNCTION__));
    Status = EFI_INVALID_PARAMETER;
  } else {
    // Program Data
    while(BufferSizeInBytes)
    {
      Status = QspiWrite(Instance, Lba++, 0, Media->BlockSize, Buffer);
      BufferSizeInBytes -= Media->BlockSize;
      Buffer += Media->BlockSize;
    }
  }
  return Status;
}

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.FlushBlocks
//
EFI_STATUS
  EFIAPI
QspiBlockIoFlushBlocks (
    IN EFI_BLOCK_IO_PROTOCOL  *This
    )
{

  DEBUG ((DEBUG_BLKIO, "%a: Function NOT IMPLEMENTED (not required).\n",
        __FUNCTION__));

  // Nothing to do so just return without error
  return EFI_SUCCESS;
}
