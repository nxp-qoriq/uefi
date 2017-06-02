/** QspiInterface.c
  Qspi flash library containing functions for read, write, initialize etc

  Copyright (c) 2016, freescale semiconductor, inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the bsd
  license which accompanies this distribution. the full text of the license may
  be found at http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include <Library/Qspi.h>

struct QspiFlash *GFlash;

EFI_STATUS
QspiFlashErase (
  IN  UINT32 Offset,
  IN  UINT64 Len
  )
{
  UINT32 EraseSize, EraseAddr;
  UINT8 Cmd[SPI_COMMON_FLASH_CMD_LEN];
  INT32 Ret = -1;

  /* Consistency Checking */
  if (Offset + Len > GFlash->Size) {
    DEBUG((EFI_D_ERROR, "ERROR: Attempting %d Past Flash Size (0x%x)\n",
		Len, GFlash->Size));
    return EFI_INVALID_PARAMETER;
  }

  EraseSize = GFlash->EraseSize;

  if (Offset % EraseSize || Len % EraseSize) {
    DEBUG((EFI_D_ERROR, "QSPI Erase Offset/Length Not"\
          "Multiple Of Erase Size\n"));
    return EFI_INVALID_PARAMETER;
  }

  Cmd[0] = GFlash->EraseCmd;
  while (Len) {
    EraseAddr = Offset;

    QspiCreateAddr(EraseAddr, Cmd);

    Ret = QspiCommonWrite(GFlash, Cmd, sizeof(Cmd), NULL, 0);
    if (Ret != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "QSPI Erase Failed\n"));
      break;
    }

    Offset += EraseSize;
    Len -= EraseSize;
  }

  return Ret;
}

EFI_STATUS
QspiFlashWrite (
  IN  UINT32 Offset,
  IN  UINT64 Len,
  IN  CONST VOID *Buf
  )
{
  UINT64 ByteAddr, PageSize;
  UINT32 WriteAddr;
  UINT64 ChunkLen = 1, Actual;
  UINT8 Cmd[SPI_COMMON_FLASH_CMD_LEN];
  INT32 Ret = -1;

  /* Consistency Checking */
  if (Offset + Len > GFlash->Size) {
    DEBUG((EFI_D_ERROR, "ERROR: Attempting %d Past Flash Size (0x%x)\n",
		Len, GFlash->Size));
    return EFI_INVALID_PARAMETER;
  }

  PageSize = GFlash->PageSize;

  Cmd[0] = GFlash->WriteCmd;
  for (Actual = 0; Actual < Len; Actual += ChunkLen) {
    WriteAddr = Offset;
    ByteAddr = Offset % PageSize;
    ChunkLen = Min(Len - Actual, (PageSize - ByteAddr));

    if (GFlash->Qspi->Slave.MaxWriteSize)
      ChunkLen = Min(ChunkLen,
		GFlash->Qspi->Slave.MaxWriteSize);

    QspiCreateAddr(WriteAddr, Cmd);

    Ret = QspiCommonWrite(GFlash, Cmd, sizeof(Cmd),
                         Buf + Actual, ChunkLen);
    if (Ret != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "QSPI Write Failed\n"));
      break;
    }

    Offset += ChunkLen;
  }

  return Ret;
}

EFI_STATUS
QspiFlashRead (
  IN  UINT32 Offset,
  IN  UINT64 Len,
  OUT VOID *Buf
  )
{
  UINT8 CmdSz = 0;
  UINT8 *Cmd = NULL;
  UINT32 RemainLen, ReadLen, ReadAddr;
  INT32 BankSel = 0;
  INT32 Ret = -1;

  /* Consistency Checking */
  if (Offset + Len > GFlash->Size) {
    DEBUG((EFI_D_ERROR, "ERROR: Attempting %d Past Flash Size (0x%x)\n",
		Len, GFlash->Size));
    return EFI_INVALID_PARAMETER;
  }
  /* Handle Memory-Mapped SPI */
  if (GFlash->MemoryMap) {
    Ret = QspiClaimBus(GFlash->Qspi);
    if (Ret != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "Unable To Claim QSPI Bus\n"));
      return Ret;
    }
    QspiXfer(GFlash->Qspi, 0, NULL, NULL, SPI_COMMON_XFER_MMAP);
    InternalMemCopyMem(Buf, GFlash->MemoryMap + Offset, Len);
    QspiXfer(GFlash->Qspi, 0, NULL, NULL, SPI_COMMON_XFER_MMAP_END);
    QspiReleaseBus(GFlash->Qspi);
    return EFI_SUCCESS;
  }

  CmdSz = SPI_COMMON_FLASH_CMD_LEN + GFlash->DummyByte;

  Cmd = (UINT8 *)AllocatePool(CmdSz);
  if (!Cmd) {
    DEBUG((EFI_D_ERROR, "QSPI: Out Of Memory\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  Cmd[0] = GFlash->ReadCmd;
  while (Len) {
    ReadAddr = Offset;

    RemainLen = ((SPI_COMMON_FLASH_16MB_BOUN << GFlash->Shift) *
                 (BankSel + 1)) - Offset;
    if (Len < RemainLen)
      ReadLen = Len;
    else
      ReadLen = RemainLen;

    QspiCreateAddr(ReadAddr, Cmd);
    Ret = QspiCommonRead(GFlash, Cmd, CmdSz, Buf, ReadLen);
    if (Ret != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "QSPI Read Failed\n"));
      break;
    }

    Offset += ReadLen;
    Len -= ReadLen;
    Buf += ReadLen;
  }

  FreePool(Cmd);
  return Ret;
}

VOID
QspiFlashFree (
  VOID
  )
{
  FreePool(GFlash->Qspi);
  FreePool(GFlash);
}

EFI_STATUS
QspiDetect(
  VOID
  )
{
  struct QspiSlave *Qspi;

  UINT32 Bus = DEFAULT_BUS;
  UINT32 Cs = DEFAULT_CS;
  UINT32 Speed = DEFAULT_SPEED;
  UINT32 Mode = DEFAULT_MODE;
  struct QspiFlash *New = NULL;


  Qspi = QspiSetupSlave(Bus, Cs, Speed, Mode);
  if (!Qspi) {
    DEBUG((EFI_D_ERROR, "QSPI : Failed To Set Up Slave\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  New = QspiProbeDevice(Qspi);

  if (!New)
    return EFI_OUT_OF_RESOURCES;

  if (GFlash)
    QspiFlashFree();

  GFlash = New;

  return EFI_SUCCESS;
}

