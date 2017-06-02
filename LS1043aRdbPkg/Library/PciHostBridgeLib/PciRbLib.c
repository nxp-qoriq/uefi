/** PciRbLib.c
 Functions supporting PCI Root Bridge Io Protocol 

 Some part of code is inspired from EDK II, file:
 PcAtChipsetPkg/PciHostBridgeDxe/PciRootBridgeIo.c

 Copyright (c) 2008 - 2012, Intel Corporation. All rights reserved.
 Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution. The full text of the license may be found 
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/ 

#include "PciHostBridge.h"

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


EFI_STATUS
RootBridgeIoCheckParameter (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN OPERATION_TYPE                         OperationType,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN UINT64                                 Address,
  IN UINTN                                  Count,
  IN VOID                                   *Buffer
  )
{
  if ((Width >= EfiPciWidthMaximum) || (Width < EfiPciWidthUint8))
    return EFI_INVALID_PARAMETER;
  if (NULL == Buffer)
    return EFI_INVALID_PARAMETER;

  return EFI_SUCCESS;
}


/**
   Internal help function for read and write memory space.

   @param[in]   This          A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Write         Switch value for Read or Write.
   @param[in]   Width         Signifies the width of the memory operations.
   @param[in]   UserAddress   The address within the PCI configuration space for the PCI controller.
   @param[in]   Count         The number of PCI configuration operations to perform. Bytes
                              moved is Width size * Count, starting at Address.
   @param[in, out] UserBuffer For read operations, the destination buffer to store the results. For
                              write operations, the source buffer to write data from.

   @retval EFI_SUCCESS            The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
RootBridgeIoMemRW (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     BOOLEAN                                Write,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  )
{
  EFI_STATUS                             Status;
  UINT8                                  InStride;
  UINT8                                  OutStride;
  UINT8                                  *Uint8Buffer;
  PCI_ROOT_BRIDGE_INSTANCE              *PrivateData;

  Status = RootBridgeIoCheckParameter (This, MemOperation, Width, Address, Count, Buffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((Address < CONFIG_SYS_LS_PCIE_MEM_PHYS_OFF) ||
      (Address >= ((UINT32)CONFIG_SYS_LS_PCIE_MEM_PHYS_OFF +
                   CONFIG_SYS_LS_PCIE_MEM_SIZE))) {
    return EFI_INVALID_PARAMETER;
  }

  PrivateData = DRIVER_INSTANCE_FROM_PCI_ROOT_BRIDGE_IO_THIS(This);

  Address += PrivateData->PciBaseAddress64;

  InStride = mInStride[Width];
  OutStride = mOutStride[Width];

  for (Uint8Buffer = Buffer; Count > 0; Address += InStride, Uint8Buffer += OutStride, Count--) {
    if (Write) {
      switch (Width) {
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
      switch (Width) {
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


/**
   Internal help function for read and write IO space.

   @param[in]   This          A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Write         Switch value for Read or Write.
   @param[in]   Width         Signifies the width of the memory operations.
   @param[in]   UserAddress   The address within the PCI configuration space for the PCI controller.
   @param[in]   Count         The number of PCI configuration operations to perform. Bytes
                              moved is Width size * Count, starting at Address.
   @param[in, out] UserBuffer For read operations, the destination buffer to store the results. For
                              write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS            The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
RootBridgeIoIoRW (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     BOOLEAN                                Write,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  )
{
  DEBUG((EFI_D_INFO, "PCI Root Bridge Io Io Read/Write function is not implemented yet.\n"));
  return EFI_SUCCESS;
}


/**
   Internal help function for read and write PCI configuration space.

   @param[in]   This          A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Write         Switch value for Read or Write.
   @param[in]   Width         Signifies the width of the memory operations.
   @param[in]   UserAddress   The address within the PCI configuration space for the PCI controller.
   @param[in]   Count         The number of PCI configuration operations to perform. Bytes
                              moved is Width size * Count, starting at Address.
   @param[in, out] UserBuffer For read operations, the destination buffer to store the results. For
                              write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS            The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
RootBridgeIoPciRW (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN BOOLEAN                                Write,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN UINT64                                 Address,
  IN UINTN                                  Count,
  IN OUT VOID                               *Buffer
  )
{
  EFI_STATUS                                   Status;
  INT32					Offset;
  UINT32					BusDev;
  UINT8                                        InStride;
  UINT8                                        OutStride;
  UINT8                                        *Uint8Buffer;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS  *PciRbAddr;
  PCI_ROOT_BRIDGE_INSTANCE          		*PrivateData;

  PrivateData = DRIVER_INSTANCE_FROM_PCI_ROOT_BRIDGE_IO_THIS (This);
  Status = RootBridgeIoCheckParameter (This, PciOperation, Width, Address, Count, Buffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PciRbAddr = (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS*) &Address;

  if (PciRbAddr->ExtendedRegister)
    Offset = (INT32) PciRbAddr->ExtendedRegister;
  else
    Offset = (INT32) PciRbAddr->Register;

  BusDev = (UINT32)PCI_BUS_DEV (
                          PciRbAddr->Bus,
                          PciRbAddr->Device,
                          PciRbAddr->Function
                          );

  InStride = mInStride[Width];
  OutStride = mOutStride[Width];

  for (Uint8Buffer = Buffer; Count > 0; Offset += InStride, Uint8Buffer += OutStride, Count--) {
    if (Write) {
      switch (Width) {
        case EfiPciWidthUint8:
          Status = PcieWriteConfigByte(PrivateData, (UINT32)BusDev, Offset, *Uint8Buffer);
          break;
        case EfiPciWidthUint16:
          Status = PcieWriteConfigWord(PrivateData, (UINT32)BusDev, Offset, *(UINT16 *)Uint8Buffer);
          break;
        case EfiPciWidthUint32:
          Status = PcieWriteConfig(PrivateData, (UINT32)BusDev, Offset, *(UINT32 *)Uint8Buffer);
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
      switch (Width) {
        case EfiPciWidthUint8:
          Status = PcieReadConfigByte(PrivateData, (UINT32)BusDev, Offset, Uint8Buffer);
          break;
        case EfiPciWidthUint16:
          Status = PcieReadConfigWord(PrivateData, (UINT32)BusDev, Offset, (UINT16 *)Uint8Buffer);
          break;
        case EfiPciWidthUint32:
          Status = PcieReadConfig(PrivateData, (UINT32)BusDev, Offset, (UINT32 *)Uint8Buffer);
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

  return Status;
}
