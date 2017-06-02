/** SdxcBoot.c
  Containing Functions for SD boot

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This Program And The Accompanying Materials
  Are Licensed And Made Available Under The Terms And Conditions Of The BSD License
  Which Accompanies This Distribution.  The Full Text Of The License May Be Found At
  Http://Opensource.Org/Licenses/Bsd-License.Php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/Sdxc.h>

extern CONST INT32 FreqBase[];
extern CONST INT32 FreqMult[];

static struct SdxcBoot gBoot;

INT32
BlockLen (
  IN  INT32 Len
  )
{
  struct SdCmd Cmd;

  Cmd.CmdIdx = EMMC_CMD_SET_BLOCKLEN;
  Cmd.RespType = MMC_RSP_R1;
  Cmd.CmdArg = Len;

  return SdxcSendCmd(gBoot.SdxcBase, gBoot.Clock, &Cmd, NULL);
}

static EFI_STATUS
AppCmd (
  IN BOOLEAN Rca
  )
{
  struct SdCmd Cmd;

  Cmd.CmdIdx = EMMC_CMD_APP_CMD;
  Cmd.RespType = MMC_RSP_R1;

  if (Rca)
    Cmd.CmdArg = gBoot.Rca << 16;
  else
    Cmd.CmdArg = 0;

  return SdxcSendCmd(gBoot.SdxcBase, gBoot.Clock, &Cmd, NULL);
}

static EFI_STATUS
GetStatus (
  IN  INT32 Timeout
  )
{
  struct SdCmd Cmd;
  INT32 Retries = 5;
  EFI_STATUS Status;

  Cmd.CmdIdx = EMMC_CMD_SEND_STATUS;
  Cmd.RespType = MMC_RSP_R1;
  Cmd.CmdArg = gBoot.Rca << 16;

  do {
    Status = SdxcSendCmd(gBoot.SdxcBase, gBoot.Clock, &Cmd, NULL);
    if (!Status) {
      if ((Cmd.Response[0] & MMC_STATUS_RDY_FOR_DATA) &&
          ((Cmd.Response[0] & MMC_STATUS_CURR_STATE) !=
           MMC_STATE_PRG))
        break;
      else if (Cmd.Response[0] & MMC_STATUS_MASK) {
        return EFI_DEVICE_ERROR;
      }
    } else if (--Retries < 0)
      return Status;

    MicroSecondDelay(1000);
  } while (Timeout--);

  if (Timeout <= 0) {
    DEBUG((EFI_D_ERROR, "Timeout Waiting Card Ready\n"));
    return EFI_TIMEOUT;
  }

  if (Cmd.Response[0] & MMC_STATUS_SWITCH_ERROR)
    return EFI_NO_RESPONSE;

  return EFI_SUCCESS;
}

static EFI_STATUS
SendIfCond  (
  VOID
  )
{
  struct SdCmd Cmd;
  EFI_STATUS Status;

  Cmd.CmdIdx = SD_CMD_SEND_IF_COND;
  /* We Set The Bit if The Host Supports Voltages Between 2.7 And 3.6 V */
  Cmd.CmdArg = ((gBoot.Voltages & 0xff8000) != 0) << 8 | 0xaa;
  Cmd.RespType = MMC_RSP_R7;

  Status = SdxcSendCmd(gBoot.SdxcBase, gBoot.Clock, &Cmd, NULL);

  if (Status)
    return Status;

  if ((Cmd.Response[0] & 0xff) != 0xaa)
    return EFI_NO_RESPONSE;
  else
    gBoot.Version = SD_VER_2;

  return EFI_SUCCESS;
}

static EFI_STATUS
OpCond (
  VOID
  )
{
  INT32 Timeout = 1000;
  EFI_STATUS Status;
  struct SdCmd Cmd;

  do {
    Status = AppCmd(FALSE);
    if (Status)
      return Status;

    Cmd.RespType = MMC_RSP_R3;
    Cmd.CmdIdx = SD_CMD_APP_SEND_OP_COND;
    Cmd.CmdArg = gBoot.Voltages & 0xff8000;

    if (gBoot.Version == SD_VER_2)
      Cmd.CmdArg |= OCR_HCS;

    Status = SdxcSendCmd(gBoot.SdxcBase, gBoot.Clock, &Cmd, NULL);
    if (Status)
      return Status;

    MicroSecondDelay(10000);
  } while (Timeout-- && (!(Cmd.Response[0] & OCR_BUSY)));

  if (Timeout <= 0)
    return EFI_TIMEOUT;

  gBoot.Ocr = Cmd.Response[0];
  gBoot.HighCapacity = ((gBoot.Ocr & OCR_HCS) == OCR_HCS);
  gBoot.Rca = 0;

  return EFI_SUCCESS;
}

