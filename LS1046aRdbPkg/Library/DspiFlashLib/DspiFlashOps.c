/** @DspiFlashOps.c

  Functions for implementing DSI Controller functionality.

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Library/Dspi.h"

VOID DumpDspiRegs(VOID)
{
  struct Dspi *Regs = (VOID *)CONFIG_SYS_FSL_DSPI_ADDR;

  DEBUG((EFI_D_INFO,"Mcr	:0x%x \n", MmioReadBe32((UINTN)&Regs->Mcr)));
  DEBUG((EFI_D_INFO,"Tcr	:0x%x \n", MmioReadBe32((UINTN)&Regs->Tcr)));
  DEBUG((EFI_D_INFO,"Ctar[0]:0x%x \n", MmioReadBe32((UINTN)&Regs->Ctar[0])));
  DEBUG((EFI_D_INFO,"Ctar[1]:0x%x \n", MmioReadBe32((UINTN)&Regs->Ctar[1])));
  DEBUG((EFI_D_INFO,"Ctar[2]:0x%x \n", MmioReadBe32((UINTN)&Regs->Ctar[2])));
  DEBUG((EFI_D_INFO,"Ctar[3]:0x%x \n", MmioReadBe32((UINTN)&Regs->Ctar[3])));
  DEBUG((EFI_D_INFO,"Sr	:0x%x \n", MmioReadBe32((UINTN)&Regs->Sr)));
  DEBUG((EFI_D_INFO,"Irsr	:0x%x \n", MmioReadBe32((UINTN)&Regs->Irsr)));
  DEBUG((EFI_D_INFO,"Tfr	:0x%x \n", MmioReadBe32((UINTN)&Regs->Tfr)));
  DEBUG((EFI_D_INFO,"Tfdr[0]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Tfdr[0] )));
  DEBUG((EFI_D_INFO,"Tfdr[1]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Tfdr[1] )));
  DEBUG((EFI_D_INFO,"Tfdr[2]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Tfdr[2] )));
  DEBUG((EFI_D_INFO,"Tfdr[3]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Tfdr[3] )));
  DEBUG((EFI_D_INFO,"Tfdr[4]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Tfdr[4] )));
  DEBUG((EFI_D_INFO,"Tfdr[5]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Tfdr[5] )));
  DEBUG((EFI_D_INFO,"Tfdr[6]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Tfdr[6] )));
  DEBUG((EFI_D_INFO,"Tfdr[7]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Tfdr[7] )));
  DEBUG((EFI_D_INFO,"Tfdr[8]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Tfdr[8] )));
  DEBUG((EFI_D_INFO,"Tfdr[9]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Tfdr[9] )));
  DEBUG((EFI_D_INFO,"Tfdr[10]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Tfdr[10])));
  DEBUG((EFI_D_INFO,"Tfdr[11]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Tfdr[11])));
  DEBUG((EFI_D_INFO,"Tfdr[12]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Tfdr[12])));
  DEBUG((EFI_D_INFO,"Tfdr[13]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Tfdr[13])));
  DEBUG((EFI_D_INFO,"Tfdr[14]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Tfdr[14])));
  DEBUG((EFI_D_INFO,"Tfdr[15]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Tfdr[15])));
  DEBUG((EFI_D_INFO,"Rfdr[0] 	:0x%x \n", MmioReadBe32((UINTN)&Regs->Rfdr[0] )));
  DEBUG((EFI_D_INFO,"Rfdr[1] 	:0x%x \n", MmioReadBe32((UINTN)&Regs->Rfdr[1] )));
  DEBUG((EFI_D_INFO,"Rfdr[2] 	:0x%x \n", MmioReadBe32((UINTN)&Regs->Rfdr[2] )));
  DEBUG((EFI_D_INFO,"Rfdr[3] 	:0x%x \n", MmioReadBe32((UINTN)&Regs->Rfdr[3] )));
  DEBUG((EFI_D_INFO,"Rfdr[4] 	:0x%x \n", MmioReadBe32((UINTN)&Regs->Rfdr[4] )));
  DEBUG((EFI_D_INFO,"Rfdr[5] 	:0x%x \n", MmioReadBe32((UINTN)&Regs->Rfdr[5] )));
  DEBUG((EFI_D_INFO,"Rfdr[6] 	:0x%x \n", MmioReadBe32((UINTN)&Regs->Rfdr[6] )));
  DEBUG((EFI_D_INFO,"Rfdr[7] 	:0x%x \n", MmioReadBe32((UINTN)&Regs->Rfdr[7] )));
  DEBUG((EFI_D_INFO,"Rfdr[8] 	:0x%x \n", MmioReadBe32((UINTN)&Regs->Rfdr[8] )));
  DEBUG((EFI_D_INFO,"Rfdr[9] 	:0x%x \n", MmioReadBe32((UINTN)&Regs->Rfdr[9] )));
  DEBUG((EFI_D_INFO,"Rfdr[10]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Rfdr[10])));
  DEBUG((EFI_D_INFO,"Rfdr[11]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Rfdr[11])));
  DEBUG((EFI_D_INFO,"Rfdr[12]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Rfdr[12])));
  DEBUG((EFI_D_INFO,"Rfdr[13]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Rfdr[13])));
  DEBUG((EFI_D_INFO,"Rfdr[14]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Rfdr[14])));
  DEBUG((EFI_D_INFO,"Rfdr[15]	:0x%x \n", MmioReadBe32((UINTN)&Regs->Rfdr[15])));
}

#ifdef CONFIG_DSPI_FLASH_BAR
EFI_STATUS
DspiCmdBankAddrWrite (
  IN  struct DspiFlash *Flash,
  IN  UINT8 BankSel
  )
{
  UINT8 Cmd;
  INT32 Ret;

  if (Flash->BankCurr == BankSel) {
    return EFI_SUCCESS;
  }

  Cmd = Flash->BankWriteCmd;

  Ret = DspiCommonWrite(Flash, &Cmd, 1, &BankSel, 1);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR,"Fail to write bank register\n"));
    return Ret;
  }

  Flash->BankCurr = BankSel;

  return EFI_SUCCESS;
}  
 
INT32
DspiWriteBankAddr (
  IN struct DspiFlash *Flash,
  IN  UINT32 Offset
  )
{  
  UINT8 BankSel;
  INT32 Ret;

  BankSel = Offset / (SPI_COMMON_FLASH_16MB_BOUN << Flash->Shift);

  Ret = DspiCmdBankAddrWrite(Flash, BankSel);
  if (Ret) {
    DEBUG((EFI_D_ERROR,"Fail to set bank%d\n", BankSel));
    return Ret;
  }

  return BankSel;
}
#endif

struct DspiSlave *
DspiCreateSlave (
  IN  INT32 Offset,
  IN  INT32 Size,
  IN  UINT32 Bus,
  IN  UINT32 Cs
 )
{

  struct DspiSlave *Ptr = NULL;
  Ptr = (struct DspiSlave*)AllocatePool(Size);
  if (!Ptr)
    return Ptr;

  InternalMemZeroMem(Ptr, Size);

  Ptr->Slave.Bus = Bus;
  Ptr->Slave.Cs = Cs;
  Ptr->Slave.Wordlen = SPI_COMMON_DEFAULT_WORDLEN;

  return Ptr;
}

INT32
IsDataflash (
  IN  UINT32 Bus,
  IN  UINT32 Cs
  )
{
  if (Bus == BUS_DSPI1 && Cs == 0)
    return 1;
  else
    return 0;
}

/*
 * PrINT32 Sizes As "Xxx KiB", "Xxx.Y KiB", "Xxx MiB", "Xxx.Y MiB",
 * Xxx GiB, Xxx.Y GiB, Etc As Needed; Allow for Optional Trailing String
 * (Like "\n")
 */
