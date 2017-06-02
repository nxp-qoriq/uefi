/** @DdrCtrlRegs.c

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include <Library/FslDdr.h>

UINT32
PicosToMclk (
  IN   UINT32    Picos
  )
{
  UINT64 Clks, ClksRem;
  UINT64 DataRate = GetDdrFreq();

  if (!Picos)
    return 0;

  Clks = Picos * (UINT64)DataRate;
  ClksRem = DoDiv(Clks, UL_5POW12);
  ClksRem += (Clks & (UL_2POW13-1)) * UL_5POW12;
  Clks >>= 13;

  if (ClksRem > DataRate) {
    Clks++;
  }

  if (Clks > ULL_8FS) {
    Clks = ULL_8FS;
  }

  return (UINT32) Clks;
}


UINT32
DdrGetCtrlVersion (
   VOID
  )
{
  struct CcsrDdr  *Ddr;
  UINT32 VerMajorMinorErrata;

  Ddr = (VOID *)DDR_CTRL1_ADDR;
  VerMajorMinorErrata = (DdrRead32(&Ddr->IpRev1) & 0xFFFF) << 8;
  VerMajorMinorErrata |= (DdrRead32(&Ddr->IpRev2) & 0xFF00) >> 8;

  return VerMajorMinorErrata;
}

VOID
SetDdrCdr1 (
  OUT  DdrCfgRegsT             *Ddr,
  IN   CONST MemctlOptionsT    *Popts
  )
{
  Ddr->DdrCdr1 = Popts->DdrCdr1;
  DEBUG((EFI_D_INFO, "FSLDDR: DdrCdr1 = 0x%08x\n", Ddr->DdrCdr1));
}


VOID
SetDdrCdr2 (
  OUT  DdrCfgRegsT             *Ddr,
  IN   CONST MemctlOptionsT    *Popts
  )
{
  Ddr->DdrCdr2 = Popts->DdrCdr2;
  DEBUG((EFI_D_INFO, "FSLDDR: DdrCdr2 = 0x%08x\n", Ddr->DdrCdr2));
}


UINT32
CheckFslMemctlConfigRegs (
  IN   CONST DdrCfgRegsT    *Ddr
  )
{
  UINT32 Res = 0;

  if (Ddr->DdrSdramCfg & 0x10000000 && Ddr->DdrSdramCfg & 0x00008000) {
    DEBUG((EFI_D_ERROR,"Error: DDR_SDRAM_CFG[RD_EN] and DDR_SDRAM_CFG[2T_EN] "
                      " should not be set at the same time.\n"));
    Res++;
  }

  return Res;
}

#ifdef MEMORY_TYPE_DDR4
/**
   compute CAS write latency according to DDR4 spec
   CWL = 9 for <= 1600MT/s
        10 for <= 1866MT/s
        11 for <= 2133MT/s
        12 for <= 2400MT/s
        14 for <= 2667MT/s
        16 for <= 2933MT/s
        18 for higher
 **/
STATIC inline UINT32
ComputeCasWriteLatency(
        VOID
  )
{
  UINT32 Cwl;
  CONST UINT32 MclkPs = GetMemoryClkPeriodPs();

  if (MclkPs >= 1250)
    Cwl = 9;
  else if (MclkPs >= 1070)
    Cwl = 10;
  else if (MclkPs >= 935)
    Cwl = 11;
  else if (MclkPs >= 833)
    Cwl = 12;
  else if (MclkPs >= 750)
    Cwl = 14;
  else if (MclkPs >= 681)
    Cwl = 16;
  else
    Cwl = 18;

  return Cwl;
}
#else
STATIC inline UINT32
ComputeCasWriteLatency ( // for DDR3
  VOID
  )
{
  UINT32 Cwl;
  CONST UINT32 MclkPs = GetMemoryClkPeriodPs();

  if (MclkPs >= 2500) {
    Cwl = 5;
  } else if (MclkPs >= 1875) {
    Cwl = 6;
  } else if (MclkPs >= 1500) {
    Cwl = 7;
  } else if (MclkPs >= 1250) {
    Cwl = 8;
  } else if (MclkPs >= 1070) {
    Cwl = 9;
  } else if (MclkPs >= 935) {
    Cwl = 10;
  } else if (MclkPs >= 833) {
    Cwl = 11;
  } else if (MclkPs >= 750) {
    Cwl = 12;
  } else {
    Cwl = 12;
  }
  return Cwl;
}
#endif

inline INT32
AvoidOdtOverlap (
  IN   CONST DimmParamsT   *DimmParams)
{
#if DDR_DIMM_SLOTS_PER_CTLR == 1
  if (DimmParams[0].NRanks == 4)
    return 2;
#endif

#if DDR_DIMM_SLOTS_PER_CTLR == 2
  if ((DimmParams[0].NRanks == 2) && (DimmParams[1].NRanks == 2)) {
    return 2;
  }

#ifdef DDR_FIRST_SLOT_QUAD_CAPABLE
  if (DimmParams[0].NRanks == 4) {
    return 2;
  }
#endif

  if ((DimmParams[0].NRanks != 0) && (DimmParams[2].NRanks != 0)) {
      return 1;
  }
#endif
  return 0;
}


VOID
SetTimingCfg0 (
  OUT  DdrCfgRegsT              *Ddr,
  IN   CONST MemctlOptionsT     *Popts,
  IN   CONST DimmParamsT        *DimmParams
  )
{
  UINT8 TrwtMclk = 0;   // Read-to-write turnaround
  UINT8 TwrtMclk = 0;   // Write-to-read turnaround
  UINT8 TrrtMclk = 0;   // Read-to-read turnaround
  UINT8 TwwtMclk = 0;   // Write-to-write turnaround
  UINT8 ActPdExitMclk;  // Active powerdown exit timing (tXARD and tXARDS)
  UINT8 PrePdExitMclk;  // Precharge powerdown exit timing (tXP)
  UINT8 TaxpdMclk = 0;  // ODT powerdown exit timing (tAXPD)
  UINT8 TmrdMclk;       // Mode register set cycle time (tMRD)
  CONST UINT32 MclkPs = GetMemoryClkPeriodPs();

#if defined(MEMORY_TYPE_DDR4)
  // TXP=Max(4nCK, 6ns)
  INT32 Txp = Max((INT32)MclkPs * 4, 6000); // 6000ps=6ns
  UINT64 DataRate = GetDdrFreq();

  TrwtMclk = (DataRate/1000000 > 1900) ? 3 : 2;

  /*
   * for single quad-rank DIMM and two-slot DIMMs
   */
  switch (AvoidOdtOverlap(DimmParams)) {
  case 2:
	TwrtMclk = 2;
	TwwtMclk = 2;
	TrrtMclk = 2;
	break;
  default:
	TwrtMclk = 1;
	TwwtMclk = 1;
	TrrtMclk = 0;
	break;
  }

  ActPdExitMclk = PicosToMclk(Txp);
  PrePdExitMclk = ActPdExitMclk;
  // MRS_CYC = Max(tMRD, tMOD)
  // tMRD = 8nCK, TMOD = Max(24nCK, 15ns)
  TmrdMclk = Max(24U, PicosToMclk(15000));
#elif defined(MEMORY_TYPE_DDR3)
  UINT64 DataRate = GetDdrFreq();
  INT32 Txp;
  UINT32 IpRev;
  INT32 OdtOverlap;

  Txp = Max(((INT32)MclkPs * 3), (MclkPs > 1540 ? 7500 : 6000));
  IpRev = DdrGetCtrlVersion();
  if (IpRev >= 0x40700) {
    TmrdMclk = Max((UINT32)12, PicosToMclk(15000));
  } else {
     if (Popts->RegisteredDimmEn) {
         TmrdMclk = 8;
     } else {
        // tMRD = 4nCK for UDIMM
        TmrdMclk = 4;
     }
  }

  // set turnaround time
  /*
   * for single quad-rank DIMM and two-slot DIMMs
  */
  OdtOverlap = AvoidOdtOverlap(DimmParams);
  switch (OdtOverlap) {
    case 2:
      TwwtMclk = 2;
      TrrtMclk = 1;
      break;
    case 1:
      TwwtMclk = 1;
      TrrtMclk = 0;
      break;
    default:
      break;
  }

  TrwtMclk = (DataRate/1000000 > 1800) ? 2 : 1;

  if ((DataRate/1000000 > 1150) || (Popts->MemctlInterleaving))
    TwrtMclk = 1;

  if (Popts->DynamicPower == 0) {
    ActPdExitMclk = 1;
    PrePdExitMclk = 1;
    TaxpdMclk = 1;
  } else { // powerdown used
    ActPdExitMclk = PicosToMclk(Txp);
    PrePdExitMclk = ActPdExitMclk;
    TaxpdMclk = 1;
  }
#endif

  if (Popts->TrwtOverride)
    TrwtMclk = Popts->Trwt;

  DEBUG((EFI_D_INFO, "Popts->TrwtOverride %d \n", Popts->TrwtOverride));
  DEBUG((EFI_D_INFO, "FSLDDR: TwrtMclk 0x%08x, TwrtMclk 0x%08x, TrrtMclk 0x%08x, TwwtMclk 0x%08x, ActPdExitMclk 0x%08x, PrePdExitMclk 0x%08x, TaxpdMclk 0x%08x, TmrdMclk 0x%08x\n",
  TwrtMclk, TwrtMclk , TrrtMclk , TwwtMclk , ActPdExitMclk , PrePdExitMclk , TaxpdMclk , TmrdMclk));
  Ddr->TimingCfg0 = (0
        | ((TrwtMclk & 0x3) << 30)        // RWT
        | ((TwrtMclk & 0x3) << 28)        // WRT
        | ((TrrtMclk & 0x3) << 26)        // RRT
        | ((TwwtMclk & 0x3) << 24)        // WWT
        | ((ActPdExitMclk & 0xf) << 20)   // ACT_PD_EXIT
        | ((PrePdExitMclk & 0xF) << 16)   // PRE_PD_EXIT
        | ((TaxpdMclk & 0xf) << 8)        // ODT_PD_EXIT
        | ((TmrdMclk & 0x1f) << 0)        // MRS_CYC
  );
  DEBUG((EFI_D_INFO, "FSLDDR: timing_cfg_0 = 0x%08x\n", Ddr->TimingCfg0));
}


