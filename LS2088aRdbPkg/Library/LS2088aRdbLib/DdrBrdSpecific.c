/** @DdrBrdSpecific.c

  LS2088aRDB Board Specific DDR timing parameters

  Copyright (c) 2015, Freescale Ltd. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/FslDdr.h>
#include <LS2088aRdb.h>

//Data rate should be in ascending order
static const struct BoardSpecificParameters UDimm0[] = {
  /**
      memory controller 0
        num|  hi| rank|  clk| wrlvl | wrlvl | wrlvl
      ranks| mhz| GB  |adjst| start | ctl2  | ctl3
   **/
        {2,  1350,  0,   8,      6,  0x0708090B, 0x0C0D0E09,},
        {2,  1666,  0,   10,     9,  0x0B0C0E11, 0x0F11110C,},
        {2,  1900,  0,   12,   0xA,  0x0B0C0E11, 0x1214140F,},
        {2,  2300,  0,   12,   0xB,  0x0C0D0F12, 0x14161610,},
        {}
};

static const struct BoardSpecificParameters RDimm0[] = {
  /**
      memory controller 0
        num|  hi| rank|  clk| wrlvl | wrlvl | wrlvl
      ranks| mhz| GB  |adjst| start | ctl2  | ctl3
   **/
        {2,  1350,  0,  8,   6,  0x0708090B, 0x0C0D0E09,},
        {2,  1666,  0,  8,   7,  0x08090A0C, 0x0D0F100B,},
        {2,  1900,  0,  8,   7,  0x09090B0D, 0x0E10120B,},
        {2,  2200,  0,  8,   8,  0x090A0C0F, 0x1012130C,},
        {}
};

static const struct BoardSpecificParameters *UDimm[] = {
        UDimm0,
        UDimm0,
};

static const struct BoardSpecificParameters *RDimm[] = {
        RDimm0,
        RDimm0,
};


VOID
DdrBoardOptions (
  OUT   MemctlOptionsT  *Popts,
  IN    DimmParamsT     *Pdimm
 )
{
  CONST struct BoardSpecificParameters *Pbsp, *PbspHighest = NULL;
  UINTN DdrFreq;
  INT32 Slot;

  for (Slot = 0; Slot < DDR_DIMM_SLOTS_PER_CTLR; Slot++) {
      if (Pdimm[Slot].NRanks)
	  break;
  }

  if (Slot >= DDR_DIMM_SLOTS_PER_CTLR)
     return;

  if (Popts->RegisteredDimmEn)
    Pbsp = RDimm[0];
  else
    Pbsp = UDimm[0];

  DdrFreq = GetDdrFreq() / 1000000;
  while (Pbsp->DatarateMhzHigh) {
    if (Pbsp->NRanks == Pdimm->NRanks && (Pdimm->RankDensity >> 30) >= Pbsp->RankGb) {
      if (DdrFreq <= Pbsp->DatarateMhzHigh) {
         Popts->ClkAdjust = Pbsp->ClkAdjust;
         Popts->WrlvlStart = Pbsp->WrlvlStart;
         Popts->WrlvlCtl2 = Pbsp->WrlvlCtl2;
         Popts->WrlvlCtl3 = Pbsp->WrlvlCtl3;
         goto Found;
      }
      PbspHighest = Pbsp;
    }
    Pbsp++;
  }

  if (PbspHighest) {
    Popts->ClkAdjust = PbspHighest->ClkAdjust;
    Popts->WrlvlStart = PbspHighest->WrlvlStart;
    Popts->WrlvlCtl2 = Pbsp->WrlvlCtl2;
    Popts->WrlvlCtl3 = Pbsp->WrlvlCtl3;
  } else {
    DEBUG((EFI_D_INFO,"DIMM is not supported by this board"));
  }

Found:
  DEBUG ((EFI_D_INFO, "Found timing match: n_ranks %d, data rate %d, rank_gb %d\n"
	"\tclk_adjust %d, wrlvl_start %d, wrlvl_ctrl_2 0x%x, wrlvl_ctrl_3 0x%x\n",
	Pbsp->NRanks, Pbsp->DatarateMhzHigh, Pbsp->RankGb,
	Pbsp->ClkAdjust, Pbsp->WrlvlStart, Pbsp->WrlvlCtl2,
	Pbsp->WrlvlCtl3));

  // to work at higher than 1333MT/s
  Popts->HalfStrengthDriverEnable = 0;

  Popts->WrlvlOverride = 1;  // write leveling override
  Popts->WrlvlSample = 0x0;  // 32 clocks

  // Rtt and Rtt_WR override
  Popts->RttOverride = 0;

  // enable ZQ calibration
  Popts->ZqEn = 1;

  if (DdrFreq < 2350) {
    if (Pdimm[0].NRanks == 2 && Pdimm[1].NRanks == 2) {
    	/* four chip-selects */
      Popts->DdrCdr1 = DDRCDR1_DHC_EN | DDRCDR1_ODT(DDRCDR_ODT_80ohm);
      Popts->DdrCdr2 = DDRCDR2_ODT(DDRCDR_ODT_80ohm);
      Popts->TwotEn = 1;	/* enable 2T timing */
    } else {
      Popts->DdrCdr1 = DDRCDR1_DHC_EN | DDRCDR1_ODT(DDRCDR_ODT_80ohm);
      Popts->DdrCdr2 = DDRCDR2_ODT(DDRCDR_ODT_80ohm) | DDRCDR2_VREF_RANGE_2;
    }
  } else {
      Popts->DdrCdr1 = DDRCDR1_DHC_EN | DDRCDR1_ODT(DDRCDR_ODT_100ohm);
      Popts->DdrCdr2 = DDRCDR2_ODT(DDRCDR_ODT_100ohm) | DDRCDR2_VREF_RANGE_2;
  }

  DEBUG((EFI_D_INFO," Popts->DdrCdr1 0x%x \n", Popts->DdrCdr1));
  DEBUG((EFI_D_INFO," Popts->DdrCdr2 0x%x \n", Popts->DdrCdr2));
}

UINT64
GetDdrFreq (
  IN  VOID
  )
{
  UINT64 DdrClk = 0;

  DdrClk = PcdGet64(PcdDdrClk);
  return DdrClk * 14;
}
