/** @DspiFlashProbe.c

  Probing function to detect device connected to DSPI controller

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Library/Dspi.h"

/* Read Commands Array */
static UINT8 ReadCmdsArray[] = {
  READ_ARRAY_SLOW,
  READ_ARRAY_FAST,
  READ_DUAL_OUTPUT_FAST,
  READ_DUAL_IO_FAST,
  READ_QUAD_OUTPUT_FAST,
  READ_QUAD_IO_FAST,
};

CONST struct SpiFlashParameters DspiFlashTable[] = {
#ifdef CONFIG_SIMULATOR
  {"SST25WF512", 0xbf2501, 0x0,
	64 * 1024, 1, 0, SECT_4K | SST_WP},
  {"SST25WF010", 0xbf2502, 0x0,
	64 * 1024, 2, 0, SECT_4K | SST_WP},
  {"SST25WF020", 0xbf2503, 0x0,
	64 * 1024, 4, 0, SECT_4K | SST_WP},
  {"SST25WF040", 0xbf2504, 0x0,
	64 * 1024, 8, 0, SECT_4K | SST_WP},
#endif
#ifdef CONFIG_RDB
  {"N25Q128",  0x20ba18, 0x0,
	64 * 1024, 256,  RD_FULL, WR_QPP},
  {"N25Q128A", 0x20bb18, 0x0,
	64 * 1024, 256,  RD_FULL, WR_QPP},
#endif
};

