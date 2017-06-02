/** @DdrMain.c

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include <Library/BaseLib.h>
#include <Library/FslDdr.h>
#include <Library/PcdLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <I2c.h>


//#define DDR_BUILTIN_SELF_TEST
UINT32 DdrRead32(VOID *a)
{
	UINT32 Val = 0;
	Val = MmioRead32((UINTN)a);
//	DEBUG((EFI_D_INFO," READ : 0x%x : 0x%x\n", a, Val));
	return Val;
}

VOID DdrWrite32(VOID *a, UINT32 v)
{
   MmioWrite32((UINTN)a, v);
//	DEBUG((EFI_D_INFO," WRITE : 0x%x : 0x%x\n", a, v));
}

UINT64
DdrAssignAddresses (
  IN   DdrInfoT      *Pinfo,
  IN   UINT32        DbwCapAdj[]
  )
{
  INT32 I, J;
  UINT64 TotalMem, CurrentMemBase, TotalCtlrMem = 0;
  UINT64 RankDensity, CtlrDensity, Cap;
  UINT32 FirstCtrl = Pinfo->FirstCtrl;
  UINT32 LastCtrl = FirstCtrl + Pinfo->NumCtrls - 1;

  for (I = FirstCtrl; I <= LastCtrl; I++) {
    UINT32 Found = 0;

    switch (Pinfo->MemctlOpts[I].DataBusWidth) {
      case 2: // 16-bit
        for (J = 0; J < DDR_DIMM_SLOTS_PER_CTLR; J++) {
           UINT32 Dw;
           if (!Pinfo->DimmParams[I][J].NRanks)
             continue;
           Dw = Pinfo->DimmParams[I][J].PrimarySdramWidth;
             if ((Dw == 72 || Dw == 64)) {
                      DbwCapAdj[I] = 2;
                      break;
             } else if ((Dw == 40 || Dw == 32)) {
                      DbwCapAdj[I] = 1;
                      break;
             }
         }
         break;
      case 1: // 32-bit
        for (J = 0; J < DDR_DIMM_SLOTS_PER_CTLR; J++) {
          UINT32 Dw;
          Dw = Pinfo->DimmParams[I][J].DataWidth;
          if (Pinfo->DimmParams[I][J].NRanks && (Dw == 72 || Dw == 64)) {
            Found = 1;
            break;
          }
        }
        if (Found) {
          DbwCapAdj[I] = 1;
        }
        break;
      case 0: // 64-bit
        break;
      default:
        return 1;
    }
  }

  CurrentMemBase = Pinfo->MemBase;
  TotalMem = 0;

  if (Pinfo->MemctlOpts[FirstCtrl].MemctlInterleaving) {
     RankDensity = Pinfo->DimmParams[FirstCtrl][0].RankDensity
                   >> DbwCapAdj[FirstCtrl];
     switch (Pinfo->MemctlOpts[FirstCtrl].BaIntlvCtl & DDR_CS0_CS1_CS2_CS3) {
        case DDR_CS0_CS1_CS2_CS3:
                 CtlrDensity = 4 * RankDensity;
               break;
        case DDR_CS0_CS1:
        case DDR_CS0_CS1_AND_CS2_CS3:
           CtlrDensity = 2 * RankDensity;
                 break;
        case DDR_CS2_CS3:
        default:
               CtlrDensity = RankDensity;
               break;
    }
    DEBUG((EFI_D_INFO, "RankDensity = 0x%llx, CtlrDensity = 0x%llx\n",
           RankDensity, CtlrDensity));

    for (I = FirstCtrl; I <= LastCtrl; I++) {
       if (Pinfo->MemctlOpts[I].MemctlInterleaving) {
          switch (Pinfo->MemctlOpts[I].MemctlInterleavingMode) {
             case DDR_256B_INTERLEAVING:
             case DDR_CACHE_LINE_INTERLEAVING:
             case DDR_PAGE_INTERLEAVING:
             case DDR_BANK_INTERLEAVING:
             case DDR_SUPERBANK_INTERLEAVING:
                TotalCtlrMem = 2 * CtlrDensity;
                break;
             case DDR_3WAY_1KB_INTERLEAVING:
             case DDR_3WAY_4KB_INTERLEAVING:
             case DDR_3WAY_8KB_INTERLEAVING:
                   TotalCtlrMem = 3 * CtlrDensity;
                   break;
             case DDR_4WAY_1KB_INTERLEAVING:
             case DDR_4WAY_4KB_INTERLEAVING:
             case DDR_4WAY_8KB_INTERLEAVING:
                   TotalCtlrMem = 4 * CtlrDensity;
                   break;
          }
          Pinfo->CommonTimingParams[I].BaseAddress = CurrentMemBase;
          Pinfo->CommonTimingParams[I].TotalMem = TotalCtlrMem;
          TotalMem = CurrentMemBase + TotalCtlrMem;
          DEBUG((EFI_D_INFO, "ctrl %d base 0x%llx\n", I, CurrentMemBase));
          DEBUG((EFI_D_INFO, "ctrl %d total 0x%llx\n", I, TotalCtlrMem));
       } else {
         // if the third controller is not interleaved
         CurrentMemBase = TotalMem;
         TotalCtlrMem = 0;
         Pinfo->CommonTimingParams[I].BaseAddress = CurrentMemBase;

         for (J = 0; J < DDR_DIMM_SLOTS_PER_CTLR; J++) {
            Cap = Pinfo->DimmParams[I][J].Capacity >> DbwCapAdj[I];
            Pinfo->DimmParams[I][J].BaseAddress = CurrentMemBase;
            DEBUG((EFI_D_INFO,"Ctrl %d dimm %d base 0x%llx\n",
                   I, J, CurrentMemBase));
            CurrentMemBase += Cap;
            TotalCtlrMem += Cap;
         }
         Pinfo->CommonTimingParams[I].TotalMem = TotalCtlrMem;
         TotalMem += TotalCtlrMem;
      }
    }
  } else {
    // simple linear assignment if controllers are not interleaved.
    for (I = FirstCtrl; I <= LastCtrl; I++) {
      TotalCtlrMem = 0;
      Pinfo->CommonTimingParams[I].BaseAddress = CurrentMemBase;
      for (J = 0; J < DDR_DIMM_SLOTS_PER_CTLR; J++) {
        // compute DIMM base addresses
         Cap = Pinfo->DimmParams[I][J].Capacity >> DbwCapAdj[I];
         Pinfo->DimmParams[I][J].BaseAddress = CurrentMemBase;
         DEBUG((EFI_D_RELEASE,"Ctrl %d dimm %d base 0x%llx\n",
                I, J, CurrentMemBase));
         CurrentMemBase += Cap;
         TotalCtlrMem += Cap;
      }
      Pinfo->CommonTimingParams[I].TotalMem = TotalCtlrMem;
      TotalMem += TotalCtlrMem;
    }
  }
  DEBUG((EFI_D_INFO,"DdrAssignAddresses: TotalMem = 0x%llx\n", TotalMem));

  return TotalMem;
}


VOID
PrintDdrInfo(
  IN  UINT32   StartCtrl
  )
{
  struct CcsrDdr  *Ddr = (struct CcsrDdr *)(DDR_CTRL1_ADDR);
  UINT32 SdramCfg = DdrRead32(&Ddr->SdramCfg);
  INT32  CasLat;

#if (DDR_CTLRS_NUM > 1)
  UINT32 Cs0Config = DdrRead32(&Ddr->Cs0Config);
#endif

#if DDR_CTLRS_NUM >= 2
  if ((!(SdramCfg & SDRAM_CFG_MEM_EN)) || (StartCtrl == 1)) {
    Ddr = (VOID *)DDR_CTRL2_ADDR;
    SdramCfg = DdrRead32(&Ddr->SdramCfg);
  }
#endif

#if DDR_CTLRS_NUM >= 3
  if ((!(SdramCfg & SDRAM_CFG_MEM_EN)) || (StartCtrl == 2)) {
    Ddr = (VOID *)DDR_CTRL3_ADDR;
    SdramCfg = DdrRead32(&Ddr->SdramCfg);
  }
#endif

  if (!(SdramCfg & SDRAM_CFG_MEM_EN)) {
    DEBUG((EFI_D_RELEASE," (DDR not enabled)\n"));
    return;
  }

  DEBUG((EFI_D_RELEASE,"(DDR"));
  switch ((SdramCfg & SDRAM_CFG_SDRAM_TYPE_MASK) >>
           SDRAM_CFG_SDRAM_TYPE_SHIFT) {
    case SDRAM_TYPE_DDR3:
        DEBUG((EFI_D_RELEASE,"3"));
        break;
    case SDRAM_TYPE_DDR4:
        DEBUG((EFI_D_RELEASE,"4"));
        break;
    default:
        DEBUG((EFI_D_RELEASE,"?"));
        break;
  }

  if (SdramCfg & SDRAM_CFG_32_BE) {
    DEBUG((EFI_D_RELEASE,", 32-bit"));
  } else if (SdramCfg & SDRAM_CFG_16_BE) {
    DEBUG((EFI_D_RELEASE,", 16-bit"));
  } else {
    DEBUG((EFI_D_RELEASE,", 64-bit"));
  }

  // calculate CAS latency
  CasLat = ((DdrRead32(&Ddr->TimingCfg1) >> 16) & 0xf);
  if (DdrGetCtrlVersion() <= 0x40400) {
        CasLat += 1;
  } else {
        CasLat += 2;
  }
  CasLat += ((DdrRead32(&Ddr->TimingCfg3) >> 12) & 3) << 4;
  DEBUG((EFI_D_RELEASE,", CL=%d", CasLat >> 1));
  if (CasLat & 0x1) {
        DEBUG((EFI_D_RELEASE,".5"));
  }

  if (SdramCfg & SDRAM_CFG_ECC_EN) {
    DEBUG((EFI_D_RELEASE,", ECC on)"));
  } else {
    DEBUG((EFI_D_RELEASE,", ECC off)"));
  }

#if (DDR_CTLRS_NUM >= 2)
  if ((Cs0Config & 0x20000000) && (StartCtrl == 0)) {
    DEBUG((EFI_D_RELEASE,"\nDDR Controller Interleaving Mode: "));

    switch ((Cs0Config >> 24) & 0xf) {
    case DDR_256B_INTERLEAVING:
        DEBUG((EFI_D_RELEASE,"256B"));
        break;
    case DDR_CACHE_LINE_INTERLEAVING:
        DEBUG((EFI_D_RELEASE,"cache line"));
        break;
    case DDR_PAGE_INTERLEAVING:
        DEBUG((EFI_D_RELEASE,"page"));
        break;
    case DDR_BANK_INTERLEAVING:
        DEBUG((EFI_D_RELEASE,"bank"));
        break;
    case DDR_SUPERBANK_INTERLEAVING:
        DEBUG((EFI_D_RELEASE,"super-bank"));
        break;
    default:
        DEBUG((EFI_D_RELEASE,"invalid"));
        break;
    }
  }
#endif

  if ((SdramCfg >> 8) & 0x7f) {
    DEBUG((EFI_D_RELEASE,"\nDDR Chip-Select Interleaving Mode: "));
    switch(SdramCfg >> 8 & 0x7f) {
    case DDR_CS0_CS1_CS2_CS3:
        DEBUG((EFI_D_RELEASE,"CS0+CS1+CS2+CS3"));
        break;
    case DDR_CS0_CS1:
        DEBUG((EFI_D_RELEASE,"CS0+CS1"));
        break;
    case DDR_CS2_CS3:
        DEBUG((EFI_D_RELEASE,"CS2+CS3"));
        break;
    case DDR_CS0_CS1_AND_CS2_CS3:
        DEBUG((EFI_D_RELEASE,"CS0+CS1 and CS2+CS3"));
        break;
    default:
        DEBUG((EFI_D_RELEASE,"invalid"));
        break;
    }
    DEBUG((EFI_D_RELEASE,"\n"));
  }
}


VOID
RemoveUnusedControllers (
   DdrInfoT   *Info
  )
{
#ifdef CONFIG_FSL_LSCH3
  INT32 I;
  UINT64 NodeId;
  VOID *HnfSamCtrl = (VOID *)(CCI_HN_F_0_BASE + CCN_HN_F_SAM_CTL);
  bool Ddr0Used = false;
  bool Ddr1Used = false;

  for (I = 0; I < 8; I++) {
    NodeId = MmioRead64(HnfSamCtrl) & CCN_HN_F_SAM_NODEID_MASK;
    if (NodeId == CCN_HN_F_SAM_NODEID_DDR0) {
      Ddr0Used = true;
    } else if (NodeId == CCN_HN_F_SAM_NODEID_DDR1) {
      Ddr1Used = true;
    } else {
      DEBUG((EFI_D_RELEASE, "Unknown NodeId in HN-F SAM control: 0x%llx\n",
             NodeId));
    }
    HnfSamCtrl += (CCI_HN_F_1_BASE - CCI_HN_F_0_BASE);
  }

  if (!Ddr0Used && !Ddr1Used) {
    DEBUG((EFI_D_RELEASE, "Invalid configuration in HN-F SAM control\n"));
    return;
  }

  if (!Ddr0Used && Info->FirstCtrl == 0) {
    Info->FirstCtrl = 1;
    Info->NumCtrls = 1;
    DEBUG((EFI_D_INFO,"First DDR controller disabled\n"));
    return;
  }

  if (!Ddr1Used && Info->FirstCtrl + Info->NumCtrls > 1) {
    Info->NumCtrls = 1;
    DEBUG((EFI_D_INFO, "Second DDR controller disabled\n"));
  }
#endif
}

#ifdef DDR_STATIC_INIT
// initialize DDR in hardcoded way
VOID
DramStaticInit (
    VOID
  )
{
  struct CcsrDdr *Ddr;
  UINT32 Count, CtrlNum;

  for (CtrlNum = 0; CtrlNum < DDR_CTLRS_NUM; CtrlNum++) {

    if (CtrlNum == 0)
       Ddr = (VOID *)DDR_CTRL1_ADDR;
#if defined(DDR_CTRL2_ADDR) && (DDR_CTLRS_NUM > 1)
    if (CtrlNum == 1)
       Ddr = (VOID *)DDR_CTRL2_ADDR;
#endif
#if defined(DDR_CTRL3_ADDR) && (DDR_CTLRS_NUM > 2)
    if (CtrlNum == 2)
      Ddr = (VOID *)DDR_CTRL3_ADDR;
#endif
#if defined(DDR_CTRL4_ADDR) && (DDR_CTLRS_NUM > 3)
    if (CtrlNum == 3)
      Ddr = (VOID *)DDR_CTRL4_ADDR;
#endif

    DdrWrite32(&Ddr->SdramClkCntl, DDR_SDRAM_CLK_CNTL);
    DdrWrite32(&Ddr->Cs0Config2, 0);
    DdrWrite32(&Ddr->SdramCfg, DDR_SDRAM_CFG);
    DdrWrite32(&Ddr->Cs0Bnds, DDR_CS0_BNDS);
    DdrWrite32(&Ddr->Cs1Bnds, DDR_CS1_BNDS);
    DdrWrite32(&Ddr->Cs0Config, DDR_CS0_CONFIG);
    DdrWrite32(&Ddr->Cs1Config, DDR_CS1_CONFIG);
    DdrWrite32(&Ddr->TimingCfg0, DDR_TIMING_CFG_0);
    DdrWrite32(&Ddr->TimingCfg1, DDR_TIMING_CFG_1);
    DdrWrite32(&Ddr->TimingCfg2, DDR_TIMING_CFG_2);
    DdrWrite32(&Ddr->TimingCfg3, DDR_TIMING_CFG_3);
    DdrWrite32(&Ddr->TimingCfg4, DDR_TIMING_CFG_4);
    DdrWrite32(&Ddr->TimingCfg5, DDR_TIMING_CFG_5);
    DdrWrite32(&Ddr->TimingCfg7, DDR_TIMING_CFG_7);
    DdrWrite32(&Ddr->TimingCfg8, DDR_TIMING_CFG_8);
    DdrWrite32(&Ddr->DdrZqCntl, DDR_ZQ_CNTL);
    DdrWrite32(&Ddr->DqMap0, DDR_DQ_MAP_0);
    DdrWrite32(&Ddr->DqMap1, DDR_DQ_MAP_1);
    DdrWrite32(&Ddr->DqMap2, DDR_DQ_MAP_2);
    DdrWrite32(&Ddr->DqMap3, DDR_DQ_MAP_3);
    DdrWrite32(&Ddr->SdramCfg3, DDR_SDRAM_CFG_3);
    DdrWrite32(&Ddr->SdramMode, DDR_SDRAM_MODE);
    DdrWrite32(&Ddr->SdramMode2, DDR_SDRAM_MODE_2);
    DdrWrite32(&Ddr->SdramMode3, DDR_SDRAM_MODE_3);
    DdrWrite32(&Ddr->SdramMode4, DDR_SDRAM_MODE_4);
    DdrWrite32(&Ddr->SdramMode5, DDR_SDRAM_MODE_5);
    DdrWrite32(&Ddr->SdramMode6, DDR_SDRAM_MODE_6);
    DdrWrite32(&Ddr->SdramMode7, DDR_SDRAM_MODE_7);
    DdrWrite32(&Ddr->SdramMode8, DDR_SDRAM_MODE_8);
    DdrWrite32(&Ddr->SdramMode9, DDR_SDRAM_MODE_9);
    DdrWrite32(&Ddr->SdramMode10, DDR_SDRAM_MODE_10);
    DdrWrite32(&Ddr->SdramMode11, DDR_SDRAM_MODE_11);
    DdrWrite32(&Ddr->SdramMode12, DDR_SDRAM_MODE_12);
    DdrWrite32(&Ddr->SdramMode13, DDR_SDRAM_MODE_13);
    DdrWrite32(&Ddr->SdramMode14, DDR_SDRAM_MODE_14);
    DdrWrite32(&Ddr->SdramMode15, DDR_SDRAM_MODE_15);
    DdrWrite32(&Ddr->SdramMode16, DDR_SDRAM_MODE_16);
    DdrWrite32(&Ddr->SdramMdCntl, DDR_SDRAM_MODE_CNTL);
    DdrWrite32(&Ddr->SdramInterval, DDR_SDRAM_INTERVAL);
    DdrWrite32(&Ddr->SdramDataInit, DDR_DATA_INIT);
    DdrWrite32(&Ddr->DdrWrlvlCntl, DDR_WRLVL_CNTL);
    DdrWrite32(&Ddr->DdrWrlvlCntl2, DDR_WRLVL_CNTL_2);
    DdrWrite32(&Ddr->DdrWrlvlCntl3, DDR_WRLVL_CNTL_3);
    DdrWrite32(&Ddr->DdrCdr1, DDRCDR_1);
    DdrWrite32(&Ddr->SdramCfg2, DDR_SDRAM_CFG_2);
    DdrWrite32(&Ddr->DdrCdr2, DDRCDR_2);
    for(Count = 0; Count < 10000; Count++)
      ;
    DdrWrite32(&Ddr->SdramCfg, DDR_SDRAM_CFG | SDRAM_CFG_MEM_EN);
  }
  return;
}
#endif

VOID
DdrRegDump (
  IN  UINT32   CtrlNum
  )
{
  struct CcsrDdr *Ddr = NULL;

    if (CtrlNum == 0)
       Ddr = (VOID *)DDR_CTRL1_ADDR;
#if defined(DDR_CTRL2_ADDR) && (DDR_CTLRS_NUM > 1)
    if (CtrlNum == 1)
       Ddr = (VOID *)DDR_CTRL2_ADDR;
#endif
#if defined(DDR_CTRL3_ADDR) && (DDR_CTLRS_NUM > 2)
    if (CtrlNum == 2)
      Ddr = (VOID *)DDR_CTRL3_ADDR;
#endif
#if defined(DDR_CTRL4_ADDR) && (DDR_CTLRS_NUM > 3)
    if (CtrlNum == 3)
      Ddr = (VOID *)DDR_CTRL4_ADDR;
#endif
    if (Ddr == NULL) {
      DEBUG((EFI_D_ERROR,"Invalid Ddr Controller %u\n",CtrlNum));
      return;
    }

  DEBUG((EFI_D_INFO, "Cs0Bnds = 0x%08x\n", DdrRead32(&Ddr->Cs0Bnds)));
  DEBUG((EFI_D_INFO, "Cs1Bnds = 0x%08x\n", DdrRead32(&Ddr->Cs1Bnds)));
  DEBUG((EFI_D_INFO, "Cs2Bnds = 0x%08x\n", DdrRead32(&Ddr->Cs2Bnds)));
  DEBUG((EFI_D_INFO, "Cs3Bnds = 0x%08x\n", DdrRead32(&Ddr->Cs3Bnds)));
  DEBUG((EFI_D_INFO, "Cs0Config = 0x%08x\n", DdrRead32(&Ddr->Cs0Config)));
  DEBUG((EFI_D_INFO, "Cs1Config = 0x%08x\n", DdrRead32(&Ddr->Cs1Config)));
  DEBUG((EFI_D_INFO, "Cs2Config = 0x%08x\n", DdrRead32(&Ddr->Cs2Config)));
  DEBUG((EFI_D_INFO, "Cs3Config = 0x%08x\n", DdrRead32(&Ddr->Cs3Config)));
  DEBUG((EFI_D_INFO, "Cs0Config2 = 0x%08x\n", DdrRead32(&Ddr->Cs0Config2)));
  DEBUG((EFI_D_INFO, "Cs1Config2 = 0x%08x\n", DdrRead32(&Ddr->Cs1Config2)));
  DEBUG((EFI_D_INFO, "Cs2Config2 = 0x%08x\n", DdrRead32(&Ddr->Cs2Config2)));
  DEBUG((EFI_D_INFO, "Cs3Config2 = 0x%08x\n", DdrRead32(&Ddr->Cs3Config2)));
  DEBUG((EFI_D_INFO, "TimingCfg3 = 0x%08x\n", DdrRead32(&Ddr->TimingCfg3)));
  DEBUG((EFI_D_INFO, "TimingCfg0 = 0x%08x\n", DdrRead32(&Ddr->TimingCfg0)));
  DEBUG((EFI_D_INFO, "TimingCfg1 = 0x%08x\n", DdrRead32(&Ddr->TimingCfg1)));
  DEBUG((EFI_D_INFO, "TimingCfg2 = 0x%08x\n", DdrRead32(&Ddr->TimingCfg2)));
  DEBUG((EFI_D_INFO, "SdramCfg = 0x%08x\n", DdrRead32(&Ddr->SdramCfg)));
  DEBUG((EFI_D_INFO, "SdramCfg2 = 0x%08x\n", DdrRead32(&Ddr->SdramCfg2)));
  DEBUG((EFI_D_INFO, "SdramMode = 0x%08x\n", DdrRead32(&Ddr->SdramMode)));
  DEBUG((EFI_D_INFO, "SdramMode2 = 0x%08x\n", DdrRead32(&Ddr->SdramMode2)));
  DEBUG((EFI_D_INFO, "SdramMdCntl = 0x%08x\n", DdrRead32(&Ddr->SdramMdCntl)));
  DEBUG((EFI_D_INFO, "SdramInterval = 0x%08x\n", DdrRead32(&Ddr->SdramInterval)));
  DEBUG((EFI_D_INFO, "SdramDataInit = 0x%08x\n", DdrRead32(&Ddr->SdramDataInit)));
  DEBUG((EFI_D_INFO, "SdramClkCntl = 0x%08x\n", DdrRead32(&Ddr->SdramClkCntl)));
  DEBUG((EFI_D_INFO, "InitAddr = 0x%08x\n", DdrRead32(&Ddr->InitAddr)));
  DEBUG((EFI_D_INFO, "InitExtAddr = 0x%08x\n", DdrRead32(&Ddr->InitExtAddr)));
  DEBUG((EFI_D_INFO, "TimingCfg4 = 0x%08x\n", DdrRead32(&Ddr->TimingCfg4)));
  DEBUG((EFI_D_INFO, "TimingCfg5 = 0x%08x\n", DdrRead32(&Ddr->TimingCfg5)));
  DEBUG((EFI_D_INFO, "TimingCfg6 = 0x%08x\n", DdrRead32(&Ddr->TimingCfg6)));
  DEBUG((EFI_D_INFO, "TimingCfg7 = 0x%08x\n", DdrRead32(&Ddr->TimingCfg7)));
  DEBUG((EFI_D_INFO, "DdrZqCntl = 0x%08x\n", DdrRead32(&Ddr->DdrZqCntl)));
  DEBUG((EFI_D_INFO, "DdrSrCntr = 0x%08x\n", DdrRead32(&Ddr->DdrSrCntr)));
  DEBUG((EFI_D_INFO, "DdrSdramRcw1 = 0x%08x\n", DdrRead32(&Ddr->DdrSdramRcw1)));
  DEBUG((EFI_D_INFO, "DdrSdramRcw2 = 0x%08x\n", DdrRead32(&Ddr->DdrSdramRcw2)));
  DEBUG((EFI_D_INFO, "DdrWrlvlCntl = 0x%08x\n", DdrRead32(&Ddr->DdrWrlvlCntl)));
  DEBUG((EFI_D_INFO, "DdrWrlvlCntl2 = 0x%08x\n", DdrRead32(&Ddr->DdrWrlvlCntl2)));
  DEBUG((EFI_D_INFO, "DdrWrlvlCntl3 = 0x%08x\n", DdrRead32(&Ddr->DdrWrlvlCntl3)));
  DEBUG((EFI_D_INFO, "DdrSdramRcw3 = 0x%08x\n", DdrRead32(&Ddr->DdrSdramRcw3)));
  DEBUG((EFI_D_INFO, "DdrSdramRcw4 = 0x%08x\n", DdrRead32(&Ddr->DdrSdramRcw4)));
  DEBUG((EFI_D_INFO, "DdrSdramRcw5 = 0x%08x\n", DdrRead32(&Ddr->DdrSdramRcw5)));
  DEBUG((EFI_D_INFO, "DdrSdramRcw6 = 0x%08x\n", DdrRead32(&Ddr->DdrSdramRcw6)));
  DEBUG((EFI_D_INFO, "SdramMode3 = 0x%08x\n", DdrRead32(&Ddr->SdramMode3)));
  DEBUG((EFI_D_INFO, "SdramMode4 = 0x%08x\n", DdrRead32(&Ddr->SdramMode4)));
  DEBUG((EFI_D_INFO, "SdramMode5 = 0x%08x\n", DdrRead32(&Ddr->SdramMode5)));
  DEBUG((EFI_D_INFO, "SdramMode6 = 0x%08x\n", DdrRead32(&Ddr->SdramMode6)));
  DEBUG((EFI_D_INFO, "SdramMode7 = 0x%08x\n", DdrRead32(&Ddr->SdramMode7)));
  DEBUG((EFI_D_INFO, "SdramMode8 = 0x%08x\n", DdrRead32(&Ddr->SdramMode8)));
  DEBUG((EFI_D_INFO, "SdramMode9 = 0x%08x\n", DdrRead32(&Ddr->SdramMode9)));
  DEBUG((EFI_D_INFO, "SdramMode10 = 0x%08x\n", DdrRead32(&Ddr->SdramMode10)));
  DEBUG((EFI_D_INFO, "SdramMode11 = 0x%08x\n", DdrRead32(&Ddr->SdramMode11)));
  DEBUG((EFI_D_INFO, "SdramMode12 = 0x%08x\n", DdrRead32(&Ddr->SdramMode12)));
  DEBUG((EFI_D_INFO, "SdramMode13 = 0x%08x\n", DdrRead32(&Ddr->SdramMode13)));
  DEBUG((EFI_D_INFO, "SdramMode14 = 0x%08x\n", DdrRead32(&Ddr->SdramMode14)));
  DEBUG((EFI_D_INFO, "SdramMode15 = 0x%08x\n", DdrRead32(&Ddr->SdramMode15)));
  DEBUG((EFI_D_INFO, "SdramMode16 = 0x%08x\n", DdrRead32(&Ddr->SdramMode16)));
  DEBUG((EFI_D_INFO, "TimingCfg8 = 0x%08x\n", DdrRead32(&Ddr->TimingCfg8)));
  DEBUG((EFI_D_INFO, "TimingCfg9 = 0x%08x\n", DdrRead32(&Ddr->TimingCfg9)));
  DEBUG((EFI_D_INFO, "SdramCfg3 = 0x%08x\n", DdrRead32(&Ddr->SdramCfg3)));
  DEBUG((EFI_D_INFO, "DqMap0 = 0x%08x\n", DdrRead32(&Ddr->DqMap0)));
  DEBUG((EFI_D_INFO, "DqMap1 = 0x%08x\n", DdrRead32(&Ddr->DqMap1)));
  DEBUG((EFI_D_INFO, "DqMap2 = 0x%08x\n", DdrRead32(&Ddr->DqMap2)));
  DEBUG((EFI_D_INFO, "DqMap3 = 0x%08x\n", DdrRead32(&Ddr->DqMap3)));
  DEBUG((EFI_D_INFO, "DdrDsr1 = 0x%08x\n", DdrRead32(&Ddr->DdrDsr1)));
  DEBUG((EFI_D_INFO, "DdrDsr2 = 0x%08x\n", DdrRead32(&Ddr->DdrDsr2)));
  DEBUG((EFI_D_INFO, "DdrCdr1 = 0x%08x\n", DdrRead32(&Ddr->DdrCdr1)));
  DEBUG((EFI_D_INFO, "DdrCdr2 = 0x%08x\n", DdrRead32(&Ddr->DdrCdr2)));
  DEBUG((EFI_D_INFO, "IpRev1 = 0x%08x\n", DdrRead32(&Ddr->IpRev1)));
  DEBUG((EFI_D_INFO, "IpRev2 = 0x%08x\n", DdrRead32(&Ddr->IpRev2)));
  DEBUG((EFI_D_INFO, "Eor = 0x%08x\n", DdrRead32(&Ddr->Eor)));
  DEBUG((EFI_D_INFO, "Mtcr = 0x%08x\n", DdrRead32(&Ddr->Mtcr)));
  DEBUG((EFI_D_INFO, "Mtp1 = 0x%08x\n", DdrRead32(&Ddr->Mtp1)));
  DEBUG((EFI_D_INFO, "Mtp2 = 0x%08x\n", DdrRead32(&Ddr->Mtp2)));
  DEBUG((EFI_D_INFO, "Mtp3 = 0x%08x\n", DdrRead32(&Ddr->Mtp3)));
  DEBUG((EFI_D_INFO, "Mtp4 = 0x%08x\n", DdrRead32(&Ddr->Mtp4)));
  DEBUG((EFI_D_INFO, "Mtp5 = 0x%08x\n", DdrRead32(&Ddr->Mtp5)));
  DEBUG((EFI_D_INFO, "Mtp6 = 0x%08x\n", DdrRead32(&Ddr->Mtp6)));
  DEBUG((EFI_D_INFO, "Mtp7 = 0x%08x\n", DdrRead32(&Ddr->Mtp7)));
  DEBUG((EFI_D_INFO, "Mtp8 = 0x%08x\n", DdrRead32(&Ddr->Mtp8)));
  DEBUG((EFI_D_INFO, "Mtp9 = 0x%08x\n", DdrRead32(&Ddr->Mtp9)));
  DEBUG((EFI_D_INFO, "Mtp10 = 0x%08x\n", DdrRead32(&Ddr->Mtp10)));
  DEBUG((EFI_D_INFO, "DataErrInjectHi = 0x%08x\n", DdrRead32(&Ddr->DataErrInjectHi)));
  DEBUG((EFI_D_INFO, "DataErrInjectLo = 0x%08x\n", DdrRead32(&Ddr->DataErrInjectLo)));
  DEBUG((EFI_D_INFO, "EccErrInject = 0x%08x\n", DdrRead32(&Ddr->EccErrInject)));
  DEBUG((EFI_D_INFO, "CaptureDataHi = 0x%08x\n", DdrRead32(&Ddr->CaptureDataHi)));
  DEBUG((EFI_D_INFO, "CaptureDataLo = 0x%08x\n", DdrRead32(&Ddr->CaptureDataLo)));
  DEBUG((EFI_D_INFO, "CaptureEcc = 0x%08x\n", DdrRead32(&Ddr->CaptureEcc)));
  DEBUG((EFI_D_INFO, "ErrDetect = 0x%08x\n", DdrRead32(&Ddr->ErrDetect)));
  DEBUG((EFI_D_INFO, "ErrDisable = 0x%08x\n", DdrRead32(&Ddr->ErrDisable)));
  DEBUG((EFI_D_INFO, "ErrIntEn = 0x%08x\n", DdrRead32(&Ddr->ErrIntEn)));
  DEBUG((EFI_D_INFO, "CaptureAttributes = 0x%08x\n", DdrRead32(&Ddr->CaptureAttributes)));
  DEBUG((EFI_D_INFO, "CaptureAddress = 0x%08x\n", DdrRead32(&Ddr->CaptureAddress)));
  DEBUG((EFI_D_INFO, "CaptureExtAddress = 0x%08x\n", DdrRead32(&Ddr->CaptureExtAddress)));
  DEBUG((EFI_D_INFO, "ErrSbe = 0x%08x\n", DdrRead32(&Ddr->ErrSbe)));
  DEBUG((EFI_D_INFO, "Debug[0] = 0x%08x\n", DdrRead32(&Ddr->Debug[0])));
  DEBUG((EFI_D_INFO, "Debug[1] = 0x%08x\n", DdrRead32(&Ddr->Debug[1])));
  DEBUG((EFI_D_INFO, "Debug[5] = 0x%08x\n", DdrRead32(&Ddr->Debug[5])));
  DEBUG((EFI_D_INFO, "Debug[6] = 0x%08x\n", DdrRead32(&Ddr->Debug[6])));
  DEBUG((EFI_D_INFO, "Debug[12] = 0x%08x\n", DdrRead32(&Ddr->Debug[12])));
  DEBUG((EFI_D_INFO, "Debug[21] = 0x%08x\n", DdrRead32(&Ddr->Debug[21])));
  DEBUG((EFI_D_INFO, "Debug[25] = 0x%08x\n", DdrRead32(&Ddr->Debug[25])));
  DEBUG((EFI_D_INFO, "Debug[28] = 0x%08x\n", DdrRead32(&Ddr->Debug[28])));
  DEBUG((EFI_D_INFO, "\n"));
}


//config DDR options
static CONST struct DynamicOdt DualDD[4] = {
  { // cs0
    DDR_ODT_NEVER,
    DDR_ODT_SAME_DIMM,
    DDR3_RTT_120_OHM,
    DDR3_RTT_OFF
  },
  { // cs1
    DDR_ODT_OTHER_DIMM,
    DDR_ODT_OTHER_DIMM,
    DDR3_RTT_30_OHM,
    DDR3_RTT_OFF
  },
  { // cs2
    DDR_ODT_NEVER,
    DDR_ODT_SAME_DIMM,
    DDR3_RTT_120_OHM,
    DDR3_RTT_OFF
  },
  { // cs3
    DDR_ODT_OTHER_DIMM,
    DDR_ODT_OTHER_DIMM,
    DDR3_RTT_30_OHM,
    DDR3_RTT_OFF
  }
};

static CONST struct DynamicOdt DualDS[4] = {
  { // cs0
    DDR_ODT_NEVER,
    DDR_ODT_SAME_DIMM,
    DDR3_RTT_120_OHM,
    DDR3_RTT_OFF
  },
  { // cs1
    DDR_ODT_OTHER_DIMM,
    DDR_ODT_OTHER_DIMM,
    DDR3_RTT_30_OHM,
    DDR3_RTT_OFF
  },
  { // cs2
    DDR_ODT_OTHER_DIMM,
    DDR_ODT_ALL,
    DDR3_RTT_20_OHM,
    DDR3_RTT_120_OHM
  },
  {0, 0, 0, 0}
};


static CONST struct DynamicOdt DualSD[4] = {
  { // cs0
    DDR_ODT_OTHER_DIMM,
    DDR_ODT_ALL,
    DDR3_RTT_20_OHM,
    DDR3_RTT_120_OHM
  },
  {0, 0, 0, 0},
  { // cs2
    DDR_ODT_NEVER,
    DDR_ODT_SAME_DIMM,
    DDR3_RTT_120_OHM,
    DDR3_RTT_OFF
  },
  { // cs3
    DDR_ODT_OTHER_DIMM,
    DDR_ODT_OTHER_DIMM,
    DDR3_RTT_20_OHM,
    DDR3_RTT_OFF
  }
};


static CONST struct DynamicOdt DualSS[4] = {
  { // cs0
    DDR_ODT_OTHER_DIMM,
    DDR_ODT_ALL,
    DDR3_RTT_30_OHM,
    DDR3_RTT_120_OHM
  },
  {0, 0, 0, 0},
  { // cs2
    DDR_ODT_OTHER_DIMM,
    DDR_ODT_ALL,
    DDR3_RTT_30_OHM,
    DDR3_RTT_120_OHM
  },
  {0, 0, 0, 0}
};

static CONST struct DynamicOdt DualD0[4] = {
  { // cs0
    DDR_ODT_NEVER,
    DDR_ODT_SAME_DIMM,
    DDR3_RTT_40_OHM,
    DDR3_RTT_OFF
  },
  { // cs1
    DDR_ODT_NEVER,
    DDR_ODT_NEVER,
    DDR3_RTT_OFF,
    DDR3_RTT_OFF
  },
  {0, 0, 0, 0},
  {0, 0, 0, 0}
};

static CONST struct DynamicOdt Dual0D[4] = {
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  { // cs2
    DDR_ODT_NEVER,
    DDR_ODT_SAME_DIMM,
    DDR3_RTT_40_OHM,
    DDR3_RTT_OFF
  },
  { // cs3
    DDR_ODT_NEVER,
    DDR_ODT_NEVER,
    DDR3_RTT_OFF,
    DDR3_RTT_OFF
  }
};

static CONST struct DynamicOdt DualS0[4] = {
  { // cs0
    DDR_ODT_NEVER,
    DDR_ODT_CS,
    DDR3_RTT_40_OHM,
    DDR3_RTT_OFF
  },
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0}
};

static CONST struct DynamicOdt Dual0S[4] = {
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  { // cs2
    DDR_ODT_NEVER,
    DDR_ODT_CS,
    DDR3_RTT_40_OHM,
    DDR3_RTT_OFF
  },
  {0, 0, 0, 0}
};

STATIC CONST struct DynamicOdt SingleQ[4] = {
  { // cs0
    DDR_ODT_NEVER,
    DDR_ODT_CS_AND_OTHER_DIMM,
    DDR3_RTT_20_OHM,
    DDR3_RTT_120_OHM
  },
  { // cs1
    DDR_ODT_NEVER,
    DDR_ODT_NEVER,
    DDR3_RTT_OFF,
    DDR3_RTT_120_OHM
  },
  { // cs2
    DDR_ODT_NEVER,
    DDR_ODT_CS_AND_OTHER_DIMM,
    DDR3_RTT_20_OHM,
    DDR3_RTT_120_OHM
  },
  { // cs3
    DDR_ODT_NEVER,
    DDR_ODT_NEVER,
    DDR3_RTT_OFF,
    DDR3_RTT_120_OHM
  }
};

static CONST struct DynamicOdt SingleD[4] = {
  { // cs0
    DDR_ODT_NEVER,
    DDR_ODT_ALL,
    DDR3_RTT_40_OHM,
    DDR3_RTT_OFF
  },
  { // cs1
    DDR_ODT_NEVER,
    DDR_ODT_NEVER,
    DDR3_RTT_OFF,
    DDR3_RTT_OFF
  },
  {0, 0, 0, 0},
  {0, 0, 0, 0}
};


static CONST struct DynamicOdt SingleS[4] = {
  { // cs0
    DDR_ODT_NEVER,
    DDR_ODT_ALL,
    DDR3_RTT_40_OHM,
    DDR3_RTT_OFF
  },
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
};

STATIC CONST struct DynamicOdt OdtUnknown[4] = {
  { // cs0
    DDR_ODT_NEVER,
    DDR_ODT_CS,
    DDR3_RTT_120_OHM,
    DDR3_RTT_OFF
  },
  { // cs1
    DDR_ODT_NEVER,
    DDR_ODT_CS,
    DDR3_RTT_120_OHM,
    DDR3_RTT_OFF
  },
  { // cs2
    DDR_ODT_NEVER,
    DDR_ODT_CS,
    DDR3_RTT_120_OHM,
    DDR3_RTT_OFF
  },
  { // cs3
    DDR_ODT_NEVER,
    DDR_ODT_CS,
    DDR3_RTT_120_OHM,
    DDR3_RTT_OFF
  }
};

#if defined(ERRATUM_A008511) | defined(ERRATUM_A009803)
STATIC VOID
SetWaitForBitsClear(
  IN   VOID     *Ptr,
  IN   UINT32   Value,
  IN   UINT32   Bits
  )
{
  UINT32 Timeout = 1000;

  DdrWrite32(Ptr, Value);
  while (DdrRead32(Ptr) & Bits) {
     MicroSecondDelay(100);
     Timeout--;
  }
  if (Timeout <= 0) {
     DEBUG((EFI_D_ERROR,"Error: timeout for wait clean\n"));
  }
}
#endif

UINT32
CfgBankIntlvMode (
  IN   DimmParamsT   *Pdimm
  )
{
#if (DDR_DIMM_SLOTS_PER_CTLR == 1)
  if (Pdimm[0].NRanks == 4)
    return DDR_CS0_CS1_CS2_CS3;
  else if (Pdimm[0].NRanks == 2)
    return DDR_CS0_CS1;
#elif (DDR_DIMM_SLOTS_PER_CTLR == 2)
#ifdef DDR_FIRST_SLOT_QUAD_CAPABLE
  if (Pdimm[0].NRanks == 4)
    return DDR_CS0_CS1_CS2_CS3;
#endif
  if (Pdimm[0].NRanks == 2) {
    if (Pdimm[1].NRanks == 2) {
      return DDR_CS0_CS1_CS2_CS3;
    } else {
      return DDR_CS0_CS1;
    }
  }
#endif
  return 0;
}


UINT32
DdrCfgMemCtrlOptions (
  IN   CommonTimingParamsT  *CommonDimm,
  OUT  MemctlOptionsT       *Popts,
  IN   DimmParamsT          *Pdimm,
  IN   UINT32               CtrlNum
  )
{
  UINT32 I;
  UINT64 DdrFreq;
  CONST struct DynamicOdt *Pdodt = OdtUnknown;

#if (DDR_DIMM_SLOTS_PER_CTLR == 1)
    switch (Pdimm[0].NRanks) {
      case 1:
        Pdodt = SingleS;
         break;
      case 2:
        Pdodt = SingleD;
         break;
      case 4:
        Pdodt = SingleQ;
         break;
    }
#elif (DDR_DIMM_SLOTS_PER_CTLR == 2)
    switch (Pdimm[0].NRanks) {
#ifdef DDR_FIRST_SLOT_QUAD_CAPABLE
      case 4:
        Pdodt = SingleQ;
        if (Pdimm[1].NRanks) {
          DEBUG((EFI_D_INFO,"Error: can't mix quad-rank "
                "and dual-rank DIMMs together\n"));
        }
        break;
#endif
      case 2:
         switch (Pdimm[1].NRanks) {
           case 2:
             Pdodt = DualDD;
             break;
           case 1:
             Pdodt = DualDS;
             break;
           case 0:
             Pdodt = DualD0;
             break;
         }
         break;
      case 1:
         switch (Pdimm[1].NRanks) {
           case 2:
             Pdodt = DualSD;
             break;
           case 1:
             Pdodt = DualSS;
             break;
           case 0:
             Pdodt = DualS0;
             break;
         }
         break;
      case 0:
         switch (Pdimm[1].NRanks) {
           case 2:
             Pdodt = Dual0D;
             break;
           case 1:
             Pdodt = Dual0S;
             break;
         }
         break;
    }
#endif

  for (I = 0; I < DDR_CHIP_SELECTS_PER_CTRL; I++) {
    Popts->CsLocalOpts[I].OdtRdCfg = Pdodt[I].OdtRdCfg;
    Popts->CsLocalOpts[I].OdtWrCfg = Pdodt[I].OdtWrCfg;
    Popts->CsLocalOpts[I].OdtRttNorm = Pdodt[I].OdtRttNorm;
    Popts->CsLocalOpts[I].OdtRttWr = Pdodt[I].OdtRttWr;
    Popts->CsLocalOpts[I].AutoPrecharge = 0;
  }

  Popts->MemctlInterleaving = 0;
  Popts->MemctlInterleavingMode = 0;
  Popts->BaIntlvCtl = 0;
  Popts->RegisteredDimmEn = CommonDimm->AllDimmsRegistered;
#ifdef DDR_ECC_ENABLE
  Popts->EccMode = 1;
#else
  Popts->EccMode = 0;
#endif
  Popts->EccInitUsingMemctl = 1;
  Popts->DqsConfig = 1;
  Popts->SelfRefreshInSleep = 1;
  Popts->DynamicPower = 0;

  if (Pdimm[0].NRanks != 0) {
    if (Pdimm[0].PrimarySdramWidth == 64) {
      Popts->DataBusWidth = 0;
    } else if (Pdimm[0].PrimarySdramWidth == 32) {
      Popts->DataBusWidth = 1;
    } else if (Pdimm[0].PrimarySdramWidth == 16) {
      Popts->DataBusWidth = 2;
    } else {
      DEBUG((EFI_D_ERROR, "Error: primary sdram width %d invalid.\n",
             Pdimm[0].PrimarySdramWidth));
    }
  }

  Popts->X4En = (Pdimm[0].DeviceWidth == 4) ? 1 : 0;

  if ((Popts->DataBusWidth == 1) || (Popts->DataBusWidth == 2)) {
    // 32-bit or 16-bit bus
    Popts->OtfBurstChopEn = 0;
    Popts->BurstLength = DDR_BL8;
  } else {
    Popts->OtfBurstChopEn = 1;     // on-the-fly burst chop
    Popts->BurstLength = DDR_OTF;  // on-the-fly BC4 and BL8
  }

  Popts->MirroredDimm = Pdimm[0].MirroredDimm;
  DEBUG((EFI_D_INFO, "mclk_ps = %u ps\n", GetMemoryClkPeriodPs()));
  Popts->CasLatencyOverride = 0;
  Popts->CasLatencyOverrideValue = 3;
  Popts->UseDeratedCaslat = 0;
  Popts->AdditiveLatencyOverride = 0;
  Popts->AdditiveLatencyOverrideValue = 3;
  Popts->TwotEn = 0;
  Popts->ThreetEn = 0;
  Popts->ApEn = 1; // enable address parity for RDIMM

  /**
    DDR_SDRAM_INTERVAL[BSTOPRE]
    If BSTOPRE is zero, DDR controller uses auto-precharge read and write
    commands rather than operating in page mode.
  **/
  Popts->Bstopre = PicosToMclk(CommonDimm->RefreshRatePs) >> 2;
  Popts->TfawWindowFourActivatesPs = Pdimm[0].TfawPs;
  Popts->ZqEn = 0;
  Popts->WrlvlEn = 0;
  Popts->WrlvlEn = 1;
  Popts->ZqEn = 1;
  Popts->WrlvlOverride = 0;

