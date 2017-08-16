/** @file
  Include file.

  Based on MdePkg/Include/Pi/PiI2c.h

  Copyright (c) 2013, Intel Corporation. All rights reserved.<BR>
  Copyright 2017 NXP

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the bsd
  license which accompanies this distribution. the full text of the license may
  be found at http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PI_SPI_H__
#define __PI_SPI_H__

///
/// SPI controller capabilities
///
/// The EFI_SPI_CONTROLLER_CAPABILITIES specifies the capabilities of the
/// SPI host controller.  The StructureSizeInBytes enables variations of
/// this structure to be identified if there is need to extend this
/// structure in the future.
///
typedef struct {
  ///
  /// Length of this data structure in bytes
  ///
  UINT32 StructureSizeInBytes;

  ///
  /// The maximum number of bytes the SPI host controller is able to
  /// receive from the SPI bus.
  ///
  UINT32 MaximumReceiveBytes;

  ///
  /// The maximum number of bytes the SPI host controller is able to send
  /// on the SPI  bus.
  ///
  UINT32 MaximumTransmitBytes;

  ///
  /// The minimum number of bytes the SPI host controller is able to send
  /// on the SPI  bus.
  ///
  UINT32 MinimumTransmitBytes;
  ///
  /// The minimum number of bytes the SPI host controller is able to
  /// receive from the SPI bus.
  ///
  UINT32 MinimumReceiveBytes;
} EFI_SPI_CONTROLLER_CAPABILITIES;

///
/// SPI device description
///
/// The EFI_SPI_ENUMERATE_PROTOCOL uses the EFI_SPI_DEVICE to describe
/// the platform specific details associated with an SPI device.  This
/// description is passed to the SPI bus driver during enumeration where
/// it is made available to the third party SPI device driver via the
/// EFI_SPI_IO_PROTOCOL.
///
typedef struct {
  ///
  /// Unique value assigned by the silicon manufacture or the third
  /// party SPI driver writer for the SPI part.  This value logically
  /// combines both the manufacture name and the SPI part number into
  /// a single value specified as a GUID.
  ///
  CONST EFI_GUID *DeviceGuid;

  ///
  /// Unique ID of the SPI part within the system
  ///
  UINT32 DeviceIndex;

  ///
  /// Hardware revision - ACPI _HRV value.  See the Advanced
  /// Configuration and Power Interface Specification, Revision 5.0
  /// for the field format and the Plug and play support for SPI
  /// web-page for restriction on values.
  ///
  /// http://www.acpi.info/spec.htm
  /// http://msdn.microsoft.com/en-us/library/windows/hardware/jj131711(v=vs.85).aspx
  ///
  UINT32 HardwareRevision;

  ///
  /// SPI bus configuration for the SPI device
  ///
  UINT32 SpiBusConfiguration;

  ///
  /// Number of slave addresses for the SPI device.
  ///
  UINT32 ChipSelectCount;

  ///
  /// Pointer to the array of slave addresses for the SPI device.
  ///
  CONST UINT32 *ChipSelectArray;
} EFI_SPI_DEVICE;

///
/// SPI device transfer type
///
/// The SPI_TRANSFER_TYPE describes the type of SPI transaction to either
/// send or recievce from SPI bus. some SPI controllers need this information
/// to complete a SPI operation (which consists of one or many transactions)
///
/// for other controllers this field may be left 0 (SPI_TRANSFER_NONE)
///
typedef enum {
  SPI_TRANSFER_NONE = 0,

  /// Command to send to SPI device
  SPI_TRANSFER_CMD,

  /// Address from/to which data is to be read/written
  /// can be 3 bytes (24 bit addressing) or 4 bytes (32 bit addressing) long
  SPI_TRANSFER_ADDR,

  /// Dummy bytes
  /// Some SPI devices need dummy bits which can be used by device to have some
  /// delay between transfers.
  SPI_TRANSFER_DUMMY,

  /// 8 bit (1 bytes) Mode
  /// The data is 1 byte long
  SPI_TRANSFER_MODE,

  /// 2 bit Mode
  /// The data should be considered as 1 byte long
  SPI_TRANSFER_MODE2,

  /// 4 bit Mode
  /// The data should be considered as 1 byte long
  SPI_TRANSFER_MODE4,

  /// Read data Request
  /// Length is number of bytes that are to be read from SPI device
  SPI_TRANSFER_READ,

  /// Write data Request
  /// Length is number of bytes that are to be written to SPI device
  SPI_TRANSFER_WRITE
} SPI_TRANSFER_TYPE;

///
/// SPI device transfer lines
///
/// The SPI_TRANSFER_LINE describes the number of data lines to use for
/// a transaction.
///
typedef enum {
  /// One line transfer
  SPI_TRANSFER_LINE1 = 0,

  /// Two line tranfer
  SPI_TRANSFER_LINE2,

  /// Four line transfer
  SPI_TRANSFER_LINE4,

  /// Eight line transfer
  SPI_TRANSFER_LINE8
} SPI_TRANSFER_LINE;

///
/// SPI device operation
///
/// The EFI_SPI_OPERATION describes a subset of an SPI transaction in which
/// the SPI controller is either sending or receiving bytes from the bus.
/// Some transactions will consist of a single operation while others will
/// be two or more.
///
/// Note: Some SPI controllers do not support read or write ping (address
/// only) operation and will return EFI_UNSUPPORTED status when these
/// operations are requested.
///
/// Note: SPI controllers which do not support complex transactions requiring
/// multiple repeated start bits return EFI_UNSUPPORTED without processing
/// any of the transaction.
///
typedef struct {
  ///
  /// 1x, 2x, 4x, 8x
  ///
  SPI_TRANSFER_LINE Line;

  /// delay afer transaction
  UINT32 DelayUsec;

  /// Some transfer require the frequency different than the standard SPI device
  /// frequency.
  /// if this is 0 then the standard SPI bus frequency configured by EFI_SPI_MASTER_PROTOCOL_SET_BUS_FREQUENCY
  /// is used.
  UINT32 Frequency;

  /// Type of a transaction.
  SPI_TRANSFER_TYPE Type;

  ///
  /// Number of bytes to send to or receive from the SPI device.
  ///
  UINT32 LengthInBytes;

  ///
  /// Pointer to a buffer containing the data to send or to receive from
  /// the SPI device.  The Buffer must be at least LengthInBytes in size.
  ///
  UINT8 *Buffer;
} EFI_SPI_OPERATION;

///
/// SPI device request
///
/// The EFI_SPI_REQUEST_PACKET describes a single SPI transaction.  The
/// transaction starts with a start bit followed by the first operation
/// in the operation array.  Subsequent operations are separated with
/// repeated start bits and the last operation is followed by a stop bit
/// which concludes the transaction.  Each operation is described by one
/// of the elements in the Operation array.
///
typedef struct {
  ///
  /// Number of elements in the operation array
  ///
  UINTN OperationCount;

  ///
  /// Description of the SPI operation
  ///
  EFI_SPI_OPERATION Operation [1];
} EFI_SPI_REQUEST_PACKET;

#endif  //  __PI_SPI_H__