EFI_STATUS
GoIdle (
  VOID
  )
{
  struct SdCmd Cmd;
  INT32 Status;

  MicroSecondDelay(1000);

  Cmd.CmdIdx = EMMC_CMD_GO_IDLE_STATE;
  Cmd.CmdArg = 0;
  Cmd.RespType = MMC_RSP_NONE;

  Status = SdxcSendCmd(gBoot.SdxcBase, gBoot.Clock, &Cmd, NULL);
  if (Status)
    return Status;

  MicroSecondDelay(2000);

  return EFI_SUCCESS;
}

VOID
SetClkRegs(
  VOID
  )
{
  INT32 Div = 0, PreDiv = 0, Clk = 0;

  /* Set The Clock Speed */
  if (gBoot.SdxcClk / 16 > gBoot.Clock) {
    for (PreDiv = 2; PreDiv < 256; PreDiv *= 2)
      if ((gBoot.SdxcClk / PreDiv) <= (gBoot.Clock * 16))
        break;
  } else
    PreDiv = 2;

  for (Div = 1; Div <= 16; Div++)
    if ((gBoot.SdxcClk / (Div * PreDiv)) <= gBoot.Clock)
      break;

  PreDiv >>= 1;
  Div -= 1;

  Clk = (PreDiv << 8) | (Div << 4);

  MmioClearBitsBe32((UINTN)&gBoot.SdxcBase->Sysctl, SYSCTL_CKEN);
  MmioClearSetBe32((UINTN)&gBoot.SdxcBase->Sysctl, SYSCTL_CLOCK_MASK, Clk);

  MicroSecondDelay(10000);

  Clk = SYSCTL_PEREN | SYSCTL_CKEN;
  MmioSetBitsBe32((UINTN)&gBoot.SdxcBase->Sysctl, Clk);

  /* Set The Bus Width */
  MmioClearBitsBe32((UINTN)&gBoot.SdxcBase->Proctl, PRCTL_DTW_4 | PRCTL_DTW_8);

  if (gBoot.BusWidth == 4)
    MmioSetBitsBe32((UINTN)&gBoot.SdxcBase->Proctl, PRCTL_DTW_4);
  else if (gBoot.BusWidth == 8)
    MmioSetBitsBe32((UINTN)&gBoot.SdxcBase->Proctl, PRCTL_DTW_8);
}

VOID
SetClock (
  IN  UINT32 Clock
  )
{
  if (gBoot.Clock > gBoot.FMax)
    gBoot.Clock = gBoot.FMax;
  if (gBoot.Clock < gBoot.FMin)
    gBoot.Clock = gBoot.FMin;

  SetClkRegs();
}

VOID
SetBusWidth (
  IN  UINT32 BusWidth
  )
{
  gBoot.BusWidth = BusWidth;

  SetClkRegs();
}