#if (DDR_CTLRS_NUM > 1)
#ifndef DDR_CTRL_INTLV_256B
    goto done;
#endif

  if (Pdimm[0].NRanks == 0) {
    DEBUG((EFI_D_RELEASE,"There is no rank on CS0 for controller %d.\n", CtrlNum));
    Popts->MemctlInterleaving = 0;
    goto done;
  }

#ifdef DDR_CTRL_INTLV_256B
  Popts->MemctlInterleavingMode = DDR_256B_INTERLEAVING;
  Popts->MemctlInterleaving = 1;
  DEBUG((EFI_D_INFO,"256 Byte interleaving\n"));
#else
  Popts->MemctlInterleavingMode = 0;
  Popts->MemctlInterleaving = 0;
#endif

done:
#endif /* DDR_CTLRS_NUM > 1 */

  Popts->BaIntlvCtl = CfgBankIntlvMode(Pdimm);

  switch (Popts->BaIntlvCtl & DDR_CS0_CS1_CS2_CS3) {
    case DDR_CS0_CS1_CS2_CS3:
#if (DDR_DIMM_SLOTS_PER_CTLR == 1)
    if (Pdimm[0].NRanks < 4) {
      Popts->BaIntlvCtl = 0;
      DEBUG((EFI_D_RELEASE,"Not enough bank(chip-select) for "
            "CS0+CS1+CS2+CS3 on controller %d,"
            "interleaving disabled!\n", CtrlNum));
    }

#elif (DDR_DIMM_SLOTS_PER_CTLR == 2)
#ifdef DDR_FIRST_SLOT_QUAD_CAPABLE
    if (Pdimm[0].NRanks == 4) {
      break;
    }
#endif
    if ((Pdimm[0].NRanks < 2) && (Pdimm[1].NRanks < 2)) {
      Popts->BaIntlvCtl = 0;
      DEBUG((EFI_D_RELEASE,"Not enough bank(chip-select) for "
            "CS0+CS1+CS2+CS3 on controller %d, "
            "interleaving disabled!\n", CtrlNum));
    }
    if (Pdimm[0].Capacity != Pdimm[1].Capacity) {
      Popts->BaIntlvCtl = 0;
      DEBUG((EFI_D_RELEASE,"Not identical DIMM size for "
            "CS0+CS1+CS2+CS3 on controller %d, "
            "interleaving disabled!\n", CtrlNum));
    }
#endif
     break;
   case DDR_CS0_CS1:
     if (Pdimm[0].NRanks < 2) {
       Popts->BaIntlvCtl = 0;
       DEBUG((EFI_D_RELEASE,"Not enough bank(chip-select) for "
             "CS0+CS1 on controller %d, "
             "interleaving disabled!\n", CtrlNum));
     }
     break;
   case DDR_CS2_CS3:
#if (DDR_DIMM_SLOTS_PER_CTLR == 1)
     if (Pdimm[0].NRanks < 4) {
       Popts->BaIntlvCtl = 0;
       DEBUG((EFI_D_RELEASE,"Not enough bank(chip-select) for CS2+CS3 "
             "on controller %d, interleaving disabled!\n", CtrlNum));
     }
#elif (DDR_DIMM_SLOTS_PER_CTLR == 2)
     if (Pdimm[1].NRanks < 2) {
       Popts->BaIntlvCtl = 0;
       DEBUG((EFI_D_RELEASE,"Not enough bank(chip-select) for CS2+CS3 "
             "on controller %d, interleaving disabled!\n", CtrlNum));
     }
#endif
     break;
   case DDR_CS0_CS1_AND_CS2_CS3:
#if (DDR_DIMM_SLOTS_PER_CTLR == 1)
     if (Pdimm[0].NRanks < 4) {
       Popts->BaIntlvCtl = 0;
       DEBUG((EFI_D_RELEASE,"Not enough bank(CS) for CS0+CS1 and "
             "CS2+CS3 on controller %d, "
             "interleaving disabled!\n", CtrlNum));
     }
#elif (DDR_DIMM_SLOTS_PER_CTLR == 2)
     if ((Pdimm[0].NRanks < 2) || (Pdimm[1].NRanks < 2)) {
       Popts->BaIntlvCtl = 0;
       DEBUG((EFI_D_RELEASE,"Not enough bank(CS) for CS0+CS1 and "
             "CS2+CS3 on controller %d, interleaving disabled!\n", CtrlNum));
     }
#endif
     break;
   default:
     Popts->BaIntlvCtl = 0;
     break;
   }

  Popts->AddrHash = 0;

  if (Pdimm[0].NRanks == 4)
    Popts->QuadRankPresent = 1;

  DdrFreq = GetDdrFreq() / 1000000;
  if (Popts->RegisteredDimmEn) {
    Popts->RcwOverride = 1;
    Popts->Rcw1 = 0x000a5a00;
    if (DdrFreq <= 800)
      Popts->Rcw2 = 0x00000000;
    else if (DdrFreq <= 1066)
      Popts->Rcw2 = 0x00100000;
    else if (DdrFreq <= 1333)
      Popts->Rcw2 = 0x00200000;
    else
      Popts->Rcw2 = 0x00300000;
  }

  DdrBoardOptions(Popts, Pdimm);

  return 0;
}


