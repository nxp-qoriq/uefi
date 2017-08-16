/** @file
  SPI Bus Configuration Management Protocol.

  The EFI SPI bus configuration management protocol provides platform specific
  services that allow the SPI host protocol to reconfigure the switches and multiplexers
  and set the clock frequency for the SPI bus. This protocol also enables the SPI host protocol
  to reset an SPI device which may be locking up the SPI bus by holding the clock or data line low.

  Based on MdePkg/Include/Protocol/I2cBusConfigurationManagement.h

  Copyright (c) 2013, Intel Corporation. All rights reserved.<BR>
  Copyright 2017 NXP

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __SPI_BUS_CONFIGURATION_MANAGEMENT_H__
#define __SPI_BUS_CONFIGURATION_MANAGEMENT_H__

#define EFI_SPI_BUS_CONFIGURATION_MANAGEMENT_PROTOCOL_GUID \
  { 0xb174b238, 0x38fd, 0x4802, { 0x48, 0x81, 0x8b, 0xd9, 0x77, 0xb8, 0x84, 0xc6 }}

///
/// SPI bus configuration management protocol
///
/// The EFI SPI bus configuration management protocol provides platform
/// specific services that allow the SPI host protocol to reconfigure the
/// switches and multiplexers and set the clock frequency for the SPI bus.
/// This protocol also enables the SPI host protocol to reset an SPI device
/// which may be locking up the SPI bus by holding the clock or data line
/// low.
///
/// The SPI protocol stack uses the concept of an SPI bus configuration as
/// a way to describe a particular state of the switches and multiplexers
/// in the SPI bus.
///
/// A simple SPI bus does not have any multiplexers or switches is described
/// to the SPI protocol stack with a single SPI bus configuration which
/// specifies the SPI bus frequency.
///
/// An SPI bus with switches and multiplexers use an SPI bus configuration
/// to describe each of the unique settings for the switches and multiplexers
/// and the SPI bus frequency.  However the SPI bus configuration management
/// protocol only needs to define the SPI bus configurations that the software
/// uses, which may be a subset of the total.
///
/// The SPI bus configuration description includes a list of SPI devices
/// which may be accessed when this SPI bus configuration is enabled.  SPI
/// devices before a switch or multiplexer must be included in one SPI bus
/// configuration while SPI devices after a switch or multiplexer are on
/// another SPI bus configuration.
///
/// The SPI bus configuration management protocol is an optional protocol.
/// When the SPI bus configuration protocol is not defined the SPI host
/// protocol does not start and the SPI master protocol may be used for
/// other purposes such as SMBus traffic.  When the SPI bus configuration
/// protocol is available, the SPI host protocol uses the SPI bus
/// configuration protocol to call into the platform specific code to set
/// the switches and multiplexers and set the maximum SPI bus frequency.
///
/// The platform designers determine the maximum SPI bus frequency by
/// selecting a frequency which supports all of the SPI devices on the
/// SPI bus for the setting of switches and multiplexers.  The platform
/// designers must validate this against the SPI device data sheets and
/// any limits of the SPI controller or bus length.
///
/// During SPI device enumeration, the SPI bus driver retrieves the SPI
/// bus configuration that must be used to perform SPI transactions to
/// each SPI device.  This SPI bus configuration value is passed into
/// the SPI host protocol to identify the SPI bus configuration required
/// to access a specific SPI device.  The SPI host protocol calls
/// EnableBusConfiguration() to set the switches and multiplexers in the
/// SPI bus and the SPI clock frequency.  The SPI host protocol may
/// optimize calls to EnableBusConfiguration() by only making the call
/// when the SPI bus configuration value changes between SPI requests.
///
/// When SPI transactions are required on the same SPI bus to change the
/// state of multiplexers or switches, the SPI master protocol must be
/// used to perform the necessary SPI transactions.
///
/// It is up to the platform specific code to choose the proper SPI bus
/// configuration when ExitBootServices() is called. Some operating systems
/// are not able to manage the SPI bus configurations and must use the SPI
/// bus configuration that is established by the platform firmware before
/// ExitBootServices() returns.
///
typedef struct _EFI_SPI_BUS_CONFIGURATION_MANAGEMENT_PROTOCOL EFI_SPI_BUS_CONFIGURATION_MANAGEMENT_PROTOCOL;


/**
  Enable access to an SPI bus configuration.

  This routine must be called at or below TPL_NOTIFY.  For synchronous
  requests this routine must be called at or below TPL_CALLBACK.

  Reconfigure the switches and multiplexers in the SPI bus to enable
  access to a specific SPI bus configuration.  Also select the maximum
  clock frequency for this SPI bus configuration.

  This routine uses the SPI Master protocol to perform SPI transactions
  on the local bus.  This eliminates any recursion in the SPI stack for
  configuration transactions on the same SPI bus.  This works because the
  local SPI bus is idle while the SPI bus configuration is being enabled.

  If SPI transactions must be performed on other SPI busses, then the
  EFI_SPI_HOST_PROTOCOL, the EFI_SPI_IO_PROTCOL, or a third party SPI
  driver interface for a specific device must be used.  This requirement
  is because the SPI host protocol controls the flow of requests to the
  SPI controller.  Use the EFI_SPI_HOST_PROTOCOL when the SPI device is
  not enumerated by the EFI_SPI_ENUMERATE_PROTOCOL.  Use a protocol
  produced by a third party driver when it is available or the
  EFI_SPI_IO_PROTOCOL when the third party driver is not available but
  the device is enumerated with the EFI_SPI_ENUMERATE_PROTOCOL.

  When Event is NULL, EnableSpiBusConfiguration operates synchronously
  and returns the SPI completion status as its return value.

  @param[in]  This            Pointer to an EFI_SPI_BUS_CONFIGURATION_MANAGEMENT_PROTOCOL
                              structure.
  @param[in]  SpiBusConfiguration Index of an SPI bus configuration.  All
                                  values in the range of zero to N-1 are
                                  valid where N is the total number of SPI
                                  bus configurations for an SPI bus.
  @param[in]  Event           Event to signal when the transaction is complete
  @param[out] SpiStatus       Buffer to receive the transaction status.

  @return  When Event is NULL, EnableSpiBusConfiguration operates synchrouously
  and returns the SPI completion status as its return value.  In this case it is
  recommended to use NULL for SpiStatus.  The values returned from
  EnableSpiBusConfiguration are:

  @retval EFI_SUCCESS           The asynchronous bus configuration request
                                was successfully started when Event is not
                                NULL.
  @retval EFI_SUCCESS           The bus configuration request completed
                                successfully when Event is NULL.
  @retval EFI_DEVICE_ERROR      The bus configuration failed.
  @retval EFI_NO_MAPPING        Invalid SpiBusConfiguration value

**/
typedef
EFI_STATUS
(EFIAPI *EFI_SPI_BUS_CONFIGURATION_MANAGEMENT_PROTOCOL_ENABLE_SPI_BUS_CONFIGURATION) (
  IN CONST EFI_SPI_BUS_CONFIGURATION_MANAGEMENT_PROTOCOL *This,
  IN UINTN                                               SpiBusConfiguration,
  IN EFI_EVENT                                           Event      OPTIONAL,
  IN EFI_STATUS                                          *SpiStatus OPTIONAL
);

///
/// SPI bus configuration management protocol
///
struct _EFI_SPI_BUS_CONFIGURATION_MANAGEMENT_PROTOCOL {
  ///
  /// Enable an SPI bus configuration for use.
  ///
  EFI_SPI_BUS_CONFIGURATION_MANAGEMENT_PROTOCOL_ENABLE_SPI_BUS_CONFIGURATION EnableSpiBusConfiguration;
};

///
/// Reference to variable defined in the .DEC file
///
extern EFI_GUID gEfiSpiBusConfigurationManagementProtocolGuid;

#endif  //  __SPI_BUS_CONFIGURATION_MANAGEMENT_H__
