/** @file
  I/O Library for ARM. 

  Copyright (c) 2006 - 2009, Intel Corporation. All rights reserved.<BR>
  Portions copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


//
// Include common header file for this module.
//
#include "BaseIoLibIntrinsicInternal.h"

/**
  Reads an 8-bit I/O port.

  Reads the 8-bit I/O port specified by Port. The 8-bit read value is returned.
  This function must guarantee that all I/O read and write operations are
  serialized.

  If 8-bit I/O port operations are not supported, then ASSERT().

  @param  Port  The I/O port to read.

  @return The value read.

**/
UINT8
EFIAPI
IoRead8 (
  IN      UINTN                     Port
  )
{
  ASSERT (FALSE);
  return 0;
}

/**
  Writes an 8-bit I/O port.

  Writes the 8-bit I/O port specified by Port with the value specified by Value
  and returns Value. This function must guarantee that all I/O read and write
  operations are serialized.

  If 8-bit I/O port operations are not supported, then ASSERT().

  @param  Port  The I/O port to write.
  @param  Value The value to write to the I/O port.

  @return The value written the I/O port.

**/
UINT8
EFIAPI
IoWrite8 (
  IN      UINTN                     Port,
  IN      UINT8                     Value
  )
{
  ASSERT (FALSE);
  return Value;
}

/**
  Reads a 16-bit I/O port.

  Reads the 16-bit I/O port specified by Port. The 16-bit read value is returned.
  This function must guarantee that all I/O read and write operations are
  serialized.

  If 16-bit I/O port operations are not supported, then ASSERT().

  @param  Port  The I/O port to read.

  @return The value read.

**/
UINT16
EFIAPI
IoRead16 (
  IN      UINTN                     Port
  )
{
  ASSERT (FALSE);
  return 0;
}

/**
  Writes a 16-bit I/O port.

  Writes the 16-bit I/O port specified by Port with the value specified by Value
  and returns Value. This function must guarantee that all I/O read and write
  operations are serialized.

  If 16-bit I/O port operations are not supported, then ASSERT().

  @param  Port  The I/O port to write.
  @param  Value The value to write to the I/O port.

  @return The value written the I/O port.

**/
UINT16
EFIAPI
IoWrite16 (
  IN      UINTN                     Port,
  IN      UINT16                    Value
  )
{
  ASSERT (FALSE);
  return Value;
}

/**
  Reads a 32-bit I/O port.

  Reads the 32-bit I/O port specified by Port. The 32-bit read value is returned.
  This function must guarantee that all I/O read and write operations are
  serialized.

  If 32-bit I/O port operations are not supported, then ASSERT().

  @param  Port  The I/O port to read.

  @return The value read.

**/
UINT32
EFIAPI
IoRead32 (
  IN      UINTN                     Port
  )
{
  ASSERT (FALSE);
  return 0;
}

/**
  Writes a 32-bit I/O port.

  Writes the 32-bit I/O port specified by Port with the value specified by Value
  and returns Value. This function must guarantee that all I/O read and write
  operations are serialized.

  If 32-bit I/O port operations are not supported, then ASSERT().

  @param  Port  The I/O port to write.
  @param  Value The value to write to the I/O port.

  @return The value written the I/O port.

**/
UINT32
EFIAPI
IoWrite32 (
  IN      UINTN                     Port,
  IN      UINT32                    Value
  )
{
  ASSERT (FALSE);
  return Value;
}

/**
  Reads a 64-bit I/O port.

  Reads the 64-bit I/O port specified by Port. The 64-bit read value is returned.
  This function must guarantee that all I/O read and write operations are
  serialized.

  If 64-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 64-bit boundary, then ASSERT().

  @param  Port  The I/O port to read.

  @return The value read.

**/
UINT64
EFIAPI
IoRead64 (
  IN      UINTN                     Port
  )
{
  ASSERT (FALSE);
  return 0;
}

/**
  Writes a 64-bit I/O port.

  Writes the 64-bit I/O port specified by Port with the value specified by Value
  and returns Value. This function must guarantee that all I/O read and write
  operations are serialized.

  If 64-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 64-bit boundary, then ASSERT().

  @param  Port  The I/O port to write.
  @param  Value The value to write to the I/O port.

  @return The value written to the I/O port.

**/
UINT64
EFIAPI
IoWrite64 (
  IN      UINTN                     Port,
  IN      UINT64                    Value
  )
{
  ASSERT (FALSE);
  return 0;
}