VOID
DdrCheckIntlvOptions (
  IN   DdrInfoT    *Pinfo
  )
{
  INT32 I, J = 0, K, CheckNRanks, IntlvInvalid = 0;
  UINT32 CheckIntlv, CheckNRowAddr, CheckNColAddr;
  UINT64 CheckRankDensity;
  DimmParamsT *Dimm;
  INT32 FirstCtrl = Pinfo->FirstCtrl;
  INT32 LastCtrl = FirstCtrl + Pinfo->NumCtrls - 1;

  CheckNRanks = Pinfo->DimmParams[FirstCtrl][0].NRanks;
  CheckRankDensity = Pinfo->DimmParams[FirstCtrl][0].RankDensity;
  CheckNRowAddr =  Pinfo->DimmParams[FirstCtrl][0].NRowAddr;
  CheckNColAddr = Pinfo->DimmParams[FirstCtrl][0].NColAddr;
  CheckIntlv = Pinfo->MemctlOpts[FirstCtrl].MemctlInterleavingMode;
  for (I = FirstCtrl; I <= LastCtrl; I++) {
    Dimm = &Pinfo->DimmParams[I][0];
    if (!Pinfo->MemctlOpts[I].MemctlInterleaving) {
      continue;
    } else if (((CheckRankDensity != Dimm->RankDensity) ||
             (CheckNRanks != Dimm->NRanks) ||
             (CheckNRowAddr != Dimm->NRowAddr) ||
             (CheckNColAddr != Dimm->NColAddr) ||
             (CheckIntlv != Pinfo->MemctlOpts[I].MemctlInterleavingMode))){
      IntlvInvalid = 1;
      break;
    } else {
      J++;
    }
  }
  if (IntlvInvalid) {
    for (I = FirstCtrl; I <= LastCtrl; I++) {
      Pinfo->MemctlOpts[I].MemctlInterleaving = 0;
    }
    DEBUG((EFI_D_INFO, "Not all DIMMs are identical, interleaving disabled.\n"));
  } else {
    switch (CheckIntlv) {
      case DDR_256B_INTERLEAVING:
      case DDR_CACHE_LINE_INTERLEAVING:
      case DDR_PAGE_INTERLEAVING:
      case DDR_BANK_INTERLEAVING:
      case DDR_SUPERBANK_INTERLEAVING:
         if (3 == DDR_CTLRS_NUM) {
           K = 2;
         } else {
           K = DDR_CTLRS_NUM;
         }
         break;
      case DDR_3WAY_1KB_INTERLEAVING:
      case DDR_3WAY_4KB_INTERLEAVING:
      case DDR_3WAY_8KB_INTERLEAVING:
      case DDR_4WAY_1KB_INTERLEAVING:
      case DDR_4WAY_4KB_INTERLEAVING:
      case DDR_4WAY_8KB_INTERLEAVING:
      default:
         K = DDR_CTLRS_NUM;
         break;
    }
    if (J && (J != K)) {
      for (I = FirstCtrl; I <= LastCtrl; I++)
        Pinfo->MemctlOpts[I].MemctlInterleaving = 0;
      if ((LastCtrl - FirstCtrl) > 1) {
         DEBUG((EFI_D_INFO,"Not all controllers have compatible "
                "interleaving mode. all disabled.\n"));
      }
    }
  }
}


