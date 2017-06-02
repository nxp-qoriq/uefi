/** @CpldLib.c
  Cpld specific Library for LS1043A-RDB board, containing functions to
  program and read the Cpld registers.

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

#include <LS1043aRdb.h>
#include <CpldLib.h>
#include <Library/FslIfc.h>

UINT8
CpldRead (
  OUT UINTN Reg
  )
{
	VOID *Base = (VOID *)CONFIG_CPLD_BASE;

	return MmioRead8((UINTN)(Base + Reg));
}

VOID
CpldWrite (
  IN UINTN Reg,
  IN UINT8 Value
  )
{
	VOID *Base = (VOID *)CONFIG_CPLD_BASE;

	MmioWrite8((UINTN)(Base + Reg), Value);
}

/* Set the boot bank to the alternate bank */
VOID
CpldSetAlternatebank (
  VOID
  )
{
	UINT8 Reg4 = CPLD_READ(SoftMuxOn);
	UINT8 Reg7 = CPLD_READ(Vbank);

	CPLD_WRITE(SoftMuxOn, Reg4 | CPLD_SW_MUX_BANK_SEL);

	Reg7 = (Reg7 & ~CPLD_BANK_SEL_MASK) | CPLD_BANK_SEL_ALTBANK;
	CPLD_WRITE(Vbank, Reg7);

	CPLD_WRITE(SystemReset, 1);
}

/* Set the boot bank to the default bank */
VOID
CpldSetDefaultBank (
  VOID
  )
{
	CPLD_WRITE(GlobalReset, 1);
}

VOID
CpldDumpRegs (
  VOID
  )
{
	DEBUG((EFI_D_INFO, "CpldVersionMajor	= %x\n", CPLD_READ(CpldVersionMajor)));
	DEBUG((EFI_D_INFO, "CpldVersionMinor	= %x\n", CPLD_READ(CpldVersionMinor)));
	DEBUG((EFI_D_INFO, "PcbaVersion	= %x\n", CPLD_READ(PcbaVersion)));
	DEBUG((EFI_D_INFO, "SoftMuxOn	= %x\n", CPLD_READ(SoftMuxOn)));
	DEBUG((EFI_D_INFO, "RcwSource1	= %x\n", CPLD_READ(RcwSource1)));
	DEBUG((EFI_D_INFO, "RcwSource2	= %x\n", CPLD_READ(RcwSource2)));
	DEBUG((EFI_D_INFO, "Vbank		= %x\n", CPLD_READ(Vbank)));
	DEBUG((EFI_D_INFO, "SysclkSelect	= %x\n", CPLD_READ(SysclkSelect)));
	DEBUG((EFI_D_INFO, "UartSel	= %x\n", CPLD_READ(UartSel)));
	DEBUG((EFI_D_INFO, "Sd1RefClkSel	= %x\n", CPLD_READ(Sd1RefClkSel)));
	DEBUG((EFI_D_INFO, "TdmClkMuxSel	= %x\n", CPLD_READ(TdmClkMuxSel)));
	DEBUG((EFI_D_INFO, "SdhcSpiCsSel	= %x\n", CPLD_READ(SdhcSpiCsSel)));
	DEBUG((EFI_D_INFO, "StatusLed	= %x\n", CPLD_READ(StatusLed)));
}

VOID
CpldRevBit (
  OUT UINT8 *Value
  )
{
	UINT8 Rev, Val;
	UINTN Index;

	Val = *Value;
	Rev = Val & 1;
	for (Index = 1; Index <= 7; Index++) {
		Val >>= 1;
		Rev <<= 1;
		Rev |= Val & 1;
	}

	*Value = Rev;
}

VOID
DoCpld (
  IN CpldCmd Cmd
  )
{
	switch (Cmd) {
	case RESET:
		CpldSetDefaultBank();
		break;
	case RESET_ALTBANK:
		CpldSetAlternatebank();
		break;
	case DUMP_REGISTERS:
		CpldDumpRegs();
		break;
	default:
		DEBUG((EFI_D_ERROR, "Error: Unknown Cpld Command!\n"));
		break;
	}
}

VOID
CpldInit (
  VOID
  )
{
	MmioWriteBe32((UINTN) &(FSL_IFC_REGS_BASE)->cspr_cs[FSL_IFC_CS2].cspr_ext, FSL_IFC_CPLD_CSPR_EXT);

	MmioWriteBe32((UINTN) &(FSL_IFC_REGS_BASE)->ftim_cs[FSL_IFC_CS2].ftim[FSL_IFC_FTIM0],
			FSL_IFC_CPLD_FTIM0);
	MmioWriteBe32((UINTN) &(FSL_IFC_REGS_BASE)->ftim_cs[FSL_IFC_CS2].ftim[FSL_IFC_FTIM1],
			FSL_IFC_CPLD_FTIM1);
	MmioWriteBe32((UINTN) &(FSL_IFC_REGS_BASE)->ftim_cs[FSL_IFC_CS2].ftim[FSL_IFC_FTIM2],
			FSL_IFC_CPLD_FTIM2);
	MmioWriteBe32((UINTN) &(FSL_IFC_REGS_BASE)->ftim_cs[FSL_IFC_CS2].ftim[FSL_IFC_FTIM3],
			FSL_IFC_CPLD_FTIM3);

	MmioWriteBe32((UINTN) &(FSL_IFC_REGS_BASE)->csor_cs[FSL_IFC_CS2].csor, FSL_IFC_CPLD_CSOR);
	MmioWriteBe32((UINTN) &(FSL_IFC_REGS_BASE)->amask_cs[FSL_IFC_CS2].amask, FSL_IFC_CPLD_AMASK);
	MmioWriteBe32((UINTN) &(FSL_IFC_REGS_BASE)->cspr_cs[FSL_IFC_CS2].cspr, FSL_IFC_CPLD_CSPR);
}
