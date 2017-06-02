/** @QspiFlashOps.c

  Functions for implementing qspi controller functionality.

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

#include <Library/Qspi.h>

VOID DumpQspiRegs(struct QspiRegs *QspiRegs)
{
  UINT32 Index = 0;

  DEBUG((EFI_D_INFO,"Mcr		:0x%x \n", MmioRead32((UINTN)&QspiRegs->Mcr)));
  DEBUG((EFI_D_INFO,"Ipcr    	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Ipcr)));
  DEBUG((EFI_D_INFO,"Flshcr  	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Flshcr)));
  DEBUG((EFI_D_INFO,"Buf0cr  	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Buf0cr)));
  DEBUG((EFI_D_INFO,"Buf1cr  	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Buf1cr)));
  DEBUG((EFI_D_INFO,"Buf2cr  	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Buf2cr)));
  DEBUG((EFI_D_INFO,"Buf3cr  	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Buf3cr)));
  DEBUG((EFI_D_INFO,"Bfgencr 	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Bfgencr)));
  DEBUG((EFI_D_INFO,"Soccr   	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Soccr)));
  DEBUG((EFI_D_INFO,"Buf0ind 	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Buf0ind)));
  DEBUG((EFI_D_INFO,"Buf1ind 	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Buf1ind)));
  DEBUG((EFI_D_INFO,"Buf2ind 	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Buf2ind)));
  DEBUG((EFI_D_INFO,"Sfar    	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Sfar)));
  DEBUG((EFI_D_INFO,"Smpr    	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Smpr)));
  DEBUG((EFI_D_INFO,"Rbsr    	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbsr)));
  DEBUG((EFI_D_INFO,"Rbct    	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbct)));
  DEBUG((EFI_D_INFO,"Tbsr    	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Tbsr)));
  DEBUG((EFI_D_INFO,"Tbdr    	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Tbdr)));
  DEBUG((EFI_D_INFO,"Sr      	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Sr)));
  DEBUG((EFI_D_INFO,"Fr      	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Fr)));
  DEBUG((EFI_D_INFO,"Rser    	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rser)));
  DEBUG((EFI_D_INFO,"Spndst  	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Spndst)));
  DEBUG((EFI_D_INFO,"Sptrclr 	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Sptrclr)));
  DEBUG((EFI_D_INFO,"Sfa1ad  	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Sfa1ad)));
  DEBUG((EFI_D_INFO,"Sfa2ad  	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Sfa2ad)));
  DEBUG((EFI_D_INFO,"Sfb1ad  	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Sfb1ad)));
  DEBUG((EFI_D_INFO,"Sfb2ad  	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Sfb2ad)));
  DEBUG((EFI_D_INFO,"Rbdr[0]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[0])));
  DEBUG((EFI_D_INFO,"Rbdr[1]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[1])));
  DEBUG((EFI_D_INFO,"Rbdr[2]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[2])));
  DEBUG((EFI_D_INFO,"Rbdr[3]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[3])));
  DEBUG((EFI_D_INFO,"Rbdr[4]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[4])));
  DEBUG((EFI_D_INFO,"Rbdr[5]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[5])));
  DEBUG((EFI_D_INFO,"Rbdr[6]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[6])));
  DEBUG((EFI_D_INFO,"Rbdr[7]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[7])));
  DEBUG((EFI_D_INFO,"Rbdr[8]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[8])));
  DEBUG((EFI_D_INFO,"Rbdr[9]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[9])));
  DEBUG((EFI_D_INFO,"Rbdr[10]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[10])));
  DEBUG((EFI_D_INFO,"Rbdr[11]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[11])));
  DEBUG((EFI_D_INFO,"Rbdr[12]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[12])));
  DEBUG((EFI_D_INFO,"Rbdr[13]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[13])));
  DEBUG((EFI_D_INFO,"Rbdr[14]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[14])));
  DEBUG((EFI_D_INFO,"Rbdr[15]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[15])));
  DEBUG((EFI_D_INFO,"Rbdr[16]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[16])));
  DEBUG((EFI_D_INFO,"Rbdr[17]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[17])));
  DEBUG((EFI_D_INFO,"Rbdr[18]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[18])));
  DEBUG((EFI_D_INFO,"Rbdr[19]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[19])));
  DEBUG((EFI_D_INFO,"Rbdr[20]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[20])));
  DEBUG((EFI_D_INFO,"Rbdr[21]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[21])));
  DEBUG((EFI_D_INFO,"Rbdr[22]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[22])));
  DEBUG((EFI_D_INFO,"Rbdr[23]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[23])));
  DEBUG((EFI_D_INFO,"Rbdr[24]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[24])));
  DEBUG((EFI_D_INFO,"Rbdr[25]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[25])));
  DEBUG((EFI_D_INFO,"Rbdr[26]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[26])));
  DEBUG((EFI_D_INFO,"Rbdr[27]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[27])));
  DEBUG((EFI_D_INFO,"Rbdr[28]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[28])));
  DEBUG((EFI_D_INFO,"Rbdr[29]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[29])));
  DEBUG((EFI_D_INFO,"Rbdr[30]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[30])));
  DEBUG((EFI_D_INFO,"Rbdr[31]	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Rbdr[31])));
  DEBUG((EFI_D_INFO,"Lutkey  	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Lutkey)));
  DEBUG((EFI_D_INFO,"Lckcr   	:0x%x \n", MmioRead32((UINTN)&QspiRegs->Lckcr)));
  for (Index = 0; Index<64; Index++)
    DEBUG((EFI_D_INFO,"Lut[%d] 	:0x%x \n",
	Index, MmioRead32((UINTN)&QspiRegs->Lut[Index])));
}

VOID
DisableQspiModule (
  IN  UINT32  *Mcr,
  IN  UINT32  Halt
  )
{
  UINT32 Val;

  Val = MmioRead32((UINTN)Mcr);

  if (Halt)
    Val |= MCR_MDIS_MASK;
  else
    Val &= ~MCR_MDIS_MASK;

  MmioWrite32((UINTN)Mcr, Val);
}

struct QspiSlave *
QspiCreateSlave (
  IN  INT32 Offset,
  IN  INT32 Size,
  IN  UINT32 Bus,
  IN  UINT32 Cs,
  IN  UINT32 Speed,
  IN  UINT32 Mode
 )
{
  struct QspiSlave *Ptr = NULL;

  Ptr = (struct QspiSlave*)AllocatePool(Size);
  if (!Ptr)
    return Ptr;

  InternalMemZeroMem((UINT8 *)Ptr, Size);

  Ptr->Slave.Bus = Bus;
  Ptr->Slave.Cs = Cs;
  Ptr->Slave.Speed = Speed;
  Ptr->Slave.Mode = Mode;
  Ptr->Slave.Wordlen = SPI_COMMON_DEFAULT_WORDLEN;
  Ptr->Slave.MaxWriteSize = TX_BUFFER_SIZE;
  Ptr->CurAmbaBase = AMBA_BASE;
  Ptr->AmbaTotalSize = AMBA_TOTAL_SIZE;
  Ptr->Regs = (VOID *)QSPI_BASE_ADDR;
  Ptr->SpeedHz = SPEED_HZ;
  Ptr->AmbaBase[0] = AMBA_BASE;
  Ptr->FlashNum = FLASH_NUM;
  Ptr->NumChipselect = MAX_CHIPSELECT_NUM;

  return Ptr;
}

/*
 * PrINT32 Sizes As "Xxx KiB", "Xxx.Y KiB", "Xxx MiB", "Xxx.Y MiB",
 * Xxx GiB, Xxx.Y GiB, Etc As Needed; Allow for Optional Trailing String
 * (Like "\n")
 */