VOID
PrintMemorySize (
  IN  UINT64 Size,
  IN  CONST INT8 *S
  )
{
  UINT64 M = 0, N;
  UINT64 F;
  static CONST INT8 Names[] = {'E', 'P', 'T', 'G', 'M', 'K'};
  UINT64 D = 10 * ARRAY_SIZE(Names);
  CHAR8 C = 0;
  UINT32 I;

  for (I = 0; I < ARRAY_SIZE(Names); I++, D -= 10) {
    if (Size >> D) {
      C = Names[I];
      break;
    }
  }

  if (!C) {
    DEBUG((EFI_D_RELEASE, "%Ld Bytes,\n %a", Size, S));
    return;
  }

  N = Size >> D;
  F = Size & ((1ULL << D) - 1);

  /* if There'S A Remainder, Deal With It */
  if (F) {
    M = (10ULL * F + (1ULL << (D - 1))) >> D;

    if (M >= 10) {
           M -= 10;
           N += 1;
    }
  }

  DEBUG((EFI_D_RELEASE, "%Ld", N));
  if (M) {
    DEBUG((EFI_D_RELEASE, ".%Ld", M));
  }
  DEBUG((EFI_D_RELEASE, " %ciB, %a ", C, S));
}

EFI_STATUS
DspiReadStatus (
  IN  struct DspiFlash *Flash,
  OUT UINT8 *Res
  )
{
  INT32 Ret;
  UINT8 Cmd;
  Cmd = READ_STATUS;

  Ret = DspiCommonRead(Flash, &Cmd, 1, Res, 1);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "Fail To Read DSPI Status Register\n"));
    return Ret;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