static EFI_STATUS
CompleteInit (
  VOID
  )
{
  INT32   Status;
  UINT32 Mult, Freq;
  struct SdCmd Cmd;

  INT32 Timeout = 10000;

  /* Put The Card In Identify Mode */
  Cmd.RespType = MMC_RSP_R2;
  Cmd.CmdArg = 0;
  Cmd.CmdIdx = EMMC_CMD_ALL_SEND_CID;

  Status = SdxcSendCmd(gBoot.SdxcBase, gBoot.Clock, &Cmd, NULL);

  if (Status)
    return Status;

  gBoot.Cid[0] = Cmd.Response[0];
  gBoot.Cid[1] = Cmd.Response[1];
  gBoot.Cid[2] = Cmd.Response[2];
  gBoot.Cid[3] = Cmd.Response[3];
  /*
   * Set Relative Address for MMC cards
   * Get Relatvie Address for SD cards
   * It will put Card Into Standby State
  */
  Cmd.CmdIdx = SD_CMD_SEND_RELATIVE_ADDR;
  Cmd.RespType = MMC_RSP_R6;
  Cmd.CmdArg = gBoot.Rca << 16;

  Status = SdxcSendCmd(gBoot.SdxcBase, gBoot.Clock, &Cmd, NULL);

  if (Status)
    return Status;

  gBoot.Rca = (Cmd.Response[0] >> 16) & 0xffff;

  /* Get The Card-Specific Data */
  Cmd.CmdIdx = EMMC_CMD_SEND_CSD;
  Cmd.RespType = MMC_RSP_R2;
  Cmd.CmdArg = gBoot.Rca << 16;

  Status = SdxcSendCmd(gBoot.SdxcBase, gBoot.Clock, &Cmd, NULL);

  /* Wait for Ready Status */
  GetStatus(Timeout);

  if (Status)
    return Status;

  gBoot.Csd[0] = Cmd.Response[0];
  gBoot.Csd[1] = Cmd.Response[1];
  gBoot.Csd[2] = Cmd.Response[2];
  gBoot.Csd[3] = Cmd.Response[3];

  if (gBoot.Version == MMC_VER_UNKNOWN) {
    INT32 Version = (Cmd.Response[0] >> 26) & 0xf;

    switch (Version) {
      case 0:
        gBoot.Version = MMC_VER_1_2;
        break;
      case 1:
        gBoot.Version = MMC_VER_1_4;
        break;
      case 2:
        gBoot.Version = MMC_VER_2_2;
        break;
      case 3:
        gBoot.Version = MMC_VER_3;
        break;
      case 4:
        gBoot.Version = MMC_VER_4;
        break;
      default:
        gBoot.Version = MMC_VER_1_2;
        break;
    }
  }

  /* Divide Frequency By 10, Since The Mults Are 10x Bigger */
  Freq = FreqBase[(Cmd.Response[0] & 0x7)];
  Mult = FreqMult[((Cmd.Response[0] >> 3) & 0xf)];

  gBoot.TranSpeed = Freq * Mult;

  gBoot.DsrImp = ((Cmd.Response[1] >> 12) & 0x1);
  gBoot.ReadBlkLen = 1 << ((Cmd.Response[1] >> 16) & 0xf);

  if (gBoot.ReadBlkLen > MMC_MAX_BLOCK_LEN)
    gBoot.ReadBlkLen = MMC_MAX_BLOCK_LEN;

  if ((gBoot.DsrImp) && (0xffffffff != gBoot.Dsr)) {
    Cmd.CmdIdx = EMMC_CMD_SET_DSR;
    Cmd.CmdArg = (gBoot.Dsr & 0xffff) << 16;
    Cmd.RespType = MMC_RSP_NONE;
    if (SdxcSendCmd(gBoot.SdxcBase, gBoot.Clock, &Cmd, NULL))
      DEBUG((EFI_D_ERROR, "MMC: SET_DSR Failed\n"));
  }

  /* Select Card And Put Into Transfer Mode */
  Cmd.CmdArg = gBoot.Rca << 16;
  Cmd.CmdIdx = EMMC_CMD_SELECT_CARD;
  Cmd.RespType = MMC_RSP_R1;

  Status = SdxcSendCmd(gBoot.SdxcBase, gBoot.Clock, &Cmd, NULL);

  if (Status)
    return Status;

  /* Restrict Card'S Capabilities By What The Host Can do */
  gBoot.CardCaps &= gBoot.HostCaps;
  gBoot.CardCaps |= MMC_MODE_HS;
  gBoot.CardCaps |= (MMC_MODE_4_BIT | MMC_MODE_8_BIT);

  if (gBoot.CardCaps & MMC_MODE_4_BIT) {
    Status = AppCmd(TRUE);
    if (Status)
      return Status;

    Cmd.CmdIdx = SD_CMD_APP_SET_BUS_WIDTH;
    Cmd.RespType = MMC_RSP_R1;
    Cmd.CmdArg = 2;
    Status = SdxcSendCmd(gBoot.SdxcBase, gBoot.Clock, &Cmd, NULL);
    if (Status)
      return Status;

    SetBusWidth(4);
  }

  if (gBoot.CardCaps & MMC_MODE_HS)
    gBoot.TranSpeed = 50000000;
  else
    gBoot.TranSpeed = 25000000;

  SetClock(gBoot.TranSpeed);

  /* Fill In Device Description */
  gBoot.Lba = LBA;

  return EFI_SUCCESS;
}

