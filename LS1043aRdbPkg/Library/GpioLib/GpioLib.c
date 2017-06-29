/** GpioLib.c
  Gpio Library containing functions for gpio.

  Copyright 2016 NXP

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Library/Gpio.h"

VOID *
GetBaseAddr (
  IN  UINT8  Id
  )
{
  switch (Id) {
    case GPIO1:
      return (VOID *)GPIO1_BASE_ADDR;

    case GPIO2:
      return (VOID *)GPIO2_BASE_ADDR;

    case GPIO3:
      return (VOID *)GPIO3_BASE_ADDR;

    case GPIO4:
      return (VOID *)GPIO4_BASE_ADDR;

    default:
      DEBUG((EFI_D_ERROR, "Invalid Gpio Id %d, Allowed Ids are %d-%d \n",
				Id, GPIO1, GPIO4));
      return NULL;
  }
}

BOOLEAN
IsPinSupported (
  IN  UINT8   Id,
  IN  UINT32  Pin
  )
{
  switch (Id) {
    case GPIO1:
      if (Pin >= 13 && Pin <= 31)
        return TRUE;

      break;

    case GPIO2:
      if ((Pin >= 0 && Pin <= 15) ||
          (Pin >= 25 && Pin <= 27))
        return TRUE;

      break;

    case GPIO3:
      if (Pin >= 0 && Pin <= 27)
        return TRUE;

      break;

    case GPIO4:
      if ((Pin >= 0 && Pin <= 3) ||
          (Pin >= 10 && Pin <= 13) ||
          (Pin >= 29 && Pin <= 30))
        return TRUE;

      break;

    default:
      DEBUG((EFI_D_ERROR, "Invalid Gpio Id %d, Allowed Ids are %d-%d \n",
         			Id, GPIO1, GPIO4));
      return FALSE;
  }

  DEBUG((EFI_D_ERROR, "Invalid Gpio Pin %d, For Gpio id 0x%x \n",
				Pin, Id));

  return FALSE;
}

EFI_STATUS
SetDir (
  IN  UINT8    Id,
  IN  UINT32   Bit,
  IN  BOOLEAN  Dir
  )
{
  BOOLEAN  Status;
  GPIO_REGS *Regs = NULL;

  Status = IsPinSupported(Id, Bit);

  if (!Status)
    return EFI_UNSUPPORTED;

  Regs = GetBaseAddr(Id);
  
  if (Dir)
    MmioSetBitsBe32((UINTN)&Regs->GpDir, BIT(Bit));
  else
    MmioClearBitsBe32((UINTN)&Regs->GpDir, BIT(Bit));

  return EFI_SUCCESS;
}

INT32
GetDir (
  IN  UINT8    Id,
  IN  UINT32   Bit
  )
{
  BOOLEAN  Status;
  GPIO_REGS *Regs = NULL;
  UINT32 Value = 0;

  Status = IsPinSupported(Id, Bit);

  if (!Status)
    return -1;

  Regs = GetBaseAddr(Id);
  
  Value = MmioReadBe32((UINTN)&Regs->GpDir);

  return (Value & BIT(Bit));
}

INT32
GetData (
  IN  UINT8    Id,
  IN  UINT32   Bit
  )
{
  BOOLEAN  Status;
  GPIO_REGS *Regs = NULL;
  UINT32 Value = 0;

  Status = IsPinSupported(Id, Bit);

  if (!Status)
    return -1;

  Regs = (VOID *)GetBaseAddr(Id);

  Value = MmioReadBe32((UINTN)&Regs->GpData);

  return (Value & BIT(Bit));
}

EFI_STATUS
SetData (
  IN  UINT8    Id,
  IN  UINT32   Bit,
  IN  BOOLEAN  Data
  )
{
  BOOLEAN  Status;
  GPIO_REGS *Regs = NULL;

  Status = IsPinSupported(Id, Bit);

  if (!Status)
    return -1;

  Regs = GetBaseAddr(Id);

  if (Data)
    MmioSetBitsBe32((UINTN)&Regs->GpData, BIT(Bit));
  else
    MmioClearBitsBe32((UINTN)&Regs->GpData, BIT(Bit));

  return EFI_SUCCESS;
}

EFI_STATUS
SetOpenDrain (
  IN  UINT8    Id,
  IN  UINT32   Bit,
  IN  BOOLEAN  OpenDrain
  )
{
  BOOLEAN  Status;
  GPIO_REGS *Regs = NULL;

  Status = IsPinSupported(Id, Bit);

  if (!Status)
    return EFI_UNSUPPORTED;

  Regs = GetBaseAddr(Id);

  if (OpenDrain)
    MmioSetBitsBe32((UINTN)&Regs->GpOdr, BIT(Bit));
  else
    MmioClearBitsBe32((UINTN)&Regs->GpOdr, BIT(Bit));

  return EFI_SUCCESS;
}

BOOLEAN
ReadInterrupt (
  IN  UINT8    Id,
  IN  UINT32   Bit
  )
{
  GPIO_REGS *Regs = NULL;
  UINT32 Value = 0;

  Regs = GetBaseAddr(Id);

  Value = MmioReadBe32((UINTN)&Regs->GpIer);

  return (Value & BIT(Bit));
}

VOID
ClearInterrupt (
  IN  UINT8    Id,
  IN  UINT32   Bit
  )
{
  GPIO_REGS *Regs = NULL;

  Regs = GetBaseAddr(Id);

  MmioWriteBe32((UINTN)&Regs->GpIer, BIT(Bit));
}

VOID
SetInterruptMask (
  IN  UINT8    Id,
  IN  UINT32   Bit
  )
{
  GPIO_REGS *Regs = NULL;

  Regs = GetBaseAddr(Id);

  MmioSetBitsBe32((UINTN)&Regs->GpImr, BIT(Bit));
}

VOID
ClearInterruptMask (
  IN  UINT8    Id,
  IN  UINT32   Bit
  )
{
  GPIO_REGS *Regs = NULL;

  Regs = GetBaseAddr(Id);

  MmioClearBitsBe32((UINTN)&Regs->GpImr, BIT(Bit));
}

VOID
ControlInterrupt (
  IN  UINT8    Id,
  IN  UINT32   Bit,
  IN  BOOLEAN  Edge
  )
{
  GPIO_REGS *Regs = NULL;

  Regs = GetBaseAddr(Id);

  if (Edge == ALWAYS)
    MmioClearBitsBe32((UINTN)&Regs->GpIcr, BIT(Bit));
  else if (Edge == EDGE)
    MmioSetBitsBe32((UINTN)&Regs->GpIcr, BIT(Bit));
}

