/** @MmcInterface.c

  Functions for providing Library interface APIs.

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD
  License which accompanies this distribution. The full text of the license
  may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/Sdxc.h>

extern struct Mmc *gMmc;

EFI_STATUS
DetectMmcPresence (
  OUT UINT8*  Data
  )
{
  if (Getcd() == 0) {
    DEBUG((EFI_D_ERROR, "MMC, No Card Present\n"));
    *Data = FALSE;
    return EFI_NO_MEDIA;
  } else {
    *Data = TRUE;
    return EFI_SUCCESS;
  }
}

EFI_STATUS
MmcRcvResp (
  IN   UINT32   RespType,
  OUT  UINT32*  Buffer
  )
{
  EFI_STATUS Status;
  struct SdxcCfg *Cfg = gMmc->Private;
  struct SdxcRegs *Regs = (struct SdxcRegs *)Cfg->SdxcBase;

  Status = RecvResp(Regs, NULL, RespType, Buffer);

  return Status;
}

EFI_STATUS
MmcSendCommand (
  IN  struct SdCmd *Cmd
  )
{
  return SendCmd(Cmd, NULL);
}

EFI_STATUS
MmcSendReset (
  VOID
  )
{
  gMmc->HasInit = 0;
  return MmcInit(gMmc);
}

EFI_STATUS
InitMmc (
  IN EFI_BLOCK_IO_MEDIA *Media
  )
{
  EFI_STATUS Status;

  Status = DoMmcInfo();

  if (Status == EFI_SUCCESS) {
    Media->BlockSize = gMmc->BlockDev.Blksz;
    Media->LastBlock = gMmc->BlockDev.Lba;
  }

  return Status;
}

VOID
DestroyMmc (
  IN VOID
  )
{
  if (gMmc) {
    FreePool(gMmc->Cfg);
    FreePool(gMmc->Private);
    FreePool(gMmc);
  }
}

EFI_STATUS
MmcInitialize (
  VOID
  )
{
  if (EFI_ERROR(SdxcMmcInit()))
    return EFI_DEVICE_ERROR;

  return EFI_SUCCESS;
}