VOID
PrintMemorySize (
  IN  UINT64 Size,
  IN  CONST INT8 *s
  )
{
  UINT64 M = 0, n;
  UINT64 F;
  static CONST INT8 Names[] = {'E', 'P', 'T', 'G', 'M', 'K'};
  UINT64 d = 10 * ARRAY_SIZE(Names);
  CHAR8 C = 0;
  UINT32 I;

  for (I = 0; I < ARRAY_SIZE(Names); I++, d -= 10) {
    if (Size >> d) {
      C = Names[I];
      break;
    }
  }

  if (!C) {
    DEBUG((EFI_D_RELEASE, "%ld Bytes,\n %a", Size, s));
    return;
  }

  n = Size >> d;
  F = Size & ((1ULL << d) - 1);

  /* if There's a Remainder, Deal With It */
  if (F) {
    M = (10ULL * F + (1ULL << (d - 1))) >> d;

    if (M >= 10) {
           M -= 10;
           n += 1;
    }
  }

  DEBUG((EFI_D_RELEASE, "%ld", n));
  if (M) {
    DEBUG((EFI_D_RELEASE, ".%ld", M));
  }
  DEBUG((EFI_D_RELEASE, " %ciB, %a ", C, s));
}

EFI_STATUS
QspiReadStatus (
  IN  struct QspiFlash *Flash,
  OUT UINT8 *Res
  )
{
  INT32 Ret;
  UINT8 Cmd;
  Cmd = READ_STATUS;

  Ret = QspiCommonRead(Flash, &Cmd, 1, Res, 1);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "Fail To Read QSPI Status Register\n"));
    return Ret;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
QspiWriteConfig (
  IN  struct QspiFlash *Flash,
  IN  UINT8 WCmd
  )
{
  UINT8 Data[2];
  UINT8 Cmd;
  INT32 Ret;

  Ret = QspiReadStatus(Flash, &Data[0]);
  if (Ret != EFI_SUCCESS)
    return Ret;

  Cmd = WRITE_STATUS;
  Data[1] = WCmd;
  Ret = QspiCommonWrite(Flash, &Cmd, 1, &Data, 2);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "Fail To Write QSPI Config Register\n"));
    return Ret;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