struct DspiFlash *
ValidateParameters (
  IN  struct DspiSlave *Dspi,
  IN  UINT8 *Id
  )
{
  CONST struct SpiFlashParameters *Parameters;
  struct DspiFlash *Flash;
  UINT8 Cmd;
  UINT16 Jedec = Id[1] << 8 | Id[2];
  UINT16 ExtJedec = Id[3] << 8 | Id[4];
  UINT8 CurrBank = 0;

  Parameters = DspiFlashTable;
  for (; Parameters->Name != NULL; Parameters++) {
    if ((Parameters->Jedec >> 16) == Id[0]) {
      if ((Parameters->Jedec & 0xFFFF) == Jedec) {
	if (Parameters->ExtJedec == 0)
	  break;
	else if (Parameters->ExtJedec == ExtJedec)
	  break;
      }
    }
  }

  if (!Parameters->Name) {
    DEBUG((EFI_D_ERROR, "Unsupported DSPI Flash IDs: "));
    DEBUG((EFI_D_ERROR, "Manuf %02x, Jedec %04x, ExtJedec %04x\n",
		Id[0], Jedec, ExtJedec));
    return NULL;
  }
  Flash = (struct DspiFlash *)AllocatePool(sizeof(struct DspiFlash));
  InternalMemZeroMem(Flash, sizeof(struct DspiFlash));
  if (!Flash) {
    DEBUG((EFI_D_ERROR, "Failed To Allocate DspiFlash\n"));
    return NULL;
  }

  /* Assign Spi Data */
  Flash->Dspi = Dspi;
  Flash->Name = Parameters->Name;
  Flash->MemoryMap = Dspi->Slave.MemoryMap;
  Flash->DualFlash = Flash->Dspi->Slave.Option;

  /* Assign DspiFlash Ops */
  Flash->Write = DspiWriteOps;
  Flash->Erase = DspiEraseOps;
  Flash->Read = DspiReadOps;

  /* Compute The Flash Size */
  Flash->Shift = (Flash->DualFlash & DUAL_PARALLEL_FLASH) ? 1 : 0;
  /*
   * The Spansion S25FL032P And S25FL064P Have 256b Pages, Yet Use The
   * 0x4d00 Extended JEDEC Code. The Rest Of The Spansion Flashes With
   * The 0x4d00 Extended JEDEC Code Have 512b Pages. All Of The Others
   * Have 256b Pages.
   */
  if (ExtJedec == 0x4d00) {
    if ((Jedec == 0x0215) || (Jedec == 0x216))
      Flash->PageSize = 256;
    else
      Flash->PageSize = 512;
  } else {
    Flash->PageSize = 256;
  }
  Flash->PageSize <<= Flash->Shift;
  Flash->SectorSize = Parameters->SectorSize << Flash->Shift;
  Flash->Size = Flash->SectorSize * Parameters->NoOfSectors << Flash->Shift;

  /* Compute Erase Sector And Command */
  if (Parameters->Flags & SECT_4K) {
    Flash->EraseCmd = ERASE_4K;
    Flash->EraseSize = 4096 << Flash->Shift;
  } else if (Parameters->Flags & SECT_32K) {
    Flash->EraseCmd = ERASE_32K;
    Flash->EraseSize = 32768 << Flash->Shift;
  } else {
    Flash->EraseCmd = ERASE_64K;
    Flash->EraseSize = Flash->SectorSize;
  }

  if (!AsciiStrnCmp((CONST CHAR8 *)Parameters->Name, "N25Q128", AsciiStrLen("N25Q128")))
    Flash->BlockSize = Flash->SectorSize/NUM_OF_SUBSECTOR;
  else
    Flash->BlockSize = Flash->SectorSize;

  /* Look for The Fastest Read Cmd */
  Cmd = GenericFls(Parameters->EnumRdCmd & Flash->Dspi->Slave.OpModeRx);
  if (Cmd && (Cmd < (ARRAY_SIZE(ReadCmdsArray) + 1))) {
    Cmd = ReadCmdsArray[Cmd - 1];
    Flash->ReadCmd = Cmd;
  } else {
    /* Go for default Supported Read Cmd */
    Flash->ReadCmd = READ_ARRAY_FAST;
  }

  /* Not Require To Look for Fastest Only Two Write Cmds Yet */
  if (Parameters->Flags & WR_QPP && Flash->Dspi->Slave.OpModeTx & SPI_COMMON_OPM_TX_QPP)
    Flash->WriteCmd = QUAD_PAGE_PROGRAM;
  else
    /* Go for default Supported Write Cmd */
    Flash->WriteCmd = BYTE_PROGRAM;

  /* Read DummyByte: Dummy Byte Is Determined Based On The
   * Dummy Cycles Of A Particular Command.
   * Fast Commands - DummyByte = DummyCycles/8
   * I/O Commands- DummyByte = (DummyCycles * No.Of Lines)/8
   * for I/O Commands Except Cmd[0] Everything Goes On No.Of Lines
   * Based On Particular Command But Incase Of Fast Commands Except
   * Data All Go On Single Line Irrespective Of Command.
   */
  switch (Flash->ReadCmd) {
  case READ_QUAD_IO_FAST:
    Flash->DummyByte = 2;
    break;
  case READ_ARRAY_SLOW:
    Flash->DummyByte = 0;
    break;
  default:
    Flash->DummyByte = 1;
  }

  /* Poll Cmd Selection */
  Flash->PollCmd = READ_STATUS;


  if (Parameters->Flags & E_FSR)
    Flash->PollCmd = FLAG_STATUS;


  /* Configure the BAR - discover bank cmds and read current bank */
  if (Flash->Size > SPI_COMMON_FLASH_16MB_BOUN) {
    INT32 Ret;

    Flash->BankReadCmd = (Id[0] == 0x01) ?
                         DSPI_CMD_BANKADDR_BRRD : DSPI_CMD_EXTNADDR_RDEAR;
    Flash->BankWriteCmd = (Id[0] == 0x01) ?
                         DSPI_CMD_BANKADDR_BRWR : DSPI_CMD_EXTNADDR_WREAR;

    Ret = DspiCommonRead(Flash, &Flash->BankReadCmd, 1,
                             &CurrBank, 1);
    if (Ret != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR,"Fail to read bank addr register\n"));
      return NULL;
    }
    Flash->BankCurr = CurrBank;
  } else {
    Flash->BankCurr = CurrBank;
  }