/**
  Reads an 8-bit MMIO register.

  Reads the 8-bit MMIO register specified by Address. The 8-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 8-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT8
EFIAPI
MmioRead8 (
  IN      UINTN                     Address
  )
{
  UINT8                             Value;

  Value = *(volatile UINT8*)Address;
  return Value;
}

/**
  Writes an 8-bit MMIO register.

  Writes the 8-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 8-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

**/
UINT8
EFIAPI
MmioWrite8 (
  IN      UINTN                     Address,
  IN      UINT8                     Value
  )
{
  *(volatile UINT8*)Address = Value;
  return Value;
}

/**
  Reads a 16-bit MMIO register.

  Reads the 16-bit MMIO register specified by Address. The 16-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT16
EFIAPI
MmioRead16 (
  IN      UINTN                     Address
  )
{
  UINT16                            Value;

  ASSERT ((Address & 1) == 0);
  Value = *(volatile UINT16*)Address;
  return Value;
}

/**
  Writes a 16-bit MMIO register.

  Writes the 16-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

**/
UINT16
EFIAPI
MmioWrite16 (
  IN      UINTN                     Address,
  IN      UINT16                    Value
  )
{
  ASSERT ((Address & 1) == 0);
  *(volatile UINT16*)Address = Value;
  return Value;
}

/**
  Reads a 32-bit MMIO register.

  Reads the 32-bit MMIO register specified by Address. The 32-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT32
EFIAPI
MmioRead32 (
  IN      UINTN                     Address
  )
{
  UINT32                            Value;

  ASSERT ((Address & 3) == 0);
  Value = *(volatile UINT32*)Address;
  return Value;
}

/**
  Writes a 32-bit MMIO register.

  Writes the 32-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

**/
UINT32
EFIAPI
MmioWrite32 (
  IN      UINTN                     Address,
  IN      UINT32                    Value
  )
{
  ASSERT ((Address & 3) == 0);
  *(volatile UINT32*)Address = Value;
  return Value;
}

/**
  Reads a 64-bit MMIO register.

  Reads the 64-bit MMIO register specified by Address. The 64-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT64
EFIAPI
MmioRead64 (
  IN      UINTN                     Address
  )
{
  UINT64                            Value;

  ASSERT ((Address & 7) == 0);
  Value = *(volatile UINT64*)Address;
  return Value;
}

/**
  Writes a 64-bit MMIO register.

  Writes the 64-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

**/
UINT64
EFIAPI
MmioWrite64 (
  IN      UINTN                     Address,
  IN      UINT64                    Value
  )
{
  ASSERT ((Address & 7) == 0);
  *(volatile UINT64*)Address = Value;
  return Value;
}


/**
  Reads a 16-bit MMIO register in Big Endian format.

  Reads the 16-bit MMIO register specified by Address. The 16-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT16
EFIAPI
MmioReadBe16 (
  IN      UINTN                     Address
  )
{
  UINT16                            Value;

  ASSERT ((Address & 1) == 0);
  Value = *(volatile UINT16*)Address;
  return SwapBytes16(Value);
}

/**
  Writes a 16-bit MMIO register in Big Endian format.

  Writes the 16-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

**/
UINT16
EFIAPI
MmioWriteBe16 (
  IN      UINTN                     Address,
  IN      UINT16                    Value
  )
{
  ASSERT ((Address & 1) == 0);
  *(volatile UINT16*)Address = SwapBytes16(Value);
  return Value;
}

/**
  Reads a 32-bit MMIO register in Big Endian format.

  Reads the 32-bit MMIO register specified by Address. The 32-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT32
EFIAPI
MmioReadBe32 (
  IN      UINTN                     Address
  )
{
  UINT32                            Value;

  ASSERT ((Address & 3) == 0);
  Value = *(volatile UINT32*)Address;
  return SwapBytes32(Value);
}

/**
  Writes a 32-bit MMIO register in Big Endian format.

  Writes the 32-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

**/
UINT32
EFIAPI
MmioWriteBe32 (
  IN      UINTN                     Address,
  IN      UINT32                    Value
  )
{
  ASSERT ((Address & 3) == 0);
  *(volatile UINT32*)Address = SwapBytes32(Value);
  return Value;
}

/**
  Reads a 64-bit MMIO register in Big Endian format.

  Reads the 64-bit MMIO register specified by Address. The 64-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT64
EFIAPI
MmioReadBe64 (
  IN      UINTN                     Address
  )
{
  UINT64                            Value;

  ASSERT ((Address & 7) == 0);
  Value = *(volatile UINT64*)Address;
  return SwapBytes64(Value);
}

/**
  Writes a 64-bit MMIO register in Big Endian format.

  Writes the 64-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

**/
UINT64
EFIAPI
MmioWriteBe64 (
  IN      UINTN                     Address,
  IN      UINT64                    Value
  )
{
  ASSERT ((Address & 7) == 0);
  *(volatile UINT64*)Address = SwapBytes64(Value);
  return Value;
}

