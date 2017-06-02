/** @QspiFlashProbe.c

  Probing function to detect device connected to qspi controller

  Copyright (c) 2016, freescale semiconductor, inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the bsd
  license which accompanies this distribution. the full text of the license may
  be found at http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include "QspiFlashLib.h"

/* Read Commands Array */
static UINT8 ReadCmdsArray[] = {
  READ_ARRAY_SLOW,
  READ_ARRAY_FAST,
  READ_DUAL_OUTPUT_FAST,
  READ_DUAL_IO_FAST,
  READ_QUAD_OUTPUT_FAST,
  READ_QUAD_IO_FAST,
};

struct SpiFlashParameters QspiFlashTable[] = {
  {"SST25WF512", 0xbf2501, 0x0,
	64 * 1024, 1, 0, SECT_4K | SST_WP},
  {"SST25WF010", 0xbf2502, 0x0,
	64 * 1024, 2, 0, SECT_4K | SST_WP},
  {"SST25WF020", 0xbf2503, 0x0,
	64 * 1024, 4, 0, SECT_4K | SST_WP},
  {"SST25WF040", 0xbf2504, 0x0,
	64 * 1024, 8, 0, SECT_4K | SST_WP},
  {"M25P10",     0x202011, 0x0,
	32 * 1024, 4, RD_NORM,   0},
  {"M25P20",     0x202012, 0x0,
	64 * 1024, 4, RD_NORM,   0},
  {"M25P40",     0x202013, 0x0,
	64 * 1024, 8, RD_NORM,   0},
  {"M25P80",     0x202014, 0x0,
	64 * 1024, 16, RD_NORM,  0},
  {"M25P16",     0x202015, 0x0,
	64 * 1024, 32, RD_NORM,  0},
  {"M25PE16",    0x208015, 0x1000,
	64 * 1024, 32, RD_NORM,  0},
  {"M25PX16",    0x207115, 0x1000,
	64 * 1024, 32, RD_EXTN,  0},
  {"M25P32",     0x202016, 0x0,
	64 * 1024, 64, RD_NORM,  0},
  {"M25P64",     0x202017, 0x0,
	64 * 1024, 128, RD_NORM, 0},
  {"M25P128",    0x202018, 0x0,
	256 * 1024,64, RD_NORM,  0},
  {"M25PX64",    0x207117, 0x0,
	64 * 1024, 128, RD_NORM, SECT_4K},
  {"N25Q32",     0x20ba16, 0x0,
	64 * 1024, 64, RD_FULL,  WR_QPP | SECT_4K},
  {"N25Q32A",    0x20bb16, 0x0,
	64 * 1024, 64, RD_FULL,  WR_QPP | SECT_4K},
  {"N25Q64",     0x20ba17, 0x0,
	64 * 1024, 128, RD_FULL, WR_QPP | SECT_4K},
  {"N25Q64A",    0x20bb17, 0x0,
	64 * 1024, 128, RD_FULL, WR_QPP | SECT_4K},
  {"N25Q128",    0x20ba18, 0x0,
	64 * 1024, 256, RD_FULL, WR_QPP},
  {"N25Q128A",   0x20bb18, 0x0,
	64 * 1024, 256, RD_FULL, WR_QPP},
  {"N25Q256",    0x20ba19, 0x0,
	64 * 1024, 512, RD_FULL, WR_QPP | SECT_4K},
  {"N25Q256A",   0x20bb19, 0x0,
	64 * 1024, 512, RD_FULL, WR_QPP | SECT_4K},
  {"N25Q512",    0x20ba20, 0x0,
	64 * 1024, 1024, RD_FULL, WR_QPP | E_FSR | SECT_4K},
  {"N25Q512A",   0x20bb20, 0x0,
	64 * 1024, 1024, RD_FULL, WR_QPP | E_FSR | SECT_4K},
  {"N25Q1024",   0x20ba21, 0x0,
	64 * 1024, 2048, RD_FULL, WR_QPP | E_FSR | SECT_4K},
  {"N25Q1024A",  0x20bb21, 0x0,
	64 * 1024, 2048, RD_FULL, WR_QPP | E_FSR | SECT_4K},
  {"S25FL128S_256K", 0x012018, 0x4d0000,
	256 * 1024,64,   RD_FULL, WR_QPP},
  {"S25FL128S_64K",  0x012018, 0x4d0100,
       64 * 1024, 256,  RD_FULL, WR_QPP},
};

