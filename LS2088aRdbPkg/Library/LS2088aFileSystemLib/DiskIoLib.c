/** @DiskIoLib.c
#
#  API for implementing DiskIo protocol over BlockIo protocol
#
#  Based on DiskIo driver implementation available in
#  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIo.c 
#
#  Copyright (c) 2006 - 2013, Intel Corporation. All rights reserved.<BR>
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


#include <Library/LS2088aFileSystem.h>

/**
  Read BufferSize bytes from Offset into Buffer.
  Reads may support reads that are not aligned on
  sector boundaries. There are three cases:
    UnderRun - The first byte is not on a sector boundary or the read request is
               less than a sector in length.
    Aligned  - A read of N contiguous sectors.
    OverRun  - The last byte is not on a sector boundary.

  @param  This                  Protocol instance pointer.
  @param  MediaId               Id of the media, changes every time the media is
				replaced.
  @param  Offset                The starting byte offset to read from
  @param  BufferSize            Size of Buffer
  @param  Buffer                Buffer containing read data

  @retval EFI_SUCCESS           The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing
				the read.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHNAGED     The MediaId does not matched the current device.
  @retval EFI_INVALID_PARAMETER The read request contains device addresses that
				are not valid for the device.

**/

EFI_STATUS
EFIAPI
DiskIoReadDisk (
  IN EFI_DISK_IO_PROTOCOL  *This,
  IN UINT32                MediaId,
  IN UINT64                Offset,
  IN UINTN                 BufferSize,
  OUT VOID                 *Buffer
  )
{
  EFI_STATUS		Status;
  UINT32                BlockSize;
  UINT64                Lba;
  UINT32		LbaOffset;
  UINT32		OverRun;
  UINT32		NumBlocks;
  VOID			*DiskIoBuffer;
  EFI_BLOCK_IO_PROTOCOL *BlockIo;
  LS2088A_FILE_SYSTEM		*Instance;

  Instance = CR(This, LS2088A_FILE_SYSTEM, DiskIo,
		  LS2088A_FILE_SYSTEM_SIGNATURE);
  BlockIo = Instance->BlockIo;
  BlockSize = BlockIo->Media->BlockSize;
  Lba = DivU64x32Remainder (Offset, BlockSize, &LbaOffset);
  NumBlocks = DivU64x32Remainder (BufferSize + LbaOffset, BlockSize, &OverRun);
  if(OverRun > 0)
	NumBlocks++;
  DiskIoBuffer = AllocatePages(EFI_SIZE_TO_PAGES(NumBlocks * BlockSize));
  ASSERT(DiskIoBuffer != NULL);
  Status = BlockIo->ReadBlocks(BlockIo, MediaId, Lba, NumBlocks * BlockSize,
		  DiskIoBuffer);
  ASSERT(Status == EFI_SUCCESS);
  CopyMem(Buffer, DiskIoBuffer + LbaOffset, BufferSize);
  FreePages(DiskIoBuffer, EFI_SIZE_TO_PAGES(NumBlocks * BlockSize));
  return EFI_SUCCESS;
}

/**
  Writes BufferSize bytes from Buffer into Offset.
  Writes may require a read modify write to support writes that are not
  aligned on sector boundaries. There are three cases:
    UnderRun - The first byte is not on a sector boundary or the write request
               is less than a sector in length. Read modify write is required.
    Aligned  - A write of N contiguous sectors.
    OverRun  - The last byte is not on a sector boundary. Read modified write
               required.

  @param  This       Protocol instance pointer.
  @param  MediaId    Id of the media, changes every time the media is replaced.
  @param  Offset     The starting byte offset to read from
  @param  BufferSize Size of Buffer
  @param  Buffer     Buffer containing read data

  @retval EFI_SUCCESS           The data was written correctly to the device.
  @retval EFI_WRITE_PROTECTED   The device can not be written to.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing
				the write.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHNAGED     The MediaId does not matched the current device.
  @retval EFI_INVALID_PARAMETER The write request contains device addresses that
				are not valid for the device.

**/
EFI_STATUS
EFIAPI
DiskIoWriteDisk (
  IN EFI_DISK_IO_PROTOCOL  *This,
  IN UINT32                MediaId,
  IN UINT64                Offset,
  IN UINTN                 BufferSize,
  IN VOID                  *Buffer
  )
{
  EFI_STATUS		Status;
  UINT32                BlockSize;
  UINT64                Lba;
  UINT32		LbaOffset;
  UINT32		OverRun;
  UINT32		NumBlocks;
  VOID			*DiskIoBuffer;
  EFI_BLOCK_IO_PROTOCOL *BlockIo;
  LS2088A_FILE_SYSTEM		*Instance;

  Instance = CR(This, LS2088A_FILE_SYSTEM, DiskIo,
		  LS2088A_FILE_SYSTEM_SIGNATURE);
  BlockIo = Instance->BlockIo;
  BlockSize = BlockIo->Media->BlockSize;

  Lba = DivU64x32Remainder (Offset, BlockSize, &LbaOffset);
  NumBlocks = DivU64x32Remainder (BufferSize + LbaOffset, BlockSize, &OverRun);
  if(OverRun > 0)
	NumBlocks++;
  DiskIoBuffer = AllocatePages(EFI_SIZE_TO_PAGES(NumBlocks * BlockSize));
  ASSERT(DiskIoBuffer != NULL);
  if(LbaOffset > 0) {
	Status = BlockIo->ReadBlocks(BlockIo, MediaId, Lba, BlockSize,
			DiskIoBuffer);
	ASSERT(Status == EFI_SUCCESS);
  }
  if(OverRun > 0 && NumBlocks > 1) {
	Status = BlockIo->ReadBlocks(BlockIo, MediaId, Lba + NumBlocks - 1,
			BlockSize, DiskIoBuffer + (NumBlocks - 1) * BlockSize);
	ASSERT(Status == EFI_SUCCESS);
  }
  CopyMem(DiskIoBuffer + LbaOffset, Buffer, BufferSize);
  Status = BlockIo->WriteBlocks(BlockIo, MediaId, Lba, NumBlocks * BlockSize,
		  DiskIoBuffer);
  ASSERT(Status == EFI_SUCCESS);
  FreePages(DiskIoBuffer, EFI_SIZE_TO_PAGES(NumBlocks * BlockSize));
  return EFI_SUCCESS;
}
