/** QspiBlockIoApi.c
  APIs for read, write, initialize etc of block IO layer

  Copyright (c) 2016, freescale semiconductor, inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the bsd
  license which accompanies this distribution. the full text of the license may
  be found at http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include <Library/Qspi.h>

extern struct QspiFlash *GFlash;

EFI_STATUS
EFIAPI
QspiRead (
  IN EFI_BLOCK_IO_PROTOCOL          *This,
  IN UINT32                         MediaId,
  IN EFI_LBA                        Offset,
  IN UINTN                          BufferSize,
  OUT VOID                          *Buffer
  )
{
  EFI_STATUS Status;

  if (Buffer == NULL) {
    DEBUG((EFI_D_ERROR, "Buffer Is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  if ((Offset/GFlash->SectorSize) > QSPI_LAST_BLOCK) {
    DEBUG((EFI_D_ERROR, "Read : Offset Value Is Invalid 0x%x > 0x%x\n", Offset, QSPI_LAST_BLOCK));
    return EFI_INVALID_PARAMETER;
  }

  //Read Block
  Status = QspiFlashRead((UINTN)Offset, BufferSize, Buffer);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Read Block Fails: 0x%x\n", Status));
    return Status;
  }

  DEBUG((EFI_D_RELEASE,"QSPI : %d Bytes Read %a\n",
		BufferSize, Status ? "ERROR" : "OK"));
  return Status;
}

EFI_STATUS
EFIAPI
QspiErase (
  IN  EFI_BLOCK_IO_PROTOCOL  *This,
  IN  UINT32                 MediaId,
  IN  EFI_LBA                Offset,
  IN  UINTN                  BufferSize
)
{
  EFI_STATUS Status;

  if ((Offset/GFlash->SectorSize) > QSPI_LAST_BLOCK) {
    DEBUG((EFI_D_ERROR, "Erase : Offset Value Is Invalid 0x%x > 0x%x\n", Offset, QSPI_LAST_BLOCK));
    return EFI_INVALID_PARAMETER;
  }

  if ((BufferSize % GFlash->BlockSize) != 0) {
    DEBUG((EFI_D_ERROR, "Buffer Size Is Not Multiple Of Blocksize (%d)\n",
                     GFlash->BlockSize));
    return EFI_BAD_BUFFER_SIZE;
  }

  // Erase Data
  Status = QspiFlashErase((UINTN)Offset, BufferSize);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Block Erase Fails: 0x%x\n", Status));
    return Status;
  }
  DEBUG((EFI_D_RELEASE,"QSPI : %d Bytes Erased %a\n",
		BufferSize, Status ? "ERROR" : "OK"));

  return Status;
}

EFI_STATUS
EFIAPI
QspiWrite (
  IN  EFI_BLOCK_IO_PROTOCOL  *This,
  IN  UINT32                 MediaId,
  IN  EFI_LBA                Offset,
  IN  UINTN                  BufferSize,
  IN  VOID                   *Buffer
)
{
  EFI_STATUS Status;

  if (Buffer == NULL) {
    DEBUG((EFI_D_ERROR, "Buffer Is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  if ((Offset/GFlash->SectorSize) > QSPI_LAST_BLOCK) {
    DEBUG((EFI_D_ERROR, "Offset Value Is Invalid 0x%x > 0x%x\n", Offset, QSPI_LAST_BLOCK));
    return EFI_INVALID_PARAMETER;
  }

  // Program Data
  Status = QspiFlashWrite((UINTN)Offset, BufferSize, Buffer);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Block Write Fails: %x\n", Status));
    return Status;
  }
  DEBUG((EFI_D_RELEASE,"QSPI : %d Bytes Written %a\n",
		BufferSize, Status ? "ERROR" : "OK"));

  return Status;
}

EFI_STATUS
QspiInit (
  EFI_BLOCK_IO_MEDIA *GQspiMedia
  )
{
  EFI_STATUS  Status;

  Status = QspiDetect();

  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Qspi Initialization Failure: Status: %x\n", Status));
    return Status;
  }

  //Patch EFI_BLOCK_IO_MEDIA Structure.
  if (GQspiMedia) {
    GQspiMedia->BlockSize = GFlash->BlockSize;
    GQspiMedia->LastBlock = QSPI_LAST_BLOCK;
  }

  return EFI_SUCCESS;
}