VOID
DdrSetMemCtrlRegs (
  IN   CONST DdrCfgRegsT    *Regs,
  IN   UINT32               CtrlNum,
  IN   INT32                Step
  )
{
  UINT8 I = 0;;
  UINT32 TempSdramCfg = 0;
  UINT32 TotalGbSizePerController = 0;
  UINT32 BusWidth = 0, Timeout = 0, Temp32 = 0;
  struct CcsrDdr *Ddr;
#if defined(ERRATUM_A008336) || defined(ERRATUM_A008514)
  UINT32 *Eddrtqcr1;
#endif
#ifdef ERRATUM_A008511
  UINT32 MR6;
  UINT32 VrefSeq1[3] = {0x80, 0x96, 0x16};	/* for range 1 */
  UINT32 VrefSeq2[3] = {0xc0, 0xf0, 0x70};	/* for range 2 */
  UINT32 *VrefSeq = VrefSeq1;
#endif
#ifdef DDR_BUILTIN_SELF_TEST
  UINT32 Mtcr = 0, ErrDetect = 0, ErrSbe = 0;
  UINT32 Cs0Bnds = 0, Cs1Bnds = 0, Cs2Bnds = 0, Cs3Bnds = 0, Cs0Config = 0;
#endif

  DEBUG((EFI_D_INFO, "--------- START WRITING ---------\n"));
  switch (CtrlNum) {
    case 0:
      Ddr = (VOID *)DDR_CTRL1_ADDR;
#if defined(ERRATUM_A008336) || defined(ERRATUM_A008514)
      Eddrtqcr1 = (VOID *)CONFIG_SYS_FSL_DCSR_DDR_ADDR + 0x800;
#endif
      break;
#if defined(DDR_CTRL2_ADDR) && (DDR_CTLRS_NUM > 1)
    case 1:
      Ddr = (VOID *)DDR_CTRL2_ADDR;
#if defined(ERRATUM_A008336) || defined(ERRATUM_A008514)
      Eddrtqcr1 = (VOID *)CONFIG_SYS_FSL_DCSR_DDR2_ADDR + 0x800;
#endif
      break;
#endif
#if defined(DDR_CTRL3_ADDR) && (DDR_CTLRS_NUM > 2)
    case 2:
      Ddr = (VOID *)DDR_CTRL3_ADDR;
#if defined(ERRATUM_A008336) || defined(ERRATUM_A008514)
      Eddrtqcr1 = (VOID *)CONFIG_SYS_FSL_DCSR_DDR3_ADDR + 0x800;
#endif
      break;
#endif
#if defined(DDR_CTRL4_ADDR) && (DDR_CTLRS_NUM > 3)
    case 3:
      Ddr = (VOID *)DDR_CTRL4_ADDR;
#if defined(ERRATUM_A008336) || defined(ERRATUM_A008514)
      Eddrtqcr1 = (VOID *)CONFIG_SYS_FSL_DCSR_DDR4_ADDR + 0x800;
#endif
      break;
#endif
    default:
      DEBUG((EFI_D_ERROR,"%s unexpected CtrlNum = %u\n",__func__, CtrlNum));
      return;
   }

  if (Step == 2)
    goto Step2;

#ifdef ERRATUM_A008336
#if defined(CONFIG_LS2088A) || defined(CONFIG_LS2085A)
  /* A008336 only applies to general DDR controllers */
  if ((CtrlNum == 0) || (CtrlNum == 1))
#endif
    DdrWrite32(Eddrtqcr1, 0x63b30002);
#endif

  if (Regs->DdrEor) {
    DdrWrite32(&Ddr->Eor, Regs->DdrEor);
  }

  DdrWrite32(&Ddr->SdramClkCntl, Regs->DdrSdramClkCntl);

  for (I = 0; I < DDR_CHIP_SELECTS_PER_CTRL; I++) {
    if (I == 0) {
      DdrWrite32(&Ddr->Cs0Bnds, Regs->Cs[I].Bnds);
      DdrWrite32(&Ddr->Cs0Config, Regs->Cs[I].Config);
      DdrWrite32(&Ddr->Cs0Config2, Regs->Cs[I].Config2);
    } else if (I == 1) {
      DdrWrite32(&Ddr->Cs1Bnds, Regs->Cs[I].Bnds);
      DdrWrite32(&Ddr->Cs1Config, Regs->Cs[I].Config);
      DdrWrite32(&Ddr->Cs1Config2, Regs->Cs[I].Config2);
    } else if (I == 2) {
      DdrWrite32(&Ddr->Cs2Bnds, Regs->Cs[I].Bnds);
      DdrWrite32(&Ddr->Cs2Config, Regs->Cs[I].Config);
      DdrWrite32(&Ddr->Cs2Config2, Regs->Cs[I].Config2);
    } else if (I == 3) {
      DdrWrite32(&Ddr->Cs3Bnds, Regs->Cs[I].Bnds);
      DdrWrite32(&Ddr->Cs3Config, Regs->Cs[I].Config);
      DdrWrite32(&Ddr->Cs3Config2, Regs->Cs[I].Config2);
    }
  }

  DdrWrite32(&Ddr->TimingCfg3, Regs->TimingCfg3);
  DdrWrite32(&Ddr->TimingCfg0, Regs->TimingCfg0);
  DdrWrite32(&Ddr->TimingCfg1, Regs->TimingCfg1);
  DdrWrite32(&Ddr->TimingCfg2, Regs->TimingCfg2);
  DdrWrite32(&Ddr->TimingCfg4, Regs->TimingCfg4);
  DdrWrite32(&Ddr->TimingCfg5, Regs->TimingCfg5);
  DdrWrite32(&Ddr->TimingCfg6, Regs->TimingCfg6);
  DdrWrite32(&Ddr->TimingCfg7, Regs->TimingCfg7);
  DdrWrite32(&Ddr->TimingCfg8, Regs->TimingCfg8);
  DdrWrite32(&Ddr->TimingCfg9, Regs->TimingCfg9);
  DdrWrite32(&Ddr->DdrZqCntl, Regs->DdrZqCntl);
  DdrWrite32(&Ddr->DqMap0, Regs->DqMap0);
  DdrWrite32(&Ddr->DqMap1, Regs->DqMap1);
  DdrWrite32(&Ddr->DqMap2, Regs->DqMap2);
  DdrWrite32(&Ddr->DqMap3, Regs->DqMap3);
  DdrWrite32(&Ddr->SdramCfg3, Regs->DdrSdramCfg3);
  DdrWrite32(&Ddr->SdramMode, Regs->DdrSdramMode);
  DdrWrite32(&Ddr->SdramMode2, Regs->DdrSdramMode2);
  DdrWrite32(&Ddr->SdramMode3, Regs->DdrSdramMode3);
  DdrWrite32(&Ddr->SdramMode4, Regs->DdrSdramMode4);
  DdrWrite32(&Ddr->SdramMode5, Regs->DdrSdramMode5);
  DdrWrite32(&Ddr->SdramMode6, Regs->DdrSdramMode6);
  DdrWrite32(&Ddr->SdramMode7, Regs->DdrSdramMode7);
  DdrWrite32(&Ddr->SdramMode8, Regs->DdrSdramMode8);
  DdrWrite32(&Ddr->SdramMode9, Regs->DdrSdramMode9);
  DdrWrite32(&Ddr->SdramMode10, Regs->DdrSdramMode10);
  DdrWrite32(&Ddr->SdramMode11, Regs->DdrSdramMode11);
  DdrWrite32(&Ddr->SdramMode12, Regs->DdrSdramMode12);
  DdrWrite32(&Ddr->SdramMode13, Regs->DdrSdramMode13);
  DdrWrite32(&Ddr->SdramMode14, Regs->DdrSdramMode14);
  DdrWrite32(&Ddr->SdramMode15, Regs->DdrSdramMode15);
  DdrWrite32(&Ddr->SdramMode16, Regs->DdrSdramMode16);
  DdrWrite32(&Ddr->SdramMdCntl, Regs->DdrSdramMdCntl);
#ifdef ERRATUM_A009663
  DdrWrite32(&Ddr->SdramInterval,
             Regs->DdrSdramInterval & ~SDRAM_INTERVAL_BSTOPRE);
#else
  DdrWrite32(&Ddr->SdramInterval, Regs->DdrSdramInterval);
#endif
  DdrWrite32(&Ddr->SdramDataInit, Regs->DdrDataInit);
  DdrWrite32(&Ddr->DdrWrlvlCntl, Regs->DdrWrlvlCntl);
  if (Regs->DdrWrlvlCntl2) {
    DdrWrite32(&Ddr->DdrWrlvlCntl2, Regs->DdrWrlvlCntl2);
  }
  if (Regs->DdrWrlvlCntl3) {
    DdrWrite32(&Ddr->DdrWrlvlCntl3, Regs->DdrWrlvlCntl3);
  }
  DdrWrite32(&Ddr->DdrSrCntr, Regs->DdrSrCntr);
  DdrWrite32(&Ddr->DdrSdramRcw1, Regs->DdrSdramRcw1);
  DdrWrite32(&Ddr->DdrSdramRcw2, Regs->DdrSdramRcw2);
  DdrWrite32(&Ddr->DdrSdramRcw3, Regs->DdrSdramRcw3);
  DdrWrite32(&Ddr->DdrSdramRcw4, Regs->DdrSdramRcw4);
  DdrWrite32(&Ddr->DdrSdramRcw5, Regs->DdrSdramRcw5);
  DdrWrite32(&Ddr->DdrSdramRcw6, Regs->DdrSdramRcw6);
  DdrWrite32(&Ddr->DdrCdr1, Regs->DdrCdr1);
  DdrWrite32(&Ddr->SdramCfg2, Regs->DdrSdramCfg2);
  DdrWrite32(&Ddr->InitAddr, Regs->DdrInitAddr);
  DdrWrite32(&Ddr->InitExtAddr, Regs->DdrInitExtAddr);
  DdrWrite32(&Ddr->DdrCdr2, Regs->DdrCdr2);

#ifdef ERRATUM_A009803
      /* part 1 of 2 */
  if (Regs->DdrSdramCfg2 & SDRAM_CFG2_AP_EN) {
    if (Regs->DdrSdramCfg & SDRAM_CFG_RD_EN) { /* for RDIMM */
      DdrWrite32(&Ddr->DdrSdramRcw2,
                  Regs->DdrSdramRcw2 & ~0x0f000000);
    }

    DdrWrite32(&Ddr->ErrDisable, Regs->ErrDisable | DDR_APARITY_ERR_DISABLE);
  }
#else
  DdrWrite32(&Ddr->ErrDisable, Regs->ErrDisable);
#endif

  DdrWrite32(&Ddr->ErrIntEn, Regs->ErrIntEn);
#if 1
  for (I = 0; I < 64; I++) {
    if (Regs->Debug[I]) {
	DEBUG((EFI_D_INFO,"I %d \n", I));
      DdrWrite32(&Ddr->Debug[I], Regs->Debug[I]);
    }
  }
#endif
#ifdef ERRATUM_A008378
#define IS_ACC_ECC_EN(v) ((v) & 0x4)
#define IS_DBI(v) ((((v) >> 12) & 0x3) == 0x2)
  if (IS_ACC_ECC_EN(Regs->DdrSdramCfg) || IS_DBI(Regs->DdrSdramCfg3))
     DdrSetBits32(Ddr->Debug[28], 0x9 << 20);
#endif

#ifdef ERRATUM_A008511
  // part 1 of 2
  if (DdrGetCtrlVersion() == 0x50200) {
     // Disable DRAM VRef training
     DdrWrite32(&Ddr->DdrCdr2, Regs->DdrCdr2 & ~DDRCDR2_VREF_TRAIN_EN);
    /* disable transmit bit deskew */
     Temp32 = DdrRead32(&Ddr->Debug[28]);
     Temp32 |= DDR_TX_DSKW_DISABLE;
     DdrWrite32(&Ddr->Debug[28], Temp32);
     DdrWrite32(&Ddr->Debug[25], 0x9000);
  } else if (DdrGetCtrlVersion() == 0x50201) {
     /* Output enable forced off */
     DdrWrite32(&Ddr->Debug[37], 1 << 31);
     /* Enable Vref training */
     DdrWrite32(&Ddr->DdrCdr2, Regs->DdrCdr2 | DDRCDR2_VREF_TRAIN_EN);
  } else {
	DEBUG((EFI_D_ERROR, "Erratum A008511 doesn't apply\n"));
  }
#endif
#if defined(ERRATUM_A009803) || defined(ERRATUM_A008511)
   // Disable D_INIT
   DdrWrite32(&Ddr->SdramCfg2, Regs->DdrSdramCfg2 & ~SDRAM_CFG2_D_INIT);
#endif

#ifdef ERRATUM_A009801
	Temp32 = DdrRead32(&Ddr->Debug[25]);
	Temp32 &= ~CAS_TO_PRE_SUB_MASK;
	Temp32 |= 9 << CAS_TO_PRE_SUB_SHIFT;
	DdrWrite32(&Ddr->Debug[25], Temp32);
#endif

#ifdef ERRATUM_A009942
  UINT64 DdrFreq = GetDdrFreq() / 1000000;

  Temp32 = DdrRead32(&Ddr->Debug[28]);
  DEBUG((EFI_D_INFO,"ERRATUM_A009942 Temp32 0x%x \n", Temp32));

  if (DdrFreq <= 1333)
    DdrWrite32(&Ddr->Debug[28], Temp32 | 0x0080006a);
  else if (DdrFreq <= 1600)
    DdrWrite32(&Ddr->Debug[28], Temp32 | 0x0070006f);
  else if (DdrFreq <= 1867)
    DdrWrite32(&Ddr->Debug[28], Temp32 | 0x00700076);
  else if (DdrFreq <= 2133)
    DdrWrite32(&Ddr->Debug[28], Temp32 | 0x0060007b);
#endif
#ifdef ERRATUM_A010165
  UINT64 DdrF = GetDdrFreq() / 1000000;
  if ((DdrF > 1900) && (DdrF < 2300)) {
         Temp32 = DdrRead32(&Ddr->Debug[28]);
  	  DEBUG((EFI_D_INFO,"ERRATUM_A010165 Temp 0x%x \n", Temp32));
         DdrWrite32(&Ddr->Debug[28], Temp32 | 0x000a0000);
  }
#endif

  if (Step == 1) {
    if (PcdGet32(PcdBootMode) == QSPI_BOOT)
      MicroSecondDelay(160000);
    else
      MicroSecondDelay(200);
    return;
  }

Step2:
  // Set, but do not enable the memory
  TempSdramCfg = Regs->DdrSdramCfg;
  TempSdramCfg &= ~(SDRAM_CFG_MEM_EN);
  DdrWrite32(&Ddr->SdramCfg, TempSdramCfg);

  // Spec requires 500us elapsed between DDR clock setup and DDR Config enable
  if (PcdGet32(PcdBootMode) == QSPI_BOOT)
    MicroSecondDelay(40000);
  else
    MicroSecondDelay(1000);

  asm volatile("dsb sy;isb");

  // enable controller
  TempSdramCfg = DdrRead32(&Ddr->SdramCfg) & ~SDRAM_CFG_BI;
  DdrWrite32(&Ddr->SdramCfg, TempSdramCfg | SDRAM_CFG_MEM_EN);
  asm volatile("dsb sy;isb");

#if defined(ERRATUM_A008511) | defined(ERRATUM_A009803)
  // Part 2 of 2, This erraum only applies to verion 5.2.0
//  if (DdrGetCtrlVersion() == 0x50200 || DdrGetCtrlVersion() == 0x50201) {
     // wait for idle
     Timeout = 5000;
     while (!(DdrRead32(&Ddr->Debug[1]) & 0x2) && (Timeout > 0)) {
         MicroSecondDelay(1000);
         Timeout--;
     }
     if (Timeout <= 0) {
        DEBUG((EFI_D_INFO,"Controler %d Timeout, debug_2 = 0x%x\n",
               CtrlNum, DdrRead32(&Ddr->Debug[1])));
     }
#ifdef ERRATUM_A008511
     // This erraum only applies to verion 5.2.0
   if (DdrGetCtrlVersion() == 0x50200) {
     // The vref setting sequence is different for range 2
     if (Regs->DdrCdr2 & DDRCDR2_VREF_RANGE_2)
       VrefSeq = VrefSeq2;
 
     // Set VREF
     for (I = 0; I < DDR_CHIP_SELECTS_PER_CTRL; I++) {
        if (!(Regs->Cs[I].Config & SDRAM_CS_CONFIG_EN))
           continue;

        MR6 = (Regs->DdrSdramMode10 >> 16) | DDR_MODE_EN | DDR_MODE_CS_SEL(I) |
               DDR_MODE_MD_SEL(6) | 0x00200000;
        Temp32 = MR6 | VrefSeq[0];
        SetWaitForBitsClear(&Ddr->SdramMdCntl, Temp32, DDR_MODE_EN);
        MicroSecondDelay(1);
        DEBUG((EFI_D_INFO,"MR6 = 0x%08x\n", Temp32));
        Temp32 = MR6 | VrefSeq[1];
        SetWaitForBitsClear(&Ddr->SdramMdCntl, Temp32, DDR_MODE_EN);
        MicroSecondDelay(1);
        DEBUG((EFI_D_INFO,"MR6 = 0x%08x\n", Temp32));
        Temp32 = MR6 | VrefSeq[2];
        SetWaitForBitsClear(&Ddr->SdramMdCntl, Temp32, DDR_MODE_EN);
        MicroSecondDelay(1);
        DEBUG((EFI_D_INFO,"MR6 = 0x%08x\n", Temp32));
    }

    DdrWrite32(&Ddr->SdramMdCntl, 0);

    Temp32 = DdrRead32(&Ddr->Debug[28]);
    Temp32 &= ~DDR_TX_DSKW_DISABLE; /* Enable deskew */
    DdrWrite32(&Ddr->Debug[28], Temp32);
    DdrWrite32(&Ddr->Debug[1], 0x400);    /* restart deskew */
    /* wait for idle */
    Timeout = 200;
    while (!(DdrRead32(&Ddr->Debug[1]) & 0x2) && (Timeout > 0)) {
        MicroSecondDelay(1000);
        Timeout--;
    }
    if (Timeout <= 0) {
        DEBUG((EFI_D_INFO,"Controler %d Timeout, debug_2 = %x\n",
               CtrlNum, DdrRead32(&Ddr->Debug[1])));
    }
  }
    /* restore D_INIT */
//    DdrWrite32(&Ddr->SdramCfg2, Regs->DdrSdramCfg2);
#endif /* ERRATUM_A008511 */

#ifdef ERRATUM_A009803
  // if it's RDIMM
  if (Regs->DdrSdramCfg2 & SDRAM_CFG2_AP_EN) {
    if (Regs->DdrSdramCfg & SDRAM_CFG_RD_EN) {
      for (I = 0; I < DDR_CHIP_SELECTS_PER_CTRL; I++) {
        if (!(Regs->Cs[I].Config & SDRAM_CS_CONFIG_EN))
          continue;
        SetWaitForBitsClear(&Ddr->SdramMdCntl,
                             DDR_MODE_EN|
                             DDR_MODE_CS_SEL(I) |
                             0x070000ed,
                             DDR_MODE_EN);
        MicroSecondDelay(1);
      }
    }
    DdrWrite32(&Ddr->ErrDisable, Regs->ErrDisable & ~DDR_APARITY_ERR_DISABLE);
  }
#endif
  //}
  /* restore D_INIT */
  DdrWrite32(&Ddr->SdramCfg2, Regs->DdrSdramCfg2);
#endif

  TotalGbSizePerController = 0;
  for (I = 0; I < DDR_CHIP_SELECTS_PER_CTRL; I++) {
    if (!(Regs->Cs[I].Config & 0x80000000)) {
      continue;
    }
    TotalGbSizePerController += 1 << (
      ((Regs->Cs[I].Config >> 14) & 0x3) + 2 +
      ((Regs->Cs[I].Config >> 8) & 0x7) + 12 +
      ((Regs->Cs[I].Config >> 4) & 0x3) + 0 +
      ((Regs->Cs[I].Config >> 0) & 0x7) + 8 + 3 -
      ((Regs->DdrSdramCfg >> 19) & 0x3) - 26); //minus 2^26 = 64M
  }

  if (Regs->Cs[0].Config & 0x20000000) { // 2-way interleaving
    TotalGbSizePerController <<= 1;
  }

  BusWidth = 3 - ((DdrRead32(&Ddr->SdramCfg) &
             SDRAM_CFG_DBW_MASK) >> SDRAM_CFG_DBW_SHIFT);
  Timeout = ((TotalGbSizePerController << (6 - BusWidth)) * 100 /
             (GetDdrFreq() >> 20)) << 2;
  TotalGbSizePerController >>= 4;  // shift down to gb size
  DEBUG((EFI_D_INFO,"Total %d GB\n", TotalGbSizePerController));
  DEBUG((EFI_D_INFO,"need to wait up to %d * 10ms\n", Timeout));

  if (PcdGet32(PcdBootMode) == QSPI_BOOT)
    Timeout *= 80;
  // wait for init done
  while ((DdrRead32(&Ddr->SdramCfg2) & SDRAM_CFG2_D_INIT) && (Timeout > 0)) {
    MicroSecondDelay(10000);
    Timeout--;
  }

  if (Timeout <= 0) {
    DEBUG((EFI_D_INFO,"Waiting for D_INIT timeout. memory may not work.\n"));
  }
  else {
    DEBUG((EFI_D_INFO,"D_INIT set successfully\n"));
  }

#ifdef ERRATUM_A009663
  DdrWrite32(&Ddr->SdramInterval, Regs->DdrSdramInterval);
#endif

#ifdef DDR_BUILTIN_SELF_TEST
#define BIST_PATTERN1   0xFFFFFFFF
#define BIST_PATTERN2   0x0
#define BIST_CR         0x80010000
#define BIST_CR_EN      0x80000000
#define BIST_CR_STAT    0x00000001
#define CTLR_INTLV_MASK 0x20000000
  DEBUG((EFI_D_RELEASE,"Running DDR BIST test..."));
  Cs0Config = DdrRead32(&Ddr->Cs0Config);
  if (Cs0Config & CTLR_INTLV_MASK) {
      Cs0Bnds = DdrRead32(&Ddr->Cs0Bnds);
      Cs1Bnds = DdrRead32(&Ddr->Cs1Bnds);
      Cs2Bnds = DdrRead32(&Ddr->Cs2Bnds);
      Cs3Bnds = DdrRead32(&Ddr->Cs3Bnds);
      /* set bnds to non-interleaving */
      DdrWrite32(&Ddr->Cs0Bnds, (Cs0Bnds & 0xfffefffe) >> 1);
      DdrWrite32(&Ddr->Cs1Bnds, (Cs1Bnds & 0xfffefffe) >> 1);
      DdrWrite32(&Ddr->Cs2Bnds, (Cs2Bnds & 0xfffefffe) >> 1);
      DdrWrite32(&Ddr->Cs3Bnds, (Cs3Bnds & 0xfffefffe) >> 1);
  }
  DdrWrite32(&Ddr->Mtp1, BIST_PATTERN1);
  DdrWrite32(&Ddr->Mtp2, BIST_PATTERN1);
  DdrWrite32(&Ddr->Mtp3, BIST_PATTERN2);
  DdrWrite32(&Ddr->Mtp4, BIST_PATTERN2);
  DdrWrite32(&Ddr->Mtp5, BIST_PATTERN1);
  DdrWrite32(&Ddr->Mtp6, BIST_PATTERN1);
  DdrWrite32(&Ddr->Mtp7, BIST_PATTERN2);
  DdrWrite32(&Ddr->Mtp8, BIST_PATTERN2);
  DdrWrite32(&Ddr->Mtp9, BIST_PATTERN1);
  DdrWrite32(&Ddr->Mtp10, BIST_PATTERN2);
  Mtcr = BIST_CR;
  DdrWrite32(&Ddr->Mtcr, Mtcr);

  if (PcdGet32(PcdBootMode) == QSPI_BOOT)
    Timeout = 1000000;
  else
    Timeout = 10000;
  while (Timeout > 0 && (Mtcr & BIST_CR_EN)) {
      MicroSecondDelay(1000);
      Timeout--;
      Mtcr = DdrRead32(&Ddr->Mtcr);
  }
  if (Timeout <= 0) {
      DEBUG((EFI_D_RELEASE,"Timeout\n"));
  } else {
      DEBUG((EFI_D_RELEASE,"Done\n"));
  }
  ErrDetect = DdrRead32(&Ddr->ErrDetect);
  ErrSbe = DdrRead32(&Ddr->ErrSbe);
  if (Mtcr & BIST_CR_STAT) {
      DEBUG((EFI_D_RELEASE,"BIST failed on controller %d\n", CtrlNum));
  }
  if (ErrDetect || (ErrSbe & 0xffff)) {
      DEBUG((EFI_D_RELEASE,"detected ECC error on controller %d\n", CtrlNum));
  }

  if (Cs0Config & CTLR_INTLV_MASK) {
      /* restore bnds registers */
      DdrWrite32(&Ddr->Cs0Bnds, Cs0Bnds);
      DdrWrite32(&Ddr->Cs1Bnds, Cs1Bnds);
      DdrWrite32(&Ddr->Cs2Bnds, Cs2Bnds);
      DdrWrite32(&Ddr->Cs3Bnds, Cs3Bnds);
  }
#endif /* DDR_BUILTIN_SELF_TEST */
}


