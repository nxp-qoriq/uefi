/** @QixisLib.c
  QIXIS FPGA/CPLD module common Library for QorIQ boards, containing
  functions to program and read the FPGA/CPLD registers.

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <PiPei.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PrintLib.h>
#include <Library/SerialPortLib.h>
#include <QixisLib.h>
#include <Library/FslIfc.h>
#include <I2c.h>

UINTN
GetQixisAddr (
  VOID
  )
{
   UINTN Addr;

   Addr = QIXIS_BASE_PHYS;

   /*
    * IFC address under 256MB is mapped to 0x30000000, any address above
    * is mapped to 0x5_10000000 up to 4GB.
    */
   Addr = Addr  > 0x10000000 ? Addr + 0x500000000ULL : Addr + 0x30000000;

   return Addr;
}

UINT8
QixisRead (
  IN UINTN Reg
  )
{
  UINT8 Val = 0;

  if (PcdGet32(PcdBootMode) == QSPI_BOOT) {
    I2cDataRead((VOID *)I2C1_BASE_ADDRESS, 0x66, Reg, 1, &Val, 1);
  } else {
    VOID *Base = (VOID *)QIXIS_BASE;
    Val = MmioRead8((UINTN)(Base + Reg));
  }

  return Val;
}

VOID
QixisWrite (
  IN UINTN Reg,
  IN UINT8 Value
  )
{
  if (PcdGet32(PcdBootMode) == QSPI_BOOT) {
    UINT8 Val = Value;
    I2cDataWrite((VOID *)I2C1_BASE_ADDRESS, 0x66, Reg, 1, &Val, 1);
  } else {
    VOID *Base = (VOID *)QIXIS_BASE;
    MmioWrite8((UINTN)(Base + Reg), Value);
  }
}

/* Set the boot bank to the alternate bank */
VOID
QixisSetAlternatebank (
  VOID
  )
{
  QIXIS_WRITE(rcfg_ctl, QIXIS_RCFG_CTL_RECONFIG_IDLE);
  QIXIS_WRITE(rcfg_ctl, QIXIS_RCFG_CTL_RECONFIG_START);
}


/* Set the boot bank to the default bank */
VOID
QixisSetDefaultBank (
  VOID
  )
{
  QIXIS_WRITE(rst_ctl, QIXIS_RST_CTL_RESET);
}

VOID
QixisDumpRegs (
  VOID
  )
{
  INT32 i;

  DEBUG((EFI_D_INFO, "id      = %02x\n", QIXIS_READ(id)));
  DEBUG((EFI_D_INFO, "arch    = %02x\n", QIXIS_READ(arch)));
  DEBUG((EFI_D_INFO, "scver   = %02x\n", QIXIS_READ(scver)));
  DEBUG((EFI_D_INFO, "model   = %02x\n", QIXIS_READ(model)));
  DEBUG((EFI_D_INFO, "rst_ctl = %02x\n", QIXIS_READ(rst_ctl)));
  DEBUG((EFI_D_INFO, "aux     = %02x\n", QIXIS_READ(aux)));
  for (i = 0; i < 16; i++)
    DEBUG((EFI_D_INFO, "brdcfg%02d = %02x\n", i, QIXIS_READ(brdcfg[i])));
  for (i = 0; i < 16; i++)
    DEBUG((EFI_D_INFO, "dutcfg%02d = %02x\n", i, QIXIS_READ(dutcfg[i])));
  DEBUG((EFI_D_INFO, "sclk    = %02x%02x%02x\n", QIXIS_READ(sclk[0]),
    QIXIS_READ(sclk[1]), QIXIS_READ(sclk[2])));
  DEBUG((EFI_D_INFO, "dclk    = %02x%02x%02x\n", QIXIS_READ(dclk[0]),
    QIXIS_READ(dclk[1]), QIXIS_READ(dclk[2])));
  DEBUG((EFI_D_INFO, "aux     = %02x\n", QIXIS_READ(aux)));
  DEBUG((EFI_D_INFO, "watch   = %02x\n", QIXIS_READ(watch)));
  DEBUG((EFI_D_INFO, "ctl_sys = %02x\n", QIXIS_READ(ctl_sys)));
  DEBUG((EFI_D_INFO, "rcw_ctl = %02x\n", QIXIS_READ(rcw_ctl)));
  DEBUG((EFI_D_INFO, "present = %02x\n", QIXIS_READ(present)));
  DEBUG((EFI_D_INFO, "present2 = %02x\n", QIXIS_READ(present2)));
  DEBUG((EFI_D_INFO, "clk_spd = %02x\n", QIXIS_READ(clk_spd)));
  DEBUG((EFI_D_INFO, "stat_dut = %02x\n", QIXIS_READ(stat_dut)));
  DEBUG((EFI_D_INFO, "stat_sys = %02x\n", QIXIS_READ(stat_sys)));
  DEBUG((EFI_D_INFO, "stat_alrm = %02x\n", QIXIS_READ(stat_alrm)));
}

VOID
DoQixis (
  IN QixisCmd Cmd
  )
{
  switch (Cmd) {
  case RESET:
    QixisSetDefaultBank();
    break;
  case RESET_ALTBANK:
    QixisSetAlternatebank();
    break;
  case DUMP_REGISTERS:
    QixisDumpRegs();
    break;
  default:
    DEBUG((EFI_D_ERROR, "Error: Unknown Qixis Command!\n"));
    break;
  }
}

VOID
QixisInit (
  VOID
  )
{
  MmioWrite32((UINTN)&(FSL_IFC_REGS_BASE)->cspr_cs[FSL_IFC_CS3].cspr_ext,
                      FSL_IFC_QIXIS_CSPR_EXT);
  MmioWrite32((UINTN)&(FSL_IFC_REGS_BASE)->ftim_cs[FSL_IFC_CS3].ftim[FSL_IFC_FTIM0],
                      FSL_IFC_QIXIS_FTIM0);
  MmioWrite32((UINTN)&(FSL_IFC_REGS_BASE)->ftim_cs[FSL_IFC_CS3].ftim[FSL_IFC_FTIM1],
                      FSL_IFC_QIXIS_FTIM1);
  MmioWrite32((UINTN)&(FSL_IFC_REGS_BASE)->ftim_cs[FSL_IFC_CS3].ftim[FSL_IFC_FTIM2],
                      FSL_IFC_QIXIS_FTIM2);
  MmioWrite32((UINTN)&(FSL_IFC_REGS_BASE)->ftim_cs[FSL_IFC_CS3].ftim[FSL_IFC_FTIM3],
                      FSL_IFC_QIXIS_FTIM3);
  MmioWrite32((UINTN)&(FSL_IFC_REGS_BASE)->csor_cs[FSL_IFC_CS3].csor,
                      FSL_IFC_QIXIS_CSOR);
  MmioWrite32((UINTN)&(FSL_IFC_REGS_BASE)->amask_cs[FSL_IFC_CS3].amask,
                      FSL_IFC_QIXIS_AMASK);
  MmioWrite32((UINTN)&(FSL_IFC_REGS_BASE)->cspr_cs[FSL_IFC_CS3].cspr,
                      FSL_IFC_QIXIS_CSPR);
}