/**
  Clear and set a 8-bit MMIO register.

  Mask the 8-bit MMIO register specified by Address with the mask specified
  by Mask and then Writes the 8-bit MMIO register specified by Address with
  the value specified by Value and returns current value on register. This
  function must guarantee that all MMIO read and write operations are serialized.

  @param  Address The MMIO register to write.
  @param  Mask    The Mask to clear the MMIO register.
  @param  Value   The value to write to the MMIO register.

**/
UINT8
EFIAPI
MmioClearSet8 (
  IN      UINTN                    Address,
  IN      UINT8                    Mask,
  IN      UINT8                    Value
  )
{
  *(volatile UINT8*)Address = (*(volatile UINT8*)Address & ~Mask) | Value;
  return *(volatile UINT8*)Address;
}

/**
  Clear and set a 16-bit MMIO register.

  Mask the 16-bit MMIO register specified by Address with the mask specified
  by Mask and then Writes the 16-bit MMIO register specified by Address with
  the value specified by Value and returns current value on register. This
  function must guarantee that all MMIO read and write operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Mask    The Mask to clear the MMIO register.
  @param  Value   The value to write to the MMIO register.

**/
UINT16
EFIAPI
MmioClearSet16 (
  IN      UINTN                     Address,
  IN      UINT16                    Mask,
  IN      UINT16                    Value
  )
{
  ASSERT ((Address & 1) == 0);
  *(volatile UINT16*)Address = (*(volatile UINT16*)Address & ~Mask) | Value;
  return *(volatile UINT16*)Address;
}

/**
  Clear and set a 32-bit MMIO register.

  Mask the 32-bit MMIO register specified by Address with the mask specified
  by Mask and then Writes the 32-bit MMIO register specified by Address with
  the value specified by Value and returns current value on register. This
  function must guarantee that all MMIO read and write operations are serialized.


  If 32-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Mask    The Mask to clear the MMIO register.
  @param  Value   The value to write to the MMIO register.

**/
UINT32
EFIAPI
MmioClearSet32 (
  IN      UINTN                     Address,
  IN      UINT32                    Mask,
  IN      UINT32                    Value
  )
{
  ASSERT ((Address & 3) == 0);
  *(volatile UINT32*)Address = (*(volatile UINT32*)Address & ~Mask) | Value;
  return *(volatile UINT32*)Address;
}

/**
  Clear and set a 64-bit MMIO register.

  Mask the 64-bit MMIO register specified by Address with the mask specified
  by Mask and then Writes the 64-bit MMIO register specified by Address with
  the value specified by Value and returns current value on register. This
  function must guarantee that all MMIO read and write operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Mask    The Mask to clear the MMIO register.
  @param  Value   The value to write to the MMIO register.

**/
UINT64
EFIAPI
MmioClearSet64 (
  IN      UINTN                     Address,
  IN      UINT64                    Mask,
  IN      UINT64                    Value
  )
{
  ASSERT ((Address & 7) == 0);
  *(volatile UINT64*)Address = (*(volatile UINT64*)Address & ~Mask) | Value;
  return *(volatile UINT64*)Address;
}

/**
  Clear and set a 16-bit MMIO register in Big Endian format.

  Mask the 16-bit MMIO register specified by Address with the mask specified
  by Mask and then Writes the 16-bit MMIO register specified by Address with
  the value specified by Value and returns current value on register. This
  function must guarantee that all MMIO read and write operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Mask    The Mask to clear the MMIO register.
  @param  Value   The value to write to the MMIO register.

**/
UINT16
EFIAPI
MmioClearSetBe16 (
  IN      UINTN                     Address,
  IN      UINT16                    Mask,
  IN      UINT16                    Value
  )
{
  ASSERT ((Address & 1) == 0);
  *(volatile UINT16*)Address = (*(volatile UINT16*)Address & SwapBytes16(~Mask))
								 | SwapBytes16(Value);
  return *(volatile UINT16*)Address;
}

/**
  Clear and set a 32-bit MMIO register in Big Endian format.

  Mask the 32-bit MMIO register specified by Address with the mask specified
  by Mask and then Writes the 32-bit MMIO register specified by Address with
  the value specified by Value and returns current value on register. This
  function must guarantee that all MMIO read and write operations are serialized.


  If 32-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Mask    The Mask to clear the MMIO register.
  @param  Value   The value to write to the MMIO register.

**/
UINT32
EFIAPI
MmioClearSetBe32 (
  IN      UINTN                     Address,
  IN      UINT32                    Mask,
  IN      UINT32                    Value
  )
{
  ASSERT ((Address & 3) == 0);
  *(volatile UINT32*)Address = (*(volatile UINT32*)Address & SwapBytes32(~Mask))
								 | SwapBytes32(Value);
  return *(volatile UINT32*)Address;
}