UINT32
GetMemoryClkPeriodPs (
    VOID
  )
{
  UINT64 DataRate = GetDdrFreq();
  UINT32 Result;
  UINT64 Rem, MclkPs = ULL_2E12; // round to nearest 10ps

  Rem = DoDiv(MclkPs, DataRate);
  Result = (Rem >= (DataRate >> 1)) ? MclkPs + 1 : MclkPs;

  return Result;
}


UINT32
ComputeCasLatency (
  IN   CONST DimmParamsT        *DimmParams,
  OUT  CommonTimingParamsT      *Outpdimm,
  IN   UINT32                   NumberOfDimms
  )
{
  UINT32 I, Tmp = ~0;
  UINT32 CommonCasLatency, CasLatencyActual;
  UINT32 Retry = 16;
  CONST UINT32 MclkPs = GetMemoryClkPeriodPs();

#ifdef MEMORY_TYPE_DDR3
  CONST UINT32 Taamax = 20000;
#else
  CONST UINT32 Taamax = 18000;
#endif

  // compute the common CAS latency supported between slots
  Tmp = DimmParams[0].CaslatX;
  for (I = 1; I < NumberOfDimms; I++) {
    if (DimmParams[I].NRanks)
      Tmp &= DimmParams[I].CaslatX;
  }
  CommonCasLatency = Tmp;

  //check if the memory clk is in the range of DIMMs
  if (MclkPs < Outpdimm->TckminXPs) {
    DEBUG((EFI_D_ERROR,"DDR clock (Mclk cycle %d Ps) is faster than "
        "the slowest DIMM(s) (TCKmin %d Ps) can support.\n",
        MclkPs, Outpdimm->TckminXPs));
  }

#ifdef MEMORY_TYPE_DDR4
  if (MclkPs > Outpdimm->TckmaxPs) {
    DEBUG((EFI_D_ERROR,"DDR clock (MCLK cycle %d Ps) is slower than "
          "DIMM(s) (TCKmax %d Ps) can support.\n",
          MclkPs, Outpdimm->TckmaxPs));
  }
#endif

  // determine the acutal cas latency
  CasLatencyActual = (Outpdimm->TaaminPs + MclkPs - 1) / MclkPs;
  // check if the dimms support the CAS latency
  while (!(CommonCasLatency & (1 << CasLatencyActual)) && Retry > 0) {
    CasLatencyActual++;
    Retry--;
  }
  if (CasLatencyActual * MclkPs > Taamax) {
    DEBUG((EFI_D_ERROR,"The choosen cas latency %d is too large\n",
           CasLatencyActual));
  }
  Outpdimm->LowestCommonSpdCaslat = CasLatencyActual;
  DEBUG((EFI_D_INFO, "LowestCommonSpdCaslat is 0x%x\n", CasLatencyActual));

  return 0;
}