VOID
SetTimingCfg3 (
  OUT  DdrCfgRegsT                    *Ddr,
  IN   CONST MemctlOptionsT           *Popts,
  IN   CONST CommonTimingParamsT      *CommonDimm,
  IN   UINT32                         CasLatency,
  IN   UINT32                         AdditiveLatency
  )
{
  UINT32 ExtPretoact = 0; // Extended precharge to activate interval (tRP)
  UINT32 ExtActtopre = 0; // Extended Activate to precharge interval (tRAS)
  UINT32 ExtActtorw = 0;  // Extended activate to read/write interval (tRCD)
  UINT32 ExtRefrec;       // Extended refresh recovery time (tRFC)
  UINT32 ExtCaslat = 0;   // Extended MCAS latency from READ cmd
  UINT32 ExtAddLat = 0;   // Extended additive latency
  UINT32 ExtWrrec = 0;    // Extended last data to precharge interval (tWR)
  UINT32 CntlAdj = 0;     // Control Adjust

  ExtPretoact = PicosToMclk(CommonDimm->TrpPs) >> 4;
  ExtActtopre = PicosToMclk(CommonDimm->TrasPs) >> 4;
  ExtActtorw = PicosToMclk(CommonDimm->TrcdPs) >> 4;
  ExtCaslat = (2 * CasLatency - 1) >> 4;
  ExtAddLat = AdditiveLatency >> 4;
#if defined(MEMORY_TYPE_DDR4)
  ExtRefrec = (PicosToMclk(CommonDimm->Trfc1Ps) - 8) >> 4;
#else
  ExtRefrec = (PicosToMclk(CommonDimm->TrfcPs) - 8) >> 4;
#endif
  ExtWrrec = (PicosToMclk(CommonDimm->TwrPs) +
          (Popts->OtfBurstChopEn ? 2 : 0)) >> 4;

  Ddr->TimingCfg3 = (0
        | ((ExtPretoact & 0x1) << 28)
        | ((ExtActtopre & 0x3) << 24)
        | ((ExtActtorw & 0x1) << 22)
        | ((ExtRefrec & 0x1F) << 16)
        | ((ExtCaslat & 0x3) << 12)
        | ((ExtAddLat & 0x1) << 10)
        | ((ExtWrrec & 0x1) << 8)
        | ((CntlAdj & 0x7) << 0));
  DEBUG((EFI_D_INFO,"FSLDDR: TimingCfg3 = 0x%08x\n", Ddr->TimingCfg3));
}


VOID
SetTimingCfg1(
  OUT  DdrCfgRegsT                  *Ddr,
  IN   CONST MemctlOptionsT         *Popts,
  IN   CONST CommonTimingParamsT    *CommonDimm,
  IN   UINT32                       CasLatency
  )
{
  UINT8 PretoactMclk;  // Precharge-to-activate interval (tRP)
  UINT8 ActtopreMclk;  // Activate to precharge interval (tRAS)
  UINT8 ActtorwMclk;   // Activate to read/write interval (tRCD)
  UINT8 CaslatCtrl;    // Cas latency
  UINT8 RefrecCtrl;    // Refresh recovery time (tRFC)
  UINT8 WrrecMclk;     // Last data to precharge minimum interval (tWR)
  UINT8 ActtoactMclk;  // Activate-to-activate interval (tRRD)
  UINT8 WrtordMclk;    // Last write data to read command issue interval (tWTR)
#ifdef MEMORY_TYPE_DDR4
  STATIC CONST UINT8 WrrecTable[] = {
        10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
        12, 12, 14, 14, 16, 16, 18, 18, 20, 20,
        24, 24, 24, 24};
#else
  CONST UINT8 WrrecTable[] = {
          1, 2, 3, 4, 5, 6, 7, 8, 10, 10, 12, 12, 14, 14, 0, 0};
#endif
  PretoactMclk = PicosToMclk(CommonDimm->TrpPs);
  ActtopreMclk = PicosToMclk(CommonDimm->TrasPs);
  ActtorwMclk = PicosToMclk(CommonDimm->TrcdPs);

  if (DdrGetCtrlVersion() <= 0x40400) {
    CaslatCtrl = 2 * CasLatency - 1;
  } else {
    CaslatCtrl = (CasLatency - 1) << 1;
  }
  DEBUG((EFI_D_INFO,"SetTimingCfg1: CaslatCtrl = 0x%x, version=0x%x\n",
         CaslatCtrl, DdrGetCtrlVersion()));

#ifdef MEMORY_TYPE_DDR4
  RefrecCtrl = PicosToMclk(CommonDimm->Trfc1Ps) - 8;
  WrrecMclk = PicosToMclk(CommonDimm->TwrPs);
  ActtoactMclk = Max(PicosToMclk(CommonDimm->TrrdPs), 4U);
  WrtordMclk = Max(2U, PicosToMclk(2500));
  if (!((WrrecMclk < 1) || (WrrecMclk > 24))) {
    WrrecMclk = WrrecTable[WrrecMclk - 1];
  }
#else
  RefrecCtrl = PicosToMclk(CommonDimm->TrfcPs) - 8;
  WrrecMclk = PicosToMclk(CommonDimm->TwrPs);
  ActtoactMclk = PicosToMclk(CommonDimm->TrrdPs);
  WrtordMclk = PicosToMclk(CommonDimm->TwtrPs);
  if (!((WrrecMclk < 1) || (WrrecMclk > 16))) {
    WrrecMclk = WrrecTable[WrrecMclk - 1];
  }
#endif


  if (Popts->OtfBurstChopEn) {
    WrrecMclk += 2;
  }

#if defined(MEMORY_TYPE_DDR3)
  if (ActtoactMclk < 4) {
    ActtoactMclk = 4;
  }
#endif
#if defined(MEMORY_TYPE_DDR3)
  if (WrtordMclk < 4) {
    WrtordMclk = 4;
  }
#endif

  if (Popts->OtfBurstChopEn) {
    WrtordMclk += 2;
  }

  Ddr->TimingCfg1 = ((PretoactMclk & 0x0F) << 28)
                  | ((ActtopreMclk & 0x0F) << 24)
                  | ((ActtorwMclk & 0xF) << 20)
                  | ((CaslatCtrl & 0xF) << 16)
                  | ((RefrecCtrl & 0xF) << 12)
                  | ((WrrecMclk & 0x0F) << 8)
                  | ((ActtoactMclk & 0x0F) << 4)
                  | ((WrtordMclk & 0x0F) << 0);
  DEBUG((EFI_D_INFO,"FSLDDR: TimingCfg1 = 0x%08x\n", Ddr->TimingCfg1));
}


VOID
SetTimingCfg2 (
  OUT  DdrCfgRegsT                   *Ddr,
  IN   CONST MemctlOptionsT          *Popts,
  IN   CONST CommonTimingParamsT     *CommonDimm,
  IN   UINT32                        CasLatency,
  IN   UINT32                        AdditiveLatency
  )
{
  UINT8 AddLatMclk; // Additive latency
  UINT16 Cpo;       // CAS-to-preamble override
  UINT8 WrLat;      // Write latency
  UINT8 RdToPre;    // Read to precharge (tRTP)
  UINT8 WrDataDelay;// Write command to write data strobe timing adjustment
  UINT8 CkePls;     // Minimum CKE pulse width (tCKE)
  UINT16 FourAct;   // Window for four activates (tFAW)

#ifdef MEMORY_TYPE_DDR3
  CONST UINT32 MclkPs = GetMemoryClkPeriodPs();
#endif

  AddLatMclk = AdditiveLatency;
  Cpo = Popts->CpoOverride;
  WrLat = ComputeCasWriteLatency();

#ifdef MEMORY_TYPE_DDR4
  RdToPre = PicosToMclk(7500);
#else
  RdToPre = PicosToMclk(CommonDimm->TrtpPs);
#endif

  if (RdToPre < 4) {
    RdToPre = 4;
  }

  if (Popts->OtfBurstChopEn) {
    RdToPre += 2;
  }

  WrDataDelay = Popts->WriteDataDelay;

#ifdef MEMORY_TYPE_DDR4
  Cpo = 0;
  CkePls = Max(3U, PicosToMclk(5000));
#elif defined(MEMORY_TYPE_DDR3)
  CkePls = Max(3U, PicosToMclk(MclkPs > 1870 ? 7500 :
                   (MclkPs > 1245 ? 5625 : 5000)));
#else
  CkePls = DDR_MIN_TCKE_PULSE_WIDTH_DDR;
#endif
  FourAct = PicosToMclk(Popts->TfawWindowFourActivatesPs);

  Ddr->TimingCfg2 = (0
        | ((AddLatMclk & 0xf) << 28)
        | ((Cpo & 0x1f) << 23)
        | ((WrLat & 0xf) << 19)
        | ((WrLat & 0x10) << 18)
        | ((RdToPre & RD_TO_PRE_MASK) << RD_TO_PRE_SHIFT)
        | ((WrDataDelay & WR_DATA_DELAY_MASK) << WR_DATA_DELAY_SHIFT)
        | ((CkePls & 0x7) << 6)
        | ((FourAct & 0x3f) << 0));
  DEBUG((EFI_D_INFO,"FSLDDR: TimingCfg2 = 0x%08x\n", Ddr->TimingCfg2));
}


VOID
SetDdrEor (
  OUT  DdrCfgRegsT           *Ddr,
  IN   CONST MemctlOptionsT     *Popts
  )
{
  if (Popts->AddrHash) {
    Ddr->DdrEor = 0x40000000; // enable address hash
  }
}