/**
  Clear and set a 64-bit MMIO register in Big Endian format.

  Mask the 64-bit MMIO register specified by Address with the mask specified
  by Mask and then Writes the 64-bit MMIO register specified by Address with
  the value specified by Value and returns current value on register. This
  function must guarantee that all MMIO read and write operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Mask    The Mask to clear the MMIO register.
  @param  Value   The value to write to the MMIO register.

**/
UINT64
EFIAPI
MmioClearSetBe64 (
  IN      UINTN                     Address,
  IN      UINT64                    Mask,
  IN      UINT64                    Value
  )
{
  ASSERT ((Address & 7) == 0);
  *(volatile UINT64*)Address = (*(volatile UINT64*)Address & SwapBytes64(~Mask))
								| SwapBytes64(Value);
  return *(volatile UINT64*)Address;
}

/**
  Set a 8-bit MMIO register in Big Endian format.

  Set bits of the 8-bit MMIO register specified by Address with the Bits
  specified by Bits and returns register content. This function must
  guarantee that all MMIO read and write operations are serialized.

  If 8-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Bits    The Bits to set in MMIO register.

**/
UINT8
EFIAPI
MmioSetBits8 (
  IN      UINTN                     Address,
  IN      UINT8                     Bits
  )
{
  *(volatile UINT8*)Address = *(volatile UINT8*)Address | Bits;
  return *(volatile UINT8*)Address;
}

/**
  Set a 16-bit MMIO register.

  Set bits of the 16-bit MMIO register specified by Address with the Bits
  specified by Bits and returns register content. This function must
  guarantee that all MMIO read and write operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Bits    The Bits to set in MMIO register.

**/
UINT16
EFIAPI
MmioSetBits16 (
  IN      UINTN                     Address,
  IN      UINT16                    Bits
  )
{
  ASSERT ((Address & 1) == 0);
  *(volatile UINT16*)Address = *(volatile UINT16*)Address | Bits;
  return *(volatile UINT16*)Address;
}

/**
  Set a 32-bit MMIO register.

  Set bits of the 32-bit MMIO register specified by Address with the Bits
  specified by Bits and returns register content. This function must
  guarantee that all MMIO read and write operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Bits    The Bits to set in MMIO register.

**/
UINT32
EFIAPI
MmioSetBits32 (
  IN      UINTN                     Address,
  IN      UINT32                    Bits
  )
{
  ASSERT ((Address & 3) == 0);
  *(volatile UINT32*)Address = *(volatile UINT32*)Address | Bits;
  return *(volatile UINT32*)Address;
}

/**
  Set a 64-bit MMIO register.

  Set bits of the 64-bit MMIO register specified by Address with the Bits
  specified by Bits and returns register content. This function must
  guarantee that all MMIO read and write operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Bits    The Bits to set in MMIO register.

**/
UINT64
EFIAPI
MmioSetBits64 (
  IN      UINTN                     Address,
  IN      UINT64                    Bits
  )
{
  ASSERT ((Address & 7) == 0);
  *(volatile UINT64*)Address = *(volatile UINT64*)Address | Bits;
  return *(volatile UINT64*)Address;
}

/**
  Set a 16-bit MMIO register in Big Endian format.

  Set bits of the 16-bit MMIO register specified by Address with the Bits
  specified by Bits and returns register content. This function must
  guarantee that all MMIO read and write operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Bits    The Bits to set in MMIO register.

**/
UINT16
EFIAPI
MmioSetBitsBe16 (
  IN      UINTN                     Address,
  IN      UINT16                    Bits
  )
{
  ASSERT ((Address & 1) == 0);
  *(volatile UINT16*)Address = *(volatile UINT16*)Address | SwapBytes16(Bits);
  return *(volatile UINT16*)Address;
}

/**
  Set a 32-bit MMIO register in Big Endian format.

  Set bits of the 32-bit MMIO register specified by Address with the Bits
  specified by Bits and returns register content. This function must
  guarantee that all MMIO read and write operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Bits    The Bits to set in MMIO register.

**/
UINT32
EFIAPI
MmioSetBitsBe32 (
  IN      UINTN                     Address,
  IN      UINT32                    Bits
  )
{
  ASSERT ((Address & 3) == 0);
  *(volatile UINT32*)Address = *(volatile UINT32*)Address | SwapBytes32(Bits);
  return *(volatile UINT32*)Address;
}