UINT32
MclkToPicos (
  IN   UINT32   Mclk
  )
{
  return GetMemoryClkPeriodPs() * Mclk;
}


UINT32
DdrComputeCommonDimmParameters (
  IN   CONST DimmParamsT        *DimmParams,
  OUT  CommonTimingParamsT      *Outpdimm,
  IN   CONST UINT32             NumberOfDimms
  )
{
  UINT32 I, J;

  UINT32 TckminXPs = 0;
  UINT32 TckmaxPs = 0xFFFFFFFF;
  UINT32 TrcdPs = 0;
  UINT32 TrpPs = 0;
  UINT32 TrasPs = 0;
  UINT32 TaaminPs = 0;
#ifdef MEMORY_TYPE_DDR4
  UINT32 TwrPs = 15000;
  UINT32 Trfc1Ps = 0;
  UINT32 Trfc2Ps = 0;
  UINT32 Trfc4Ps = 0;
  UINT32 TrrdsPs = 0;
  UINT32 TrrdlPs = 0;
  UINT32 TccdlPs = 0;
#else
  UINT32 TwrPs = 0;
  UINT32 TwtrPs = 0;
  UINT32 TrfcPs = 0;
  UINT32 TrrdPs = 0;
  UINT32 TrtpPs = 0;
#endif
  UINT32 TrcPs = 0;
  UINT32 RefreshRatePs = 0;
  UINT32 ExtendedOpSrt = 1;
  UINT32 Temp1, Temp2;
  UINT32 AdditiveLatency = 0;

  Temp1 = 0;
  for (I = 0; I < NumberOfDimms; I++) {
    // if no rank on this DIMM, just skip it */
    if (DimmParams[I].NRanks == 0) {
      Temp1++;
      continue;
    }
    if (DimmParams[I].NRanks == 4 && I != 0) {
      DEBUG((EFI_D_RELEASE, "found quad-rank DIMM in wrong bank, ignored\n"));
      Temp1++;
      continue;
    }

#ifndef DDR_FIRST_SLOT_QUAD_CAPABLE
    if (DimmParams[I].NRanks == 4 && \
           DDR_CHIP_SELECTS_PER_CTRL / DDR_DIMM_SLOTS_PER_CTLR < 4) {
      DEBUG((EFI_D_RELEASE,"found quad-rank DIMM, unable to support."));
      Temp1++;
      continue;
    }
#endif

    TckmaxPs = Min(TckmaxPs, DimmParams[I].TckmaxPs);
    TaaminPs = Max(TaaminPs, DimmParams[I].TaaPs);
    TckminXPs = Max(TckminXPs, DimmParams[I].TckminXPs);
    TrcdPs = Max(TrcdPs, DimmParams[I].TrcdPs);
    TrpPs = Max(TrpPs, DimmParams[I].TrpPs);
    TrasPs = Max(TrasPs, DimmParams[I].TrasPs);
#ifdef MEMORY_TYPE_DDR4
    Trfc1Ps = Max(Trfc1Ps, DimmParams[I].Trfc1Ps);
    Trfc2Ps = Max(Trfc2Ps, DimmParams[I].Trfc2Ps);
    Trfc4Ps = Max(Trfc4Ps, DimmParams[I].Trfc4Ps);
    TrrdsPs = Max(TrrdsPs, DimmParams[I].TrrdsPs);
    TrrdlPs = Max(TrrdlPs, DimmParams[I].TrrdlPs);
    TccdlPs = Max(TccdlPs, DimmParams[I].TccdlPs);
#else
    TwrPs = Max(TwrPs, DimmParams[I].TwrPs);
    TwtrPs = Max(TwtrPs, DimmParams[I].TwtrPs);
    TrfcPs = Max(TrfcPs, DimmParams[I].TrfcPs);
    TrrdPs = Max(TrrdPs, DimmParams[I].TrrdPs);
    TrtpPs = Max(TrtpPs, DimmParams[I].TrtpPs);
#endif
    TrcPs = Max(TrcPs, DimmParams[I].TrcPs);
    RefreshRatePs = Max(RefreshRatePs, DimmParams[I].RefreshRatePs);
    ExtendedOpSrt = Min(ExtendedOpSrt, DimmParams[I].ExtendedOpSrt);
  }

  Outpdimm->NdimmsPresent = NumberOfDimms - Temp1;

  if (Temp1 == NumberOfDimms) {
    return 0;
  }

  Outpdimm->TckminXPs = TckminXPs;
  Outpdimm->TckmaxPs = TckmaxPs;
  Outpdimm->TaaminPs = TaaminPs;
  Outpdimm->TrcdPs = TrcdPs;
  Outpdimm->TrpPs = TrpPs;
  Outpdimm->TrasPs = TrasPs;
#ifdef MEMORY_TYPE_DDR4
  Outpdimm->Trfc1Ps = Trfc1Ps;
  Outpdimm->Trfc2Ps = Trfc2Ps;
  Outpdimm->Trfc4Ps = Trfc4Ps;
  Outpdimm->TrrdsPs = TrrdsPs;
  Outpdimm->TrrdlPs = TrrdlPs;
  Outpdimm->TccdlPs = TccdlPs;
#else
  Outpdimm->TwtrPs = TwtrPs;
  Outpdimm->TrfcPs = TrfcPs;
  Outpdimm->TrrdPs = TrrdPs;
  Outpdimm->TrtpPs = TrtpPs;
#endif
  Outpdimm->TwrPs = TwrPs;
  Outpdimm->TrcPs = TrcPs;
  Outpdimm->RefreshRatePs = RefreshRatePs;
  Outpdimm->ExtendedOpSrt = ExtendedOpSrt;

  Temp1 = 0xff;
  for (I = 0; I < NumberOfDimms; I++) {
    if (DimmParams[I].NRanks) {
      Temp1 &= DimmParams[I].BurstLengthsBitmask;
    }
  }
  Outpdimm->AllDimmsBurstLengthsBitmask = Temp1;

  Temp1 = Temp2 = 0;
  for (I = 0; I < NumberOfDimms; I++) {
    if (DimmParams[I].NRanks) {
      if (DimmParams[I].RegisteredDimm) {
        Temp1 = 1;
         DEBUG((EFI_D_RELEASE, "Detected RDIMM %a\n", DimmParams[I].Mpart));
      } else {
        Temp2 = 1;
         DEBUG((EFI_D_RELEASE, "Detected UDIMM %a\n", DimmParams[I].Mpart));
      }
    }
  }

  Outpdimm->AllDimmsRegistered = 0;
  Outpdimm->AllDimmsUnbuffered = 0;
  if (Temp1 && !Temp2) {
    Outpdimm->AllDimmsRegistered= 1;
  } else if (!Temp1 && Temp2) {
    Outpdimm->AllDimmsUnbuffered = 1;
  } else {
    DEBUG((EFI_D_ERROR, "ERROR: mix of registered buffered and unbuffered "
          "DIMMs detected!\n"));
  }

  Temp1 = 0;
  if (Outpdimm->AllDimmsRegistered)
    for (J = 0; J < 16; J++) {
      Outpdimm->Rcw[J] = DimmParams[0].Rcw[J];
      for (I = 1; I < NumberOfDimms; I++) {
           if (!DimmParams[I].NRanks)
             continue;
           if (DimmParams[I].Rcw[J] != DimmParams[0].Rcw[J]) {
             Temp1 = 1;
             break;
           }
      }
    }

  if (Temp1 != 0) {
    DEBUG((EFI_D_INFO,"ERROR: mix different RDIMM detected!\n"));
  }

  if (ComputeCasLatency(DimmParams, Outpdimm, NumberOfDimms)) {
    return 1;
  }

  Temp1 = 1;
  for (I = 0; I < NumberOfDimms; I++) {
    if (DimmParams[I].NRanks &&
             !(DimmParams[I].EdcConfig & EDC_ECC)) {
      Temp1 = 0;
      break;
    }
  }
  if (Temp1) {
    DEBUG((EFI_D_INFO,"all DIMMs ECC capable\n"));
  } else {
    DEBUG((EFI_D_INFO,"Warning: not all DIMMs ECC capable, can't enable ECC\n"));
  }
  Outpdimm->AllDimmsEccCapable = Temp1;
  AdditiveLatency = 0;

  if (MclkToPicos(AdditiveLatency) > TrcdPs) {
    DEBUG((EFI_D_RELEASE,"Error: invalid additive latency exceeds TRCD(Min).\n"));
    return 1;
  }

  Outpdimm->AdditiveLatency = AdditiveLatency;
  DEBUG((EFI_D_INFO,"TCKminPs = %u\n", Outpdimm->TckminXPs));
  DEBUG((EFI_D_INFO,"TrcdPs   = %u\n", Outpdimm->TrcdPs));
  DEBUG((EFI_D_INFO,"TrpPs    = %u\n", Outpdimm->TrpPs));
  DEBUG((EFI_D_INFO,"TrasPs   = %u\n", Outpdimm->TrasPs));
#ifdef MEMORY_TYPE_DDR4
  DEBUG((EFI_D_INFO,"Trfc1Ps = %u\n", Trfc1Ps));
  DEBUG((EFI_D_INFO,"Trfc2Ps = %u\n", Trfc2Ps));
  DEBUG((EFI_D_INFO,"Trfc4Ps = %u\n", Trfc4Ps));
  DEBUG((EFI_D_INFO,"TrrdsPs = %u\n", TrrdsPs));
  DEBUG((EFI_D_INFO,"TrrdlPs = %u\n", TrrdlPs));
  DEBUG((EFI_D_INFO,"TccdlPs = %u\n", TccdlPs));
#else
  DEBUG((EFI_D_INFO,"TwtrPs = %u\n", Outpdimm->TwtrPs));
  DEBUG((EFI_D_INFO,"TrfcPs = %u\n", Outpdimm->TrfcPs));
  DEBUG((EFI_D_INFO,"TrrdPs = %u\n", Outpdimm->TrrdPs));
#endif
  DEBUG((EFI_D_INFO,"TwrPs = %u\n", Outpdimm->TwrPs));
  DEBUG((EFI_D_INFO,"TrcPs = %u\n", Outpdimm->TrcPs));

  return 0;
}


