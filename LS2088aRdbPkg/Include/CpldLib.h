/** CpldLib.h
*  Header defining the LS2088a Cpld specific constants (Base addresses, sizes, flags)
*
*  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#ifndef __LS2088A_CPLD_H__
#define __LS2088A_CPLD_H__

#include <Common.h>

/*
 * CPLD register set of LS2088ARDB board-specific.
 */
struct CpldRegSet {
	UINT8 CpldVersionMajor;	/* 0x0 - CPLD Major Revision Register */
	UINT8 CpldVersionMinor;	/* 0x1 - CPLD Minor Revision Register */
	UINT8 PcbaVersion;	/* 0x2 - PCBA Revision Register */
	UINT8 SystemReset;	/* 0x3 - system reset register */
	UINT8 SoftMuxOn;	/* 0x4 - Switch Control Enable Register */
	UINT8 RcwSource1;	/* 0x5 - Reset config word 1 */
	UINT8 RcwSource2;	/* 0x6 - Reset config word 1 */
	UINT8 Vbank;		/* 0x7 - Flash bank selection Control */
	UINT8 SysclkSelect;	/* 0x8 - */
	UINT8 UartSel;		/* 0x9 - */
	UINT8 Sd1RefClkSel;	/* 0xA - */
	UINT8 TdmClkMuxSel;	/* 0xB - */
	UINT8 SdhcSpiCsSel;	/* 0xC - */
	UINT8 StatusLed;	/* 0xD - */
	UINT8 GlobalReset;	/* 0xE - */
};

/*
 * Reset the board, Reset to alternate bank or Dump registers:
 * RESET - reset to default bank
 * RESET_ALTBANK - reset to alternate bank
 * DUMP_REGISTERS - display the CPLD registers
 */
typedef enum {
	RESET = 0,
	RESET_ALTBANK,
	DUMP_REGISTERS
} CpldCmd;

UINT8 CpldRead(UINTN Reg);
VOID CpldWrite(UINTN Reg, UINT8 Value);
VOID CpldRevBit(UINT8 *Value);
VOID DoCpld (CpldCmd Cmd);
VOID CpldInit (VOID);

#define CPLD_READ(Reg) CpldRead(offsetof(struct CpldRegSet, Reg))
#define CPLD_WRITE(Reg, Value)  \
	CpldWrite(offsetof(struct CpldRegSet, Reg), Value)

/* CPLD on IFC */
#define CPLD_SW_MUX_BANK_SEL	0x40
#define CPLD_BANK_SEL_MASK	0x07
#define CPLD_BANK_SEL_ALTBANK	0x04

/* SDXC/DSPI CPLD Settings */
#define ENABLE_SDXC_SOFT_MUX	0x30
#define ENABLE_RCW_SOFT_MUX	0x01
#define SELECT_SW4_SDXC		0x40
#define SELECT_SW5_SDXC		0x01

#endif