DspiWriteConfig (
  IN  struct DspiFlash *Flash,
  IN  UINT8 WCmd
  )
{
  UINT8 Data[2];
  UINT8 Cmd;
  INT32 Ret;

  Ret = DspiReadStatus(Flash, &Data[0]);
  if (Ret != EFI_SUCCESS)
    return Ret;

  Cmd = WRITE_STATUS;
  Data[1] = WCmd;
  Ret = DspiCommonWrite(Flash, &Cmd, 1, &Data, 2);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "Fail To Write DSPI Config Register\n"));
    return Ret;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
DspiReadConfig (
  IN  struct DspiFlash *Flash,
  IN  UINT8 *RCmd
  )
{
  INT32 Ret;
  UINT8 Cmd;

  Cmd = READ_CONFIG;
  Ret = DspiCommonRead(Flash, &Cmd, 1, RCmd, 1);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "Fail To Read DSPI Config Register\n"));
    return Ret;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
DspiSetQebWinspan (
  IN  struct DspiFlash *Flash
  )
{
  UINT8 QebStatus;
  INT32 Ret;

  Ret = DspiReadConfig(Flash, &QebStatus);
  if (Ret != EFI_SUCCESS)
    return Ret;

  if (QebStatus & STATUS_QEB_WINSPAN) {
    DEBUG((EFI_D_ERROR, "Winspan: QEB Is Already Set\n"));
  } else {
    Ret = DspiWriteConfig(Flash, STATUS_QEB_WINSPAN);
    if (Ret != EFI_SUCCESS)
      return Ret;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
DspiSetQeb (
  IN  struct DspiFlash *Flash,
  IN  UINT8 Idcode0
  )
{
  switch (Idcode0) {
  case SPI_COMMON_FLASH_CFI_MFR_SPANSION:
  case SPI_COMMON_FLASH_CFI_MFR_WINBOND:
    return DspiSetQebWinspan(Flash);
  case SPI_COMMON_FLASH_CFI_MFR_STMICRO:
    DEBUG((EFI_D_INFO,"QEB is volatile for %02x flash\n", Idcode0));
    return EFI_SUCCESS;
  default:
    DEBUG((EFI_D_ERROR, "Need Set QEB Func for %02x Flash\n",
			Idcode0));
    return EFI_UNSUPPORTED;
  }
}

EFI_STATUS
DspiTx (
  IN  struct DspiSlave *Dspislave,
  IN  UINT32 Ctrl,
  IN  UINT16 Data
  )
{
  INT32 Timeout = 10000;

  while (((MmioReadBe32((UINTN)&Dspislave->Regs->Sr) & 0x0000F000) >= 4) && Timeout--)
			;

  if (Timeout <= 0) {
    DEBUG((EFI_D_ERROR, "DspiTx: Failed to get status register\n"));
    return EFI_TIMEOUT;
  }

  MmioWriteBe32((UINTN)&Dspislave->Regs->Tfr, (Ctrl | Data));
  return EFI_SUCCESS;
}

INT32
DspiRx (
  IN  struct DspiSlave *Dspislave
  )
{
  INT32 Timeout = 10000;

  while (((MmioReadBe32((UINTN)&Dspislave->Regs->Sr) & 0x000000F0) == 0) && Timeout--)
			;

  if (Timeout <= 0) {
    DEBUG((EFI_D_ERROR, "DspiRx : Timeout in reading status register\n"));
    return -1;
  }

  return ((UINT16)(MmioReadBe32((UINTN)&Dspislave->Regs->Rfr) & 0xFFFF));
}

EFI_STATUS
DspiXfer (
  IN  struct DspiSlave *DspiSlave,
  IN  UINT32 Bitlen,
  IN  CONST VOID *OutData,
  OUT VOID *InData,
  IN  UINT64 Flags
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  INT32 Data = 0;
  UINT16 *DspiRead16 = NULL, *DspiWrite16 = NULL;
  UINT8 *DspiRead = NULL, *DspiWrite = NULL;
  static UINT32 Ctrl = 0;
  UINT32 Len = Bitlen >> 3;

  if ((Flags & SPI_COMMON_XFER_BEGIN) == SPI_COMMON_XFER_BEGIN)
    Ctrl |= DSPI_TFR_CONT;

  Ctrl &= ~DSPI_PUSHR_CTAS_MASK;
  Ctrl &= ~DSPI_PUSHR_PCS_MASK;
  Ctrl = (Ctrl & 0xFF000000) | ((1 << DspiSlave->Slave.Cs) << 16);

  if (DspiSlave->Charbit == 16) {
    Bitlen >>= 1;
    DspiWrite16 = (UINT16 *) OutData;
    DspiRead16 = (UINT16 *) InData;
  } else {
    DspiWrite = (UINT8 *) OutData;
    DspiRead = (UINT8 *) InData;
  }

  if (Len > 1) {
    INT32 TempLen = Len - 1;
    while (TempLen--) {
      if (OutData != NULL) {
        if (DspiSlave->Charbit == 16)
	   Status = DspiTx(DspiSlave, Ctrl, *DspiWrite16++);
	 else
	   Status = DspiTx(DspiSlave, Ctrl, *DspiWrite++);

        if (Status != EFI_SUCCESS)
	   return Status;

	 Data = DspiRx(DspiSlave);
        if (Data < 0)
	   return EFI_TIMEOUT;
      }

      if (InData != NULL) {
	 Status = DspiTx(DspiSlave, Ctrl, CONFIG_SPI_COMMON_IDLE_VAL);
        if (Status != EFI_SUCCESS)
	   return Status;

	 Data = DspiRx(DspiSlave);
        if (Data < 0)
	   return EFI_TIMEOUT;

	 if (DspiSlave->Charbit == 16)
	   *DspiRead16++ = Data;
	 else
	   *DspiRead++ = Data;
      }
    }
    Len = 1;	/* Remaining Byte */
  }

  if ((Flags & SPI_COMMON_XFER_END) == SPI_COMMON_XFER_END)
    Ctrl &= ~DSPI_TFR_CONT;

  if (Len) {
    if (OutData != NULL) {
      if (DspiSlave->Charbit == 16)
        Status = DspiTx(DspiSlave, Ctrl, *DspiWrite16);
      else
        Status = DspiTx(DspiSlave, Ctrl, *DspiWrite);
	
      if (Status != EFI_SUCCESS)
	 return Status;

      Data = DspiRx(DspiSlave);
      if (Data < 0)
        return EFI_TIMEOUT;
    }

    if (InData != NULL) {
      Status = DspiTx(DspiSlave, Ctrl, CONFIG_SPI_COMMON_IDLE_VAL);
       
      if (Status != EFI_SUCCESS)
	 return Status;

      Data = DspiRx(DspiSlave);
      if (Data < 0)
	 return EFI_TIMEOUT;

      if (DspiSlave->Charbit == 16)
        *DspiRead16 = Data;
      else
	*DspiRead = Data;
    }
  } else {
    /* Dummy Read */
    Status = DspiTx(DspiSlave, Ctrl, CONFIG_SPI_COMMON_IDLE_VAL);
    if (Status != EFI_SUCCESS)
      return Status;

    Data = DspiRx(DspiSlave);
    if (Data < 0)
      return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}

struct DspiSlave *
DspiSetupSlave (
  IN  UINT32 Bus,
  IN  UINT32 Cs,
  IN  UINT32 MaxHz,
  IN  UINT32 Mode
  )
{
  /*
   * Bit Definition for Mode:
   * Bit 31 - 28: Transfer Size 3 To 16 Bits
   *     27 - 26: PCS To SCK Delay Prescaler
   *     25 - 24: After SCK Delay Prescaler
   *     23 - 22: Delay After Transfer Prescaler
   *     21     : Allow Overwrite for Bit 31-22 And Bit 20-8
   *     20     : Double Baud Rate
   *     19 - 16: PCS To SCK Delay Scaler
   *     15 - 12: After SCK Delay Scaler
   *     11 -  8: Delay After Transfer Scaler
   *      7 -  0: SPI_COMMON_CPHA, SPI_COMMON_CPOL, SPI_COMMON_LSB_FIRST
   */
  struct DspiSlave *Dspislave;
  INT32 Prescaler[] = { 2, 3, 5, 7 };
  STATIC INT32 Scaler[] = {
    2, 4, 6, 8,
    16, 32, 64, 128,
    256, 512, 1024, 2048,
    4096, 8192, 16384, 32768
  };
  UINT32 I, J, PrescalerCnt, ScalerCnt, Diff, Temp, Dbr = 0;
  UINT32 BestI, BestJ, Bestmatch = 0x7FFFFFFF, BaudSpeed;
  UINTN  BusClk;
  UINT32 BusSetup = 0;
  UINT32 Ret = 0;
  struct SysInfo SocSysInfo;

  Dspislave = DspiCreateSlave(0, sizeof(struct DspiSlave), Bus, Cs);
  if (!Dspislave)
    return NULL;

  Dspislave->Baudrate = MaxHz;
  Dspislave->Regs = (VOID *)CONFIG_SYS_FSL_DSPI_ADDR;

  MmioWriteBe32((UINTN)&Dspislave->Regs->Mcr,
    DSPI_MC_MSTR | DSPI_MC_CSIS7 | DSPI_MC_CSIS6 |
    DSPI_MC_CSIS5 | DSPI_MC_CSIS4 | DSPI_MC_CSIS3 |
    DSPI_MC_CSIS2 | DSPI_MC_CSIS1 | DSPI_MC_CSIS0 |
    DSPI_MC_CRXF | DSPI_MC_CTXF);

  /* default Setting In Platform Configuration */
  MmioWriteBe32((UINTN)&Dspislave->Regs->Ctar[0], CONFIG_SYS_DSPI_CTR0);

  GetSysInfo(&SocSysInfo);
  BusClk = (UINT32)SocSysInfo.FreqSystemBus;

  /* Maximum And Minimum Baudrate It Can Handle */
  Temp = (Prescaler[3] * Scaler[15]);
  if ((Dspislave->Baudrate > (BusClk >> 1)) ||
    (Dspislave->Baudrate < (BusClk / Temp))) {
    DEBUG((EFI_D_INFO, "Exceed Baudrate Limitation: Max %d - Min %d\n",
		(INT32)(BusClk >> 1), (INT32)(BusClk / Temp)));

    return NULL;
  }

  /* Activate Double Baud When It Exceed 1/4 The Bus Clk */
  if ((CONFIG_SYS_DSPI_CTR0 & DSPI_CTR_DBR) ||
    (Dspislave->Baudrate > (BusClk / (Prescaler[0] * Scaler[0])))) {
    BusSetup |= DSPI_CTR_DBR;
    Dbr = 1;
  }

  if (Mode & SPI_COMMON_CPOL)
    BusSetup |= DSPI_CTR_CPOL;
  if (Mode & SPI_COMMON_CPHA)
    BusSetup |= DSPI_CTR_CPHA;
  if (Mode & SPI_COMMON_LSB_FIRST)
    BusSetup |= DSPI_CTR_LSBFE;

  /* Overwrite default Value Set In Platform Configuration File */
  Ret = MmioReadBe32((UINTN)&Dspislave->Regs->Ctar[0]);
  if (Mode & SPI_COMMON_MODE_MOD) {
    if ((Mode & 0xF0000000) == 0) {
      BusSetup |= Ret & 0x78000000;	/* 0x0000 002F */
    } else
      BusSetup |= ((Mode & 0xF0000000) >> 1);

    /*
     * Check To See if It Is Enabled By default In Platform
     * Config, Or Manual Setting Passed By Mode Parameter
     **/
    if (Mode & SPI_COMMON_DBLRATE) {
      BusSetup |= DSPI_CTR_DBR;
      Dbr = 1;
    }

    BusSetup |= (Mode & 0x0FC00000) >> 4; /* PSCSCK, PASC, PDT */
    BusSetup |= (Mode & 0x000FFF00) >> 4; /* CSSCK, ASC, DT */
  } else
    BusSetup |= Ret & 0x78FCFFF0;

  Dspislave->Charbit = ((Ret & CHARBIT_MASK) == CHARBIT_MASK) ? 16 : 8;

  /* Baudrate Calculation - To Closer Value, May Not Be Exact Match */
  PrescalerCnt = sizeof(Prescaler) / sizeof(INT32);
  ScalerCnt = sizeof(Scaler) / sizeof(INT32);

  for (BestI = 0, BestJ = 0, I = 0; I < PrescalerCnt; I++) {
    BaudSpeed = BusClk / Prescaler[I];
    for (J = 0; J < ScalerCnt; J++) {
      Temp = (BaudSpeed / Scaler[J]) * (1 + Dbr);

      if (Temp > Dspislave->Baudrate)
	Diff = Temp - Dspislave->Baudrate;
      else
	Diff = Dspislave->Baudrate - Temp;

      if (Diff < Bestmatch) {
	Bestmatch = Diff;
	BestI = I;
	BestJ = J;
      }
    }
  }

  BusSetup |= (DSPI_CTR_PREBR(BestI) | DSPI_CTR_BR(BestJ));
  MmioWriteBe32((UINTN)&Dspislave->Regs->Ctar[0], BusSetup);

  return Dspislave;
}

VOID
DspiFreeSlave (
  IN  struct DspiSlave *Slave
  )
{
  FreePool(Slave);
}

EFI_STATUS
DspiClaimBus (
  IN  struct DspiSlave *Slave
  )
{
  return EFI_SUCCESS;
}

VOID
DspiReleaseBus (
  IN  struct DspiSlave *Slave
  )
{
  return;
}

EFI_STATUS
DspiReadWrite (
  IN  struct DspiSlave *Dspi,
  IN  CONST UINT8 *Cmd,
  IN  UINT32 CmdLen,
  IN  CONST UINT8 *DataOut,
  OUT UINT8 *DataIn,
  IN  UINT64 DataLen
  )
{
  UINT64 Flags = SPI_COMMON_XFER_BEGIN;
  INT32 Ret;

  if (DataLen == 0)
    Flags |= SPI_COMMON_XFER_END;

  Ret = DspiXfer(Dspi, CmdLen * 8, Cmd, NULL, Flags);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "DSPI: Failed To Send Command (%zu Bytes): %d\n",
			CmdLen, Ret));
    return Ret;
  } else if (DataLen != 0) {
    Ret = DspiXfer(Dspi, DataLen * 8, DataOut, DataIn, SPI_COMMON_XFER_END);
    if (Ret != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "DSPI: Failed To Transfer %zu Bytes Of Data: %d\n",
			DataLen, Ret));
      return Ret;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
DspiWaitReady (
  IN  struct DspiFlash *Flash,
  IN  UINT64 Timeout
  )
{
  struct DspiSlave *Dspi = Flash->Dspi;
  UINT64 Timebase;
  UINT64 Flags = SPI_COMMON_XFER_BEGIN;
  EFI_STATUS Ret = EFI_SUCCESS;
  UINT8 Status = 0;
  UINT8 CheckStatus = 0x0;
  UINT8 PollBit = STATUS_WIP;
  UINT8 Cmd = Flash->PollCmd;

  if (Cmd == FLAG_STATUS || Cmd == DSPI_CMD_ATMEL_FLAG_STATUS) {
    PollBit = STATUS_PEC;
    CheckStatus = PollBit;
  }

  Ret = DspiXfer(Dspi, 8, &Cmd, NULL, Flags);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "DSPI: Fail To Read %a Status Register\n",
          Cmd == READ_STATUS ? "Read" : "Flag"));
    return Ret;
  }

  Timebase = 0;
  do {
    MicroSecondDelay(10000);

    Ret = DspiXfer(Dspi, 8, NULL, &Status, 0);
    if (Ret != EFI_SUCCESS) {
      DspiXfer(Dspi, 0, NULL, NULL, SPI_COMMON_XFER_END);
      return Ret;
    }

    if ((Status & PollBit) == CheckStatus) {
      break;
    }

  } while (++Timebase < Timeout);

  DspiXfer(Dspi, 0, NULL, NULL, SPI_COMMON_XFER_END);
  if ((Status & PollBit) == CheckStatus)
    return EFI_SUCCESS;

  /* Timed Out */
  DEBUG((EFI_D_ERROR, "DSPI: Time Out!\n"));
  Ret = EFI_TIMEOUT;

  return Ret;
}