EFI_STATUS
SdxcBootInit (
  IN  BOOLEAN Init
  )
{
  struct SysInfo SocSysInfo;
  EFI_STATUS Status = EFI_SUCCESS;
  INT32 Timeout = 1000;
  UINT32 Caps = 0, VoltageCaps = 0;

  gBoot.SdxcBase = (VOID *)SDXC_BASE_ADDR;
  GetSysInfo(&SocSysInfo);
 
  gBoot.SdxcClk = (UINT32)SocSysInfo.FreqSdhc;

  SdxcReset(gBoot.SdxcBase);

  Caps = MmioReadBe32((UINTN)&gBoot.SdxcBase->Hostcapblt);
  Caps = Caps | SDXC_HOSTCAPBLT_VS33;

  if (Caps & SDXC_HOSTCAPBLT_VS30)
    VoltageCaps |= MMC_VDD_29_30 | MMC_VDD_30_31;
  if (Caps & SDXC_HOSTCAPBLT_VS33)
    VoltageCaps |= MMC_VDD_32_33 | MMC_VDD_33_34;
  if (Caps & SDXC_HOSTCAPBLT_VS18)
    VoltageCaps |= MMC_VDD_165_195;

  gBoot.Voltages = MMC_VDD_32_33 | MMC_VDD_33_34;
  if ((gBoot.Voltages & VoltageCaps) == 0) {
    DEBUG((EFI_D_ERROR, "Voltage Not Supported By Controller\n"));
    return EFI_DEVICE_ERROR;
  }

  gBoot.HostCaps = MMC_MODE_4_BIT | MMC_MODE_8_BIT | MMC_MODE_HC;
  if (Caps & SDXC_HOSTCAPBLT_HSS)
    gBoot.HostCaps |= MMC_MODE_HS_52MHz | MMC_MODE_HS;

  gBoot.FMin = 400000;
  gBoot.FMax = Min((UINT32)SocSysInfo.FreqSdhc, 52000000);
  gBoot.BMax = CONFIG_SYS_MMC_MAX_BLK_COUNT;

  /* Setup Dsr Related Values */
  gBoot.DsrImp = 0;
  gBoot.Dsr = 0xffffffff;

  while (!(MmioReadBe32((UINTN)&gBoot.SdxcBase->Prsstat) & PRSSTATE_CINS) && --Timeout)
    MicroSecondDelay(1000);

  if (Timeout <= 0) {
    DEBUG((EFI_D_ERROR, "Card not present\n"));
    return EFI_NO_MEDIA;
  }

  Timeout = 1000;
  
  MmioSetBitsBe32((UINTN)&gBoot.SdxcBase->Sysctl, SYSCTL_RSTA);
  while ((MmioReadBe32((UINTN)&gBoot.SdxcBase->Sysctl) & SYSCTL_RSTA) && --Timeout)
    MicroSecondDelay(1000);

  MmioWriteBe32((UINTN)&gBoot.SdxcBase->Scr, 0x00000040);

  SetClock(400000);

  MmioClearBitsBe32((UINTN)&gBoot.SdxcBase->Irqstaten, IRQSTATE_EN_BRR | IRQSTATE_EN_BWR);
  MmioWriteBe32((UINTN)&gBoot.SdxcBase->Proctl, PRCTL_INIT);
  MmioClearSetBe32((UINTN)&gBoot.SdxcBase->Sysctl, SYSCTL_TIMEOUT_MASK, 14 << 16);

  SetBusWidth(1);
  SetClock(1);

  MmioClearBitsBe32((UINTN)&gBoot.SdxcBase->Proctl, PRCTL_BE);

  Status = GoIdle();
  if (Status)
    return Status;

  /* Test for SD Version 2 */
  Status = SendIfCond();
  if (Status != EFI_NO_RESPONSE)
    DEBUG((EFI_D_INFO, "Not SD version 2\n"));

  Status = OpCond();
  if (Status) {
    DEBUG((EFI_D_ERROR, "Card Did Not Respond To Voltage Select!\n"));
    return Status;
  }

  Status = CompleteInit();
  if (Status)
    return Status;
  
  DEBUG((EFI_D_INFO, "Name: %c%c%c%c%c \n", gBoot.Cid[0] & 0xff,
		(gBoot.Cid[1] >> 24), (gBoot.Cid[1] >> 16) & 0xff,
		(gBoot.Cid[1] >> 8) & 0xff, gBoot.Cid[1] & 0xff));

  DEBUG((EFI_D_INFO, "Tran Speed: %d\n", gBoot.TranSpeed));
  DEBUG((EFI_D_INFO, "Rd Block Len: %d\n", gBoot.ReadBlkLen));

  DEBUG((EFI_D_INFO, "High Capacity: %a\n", gBoot.HighCapacity ? "Yes" : "No"));
  DEBUG((EFI_D_INFO, "Bus Width: %d-Bit\n", gBoot.BusWidth));

  return Status;
}

