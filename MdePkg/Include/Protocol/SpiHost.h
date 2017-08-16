/** @file
  SPI Host Protocol.

  This protocol provides callers with the ability to do I/O transactions
  to all of the devices on the SPI bus.

  Based on MdePkg/Include/Protocol/I2cHost.h

  Copyright (c) 2013, Intel Corporation. All rights reserved.<BR>
  Copyright 2017 NXP

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __SPI_HOST_H__
#define __SPI_HOST_H__

#include <Pi/PiSpi.h>

#define EFI_SPI_HOST_PROTOCOL_GUID  { 0xaf60eb43, 0x2ea5, 0x4371, { 0x4a, 0x96, 0x5e, 0x2f, 0x2d, 0x85, 0x3d, 0x24 }}

///
/// SPI Host Protocol
///
/// The SPI bus driver uses the services of the EFI_SPI_HOST_PROTOCOL
/// to produce an instance of the EFI_SPI_IO_PROTOCOL for each SPI
/// device on an SPI bus.
///
/// The EFI_SPI_HOST_PROTOCOL exposes an asynchronous interface to
/// callers to perform transactions to any device on the SPI bus.
/// Internally, the SPI host protocol manages the flow of the SPI
/// transactions to the host controller, keeping them in FIFO order.
/// Prior to each transaction, the SPI host protocol ensures that the
/// switches and multiplexers are properly configured.  The SPI host
/// protocol then starts the transaction on the host controller using
/// the EFI_SPI_MASTER_PROTOCOL.
///
typedef struct _EFI_SPI_HOST_PROTOCOL EFI_SPI_HOST_PROTOCOL;


/**
  Queue an SPI transaction for execution on the SPI controller.

  This routine must be called at or below TPL_NOTIFY.  For
  synchronous requests this routine must be called at or below
  TPL_CALLBACK.

  The SPI host protocol uses the concept of SPI bus configurations
  to describe the SPI bus.  An SPI bus configuration is defined as
  a unique setting of the multiplexers and switches in the SPI bus
  which enable access to one or more SPI devices.  When using a
  switch to divide a bus, due to bus frequency differences, the
  SPI bus configuration management protocol defines an SPI bus
  configuration for the SPI devices on each side of the switch.
  When using a multiplexer, the SPI bus configuration management
  defines an SPI bus configuration for each of the selector values
  required to control the multiplexer.  See Figure 1 in the SPI -bus
  specification and user manual for a complex SPI bus configuration.

  The SPI host protocol processes all transactions in FIFO order.
  Prior to performing the transaction, the SPI host protocol calls
  EnableSpiBusConfiguration to reconfigure the switches and
  multiplexers in the SPI bus enabling access to the specified SPI
  device.  The EnableSpiBusConfiguration also selects the SPI bus
  frequency for the SPI device.  After the SPI bus is configured,
  the SPI host protocol calls the SPI master protocol to start the
  SPI transaction.

  When Event is NULL, QueueRequest() operates synchronously and
  returns the SPI completion status as its return value.

  When Event is not NULL, QueueRequest() synchronously returns
  EFI_SUCCESS indicating that the asynchronously SPI transaction was
  queued.  The values above are returned in the buffer pointed to by
  SpiStatus upon the completion of the SPI transaction when SpiStatus
  is not NULL.

  @param[in] This             Pointer to an EFI_SPI_HOST_PROTOCOL structure.
  @param[in] SpiBusConfiguration  SPI bus configuration to access the SPI
                                  device
  @param[in] ChipSelect     Address of the device on the SPI bus.  Set
  @param[in] Event            Event to signal for asynchronous transactions,
                              NULL for synchronous transactions
  @param[in] RequestPacket    Pointer to an EFI_SPI_REQUEST_PACKET structure
                              describing the SPI transaction
  @param[out] SpiStatus       Optional buffer to receive the SPI transaction
                              completion status

  @retval EFI_SUCCESS           The asynchronous transaction was successfully
                                queued when Event is not NULL.
  @retval EFI_SUCCESS           The transaction completed successfully when
                                Event is NULL.
  @retval EFI_BAD_BUFFER_SIZE   The RequestPacket->LengthInBytes value is
                                too large.
  @retval EFI_DEVICE_ERROR      There was an SPI error (NACK) during the
                                transaction.
  @retval EFI_INVALID_PARAMETER RequestPacket is NULL
  @retval EFI_NOT_FOUND         Reserved bit set in the ChipSelect parameter
  @retval EFI_NO_MAPPING        Invalid SpiBusConfiguration value
  @retval EFI_NO_RESPONSE       The SPI device is not responding to the slave
                                address.  EFI_DEVICE_ERROR will be returned
                                if the controller cannot distinguish when the
                                NACK occurred.
  @retval EFI_OUT_OF_RESOURCES  Insufficient memory for SPI transaction
  @retval EFI_UNSUPPORTED       The controller does not support the requested
                                transaction.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_SPI_HOST_PROTOCOL_QUEUE_REQUEST) (
  IN CONST EFI_SPI_HOST_PROTOCOL *This,
  IN UINTN                       SpiBusConfiguration,
  IN UINTN                       ChipSelect,
  IN EFI_EVENT                   Event      OPTIONAL,
  IN EFI_SPI_REQUEST_PACKET      *RequestPacket,
  OUT EFI_STATUS                 *SpiStatus OPTIONAL
);

///
/// SPI Host Protocol
///
struct _EFI_SPI_HOST_PROTOCOL {
  ///
  /// Queue an SPI transaction for execution on the SPI bus
  ///
  EFI_SPI_HOST_PROTOCOL_QUEUE_REQUEST     QueueRequest;

  ///
  /// Pointer to an EFI_SPI_CONTROLLER_CAPABILITIES data structure
  /// containing the capabilities of the SPI host controller.
  ///
  CONST EFI_SPI_CONTROLLER_CAPABILITIES   *SpiControllerCapabilities;
};

///
/// Reference to variable defined in the .DEC file
///
extern EFI_GUID gEfiSpiHostProtocolGuid;

#endif  //  __SPI_HOST_H__
