/** DspiBlockIoApi.c
  APIs for Read, Write, Initialize etc of Block IO Layer

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This Program And The Accompanying Materials
  Are Licensed And Made Available Under The Terms And Conditions Of The BSD
  License Which Accompanies This Distribution. The Full Text Of The License May
  Be Found At http://Opensource.Org/Licenses/Bsd-License.Php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include "Library/Dspi.h"

extern struct DspiFlash *gFlash;

EFI_STATUS
EFIAPI
DspiRead (
  IN EFI_BLOCK_IO_PROTOCOL          *This,
  IN UINT32                         MediaId,
  IN EFI_LBA                        Lba,
  IN UINTN                          BufferSize,
  OUT VOID                          *Buffer
  )
{
  EFI_STATUS Status;

  if (Buffer == NULL) {
    DEBUG((EFI_D_ERROR, "Buffer is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (Lba > LAST_BLOCK) {
    DEBUG((EFI_D_ERROR, "Offset value is invalid %d > %d\n", Lba, LAST_BLOCK));
    return EFI_INVALID_PARAMETER;
  }

  if ((BufferSize % gFlash->BlockSize) != 0) {
    DEBUG((EFI_D_ERROR, "Buffer size is not multiple of blocksize (%d)\n",
			gFlash->BlockSize));
    return EFI_BAD_BUFFER_SIZE;
  }

  //Read block
  Status = DspiFlashRead(((UINTN)Lba * gFlash->BlockSize), BufferSize, Buffer);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Read block fails: %x\n", Status));
    return Status;
  }

  DEBUG((EFI_D_INFO,"Dspi: %d bytes read %a\n",
		BufferSize, Status ? "ERROR" : "OK"));
  return Status;
}

EFI_STATUS
EFIAPI
DspiErase (
  IN  EFI_BLOCK_IO_PROTOCOL  *This,
  IN  UINT32                 MediaId,
  IN  EFI_LBA                Lba,
  IN  UINTN                  BufferSize
)
{
  EFI_STATUS Status;

  if (Lba > LAST_BLOCK) {
    DEBUG((EFI_D_ERROR, "Offset value is invalid %d > %d\n", Lba, LAST_BLOCK));
    return EFI_INVALID_PARAMETER;
  }

  if ((BufferSize % gFlash->BlockSize) != 0) {
    DEBUG((EFI_D_ERROR, "Buffer size is not multiple of blocksize (%d)\n",
                     gFlash->BlockSize));
    return EFI_BAD_BUFFER_SIZE;
  }

  // Erase data
  Status = DspiFlashErase(((UINTN)Lba * gFlash->BlockSize), BufferSize * NUM_OF_SUBSECTOR);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Block Erase fails: %x\n", Status));
    return Status;
  }
  DEBUG((EFI_D_INFO,"Dspi: %d bytes erased %a\n",
		BufferSize * NUM_OF_SUBSECTOR, Status ? "ERROR" : "OK"));

  return Status;
}

EFI_STATUS
EFIAPI
DspiWrite (
  IN  EFI_BLOCK_IO_PROTOCOL  *This,
  IN  UINT32                 MediaId,
  IN  EFI_LBA                Lba,
  IN  UINTN                  BufferSize,
  IN  VOID                   *Buffer
)
{
  EFI_STATUS Status;

  if (Buffer == NULL) {
    DEBUG((EFI_D_ERROR, "Buffer is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (Lba > LAST_BLOCK) {
    DEBUG((EFI_D_ERROR, "Offset value is invalid %d > %d\n", Lba, LAST_BLOCK));
    return EFI_INVALID_PARAMETER;
  }

  if ((BufferSize % gFlash->BlockSize) != 0) {
    DEBUG((EFI_D_ERROR, "Buffer size is not multiple of blocksize (%d)\n",
			gFlash->BlockSize));
    return EFI_BAD_BUFFER_SIZE;
  }

  // Program data
  Status = DspiFlashWrite(((UINTN)Lba * gFlash->BlockSize), BufferSize, Buffer);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Block write fails: %x\n", Status));
    return Status;
  }
  DEBUG((EFI_D_INFO,"Dspi: %d bytes written %a\n",
		BufferSize, Status ? "ERROR" : "OK"));

  return Status;
}

EFI_STATUS
DspiInit (
  EFI_BLOCK_IO_MEDIA *gDspiMedia
  )
{

  EFI_STATUS  Status;

  Status = DspiDetect();

  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Dspi initialization failure: Status: %x\n", Status));
    return Status;
  }

  //Patch EFI_BLOCK_IO_MEDIA structure.
  if (gDspiMedia) {
    gDspiMedia->BlockSize = gFlash->BlockSize;
    gDspiMedia->LastBlock = LAST_BLOCK;
  }

  return EFI_SUCCESS;
}