VOID
SetCsnConfig (
  IN   INT32                   DimmNumber,
  IN   INT32                   I,
  OUT  DdrCfgRegsT             *Ddr,
  IN   CONST MemctlOptionsT    *Popts,
  IN   CONST DimmParamsT       *DimmParams
  )
{
  UINT32 CsNEn = 0;      // Chip Select enable
  UINT32 IntlvEn = 0;    // Memory controller interleave enable
  UINT32 IntlvCtl = 0;   // Interleaving control
  UINT32 ApNEn = 0;      // Chip select n auto-precharge enable
  UINT32 OdtRdCfg = 0;   // ODT for reads Configuration
  UINT32 OdtWrCfg = 0;   // ODT for writes Configuration
  UINT32 BaBitsCsN = 0;  // Num of bank bits for SDRAM on CSn
  UINT32 RowBitsCsN = 0; // Num of row bits for SDRAM on CSn
  UINT32 ColBitsCsN = 0; // Num of ocl bits for SDRAM on CSn
  INT32 GoConfig = 0;
#ifdef MEMORY_TYPE_DDR4
  UINT32 BgBitsCsN = 0;  // Num of bank group bits
#else
  UINT32 NBanksPerSdramDevice;
#endif

  switch (I) {
    case 0:
      if (DimmParams[DimmNumber].NRanks > 0) {
        GoConfig = 1;
        if (!Popts->MemctlInterleaving) {
           break;
        }
        switch (Popts->MemctlInterleavingMode) {
           case DDR_256B_INTERLEAVING:
           case DDR_CACHE_LINE_INTERLEAVING:
           case DDR_PAGE_INTERLEAVING:
           case DDR_BANK_INTERLEAVING:
           case DDR_SUPERBANK_INTERLEAVING:
             IntlvEn = Popts->MemctlInterleaving;
             IntlvCtl = Popts->MemctlInterleavingMode;
             break;
           default:
             break;
        }
      }
      break;
    case 1:
      if ((DimmNumber == 0 && DimmParams[0].NRanks > 1) ||
            (DimmNumber == 1 && DimmParams[1].NRanks > 0)) {
        GoConfig = 1;
      }
      break;
    case 2:
      if ((DimmNumber == 0 && DimmParams[0].NRanks > 2) ||
          (DimmNumber >= 1 && DimmParams[DimmNumber].NRanks > 0)) {
        GoConfig = 1;
      }
      break;
    case 3:
      if ((DimmNumber == 0 && DimmParams[0].NRanks > 3) ||
          (DimmNumber == 1 && DimmParams[1].NRanks > 1) ||
          (DimmNumber == 3 && DimmParams[3].NRanks > 0)) {
        GoConfig = 1;
      }
      break;
    default:
      break;
  }

  if (GoConfig) {
    CsNEn = 1;
    ApNEn = Popts->CsLocalOpts[I].AutoPrecharge;
    OdtRdCfg = Popts->CsLocalOpts[I].OdtRdCfg;
    OdtWrCfg = Popts->CsLocalOpts[I].OdtWrCfg;
#ifdef MEMORY_TYPE_DDR4
    BaBitsCsN = DimmParams[DimmNumber].BankAddrBits;
    BgBitsCsN = DimmParams[DimmNumber].BankGroupBits;
#else
    NBanksPerSdramDevice
        = DimmParams[DimmNumber].NBanksPerSdramDevice;
    BaBitsCsN = __ILog2(NBanksPerSdramDevice) - 2;
#endif
    RowBitsCsN = DimmParams[DimmNumber].NRowAddr - 12;
    ColBitsCsN = DimmParams[DimmNumber].NColAddr - 8;
  }
  Ddr->Cs[I].Config = ((CsNEn & 0x1) << 31)
                    | ((IntlvEn & 0x3) << 29)
                    | ((IntlvCtl & 0xf) << 24)
                    | ((ApNEn & 0x1) << 23)
                    | ((OdtRdCfg & 0x7) << 20)
                    | ((OdtWrCfg & 0x7) << 16)
                    | ((BaBitsCsN & 0x3) << 14)
                    | ((RowBitsCsN & 0x7) << 8)
#ifdef MEMORY_TYPE_DDR4
                    | ((BgBitsCsN & 0x3) << 4)
#endif
                    | ((ColBitsCsN & 0x7) << 0);
  DEBUG((EFI_D_INFO, "FSLDDR: Cs[%d]Config = 0x%08x\n", I,Ddr->Cs[I].Config));
}


VOID
SetCsnConfig2 (
  IN   INT32          Num,
  OUT  DdrCfgRegsT    *Ddr
  )
{
  UINT32 PasrCfg = 0;  // Partial array self refresh Config

  Ddr->Cs[Num].Config2 = ((PasrCfg & 7) << 24);
  DEBUG((EFI_D_INFO, "FSLDDR: Cs[%d]Config2 = 0x%08x\n", Num, Ddr->Cs[Num].Config2));
}

// DDR SDRAM Register Control Word
VOID
SetDdrSdramRcw (
  OUT  DdrCfgRegsT                 *Ddr,
  IN   CONST MemctlOptionsT           *Popts,
  IN   CONST CommonTimingParamsT      *CommonDimm
  )
{
  if (CommonDimm->AllDimmsRegistered && !CommonDimm->AllDimmsUnbuffered) {
    if (Popts->RcwOverride) {
      Ddr->DdrSdramRcw1 = Popts->Rcw1;
      Ddr->DdrSdramRcw2 = Popts->Rcw2;
    } else {
      Ddr->DdrSdramRcw1 =
                   CommonDimm->Rcw[0] << 28 | \
                   CommonDimm->Rcw[1] << 24 | \
                   CommonDimm->Rcw[2] << 20 | \
                   CommonDimm->Rcw[3] << 16 | \
                   CommonDimm->Rcw[4] << 12 | \
                   CommonDimm->Rcw[5] << 8  | \
                   CommonDimm->Rcw[6] << 4  | \
                   CommonDimm->Rcw[7];
      Ddr->DdrSdramRcw2 =
                   CommonDimm->Rcw[8] << 28  | \
                   CommonDimm->Rcw[9] << 24  | \
                   CommonDimm->Rcw[10] << 20 | \
                   CommonDimm->Rcw[11] << 16 | \
                   CommonDimm->Rcw[12] << 12 | \
                   CommonDimm->Rcw[13] << 8  | \
                   CommonDimm->Rcw[14] << 4  | \
                   CommonDimm->Rcw[15];
     }
     DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramRcw1 = 0x%08x\n", Ddr->DdrSdramRcw1));
     DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramRcw2 = 0x%08x\n", Ddr->DdrSdramRcw2));
  }
}


// DDR SDRAM Control Configuration (DDR_SDRAM_CFG)
VOID
SetDdrSdramCfg (
  OUT  DdrCfgRegsT                *Ddr,
  IN   CONST MemctlOptionsT          *Popts,
  IN   CONST CommonTimingParamsT     *CommonDimm
  )
{
  UINT32 MemEn;               // Ddr SDRAM interface logic enable
  UINT32 Sren;                // Self refresh enable (during sleep)
  UINT32 EccEn;               // ECC enable
  UINT32 RdEn;                // Registered DIMM enable
  UINT32 SdramType;           // Type of SDRAM
  UINT32 DynPwr;              // Dynamic power management Mode
  UINT32 Dbw;                 // DRAM dta bus width
  UINT32 EightBe = 0;         // 8-beat burst enable, DDR2 is zero
  UINT32 Ncap = 0;            // Non-concurrent auto-precharge
  UINT32 ThreetEn;            // Enable 3T timing
  UINT32 TwotEn;              // Enable 2T timing
  UINT32 BaIntlvCtl;          // Bank (CS) interleaving control
  UINT32 X32En = 0;           // x32 enable
  UINT32 Pchb8 = 0;           // precharge bit 8 enable
  UINT32 Hse;                 // Global half strength override
  UINT32 MemHalt = 0;         // memory controller halt
  UINT32 Bi = 0;              // Bypass initialization

  MemEn = 1;
  Sren = Popts->SelfRefreshInSleep;
  if (CommonDimm->AllDimmsEccCapable) {
    // Allow setting of ECC only if all DIMMs are ECC
    EccEn = Popts->EccMode;
  } else {
    EccEn = 0;
  }

  if (CommonDimm->AllDimmsRegistered && !CommonDimm->AllDimmsUnbuffered) {
    RdEn = 1;
    TwotEn = 0;
  } else {
    RdEn = 0;
    TwotEn = Popts->TwotEn;
  }

  SdramType = CONFIG_FSL_SDRAM_TYPE;

  DynPwr = Popts->DynamicPower;
  Dbw = Popts->DataBusWidth;
  /**
    8-beat burst enable Ddr-III case
    we must clear it when use the on-the-fly Mode,
    must set it when use the 32-bits bus Mode.
   */
  if (SdramType == SDRAM_TYPE_DDR3) {
    if (Popts->BurstLength == DDR_BL8) {
      EightBe = 1;
    }
    if (Popts->BurstLength == DDR_OTF) {
      EightBe = 0;
    }
    if (Dbw == 0x1) {
      EightBe = 1;
    }
  }

  ThreetEn = Popts->ThreetEn;
  BaIntlvCtl = Popts->BaIntlvCtl;
  Hse = Popts->HalfStrengthDriverEnable;

  Ddr->DdrSdramCfg = (0
                  | ((MemEn & 0x1) << 31)
                  | ((Sren & 0x1) << 30)
                  | ((EccEn & 0x1) << 29)
                  | ((RdEn & 0x1) << 28)
                  | ((SdramType & 0x7) << 24)
                  | ((DynPwr & 0x1) << 21)
                  | ((Dbw & 0x3) << 19)
                  | ((EightBe & 0x1) << 18)
                  | ((Ncap & 0x1) << 17)
                  | ((ThreetEn & 0x1) << 16)
                  | ((TwotEn & 0x1) << 15)
                  | ((BaIntlvCtl & 0x7F) << 8)
                  | ((X32En & 0x1) << 5)
                  | ((Pchb8 & 0x1) << 4)
                  | ((Hse & 0x1) << 3)
                  | ((MemHalt & 0x1) << 1)
                  | ((Bi & 0x1) << 0));
  DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramCfg = 0x%08x\n", Ddr->DdrSdramCfg));
}


