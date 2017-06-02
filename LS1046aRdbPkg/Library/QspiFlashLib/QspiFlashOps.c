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

#include <PiDxe.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>

#include "QspiFlashLib.h"

VOID DumpQspiRegs(struct QspiRegs *QspiRegs)
{
  UINT32 Index = 0;

  DEBUG((EFI_D_INFO,"Mcr		:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Mcr)));
  DEBUG((EFI_D_INFO,"Ipcr    	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Ipcr)));
  DEBUG((EFI_D_INFO,"Flshcr  	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Flshcr)));
  DEBUG((EFI_D_INFO,"Buf0cr  	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Buf0cr)));
  DEBUG((EFI_D_INFO,"Buf1cr  	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Buf1cr)));
  DEBUG((EFI_D_INFO,"Buf2cr  	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Buf2cr)));
  DEBUG((EFI_D_INFO,"Buf3cr  	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Buf3cr)));
  DEBUG((EFI_D_INFO,"Bfgencr 	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Bfgencr)));
  DEBUG((EFI_D_INFO,"Soccr   	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Soccr)));
  DEBUG((EFI_D_INFO,"Buf0ind 	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Buf0ind)));
  DEBUG((EFI_D_INFO,"Buf1ind 	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Buf1ind)));
  DEBUG((EFI_D_INFO,"Buf2ind 	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Buf2ind)));
  DEBUG((EFI_D_INFO,"Sfar    	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Sfar)));
  DEBUG((EFI_D_INFO,"Smpr    	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Smpr)));
  DEBUG((EFI_D_INFO,"Rbsr    	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbsr)));
  DEBUG((EFI_D_INFO,"Rbct    	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbct)));
  DEBUG((EFI_D_INFO,"Tbsr    	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Tbsr)));
  DEBUG((EFI_D_INFO,"Tbdr    	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Tbdr)));
  DEBUG((EFI_D_INFO,"Sr      	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Sr)));
  DEBUG((EFI_D_INFO,"Fr      	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Fr)));
  DEBUG((EFI_D_INFO,"Rser    	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rser)));
  DEBUG((EFI_D_INFO,"Spndst  	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Spndst)));
  DEBUG((EFI_D_INFO,"Sptrclr 	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Sptrclr)));
  DEBUG((EFI_D_INFO,"Sfa1ad  	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Sfa1ad)));
  DEBUG((EFI_D_INFO,"Sfa2ad  	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Sfa2ad)));
  DEBUG((EFI_D_INFO,"Sfb1ad  	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Sfb1ad)));
  DEBUG((EFI_D_INFO,"Sfb2ad  	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Sfb2ad)));
  DEBUG((EFI_D_INFO,"Rbdr[0]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[0])));
  DEBUG((EFI_D_INFO,"Rbdr[1]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[1])));
  DEBUG((EFI_D_INFO,"Rbdr[2]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[2])));
  DEBUG((EFI_D_INFO,"Rbdr[3]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[3])));
  DEBUG((EFI_D_INFO,"Rbdr[4]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[4])));
  DEBUG((EFI_D_INFO,"Rbdr[5]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[5])));
  DEBUG((EFI_D_INFO,"Rbdr[6]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[6])));
  DEBUG((EFI_D_INFO,"Rbdr[7]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[7])));
  DEBUG((EFI_D_INFO,"Rbdr[8]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[8])));
  DEBUG((EFI_D_INFO,"Rbdr[9]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[9])));
  DEBUG((EFI_D_INFO,"Rbdr[10]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[10])));
  DEBUG((EFI_D_INFO,"Rbdr[11]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[11])));
  DEBUG((EFI_D_INFO,"Rbdr[12]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[12])));
  DEBUG((EFI_D_INFO,"Rbdr[13]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[13])));
  DEBUG((EFI_D_INFO,"Rbdr[14]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[14])));
  DEBUG((EFI_D_INFO,"Rbdr[15]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[15])));
  DEBUG((EFI_D_INFO,"Rbdr[16]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[16])));
  DEBUG((EFI_D_INFO,"Rbdr[17]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[17])));
  DEBUG((EFI_D_INFO,"Rbdr[18]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[18])));
  DEBUG((EFI_D_INFO,"Rbdr[19]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[19])));
  DEBUG((EFI_D_INFO,"Rbdr[20]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[20])));
  DEBUG((EFI_D_INFO,"Rbdr[21]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[21])));
  DEBUG((EFI_D_INFO,"Rbdr[22]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[22])));
  DEBUG((EFI_D_INFO,"Rbdr[23]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[23])));
  DEBUG((EFI_D_INFO,"Rbdr[24]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[24])));
  DEBUG((EFI_D_INFO,"Rbdr[25]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[25])));
  DEBUG((EFI_D_INFO,"Rbdr[26]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[26])));
  DEBUG((EFI_D_INFO,"Rbdr[27]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[27])));
  DEBUG((EFI_D_INFO,"Rbdr[28]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[28])));
  DEBUG((EFI_D_INFO,"Rbdr[29]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[29])));
  DEBUG((EFI_D_INFO,"Rbdr[30]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[30])));
  DEBUG((EFI_D_INFO,"Rbdr[31]	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Rbdr[31])));
  DEBUG((EFI_D_INFO,"Lutkey  	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Lutkey)));
  DEBUG((EFI_D_INFO,"Lckcr   	:0x%x \n", MmioReadBe32((UINTN)&QspiRegs->Lckcr)));
  for (Index = 0; Index<64; Index++)
    DEBUG((EFI_D_INFO,"Lut[%d] 	:0x%x \n",
	Index, MmioReadBe32((UINTN)&QspiRegs->Lut[Index])));
}

VOID
DisableQspiModule (
  IN  UINT32  *Mcr,
  IN  UINT32  Halt
  )
{
  UINT32 Val;

  Val = MmioReadBe32((UINTN)Mcr);

  if (Halt)
    Val |= MCR_MDIS_MASK;
  else
    Val &= ~MCR_MDIS_MASK;

  MmioWriteBe32((UINTN)Mcr, Val);
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
  EFI_STATUS       Status = EFI_SUCCESS;

  Ptr = (struct QspiSlave*)AllocateRuntimePool(Size);
  if (!Ptr)
    return Ptr;

  InternalMemZeroMem((UINT8 *)Ptr, Size);

  Ptr->Slave.Bus = Bus;
  Ptr->Slave.Cs = Cs;
  Ptr->Slave.Speed = Speed;
  Ptr->Slave.Mode = Mode;
  Ptr->Slave.Wordlen = SPI_COMMON_DEFAULT_WORDLEN;
  Ptr->Slave.MaxWriteSize = 0;
  Ptr->CurAmbaBase = AMBA_BASE;
  Ptr->AmbaTotalSize = AMBA_TOTAL_SIZE;
  Ptr->Regs = (VOID *)QSPI_BASE_ADDR;

  // Declare the controller as EFI_MEMORY_RUNTIME
  Status = gDS->AddMemorySpace (
                  EfiGcdMemoryTypeMemoryMappedIo,
                  (UINTN)Ptr->Regs, QSPI_REG_SIZE,
                  EFI_MEMORY_UC | EFI_MEMORY_RUNTIME
                  );
  if (EFI_ERROR (Status)){
    FreePool(Ptr);
    return NULL;
  }
  Status = gDS->SetMemorySpaceAttributes ((UINTN)Ptr->Regs, QSPI_REG_SIZE,
                                          EFI_MEMORY_UC | EFI_MEMORY_RUNTIME);
  if (EFI_ERROR (Status)){
    FreePool(Ptr);
    return NULL;
  }

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
    Ret = QspiWriteConfig(Flash, QebStatus|STATUS_QEB_WINSPAN);
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

  McrReg = MmioReadBe32((UINTN)Mcr);
  MmioWriteBe32((UINTN)Mcr,
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
   UINT32 TxBuffEmpty = 0;
   UINT32 TxBuffFill = 0;

   McrReg = UpdateMcr(&Regs->Mcr);
   MmioWriteBe32((UINTN)&Regs->Rbct, RBCT_RXBRD_USEIPS);

   StatusReg = 0;
   while ((StatusReg & STATUS_WEL) != STATUS_WEL) {
     MmioWriteBe32((UINTN)&Regs->Ipcr,
                 (SEQ_ID_WREN << IPCR_SEQID_SHIFT) | 0);
     while (MmioReadBe32((UINTN)&Regs->Sr) & SR_BUSY_MASK)
            ;

     MmioWriteBe32((UINTN)&Regs->Ipcr,
                 (SEQ_ID_RDSR << IPCR_SEQID_SHIFT) | 1);
     while (MmioReadBe32((UINTN)&Regs->Sr) & SR_BUSY_MASK)
            ;

     Reg = MmioReadBe32((UINTN)&Regs->Rbsr);
     if (Reg & RBSR_RDBFL_MASK) {
       StatusReg = MmioReadBe32((UINTN)&Regs->Rbdr[0]);
     }
     MmioWriteBe32((UINTN)&Regs->Mcr,
                 MmioReadBe32((UINTN)&Regs->Mcr) |
                 MCR_CLR_RXF_MASK);
   }

   switch(QSlave->CurSeqid) {
     case CMD_WRAR:
       Seqid = SEQ_ID_WRAR;
       break;
     case CMD_PP:
     case QUAD_PAGE_PROGRAM:
       /*
        * Write Lut is programmed after flash probe for either Page Program or
        * Quad Page Program depending on the Program mode supported by flash
        */
       Seqid = SEQ_ID_WRITE;
       break;
     case WRITE_STATUS:
       Seqid = SEQ_ID_WRSR;
       break;
     default:
       DEBUG((EFI_D_ERROR,"%a Could not recognize write command\n",__FUNCTION__));
       return;
   }

   ToOrFrom = QSlave->SfAddr + QSlave->CurAmbaBase;

   MmioWriteBe32((UINTN)&Regs->Sfar, ToOrFrom);

   TxSize = (Len > TX_BUFFER_SIZE) ?
          TX_BUFFER_SIZE : Len;

   Size = TxSize / 4;
   for (I = 0; I < Size; I++) {
     InternalMemCopyMem(&Data, Txbuf, 4);
     MmioWriteBe32((UINTN)&Regs->Tbdr, Data);
     Txbuf += 4;
   }

   Size = TxSize % 4;
   if (Size) {
     Data = 0;
     InternalMemCopyMem(&Data, Txbuf, Size);
     MmioWriteBe32((UINTN)&Regs->Tbdr, Data);
   }

   MmioWriteBe32((UINTN)&Regs->Ipcr,
                (Seqid << IPCR_SEQID_SHIFT) | Len);
   Len -= TxSize;
   while (Len) {
     if(MmioReadBe32((UINTN)&Regs->Sr) & SR_TXFULL) {
       /* TX Buffer Full. No more data can be stored. */
       continue;
     }
     TxSize = MIN(TX_BUFFER_SIZE,Len);
     /*
      * TX Buffer Fill Level. The TRBFL field contains the number of *
      * entries of 4 bytes each available in the TX Buffer for the   *
      * QuadSPI module to transmit to the serial flash device.       *
      */
     TxBuffFill = ((MmioReadBe32((UINTN)&Regs->Tbsr) & TBSR_TRBL_MASK) >> TBSR_TRBL_SHIFT)*4;
     TxBuffEmpty = TX_BUFFER_SIZE - TxBuffFill;
     TxSize = MIN(TxSize, TxBuffEmpty);
     if (TxSize) {
       Size = TxSize / 4;
       for (I = 0; I < Size; I++) {
         InternalMemCopyMem(&Data, Txbuf, 4);
         MmioWriteBe32((UINTN)&Regs->Tbdr, Data);
         Txbuf += 4;
       }

       Size = TxSize % 4;
       if (Size) {
         Data = 0;
         InternalMemCopyMem(&Data, Txbuf, Size);
         MmioWriteBe32((UINTN)&Regs->Tbdr, Data);
       }
       Len -= TxSize;
     }
   }
   while (MmioReadBe32((UINTN)&Regs->Sr) & SR_BUSY_MASK)
          ;

   MmioWriteBe32((UINTN)&Regs->Mcr, McrReg);
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
  MmioWriteBe32((UINTN)&Regs->Rbct, RBCT_RXBRD_USEIPS);

  ToOrFrom = QSlave->SfAddr + QSlave->CurAmbaBase;
  MmioWriteBe32((UINTN)&Regs->Sfar, ToOrFrom);

  MmioWriteBe32((UINTN)&Regs->Ipcr,
              (SEQ_ID_WREN << IPCR_SEQID_SHIFT) | 0);
  while (MmioReadBe32((UINTN)&Regs->Sr) & SR_BUSY_MASK)
         ;

    MmioWriteBe32((UINTN)&Regs->Ipcr,
             (SEQ_ID_SECTOR_ERASE << IPCR_SEQID_SHIFT) | 0);
  while (MmioReadBe32((UINTN)&Regs->Sr) & SR_BUSY_MASK)
         ;

  MmioWriteBe32((UINTN)&Regs->Mcr, McrReg);
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
    Seqid = SEQ_ID_READ;

  McrReg = UpdateMcr(&Regs->Mcr);
  MmioWriteBe32((UINTN)&Regs->Rbct, RBCT_RXBRD_USEIPS);

  From = QSlave->SfAddr + QSlave->CurAmbaBase;

  while (Len > 0) {
    MmioWriteBe32((UINTN)&Regs->Sfar, From);

    Size = (Len > RX_BUFFER_SIZE) ? RX_BUFFER_SIZE : Len;

    MmioWriteBe32((UINTN)&Regs->Ipcr,
                (Seqid << IPCR_SEQID_SHIFT) | Size);
    while (MmioReadBe32((UINTN)&Regs->Sr) & SR_BUSY_MASK)
           ;

    From += Size;
    Len -= Size;

    I = 0;
    while ((RX_BUFFER_SIZE >= Size) && (Size > 0)) {
      Data = MmioReadBe32((UINTN)&Regs->Rbdr[I]);
      if (Size < 4)
        InternalMemCopyMem(Rxbuf, &Data, Size);
      else
        InternalMemCopyMem(Rxbuf, &Data, 4);
      Rxbuf++;
      Size -= 4;
      I++;
    }
    MmioWriteBe32((UINTN)&Regs->Mcr,
                MmioReadBe32((UINTN)&Regs->Mcr) |
                MCR_CLR_RXF_MASK);
  }

  MmioWriteBe32((UINTN)&Regs->Mcr, McrReg);
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
  MmioWriteBe32((UINTN)&Regs->Rbct, RBCT_RXBRD_USEIPS);

  MmioWriteBe32((UINTN)&Regs->Sfar, QSlave->CurAmbaBase);

  MmioWriteBe32((UINTN)&Regs->Ipcr,
              (SEQ_ID_RDID << IPCR_SEQID_SHIFT) | 0);
  while (MmioReadBe32((UINTN)&Regs->Sr) & SR_BUSY_MASK)
         ;

  Size = Len;
  while ((RX_BUFFER_SIZE >= Size) && (Size > 0)) {
    RbsrReg = MmioReadBe32((UINTN)&Regs->Rbsr);
    if (RbsrReg & RBSR_RDBFL_MASK) {
      Data = MmioReadBe32((UINTN)&Regs->Rbdr[I]);
      InternalMemCopyMem(Rxbuf, &Data, BYTES_IN_RX_BUFFER);
      Rxbuf++;
      Size -= 4;
      I++;
    }
  }

  MmioWriteBe32((UINTN)&Regs->Mcr, McrReg);
}

/*
 * Read Status register (Sr) and Configuration register (Cr)
 */
VOID
ReadSrCrOperation (
  IN   struct QspiSlave *QSlave,
  OUT UINT32 *Rxbuf
  )
{
  struct QspiRegs *Regs = QSlave->Regs;
  UINT32 McrReg, Reg, Data;
  UINT32 Seqid;

  if (QSlave->CurSeqid == READ_CONFIG) {
    Seqid = SEQ_ID_RDCR;
  } else {
    Seqid = SEQ_ID_RDSR;
  }

  McrReg = MmioReadBe32((UINTN)&Regs->Mcr);
  McrReg = UpdateMcr(&Regs->Mcr);
  MmioWriteBe32((UINTN)&Regs->Rbct, RBCT_RXBRD_USEIPS);

  MmioWriteBe32((UINTN)&Regs->Sfar, QSlave->CurAmbaBase);

  MmioWriteBe32((UINTN)&Regs->Ipcr,
                (Seqid << IPCR_SEQID_SHIFT) | 0);
  while (MmioReadBe32((UINTN)&Regs->Sr) & SR_BUSY_MASK)
         ;

  while (1) {
    Reg = MmioReadBe32((UINTN)&Regs->Rbsr);
    if (Reg & RBSR_RDBFL_MASK) {
      Data = MmioReadBe32((UINTN)&Regs->Rbdr[0]);
      InternalMemCopyMem(Rxbuf, &Data, 4);
      MmioWriteBe32((UINTN)&Regs->Mcr,
                  MmioReadBe32((UINTN)&Regs->Mcr) |
                  MCR_CLR_RXF_MASK);
      break;
    }
  }

  MmioWriteBe32((UINTN)&Regs->Mcr, McrReg);
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
  UINT32 Txbuf = 0;

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
      case READ_QUAD_OUTPUT_FAST:
      case READ_QUAD_IO_FAST:
        QspiSlave->SfAddr = Uswap32(Txbuf) & OFFSET_MASK;
        break;
      case CMD_SE:
      case ERASE_4K:
        QspiSlave->SfAddr = Uswap32(Txbuf) & OFFSET_MASK;
        EraseOperation(QspiSlave);
        break;
      case CMD_PP:
      case CMD_WRAR:
      case QUAD_PAGE_PROGRAM:
        WrSfaddr = Uswap32(Txbuf) & OFFSET_MASK;
        break;
    }
  }

  if (InData) {
    switch (QspiSlave->CurSeqid) {
      case CMD_FAST_READ:
      case CMD_RDAR:
      case READ_QUAD_OUTPUT_FAST:
      case READ_QUAD_IO_FAST:
        ReadOperation(QspiSlave, InData, Bytes);
        break;
      case CMD_RDID:
        ReadIdOperation(QspiSlave, InData, Bytes);
        break;
      case CMD_RDSR:
      case READ_CONFIG:
        ReadSrCrOperation(QspiSlave, InData);
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

  Val = MmioReadBe32((UINTN)Smpr);
  Val &= ~Clear;
  Val |= Set;
  MmioWriteBe32((UINTN)Smpr, Val);
}

VOID
QspiPrepareLutPreProbe (
  IN  struct QspiSlave *QspiSlave
  )
{
  struct QspiRegs *Regs = QspiSlave->Regs;
  UINT32 *LutBase = Regs->Lut;
  UINT32 LutId = 0;

  /* Unlock The LUT */
  MmioWriteBe32((UINTN)&Regs->Lutkey, LUT_KEY);
  MmioWriteBe32((UINTN)&Regs->Lckcr, LCKCR_UNLOCK);

  /* READ ID */
  LutId = SEQ_ID_RDID * 4;
  CLEAR_LUT(LutBase, LutId);
  MmioWriteBe32((UINTN)&LutBase[LutId], LUT_0(CMD, PAD_1, CMD_RDID) |
                                            LUT_1(READ, PAD_1, 8));
  /* Write Enable */
  LutId = SEQ_ID_WREN * 4;
  CLEAR_LUT(LutBase, LutId);
  MmioWriteBe32((UINTN)&LutBase[LutId], LUT_0(CMD, PAD_1, CMD_WREN));

  /* Read Status */
  LutId = SEQ_ID_RDSR * 4;
  CLEAR_LUT(LutBase, LutId);
  MmioWriteBe32((UINTN)&LutBase[LutId], LUT_0(CMD, PAD_1, CMD_RDSR) |
                                            LUT_1(READ, PAD_1, 1));

  /* Write Configuration and Status Register */
  LutId = SEQ_ID_WRSR * 4;
  CLEAR_LUT(LutBase, LutId);
  MmioWriteBe32((UINTN)&LutBase[LutId], LUT_0(CMD, PAD_1, WRITE_STATUS) |
                                        LUT_1(WRITE, PAD_1, 2));

  /* Read Configuration Register */
  LutId = SEQ_ID_RDCR * 4;
  CLEAR_LUT(LutBase, LutId);
  MmioWriteBe32((UINTN)&LutBase[LutId], LUT_0(CMD, PAD_1, READ_CONFIG) |
                                        LUT_1(READ, PAD_1, 1));

  /* Lock The LUT */
  MmioWriteBe32((UINTN)&Regs->Lutkey, LUT_KEY);
  MmioWriteBe32((UINTN)&Regs->Lckcr, LCKCR_LOCK);
}

VOID
QspiPrepareLutPostProbe (
  IN  struct QspiFlash *Flash
)
{
  struct QspiSlave *QSlave = Flash->Qspi;
  struct QspiRegs *Regs = QSlave->Regs;
  UINT32 *LutBase = Regs->Lut;
  UINT32 LutId = 0;

  /* Unlock The LUT */
  MmioWriteBe32((UINTN)&Regs->Lutkey, LUT_KEY);
  MmioWriteBe32((UINTN)&Regs->Lckcr, LCKCR_UNLOCK);

  /* Read */
  LutId = SEQ_ID_READ * 4;
  CLEAR_LUT(LutBase, LutId);
  switch (Flash->ReadCmd) {
    case READ_QUAD_OUTPUT_FAST:
      MmioWriteBe32((UINTN)&LutBase[LutId], LUT_0(CMD, PAD_1, READ_QUAD_OUTPUT_FAST) |
                                            LUT_1(ADDR, PAD_1, ADDR_24BIT));
      MmioWriteBe32((UINTN)&LutBase[LutId+1], LUT_0(DUMMY, PAD_1, 8) |
                                              LUT_1(READ, PAD_4, RX_BUFFER_SIZE));
      break;

    case READ_QUAD_IO_FAST:
      MmioWriteBe32((UINTN)&LutBase[LutId], LUT_0(CMD, PAD_1, READ_QUAD_IO_FAST) |
                                            LUT_1(ADDR, PAD_4, ADDR_24BIT));
      MmioWriteBe32((UINTN)&LutBase[LutId+1], LUT_0(MODE, PAD_4, 0xFF) |
                                              LUT_1(DUMMY, PAD_4, 4));
      MmioWriteBe32((UINTN)&LutBase[LutId+2], LUT_0(READ, PAD_4, RX_BUFFER_SIZE));
      break;

    default:
      if (QSPI_FLASH_SIZE  <= SIZE_16MB) {
        MmioWriteBe32((UINTN)&LutBase[LutId], LUT_0(CMD, PAD_1, CMD_FAST_READ) |
                                              LUT_1(ADDR, PAD_1, ADDR_24BIT));
      } else {
        MmioWriteBe32((UINTN)&LutBase[LutId], LUT_0(CMD, PAD_1, CMD_FAST_READ_4B) |
                                              LUT_1(ADDR, PAD_1, ADDR_32BIT));
      }
      MmioWriteBe32((UINTN)&LutBase[LutId+1], LUT_0(DUMMY, PAD_1, 8) |
                                            LUT_1(READ, PAD_1, RX_BUFFER_SIZE));
      break;
  }

  LutId = SEQ_ID_WRITE * 4;
  CLEAR_LUT(LutBase, LutId);
  if (Flash->WriteCmd == QUAD_PAGE_PROGRAM) {
    /* Quad Page Program */
    MmioWriteBe32((UINTN)&LutBase[LutId], LUT_0(CMD, PAD_1, QUAD_PAGE_PROGRAM) |
                                          LUT_1(ADDR, PAD_1, ADDR_24BIT));
    MmioWriteBe32((UINTN)&LutBase[LutId+1], LUT_0(WRITE, PAD_4, 0));
  } else {
    /* Page Program */
    if (QSPI_FLASH_SIZE  <= SIZE_16MB) {
      MmioWriteBe32((UINTN)&LutBase[LutId], LUT_0(CMD, PAD_1, CMD_PP) |
                                            LUT_1(ADDR, PAD_1, ADDR_24BIT));
    } else {
      MmioWriteBe32((UINTN)&LutBase[LutId], LUT_0(CMD, PAD_1, CMD_PP_4B) |
                                            LUT_1(ADDR, PAD_1, ADDR_32BIT));
    }
    MmioWriteBe32((UINTN)&LutBase[LutId+1], LUT_0(WRITE, PAD_1, TX_BUFFER_SIZE));
  }

  /* Erase a Sector */
  LutId = SEQ_ID_SECTOR_ERASE * 4;
  CLEAR_LUT(LutBase, LutId);
  if (Flash->EraseCmd == ERASE_4K) {
    MmioWriteBe32((UINTN)&LutBase[LutId], LUT_0(CMD, PAD_1, ERASE_4K) |
                                          LUT_1(ADDR, PAD_1, ADDR_24BIT));
  } else {
    if (QSPI_FLASH_SIZE  <= SIZE_16MB) {
      MmioWriteBe32((UINTN)&LutBase[LutId], LUT_0(CMD, PAD_1, CMD_SE) |
                                            LUT_1(ADDR, PAD_1, ADDR_24BIT));
    } else {
      MmioWriteBe32((UINTN)&LutBase[LutId], LUT_0(CMD, PAD_1, CMD_SE_4B) |
                                            LUT_1(ADDR, PAD_1, ADDR_32BIT));
    }
  }

  /* Erase The Whole Chip */
  LutId = SEQ_ID_CHIP_ERASE * 4;
  CLEAR_LUT(LutBase, LutId);
  MmioWriteBe32((UINTN)&LutBase[LutId], LUT_0(CMD, PAD_1, CMD_CHIP_ERASE));

  if (AsciiStrStr(Flash->Name, "S25FL128S") || AsciiStrStr(Flash->Name, "S25FS512S")) {
    /*
     * Read Any Device Register.
     * Used for Spansion S25FS-s Family Flash Only.
     */
    LutId = SEQ_ID_RDAR * 4;
    CLEAR_LUT(LutBase, LutId);
    MmioWriteBe32((UINTN)&LutBase[LutId], LUT_0(CMD, PAD_1, CMD_RDAR) |
                                          LUT_1(ADDR, PAD_1, ADDR_24BIT));
    MmioWriteBe32((UINTN)&LutBase[LutId+1], LUT_0(DUMMY, PAD_1, 8) |
                                          LUT_1(READ, PAD_1, 1));
    /*
     * Write Any Device Register.
     * Used for Spansion S25FS-s Family Flash Only.
     */
    LutId = SEQ_ID_WRAR * 4;
    CLEAR_LUT(LutBase, LutId);
    MmioWriteBe32((UINTN)&LutBase[LutId], LUT_0(CMD, PAD_1, CMD_WRAR) |
                                          LUT_1(ADDR, PAD_1, ADDR_24BIT));
    MmioWriteBe32((UINTN)&LutBase[LutId+1], LUT_0(WRITE, PAD_1, 1));
  }

  /* Lock The LUT */
  MmioWriteBe32((UINTN)&Regs->Lutkey, LUT_KEY);
  MmioWriteBe32((UINTN)&Regs->Lckcr, LCKCR_LOCK);
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

  McrVal = MmioReadBe32((UINTN)&Qspislave->Regs->Mcr);
  McrVal |= MCR_RESERVED_MASK | MCR_MDIS_MASK | (McrVal & MCR_END_CFD_MASK);

  MmioWriteBe32((UINTN)&Qspislave->Regs->Mcr, McrVal);

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
  MmioWriteBe32((UINTN)&Qspislave->Regs->Sfa1ad, Qspislave->AmbaBase[1]);
  switch (Qspislave->NumChipselect) {
  case 2:
    MmioWriteBe32((UINTN)&Qspislave->Regs->Sfa2ad,
                Qspislave->AmbaBase[1]);
    MmioWriteBe32((UINTN)&Qspislave->Regs->Sfb1ad,
                Qspislave->AmbaBase[1] + AmbaSizePerChip);
    MmioWriteBe32((UINTN)&Qspislave->Regs->Sfb2ad,
                Qspislave->AmbaBase[1] + AmbaSizePerChip);
    break;
  case 4:
    MmioWriteBe32((UINTN)&Qspislave->Regs->Sfa2ad,
                Qspislave->AmbaBase[2]);
    MmioWriteBe32((UINTN)&Qspislave->Regs->Sfb1ad,
                Qspislave->AmbaBase[3]);
    MmioWriteBe32((UINTN)&Qspislave->Regs->Sfb2ad,
                Qspislave->AmbaBase[3] + AmbaSizePerChip);
    break;
  default:
    DEBUG((EFI_D_ERROR, "Error: Unsupported Chipselect Number %u!\n",
          Qspislave->NumChipselect));
    DisableQspiModule(&Qspislave->Regs->Mcr, 1);
    return NULL;
  }

  QspiPrepareLutPreProbe(Qspislave);

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

UINT64
ConvertMilliSecondsToTicks(
  IN UINTN MilliSeconds
)
{
  UINT64 TimerTicks64 = 0;

  // Calculate counter ticks that represent requested delay:
  //  = MilliSeconds x TICKS_PER_MICRO_SEC
  //  = MilliSeconds x Timer Frequency(in Hz) x 10^-3
  // GetPerformanceCounterProperties = Get Arm Timer Frequency in Hz
  TimerTicks64 = DivU64x32 (
                   MultU64x64 (
                     MilliSeconds,
                     GetPerformanceCounterProperties (NULL, NULL)
                     ),
                   1000U
                   );
  return TimerTicks64;
}

EFI_STATUS
QspiWaitReady (
  IN  struct QspiFlash *Flash,
  IN  UINT64 Timeout
  )
{
  struct QspiSlave *Qspi = Flash->Qspi;
  UINT64 Flags = SPI_COMMON_XFER_BEGIN;
  EFI_STATUS Ret = EFI_SUCCESS;
  UINT8 Status = 0;
  UINT8 CheckStatus = 0x0;
  UINT8 PollBit = STATUS_WIP;
  UINT8 Cmd = Flash->PollCmd;
  UINT64 SystemCounterVal = 0;
  UINT64 TimerTicks64 = 0;

  if (Cmd == FLAG_STATUS) {
    PollBit = STATUS_PEC;
    CheckStatus = PollBit;
  }

  Ret = QspiXfer(Qspi, 8, &Cmd, NULL, Flags);
  if (Ret != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "QSPI: Fail To Read %a Status Register\n",
          Cmd == READ_STATUS ? "Read" : "Flag"));
    return Ret;
  }

  TimerTicks64 = ConvertMilliSecondsToTicks(Timeout);
  SystemCounterVal = GetPerformanceCounter();
  TimerTicks64 += SystemCounterVal;
  do {
    Ret = QspiXfer(Qspi, 8, NULL, &Status, 0);
    if (Ret != EFI_SUCCESS) {
      QspiXfer(Qspi, 0, NULL, NULL, SPI_COMMON_XFER_END);
      return Ret;
    }

    if ((Status & PollBit) == CheckStatus) {
      break;
    }
    SystemCounterVal = GetPerformanceCounter();

  } while (SystemCounterVal < TimerTicks64);

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