QspiReadConfig (
  IN  struct QspiFlash *Flash,
  IN  UINT8 *RCmd
  )
{
  INT32 Ret;
  UINT8 Cmd;

  Cmd = READ_CONFIG;
  Ret = QspiCommonRead(Flash, &Cmd, 1, RCmd, 1);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "Fail To Read QSPI Config Register\n"));
    return Ret;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
QspiSetQebWinspan (
  IN  struct QspiFlash *Flash
  )
{
  UINT8 QebStatus;
  INT32 Ret;

  Ret = QspiReadConfig(Flash, &QebStatus);
  if (Ret != EFI_SUCCESS)
    return Ret;

  if (QebStatus & STATUS_QEB_WINSPAN) {
    DEBUG((EFI_D_ERROR, "Winspan: QEB Is Already Set\n"));
  } else {
    Ret = QspiWriteConfig(Flash, STATUS_QEB_WINSPAN);
    if (Ret != EFI_SUCCESS)
      return Ret;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
QspiSetQeb (
  IN  struct QspiFlash *Flash,
  IN  UINT8 Idcode0
  )
{
  switch (Idcode0) {
  case SPI_COMMON_FLASH_CFI_MFR_SPANSION:
  case SPI_COMMON_FLASH_CFI_MFR_WINBOND:
    return QspiSetQebWinspan(Flash);
  case SPI_COMMON_FLASH_CFI_MFR_STMICRO:
    DEBUG((EFI_D_INFO,"QEB Is volatile for %02x Flash\n", Idcode0));
    return EFI_SUCCESS;
  default:
    DEBUG((EFI_D_ERROR, "Need Set QEB Func for %02x Flash\n",
			Idcode0));
    return EFI_UNSUPPORTED;
  }
}

UINT32
UpdateMcr (
  IN  UINT32  *Mcr
  )
{
  UINT32 McrReg = 0;

  McrReg = MmioRead32((UINTN)Mcr);
  MmioWrite32((UINTN)Mcr,
              MCR_CLR_RXF_MASK | MCR_CLR_TXF_MASK |
              MCR_RESERVED_MASK | MCR_END_CFD_LE);

  return McrReg;
}

VOID
WriteOperation (
  IN  struct QspiSlave *QSlave,
  IN  UINT8 *Txbuf,
  IN  UINT32 Len
  )
{
   struct QspiRegs *Regs = QSlave->Regs;
   UINT32 McrReg, Data, Reg, StatusReg, Seqid;
   INT32 I, Size, TxSize;
   UINT32 ToOrFrom = 0;

   McrReg = UpdateMcr(&Regs->Mcr);
   MmioWrite32((UINTN)&Regs->Rbct, RBCT_RXBRD_USEIPS);

   StatusReg = 0;
   while ((StatusReg & STATUS_WEL) != STATUS_WEL) {
     MmioWrite32((UINTN)&Regs->Ipcr,
                 (SEQ_ID_WREN << IPCR_SEQID_SHIFT) | 0);
     while (MmioRead32((UINTN)&Regs->Sr) & SR_BUSY_MASK)
            ;

     MmioWrite32((UINTN)&Regs->Ipcr,
                 (SEQ_ID_RDSR << IPCR_SEQID_SHIFT) | 1);
     while (MmioRead32((UINTN)&Regs->Sr) & SR_BUSY_MASK)
            ;

     Reg = MmioRead32((UINTN)&Regs->Rbsr);
     if (Reg & RBSR_RDBFL_MASK) {
       StatusReg = MmioRead32((UINTN)&Regs->Rbdr[0]);
     }
     MmioWrite32((UINTN)&Regs->Mcr,
                 MmioRead32((UINTN)&Regs->Mcr) |
                 MCR_CLR_RXF_MASK);
   }

   /* default Is Page Programming */
   Seqid = SEQ_ID_PP;
   if (QSlave->CurSeqid == CMD_WRAR)
     Seqid = SEQ_ID_WRAR;

   ToOrFrom = QSlave->SfAddr + QSlave->CurAmbaBase;

   MmioWrite32((UINTN)&Regs->Sfar, ToOrFrom);

   TxSize = (Len > TX_BUFFER_SIZE) ?
          TX_BUFFER_SIZE : Len;

   Size = TxSize / 4;
   for (I = 0; I < Size; I++) {
     InternalMemCopyMem(&Data, Txbuf, 4);
     MmioWrite32((UINTN)&Regs->Tbdr, Data);
     Txbuf += 4;
   }

   Size = TxSize % 4;
   if (Size) {
     Data = 0;
     InternalMemCopyMem(&Data, Txbuf, Size);
     MmioWrite32((UINTN)&Regs->Tbdr, Data);
   }

   MmioWrite32((UINTN)&Regs->Ipcr,
               (Seqid << IPCR_SEQID_SHIFT) | TxSize);
   while (MmioRead32((UINTN)&Regs->Sr) & SR_BUSY_MASK)
          ;

   MmioWrite32((UINTN)&Regs->Mcr, McrReg);
}

VOID
EraseOperation (
  IN  struct QspiSlave *QSlave
  )
{
  struct QspiRegs *Regs = QSlave->Regs;
  UINT32 McrReg;
  UINT32 ToOrFrom = 0;

  McrReg = UpdateMcr(&Regs->Mcr);
  MmioWrite32((UINTN)&Regs->Rbct, RBCT_RXBRD_USEIPS);

  ToOrFrom = QSlave->SfAddr + QSlave->CurAmbaBase;
  MmioWrite32((UINTN)&Regs->Sfar, ToOrFrom);

  MmioWrite32((UINTN)&Regs->Ipcr,
              (SEQ_ID_WREN << IPCR_SEQID_SHIFT) | 0);
  while (MmioRead32((UINTN)&Regs->Sr) & SR_BUSY_MASK)
         ;

  if (QSlave->CurSeqid == CMD_SE) {
    MmioWrite32((UINTN)&Regs->Ipcr,
               (SEQ_ID_SE << IPCR_SEQID_SHIFT) | 0);
  } else if (QSlave->CurSeqid == CMD_BE_4K) {
    MmioWrite32((UINTN)&Regs->Ipcr,
               (SEQ_ID_BE_4K << IPCR_SEQID_SHIFT) | 0);
  }
  while (MmioRead32((UINTN)&Regs->Sr) & SR_BUSY_MASK)
         ;

  MmioWrite32((UINTN)&Regs->Mcr, McrReg);
}

VOID
ReadOperation (
  IN   struct QspiSlave *QSlave,
  OUT  UINT32 *Rxbuf,
  IN   UINT32 Len
  )
{
  struct QspiRegs *Regs = QSlave->Regs;
  UINT32 McrReg, Data = 0;
  INT32 I, Size;
  UINT32 From;
  UINT32 Seqid;

  if (QSlave->CurSeqid == CMD_RDAR)
    Seqid = SEQ_ID_RDAR;
  else
    Seqid = SEQ_ID_FAST_READ;

  McrReg = UpdateMcr(&Regs->Mcr);
  MmioWrite32((UINTN)&Regs->Rbct, RBCT_RXBRD_USEIPS);

  From = QSlave->SfAddr + QSlave->CurAmbaBase;

  while (Len > 0) {
    MmioWrite32((UINTN)&Regs->Sfar, From);

    Size = (Len > RX_BUFFER_SIZE) ? RX_BUFFER_SIZE : Len;

    MmioWrite32((UINTN)&Regs->Ipcr,
                (Seqid << IPCR_SEQID_SHIFT) | Size);
    while (MmioRead32((UINTN)&Regs->Sr) & SR_BUSY_MASK)
           ;

    From += Size;
    Len -= Size;

    I = 0;
    while ((RX_BUFFER_SIZE >= Size) && (Size > 0)) {
      Data = MmioRead32((UINTN)&Regs->Rbdr[I]);
      if (Size < 4)
        InternalMemCopyMem(Rxbuf, &Data, Size);
      else
        InternalMemCopyMem(Rxbuf, &Data, 4);
      Rxbuf++;
      Size -= 4;
      I++;
    }
    MmioWrite32((UINTN)&Regs->Mcr,
                MmioRead32((UINTN)&Regs->Mcr) |
                MCR_CLR_RXF_MASK);
  }

  MmioWrite32((UINTN)&Regs->Mcr, McrReg);
}

VOID
ReadIdOperation (
  IN   struct QspiSlave *QSlave,
  OUT  UINT32 *Rxbuf,
  IN   UINT32 Len
  )
{
  struct QspiRegs *Regs = QSlave->Regs;
  UINT32 McrReg = 0, RbsrReg = 0, Data = 0;
  INT32 I = 0, Size = 0;

  McrReg = UpdateMcr(&Regs->Mcr);
  MmioWrite32((UINTN)&Regs->Rbct, RBCT_RXBRD_USEIPS);

  MmioWrite32((UINTN)&Regs->Sfar, QSlave->CurAmbaBase);

  MmioWrite32((UINTN)&Regs->Ipcr,
              (SEQ_ID_RDID << IPCR_SEQID_SHIFT) | 0);
  while (MmioRead32((UINTN)&Regs->Sr) & SR_BUSY_MASK)
         ;

  Size = Len;
  while ((RX_BUFFER_SIZE >= Size) && (Size > 0)) {
    RbsrReg = MmioRead32((UINTN)&Regs->Rbsr);
    if (RbsrReg & RBSR_RDBFL_MASK) {
      Data = MmioRead32((UINTN)&Regs->Rbdr[I]);
      InternalMemCopyMem(Rxbuf, &Data, BYTES_IN_RX_BUFFER);
      Rxbuf++;
      Size -= 4;
      I++;
    }
  }

  MmioWrite32((UINTN)&Regs->Mcr, McrReg);
}

VOID
ReadSrOperation (
  IN   struct QspiSlave *QSlave,
  OUT UINT32 *Rxbuf
  )
{
  struct QspiRegs *Regs = QSlave->Regs;
  UINT32 McrReg, Reg, Data;

  McrReg = MmioRead32((UINTN)&Regs->Mcr);
  McrReg = UpdateMcr(&Regs->Mcr);
  MmioWrite32((UINTN)&Regs->Rbct, RBCT_RXBRD_USEIPS);

  MmioWrite32((UINTN)&Regs->Sfar, QSlave->CurAmbaBase);

  MmioWrite32((UINTN)&Regs->Ipcr,
              (SEQ_ID_RDSR << IPCR_SEQID_SHIFT) | 0);
  while (MmioRead32((UINTN)&Regs->Sr) & SR_BUSY_MASK)
         ;

  while (1) {
    Reg = MmioRead32((UINTN)&Regs->Rbsr);
    if (Reg & RBSR_RDBFL_MASK) {
      Data = MmioRead32((UINTN)&Regs->Rbdr[0]);
      InternalMemCopyMem(Rxbuf, &Data, 4);
      MmioWrite32((UINTN)&Regs->Mcr,
                  MmioRead32((UINTN)&Regs->Mcr) |
                  MCR_CLR_RXF_MASK);
      break;
    }
  }

  MmioWrite32((UINTN)&Regs->Mcr, McrReg);
}

EFI_STATUS
QspiXfer (
  IN  struct QspiSlave *QspiSlave,
  IN  UINT32 Bitlen,
  IN  CONST VOID *OutData,
  OUT VOID *InData,
  IN  UINT64 Flags
  )
{
  UINT32 Bytes = DIV_ROUND_UP(Bitlen, 8);
  static UINT32 WrSfaddr;
  UINT32 Txbuf;

  if (OutData) {
    if (Flags & SPI_COMMON_XFER_BEGIN) {
      QspiSlave->CurSeqid = *(UINT8 *)OutData;
      InternalMemCopyMem(&Txbuf, OutData, 4);
    }

    if (Flags == SPI_COMMON_XFER_END) {
      QspiSlave->SfAddr = WrSfaddr;
      WriteOperation(QspiSlave, (UINT8 *)OutData, Bytes);
      return EFI_SUCCESS;
    }

    switch (QspiSlave->CurSeqid) {
      case CMD_FAST_READ:
      case CMD_RDAR:
        QspiSlave->SfAddr = Uswap32(Txbuf) & OFFSET_MASK;
        break;
      case CMD_SE:
      case CMD_BE_4K:
        QspiSlave->SfAddr = Uswap32(Txbuf) & OFFSET_MASK;
        EraseOperation(QspiSlave);
        break;
      case CMD_PP:
      case CMD_WRAR:
        WrSfaddr = Uswap32(Txbuf) & OFFSET_MASK;
        break;
    }
  }

  if (InData) {
    switch (QspiSlave->CurSeqid) {
      case CMD_FAST_READ:
        ReadOperation(QspiSlave, InData, Bytes);
	 break;
      case CMD_RDAR:
	 ReadOperation(QspiSlave, InData, Bytes);
        break;
      case CMD_RDID:
	 ReadIdOperation(QspiSlave, InData, Bytes);
        break;
      case CMD_RDSR:
	 ReadSrOperation(QspiSlave, InData);
	 break;
    }
  }

  return EFI_SUCCESS;
}

VOID
QspiInitSampling (
  IN  UINT32  *Smpr,
  IN  UINT32  Clear,
  IN  UINT32  Set
  )
{
  UINT32 Val;

  Val = MmioRead32((UINTN)Smpr);
  Val &= ~Clear;
  Val |= Set;
  MmioWrite32((UINTN)Smpr, Val);
}

VOID
QspiPrepareLut (
  IN  struct QspiSlave *QspiSlave
  )
{
  struct QspiRegs *Regs = QspiSlave->Regs;
  UINT32 LutBase;

  /* Unlock The LUT */
  MmioWrite32((UINTN)&Regs->Lutkey, LUT_KEY);
  MmioWrite32((UINTN)&Regs->Lckcr, LCKCR_UNLOCK);

  /* Write Enable */
  LutBase = SEQ_ID_WREN * 4;
  MmioWrite32((UINTN)&Regs->Lut[LutBase], OPRND_0(CMD_WREN) |
         PAD_0(LUT_PAD_1) | INSTR_0(LUT_CMD));
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 1], 0);
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 2], 0);
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 3], 0);

  /* Fast Read */
  LutBase = SEQ_ID_FAST_READ * 4;
  if (QSPI_FLASH_SIZE  <= SIZE_16M)
    MmioWrite32((UINTN)&Regs->Lut[LutBase],
                     OPRND_0(CMD_FAST_READ) | PAD_0(LUT_PAD_1) |
                     INSTR_0(LUT_CMD) | OPRND_1(ADDR_24BIT) |
                     PAD_1(LUT_PAD_1) | INSTR_1(LUT_ADDR));
  else
    MmioWrite32((UINTN)&Regs->Lut[LutBase],
                     OPRND_0(CMD_FAST_READ_4B) |
                     PAD_0(LUT_PAD_1) | INSTR_0(LUT_CMD) |
                     OPRND_1(ADDR_32BIT) | PAD_1(LUT_PAD_1) |
                     INSTR_1(LUT_ADDR));
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 1],
              OPRND_0(8) | PAD_0(LUT_PAD_1) | INSTR_0(LUT_DUMMY) |
              OPRND_1(RX_BUFFER_SIZE) | PAD_1(LUT_PAD_1) |
              INSTR_1(LUT_READ));
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 2], 0);
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 3], 0);

  /* Read Status */
  LutBase = SEQ_ID_RDSR * 4;
  MmioWrite32((UINTN)&Regs->Lut[LutBase], OPRND_0(CMD_RDSR) |
         PAD_0(LUT_PAD_1) | INSTR_0(LUT_CMD) | OPRND_1(1) |
         PAD_1(LUT_PAD_1) | INSTR_1(LUT_READ));
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 1], 0);
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 2], 0);
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 3], 0);

  /* Erase a Sector */
  LutBase = SEQ_ID_SE * 4;
  if (QSPI_FLASH_SIZE  <= SIZE_16M)
    MmioWrite32((UINTN)&Regs->Lut[LutBase],
                     OPRND_0(CMD_SE) | PAD_0(LUT_PAD_1) |
                     INSTR_0(LUT_CMD) | OPRND_1(ADDR_24BIT) |
                     PAD_1(LUT_PAD_1) | INSTR_1(LUT_ADDR));
  else
    MmioWrite32((UINTN)&Regs->Lut[LutBase],
                     OPRND_0(CMD_SE_4B) | PAD_0(LUT_PAD_1) |
                     INSTR_0(LUT_CMD) | OPRND_1(ADDR_32BIT) |
                     PAD_1(LUT_PAD_1) | INSTR_1(LUT_ADDR));
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 1], 0);
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 2], 0);
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 3], 0);

  /* Erase The Whole Chip */
  LutBase = SEQ_ID_CHIP_ERASE * 4;
  MmioWrite32((UINTN)&Regs->Lut[LutBase],
              OPRND_0(CMD_CHIP_ERASE) |
              PAD_0(LUT_PAD_1) | INSTR_0(LUT_CMD));
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 1], 0);
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 2], 0);
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 3], 0);

  /* Page Program */
  LutBase = SEQ_ID_PP * 4;
  if (QSPI_FLASH_SIZE  <= SIZE_16M)
    MmioWrite32((UINTN)&Regs->Lut[LutBase],
                     OPRND_0(CMD_PP) | PAD_0(LUT_PAD_1) |
                     INSTR_0(LUT_CMD) | OPRND_1(ADDR_24BIT) |
                     PAD_1(LUT_PAD_1) | INSTR_1(LUT_ADDR));
  else
    MmioWrite32((UINTN)&Regs->Lut[LutBase],
                     OPRND_0(CMD_PP_4B) | PAD_0(LUT_PAD_1) |
                     INSTR_0(LUT_CMD) | OPRND_1(ADDR_32BIT) |
                     PAD_1(LUT_PAD_1) | INSTR_1(LUT_ADDR));
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 1],
              OPRND_0(TX_BUFFER_SIZE) |
              PAD_0(LUT_PAD_1) | INSTR_0(LUT_WRITE));
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 2], 0);
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 3], 0);

  /* READ ID */
  LutBase = SEQ_ID_RDID * 4;
  MmioWrite32((UINTN)&Regs->Lut[LutBase], OPRND_0(CMD_RDID) |
         PAD_0(LUT_PAD_1) | INSTR_0(LUT_CMD) | OPRND_1(8) |
         PAD_1(LUT_PAD_1) | INSTR_1(LUT_READ));
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 1], 0);
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 2], 0);
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 3], 0);

  /* SUB SECTOR 4K ERASE */
  LutBase = SEQ_ID_BE_4K * 4;
  MmioWrite32((UINTN)&Regs->Lut[LutBase], OPRND_0(CMD_BE_4K) |
              PAD_0(LUT_PAD_1) | INSTR_0(LUT_CMD) | OPRND_1(ADDR_24BIT) |
              PAD_1(LUT_PAD_1) | INSTR_1(LUT_ADDR));

  /*
   * Read Any Device Register.
   * Used for Spansion S25FS-s Family Flash Only.
   */
  LutBase = SEQ_ID_RDAR * 4;
  MmioWrite32((UINTN)&Regs->Lut[LutBase],
              OPRND_0(CMD_RDAR) | PAD_0(LUT_PAD_1) |
              INSTR_0(LUT_CMD) | OPRND_1(ADDR_24BIT) |
              PAD_1(LUT_PAD_1) | INSTR_1(LUT_ADDR));
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 1],
              OPRND_0(8) | PAD_0(LUT_PAD_1) | INSTR_0(LUT_DUMMY) |
              OPRND_1(1) | PAD_1(LUT_PAD_1) |
              INSTR_1(LUT_READ));

  /*
   * Write Any Device Register.
   * Used for Spansion S25FS-s Family Flash Only.
   */
  LutBase = SEQ_ID_WRAR * 4;
  MmioWrite32((UINTN)&Regs->Lut[LutBase],
              OPRND_0(CMD_WRAR) | PAD_0(LUT_PAD_1) |
              INSTR_0(LUT_CMD) | OPRND_1(ADDR_24BIT) |
              PAD_1(LUT_PAD_1) | INSTR_1(LUT_ADDR));
  MmioWrite32((UINTN)&Regs->Lut[LutBase + 1],
              OPRND_0(1) | PAD_0(LUT_PAD_1) | INSTR_0(LUT_WRITE));

  /* Lock The LUT */
  MmioWrite32((UINTN)&Regs->Lutkey, LUT_KEY);
  MmioWrite32((UINTN)&Regs->Lckcr, LCKCR_LOCK);
}