EFI_STATUS
DspiCommonWrite (
  IN  struct DspiFlash *Flash,
  IN  CONST UINT8 *Cmd,
  IN  UINT32 CmdLen,
  IN  CONST VOID *Buf,
  IN  UINT32 BufLen
  )
{
  struct DspiSlave *Dspi = Flash->Dspi;
  UINT64 Timeout = SPI_COMMON_FLASH_PROG_TIMEOUT;
  INT32 Ret;
  UINT8 Wcmd = 0;

  if (Buf == NULL)
    Timeout = SPI_COMMON_FLASH_PAGE_ERASE_TIMEOUT;

  Ret = DspiClaimBus(Flash->Dspi);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "Unable To Claim DSPI Bus\n"));
    return Ret;
  }

  Wcmd = WRITE_ENABLE;

  Ret = DspiReadWrite(Dspi, &Wcmd, 1, NULL, NULL, 0);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "Enabling DSPI Write Failed\n"));
    return Ret;
  }

  Ret = DspiReadWrite(Dspi, Cmd, CmdLen, Buf, NULL, BufLen);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "DSPI Write Cmd Failed\n"));
    return Ret;
  }

  Ret = DspiWaitReady(Flash, Timeout);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "DSPI Write %s Failed\n",
          Timeout == SPI_COMMON_FLASH_PROG_TIMEOUT ?
           "Program" : "Page Erase"));
    return Ret;
  }

  DspiReleaseBus(Dspi);
  return Ret;
}

