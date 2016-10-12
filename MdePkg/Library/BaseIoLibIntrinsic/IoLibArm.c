/** @file
  I/O Library for ARM. 

  Copyright (c) 2006 - 2009, Intel Corporation. All rights reserved.<BR>
  Portions copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  Copyright (c) 2017, AMD Incorporated. All rights reserved.<BR>

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


#define Uswap16(X) \
       ((((X) & 0xff00) >> 8) | \
        (((X) & 0x00ff) << 8))
#define Uswap32(X) \
       ((((X) & 0xff000000) >> 24) | \
        (((X) & 0x00ff0000) >>  8) | \
        (((X) & 0x0000ff00) <<  8) | \
        (((X) & 0x000000ff) << 24))
#define Uswap64(X) \
       ((((X) & 0xff00000000000000) >> 56) | \
        (((X) & 0x00ff000000000000) >> 40) | \
        (((X) & 0x0000ff0000000000) >> 24) | \
        (((X) & 0x000000ff00000000) >>  8) | \
        (((X) & 0x00000000ff000000) <<  8) | \
        (((X) & 0x0000000000ff0000) << 24) | \
        (((X) & 0x000000000000ff00) << 40) | \
        (((X) & 0x00000000000000ff) << 56))


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
  Reads an 8-bit I/O port fifo into a block of memory.

  Reads the 8-bit I/O fifo port specified by Port.
  The port is read Count times, and the read data is
  stored in the provided Buffer.

  This function must guarantee that all I/O read and write operations are
  serialized.

  If 8-bit I/O port operations are not supported, then ASSERT().

  @param  Port    The I/O port to read.
  @param  Count   The number of times to read I/O port.
  @param  Buffer  The buffer to store the read data into.

**/
VOID
EFIAPI
IoReadFifo8 (
  IN      UINTN                     Port,
  IN      UINTN                     Count,
  OUT     VOID                      *Buffer
  )
{
  ASSERT (FALSE);
}

/**
  Writes a block of memory into an 8-bit I/O port fifo.

  Writes the 8-bit I/O fifo port specified by Port.
  The port is written Count times, and the write data is
  retrieved from the provided Buffer.

  This function must guarantee that all I/O write and write operations are
  serialized.

  If 8-bit I/O port operations are not supported, then ASSERT().

  @param  Port    The I/O port to write.
  @param  Count   The number of times to write I/O port.
  @param  Buffer  The buffer to retrieve the write data from.

**/
VOID
EFIAPI
IoWriteFifo8 (
  IN      UINTN                     Port,
  IN      UINTN                     Count,
  IN      VOID                      *Buffer
  )
{
  ASSERT (FALSE);
}

/**
  Reads a 16-bit I/O port fifo into a block of memory.

  Reads the 16-bit I/O fifo port specified by Port.
  The port is read Count times, and the read data is
  stored in the provided Buffer.

  This function must guarantee that all I/O read and write operations are
  serialized.

  If 16-bit I/O port operations are not supported, then ASSERT().

  @param  Port    The I/O port to read.
  @param  Count   The number of times to read I/O port.
  @param  Buffer  The buffer to store the read data into.

**/
VOID
EFIAPI
IoReadFifo16 (
  IN      UINTN                     Port,
  IN      UINTN                     Count,
  OUT     VOID                      *Buffer
  )
{
  ASSERT (FALSE);
}

/**
  Writes a block of memory into a 16-bit I/O port fifo.

  Writes the 16-bit I/O fifo port specified by Port.
  The port is written Count times, and the write data is
  retrieved from the provided Buffer.

  This function must guarantee that all I/O write and write operations are
  serialized.

  If 16-bit I/O port operations are not supported, then ASSERT().

  @param  Port    The I/O port to write.
  @param  Count   The number of times to write I/O port.
  @param  Buffer  The buffer to retrieve the write data from.

**/
VOID
EFIAPI
IoWriteFifo16 (
  IN      UINTN                     Port,
  IN      UINTN                     Count,
  IN      VOID                      *Buffer
  )
{
  ASSERT (FALSE);
}

/**
  Reads a 32-bit I/O port fifo into a block of memory.

  Reads the 32-bit I/O fifo port specified by Port.
  The port is read Count times, and the read data is
  stored in the provided Buffer.

  This function must guarantee that all I/O read and write operations are
  serialized.

  If 32-bit I/O port operations are not supported, then ASSERT().

  @param  Port    The I/O port to read.
  @param  Count   The number of times to read I/O port.
  @param  Buffer  The buffer to store the read data into.

**/
VOID
EFIAPI
IoReadFifo32 (
  IN      UINTN                     Port,
  IN      UINTN                     Count,
  OUT     VOID                      *Buffer
  )
{
  ASSERT (FALSE);
}

/**
  Writes a block of memory into a 32-bit I/O port fifo.

  Writes the 32-bit I/O fifo port specified by Port.
  The port is written Count times, and the write data is
  retrieved from the provided Buffer.

  This function must guarantee that all I/O write and write operations are
  serialized.

  If 32-bit I/O port operations are not supported, then ASSERT().

  @param  Port    The I/O port to write.
  @param  Count   The number of times to write I/O port.
  @param  Buffer  The buffer to retrieve the write data from.

**/
VOID
EFIAPI
IoWriteFifo32 (
  IN      UINTN                     Port,
  IN      UINTN                     Count,
  IN      VOID                      *Buffer
  )
{
  ASSERT (FALSE);
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
  return Uswap16(Value);
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
  *(volatile UINT16*)Address = Uswap16(Value);
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
  return Uswap32(Value);
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
  *(volatile UINT32*)Address = Uswap32(Value);
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
  return Uswap64(Value);
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
  *(volatile UINT64*)Address = Uswap64(Value);
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
  *(volatile UINT16*)Address = (*(volatile UINT16*)Address & Uswap16(~Mask))
								 | Uswap16(Value);
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
  *(volatile UINT32*)Address = (*(volatile UINT32*)Address & Uswap32(~Mask))
								 | Uswap32(Value);
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
  *(volatile UINT64*)Address = (*(volatile UINT64*)Address & Uswap64(~Mask))
								| Uswap64(Value);
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
  *(volatile UINT16*)Address = *(volatile UINT16*)Address | Uswap16(Bits);
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
  *(volatile UINT32*)Address = *(volatile UINT32*)Address | Uswap32(Bits);
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
  *(volatile UINT64*)Address = *(volatile UINT64*)Address | Uswap64(Bits);
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
MmioClearBitsBe16 (
  IN      UINTN                     Address,
  IN      UINT16                    Mask
  )
{
  ASSERT ((Address & 1) == 0);
  *(volatile UINT16*)Address = *(volatile UINT16*)Address & Uswap16(~Mask);
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
  *(volatile UINT32*)Address = *(volatile UINT32*)Address & Uswap32(~Mask);
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
  *(volatile UINT64*)Address = *(volatile UINT64*)Address & Uswap64(~Mask);
  return *(volatile UINT64*)Address;
}
