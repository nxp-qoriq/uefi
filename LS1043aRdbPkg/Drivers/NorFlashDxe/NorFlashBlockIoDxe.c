/** @file  NorFlashBlockIoDxe.c

  Based on NorFlash implementation available in ArmPlatformPkg/Drivers/NorFlashDxe/NorFlashBlockIoDxe.c
  
  Copyright (c) 2011 - 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2016, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/NorFlash.h>

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.Reset
//
EFI_STATUS
EFIAPI
NorFlashBlockIoReset (
  IN EFI_BLOCK_IO_PROTOCOL  *This,
  IN BOOLEAN                ExtendedVerification
  )
{
  NOR_FLASH_INSTANCE *Instance;

  Instance = INSTANCE_FROM_BLKIO_THIS(This);

  DEBUG ((DEBUG_RELEASE, "NorFlashBlockIoReset(MediaId=0x%x)\n", This->Media->MediaId));

  return NorFlashPlatformReset (Instance);
}

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
  )
{
  NOR_FLASH_INSTANCE   *Instance;
  EFI_STATUS           Status = EFI_SUCCESS;
  EFI_BLOCK_IO_MEDIA   *Media;
  UINTN                NumBlocks;
  UINT8                *pReadBuffer;
  UINTN                BlockCount;
  UINTN                BlockSizeInBytes;
  EFI_LBA              CurrentBlock;

  if ((This == NULL) || (Buffer == NULL))
    return EFI_INVALID_PARAMETER;

  Instance = INSTANCE_FROM_BLKIO_THIS(This);
  Media = This->Media;
  NumBlocks = ((UINTN)BufferSizeInBytes) / Instance->Media.BlockSize ;

  DEBUG ((DEBUG_BLKIO, "%a(MediaId=0x%x, Lba=%ld, BufferSize=0x%x bytes (%d kB), BufferPtr @ 0x%p)\n",\
                        __FUNCTION__,MediaId, Lba, BufferSizeInBytes, Buffer));

  if (!Media)
    Status = EFI_INVALID_PARAMETER;
  else if (!Media->MediaPresent)
    Status = EFI_NO_MEDIA;
  else if (Media->MediaId != MediaId)
    Status = EFI_MEDIA_CHANGED;
  else if ((Media->IoAlign >= 2) && (((UINTN)Buffer & (Media->IoAlign - 1)) != 0))
    Status = EFI_INVALID_PARAMETER;
  else if (BufferSizeInBytes == 0)
    // Return if we have not any byte to read
    Status = EFI_SUCCESS;
  else if ((BufferSizeInBytes % Media->BlockSize) != 0)
    // The size of the buffer must be a multiple of the block size
    Status = EFI_BAD_BUFFER_SIZE;
  else if ((Lba + NumBlocks - 1) > Media->LastBlock) {
    // All blocks must be within the device
    DEBUG((EFI_D_ERROR, "%a: ERROR - Read will exceed last block\n",__FUNCTION__));
    Status = EFI_INVALID_PARAMETER;
  } else {
    BlockSizeInBytes = Instance->Media.BlockSize;

    /* Because the target *Buffer is a pointer to VOID, we must put all the data into a pointer */
    /* to a proper data type, so use *pReadBuffer */
    pReadBuffer = (UINT8 *)Buffer;

    CurrentBlock = Lba;
    // Read data block by Block
    for (BlockCount = 0; BlockCount < NumBlocks; BlockCount++, CurrentBlock++, pReadBuffer = pReadBuffer + BlockSizeInBytes) {
      DEBUG((DEBUG_BLKIO, "%a: Reading block #%d\n",__FUNCTION__,(UINTN)CurrentBlock));

      Status = NorFlashPlatformRead (Instance, CurrentBlock, (UINTN)0 , BlockSizeInBytes,pReadBuffer);
      if (EFI_ERROR(Status))
        break;
    }
  }
  DEBUG((DEBUG_BLKIO,"%a: Exit Status = \"%r\".\n",__FUNCTION__,Status));

  return Status;
}

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
  )
{
  NOR_FLASH_INSTANCE  *Instance;
  EFI_STATUS           Status = EFI_SUCCESS;
  EFI_BLOCK_IO_MEDIA   *Media;
  UINTN                NumBlocks;
  EFI_LBA              CurrentBlock;
  UINTN                BlockSizeInBytes;
  UINT32               BlockCount;
  UINTN                SectorAddress;
  UINT8                *pWriteBuffer;

  if ((This == NULL) || (Buffer == NULL))
    return EFI_INVALID_PARAMETER;

  Instance = INSTANCE_FROM_BLKIO_THIS(This);
  Media = This->Media;
  NumBlocks = ((UINTN)BufferSizeInBytes) / Instance->Media.BlockSize ;

  DEBUG ((DEBUG_BLKIO, "%a(MediaId=0x%x, Lba=%ld, BufferSize=0x%x bytes (%d kB), BufferPtr @ 0x%08x)\n", __FUNCTION__,MediaId, Lba, BufferSizeInBytes, Buffer));

  if (!Media) {
    DEBUG ((EFI_D_RELEASE, "Nor Flash Media pointer is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }
  
  if( !Media->MediaPresent )
    Status = EFI_NO_MEDIA;
  else if( Media->MediaId != MediaId )
    Status = EFI_MEDIA_CHANGED;
  else if( Media->ReadOnly )
    Status = EFI_WRITE_PROTECTED;
  else if (BufferSizeInBytes == 0) {
    // We must have some bytes to Write
    DEBUG((DEBUG_BLKIO, "%a: BufferSizeInBytes=0x%x\n",__FUNCTION__, BufferSizeInBytes));
    Status = EFI_BAD_BUFFER_SIZE;
  } else if ((BufferSizeInBytes % Media->BlockSize) != 0) {
    // The size of the buffer must be a multiple of the block size
    DEBUG((DEBUG_BLKIO, "%a: BlockSize in bytes =0x%x\n",__FUNCTION__, Media->BlockSize));
    Status = EFI_BAD_BUFFER_SIZE;
  } else if ((Lba + NumBlocks - 1) > Media->LastBlock) {
    // All blocks must be within the device
    DEBUG((EFI_D_ERROR, "%a: ERROR - Write will exceed last block\n",__FUNCTION__));
    Status = EFI_INVALID_PARAMETER;
  } else {
    BlockSizeInBytes = Instance->Media.BlockSize;

    pWriteBuffer = (UINT8 *)Buffer;

    CurrentBlock = Lba;
    // Program data block by Block
    for (BlockCount = 0; BlockCount < NumBlocks; BlockCount++, CurrentBlock++, pWriteBuffer = pWriteBuffer + BlockSizeInBytes) {
      DEBUG((DEBUG_BLKIO, "%a: Writing block #%d\n",__FUNCTION__,(UINTN)CurrentBlock));
      // Erase the Block(Sector) to be written to
      SectorAddress = GET_NOR_BLOCK_ADDRESS (
                                          Instance->RegionBaseAddress,
                                          CurrentBlock,
                                          Instance->Media.BlockSize
                                         );
      Status = NorFlashPlatformEraseSector (Instance, (UINTN)SectorAddress);
      if (EFI_ERROR(Status)) {
        DEBUG((EFI_D_ERROR, "%a: ERROR - Failed to erase Target 0x%x\n", __FUNCTION__,SectorAddress));
        break;
      }
      // Program Block(Sector) to be written to
      Status = NorFlashPlatformWriteBuffer (Instance, CurrentBlock, (UINTN)0, &BlockSizeInBytes, pWriteBuffer);
      if (EFI_ERROR(Status))
        break;
    }
  }
  DEBUG((DEBUG_BLKIO, "%a: Exit Status = \"%r\".\n",__FUNCTION__,Status));
  return Status;
}

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.FlushBlocks
//
EFI_STATUS
EFIAPI
NorFlashBlockIoFlushBlocks (
  IN EFI_BLOCK_IO_PROTOCOL  *This
  )
{

  DEBUG ((DEBUG_BLKIO, "NorFlashBlockIoFlushBlocks: Function NOT IMPLEMENTED (not required).\n"));

  // Nothing to do so just return without error
  return EFI_SUCCESS;
}