VOID
SetDdrSdramCfg2 (
  OUT  DdrCfgRegsT             *Ddr,
  IN   CONST MemctlOptionsT       *Popts,
  IN   CONST UINT32               UnqMrsEn
  )
{
  UINT32 FrcSr = 0;     // Force self refresh
  UINT32 SrIe = 0;      // Self-refresh interrupt enable
  UINT32 OdtCfg = 0;    // ODT Configuration
  UINT32 NumPr;         // Number of posted refreshes
  UINT64 Slow = 0;      // Ddr will be run less than 1250
  UINT32 X4En = 0;      // x4 DRAM enable
  UINT32 ObcCfg;        // On-The-Fly Burst Chop Cfg
  UINT32 ApEn;          // Address Parity Enable
  UINT32 DInit;         // DRAM data initialization
  UINT32 RcwEn = 0;     // Register Control Word Enable
  UINT32 MdEn = 0;      // Mirrored DIMM Enable
  UINT32 QdEn = 0;      // quad-rank DIMM Enable
  INT32 I;

#ifndef MEMORY_TYPE_DDR4
  UINT32 DllRstDis = 1; // DLL Reset Disable
  UINT32 DqsCfg = Popts->DqsConfig;
#endif

  for (I = 0; I < DDR_CHIP_SELECTS_PER_CTRL; I++) {
    if (Popts->CsLocalOpts[I].OdtRdCfg
              || Popts->CsLocalOpts[I].OdtWrCfg) {
      OdtCfg = SDRAM_CFG2_ODT_ONLY_READ;
      break;
    }
  }

  NumPr = 1;
  ObcCfg = Popts->OtfBurstChopEn;
  Slow = GetDdrFreq() < 1249000000;

  if (Popts->RegisteredDimmEn) {
    RcwEn = 1;
    ApEn = Popts->ApEn;
  } else {
    ApEn = 0;
  }

  X4En = Popts->X4En ? 1 : 0;

#if defined(DDR_ECC_INIT_VIA_CONTROLLER)
  // use the DDR controller to auto initialize memory
  DInit = Popts->EccInitUsingMemctl;
  Ddr->DdrDataInit = CONFIG_MEM_INIT_VALUE;
#else
  // Memory will be initialized via DMA, or not at all
  DInit = 0;
#endif

  MdEn = Popts->MirroredDimm;
  QdEn = Popts->QuadRankPresent ? 1 : 0;
  Ddr->DdrSdramCfg2 = (0
          | ((FrcSr & 0x1) << 31)
          | ((SrIe & 0x1) << 30)
#ifndef MEMORY_TYPE_DDR4
          | ((DllRstDis & 0x1) << 29)
          | ((DqsCfg & 0x3) << 26)
#endif
          | ((OdtCfg & 0x3) << 21)
          | ((NumPr & 0xf) << 12)
          | ((Slow & 1) << 11)
          | (X4En << 10)
          | (QdEn << 9)
          | (UnqMrsEn << 8)
          | ((ObcCfg & 0x1) << 6)
          | ((ApEn & 0x1) << 5)
          | ((DInit & 0x1) << 4)
          | ((RcwEn & 0x1) << 2)
          | ((MdEn & 0x1) << 0));
  DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramCfg2 = 0x%08x\n", Ddr->DdrSdramCfg2));
}


#ifdef MEMORY_TYPE_DDR4
// DDR SDRAM Mode Configuration 2 (DDR_SDRAM_MODE_2)
VOID
SetDdrSdramMode2 (
  OUT  DdrCfgRegsT                      *Ddr,
  IN   CONST MemctlOptionsT             *Popts,
  IN   CONST CommonTimingParamsT        *CommonDimm,
  IN   CONST UINT32                     UnqMrsEn
  )
{
  UINT16 ESdmode2 = 0;  // Extended SDRAM Mode 2
  UINT16 ESdmode3 = 0;  // Extended SDRAM Mode 3

  INT32 I;
  UINT32 WrCrc = 0;     // Disable
  UINT32 RttWr = 0;     // Rtt_WR - Dynamic ODT Off
  UINT32 Srt = 0;       // Self-Refresh Temerature, Normal Range
  UINT32 Cwl = ComputeCasWriteLatency() - 9;
  UINT32 Mpr = 0;       // Serial
  UINT32 WcLat, Tmp;
  CONST UINT32 MclkPs = GetMemoryClkPeriodPs();

  if (Popts->RttOverride)
    RttWr = Popts->RttWrOverrideValue;
  else
    RttWr = Popts->CsLocalOpts[0].OdtRttWr;

  if (CommonDimm->ExtendedOpSrt)
    Srt = CommonDimm->ExtendedOpSrt;

  ESdmode2 = ((WrCrc & 0x1) << 12)
           | ((RttWr & 0x3) << 9)
           | ((Srt & 0x3) << 6)
           | ((Cwl & 0x7) << 3);

  if (MclkPs >= 1250) {
    WcLat = 0;
  } else if (MclkPs >= 833) {
    WcLat = 1;
  } else {
    WcLat = 2;
  }

  ESdmode3 = ((Mpr & 0x3) << 11) | ((WcLat & 0x3) << 9);
  Ddr->DdrSdramMode2 = ((ESdmode2 & 0xFFFF) << 16) | ((ESdmode3 & 0xFFFF) << 0);
  DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramMode2 = 0x%08x\n", Ddr->DdrSdramMode2));

  if (UnqMrsEn) { // unique mode registers are supported
    for (I = 1; I < DDR_CHIP_SELECTS_PER_CTRL; I++) {
      if (Popts->RttOverride) {
        RttWr = Popts->RttWrOverrideValue;
      } else {
        RttWr = Popts->CsLocalOpts[I].OdtRttWr;
      }

      ESdmode2 &= 0xF9FF;  // clear bit 10, 9
      ESdmode2 |= (RttWr & 0x3) << 9;
      Tmp = ((ESdmode2 & 0xFFFF) << 16) | ((ESdmode3 & 0xFFFF) << 0);

      switch (I) {
         case 1:
           Ddr->DdrSdramMode4 = Tmp;
           DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramMode4 = 0x%08x\n", Tmp));
           break;
         case 2:
           Ddr->DdrSdramMode6 = Tmp;
           DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramMode6 = 0x%08x\n", Tmp));
           break;
         case 3:
           Ddr->DdrSdramMode8 = Tmp;
           DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramMode8 = 0x%08x\n", Tmp));
           break;
      }
    }
  }
}
#elif defined(MEMORY_TYPE_DDR3)
// DDR SDRAM Mode Configuration 2 (DDR_SDRAM_MODE_2)
VOID
SetDdrSdramMode2 (
  OUT  DdrCfgRegsT                 *Ddr,
  IN   CONST MemctlOptionsT           *Popts,
  IN   CONST CommonTimingParamsT      *CommonDimm,
  IN   CONST UINT32                   UnqMrsEn
  )
{
  UINT16 ESdmode2 = 0;  // Extended SDRAM Mode 2
  UINT16 ESdmode3 = 0;  // Extended SDRAM Mode 3

  INT32 I;
  UINT32 RttWr = 0;     // Rtt_WR - Dynamic ODT Off
  UINT32 Srt = 0;       // Self-Refresh Temerature, Normal Range
  UINT32 Asr = 0;       // Auto Self-Refresh Disable
  UINT32 Cwl = ComputeCasWriteLatency() - 5;
  UINT32 Pasr = 0;      // partial array self refresh disable

  if (Popts->RttOverride)
    RttWr = Popts->RttWrOverrideValue;
  else
    RttWr = Popts->CsLocalOpts[0].OdtRttWr;

  if (CommonDimm->ExtendedOpSrt)
    Srt = CommonDimm->ExtendedOpSrt;

  ESdmode2 = ((RttWr & 0x3) << 9)
           | ((Srt & 0x1) << 7)
           | ((Asr & 0x1) << 6)
           | ((Cwl & 0x7) << 3)
           | ((Pasr & 0x7) << 0);

  Ddr->DdrSdramMode2 = ((ESdmode2 & 0xFFFF) << 16) | ((ESdmode3 & 0xFFFF) << 0);

  if (UnqMrsEn) {       // unique Mode registers are supported
    for (I = 1; I < DDR_CHIP_SELECTS_PER_CTRL; I++) {
      if (Popts->RttOverride)
        RttWr = Popts->RttWrOverrideValue;
      else
        RttWr = Popts->CsLocalOpts[I].OdtRttWr;

      ESdmode2 &= 0xF9FF; // clear bit 10, 9
      ESdmode2 |= (RttWr & 0x3) << 9;
      switch (I) {
         case 1:
           Ddr->DdrSdramMode4 = ((ESdmode2 & 0xFFFF) << 16) |
                                ((ESdmode3 & 0xFFFF) << 0);
           break;
         case 2:
           Ddr->DdrSdramMode6 = ((ESdmode2 & 0xFFFF) << 16) |
                                ((ESdmode3 & 0xFFFF) << 0);
           break;
         case 3:
           Ddr->DdrSdramMode8 = (0
                        | ((ESdmode2 & 0xFFFF) << 16)
                        | ((ESdmode3 & 0xFFFF) << 0)
           );
           break;
      }
    }
  }
}
#endif

#ifdef MEMORY_TYPE_DDR4
// DDR SDRAM Mode Configuration 9 (DDR_SDRAM_MODE_9)
VOID SetDdrSdramMode9(
  OUT  DdrCfgRegsT *Ddr,
  IN   CONST MemctlOptionsT *Popts,
  IN   CONST CommonTimingParamsT *CommonDimm,
  IN   CONST UINT32 UnqMrsEn
  )
{
  INT32 I;
  UINT16 Esdmode4 = 0;      // Extended SDRAM Mode 4
  UINT16 Esdmode5;          // Extended SDRAM Mode 5

  Esdmode5 = 0x00000500;    // Data Mask Enabled

  Ddr->DdrSdramMode9 = (0
        | ((Esdmode4 & 0xffff) << 16)
        | ((Esdmode5 & 0xffff) << 0));

  // Only Mode9 Use 0x500, Others Use 0x400
  Esdmode5 = 0x00000400;    // Data Mask Enabled

  DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramMode9 = 0x%08x\n", Ddr->DdrSdramMode9));
  if (UnqMrsEn) {        // unique Mode Registers are supported
    for (I = 1; I < DDR_CHIP_SELECTS_PER_CTRL; I++) {
      switch (I) {
         case 1:
           Ddr->DdrSdramMode11 = ((Esdmode4 & 0xFFFF) << 16) |
                                 ((Esdmode5 & 0xFFFF) << 0);
           break;
         case 2:
           Ddr->DdrSdramMode13 = ((Esdmode4 & 0xFFFF) << 16) |
                                 ((Esdmode5 & 0xFFFF) << 0);
           break;
         case 3:
           Ddr->DdrSdramMode15 = ((Esdmode4 & 0xFFFF) << 16) |
                                 ((Esdmode5 & 0xFFFF) << 0);
           break;
      }
    }
    DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramMode11 = 0x%08x\n",
                      Ddr->DdrSdramMode11));
    DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramMode13 = 0x%08x\n",
                      Ddr->DdrSdramMode13));
    DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramMode15 = 0x%08x\n",
                      Ddr->DdrSdramMode15));
  }
}