VOID
DspiCreateAddr (
  IN  UINT32 Addr,
  OUT UINT8 *Cmd
  )
{
  /* Cmd[0] Is Actual Command */
  Cmd[1] = Addr >> 16;
  Cmd[2] = Addr >> 8;
  Cmd[3] = Addr >> 0;
}

EFI_STATUS
DspiCommonRead (
  IN  struct DspiFlash *Flash,
  IN  CONST UINT8 *Cmd,
  IN  UINT32 CmdLen,
  OUT VOID *Data,
  IN  UINT64 DataLen
  )
{
  struct DspiSlave *Dspi = Flash->Dspi;
  INT32 Ret;

  Ret = DspiClaimBus(Flash->Dspi);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "Unable To Claim DSPI Bus\n"));
    return Ret;
  }

  Ret = DspiReadWrite(Dspi, Cmd, CmdLen, NULL, Data, DataLen);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "DSPI Read Cmd Failed\n"));
    return Ret;
  }

  DspiReleaseBus(Dspi);

  return Ret;
}

EFI_STATUS
DspiReadOps (
  IN  struct DspiFlash *Flash,
  IN  UINT32 Offset,
  IN  UINT64 Len,
  OUT VOID *Data
  )
{
  UINT8 CmdSz = 0;
  UINT8 *Cmd = NULL;
  UINT32 RemainLen, ReadLen, ReadAddr;
  INT32 BankSel = 0;
  INT32 Ret = -1;

  /* Handle Memory-Mapped SPI */
  if (Flash->MemoryMap) {
    Ret = DspiClaimBus(Flash->Dspi);
    if (Ret != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "Unable To Claim DSPI Bus\n"));
      return Ret;
    }
    DspiXfer(Flash->Dspi, 0, NULL, NULL, SPI_COMMON_XFER_MMAP);
    InternalMemCopyMem(Data, Flash->MemoryMap + Offset, Len);
    DspiXfer(Flash->Dspi, 0, NULL, NULL, SPI_COMMON_XFER_MMAP_END);
    DspiReleaseBus(Flash->Dspi);
    return EFI_SUCCESS;
  }

  CmdSz = SPI_COMMON_FLASH_CMD_LEN + Flash->DummyByte;

  Cmd = (UINT8 *)AllocatePool(CmdSz);
  if (!Cmd) {
    DEBUG((EFI_D_ERROR, "DSPI: Out of Memory\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  Cmd[0] = Flash->ReadCmd;
  while (Len) {
    ReadAddr = Offset;

#ifdef CONFIG_DSPI_FLASH_BAR
  BankSel = DspiWriteBankAddr(Flash, ReadAddr);
  if (BankSel < 0)
    return Ret;
#endif
    RemainLen = ((SPI_COMMON_FLASH_16MB_BOUN << Flash->Shift) *
                 (BankSel + 1)) - Offset;
    if (Len < RemainLen)
      ReadLen = Len;
    else
      ReadLen = RemainLen;

    DspiCreateAddr(ReadAddr, Cmd);
    Ret = DspiCommonRead(Flash, Cmd, CmdSz, Data, ReadLen);
    if (Ret != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "DSPI Read Failed\n"));
      break;
    }

    Offset += ReadLen;
    Len -= ReadLen;
    Data += ReadLen;
  }

  FreePool(Cmd);
  return Ret;
}