UINT64
DdrCompute (
  IN   DdrInfoT  *Pinfo
  )
{
  UINT32 I = 0, J = 0;
  UINT64 TotalMem = 0;
  INT32 AssertReset = 0;
  UINT32 FirstCtrl =  Pinfo->FirstCtrl;
  UINT32 LastCtrl = FirstCtrl + Pinfo->NumCtrls - 1;
  BOOLEAN GoodSpd;
  DdrCfgRegsT *DdrReg = Pinfo->DdrConfigReg;
  CommonTimingParamsT *TimingParams = Pinfo->CommonTimingParams;
  UINT32 DbwCapacityAdjust[DDR_CTLRS_NUM];

  if (Pinfo->BoardNeedMemReset) {
    AssertReset = BoardNeedMemReset();
  }

  for (I = FirstCtrl; I <= LastCtrl; I++) {
    DbwCapacityAdjust[I] = 0;
  }

#if defined(DDR_SPD_TIMING) || defined(CONFIG_SPD_EEPROM)
  // Step 1: gather all DIMM SPD data
  for (I = FirstCtrl; I <= LastCtrl; I++) {
    DdrReadSpd(Pinfo->SpdInstalledDimms[I], I);
  }

  // Step 2: compute DIMM parameters from SPD data
  GoodSpd = FALSE;
  for (I = FirstCtrl; I <= LastCtrl; I++) {
    for (J = 0; J < DDR_DIMM_SLOTS_PER_CTLR; J++) {
       UINT32 Retval;
       GenericSpdEepromT *Spd = &(Pinfo->SpdInstalledDimms[I][J]);
       DimmParamsT *Pdimm = &(Pinfo->DimmParams[I][J]);

       Retval = DdrComputeDimmParameters(Spd, Pdimm, J);
#ifdef DDR_RAW_TIMING
       if (!J && Retval) {
         DEBUG((EFI_D_INFO,"SPD error on controller %d! "
                "trying fallback to raw timing calculation\n", I));
         DdrGetRawDimmParams(Pdimm, I, J);
       }
#else
       if (Retval == 2) {
         DEBUG((EFI_D_ERROR,"Error: ComputeDimmParameters"
                    " non-zero returned FATAL value "
                    "for Memctl=%d Dimm=%d\n", I, J));
         return 0;
       }
#endif
       if (Retval) {
         DEBUG((EFI_D_INFO, "Warning: ComputeDimmParameters"
                    " non-zero return value for Memctl=%d "
                    "Dimm=%d\n", I, J));
       } else {
         GoodSpd = TRUE;
       }
     }
  }
  if (!GoodSpd) {
     /*
      * no valid SPD found, throw an error if this is for main memory, i.e.
      * FirstCtrl == 0, otherwise, siliently return 0 as the memory size.
      */
     if (FirstCtrl == 0) {
       DEBUG((EFI_D_ERROR, "Error: no valid SPD detected.\n"));
     }
     return 0;
  }
#elif defined(DDR_RAW_TIMING)
  // compute simulated dimm parameters from board-specific file
  for (I = FirstCtrl; I <= LastCtrl; I++) {
    for (J = 0; J < DDR_DIMM_SLOTS_PER_CTLR; J++) {
       DimmParamsT *Pdimm = &(Pinfo->DimmParams[I][J]);
       DdrGetRawDimmParams(Pdimm, I, J);
     }
  }
#endif

  // Step 3: compute common timing parameters suitable for all DIMMs
  for (I = FirstCtrl; I <= LastCtrl; I++) {
    DEBUG((EFI_D_INFO, "Computing lowest common DIMM"
            " parameters for Memctl=%d\n", I));
    DdrComputeCommonDimmParameters(Pinfo->DimmParams[I],
                      &TimingParams[I], DDR_DIMM_SLOTS_PER_CTLR);
  }

  // Step 4: gather option configuration requirements
  for (I = FirstCtrl; I <= LastCtrl; I++) {
     DEBUG((EFI_D_INFO, "Reloading Memory Controller "
            "Configuration Options for Memctl=%d\n", I));
     DdrCfgMemCtrlOptions(&TimingParams[I],
                    &Pinfo->MemctlOpts[I],
                    Pinfo->DimmParams[I], I);
     if (TimingParams[I].AllDimmsRegistered) {
       AssertReset = 1;
     }
  }
  if (AssertReset) {
      if (Pinfo->BoardMemReset) {
       DEBUG((EFI_D_INFO,"Asserting mem reset\n"));
       Pinfo->BoardMemReset();
      } else {
       DEBUG((EFI_D_INFO,"Asserting mem reset missing\n"));
     }
  }

  // Step 5: assign addresses to chip selects
  DdrCheckIntlvOptions(Pinfo);
  TotalMem = DdrAssignAddresses(Pinfo, DbwCapacityAdjust);
  DEBUG((EFI_D_INFO, "Total mem %llu assigned\n", TotalMem));

  // Step 6: compute controller register values
  DEBUG((EFI_D_INFO,"step 6: computing DDRC registers\n"));
  for (I = FirstCtrl; I <= LastCtrl; I++) {
     if (TimingParams[I].NdimmsPresent == 0) {
       InternalMemZeroMem(&DdrReg[I], sizeof(DdrCfgRegsT));
       continue;
     }
     ComputeFslMemctlConfigRegs(&Pinfo->MemctlOpts[I],
            &DdrReg[I], &TimingParams[I], Pinfo->DimmParams[I],
            DbwCapacityAdjust[I]);
  }

  UINT32 MaxEnd = 0;
  for (I = FirstCtrl; I <= LastCtrl ; I++) {
    for (J = 0; J < DDR_CHIP_SELECTS_PER_CTRL; J++) {
      DdrCfgRegsT *Reg = &DdrReg[I];
      if (Reg->Cs[J].Config & 0x80000000) {
           UINT32 End;
           // 0xfffffff is a special value we put for unused Bnds
           if (Reg->Cs[J].Bnds == 0xffffffff)
             continue;
           End = Reg->Cs[J].Bnds & 0xffff;
           if (End > MaxEnd) {
             MaxEnd = End;
           }
      }
    }
  }

  TotalMem = 1 + (((UINT64)MaxEnd << 24ULL) | 0xFFFFFFULL) - Pinfo->MemBase;
  return TotalMem;
}