// DDR SDRAM Mode Configuration 10 (DDR_SDRAM_MODE_10)
VOID SetDdrSdramMode10 (
  OUT DdrCfgRegsT *Ddr,
  IN  CONST MemctlOptionsT *Popts,
  IN  CONST CommonTimingParamsT *CommonDimm,
  IN  CONST UINT32 UnqMrsEn
  )
{
  INT32 I;
  UINT16 Esdmode6 = 0;        // Extended SDRAM Mode 6
  UINT16 Esdmode7 = 0;        // Extended SDRAM Mode 7
  UINT32 Tmp, TccdlMin = PicosToMclk(CommonDimm->TccdlPs);

  Esdmode6 = ((TccdlMin - 4) & 0x7) << 10;

  if (Popts->DdrCdr2 & DDRCDR2_VREF_RANGE_2)
    Esdmode6 |= 1 << 6;	/* Range 2 */

  Tmp = ((Esdmode6 & 0xffff) << 16) | ((Esdmode7 & 0xffff) << 0);
  Ddr->DdrSdramMode10 = Tmp;

  DEBUG((EFI_D_INFO,"FSLDDR: TccdlMin 0x%08x, Popts->DdrCdr2 0x%08x, DDRCDR2_VREF_RANGE_2 0x%08x, Esdmode6 0x%08x, Esdmode7 0x%08x\n",
        TccdlMin, Popts->DdrCdr2, DDRCDR2_VREF_RANGE_2, Esdmode6, Esdmode7));
  DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramMode10 = 0x%08x\n", Tmp));

  if (UnqMrsEn) {        // Unique Mode Registers Are Supported
    for (I = 1; I < DDR_CHIP_SELECTS_PER_CTRL; I++) {
      switch (I) {
         case 1:
           Ddr->DdrSdramMode12 = Tmp;
           break;
         case 2:
           Ddr->DdrSdramMode14 = Tmp;
           break;
         case 3:
           Ddr->DdrSdramMode16 = Tmp;
           break;
         }
    }
    DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramMode12 = 0x%08x\n", Tmp));
    DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramMode14 = 0x%08x\n", Tmp));
    DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramMode16 = 0x%08x\n", Tmp));
  }
}
#endif

// DDR SDRAM Interval Configuration (DDR_SDRAM_INTERVAL)
VOID
SetDdrSdramInterval (
  OUT  DdrCfgRegsT                  *Ddr,
  IN   CONST MemctlOptionsT            *Popts,
  IN   CONST CommonTimingParamsT       *CommonDimm
  )
{
  UINT32 Refint;  // Refresh interval
  UINT32 Bstopre; // Precharge interval

  Refint = PicosToMclk(CommonDimm->RefreshRatePs);
  Bstopre = Popts->Bstopre;

  // Refint field used 0x3FFF in earlier controllers
  Ddr->DdrSdramInterval = ((Refint & 0xFFFF) << 16) | ((Bstopre & 0x3FFF) << 0);
  DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramInterval = 0x%08x\n",
         Ddr->DdrSdramInterval));
}


#ifdef MEMORY_TYPE_DDR4
// DDR SDRAM Mode Configuration Set (DDR_SDRAM_MODE)
VOID
SetDdrSdramMode (
  OUT  DdrCfgRegsT                  *Ddr,
  IN   CONST MemctlOptionsT            *Popts,
  IN   CONST CommonTimingParamsT       *CommonDimm,
  IN   UINT32                          CasLatency,
  IN   UINT32                          AdditiveLatency,
  IN   CONST UINT32                    UnqMrsEn
  )
{
  INT32 I = 0;
  UINT16 ESdmode;       // Extended SDRAM Mode
  UINT16 Sdmode;        // SDRAM Mode */

  // Mode Register - MR1
  UINT32 Qoff = 0;      // Output Buffer Enable 0=Yes, 1=No
  UINT32 TdqsEn = 0;    // TDQS Enable: 0=No, 1=Yes
  UINT32 Rtt;
  UINT32 WrlvlEn = 0;   // Write Level Enable: 0=No, 1=Yes
  UINT32 Al = 0;        // Posted CAS# Additive Latency (AL)
  UINT32 Dic = 0;       // Output Driver Impedance, 40ohm
  UINT32 DllEn = 1;     // DLL Enable,1=Enable(Normal),0=Disable(Test/Debug)

  // Mode Register - MR0
  UINT32 Wr = 0;        // Write Recovery
  UINT32 DllRst;        // DLL Reset
  UINT32 Mode;          // Normal=0 Or Test=1
  UINT32 Caslat = 4;    // CAS# Latency, default Set As 6 Cycles
  // BT: Burst Type (0=Nibble Sequential, 1=Interleaved)
  UINT32 Bt;
  UINT32 Bl;            // BL: Burst Length

  UINT32 WrMclk;
  // DDR4 Support WR 10, 12, 14, 16, 18, 20, 24
  STATIC CONST UINT8 WrTable[] = {0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6};

  // DDR4 Support CAS 9, 10, 11, 12, 13, 14, 15, 16, 18, 20, 22, 24
  STATIC CONST UINT8 CasLatencyTable[] = {
                0, 1, 2, 3, 4, 5, 6, 7, 8, 8,
                9, 9, 10, 10, 11, 11};

  if (Popts->RttOverride)
    Rtt = Popts->RttOverrideValue;
  else
    Rtt = Popts->CsLocalOpts[0].OdtRttNorm;

  if (AdditiveLatency == (CasLatency - 1))
    Al = 1;
  if (AdditiveLatency == (CasLatency - 2))
    Al = 2;

  if (Popts->QuadRankPresent)
    Dic = 1;        // Output Driver Impedance 240/7 Ohm

  /**
    The ESdmode Value Will Also Be Used for Writing
    MR1 During Write Leveling for DDR3, Although The
    Bits Specifically Related To The Write Leveling
    Scheme Will Be Handled Automatically By The Ddr
    Controller. So We Set The WrlvlEn = 0 Here.
   */
  ESdmode = (0
          | ((Qoff & 0x1) << 12)
          | ((TdqsEn & 0x1) << 11)
          | ((Rtt & 0x7) << 8)   // Rtt Field Is Split
          | ((WrlvlEn & 0x1) << 7)
          | ((Al & 0x3) << 3)
          | ((Dic & 0x3) << 1)   // DIC Field Is Split
          | ((DllEn & 0x1) << 0));

  /**
    DLL Control for Precharge PD
    0=Slow Exit DLL Off (TxpDLL)
    1=Fast Exit DLL On (Txp)
   **/
  WrMclk = PicosToMclk(CommonDimm->TwrPs);
  if (WrMclk >= 10 && WrMclk <= 24) {
    Wr = WrTable[WrMclk - 10];
  } else {
    DEBUG((EFI_D_INFO,"Error: unsupported write recovery "
          "for mode register WrMclk = %d\n", WrMclk));
  }

  DllRst = 0;   // Dll no reset
  Mode = 0;     // normal mode

  // look up table to get the Cas latency bits
  if (CasLatency >= 9 && CasLatency <= 24) {
    Caslat = CasLatencyTable[CasLatency - 9];
  } else {
    DEBUG((EFI_D_INFO,"unsupported Cas latency for mode register\n"));
  }
  Bt = 0; // nibble sequential

  switch (Popts->BurstLength) {
    case DDR_BL8:
      Bl = 0;
      break;
    case DDR_OTF:
      Bl = 1;
      break;
    case DDR_BC4:
      Bl = 2;
      break;
    default:
      DEBUG((EFI_D_INFO,"Error: invalid burst length of %d specified.",
               Popts->BurstLength));
      DEBUG((EFI_D_INFO,"Defaulting to on-the-fly BC4 or BL8 beats.\n"));
      Bl = 1;
      break;
  }

  Sdmode = ((Wr & 0x7) << 9)
         | ((DllRst & 0x1) << 8)
         | ((Mode & 0x1) << 7)
         | (((Caslat >> 1) & 0x7) << 4)
         | ((Bt & 0x1) << 3)
         | ((Caslat & 1) << 2)
         | ((Bl & 0x3) << 0);

  Ddr->DdrSdramMode = ((ESdmode & 0xFFFF) << 16) | ((Sdmode & 0xFFFF) << 0);
  DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramMode = 0x%08x\n", Ddr->DdrSdramMode));

  if (UnqMrsEn) { // unique mode registers are supported
    for (I = 1; I < DDR_CHIP_SELECTS_PER_CTRL; I++) {
      if (Popts->RttOverride) {
        Rtt = Popts->RttOverrideValue;
      } else {
        Rtt = Popts->CsLocalOpts[I].OdtRttNorm;
      }

      ESdmode &= 0xF8FF;  // clear bit 10,9,8 for Rtt
      ESdmode |= ((Rtt & 0x7) << 8);
      switch (I) {
         case 1:
           Ddr->DdrSdramMode3 = ((ESdmode & 0xFFFF) << 16)
                              | ((Sdmode & 0xFFFF) << 0);
           break;
         case 2:
           Ddr->DdrSdramMode5 = ((ESdmode & 0xFFFF) << 16)
                              | ((Sdmode & 0xFFFF) << 0);
           break;
         case 3:
           Ddr->DdrSdramMode7 = ((ESdmode & 0xFFFF) << 16)
                              | ((Sdmode & 0xFFFF) << 0);
           break;
      }
    }
    DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramMode3 = 0x%08x\n", Ddr->DdrSdramMode3));
    DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramMode5 = 0x%08x\n", Ddr->DdrSdramMode5));
    DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramMode7 = 0x%08x\n", Ddr->DdrSdramMode7));
  }
}
#elif defined(MEMORY_TYPE_DDR3)
// DDR SDRAM Mode Configuration Set (DDR_SDRAM_MODE)
VOID
SetDdrSdramMode (
  OUT  DdrCfgRegsT                   *Ddr,
  IN   CONST MemctlOptionsT          *Popts,
  IN   CONST CommonTimingParamsT     *CommonDimm,
  IN   UINT32                        CasLatency,
  IN   UINT32                        AdditiveLatency,
  IN   CONST UINT32                  UnqMrsEn
  )
{
  INT32 I;
  UINT16 ESdmode;  // Extended SDRAM mode
  UINT16 Sdmode;   // SDRAM mode

  // Mode Register - MR1
  UINT32 Qoff = 0;    // Output buffer enable 0=yes, 1=no
  UINT32 TdqsEn = 0;  // TDQS Enable: 0=no, 1=yes
  UINT32 Rtt;
  UINT32 WrlvlEn = 0; // Write level enable
  UINT32 Al = 0;      // Posted CAS# additive latency
  UINT32 Dic = 0;     // Output driver impedance, 40ohm
  UINT32 DllEn = 0;   // DLL Enable

  // Mode Register - MR0
  UINT32 DllOn;        // DLL control for precharge PD, 0=off, 1=on
  UINT32 Wr = 0;       // Write Recovery
  UINT32 DllRst;       // DLL Reset
  UINT32 Mode;         // Normal=0 or Test=1
  UINT32 Caslat = 4;   // CAS# latency, default set as 6 cycles
  UINT32 Bt;           // Burst Type (0=Nibble Sequential, 1=Interleaved)
  UINT32 Bl;           // Burst Length

  UINT32 WrMclk;
  CONST UINT8 WrTable[] = {1, 2, 3, 4, 5, 5, 6, 6, 7, 7, 0, 0};

  if (Popts->RttOverride) {
    Rtt = Popts->RttOverrideValue;
  } else {
    Rtt = Popts->CsLocalOpts[0].OdtRttNorm;
  }

  if (AdditiveLatency == (CasLatency - 1)) {
    Al = 1;
  }
  if (AdditiveLatency == (CasLatency - 2)) {
    Al = 2;
  }
  if (Popts->QuadRankPresent) {
    Dic = 1;
  }

  ESdmode = ((Qoff & 0x1) << 12)
          | ((TdqsEn & 0x1) << 11)
          | ((Rtt & 0x4) << 7)
          | ((WrlvlEn & 0x1) << 7)
          | ((Rtt & 0x2) << 5)
          | ((Dic & 0x2) << 4)
          | ((Al & 0x3) << 3)
          | ((Rtt & 0x1) << 2)
          | ((Dic & 0x1) << 1)
          | ((DllEn & 0x1) << 0);

  DllOn = 1;
  WrMclk = PicosToMclk(CommonDimm->TwrPs);
  if (WrMclk <= 16) {
    Wr = WrTable[WrMclk - 5];
  }

  DllRst = 0; // dll no reset
  Mode = 0;   // normal Mode

  // look up table to get the cas latency bits
  if (CasLatency >= 5 && CasLatency <= 16) {
    UINT8 CasLatencyTable[] = {
                0x2  // 5 clocks
                0x4  // 6 clocks
                0x6  // 7 clocks
                0x8  // 8 clocks
                0xa  // 9 clocks
                0xc  // 10 clocks
                0xe  // 11 clocks
                0x1  // 12 clocks
                0x3  // 13 clocks
                0x5  // 14 clocks
                0x7  // 15 clocks
                0x9  // 16 clocks
    };
    Caslat = CasLatencyTable[CasLatency - 5];
  }

  Bt = 0;

  switch (Popts->BurstLength) {
    case DDR_BL8:
      Bl = 0;
      break;
    case DDR_OTF:
      Bl = 1;
      break;
    case DDR_BC4:
      Bl = 2;
      break;
    default:
      DEBUG((EFI_D_INFO,"Error: invalid burst length of %u specified.",
                       Popts->BurstLength));
      DEBUG((EFI_D_INFO,"Defaulting to on-the-fly BC4 or BL8 beats.\n"));
      Bl = 1;
      break;
  }

  Sdmode = ((DllOn & 0x1) << 12)
         | ((Wr & 0x7) << 9)
         | ((DllRst & 0x1) << 8)
         | ((Mode & 0x1) << 7)
         | (((Caslat >> 1) & 0x7) << 4)
         | ((Bt & 0x1) << 3)
         | ((Caslat & 1) << 2)
         | ((Bl & 0x3) << 0);

  Ddr->DdrSdramMode = ((ESdmode & 0xFFFF) << 16)
                    | ((Sdmode & 0xFFFF) << 0);
  DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramMode = 0x%08x\n", Ddr->DdrSdramMode));

  if (UnqMrsEn) { // unique mode registers
    for (I = 1; I < DDR_CHIP_SELECTS_PER_CTRL; I++) {
      if (Popts->RttOverride) {
        Rtt = Popts->RttOverrideValue;
      } else {
           Rtt = Popts->CsLocalOpts[I].OdtRttNorm;
      }
      ESdmode &= 0xFDBB;  // clear bit 9,6,2
      ESdmode |= ((Rtt & 0x4) << 7)
               | ((Rtt & 0x2) << 5)
               | ((Rtt & 0x1) << 2);
      switch (I) {
         case 1:
           Ddr->DdrSdramMode3 = ((ESdmode & 0xFFFF) << 16)
                              | ((Sdmode & 0xFFFF) << 0);
           break;
         case 2:
           Ddr->DdrSdramMode5 = ((ESdmode & 0xFFFF) << 16)
                              | ((Sdmode & 0xFFFF) << 0);
           break;
         case 3:
           Ddr->DdrSdramMode7 = ((ESdmode & 0xFFFF) << 16)
                              | ((Sdmode & 0xFFFF) << 0);
           break;
      }
      DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramMode3 = 0x%08x\n", Ddr->DdrSdramMode3));
    }
  }
}
#endif