struct QspiSlave *
QspiSetupSlave (
  IN  UINT32 Bus,
  IN  UINT32 Cs,
  IN  UINT32 MaxHz,
  IN  UINT32 Mode
  )
{
  struct QspiSlave *Qspislave;
  UINT32 McrVal = 0;
  UINT32 AmbaSizePerChip = 0;
  UINT8 I = 0;

  Qspislave = QspiCreateSlave(0, sizeof(struct QspiSlave), Bus,
				Cs, MaxHz, Mode);
  if (!Qspislave)
    return NULL;


  McrVal = MmioRead32((UINTN)&Qspislave->Regs->Mcr);
  McrVal |= MCR_RESERVED_MASK | MCR_MDIS_MASK;

  MmioWrite32((UINTN)&Qspislave->Regs->Mcr, McrVal);

  QspiInitSampling(&Qspislave->Regs->Smpr, ~(SMPR_FSDLY_MASK | SMPR_DDRSMP_MASK |
              SMPR_FSPHS_MASK | SMPR_HSENA_MASK), 0);

  /* Assign AMBA Memory Zone for Every CS
  * QSPI Has Two Channels And Every Channel Has Two CS.
  * if No Of CS Is 2, The AMBA Memory Will Be Divided Into Two Parts
  * And Assign To Every Channel. This Indicate That Every
  * Channel Only Has One Valid CS.
  * if No Of CS Is 4, The AMBA Memory Will Be Divided Into Four Parts
  * And Assign To Every Chipselect.Every Channel Will Has Two Valid CS.
  */
  AmbaSizePerChip = Qspislave->AmbaTotalSize >>
                    (Qspislave->NumChipselect >> 1);
  for (I = 1 ; I < Qspislave->NumChipselect ; I++)
    Qspislave->AmbaBase[I] =
       AmbaSizePerChip + Qspislave->AmbaBase[I - 1];

  /*
  * In case Od Single Die Flash Devices, TOP_ADDR_MEMA2 And
  * TOP_ADDR_MEMB2 Should Be Initialized To TOP_ADDR_MEMA1
  * And TOP_ADDR_MEMB1 Respectively. This Would Ensure
  * That Complete Memory Map Is Assigned To One Flash Device.
  */
  MmioWrite32((UINTN)&Qspislave->Regs->Sfa1ad, Qspislave->AmbaBase[1]);
  switch (Qspislave->NumChipselect) {
  case 2:
    MmioWrite32((UINTN)&Qspislave->Regs->Sfa2ad,
                Qspislave->AmbaBase[1]);
    MmioWrite32((UINTN)&Qspislave->Regs->Sfb1ad,
                Qspislave->AmbaBase[1] + AmbaSizePerChip);
    MmioWrite32((UINTN)&Qspislave->Regs->Sfb2ad,
                Qspislave->AmbaBase[1] + AmbaSizePerChip);
    break;
  case 4:
    MmioWrite32((UINTN)&Qspislave->Regs->Sfa2ad,
                Qspislave->AmbaBase[2]);
    MmioWrite32((UINTN)&Qspislave->Regs->Sfb1ad,
                Qspislave->AmbaBase[3]);
    MmioWrite32((UINTN)&Qspislave->Regs->Sfb2ad,
                Qspislave->AmbaBase[3] + AmbaSizePerChip);
    break;
  default:
    DEBUG((EFI_D_ERROR, "Error: Unsupported Chipselect Number %u!\n",
          Qspislave->NumChipselect));
    DisableQspiModule(&Qspislave->Regs->Mcr, 1);
    return NULL;
  }

  QspiPrepareLut(Qspislave);

  DisableQspiModule(&Qspislave->Regs->Mcr, 0);

  return Qspislave;
}