UINT64
DdrSdramInitialization (
   VOID
  )
{
  UINT32 I = 0, FirstCtrl, LastCtrl;
  UINT64 TotalMemory;
  DdrInfoT Info;
  INT32 DeassertReset = 0;

  InternalMemZeroMem(&Info, sizeof(DdrInfoT));
  Info.MemBase = DDR_SDRAM_BASE_PHY;
  Info.FirstCtrl = 0;
  Info.NumCtrls = DDR_CTLRS_NUM;
  Info.DimmSlotsPerCtrl = DDR_DIMM_SLOTS_PER_CTLR;
  Info.BoardNeedMemReset = BoardNeedMemReset;
  Info.BoardMemReset = BoardAssertMemReset;
  Info.BoardMemDeReset = BoardDeassertMemReset;
  RemoveUnusedControllers(&Info);

  FirstCtrl = Info.FirstCtrl;
  LastCtrl = FirstCtrl + Info.NumCtrls - 1;

  TotalMemory = DdrCompute(&Info);

  if (Info.BoardNeedMemReset) {
    DeassertReset = BoardNeedMemReset();
  }
  for (I = FirstCtrl; I <= LastCtrl; I++) {
    if (Info.CommonTimingParams[I].AllDimmsRegistered) {
      DeassertReset = 1;
    }
  }

  for (I = FirstCtrl; I <= LastCtrl; I++) {
    DEBUG((EFI_D_INFO, "Programming Controller %d\n", I));
    if (Info.CommonTimingParams[I].NdimmsPresent == 0) {
      DEBUG((EFI_D_INFO, "No Dimms Present On Controller %d; "
                         "Skipping Programming\n", I));
      continue;
    }

    DdrSetMemCtrlRegs(&(Info.DdrConfigReg[I]), I, DeassertReset ? 1 : 0);
  }

  if (DeassertReset) {
    if (Info.BoardMemDeReset) {
      DEBUG((EFI_D_INFO,"Deasserting mem reset\n"));
      Info.BoardMemDeReset();
    } else {
      DEBUG((EFI_D_INFO,"Deasserting mem reset missing\n"));
    }
    for (I = FirstCtrl; I <= LastCtrl; I++) {
      DdrSetMemCtrlRegs(&(Info.DdrConfigReg[I]), I, 2);
    }
  }
  DEBUG((EFI_D_INFO, "TotalMemory = %llu\n", TotalMemory));

  return TotalMemory;
}


UINT64
DramInit (
    VOID
  )
{
  UINT64 DramSize = 0;

  DEBUG((EFI_D_RELEASE,"Initializing DDR...\n"));
#ifdef DDR_STATIC_INIT
   DramStaticInit();
#else
   DramSize = DdrSdramInitialization();
#endif
   return DramSize;
}