// DDR SDRAM Data Initialization (DDR_DATA_INIT)
VOID
SetDdrDataInit (
  OUT  DdrCfgRegsT   *Ddr
  )
{
  UINT32 InitValue;

  InitValue = 0xDEADBEEF;
  Ddr->DdrDataInit = InitValue;
}

//  DDR SDRAM Clock Control (DDR_SDRAM_CLK_CNTL)
VOID
SetDdrSdramClkCntl (
  OUT  DdrCfgRegsT            *Ddr,
  IN   CONST MemctlOptionsT   *Popts
  )
{
  UINT32 ClkAdjust;

  ClkAdjust = Popts->ClkAdjust;

  if (DdrGetCtrlVersion() >= 0x40701) {
    /* clk_adjust in 5-bits on T-series and LS-series */
    Ddr->DdrSdramClkCntl = (ClkAdjust & 0x1F) << 22;
  } else {
    /* clk_adjust in 4-bits on earlier MPC85xx and P-series */
    Ddr->DdrSdramClkCntl = (ClkAdjust & 0xF) << 23;
  }

  DEBUG((EFI_D_INFO," DdrGetCtrlVersion() 0x%x \n", DdrGetCtrlVersion()));
  DEBUG((EFI_D_INFO," Popts->ClkAdjust 0x%x \n", Popts->ClkAdjust));

  DEBUG((EFI_D_INFO,"FSLDDR: ClkCntl = 0x%08x\n", Ddr->DdrSdramClkCntl));
}

// DDR Initialization Address (DDR_INIT_ADDR)
VOID
SetDdrInitAddr (
  OUT  DdrCfgRegsT     *Ddr
  )
{
  UINT32 InitAddr = 0;  // Initialization address

  Ddr->DdrInitAddr = InitAddr;
}

// DDR Initialization Address (DDR_INIT_EXT_ADDR)
VOID
SetDdrInitExtAddr (
  OUT  DdrCfgRegsT       *Ddr
  )
{
  UINT32 Uia = 0;                // Use initialization address
  UINT32 InitExtaddr = 0;        // Initialization address

  Ddr->DdrInitExtAddr = ((Uia & 0x1) << 31) | (InitExtaddr & 0xF);
}

// DDR SDRAM Timing Configuration 4 (TIMING_CFG_4)
VOID
SetTimingCfg4 (
  OUT  DdrCfgRegsT           *Ddr,
  IN   CONST MemctlOptionsT     *Popts
  )
{
  UINT32 Rwt = 0; // Read-to-write turnaround for same CS
  UINT32 Wrt = 0; // Write-to-read turnaround for same CS
  UINT32 Rrt = 0; // Read-to-read turnaround for same CS
  UINT32 Wwt = 0; // Write-to-write turnaround for same CS
  UINT32 DllLock = 0; // Ddr SDRAM DLL Lock Time

  if (Popts->BurstLength == DDR_BL8) {
    Rrt = 0;    // BL/2 clocks
    Wwt = 0;    // BL/2 clocks
  } else {
    Rrt = 2;    // BL/2 + 2 clocks
    Wwt = 2;    // BL/2 + 2 clocks
  }
#ifdef MEMORY_TYPE_DDR4
  DllLock = 2;        // TDLLK = 1024 Clocks
#elif defined(MEMORY_TYPE_DDR3)
  DllLock = 1;        // TDLLK = 512 Clocks From Spec
#endif
  Ddr->TimingCfg4 = (0
               | ((Rwt & 0xf) << 28)
               | ((Wrt & 0xf) << 24)
               | ((Rrt & 0xf) << 20)
               | ((Wwt & 0xf) << 16)
               | (DllLock & 0x3));
  DEBUG((EFI_D_INFO,"FSLDDR: TimingCfg4 = 0x%08x\n", Ddr->TimingCfg4));
}

// DDR SDRAM Timing Configuration 5 (TIMING_CFG_5)
VOID
SetTimingCfg5 (
  OUT  DdrCfgRegsT         *Ddr,
  IN   UINT32                 CasLatency
  )
{
  UINT32 RodtOn = 0;    // Read to ODT on
  UINT32 RodtOff = 0;   // Read to ODT off
  UINT32 WodtOn = 0;    // Write to ODT on
  UINT32 WodtOff = 0;   // Write to ODT off

  UINT32 WrLat = ((Ddr->TimingCfg2 & 0x00780000) >> 19) +
                 ((Ddr->TimingCfg2 & 0x00040000) >> 14);

  if (CasLatency >= WrLat) {
    RodtOn = CasLatency - WrLat + 1;
  }

  RodtOff = 4;
  WodtOn = 1;
  WodtOff = 4;

  Ddr->TimingCfg5 = ((RodtOn & 0x1f) << 24)
                  | ((RodtOff & 0x7) << 20)
                  | ((WodtOn & 0x1f) << 12)
                  | ((WodtOff & 0x7) << 8);
  DEBUG((EFI_D_INFO,"FSLDDR: TimingCfg5 = 0x%08x\n", Ddr->TimingCfg5));
}

#ifdef MEMORY_TYPE_DDR4
STATIC VOID
SetTimingCfg6 (
  OUT  DdrCfgRegsT *Ddr
  )
{
  UINT32 HsCaslat = 0;
  UINT32 HsWrlat = 0;
  UINT32 HsWrrec = 0;
  UINT32 HsClkadj = 0;
  UINT32 HsWrlvlStart = 0;

  Ddr->TimingCfg6 = (0
                | ((HsCaslat & 0x1f) << 24)
                | ((HsWrlat & 0x1f) << 19)
                | ((HsWrrec & 0x1f) << 12)
                | ((HsClkadj & 0x1f) << 6)
                | ((HsWrlvlStart & 0x1f) << 0));
  DEBUG((EFI_D_INFO,"FSLDDR: TimingCfg6 = 0x%08x\n", Ddr->TimingCfg6));
}

