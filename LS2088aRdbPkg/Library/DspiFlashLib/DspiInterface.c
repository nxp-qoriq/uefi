/** DspiInterface.C
  Dspi Flash Library Containing Functions for Read, Write, Initialize, Set Speed
  Etc

  Copyright (c) 2016, Freescale Semiconductor, Inc. All rights reserved.

  This Program And The Accompanying Materials
  Are Licensed And Made Available Under The Terms And Conditions Of The BSD
  License Which Accompanies This Distribution. The Full Text Of The License May
  Be Found At http://Opensource.Org/Licenses/Bsd-License.Php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include <Library/Dspi.h>

struct DspiFlash *gFlash;

EFI_STATUS
DspiFlashErase (
  IN  UINT32 Offset,
  IN  UINT64 Len
  )
{
  /* Consistency Checking */
  if (Offset + Len > gFlash->Size) {
    DEBUG((EFI_D_ERROR, "ERROR: Attempting %S Past Flash Size (%#X)\n",
		Len, gFlash->Size));
    return 1;
  }

  return gFlash->Erase(gFlash, Offset, Len);
}

EFI_STATUS
DspiFlashWrite (
  IN  UINT32 Offset,
  IN  UINT64 Len,
  IN  CONST VOID *Buf
  )
{
  /* Consistency Checking */
  if (Offset + Len > gFlash->Size) {
    DEBUG((EFI_D_ERROR, "ERROR: Attempting %s Past Flash Size (0x%x)\n",
		Len, gFlash->Size));
    return EFI_INVALID_PARAMETER;
  }

  return gFlash->Write(gFlash, Offset, Len, Buf);
}

EFI_STATUS
DspiFlashRead (
  IN  UINT32 Offset,
  IN  UINT64 Len,
  OUT VOID *Buf
  )
{
  /* Consistency Checking */
  if (Offset + Len > gFlash->Size) {
    DEBUG((EFI_D_ERROR, "ERROR: Attempting %s Past Flash Size (0x%x)\n",
		Len, gFlash->Size));
    return EFI_INVALID_PARAMETER;
  }

  return gFlash->Read(gFlash, Offset, Len, Buf);
}

VOID
DspiFlashFree (
  VOID
  )
{
  /* Clear status register */
  MmioWriteBe32((UINTN)&gFlash->Dspi->Regs->Sr, SPI_SR_CLEAR);

  FreePool(gFlash->Dspi);
  FreePool(gFlash);
}

UINT8
DspiSupported (
  VOID
  )
{
  return gFlash->Dspi->Slave.HasInit;
}

EFI_STATUS
DspiDetect(
  VOID
  )
{

  struct DspiSlave *Dspi;

  UINT32 Bus = DEFAULT_BUS;
  UINT32 Cs = DEFAULT_CS;
  UINT32 Speed = DEFAULT_SPEED;
  UINT32 Mode = DEFAULT_MODE;
  struct DspiFlash *New = NULL;
  
  Dspi = DspiSetupSlave(Bus, Cs, Speed, Mode);
  if (!Dspi) {
    DEBUG((EFI_D_ERROR, "DSPI : Failed To Set Up Slave\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  New = DspiProbeDevice(Dspi);

  if (!New)
    return EFI_OUT_OF_RESOURCES;

  if (gFlash)
    DspiFlashFree();

  gFlash = New;
  gFlash->Dspi->Slave.HasInit = TRUE;

  return EFI_SUCCESS;
}