INT32
ReadBlks (
  OUT VOID *Dest,
  IN  UINT64 Start,
  IN  UINT64 Blkcnt
  )
{
  struct SdCmd Cmd;
  struct SdData Data;

  DEBUG((EFI_D_INFO, "Start 0x%x, Blkcnt 0x%x \n", Start, Blkcnt));

  if (Blkcnt > 1)
    Cmd.CmdIdx = EMMC_CMD_READ_MULTIPLE_BLOCK;
  else
    Cmd.CmdIdx = EMMC_CMD_READ_SINGLE_BLOCK;

  if (gBoot.HighCapacity)
    Cmd.CmdArg = Start;
  else
    Cmd.CmdArg = Start * gBoot.ReadBlkLen;

  Cmd.RespType = MMC_RSP_R1;

  Data.Dest = Dest;
  Data.Blocks = Blkcnt;
  Data.Blocksize = gBoot.ReadBlkLen;
  Data.Flags = MMC_DATA_READ;

  if (SdxcSendCmd(gBoot.SdxcBase, gBoot.Clock, &Cmd, &Data))
    return 0;

  if (Blkcnt > 1) {
    Cmd.CmdIdx = EMMC_CMD_STOP_TRANSMISSION;
    Cmd.CmdArg = 0;
    Cmd.RespType = MMC_RSP_R1b;
    if (SdxcSendCmd(gBoot.SdxcBase, gBoot.Clock, &Cmd, NULL)) {
      DEBUG((EFI_D_ERROR, "Failed To Send Stop Cmd\n"));
      return 0;
    }
  }

  return Blkcnt;
}

EFI_STATUS
SdxcBootRead (
  OUT  VOID * InAddr,
  IN  UINT64 StartBlk,
  IN  UINT64 Size
  )
{
  UINT32 Count = 0, BlocksTodo = 0;
  UINT64 CurBlk = 0;

  DEBUG((EFI_D_INFO, "MMC Read: Block # %d, size %d...\n", StartBlk, Size));

  if (Size % gBoot.ReadBlkLen)
    Count = Size/gBoot.ReadBlkLen + 1;
  else
    Count = Size/gBoot.ReadBlkLen;

  if (Count == 0) {
    DEBUG((EFI_D_ERROR, "Block Count is invalid \n"));
    return EFI_INVALID_PARAMETER;
  }

  if ((StartBlk + Count) > gBoot.Lba) {
    DEBUG((EFI_D_ERROR, "Block Number 0x%x Exceeds Max(0x%x)\n",
           StartBlk + Count, gBoot.Lba));
    return EFI_INVALID_PARAMETER;
  }

  BlocksTodo = Count;

  /* Wait for Ready Status */
  GetStatus(100000);

  if (BlockLen(gBoot.ReadBlkLen))
    return EFI_DEVICE_ERROR;

  do {
    CurBlk = (BlocksTodo > gBoot.BMax) ?
           gBoot.BMax : BlocksTodo;
    if (ReadBlks(InAddr, StartBlk, CurBlk) != CurBlk)
      return EFI_DEVICE_ERROR;

    BlocksTodo -= CurBlk;
    StartBlk += CurBlk;
    InAddr += CurBlk * gBoot.ReadBlkLen;
  } while (BlocksTodo > 0);

  return EFI_SUCCESS;
}

VOID
UpdateBootStruct (
  IN VOID
  )
{
  CreateBootStruct(&gBoot);
}