STATIC VOID
SetTimingCfg7 (
  OUT  DdrCfgRegsT *Ddr,
  IN  CONST CommonTimingParamsT *CommonDimm
  )
{
  UINT32 Txpr, Tcksre, Tcksrx;
  UINT32 CkeRst, Cksre, Cksrx, ParLat, CsToCmd;

  Txpr = Max(5U, PicosToMclk(CommonDimm->Trfc1Ps + 10000));
  Tcksre = Max(5U, PicosToMclk(10000));
  Tcksrx = Max(5U, PicosToMclk(10000));
  ParLat = 0;
  CsToCmd = 0;

  if (Txpr <= 200)
    CkeRst = 0;
  else if (Txpr <= 256)
    CkeRst = 1;
  else if (Txpr <= 512)
    CkeRst = 2;
  else
    CkeRst = 3;

  if (Tcksre <= 19)
    Cksre = Tcksre - 5;
  else
    Cksre = 15;

  if (Tcksrx <= 19)
    Cksrx = Tcksrx - 5;
  else
    Cksrx = 15;

  Ddr->TimingCfg7 = (0
             | ((CkeRst & 0x3) << 28)
             | ((Cksre & 0xf) << 24)
             | ((Cksrx & 0xf) << 20)
             | ((ParLat & 0xf) << 16)
             | ((CsToCmd & 0xf) << 4)
       );
  DEBUG((EFI_D_INFO,"FSLDDR: TimingCfg7 = 0x%08x\n", Ddr->TimingCfg7));
}

STATIC VOID
SetTimingCfg8 (
  OUT  DdrCfgRegsT *Ddr,
  IN   CONST MemctlOptionsT *Popts,
  IN   CONST CommonTimingParamsT *CommonDimm,
  IN   UINT32 CasLatency
  )
{
  UINT32 RwtBg, WrtBg, RrtBg, WwtBg;
  UINT32 ActtoactBg, WrtordBg, PreAllRec;
  UINT32 Tccdl = PicosToMclk(CommonDimm->TccdlPs);
  UINT32 WrLat = ((Ddr->TimingCfg2 & 0x00780000) >> 19) +
                 ((Ddr->TimingCfg2 & 0x00040000) >> 14);

  RwtBg = CasLatency + 2 + 4 - WrLat;
  if (RwtBg < Tccdl)
    RwtBg = Tccdl - RwtBg;
  else
    RwtBg = 0;

  WrtBg = WrLat + 4 + 1 - CasLatency;
  if (WrtBg < Tccdl)
    WrtBg = Tccdl - WrtBg;
  else
    WrtBg = 0;

  if (Popts->BurstLength == DDR_BL8) {
    RrtBg = Tccdl - 4;
    WwtBg = Tccdl - 4;
  } else {
    RrtBg = Tccdl - 2;
    WwtBg = Tccdl - 2;
  }

  ActtoactBg = PicosToMclk(CommonDimm->TrrdlPs);
  WrtordBg = Max(4U, PicosToMclk(7500));
  if (Popts->OtfBurstChopEn)
    WrtordBg += 2;

  PreAllRec = 0;

  Ddr->TimingCfg8 = (0
             | ((RwtBg & 0xf) << 28)
             | ((WrtBg & 0xf) << 24)
             | ((RrtBg & 0xf) << 20)
             | ((WwtBg & 0xf) << 16)
             | ((ActtoactBg & 0xf) << 12)
             | ((WrtordBg & 0xf) << 8)
             | ((PreAllRec & 0x1f) << 0));
  DEBUG((EFI_D_INFO,"FSLDDR: TimingCfg8 = 0x%08x\n", Ddr->TimingCfg8));
}

STATIC VOID
SetTimingCfg9 (
  OUT  DdrCfgRegsT *Ddr
  )
{
  Ddr->TimingCfg9 = 0;
  DEBUG((EFI_D_INFO,"FSLDDR: TimingCfg9 = 0x%08x\n", Ddr->TimingCfg9));
}

STATIC VOID
SetDdrDqMapping (
  OUT  DdrCfgRegsT *Ddr,
  IN   CONST DimmParamsT *DimmParams
  )
{
#ifndef INTPSR_DDR4_WORKAROUND
  UINT32 AccEccEn = (Ddr->DdrSdramCfg >> 2) & 0x1;

  Ddr->DqMap0 = ((DimmParams->DqMapping[0] & 0x3F) << 26) |
                        ((DimmParams->DqMapping[1] & 0x3F) << 20) |
                        ((DimmParams->DqMapping[2] & 0x3F) << 14) |
                        ((DimmParams->DqMapping[3] & 0x3F) << 8) |
                        ((DimmParams->DqMapping[4] & 0x3F) << 2);

  Ddr->DqMap1 = ((DimmParams->DqMapping[5] & 0x3F) << 26) |
                        ((DimmParams->DqMapping[6] & 0x3F) << 20) |
                        ((DimmParams->DqMapping[7] & 0x3F) << 14) |
                        ((DimmParams->DqMapping[10] & 0x3F) << 8) |
                        ((DimmParams->DqMapping[11] & 0x3F) << 2);

  Ddr->DqMap2 = ((DimmParams->DqMapping[12] & 0x3F) << 26) |
                        ((DimmParams->DqMapping[13] & 0x3F) << 20) |
                        ((DimmParams->DqMapping[14] & 0x3F) << 14) |
                        ((DimmParams->DqMapping[15] & 0x3F) << 8) |
                        ((DimmParams->DqMapping[16] & 0x3F) << 2);

  Ddr->DqMap3 = ((DimmParams->DqMapping[17] & 0x3F) << 26) |
                        ((DimmParams->DqMapping[8] & 0x3F) << 20) |
                        (AccEccEn ? 0 :
                         (DimmParams->DqMapping[9] & 0x3F) << 14) |
                        DimmParams->DqMappingOrs;
#endif
        DEBUG((EFI_D_INFO,"FSLDDR: DqMap0 = 0x%08x\n", Ddr->DqMap0));
        DEBUG((EFI_D_INFO,"FSLDDR: DqMap1 = 0x%08x\n", Ddr->DqMap1));
        DEBUG((EFI_D_INFO,"FSLDDR: DqMap2 = 0x%08x\n", Ddr->DqMap2));
        DEBUG((EFI_D_INFO,"FSLDDR: DqMap3 = 0x%08x\n", Ddr->DqMap3));
}

STATIC VOID
SetDdrSdramCfg3 (
  OUT  DdrCfgRegsT *Ddr,
  IN   CONST MemctlOptionsT *Popts
  )
{
  INT32 RdPre;

  RdPre = Popts->QuadRankPresent ? 1 : 0;

  Ddr->DdrSdramCfg3 = (RdPre & 0x1) << 16;

  DEBUG((EFI_D_INFO,"FSLDDR: DdrSdramCfg3 = 0x%08x\n", Ddr->DdrSdramCfg3));
}
#endif  /* MEMORY_TYPE_DDR4 */


// DDR ZQ Calibration Control
VOID
SetDdrZqCntl (
  OUT  DdrCfgRegsT      *Ddr,
  IN   UINT32           ZqEn
  )
{
  UINT32 Zqinit = 0; // POR ZQ Calibration Time (tZQinit)
  UINT32 Zqoper = 0; // Normal Operation Full Calibration Time (tZQoper)
  UINT32 Zqcs = 0;   // Normal Operation Short Calibration Time (tZQCS)
#ifdef MEMORY_TYPE_DDR4
  UINT32 ZqcsInit = 0;
#endif

  if (ZqEn) {
#ifdef MEMORY_TYPE_DDR4
    Zqinit = 10;       // 1024 Clocks
    Zqoper = 9;        // 512 Clocks
    Zqcs = 7;          // 128 Clocks
    ZqcsInit = 5;      // 1024 Refresh Sequences
#else
    Zqinit = 9;        // 512 Clocks
    Zqoper = 8;        // 256 Clocks
    Zqcs = 6;          // 64 Clocks
#endif
  }

  Ddr->DdrZqCntl = (0
             | ((ZqEn & 0x1) << 31)
             | ((Zqinit & 0xF) << 24)
             | ((Zqoper & 0xF) << 16)
             | ((Zqcs & 0xF) << 8)
#ifdef MEMORY_TYPE_DDR4
             | ((ZqcsInit & 0xF) << 0)
#endif
  );
  DEBUG((EFI_D_INFO,"FSLDDR: ZqCntl = 0x%08x\n", Ddr->DdrZqCntl));
}

// DDR Write Leveling Control (DDR_WRLVL_CNTL)
VOID
SetDdrWrlvlCntl (
  OUT  DdrCfgRegsT               *Ddr,
  IN   UINT32                       WrlvlEn,
  IN   CONST MemctlOptionsT         *Popts
  )
{
  // First DQS pulse rising edge after margining Mode is programmed (tWL_MRD)
  UINT32 WrlvlMrd = 0;
  // ODT delay after margining Mode is programmed (tWL_ODTEN)
  UINT32 WrlvlOdten = 0;
  // DQS/DQS_ delay after margining Mode is programmed (tWL_DQSEN)
  UINT32 WrlvlDqsen = 0;
  // WRLVL_SMPL: Write leveling sample time
  UINT32 WrlvlSmpl = 0;
  // WRLVL_WLR: Write leveling repeition time
  UINT32 WrlvlWlr = 0;
  // WRLVL_START: Write leveling start time
  UINT32 WrlvlStart = 0;

  // suggest enable write leveling for DDR3 due to fly-by topology
  if (WrlvlEn) {
    // tWL_MRD min = 40 nCK, we set it 64
    WrlvlMrd = 0x6;
    // tWL_ODTEN 128
    WrlvlOdten = 0x7;
    // tWL_DQSEN min = 25 nCK, we set it 32
    WrlvlDqsen = 0x5;
    /**
      Write leveling sample time at least need 6 clocks
      higher than tWLO to allow enough time for progagation
      delay and sampling the prime data bits.
    **/
    WrlvlSmpl = 0xf;
    /**
      Write leveling repetition time
      at least tWLO + 6 clocks clocks
      we set it 64
    **/
    WrlvlWlr = 0x6;
    /**
      Write leveling start time
      The value use for the DQS_ADJUST for the first sample
      when write leveling is enabled. It probably needs to be
      overriden per platform.
    **/
    WrlvlStart = 0x8;
    /**
      Override the write leveling sample and start time
      according to specific board
    **/
    if (Popts->WrlvlOverride) {
      WrlvlSmpl = Popts->WrlvlSample;
      WrlvlStart = Popts->WrlvlStart;
    }
  }

  DEBUG((EFI_D_INFO,"Popts->WrlvlOverride %d \n", Popts->WrlvlOverride));
  DEBUG((EFI_D_INFO,"WrlvlEn 0x%08x, WrlvlMrd 0x%08x, WrlvlOdten 0x%08x, WrlvlDqsen 0x%08x, WrlvlSmpl 0x%08x, WrlvlWlr 0x%08x, WrlvlStart 0x%08x\n",
         WrlvlEn , WrlvlMrd , WrlvlOdten , WrlvlDqsen , WrlvlSmpl , WrlvlWlr , WrlvlStart));

  Ddr->DdrWrlvlCntl = (0
         | ((WrlvlEn & 0x1) << 31)
         | ((WrlvlMrd & 0x7) << 24)
         | ((WrlvlOdten & 0x7) << 20)
         | ((WrlvlDqsen & 0x7) << 16)
         | ((WrlvlSmpl & 0xf) << 12)
         | ((WrlvlWlr & 0x7) << 8)
         | ((WrlvlStart & 0x1F) << 0));
  DEBUG((EFI_D_INFO,"FSLDDR: WrlvlCntl = 0x%08x\n", Ddr->DdrWrlvlCntl));
  Ddr->DdrWrlvlCntl2 = Popts->WrlvlCtl2;
  DEBUG((EFI_D_INFO,"FSLDDR: WrlvlCntl2 = 0x%08x\n", Ddr->DdrWrlvlCntl2));
  Ddr->DdrWrlvlCntl3 = Popts->WrlvlCtl3;
  DEBUG((EFI_D_INFO,"FSLDDR: WrlvlCntl3 = 0x%08x\n", Ddr->DdrWrlvlCntl3));
}

