/** @file
  SPI Master Protocol

  This protocol manipulates the SPI host controller to perform transactions as a master
  on the SPI bus using the current state of any switches or multiplexers in the SPI bus.

  Based on MdePkg/Include/Protocol/I2cMaster.h

  Copyright (c) 2013, Intel Corporation. All rights reserved.<BR>
  Copyright 2017 NXP

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __SPI_MASTER_H__
#define __SPI_MASTER_H__

#include <Pi/PiSpi.h>

#define EFI_SPI_MASTER_PROTOCOL_GUID { 0xc2b4fc6f, 0x9c3a, 0x42f9, { 0x4a, 0x94, 0x94, 0x57, 0xda, 0x1a, 0xa7, 0xc4 }}

typedef struct _EFI_SPI_MASTER_PROTOCOL EFI_SPI_MASTER_PROTOCOL;

/**
  Set the frequency for the SPI clock line.

  This routine must be called at or below TPL_NOTIFY.

  The software and controller do a best case effort of using the specified
  frequency for the SPI bus.  If the frequency does not match exactly then
  the SPI master protocol selects the next lower frequency to avoid
  exceeding the operating conditions for any of the SPI devices on the bus.
  For example if 400 KHz was specified and the controller's divide network
  only supports 402 KHz or 398 KHz then the SPI master protocol selects 398
  KHz.  If there are not lower frequencies available, then return
  EFI_UNSUPPORTED.

  @param[in] This           Pointer to an EFI_SPI_MASTER_PROTOCOL structure
  @param[in] BusClockHertz  Pointer to the requested SPI bus clock frequency
                            in Hertz.  Upon return this value contains the
                            actual frequency in use by the SPI controller.

  @retval EFI_SUCCESS           The bus frequency was set successfully.
  @retval EFI_ALREADY_STARTED   The controller is busy with another transaction.
  @retval EFI_INVALID_PARAMETER BusClockHertz is NULL
  @retval EFI_UNSUPPORTED       The controller does not support this frequency.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_SPI_MASTER_PROTOCOL_SET_BUS_FREQUENCY) (
  IN CONST EFI_SPI_MASTER_PROTOCOL   *This,
  IN OUT UINTN                       *BusClockHertz
);

/**
  Reset the SPI controller and configure it for use

  This routine must be called at or below TPL_NOTIFY.

  The SPI controller is reset.  The caller must call SetBusFrequench() after
  calling Reset().

  @param[in]     This       Pointer to an EFI_SPI_MASTER_PROTOCOL structure.

  @retval EFI_SUCCESS         The reset completed successfully.
  @retval EFI_ALREADY_STARTED The controller is busy with another transaction.
  @retval EFI_DEVICE_ERROR    The reset operation failed.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_SPI_MASTER_PROTOCOL_RESET) (
  IN CONST EFI_SPI_MASTER_PROTOCOL *This
);

/**
  Start an SPI transaction on the host controller.

  This routine must be called at or below TPL_NOTIFY.  For synchronous
  requests this routine must be called at or below TPL_CALLBACK.

  This function initiates an SPI transaction on the controller.  To
  enable proper error handling by the SPI protocol stack, the SPI
  master protocol does not support queuing but instead only manages
  one SPI transaction at a time.  This API requires that the SPI bus
  is in the correct configuration for the SPI transaction.

  The transaction is performed by sending a start-bit and selecting the
  SPI device with the specified SPI slave address and then performing
  the specified SPI operations.  When multiple operations are requested
  they are separated with a repeated start bit and the slave address.
  The transaction is terminated with a stop bit.

  When Event is NULL, StartRequest operates synchronously and returns
  the SPI completion status as its return value.

  When Event is not NULL, StartRequest synchronously returns EFI_SUCCESS
  indicating that the SPI transaction was started asynchronously.  The
  transaction status value is returned in the buffer pointed to by
  SpiStatus upon the completion of the SPI transaction when SpiStatus
  is not NULL.  After the transaction status is returned the Event is
  signaled.

  Note: The typical consumer of this API is the SPI host protocol.
  Extreme care must be taken by other consumers of this API to prevent
  confusing the third party SPI drivers due to a state change at the
  SPI device which the third party SPI drivers did not initiate.  SPI
  platform specific code may use this API within these guidelines.

  @param[in] This           Pointer to an EFI_SPI_MASTER_PROTOCOL structure.
  @param[in] ChipSelect     Address of the device on the SPI bus.
  @param[in] RequestPacket  Pointer to an EFI_SPI_REQUEST_PACKET
                            structure describing the SPI transaction.
  @param[in] Event          Event to signal for asynchronous transactions,
                            NULL for asynchronous transactions
  @param[out] SpiStatus     Optional buffer to receive the SPI transaction
                            completion status

  @retval EFI_SUCCESS           The asynchronous transaction was successfully
                                started when Event is not NULL.
  @retval EFI_SUCCESS           The transaction completed successfully when
                                Event is NULL.
  @retval EFI_ALREADY_STARTED   The controller is busy with another transaction.
  @retval EFI_BAD_BUFFER_SIZE   The RequestPacket->LengthInBytes value is too
                                large.
  @retval EFI_DEVICE_ERROR      There was an SPI error (NACK) during the
                                transaction.
  @retval EFI_INVALID_PARAMETER RequestPacket is NULL
  @retval EFI_NOT_FOUND         Reserved bit set in the ChipSelect parameter
  @retval EFI_NO_RESPONSE       The SPI device is not responding to the slave
                                address.  EFI_DEVICE_ERROR will be returned if
                                the controller cannot distinguish when the NACK
                                occurred.
  @retval EFI_OUT_OF_RESOURCES  Insufficient memory for SPI transaction
  @retval EFI_UNSUPPORTED       The controller does not support the requested
                                transaction.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_SPI_MASTER_PROTOCOL_START_REQUEST) (
  IN CONST EFI_SPI_MASTER_PROTOCOL *This,
  IN UINTN                         ChipSelect,
  IN EFI_SPI_REQUEST_PACKET        *RequestPacket,
  IN EFI_EVENT                     Event      OPTIONAL,
  OUT EFI_STATUS                   *SpiStatus OPTIONAL
);

///
/// SPI master mode protocol
///
/// This protocol manipulates the SPI host controller to perform transactions as a
/// master on the SPI bus using the current state of any switches or multiplexers
/// in the SPI bus.
///
struct _EFI_SPI_MASTER_PROTOCOL {
  ///
  /// Set the clock frequency for the SPI bus.
  ///
  EFI_SPI_MASTER_PROTOCOL_SET_BUS_FREQUENCY SetBusFrequency;

  ///
  /// Reset the SPI host controller.
  ///
  EFI_SPI_MASTER_PROTOCOL_RESET             Reset;

  ///
  /// Start an SPI transaction in master mode on the host controller.
  ///
  EFI_SPI_MASTER_PROTOCOL_START_REQUEST     StartRequest;

  ///
  /// Pointer to an EFI_SPI_CONTROLLER_CAPABILITIES data structure containing
  /// the capabilities of the SPI host controller.
  ///
  CONST EFI_SPI_CONTROLLER_CAPABILITIES     *SpiControllerCapabilities;
};

extern EFI_GUID gEfiSpiMasterProtocolGuid;

#endif  //  __SPI_MASTER_H__