/**
  Set a 64-bit MMIO register in Big Endian format.

  Set bits of the 64-bit MMIO register specified by Address with the Bits
  specified by Bits and returns register content. This function must
  guarantee that all MMIO read and write operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Bits    The Bits to set in MMIO register.

**/
UINT64
EFIAPI
MmioSetBitsBe64 (
  IN      UINTN                     Address,
  IN      UINT64                    Bits
  )
{
  ASSERT ((Address & 7) == 0);
  *(volatile UINT64*)Address = *(volatile UINT64*)Address | SwapBytes64(Bits);
  return *(volatile UINT64*)Address;
}

/**
  Clear bits of the 8-bit MMIO register specified by Address with the Mask
  specified by Mask and returns register content. This function must
  guarantee that all MMIO read and write operations are serialized.

  If 8-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Mask    The Bits to clear in MMIO register.

**/
UINT8
EFIAPI
MmioClearBits8 (
  IN      UINTN                     Address,
  IN      UINT8                     Mask
  )
{
  *(volatile UINT8*)Address = *(volatile UINT8*)Address & (~Mask);
  return *(volatile UINT8*)Address;
}

/**
  Clear bits of the 16-bit MMIO register specified by Address with the Mask
  specified by Mask and returns register content. This function must
  guarantee that all MMIO read and write operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Mask    The Bits to clear in MMIO register.

**/
UINT16
EFIAPI
MmioClearBits16 (
  IN      UINTN                     Address,
  IN      UINT16                    Mask
  )
{
  ASSERT ((Address & 1) == 0);
  *(volatile UINT16*)Address = *(volatile UINT16*)Address & ~Mask;
  return *(volatile UINT16*)Address;
}

/**
  Clear bits of the 32-bit MMIO register specified by Address with the Mask
  specified by Mask and returns register content. This function must
  guarantee that all MMIO read and write operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Mask    The Bits to clear in MMIO register.

**/
UINT32
EFIAPI
MmioClearBits32 (
  IN      UINTN                     Address,
  IN      UINT32                    Mask
  )
{
  ASSERT ((Address & 3) == 0);
  *(volatile UINT32*)Address = *(volatile UINT32*)Address & ~Mask;
  return *(volatile UINT32*)Address;
}

/**
  Clear bits of the 64-bit MMIO register specified by Address with the Mask
  specified by Mask and returns register content. This function must
  guarantee that all MMIO read and write operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Mask    The Bits to clear in MMIO register.

**/
UINT64
EFIAPI
MmioClearBits64 (
  IN      UINTN                     Address,
  IN      UINT64                    Mask
  )
{
  ASSERT ((Address & 7) == 0);
  *(volatile UINT64*)Address = *(volatile UINT64*)Address & ~Mask;
  return *(volatile UINT64*)Address;
}

/**
  Clear bits of the 16-bit MMIO register specified by Address with the Mask
  specified by Mask and returns register content. This function must
  guarantee that all MMIO read and write operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Mask    The Bits to clear in MMIO register.

**/
UINT16
EFIAPI
MmioClearBitsBe16 (
  IN      UINTN                     Address,
  IN      UINT16                    Mask
  )
{
  ASSERT ((Address & 1) == 0);
  *(volatile UINT16*)Address = *(volatile UINT16*)Address & SwapBytes16(~Mask);
  return *(volatile UINT16*)Address;
}

/**
  Clear bits of the 32-bit MMIO register specified by Address with the Mask
  specified by Mask and returns register content. This function must
  guarantee that all MMIO read and write operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Mask    The Bits to clear in MMIO register.

**/
UINT32
EFIAPI
MmioClearBitsBe32 (
  IN      UINTN                     Address,
  IN      UINT32                    Mask
  )
{
  ASSERT ((Address & 3) == 0);
  *(volatile UINT32*)Address = *(volatile UINT32*)Address & SwapBytes32(~Mask);
  return *(volatile UINT32*)Address;
}

/**
  Clear bits of the 64-bit MMIO register specified by Address with the Mask
  specified by Mask and returns register content. This function must
  guarantee that all MMIO read and write operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Mask    The Bits to clear in MMIO register.

**/
UINT64
EFIAPI
MmioClearBitsBe64 (
  IN      UINTN                     Address,
  IN      UINT64                    Mask
  )
{
  ASSERT ((Address & 7) == 0);
  *(volatile UINT64*)Address = *(volatile UINT64*)Address & SwapBytes64(~Mask);
  return *(volatile UINT64*)Address;
}
