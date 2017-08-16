/** @file
  SPI Device Enumerate Protocol.

  This protocol supports the enumerations of device on the SPI bus.

  Based on MdePkg/Include/Protocol/I2cEnumerate.h

  Copyright (c) 2013, Intel Corporation. All rights reserved.<BR>
  Copyright 2017 NXP

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __SPI_ENUMERATE_H__
#define __SPI_ENUMERATE_H__

#include <Pi/PiSpi.h>

#define EFI_SPI_ENUMERATE_PROTOCOL_GUID   { 0xd7bec15e, 0xab7d, 0x43f1, { 0x63, 0x9c, 0xcc, 0xc5, 0x79, 0x75, 0x19, 0x25 }}

typedef struct _EFI_SPI_ENUMERATE_PROTOCOL  EFI_SPI_ENUMERATE_PROTOCOL;

/**
  Enumerate the SPI devices

  This function enables the caller to traverse the set of SPI devices
  on an SPI bus.

  @param[in]  This              The platform data for the next device on
                                the SPI bus was returned successfully.
  @param[in, out] Device        Pointer to a buffer containing an
                                EFI_SPI_DEVICE structure.  Enumeration is
                                started by setting the initial EFI_SPI_DEVICE
                                structure pointer to NULL.  The buffer
                                receives an EFI_SPI_DEVICE structure pointer
                                to the next SPI device.

  @retval EFI_SUCCESS           The platform data for the next device on
                                the SPI bus was returned successfully.
  @retval EFI_INVALID_PARAMETER Device is NULL
  @retval EFI_NO_MAPPING        *Device does not point to a valid
                                EFI_SPI_DEVICE structure returned in a
                                previous call Enumerate().

**/
typedef
EFI_STATUS
(EFIAPI *EFI_SPI_ENUMERATE_PROTOCOL_ENUMERATE) (
  IN CONST EFI_SPI_ENUMERATE_PROTOCOL *This,
  IN OUT CONST EFI_SPI_DEVICE         **Device
);

/**
  Get the requested SPI bus frequency for a specified bus configuration.

  This function returns the requested SPI bus clock frequency for the
  SpiBusConfiguration.  This routine is provided for diagnostic purposes
  and is meant to be called after calling Enumerate to get the
  SpiBusConfiguration value.

  @param[in] This                 Pointer to an EFI_SPI_ENUMERATE_PROTOCOL
                                  structure.
  @param[in] SpiBusConfiguration  SPI bus configuration to access the SPI
                                  device
  @param[out] *BusClockHertz      Pointer to a buffer to receive the SPI
                                  bus clock frequency in Hertz

  @retval EFI_SUCCESS           The SPI bus frequency was returned
                                successfully.
  @retval EFI_INVALID_PARAMETER BusClockHertz was NULL
  @retval EFI_NO_MAPPING        Invalid SpiBusConfiguration value

**/
typedef
EFI_STATUS
(EFIAPI *EFI_SPI_ENUMERATE_PROTOCOL_GET_BUS_FREQUENCY) (
  IN CONST EFI_SPI_ENUMERATE_PROTOCOL *This,
  IN UINTN                            SpiBusConfiguration,
  OUT UINTN                           *BusClockHertz
);

///
/// SPI Enumerate protocol
///
struct _EFI_SPI_ENUMERATE_PROTOCOL {
  ///
  /// Traverse the set of SPI devices on an SPI bus.  This routine
  /// returns the next SPI device on an SPI bus.
  ///
  EFI_SPI_ENUMERATE_PROTOCOL_ENUMERATE         Enumerate;

  ///
  /// Get the requested SPI bus frequency for a specified bus
  /// configuration.
  ///
  EFI_SPI_ENUMERATE_PROTOCOL_GET_BUS_FREQUENCY GetBusFrequency;
};

///
/// Reference to variable defined in the .DEC file
///
extern EFI_GUID gEfiSpiEnumerateProtocolGuid;

#endif  //  __SPI_ENUMERATE_H__
