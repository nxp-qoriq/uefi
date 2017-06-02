/** @DdrLib.c
  Ddr Library containing functions to initialize ddr controller

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <LS1043aSocLib.h>
#include <Ddr.h>
#include <I2c.h>
#include <Library/BaseLib.h>

/**
  Function to dump DDRC registers

**/

VOID
DdrRegDump (
  VOID
  )
{

  struct CcsrDdr *Ddr = (VOID *)CONFIG_SYS_FSL_DDR_ADDR;

  DEBUG((EFI_D_INFO, "Cs0Bnds = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Cs0Bnds)));
  DEBUG((EFI_D_INFO, "Cs1Bnds = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Cs1Bnds)));
  DEBUG((EFI_D_INFO, "Cs2Bnds = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Cs2Bnds)));
  DEBUG((EFI_D_INFO, "Cs3Bnds = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Cs3Bnds)));
  DEBUG((EFI_D_INFO, "Cs0Config = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Cs0Config)));
  DEBUG((EFI_D_INFO, "Cs1Config = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Cs1Config)));
  DEBUG((EFI_D_INFO, "Cs2Config = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Cs2Config)));
  DEBUG((EFI_D_INFO, "Cs3Config = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Cs3Config)));
  DEBUG((EFI_D_INFO, "Cs0Config2 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Cs0Config2)));
  DEBUG((EFI_D_INFO, "Cs1Config2 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Cs1Config2)));
  DEBUG((EFI_D_INFO, "Cs2Config2 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Cs2Config2)));
  DEBUG((EFI_D_INFO, "Cs3Config2 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Cs3Config2)));
  DEBUG((EFI_D_INFO, "TimingCfg3 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->TimingCfg3)));
  DEBUG((EFI_D_INFO, "TimingCfg0 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->TimingCfg0)));
  DEBUG((EFI_D_INFO, "TimingCfg1 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->TimingCfg1)));
  DEBUG((EFI_D_INFO, "TimingCfg2 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->TimingCfg2)));
  DEBUG((EFI_D_INFO, "SdramCfg = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramCfg)));
  DEBUG((EFI_D_INFO, "SdramCfg2 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramCfg2)));
  DEBUG((EFI_D_INFO, "SdramMode = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramMode)));
  DEBUG((EFI_D_INFO, "SdramMode2 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramMode2)));
  DEBUG((EFI_D_INFO, "SdramMdCntl = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramMdCntl)));
  DEBUG((EFI_D_INFO, "SdramInterval = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramInterval)));
  DEBUG((EFI_D_INFO, "SdramDataInit = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramDataInit)));
  DEBUG((EFI_D_INFO, "SdramClkCntl = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramClkCntl)));
  DEBUG((EFI_D_INFO, "InitAddr = 0x%x\n",MmioReadBe32((UINTN)&Ddr->InitAddr)));
  DEBUG((EFI_D_INFO, "InitExtAddr = 0x%x\n",MmioReadBe32((UINTN)&Ddr->InitExtAddr)));
  DEBUG((EFI_D_INFO, "TimingCfg4 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->TimingCfg4)));
  DEBUG((EFI_D_INFO, "TimingCfg5 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->TimingCfg5)));
  DEBUG((EFI_D_INFO, "TimingCfg6 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->TimingCfg6)));
  DEBUG((EFI_D_INFO, "TimingCfg7 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->TimingCfg7)));
  DEBUG((EFI_D_INFO, "DdrZqCntl = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DdrZqCntl)));
  DEBUG((EFI_D_INFO, "DdrWrlvlCntl = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DdrWrlvlCntl)));
  DEBUG((EFI_D_INFO, "DdrSrCntr = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DdrSrCntr)));
  DEBUG((EFI_D_INFO, "DdrSdramRcw1 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DdrSdramRcw1)));
  DEBUG((EFI_D_INFO, "DdrSdramRcw2 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DdrSdramRcw2)));
  DEBUG((EFI_D_INFO, "DdrWrlvlCntl2 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DdrWrlvlCntl2)));
  DEBUG((EFI_D_INFO, "DdrWrlvlCntl3 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DdrWrlvlCntl3)));
  DEBUG((EFI_D_INFO, "DdrSdramRcw3 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DdrSdramRcw3)));
  DEBUG((EFI_D_INFO, "DdrSdramRcw4 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DdrSdramRcw4)));
  DEBUG((EFI_D_INFO, "DdrSdramRcw5 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DdrSdramRcw5)));
  DEBUG((EFI_D_INFO, "DdrSdramRcw6 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DdrSdramRcw6)));
  DEBUG((EFI_D_INFO, "SdramMode3 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramMode3)));
  DEBUG((EFI_D_INFO, "SdramMode4 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramMode4)));
  DEBUG((EFI_D_INFO, "SdramMode5 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramMode5)));
  DEBUG((EFI_D_INFO, "SdramMode6 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramMode6)));
  DEBUG((EFI_D_INFO, "SdramMode7 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramMode7)));
  DEBUG((EFI_D_INFO, "SdramMode8 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramMode8)));
  DEBUG((EFI_D_INFO, "SdramMode9 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramMode9)));
  DEBUG((EFI_D_INFO, "SdramMode10 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramMode10)));
  DEBUG((EFI_D_INFO, "SdramMode11 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramMode11)));
  DEBUG((EFI_D_INFO, "SdramMode12 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramMode12)));
  DEBUG((EFI_D_INFO, "SdramMode13 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramMode13)));
  DEBUG((EFI_D_INFO, "SdramMode14 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramMode14)));
  DEBUG((EFI_D_INFO, "SdramMode15 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramMode15)));
  DEBUG((EFI_D_INFO, "SdramMode16 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramMode16)));
  DEBUG((EFI_D_INFO, "TimingCfg8 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->TimingCfg8)));
  DEBUG((EFI_D_INFO, "SdramCfg3 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->SdramCfg3)));
  DEBUG((EFI_D_INFO, "DqMap0 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DqMap0)));
  DEBUG((EFI_D_INFO, "DqMap1 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DqMap1)));
  DEBUG((EFI_D_INFO, "DqMap2 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DqMap2)));
  DEBUG((EFI_D_INFO, "DqMap3 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DqMap3)));
  DEBUG((EFI_D_INFO, "DdrDsr1 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DdrDsr1)));
  DEBUG((EFI_D_INFO, "DdrDsr2 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DdrDsr2)));
  DEBUG((EFI_D_INFO, "DdrCdr1 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DdrCdr1)));
  DEBUG((EFI_D_INFO, "DdrCdr2 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DdrCdr2)));
  DEBUG((EFI_D_INFO, "IpRev1 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->IpRev1)));
  DEBUG((EFI_D_INFO, "IpRev2 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->IpRev2)));
  DEBUG((EFI_D_INFO, "Eor = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Eor)));
  DEBUG((EFI_D_INFO, "Mtcr = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Mtcr)));
  DEBUG((EFI_D_INFO, "Mtp1 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Mtp1)));
  DEBUG((EFI_D_INFO, "Mtp2 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Mtp2)));
  DEBUG((EFI_D_INFO, "Mtp3 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Mtp3)));
  DEBUG((EFI_D_INFO, "Mtp4 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Mtp4)));
  DEBUG((EFI_D_INFO, "Mtp5 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Mtp5)));
  DEBUG((EFI_D_INFO, "Mtp6 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Mtp6)));
  DEBUG((EFI_D_INFO, "Mtp7 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Mtp7)));
  DEBUG((EFI_D_INFO, "Mtp8 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Mtp8)));
  DEBUG((EFI_D_INFO, "Mtp9 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Mtp9)));
  DEBUG((EFI_D_INFO, "Mtp10 = 0x%x\n",MmioReadBe32((UINTN)&Ddr->Mtp10)));
  DEBUG((EFI_D_INFO, "DataErrInjectHi = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DataErrInjectHi)));
  DEBUG((EFI_D_INFO, "DataErrInjectLo = 0x%x\n",MmioReadBe32((UINTN)&Ddr->DataErrInjectLo)));
  DEBUG((EFI_D_INFO, "EccErrInject = 0x%x\n",MmioReadBe32((UINTN)&Ddr->EccErrInject)));
  DEBUG((EFI_D_INFO, "CaptureDataHi = 0x%x\n",MmioReadBe32((UINTN)&Ddr->CaptureDataHi)));
  DEBUG((EFI_D_INFO, "CaptureDataLo = 0x%x\n",MmioReadBe32((UINTN)&Ddr->CaptureDataLo)));
  DEBUG((EFI_D_INFO, "CaptureEcc = 0x%x\n",MmioReadBe32((UINTN)&Ddr->CaptureEcc)));
  DEBUG((EFI_D_INFO, "ErrDetect = 0x%x\n",MmioReadBe32((UINTN)&Ddr->ErrDetect)));
  DEBUG((EFI_D_INFO, "ErrDisable = 0x%x\n",MmioReadBe32((UINTN)&Ddr->ErrDisable)));
  DEBUG((EFI_D_INFO, "ErrIntEn = 0x%x\n",MmioReadBe32((UINTN)&Ddr->ErrIntEn)));
  DEBUG((EFI_D_INFO, "CaptureAttributes = 0x%x\n",MmioReadBe32((UINTN)&Ddr->CaptureAttributes)));
  DEBUG((EFI_D_INFO, "CaptureAddress = 0x%x\n",MmioReadBe32((UINTN)&Ddr->CaptureAddress)));
  DEBUG((EFI_D_INFO, "CaptureExtAddress = 0x%x\n",MmioReadBe32((UINTN)&Ddr->CaptureExtAddress)));
  DEBUG((EFI_D_INFO, "ErrSbe = 0x%x\n",MmioReadBe32((UINTN)&Ddr->ErrSbe)));
  DEBUG((EFI_D_INFO,"\n"));
}

VOID
RunBistTest (
  IN VOID
  )
{
#define BIST_PATTERN1   0xFFFFFFFF
#define BIST_PATTERN2   0x0
#define BIST_CR         0x80010000
#define BIST_CR_EN      0x80000000
#define BIST_CR_STAT    0x00000001
#define CTLR_INTLV_MASK 0x20000000

  UINT32 Mtcr = 0, ErrDetect = 0, ErrSbe = 0;
  UINT32 Timeout = 0;
  UINT32 Cs0Bnds = 0, Cs1Bnds = 0, Cs2Bnds = 0, Cs3Bnds = 0, Cs0Config = 0;

  struct CcsrDdr *Ddr = (VOID *)CONFIG_SYS_FSL_DDR_ADDR;

  DEBUG((EFI_D_RELEASE,"Running DDR BIST test..."));

  Cs0Config = MmioReadBe32((UINTN)&Ddr->Cs0Config);
  if (Cs0Config & CTLR_INTLV_MASK) {
      Cs0Bnds = MmioReadBe32((UINTN)&Ddr->Cs0Bnds);
      Cs1Bnds = MmioReadBe32((UINTN)&Ddr->Cs1Bnds);
      Cs2Bnds = MmioReadBe32((UINTN)&Ddr->Cs2Bnds);
      Cs3Bnds = MmioReadBe32((UINTN)&Ddr->Cs3Bnds);
      /* set bnds to non-interleaving */
      MmioWriteBe32((UINTN)&Ddr->Cs0Bnds, (Cs0Bnds & 0xfffefffe) >> 1);
      MmioWriteBe32((UINTN)&Ddr->Cs1Bnds, (Cs1Bnds & 0xfffefffe) >> 1);
      MmioWriteBe32((UINTN)&Ddr->Cs2Bnds, (Cs2Bnds & 0xfffefffe) >> 1);
      MmioWriteBe32((UINTN)&Ddr->Cs3Bnds, (Cs3Bnds & 0xfffefffe) >> 1);
  }
  MmioWriteBe32((UINTN)&Ddr->Mtp1, BIST_PATTERN1);
  MmioWriteBe32((UINTN)&Ddr->Mtp2, BIST_PATTERN1);
  MmioWriteBe32((UINTN)&Ddr->Mtp3, BIST_PATTERN2);
  MmioWriteBe32((UINTN)&Ddr->Mtp4, BIST_PATTERN2);
  MmioWriteBe32((UINTN)&Ddr->Mtp5, BIST_PATTERN1);
  MmioWriteBe32((UINTN)&Ddr->Mtp6, BIST_PATTERN1);
  MmioWriteBe32((UINTN)&Ddr->Mtp7, BIST_PATTERN2);
  MmioWriteBe32((UINTN)&Ddr->Mtp8, BIST_PATTERN2);
  MmioWriteBe32((UINTN)&Ddr->Mtp9, BIST_PATTERN1);
  MmioWriteBe32((UINTN)&Ddr->Mtp10, BIST_PATTERN2);
  Mtcr = BIST_CR;
  MmioWriteBe32((UINTN)&Ddr->Mtcr, Mtcr);

  Timeout = 1000;

  while (Timeout > 0 && (Mtcr & BIST_CR_EN)) {
      MicroSecondDelay(10000);
      Timeout--;
      Mtcr = MmioReadBe32((UINTN)&Ddr->Mtcr);
  }
  if (Timeout <= 0) {
      DEBUG((EFI_D_RELEASE,"Timeout\n"));
  } else {
      DEBUG((EFI_D_RELEASE,"Done\n"));
  }
  ErrDetect = MmioReadBe32((UINTN)&Ddr->ErrDetect);
  ErrSbe = MmioReadBe32((UINTN)&Ddr->ErrSbe);
  if (Mtcr & BIST_CR_STAT) {
      DEBUG((EFI_D_RELEASE,"BIST FAILED!!! \n"));
  }
  if (ErrDetect || (ErrSbe & 0xffff)) {
      DEBUG((EFI_D_RELEASE,"Detected ECC Error!!! \n"));
  }

  if (Cs0Config & CTLR_INTLV_MASK) {
      /* restore bnds registers */
      MmioWriteBe32((UINTN)&Ddr->Cs0Bnds, Cs0Bnds);
      MmioWriteBe32((UINTN)&Ddr->Cs1Bnds, Cs1Bnds);
      MmioWriteBe32((UINTN)&Ddr->Cs2Bnds, Cs2Bnds);
      MmioWriteBe32((UINTN)&Ddr->Cs3Bnds, Cs3Bnds);
  }
}

UINT64
GetDdrFreq (
  IN  VOID
  )
{
  UINT64 DdrClk = 0;

  DdrClk = CONFIG_DDR_CLK_FREQ;

  return DdrClk * 16;
}

VOID
FixCpoValue (
  IN  VOID
  )
{

  struct CcsrDdr *Ddr;
  Ddr = (VOID *)CONFIG_SYS_FSL_DDR_ADDR;

  UINT32 Value, Lane0, Lane1, Debug29, DesiredValue;
  UINT32 Cfg, I, Temp, Lanes, DdrType;
  BOOLEAN NeedUpdate = FALSE, EccUsed = FALSE;

  UINT32 Min = MmioReadBe32((UINTN)&Ddr->Debug[9]) >> 24;
  UINT32 Max = Min;

  Cfg = MmioReadBe32((UINTN)&Ddr->SdramCfg);
  if (Cfg & SDRAM_16_BE)
    Lanes = 2;
  else if (Cfg & SDRAM_32_BE)
    Lanes = 4;
  else
    Lanes = 8;

  if (Cfg & SDRAM_ECC_EN)
    EccUsed = TRUE;

  DdrType = (Cfg & SDRAM_TYPE_MASK) >> SDRAM_TYPE_SHIFT;

  DEBUG((EFI_D_INFO, "EccUsed %d DdrType %d \n", EccUsed, DdrType));

  /* determine max and min CPO values */
  for (I = 9; I < 9 + Lanes / 2; I++) {
    Value = MmioReadBe32((UINTN)&Ddr->Debug[I]);
    Lane0 = Value >> 24;
    Lane1 = (Value >> 8) & 0xff;

    Temp = Max(Lane0, Lane1);
    if (Temp > Max)
      Max = Temp;

    Temp = Min(Lane0, Lane1);
    if (Temp < Min)
      Min = Temp;
  }

  if (EccUsed) {
    Value = MmioReadBe32((UINTN)&Ddr->Debug[13]);
    Value = Value >> 24;

    if (Value > Max)
      Max = Value;

    if (Value < Min)
      Min = Value;
  }

  DEBUG((EFI_D_INFO, "Max = 0x%x, Min = 0x%x\n", Max, Min));

  Debug29 = MmioReadBe32((UINTN)&Ddr->Debug[28]) & 0xff;
  DesiredValue = ((Max + Min) >> 1) + 0x27;

  if (DdrType == DDR4)
         NeedUpdate = (Min + 0x3b) < Debug29 ? TRUE : FALSE;
  else if (DdrType == DDR3)
         NeedUpdate = (Min + 0x3f) < Debug29 ? TRUE : FALSE;

  DEBUG((EFI_D_INFO, "DesiredValue = 0x%x, Debug29 = 0x%x, NeedUpdate %d\n",
					DesiredValue, Debug29, NeedUpdate));

  if (NeedUpdate) {
    MmioWriteBe32((UINTN)&Ddr->Debug[28], DesiredValue);
    DEBUG((EFI_D_INFO, "Debug29 = 0x%x\n", MmioReadBe32((UINTN)&Ddr->Debug[28]) & 0xff));
  }
}

VOID
ErratumA009660 (
  IN  VOID
  )
{
  UINTN ScfgReg = 0x0157020C;

  MmioWriteBe32(ScfgReg, 0x63B20042);
}

/**
  Function to initialize DDR
 **/
VOID
DramInit (
  )
{
  struct CcsrDdr *Ddr;
  Ddr = (VOID *)CONFIG_SYS_FSL_DDR_ADDR;

  if (PcdGetBool(PcdDdrErratumA009660))
     ErratumA009660();

  MmioWriteBe32((UINTN)&Ddr->Eor, CONFIG_DDR_EOR);

  MmioWriteBe32((UINTN)&Ddr->SdramCfg, CONFIG_DDR_SDRAM_CFG);

  MmioWriteBe32((UINTN)&Ddr->Cs0Bnds, CONFIG_CS0_BNDS);
  MmioWriteBe32((UINTN)&Ddr->Cs0Config, CONFIG_CS0_CONFIG);

  MmioWriteBe32((UINTN)&Ddr->TimingCfg0, CONFIG_TIMING_CFG_0);
  MmioWriteBe32((UINTN)&Ddr->TimingCfg1, CONFIG_TIMING_CFG_1);
  MmioWriteBe32((UINTN)&Ddr->TimingCfg2, CONFIG_TIMING_CFG_2);
  MmioWriteBe32((UINTN)&Ddr->TimingCfg3, CONFIG_TIMING_CFG_3);
  MmioWriteBe32((UINTN)&Ddr->TimingCfg4, CONFIG_TIMING_CFG_4);
  MmioWriteBe32((UINTN)&Ddr->TimingCfg5, CONFIG_TIMING_CFG_5);
  MmioWriteBe32((UINTN)&Ddr->TimingCfg7, CONFIG_TIMING_CFG_7);
  MmioWriteBe32((UINTN)&Ddr->TimingCfg8, CONFIG_TIMING_CFG_8);

  MmioWriteBe32((UINTN)&Ddr->SdramCfg2, CONFIG_DDR_SDRAM_CFG_2);

  MmioWriteBe32((UINTN)&Ddr->SdramMode, CONFIG_DDR_SDRAM_MODE);
  MmioWriteBe32((UINTN)&Ddr->SdramMode2, 0);

  MmioWriteBe32((UINTN)&Ddr->SdramMode3, CONFIG_DDR_SDRAM_MODE_3);
  MmioWriteBe32((UINTN)&Ddr->SdramMode4, 0);
  MmioWriteBe32((UINTN)&Ddr->SdramMode5, CONFIG_DDR_SDRAM_MODE_5);
  MmioWriteBe32((UINTN)&Ddr->SdramMode6, 0);
  MmioWriteBe32((UINTN)&Ddr->SdramMode7, CONFIG_DDR_SDRAM_MODE_7);
  MmioWriteBe32((UINTN)&Ddr->SdramMode8, 0);
  MmioWriteBe32((UINTN)&Ddr->SdramMode9, CONFIG_DDR_SDRAM_MODE_9);
  MmioWriteBe32((UINTN)&Ddr->SdramMode10, CONFIG_DDR_SDRAM_MODE_10);
  MmioWriteBe32((UINTN)&Ddr->SdramMode11, CONFIG_DDR_SDRAM_MODE_11);
  MmioWriteBe32((UINTN)&Ddr->SdramMode12, CONFIG_DDR_SDRAM_MODE_12);
  MmioWriteBe32((UINTN)&Ddr->SdramMode13, CONFIG_DDR_SDRAM_MODE_13);
  MmioWriteBe32((UINTN)&Ddr->SdramMode14, CONFIG_DDR_SDRAM_MODE_14);
  MmioWriteBe32((UINTN)&Ddr->SdramMode15, CONFIG_DDR_SDRAM_MODE_15);
  MmioWriteBe32((UINTN)&Ddr->SdramMode16, CONFIG_DDR_SDRAM_MODE_16);

  if (PcdGetBool(PcdDdrErratumA009663))
    MmioWriteBe32((UINTN)&Ddr->SdramInterval, CONFIG_DDR_SDRAM_INTERVAL & ~SDRAM_INTERVAL_BSTOPRE);
  else
    MmioWriteBe32((UINTN)&Ddr->SdramInterval, CONFIG_DDR_SDRAM_INTERVAL);

  MmioWriteBe32((UINTN)&Ddr->SdramDataInit, CONFIG_DDR_INIT_DATA);

  MmioWriteBe32((UINTN)&Ddr->DdrWrlvlCntl, CONFIG_DDR_WRLVL_CNTL);
  MmioWriteBe32((UINTN)&Ddr->DdrWrlvlCntl2, CONFIG_DDR_WRLVL_CNTL_2);
  MmioWriteBe32((UINTN)&Ddr->DdrWrlvlCntl3, 0);

  MmioWriteBe32((UINTN)&Ddr->DdrCdr1, CONFIG_DDRCDR_1);
  MmioWriteBe32((UINTN)&Ddr->DdrCdr2, CONFIG_DDRCDR_2);

  MmioWriteBe32((UINTN)&Ddr->SdramClkCntl, CONFIG_DDR_SDRAM_CLK_CNTL);

  MmioWriteBe32((UINTN)&Ddr->DdrZqCntl, CONFIG_DDR_ZQ_CNTL);

  MmioWriteBe32((UINTN)&Ddr->SdramMode9, CONFIG_DDR_SDRAM_MODE_9);
  MmioWriteBe32((UINTN)&Ddr->SdramMode10, CONFIG_DDR_SDRAM_MODE_10);

  MmioWriteBe32((UINTN)&Ddr->Cs0Config2, 0);

  if (PcdGetBool(PcdDdrErratumA009442)) {
    UINT64 DdrFreq = GetDdrFreq() / 1000000;
    UINT32 Temp = 0;

    Temp = MmioReadBe32((UINTN)&Ddr->Debug[28]);

    if (DdrFreq <= 1333)
      MmioWriteBe32((UINTN)&Ddr->Debug[28], Temp | 0x0080006a);
    else if (DdrFreq <= 1600)
      MmioWriteBe32((UINTN)&Ddr->Debug[28], Temp | 0x0070006f);
    else if (DdrFreq <= 1867)
      MmioWriteBe32((UINTN)&Ddr->Debug[28], Temp | 0x00700076);
    else if (DdrFreq <= 2133)
      MmioWriteBe32((UINTN)&Ddr->Debug[28], Temp | 0x0060007b);
  }

  MicroSecondDelay(1000);

  MmioWriteBe32((UINTN)&Ddr->SdramCfg, CONFIG_DDR_SDRAM_CFG
                     | CONFIG_DDR_SDRAM_CFG_MEM_EN);

  if (PcdGetBool(PcdDdrErratumA009663))
    MmioWriteBe32((UINTN)&Ddr->SdramInterval, CONFIG_DDR_SDRAM_INTERVAL);

  if (PcdGetBool(PcdDdrErratumA009442))
    FixCpoValue();

  if (PcdGetBool(PcdDdrBistTest))
    RunBistTest();

  return;
}
