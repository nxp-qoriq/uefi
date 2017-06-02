/** PciRootBridgeIo.c
 PCI Root Bridge Io Protocol implementation

 Reference taken from PCI Emulation implementation in
 Omap35xxPkg/PciEmulation/

 Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
 Copyright (c) 2017 NXP.
 All rights reserved.

 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution. The full text of the license may be found 
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/ 

#include "PciEmulation.h"

//
// Lookup table for increment values based on transfer widths
//
UINT8 mInStride[] = {
  1, // EfiPciWidthUint8
  2, // EfiPciWidthUint16
  4, // EfiPciWidthUint32
  8, // EfiPciWidthUint64
  0, // EfiPciWidthFifoUint8
  0, // EfiPciWidthFifoUint16
  0, // EfiPciWidthFifoUint32
  0, // EfiPciWidthFifoUint64
  1, // EfiPciWidthFillUint8
  2, // EfiPciWidthFillUint16
  4, // EfiPciWidthFillUint32
  8  // EfiPciWidthFillUint64
};

//
// Lookup table for increment values based on transfer widths
//
UINT8 mOutStride[] = {
  1, // EfiPciWidthUint8
  2, // EfiPciWidthUint16
  4, // EfiPciWidthUint32
  8, // EfiPciWidthUint64
  1, // EfiPciWidthFifoUint8
  2, // EfiPciWidthFifoUint16
  4, // EfiPciWidthFifoUint32
  8, // EfiPciWidthFifoUint64
  0, // EfiPciWidthFillUint8
  0, // EfiPciWidthFillUint16
  0, // EfiPciWidthFillUint32
  0  // EfiPciWidthFillUint64
};


BOOLEAN
PciRootBridgeMemAddressValid (
  IN PCI_ROOT_BRIDGE  *Private,
  IN UINT64           Address
  )
{
  if ((Address >= Private->MemoryStart) && (Address < (Private->MemoryStart + Private->MemorySize))) {
    return TRUE;
  }

  return FALSE;
}


EFI_STATUS
PciRootBridgeIoMemRW (
  IN     BOOLEAN                                Write,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  )
{
  UINT8                                  InStride;
  UINT8                                  OutStride;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  OperationWidth;
  UINT8                                  *Uint8Buffer;

  InStride = mInStride[Width];
  OutStride = mOutStride[Width];
  OperationWidth = (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH) (Width & 0x03);
  for (Uint8Buffer = Buffer; Count > 0; Address += InStride, Uint8Buffer += OutStride, Count--) {
    if (Write) {
      switch (OperationWidth) {
        case EfiPciWidthUint8:
          MmioWrite8 ((UINTN)Address, *Uint8Buffer);
          break;
        case EfiPciWidthUint16:
          MmioWrite16 ((UINTN)Address, *((UINT16 *)Uint8Buffer));
          break;
        case EfiPciWidthUint32:
          MmioWrite32 ((UINTN)Address, *((UINT32 *)Uint8Buffer));
          break;
        case EfiPciWidthUint64:
          MmioWrite64 ((UINTN)Address, *((UINT64 *)Uint8Buffer));
          break;
        default:
          //
          // The RootBridgeIoCheckParameter call above will ensure that this
          // path is not taken.
          //
          ASSERT (FALSE);
          break;
      }
    } else {
      switch (OperationWidth) {
        case EfiPciWidthUint8:
          *Uint8Buffer = MmioRead8 ((UINTN)Address);
          break;
        case EfiPciWidthUint16:
          *((UINT16 *)Uint8Buffer) = MmioRead16 ((UINTN)Address);
          break;
        case EfiPciWidthUint32:
          *((UINT32 *)Uint8Buffer) = MmioRead32 ((UINTN)Address);
          break;
        case EfiPciWidthUint64:
          *((UINT64 *)Uint8Buffer) = MmioRead64 ((UINTN)Address);
          break;
        default:
          //
          // The RootBridgeIoCheckParameter call above will ensure that this
          // path is not taken.
          //
          ASSERT (FALSE);
          break;
      }
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PciRootBridgeIoPciRW (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN BOOLEAN                                Write,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN UINT64                                 UserAddress,
  IN UINTN                                  Count,
  IN OUT VOID                               *UserBuffer
  )
{
  return EFI_SUCCESS;
}

/**
  Enables a PCI driver to access PCI controller registers in the PCI root bridge memory space.

  @param  This                  A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param  Width                 Signifies the width of the memory operations.
  @param  Address               The base address of the memory operations.
  @param  Count                 The number of memory operations to perform.
  @param  Buffer                For read operations, the destination buffer to store the results. For write
                                operations, the source buffer to write data from.

  @retval EFI_SUCCESS           The data was read from or written to the PCI root bridge.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.

**/
EFI_STATUS
EFIAPI
PciRootBridgeIoMemRead (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  )
{
  PCI_ROOT_BRIDGE   *Private;

  if ( Buffer == NULL ) {
    return EFI_INVALID_PARAMETER;
  }

  Private = INSTANCE_FROM_PCI_ROOT_BRIDGE_IO_THIS (This);

  if (!PciRootBridgeMemAddressValid (Private, Address)) {
    DEBUG((EFI_D_ERROR, "READ ADDRESS is not valid: %llx\n", Address));
    return EFI_INVALID_PARAMETER;
  }

  return PciRootBridgeIoMemRW (FALSE, Width, Address, Count, Buffer);

}