VOID
QspiFreeSlave (
  IN  struct QspiSlave *Slave
  )
{
  FreePool(Slave);
}

EFI_STATUS
QspiClaimBus (
  IN  struct QspiSlave *Qspislave
  )
{

  DisableQspiModule(&Qspislave->Regs->Mcr, 0);

  return EFI_SUCCESS;
}

VOID
QspiReleaseBus (
  IN  struct QspiSlave *Slave
  )
{
  DisableQspiModule(&Slave->Regs->Mcr, 0);
  return;
}

EFI_STATUS
QspiReadWrite (
  IN  struct QspiSlave *Qspi,
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

  Ret = QspiXfer(Qspi, CmdLen * 8, Cmd, NULL, Flags);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "QSPI: Failed To Send Command (%zu Bytes): %d\n",
			CmdLen, Ret));
    return Ret;
  } else if (DataLen != 0) {
    Ret = QspiXfer(Qspi, DataLen * 8, DataOut, DataIn, SPI_COMMON_XFER_END);
    if (Ret != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "QSPI: Failed To Transfer %zu Bytes Of Data: %d\n",
			DataLen, Ret));
      return Ret;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
QspiWaitReady (
  IN  struct QspiFlash *Flash,
  IN  UINT64 Timeout
  )
{
  struct QspiSlave *Qspi = Flash->Qspi;
  UINT64 Timebase;
  UINT64 Flags = SPI_COMMON_XFER_BEGIN;
  EFI_STATUS Ret = EFI_SUCCESS;
  UINT8 Status = 0;
  UINT8 CheckStatus = 0x0;
  UINT8 PollBit = STATUS_WIP;
  UINT8 Cmd = Flash->PollCmd;

  if (Cmd == READ_FLAG_STATUS) {
    PollBit = STATUS_PEC;
    CheckStatus = PollBit;
  }

  Ret = QspiXfer(Qspi, 8, &Cmd, NULL, Flags);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "QSPI: Fail To Read %a Status Register\n",
          Cmd == READ_STATUS ? "Read" : "Flag"));
    return Ret;
  }

  Timebase = 0;
  do {
    MicroSecondDelay(10000);

    Ret = QspiXfer(Qspi, 8, NULL, &Status, 0);
    if (Ret != EFI_SUCCESS) {
      QspiXfer(Qspi, 0, NULL, NULL, SPI_COMMON_XFER_END);
      return Ret;
    }

    if ((Status & PollBit) == CheckStatus) {
      break;
    }

  } while (++Timebase < Timeout);

  QspiXfer(Qspi, 0, NULL, NULL, SPI_COMMON_XFER_END);
  if ((Status & PollBit) == CheckStatus)
    return EFI_SUCCESS;

  /* Timed Out */
  DEBUG((EFI_D_ERROR, "QSPI: Time Out!\n"));
  Ret = EFI_TIMEOUT;

  return Ret;
}