EFI_STATUS
DspiEraseOps (
  IN  struct DspiFlash *Flash,
  IN  UINT32 Offset,
  IN  UINT64 Len
  )
{
  UINT32 EraseSize, EraseAddr;
  UINT8 Cmd[SPI_COMMON_FLASH_CMD_LEN];
  INT32 Ret = -1;

  EraseSize = Flash->EraseSize;

  if (Offset % EraseSize || Len % EraseSize) {
    DEBUG((EFI_D_ERROR, "DSPI Erase Offset/Length Not"\
          "Multiple Of Erase Size\n"));
    return EFI_INVALID_PARAMETER;
  }

  Cmd[0] = Flash->EraseCmd;
  while (Len) {
    EraseAddr = Offset;

#ifdef CONFIG_DSPI_FLASH_BAR
  Ret = DspiWriteBankAddr(Flash, EraseAddr);
  if (Ret < 0)
    return Ret;
#endif
    DspiCreateAddr(EraseAddr, Cmd);

    Ret = DspiCommonWrite(Flash, Cmd, sizeof(Cmd), NULL, 0);
    if (Ret != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "DSPI Erase Failed\n"));
      break;
    }

    Offset += EraseSize;
    Len -= EraseSize;
  }

  return Ret;
}

EFI_STATUS
DspiWriteOps (
  IN  struct DspiFlash *Flash,
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

  PageSize = Flash->PageSize;

  Cmd[0] = Flash->WriteCmd;
  for (Actual = 0; Actual < Len; Actual += ChunkLen) {
    WriteAddr = Offset;

#ifdef CONFIG_DSPI_FLASH_BAR
    Ret = DspiWriteBankAddr(Flash, WriteAddr);
    if (Ret < 0)
      return Ret;
#endif
    ByteAddr = Offset % PageSize;
    ChunkLen = Min(Len - Actual, (PageSize - ByteAddr));

    if (Flash->Dspi->Slave.MaxWriteSize)
      ChunkLen = Min(ChunkLen,
		Flash->Dspi->Slave.MaxWriteSize);

    DspiCreateAddr(WriteAddr, Cmd);

    Ret = DspiCommonWrite(Flash, Cmd, sizeof(Cmd),
                         Buf + Actual, ChunkLen);
    if (Ret != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "DSPI Write Failed\n"));
      break;
    }

    Offset += ChunkLen;
  }

  return Ret;
}
