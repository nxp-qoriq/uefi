/** @file
  SPI I/O Protocol.

  The EFI SPI I/O protocol enables the user to manipulate a single
  SPI device independent of the host controller and SPI design.

  Based on MdePkg/Include/Protocol/I2cIo.h

  Copyright (c) 2013, Intel Corporation. All rights reserved.<BR>
  Copyright 2017 NXP

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __SPI_IO_H__
#define __SPI_IO_H__

#include <Pi/PiSpi.h>

#define EFI_SPI_IO_PROTOCOL_GUID { 0xbe99d738, 0x93b9, 0x45b1, { 0xc8, 0xaa, 0x7c, 0x92, 0xb1, 0x91, 0xeb, 0x27 }}

///
/// SPI I/O protocol
///
/// The SPI IO protocol enables access to a specific device on the SPI
/// bus.
///
/// Each SPI device is identified uniquely in the system by the tuple
/// DeviceGuid:DeviceIndex.  The DeviceGuid represents the manufacture
/// and part number and is provided by the silicon vendor or the third
/// party SPI device driver writer.  The DeviceIndex identifies the part
/// within the system by using a unique number and is created by the
/// board designer or the writer of the EFI_SPI_ENUMERATE_PROTOCOL.
///
/// SPI slave addressing is abstracted to validate addresses and limit
/// operation to the specified SPI device.  The third party providing
/// the SPI device support provides an ordered list of slave addresses
/// for the SPI device required to implement the EFI_SPI_ENUMERATE_PROTOCOL.
/// The order of the list must be preserved.
///
typedef struct _EFI_SPI_IO_PROTOCOL  EFI_SPI_IO_PROTOCOL;


/**
  Queue an SPI transaction for execution on the SPI device.

  This routine must be called at or below TPL_NOTIFY.  For synchronous
  requests this routine must be called at or below TPL_CALLBACK.

  This routine queues an SPI transaction to the SPI controller for
  execution on the SPI bus.

  When Event is NULL, QueueRequest() operates synchronously and returns
  the SPI completion status as its return value.

  When Event is not NULL, QueueRequest() synchronously returns EFI_SUCCESS
  indicating that the asynchronous SPI transaction was queued.  The values
  above are returned in the buffer pointed to by SpiStatus upon the
  completion of the SPI transaction when SpiStatus is not NULL.

  The upper layer driver writer provides the following to the platform
  vendor:

  1.  Vendor specific GUID for the SPI part
  2.  Guidance on proper construction of the slave address array when the
      SPI device uses more than one slave address.  The SPI bus protocol
      uses the ChipSelectIndex to perform relative to physical address
      translation to access the blocks of hardware within the SPI device.

  @param[in] This               Pointer to an EFI_SPI_IO_PROTOCOL structure.
  @param[in] ChipSelectIndex  Index value into an array of slave addresses
                                for the SPI device.  The values in the array
                                are specified by the board designer, with the
                                third party SPI device driver writer providing
                                the slave address order.

                                For devices that have a single slave address,
                                this value must be zero.  If the SPI device
                                uses more than one slave address then the
                                third party (upper level) SPI driver writer
                                needs to specify the order of entries in the
                                slave address array.

                                \ref ThirdPartySpiDrivers "Third Party SPI
                                Drivers" section in SpiMaster.h.
  @param[in] Event              Event to signal for asynchronous transactions,
                                NULL for synchronous transactions
  @param[in] RequestPacket      Pointer to an EFI_SPI_REQUEST_PACKET structure
                                describing the SPI transaction
  @param[out] SpiStatus         Optional buffer to receive the SPI transaction
                                completion status

  @retval EFI_SUCCESS           The asynchronous transaction was successfully
                                queued when Event is not NULL.
  @retval EFI_SUCCESS           The transaction completed successfully when
                                Event is NULL.
  @retval EFI_BAD_BUFFER_SIZE   The RequestPacket->LengthInBytes value is too
                                large.
  @retval EFI_DEVICE_ERROR      There was an SPI error (NACK) during the
                                transaction.
  @retval EFI_INVALID_PARAMETER RequestPacket is NULL.
  @retval EFI_NO_MAPPING        The EFI_SPI_HOST_PROTOCOL could not set the
                                bus configuration required to access this SPI
                                device.
  @retval EFI_NO_RESPONSE       The SPI device is not responding to the slave
                                address selected by ChipSelectIndex.
                                EFI_DEVICE_ERROR will be returned if the
                                controller cannot distinguish when the NACK
                                occurred.
  @retval EFI_OUT_OF_RESOURCES  Insufficient memory for SPI transaction
  @retval EFI_UNSUPPORTED       The controller does not support the requested
                                transaction.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_SPI_IO_PROTOCOL_QUEUE_REQUEST) (
  IN CONST EFI_SPI_IO_PROTOCOL  *This,
  IN UINTN                      ChipSelectIndex,
  IN EFI_EVENT                  Event      OPTIONAL,
  IN EFI_SPI_REQUEST_PACKET     *RequestPacket,
  OUT EFI_STATUS                *SpiStatus OPTIONAL
);

///
/// SPI I/O protocol
///
struct _EFI_SPI_IO_PROTOCOL {
  ///
  /// Queue an SPI transaction for execution on the SPI device.
  ///
  EFI_SPI_IO_PROTOCOL_QUEUE_REQUEST         QueueRequest;

  ///
  /// Unique value assigned by the silicon manufacture or the third
  /// party SPI driver writer for the SPI part.  This value logically
  /// combines both the manufacture name and the SPI part number into
  /// a single value specified as a GUID.
  ///
  CONST EFI_GUID                            *DeviceGuid;

  ///
  /// Unique ID of the SPI part within the system
  ///
  UINT32                                    DeviceIndex;

  ///
  /// Hardware revision - ACPI _HRV value.  See the Advanced Configuration
  /// and Power Interface Specification, Revision 5.0  for the field format
  /// and the Plug and play support for SPI web-page for restriction on values.
  ///
  UINT32                                    HardwareRevision;

  ///
  /// Pointer to an EFI_SPI_CONTROLLER_CAPABILITIES data structure containing
  /// the capabilities of the SPI host controller.
  ///
  CONST EFI_SPI_CONTROLLER_CAPABILITIES     *SpiControllerCapabilities;
};

///
/// Reference to variable defined in the .DEC file
///
extern EFI_GUID gEfiSpiIoProtocolGuid;

#endif  //  __SPI_IO_H__