EFI_STATUS
QspiCommonWrite (
  IN  struct QspiFlash *Flash,
  IN  CONST UINT8 *Cmd,
  IN  UINT32 CmdLen,
  IN  CONST VOID *Buf,
  IN  UINT32 BufLen
  )
{
  struct QspiSlave *Qspi = Flash->Qspi;
  UINT64 Timeout = SPI_COMMON_FLASH_PROG_TIMEOUT;
  INT32 Ret;
  UINT8 Wcmd = 0;

  if (Buf == NULL)
    Timeout = SPI_COMMON_FLASH_PAGE_ERASE_TIMEOUT;

  Ret = QspiClaimBus(Flash->Qspi);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "Unable To Claim QSPI Bus\n"));
    return Ret;
  }

  Wcmd = WRITE_ENABLE;

  Ret = QspiReadWrite(Qspi, &Wcmd, 1, NULL, NULL, 0);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "Enabling QSPI Write Failed\n"));
    return Ret;
  }

  Ret = QspiReadWrite(Qspi, Cmd, CmdLen, Buf, NULL, BufLen);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "QSPI Write Cmd Failed\n"));
    return Ret;
  }

  Ret = QspiWaitReady(Flash, Timeout);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "QSPI Write %s Failed\n",
          Timeout == SPI_COMMON_FLASH_PROG_TIMEOUT ?
           "Program" : "Page Erase"));
    return Ret;
  }

  QspiReleaseBus(Qspi);
  return Ret;
}

VOID
QspiCreateAddr (
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
QspiCommonRead (
  IN  struct QspiFlash *Flash,
  IN  CONST UINT8 *Cmd,
  IN  UINT32 CmdLen,
  OUT VOID *Data,
  IN  UINT64 DataLen
  )
{
  struct QspiSlave *Qspi = Flash->Qspi;
  INT32 Ret;

  Ret = QspiClaimBus(Flash->Qspi);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "Unable To Claim QSPI Bus\n"));
    return Ret;
  }

  Ret = QspiReadWrite(Qspi, Cmd, CmdLen, NULL, Data, DataLen);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "QSPI Read Cmd Failed\n"));
    return Ret;
  }

  QspiReleaseBus(Qspi);

  return Ret;
}
