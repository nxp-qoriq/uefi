/** @Gpio.h
  Header defining the constant, base address amd function for GPIO controller

  Copyright (c) 2016 NXP.
  All rights reserved.

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __GPIO_H___
#define __GPIO_H__

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseMemoryLib/MemLibInternals.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Uefi.h>

#include "Bitops.h"
#include "LS1046aRdb.h"
#include "LS1046aSocLib.h"

#define GPIO1        1
#define GPIO2        2
#define GPIO3        3
#define GPIO4        4

#define GPIO1_BASE_ADDR            0x02300000
#define GPIO2_BASE_ADDR            0x02310000
#define GPIO3_BASE_ADDR            0x02320000
#define GPIO4_BASE_ADDR            0x02330000

/* enum for direction */
typedef enum _GPIO_DIRECTION {
       INPUT,
       OUTPUT
}GPIO_DIRECTION;

/* enums for interrupt generation condition */
typedef enum _GPIO_RAISE_INTR {
       ALWAYS,
       EDGE
}GPIO_RAISE_INTR;

/* enum for setting Gpio pin as open drain */
typedef enum _GPIO_OPEN_DRAIN {
	INPUT,
	OPEN_DRAIN
}GPIO_OPEN_DRAIN;

typedef struct GpioRegs {
  UINT32 GpDir;
  UINT32 GpOdr;
  UINT32 GpData;
  UINT32 GpIer;
  UINT32 GpImr;
  UINT32 GpIcr;
} GPIO_REGS;

EFI_STATUS
SetDir (
  IN  UINT8    Id,
  IN  UINT32   Bit,
  IN  BOOLEAN  Dir
  );

INT32
GetDir (
  IN  UINT8    Id,
  IN  UINT32   Bit
  );

INT32
GetData (
  IN  UINT8    Id,
  IN  UINT32   Bit
  );

EFI_STATUS
SetData (
  IN  UINT8    Id,
  IN  UINT32   Bit,
  IN  BOOLEAN  Data
  );

EFI_STATUS
SetOpenDrain (
  IN  UINT8    Id,
  IN  UINT32   Bit,
  IN  BOOLEAN  OpenDrain
  );

BOOLEAN
ReadInterrupt (
  IN  UINT8    Id,
  IN  UINT32   Bit
  );

VOID
ClearInterrupt (
  IN  UINT8    Id,
  IN  UINT32   Bit
  );

VOID
SetInterruptMask (
  IN  UINT8    Id,
  IN  UINT32   Bit
  );

VOID
ClearInterruptMask (
  IN  UINT8    Id,
  IN  UINT32   Bit
  );

VOID
ControlInterrupt (
  IN  UINT8    Id,
  IN  UINT32   Bit,
  IN  BOOLEAN  Edge
  );

#endif