#if 0
  if (IsDataflash(Dspi->Slave.Bus, Dspi->Slave.Cs)) {
    Flash->PollCmd = DSPI_CMD_ATMEL_READ_STATUS;
    Flash->WriteCmd = DSPI_CMD_ATMEL_PAGE_PROGRAM;
    if (Parameters->Flags & SECT_32K)
      Flash->EraseCmd = DSPI_CMD_ATMEL_ERASE_32K;
  }
#endif
  return Flash;
}

/* enable the W#/Vpp signal to disable writing to the status register */
EFI_STATUS
DspiEnableWritePin (
  IN  struct DspiFlash *Flash
  )
{
  return EFI_SUCCESS;
}

/**
 * DspiProbeDevice() - Probe for a SPI flash device on a bus
 *
 * @Dspi: Bus to probe
 */
struct DspiFlash *
DspiProbeDevice (
  IN  struct DspiSlave *Dspi
  )
{
  struct DspiFlash *Flash = NULL;
  UINT8 Id[5] = {0};
  INT32 Ret;
  UINT8 Cmd;

  /* Claim Dspi Bus */
  Ret = DspiClaimBus(Dspi);
  if (Ret) {
    DEBUG((EFI_D_ERROR, "SF: Failed To Claim SPI Bus: %d\n", Ret));
    goto ErrClaimBus;
  }

  /* Read The ID Codes */
  Cmd = READ_JEDEC_ID;
  Ret = DspiReadWrite(Dspi, &Cmd, 1, NULL, Id,
		sizeof(Id));
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "SF: Failed To Get Ids\n"));
    goto ErrClaimBus;
  }

  /* Validate Parameters From SpiFlashParameters Table */
  Flash = ValidateParameters(Dspi, Id);
  if (!Flash)
    goto ErrClaimBus;

  /* Set The Quad Enable Bit - Only for Quad Commands */
  if ((Flash->ReadCmd == READ_QUAD_OUTPUT_FAST) ||
      (Flash->ReadCmd == READ_QUAD_IO_FAST) ||
      (Flash->WriteCmd == QUAD_PAGE_PROGRAM)) {
    if (DspiSetQeb(Flash, Id[0]) != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "Fail To Set QEB for %02x\n", Id[0]));
      return NULL;
    }
  }

  DEBUG((EFI_D_RELEASE, "Detected DSPI Flash %a With Page Size ", Flash->Name));
  PrintMemorySize(Flash->PageSize, (CONST INT8 *)"Erase Size ");
  PrintMemorySize(Flash->EraseSize, (CONST INT8 *)"Total ");
  PrintMemorySize(Flash->Size, (CONST INT8 *)"");
  DEBUG((EFI_D_RELEASE, "\n"));

  if (Flash->MemoryMap)
    DEBUG((EFI_D_INFO, ", Mapped At %p\n", Flash->MemoryMap));

  if (((Flash->DualFlash == SINGLE_FLASH) &&
       (Flash->Size > SPI_COMMON_FLASH_16MB_BOUN)) ||
       ((Flash->DualFlash > SINGLE_FLASH) &&
       (Flash->Size > SPI_COMMON_FLASH_16MB_BOUN << 1))) {
    DEBUG((EFI_D_WARN, "Warning - Only Lower 16MiB Accessible, "\
  		"Full Access #define CONFIG_DSPI_FLASH_BAR\n"));
  }

  if (DspiEnableWritePin(Flash) != EFI_SUCCESS)
    DEBUG((EFI_D_WARN,"Enable WP pin failed\n"));

  /* Release Dspi Bus */
  DspiReleaseBus(Dspi);
  return Flash;

ErrClaimBus:
  DspiReleaseBus(Dspi);
  DspiFreeSlave(Dspi);
  return NULL;
}