struct QspiFlash *
ValidateParameters (
  IN  struct QspiSlave *Qspi,
  IN  UINT8 *Id
  )
{
  struct SpiFlashParameters *Parameters;
  struct QspiFlash *Flash;
  UINT8 Cmd;
  UINT16 Jedec = Id[1] << 8 | Id[2];
  UINT32 ExtJedec = Id[3] << 16 | Id[4] << 8;

  if (Id[0] == 0 && Jedec == 0 && ExtJedec == 0) {
    DEBUG((EFI_D_ERROR, "Unsupported QSPI Flash IDs: "));
    DEBUG((EFI_D_ERROR, "Manuf %02x, Jedec %04x, ExtJedec %04x\n",
		Id[0], Jedec, ExtJedec));
    return NULL;
  }

  Parameters = QspiFlashTable;
  for (; Parameters->Name != NULL; Parameters++) {
    if ((Parameters->Jedec >> 16) == Id[0]) {
      if ((Parameters->Jedec & 0xFFFF) == Jedec) {
	if (Parameters->ExtJedec == 0)
	  break;
	else if (Parameters->ExtJedec == ExtJedec)
	  break;
       else if (Parameters->ExtJedec == (ExtJedec | Id[5]))
         break;
      }
    }
  }

  if (!Parameters->Name) {
    DEBUG((EFI_D_ERROR, "Unsupported QSPI Flash IDs: "));
    DEBUG((EFI_D_ERROR, "Manuf %02x, Jedec %04x, ExtJedec %04x\n",
		Id[0], Jedec, ExtJedec));
    return NULL;
  }
  Flash = (struct QspiFlash *)AllocateRuntimePool(sizeof(struct QspiFlash));
  InternalMemZeroMem(Flash, sizeof(struct QspiFlash));
  if (!Flash) {
    DEBUG((EFI_D_ERROR, "Failed To Allocate QspiFlash\n"));
    return NULL;
  }

  /* Assign Spi Data */
  Flash->Qspi = Qspi;
  Flash->Name = Parameters->Name;
  Flash->MemoryMap = Qspi->Slave.MemoryMap;
  Flash->DualFlash = Flash->Qspi->Slave.Option;

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

  Flash->BlockSize = Flash->SectorSize;

  /* Look for The Fastest Read Cmd */
  Cmd = GenericFls(Parameters->EnumRdCmd);
  if (Cmd) {
    Cmd = ReadCmdsArray[Cmd - 1];
    Flash->ReadCmd = Cmd;
  } else {
    /* Go for default Supported Read Cmd */
    Flash->ReadCmd = READ_ARRAY_FAST;
  }

  /* Not Require To Look for Fastest Only Two Write Cmds Yet */
  if (Parameters->Flags & WR_QPP)
    Flash->WriteCmd = QUAD_PAGE_PROGRAM;
  else
    /* Go for default Supported Write Cmd */
    Flash->WriteCmd = BYTE_PROGRAM;

  /* Read DummyByte: Dummy Byte Is Determined Based On The
   * Dummy Cycles Of a Particular Command.
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

  return Flash;
}

/**
 * QspiProbeDevice() - Probe for a SPI Flash Device On a Bus
 *
 * @Qspi: Bus To Probe
 */
struct QspiFlash *
QspiProbeDevice (
  IN  struct QspiSlave *Qspi
  )
{
  struct QspiFlash *Flash = NULL;
  UINT8 Id[5] = {0};
  INT32 Ret;
  UINT8 Cmd;

  /* Claim Qspi Bus */
  Ret = QspiClaimBus(Qspi);
  if (Ret) {
    DEBUG((EFI_D_ERROR, "Failed To Claim SPI Bus: %d\n", Ret));
    goto ErrClaimBus;
  }

  /* Read The ID Codes */
  Cmd = READ_JEDEC_ID;
  Ret = QspiReadWrite(Qspi, &Cmd, 1, NULL, Id, sizeof(Id));
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "Failed To Get IdCodes\n"));
    goto ErrClaimBus;
  }

  /* Validate Parameters From SpiFlashParameters Table */
  Flash = ValidateParameters(Qspi, Id);
  if (!Flash)
    goto ErrClaimBus;

  /* Set The Quad Enable Bit - Only for Quad Commands */
  if ((Flash->ReadCmd == READ_QUAD_OUTPUT_FAST) ||
      (Flash->ReadCmd == READ_QUAD_IO_FAST) ||
      (Flash->WriteCmd == QUAD_PAGE_PROGRAM)) {
    if (QspiSetQeb(Flash, Id[0]) != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "Fail To Set QEB for %02x\n", Id[0]));
      FreePool(Flash);
      goto ErrClaimBus;
    }
  }

  DEBUG((EFI_D_RELEASE, "Detected QSPI Flash %a With Page Size ", Flash->Name));
  PrintMemorySize(Flash->PageSize, (CONST INT8 *)"Erase Size ");
  PrintMemorySize(Flash->EraseSize, (CONST INT8 *)"Total ");
  PrintMemorySize(Flash->Size, (CONST INT8 *)"");
  DEBUG((EFI_D_RELEASE, "\n"));

  if (Flash->MemoryMap)
    DEBUG((EFI_D_INFO, ", Mapped At %P\n", Flash->MemoryMap));

  if (((Flash->DualFlash == SINGLE_FLASH) &&
       (Flash->Size > SPI_COMMON_FLASH_16MB_BOUN)) ||
       ((Flash->DualFlash > SINGLE_FLASH) &&
       (Flash->Size > SPI_COMMON_FLASH_16MB_BOUN << 1))) {
    DEBUG((EFI_D_WARN, "Warning - Only Lower 16MiB Accessible, "\
  		"Full Access #define QSPI_FLASH_BAR\n"));
  }

  /* Release Qspi Bus */
  QspiReleaseBus(Qspi);
  return Flash;

ErrClaimBus:
  QspiReleaseBus(Qspi);
  QspiFreeSlave(Qspi);
  return NULL;
}