// DDR Self Refresh Counter (DDR_SR_CNTR)
VOID
SetDdrSrCntr (
  OUT  DdrCfgRegsT *Ddr,
  IN   UINT32 SrIt
  )
{
  // self refresh idle threshold
  Ddr->DdrSrCntr = (SrIt & 0xF) << 16;
}

UINT32
ComputeFslMemctlConfigRegs (
  IN   CONST MemctlOptionsT         *Popts,
  OUT  DdrCfgRegsT               *Ddr,
  IN   CONST CommonTimingParamsT    *CommonDimm,
  IN   CONST DimmParamsT            *DimmParams,
  IN   UINT32                       DbwCapAdj
  )
{
  UINT32 I, CasLatency, AdditiveLatency;
  UINT32 SrIt, ZqEn, WrlvlEn;
  UINT32 IpRev = 0, UnqMrsEn = 0;
  INT32 CsEn = 1;

  InternalMemZeroMem(Ddr, sizeof(DdrCfgRegsT));

  if (!CommonDimm) {
    DEBUG((EFI_D_ERROR,"Error: subset DIMM params struct null pointer\n"));
    return 1;
  }

  CasLatency = (Popts->CasLatencyOverride) ? Popts->CasLatencyOverrideValue
                : CommonDimm->LowestCommonSpdCaslat;

  AdditiveLatency = (Popts->AdditiveLatencyOverride)
          ? Popts->AdditiveLatencyOverrideValue : CommonDimm->AdditiveLatency;

  SrIt = (Popts->AutoSelfRefreshEn) ? Popts->SrIt : 0;
  ZqEn = (Popts->ZqEn) ? 1 : 0;       // ZQ calibration enable
  WrlvlEn = (Popts->WrlvlEn) ? 1 : 0; // write leveling enable

  for (I = 0; I < DDR_CHIP_SELECTS_PER_CTRL; I++) {
    UINT64 StartingAddress, EndingAddress;
    UINT32 CsPerDimm
        = DDR_CHIP_SELECTS_PER_CTRL / DDR_DIMM_SLOTS_PER_CTLR;
    UINT32 DimmNumber = I / CsPerDimm;
    UINT64 RankDensity = DimmParams[DimmNumber].RankDensity >> DbwCapAdj;

    if (DimmParams[DimmNumber].NRanks == 0) {
      continue;
    }
    if (Popts->MemctlInterleaving) { // controllers interleaving
      switch (Popts->BaIntlvCtl & DDR_CS0_CS1_CS2_CS3) {
         case DDR_CS0_CS1_CS2_CS3:
           break;
         case DDR_CS0_CS1:
         case DDR_CS0_CS1_AND_CS2_CS3:
           if (I > 1)
             CsEn = 0;
             break;
         case DDR_CS2_CS3:
         default:
           if (I > 0)
             CsEn = 0;
             break;
      }
      StartingAddress = CommonDimm->BaseAddress;
      EndingAddress = StartingAddress + CommonDimm->TotalMem - 1;
    } else if (!Popts->MemctlInterleaving) {
      switch (Popts->BaIntlvCtl & DDR_CS0_CS1_CS2_CS3) { //banks interleaving
         case DDR_CS0_CS1_CS2_CS3:
           StartingAddress = CommonDimm->BaseAddress;
           EndingAddress = StartingAddress + CommonDimm->TotalMem - 1;
           break;
         case DDR_CS0_CS1_AND_CS2_CS3:
           if ((I >= 2) && (DimmNumber == 0)) {
             StartingAddress = DimmParams[DimmNumber].BaseAddress + 2 * RankDensity;
             EndingAddress = StartingAddress + 2 * RankDensity - 1;
           } else {
             StartingAddress = DimmParams[DimmNumber].BaseAddress;
             EndingAddress = StartingAddress + 2 * RankDensity - 1;
           }
           break;
         case DDR_CS0_CS1:
           if (DimmParams[DimmNumber].NRanks > (I % CsPerDimm)) {
             StartingAddress = DimmParams[DimmNumber].BaseAddress;
             EndingAddress = StartingAddress + RankDensity - 1;
             if (I != 1) {
               StartingAddress += (I % CsPerDimm) * RankDensity;
             }
             EndingAddress += (I % CsPerDimm) * RankDensity;
           } else {
               StartingAddress = 0;
               EndingAddress = 0;
           }
           if (I == 0) {
             EndingAddress += RankDensity;
           }
           break;
         case DDR_CS2_CS3:
           if (DimmParams[DimmNumber].NRanks > (I % CsPerDimm)) {
             StartingAddress = DimmParams[DimmNumber].BaseAddress;
             EndingAddress = StartingAddress + RankDensity - 1;
             if (I != 3) {
               StartingAddress += (I % CsPerDimm) * RankDensity;
             }
             EndingAddress += (I % CsPerDimm) * RankDensity;
           } else {
             StartingAddress = 0;
             EndingAddress = 0;
           }
           if (I == 2) {
             EndingAddress += (RankDensity >> DbwCapAdj);
           }
           break;
         default:  // no bank interleaving
           StartingAddress = DimmParams[DimmNumber].BaseAddress;
           EndingAddress = StartingAddress + RankDensity - 1;
           if (DimmParams[DimmNumber].NRanks > (I % CsPerDimm)) {
             StartingAddress += (I % CsPerDimm) * RankDensity;
             EndingAddress += (I % CsPerDimm) * RankDensity;
           } else {
             StartingAddress = 0;
             EndingAddress = 0;
           }
           break;
      }
    }

    StartingAddress >>= 24;
    EndingAddress >>= 24;

    if (CsEn) {
      Ddr->Cs[I].Bnds = ((StartingAddress & 0xffff) << 16)
                      | ((EndingAddress & 0xffff) << 0);
    } else {
      Ddr->Cs[I].Bnds = 0xffffffff;
    }
    DEBUG((EFI_D_INFO,"FSLDDR: Cs[%d]Bnds = 0x%08x\n", I, Ddr->Cs[I].Bnds));
    SetCsnConfig(DimmNumber, I, Ddr, Popts, DimmParams);
    SetCsnConfig2(I, Ddr);
  }

  SetDdrEor(Ddr, Popts);
  SetTimingCfg0(Ddr, Popts, DimmParams);
  SetTimingCfg3(Ddr, Popts, CommonDimm, CasLatency, AdditiveLatency);
  SetTimingCfg1(Ddr, Popts, CommonDimm, CasLatency);
  SetTimingCfg2(Ddr, Popts, CommonDimm, CasLatency, AdditiveLatency);
  SetDdrCdr1(Ddr, Popts);
  SetDdrCdr2(Ddr, Popts);
  SetDdrSdramCfg(Ddr, Popts, CommonDimm);
  IpRev = DdrGetCtrlVersion();
  if (IpRev > 0x40400) {
    UnqMrsEn = 1;
  }

  if ((IpRev > 0x40700) && (Popts->CswlOverride != 0)) {
    Ddr->Debug[18] = Popts->CswlOverride;
  }

  SetDdrSdramCfg2(Ddr, Popts, UnqMrsEn);
  SetDdrSdramMode(Ddr, Popts, CommonDimm, CasLatency,
                  AdditiveLatency, UnqMrsEn);
  SetDdrSdramMode2(Ddr, Popts, CommonDimm, UnqMrsEn);
#ifdef MEMORY_TYPE_DDR4
  SetDdrSdramMode9(Ddr, Popts, CommonDimm, UnqMrsEn);
  SetDdrSdramMode10(Ddr, Popts, CommonDimm, UnqMrsEn);
#endif
  SetDdrSdramInterval(Ddr, Popts, CommonDimm);
  SetDdrDataInit(Ddr);
  SetDdrSdramClkCntl(Ddr, Popts);
  SetDdrInitAddr(Ddr);
  SetDdrInitExtAddr(Ddr);
  SetTimingCfg4(Ddr, Popts);
  SetTimingCfg5(Ddr, CasLatency);
#ifdef MEMORY_TYPE_DDR4
  SetDdrSdramCfg3(Ddr, Popts);
  SetTimingCfg6(Ddr);
  SetTimingCfg7(Ddr, CommonDimm);
  SetTimingCfg8(Ddr, Popts, CommonDimm, CasLatency);
  SetTimingCfg9(Ddr);
  SetDdrDqMapping(Ddr, DimmParams);
#endif

  SetDdrZqCntl(Ddr, ZqEn);
  SetDdrWrlvlCntl(Ddr, WrlvlEn, Popts);
  SetDdrSrCntr(Ddr, SrIt);
  SetDdrSdramRcw(Ddr, Popts, CommonDimm);

  return CheckFslMemctlConfigRegs(Ddr);
}