/**
  Enables a PCI driver to access PCI controller registers in the PCI root bridge memory space.

  @param  This                  A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param  Width                 Signifies the width of the memory operations.
  @param  Address               The base address of the memory operations.
  @param  Count                 The number of memory operations to perform.
  @param  Buffer                For read operations, the destination buffer to store the results. For write
                                operations, the source buffer to write data from.

  @retval EFI_SUCCESS           The data was read from or written to the PCI root bridge.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.

**/
EFI_STATUS
EFIAPI
PciRootBridgeIoMemWrite (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  )
{
  PCI_ROOT_BRIDGE *Private;

  if ( Buffer == NULL ) {
    return EFI_INVALID_PARAMETER;
  }

  Private = INSTANCE_FROM_PCI_ROOT_BRIDGE_IO_THIS (This);

  if (!PciRootBridgeMemAddressValid (Private, Address)) {
    DEBUG((EFI_D_ERROR, "WRITE ADDRESS is not valid: %llx\n", Address));
    return EFI_INVALID_PARAMETER;
  }

  return PciRootBridgeIoMemRW (TRUE, Width, Address, Count, Buffer);
}

/**
  Enables a PCI driver to access PCI controller registers in the PCI root bridge memory space.

  @param  This                  A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param  Width                 Signifies the width of the memory operations.
  @param  Address               The base address of the memory operations.
  @param  Count                 The number of memory operations to perform.
  @param  Buffer                For read operations, the destination buffer to store the results. For write
                                operations, the source buffer to write data from.

  @retval EFI_SUCCESS           The data was read from or written to the PCI root bridge.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.

**/
EFI_STATUS
EFIAPI
PciRootBridgeIoPciRead (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  )
{
  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return PciRootBridgeIoPciRW (This, FALSE, Width, Address, Count, Buffer);
}



/**
  Enables a PCI driver to access PCI controller registers in the PCI root bridge memory space.

  @param  This                  A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param  Width                 Signifies the width of the memory operations.
  @param  Address               The base address of the memory operations.
  @param  Count                 The number of memory operations to perform.
  @param  Buffer                For read operations, the destination buffer to store the results. For write
                                operations, the source buffer to write data from.

  @retval EFI_SUCCESS           The data was read from or written to the PCI root bridge.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.

**/
EFI_STATUS
EFIAPI
PciRootBridgeIoPciWrite (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  )
{
  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return PciRootBridgeIoPciRW (This, TRUE, Width, Address, Count, Buffer);
}


