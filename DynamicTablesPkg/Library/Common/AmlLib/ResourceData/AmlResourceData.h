/** @file
  AML Resource Data.

  Copyright (c) 2019 - 2020, ARM Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Glossary:
  - Rd or RD   - Resource Data
  - Rds or RDS - Resource Data Small
  - Rdl or RDL - Resource Data Large
**/

#ifndef AML_RESOURCE_DATA_H_
#define AML_RESOURCE_DATA_H_

/* This header file does not include internal Node definition,
   i.e. AML_ROOT_NODE, AML_OBJECT_NODE, etc. The node definitions
   must be included by the caller file. The function prototypes must
   only expose AML_NODE_HANDLE, AML_ROOT_NODE_HANDLE, etc. node
   definitions.
   This allows to keep the functions defined here both internal and
   potentially external. If necessary, any function of this file can
   be exposed externally.
   The Api folder is internal to the AmlLib, but should only use these
   functions. They provide a "safe" way to interact with the AmlLib.
*/

#include <AmlInclude.h>

/**
  @defgroup ResourceDataLibrary Resource data library
  @ingroup AMLLib
  @{
    Resource data are defined in the ACPI 6.3 specification,
    s6.4 "Resource Data Types for ACPI". They can be created in ASL via the
    ResourceTemplate () statement, cf s19.3.3 "ASL Resource Templates".

    Resource data can be of the small or large type. The difference between
    small and large resource data elements is their encoding.

    Resource data are stored in the variable list of arguments of object
    nodes.
  @}
*/

/**
  @defgroup ResourceDataStructures Resource data structures
  @ingroup ResourceDataLibrary
  @{
    The resource data APIs rely on enum/define values and structures
    defined here.

    Note: Some large resource data elements can have a variable length
          (e.g.: the AML_RD_PIN_FUNC structure). For such structures,
          some fields have not be defined. Thus, they might require
          additional manipulations to access and check their encoding.
  @}
*/

/**
  @defgroup SmallResourceDataStructures Small resource data structures
  @ingroup ResourceDataStructures
  @{
    Resource data elements can be of two types: small or large. This group
    defines small resource data elements.
  @}
*/

/**
  @defgroup LargeResourceDataStructures Large resource data structures
  @ingroup ResourceDataStructures
  @{
    Resource data elements can be of two types: small or large. This group
    defines large resource data elements.

    Note: Some large resource data elements can have a variable length
          (e.g.: the AML_RD_PIN_FUNC structure). For such structures,
          some fields have not be defined. Thus, they might require
          additional manipulations to access and check their encoding.
  @}
*/

#pragma pack(1)

/** Resource Descriptor header for Small/Large Resource Data Object.
    This is the first byte of a Small/Large Resource Data element.

  @ingroup ResourceDataStructures
*/
typedef UINT8 AML_RD_HEADER;

/** Small resource data generic structure.

  @ingroup ResourceDataStructures
*/
typedef struct AmlRdSmall {
  /// Small resource data header.
  /// This contains the descriptor Id and the length.
  /// Tag Bit  [7]   - Type = 0 (Small Item)
  /// Tag Bits [6:3] - Small item name
  /// Tag Bits [2:0] - Length in bytes
  AML_RD_HEADER   Id;

  /// Bytes 1 to n - Data bytes (Length 0 – 7)
  // UINT8           Data[];
} AML_RD_SMALL;

/** Large resource data generic structure.

  @ingroup ResourceDataStructures
*/
typedef struct AmlRdLarge {
  /// Large resource data header.
  /// Value = 1xxxxxxxB – Type = 1 (Large Item)
  /// Large item name = xxxxxxxB
  AML_RD_HEADER   Id;

  /// Length of data items
  UINT16          Length;

  /// Data items
  // UINT8           Data[];
} AML_RD_LARGE;

/** Small resource data item type.

  @ingroup ResourceDataStructures
*/
#define AML_RD_SMALL_TYPE         0U

/** Large resource data item type.

  @ingroup ResourceDataStructures
*/
#define AML_RD_LARGE_TYPE         BIT7

/** Mask for the small resource data size.

  @ingroup ResourceDataStructures
*/
#define AML_RD_SMALL_SIZE_MASK    (0x7U)

/** Mask for the small resource data ID.

  @ingroup ResourceDataStructures
*/
#define AML_RD_SMALL_ID_MASK      (0xFU << 3)

/** Mask for the large resource data ID.

  @ingroup ResourceDataStructures
*/
#define AML_RD_LARGE_ID_MASK      (0x7FU)

/** Size of a small resource data header.
    (Descriptor Id)

  @ingroup ResourceDataStructures
*/
#define AML_RD_SMALL_HEADER_SIZE  1U

/** Size of a small resource data header.
    (Descriptor Id + Length fields)

  @ingroup ResourceDataStructures
*/
#define AML_RD_LARGE_HEADER_SIZE  3U

/* Resource data IDs.
   To build a descriptor Id, use the associated macros (small/large).
*/

/** Small resource data Ids.

  @ingroup SmallResourceDataStructures
*/
typedef enum EAmlRdSmallId {
  EAmlRdsIdReserved = 0x0,              ///< 0x0 Reserved
  EAmlRdsIdReserved1,                   ///< 0x1 Reserved
  EAmlRdsIdReserved2,                   ///< 0x2 Reserved
  EAmlRdsIdReserved3,                   ///< 0x3 Reserved
  EAmlRdsIdIrqFormat,                   ///< 0x4 IRQ Format
  EAmlRdsIdDmaFormat,                   ///< 0x5 DMA Format
  EAmlRdsIdStartDepFunc,                ///< 0x6 Start Dependent Functions
  EAmlRdsIdEndDepFunc,                  ///< 0x7 End Dependent Functions
  EAmlRdsIdIoPort,                      ///< 0x8 I/O Port
  EAmlRdsIdFixedLocIoPort,              ///< 0x9 Fixed Location I/O Port
  EAmlRdsIdFixedDma,                    ///< 0xA Fixed DMA
  EAmlRdsIdReserved4,                   ///< 0xB Reserved
  EAmlRdsIdReserved5,                   ///< 0xC Reserved
  EAmlRdsIdReserved6,                   ///< 0xD Reserved
  EAmlRdsIdVendorDefined,               ///< 0xE Vendor Defined
  EAmlRdsIdEndTag,                      ///< 0xF End Tag
  EAmlRdsIdMax                          ///< Max enum
} EAML_RD_SMALL_ID;

/** Large resource data Ids.

  @ingroup LargeResourceDataStructures
*/
typedef enum EAmlRdlargeId {
  EAmlRdlIdReserved = 0x00,             ///< 0x00 Reserved
  EAmlRdlId24BitMemoryRange,            ///< 0x01 24-Bit Memory Range
  EAmlRdlIdGenericRegister,             ///< 0x02 Generic Register
  EAmlRdlIdReserved1,                   ///< 0x03 Reserved
  EAmlRdlIdVendorDefined,               ///< 0x04 Vendor-Defined
  EAmlRdlId32BitMemoryRange,            ///< 0x05 32-Bit Memory Range
  EAmlRdlId32BitFixedMemoryRange,       ///< 0x06 32-Bit Fixed Memory Range
  EAmlRdlIdAddressSpaceResource,        ///< 0x07 Address Space Resource
  EAmlRdlIdWordAddressSpace,            ///< 0x08 Word Space Resource
  EAmlRdlIdExtendedInterrupt,           ///< 0x09 Extended Interrupt
  EAmlRdlIdQwordAddressSpace,           ///< 0x0A QWord Address Space
  EAmlRdlIdExtendedAddressSpace,        ///< 0x0B Extended Address Space
  EAmlRdlIdGpioConnection,              ///< 0x0C GPIO Connection
  EAmlRdlIdPinFunc,                     ///< 0x0D Pin Function
  EAmlRdlIdGenericSerialBusConnection,  ///< 0x0E Generic Serial Bus Connection
  EAmlRdlIdPinConf,                     ///< 0x0F Pin Configuration
  EAmlRdlIdPinGroup,                    ///< 0x10 Pin Group
  EAmlRdlIdPinGroupFunc,                ///< 0x11 Pin Group Function
  EAmlRdlIdPinGroupConf,                ///< 0x12 Pin Group Configuration
  EAmlRdlIdMax                          ///< 0x13-0x7F Reserved
} EAML_LARGE_RD_ID;

/** IRQ Descriptor
    Type 0, Small Item Name 0x4, Length = 2 or 3

  @ingroup SmallResourceDataStructures
*/
typedef struct AmlRdIrqFormat {
  /// Small resource data header.
  /// Value = 0x22 or 0x23 (0010001nB) – Type = 0
  /// Small item name = 0x4, Length = 2 or 3
  AML_RD_HEADER  Id;

  /// IRQ mask bits[16:0]
  UINT16         IrqMask;

  /// IRQ Information
  UINT8          IrqInformation;
} AML_RD_IRQ_FORMAT;

/** DMA Descriptor
    Type 0, Small Item Name 0x5, Length = 2

  @ingroup SmallResourceDataStructures
*/
typedef struct AmlRdDmaFormat {
  /// Small resource data header.
  /// Value = 0x2A (00101010B) – Type = 0
  /// Small item name = 0x5, Length = 2
  AML_RD_HEADER  Id;

  /// DMA channel mask bits [7:0] (channels 0 – 7)
  UINT8          DmaMask;

  /// DMA Information
  UINT8          DmaInformation;
} AML_RD_DMA_FORMAT;

/** Start Dependent Functions Descriptor
    Type 0, Small Item Name 0x6, Length = 0 or 1

  @ingroup SmallResourceDataStructures
*/
typedef struct AmlRdStartDepFunc {
  /// Small resource data header.
  /// Value = 0x30 or 0x31 (0011000nB) – Type = 0
  /// small item name = 0x6, Length = 0 or 1
  AML_RD_HEADER  Id;

  /// Start Dependent Function Priority Byte.
  /// This field is optional.
  UINT8          PriorityByte;
} AML_RD_START_DEP_FUNC;

/** End Dependent Functions Descriptor
    Type 0, Small Item Name 0x7, Length = 0

  @ingroup SmallResourceDataStructures
*/
typedef struct AmlRdEndDepFunc {
  /// Small resource data header.
  /// Value = 0x38 (00111000B) – Type = 0
  /// Small item name = 0x7, Length =0
  AML_RD_HEADER  Id;
} AML_RD_END_DEP_FUNC;

/** I/O Port Descriptor
    Type 0, Small Item Name 0x8, Length = 7

  @ingroup SmallResourceDataStructures
*/
typedef struct AmlRdIoPort {
  /// Small resource data header.
  /// Value = 0x47 (01000111B) –Type = 0
  /// Small item name = 0x8, Length = 7
  /// I/O Port Descriptor
  AML_RD_HEADER  Id;

  /// Information
  UINT8          Information;

  /// Range minimum base address
  UINT16         RangeMinimumBaseAddress;

  /// Range maximum base address
  UINT16         RangemaximumBaseAddress;

  /// Base alignment
  UINT8          BaseAlignment;

  /// Range length
  UINT8          RangeLength;
} AML_RD_IO_PORT;

/** Fixed Location I/O Port Descriptor
    Type 0, Small Item Name 0x9, Length = 3

  @ingroup SmallResourceDataStructures
*/
typedef struct AmlRdFixedLocIoPort {
  /// Small resource data header.
  /// Value = 0x4B (01001011B) –Type = 0
  /// Small item name = 0x9, Length = 3
  /// Fixed Location I/O Port Descriptor
  AML_RD_HEADER  Id;

  /// Range base address
  UINT16         RangeBaseAddress;

  /// Range length
  UINT8          RangeLength;
} AML_RD_FIXED_LOC_IO_PORT;

/** Fixed DMA Descriptor
    Type 0, Small Item Name 0xA, Length = 5

  @ingroup SmallResourceDataStructures
*/
typedef struct AmlRdFixedDma {
  /// Small resource data header.
  /// Value = 0x55 (01010101B) – Type = 0
  /// Small item name = 0xA, Length = 0x5
  AML_RD_HEADER  Id;

  /// DMA Request Line
  UINT16         DmaRequestLine;

  /// DMA Channel
  UINT16         DmaChannel;

  /// DMA Transfer Width
  UINT8          DmaTransferWidth;
} AML_RD_FIXED_DMA;

/** Vendor-Defined Descriptor
    Type 0, Small Item Name 0xE, Length = 1 to 7

  @ingroup SmallResourceDataStructures
*/
typedef struct AmlRdSmallVendorDefined {
  /// Small resource data header.
  /// Value = 0x71 – 0x77 (01110nnnB) – Type = 0
  /// small item name = 0xE, Length = 1–7
  AML_RD_HEADER  Id;

  /// Vendor defined data
  UINT8          Data[7];
} AML_RD_SMALL_VENDOR_DEFINED;

/** End Tag
    Type 0, Small Item Name 0xF, Length = 1
    The End tag identifies an end of resource data.

  @ingroup SmallResourceDataStructures
*/
typedef struct AmlRdEndTag {
  /// Small resource data header.
  /// Value = 0x79 (01111001B) – Type = 0
  /// Small item name = 0xF, Length = 1
  AML_RD_HEADER  Id;

  /// Checksum covering all resource data after the
  /// serial identifier. This checksum is generated
  /// such that adding it to the sum of all the data
  /// bytes will produce a zero sum.
  /// If the checksum field is zero, the resource data
  /// is treated as if the checksum operation succeeded
  /// and the configuration proceeds normally.
  UINT8          CheckSum;
} AML_RD_END_TAG;

/* Large resource data structures
*/

/** 24-Bit Memory Range Descriptor
    Type 1, Large Item Value 0x01

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRd24BitMemoryRange {
  /// Large resource data header.
  /// Value = 0x81 (10000001B) – Type = 1
  /// Large item name = 0x01
  AML_RD_HEADER  Id;

  /// Length
  UINT16         Length;

  /// Information
  UINT8          Information;

  /// Range Minimum Base
  UINT16         RangeMinimumBaseAddress;

  /// Range Maximum Base
  UINT16         RangeMaximumBaseAddress;

  /// Base Alignment
  UINT16         BaseAlignment;

  /// Range Length
  UINT16         RangeLength;
} AML_RD_24_BIT_MEMORY_RANGE;

/** Generic Register Descriptor
    Type 1, Large Item Value 0x02

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRdGenericRegister {
  /// Large resource data header.
  /// Value = 0x82 (10000010B)
  /// Type = 1, Large item name = 0x02
  AML_RD_HEADER  Id;

  /// Length
  UINT16         Length;

  /// Address Space ID,
  UINT8          AddressSpaceId;

  /// Register Bit Width
  UINT8          RegisterBitWidth;

  /// Register Bit Offset
  UINT8          RegisterBitOffset;

  /// Access Size
  UINT8          AccessSize;

  /// Register Address
  UINT64         RegisterAddress;
} AML_RD_GENERIC_REGISTER;

/** Vendor-Defined Descriptor
    Type 1, Large Item Value 0x04

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRdLargeVendorDefined {
  /// Large resource data header.
  /// Value = 0x84 (10000100B) – Type = 1
  /// Large item name = 0x04
  AML_RD_HEADER  Id;

  /// Length
  UINT16         Length;

  /// UUID Specific Descriptor Sub-Type
  UINT8          UUIDSubType;

  /// UUID
  UINT8          UUID[16];

  /// Vendor Defined Data
  // UINT8          Data[];  // Variable length data.
} AML_RD_LARGE_VENDOR_DEFINED;

/** 32-Bit Memory Range Descriptor
    Type 1, Large Item Value 0x05

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRd32BitMemoryRange {
  /// Large resource data header.
  /// Value = 0x85 (10000101B) – Type = 1
  /// Large item name = 0x05
  AML_RD_HEADER  Id;

  /// Length
  UINT16         Length;

  /// Information
  UINT8          Information;

  /// Range Maximum Base Address
  UINT32         RangeMinimumBaseAddress;

  /// Range Minimum Base Address
  UINT32         RangeMaximumBaseAddress;

  /// Base alignment
  UINT32         BaseAlignment;

  /// Range Length
  UINT32         RangeLength;
} AML_RD_32_BIT_MEMORY_RANGE;

/** 32-Bit Fixed Memory Range Descriptor
    Type 1, Large Item Value 0x06

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRd32BitFixedMemoryRange {
  /// Large resource data header.
  /// Value = 0x86 (10000110B) – Type = 1
  /// Large item name = 0x06
  AML_RD_HEADER  Id;

  /// Length
  UINT16         Length;

  /// Information
  UINT8          Information;

  /// Range Base Address
  UINT32         RangeBaseAddress;

  /// Range Length
  UINT32         RangeLength;
} AML_RD_32_BIT_FIXED_MEMORY_RANGE;

/** Address Space Resource Descriptors
    Type 1, Large Item Value 0x07

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRdAddressSpaceResource {
  /// Large resource data header.
  /// Value = 0x88 (10001000B) – Type = 1
  /// Large item name = 0x08
  AML_RD_HEADER  Id;

  /// Length
  UINT16         Length;

  /// Resource Type
  UINT8          ResourceType;

  /// General Flags
  UINT8          GeneralFlags;

  /// Type Specific Flags
  UINT8          TypeSpecificFlags;

  /// Address Space Granularity
  UINT32         AddressSpaceGranularity;

  /// Address Range Minimum
  UINT32         AddressRangeMinimum;

  /// Address Range Maximum
  UINT32         AddressRangeMaximum;

  /// Address Translation Offset
  UINT32         AddressTranslationOffset;

  /// Address Length
  UINT32         AddressLength;

  /// Resource Source Index
  UINT8          ResourceSourceIndex;

  /// Resource Source
  // CHAR8          ResourceSource[];  // NULL terminated string.
} AML_RD_ADDRESS_SPACE_RESOURCE;

/** Word Address Space Descriptor
    Type 1, Large Item Value 0x08

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRdWordAddressSpace {
  /// Large resource data header.
  /// Value = 0x88 (10001000B) – Type = 1
  /// Large item name = 0x08
  AML_RD_HEADER  Id;

  /// Length
  UINT16         Length;

  /// Resource Type
  UINT8          ResourceType;

  /// General Flags
  UINT8          GeneralFlags;

  /// Type Specific Flags
  UINT8          TypeSpecificFlags;

  /// Address Space Granularity
  UINT16         AddressSpaceGranularity;

  /// Address Range Minimum
  UINT16         AddressRangeMinimum;

  /// Address Range Maximum
  UINT16         AddressRangeMaximum;

  /// Address Translation Offset
  UINT16         AddressTranslationOffset;

  /// Address Length
  UINT16         AddressLength;

  /// Resource Source Index
  UINT8          ResourceSourceIndex;

  /// Resource Source
  // CHAR8          ResourceSource[];  // NULL terminated string.
} AML_RD_WORD_ADDRESS_SPACE;

/** Extended Interrupt Descriptor
    Type 1, Large Item Value 0x09

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRdExtendedInterrupt {
  /// Large resource data header.
  /// Value = 0x89 (10001001B) – Type = 1
  /// Large item name = 0x09
  AML_RD_HEADER  Id;

  /// Length
  UINT16         Length;

  /// Interrupt Vector Flags
  UINT8          InterruptVectorFlags;

  /// Interrupt table length
  UINT8          InterruptTableLength;

  /// Data
  // UINT8          Data[];
/*
  The size of the InterruptNumber field is not specified in the
  ACPI specification 6.3., s6.4. It occults the fields that follow.
  It has been replaced by a Data field.
  /// Interrupt Number
  UINT32         InterruptNumber[n];      // No size specified.

  /// Resource Source Index
  UINT8          ResourceSourceIndex;

  /// Resource Source
  CHAR8          ResourceSource[n];       // NULL terminated string.
*/
} AML_RD_EXTENDED_INTERRUPT;

/** QWord Address Space Descriptor
    Type 1, Large Item Value 0x0A

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRdQwordAddressSpace {
  /// Large resource data header.
  /// Value = 0x8A (10001010B) – Type = 1
  /// Large item name = 0x0A
  AML_RD_HEADER  Id;

  /// Length
  UINT16         Length;

  /// Resource Type
  UINT8          ResourceType;

  /// General Flags
  UINT8          GeneralFlags;

  /// Type Specific Flags
  UINT8          TypeSpecificFlags;

  /// Address Space Granularity
  UINT64         AddressSpaceGranularity;

  /// Address Range Minimum
  UINT64         AddressRangeMinimum;

  /// Address Range Maximum
  UINT64         AddressRangeMaximum;

  /// Address Translation Offset
  UINT64         AddressTranslationOffset;

  /// Address Length
  UINT64         AddressLength;

  /// Resource Source Index
  UINT8          ResourceSourceIndex;

  /// Resource Source
  // CHAR8          ResourceSource[];  // NULL terminated string.
} AML_RD_QWORD_ADDRESS_SPACE;

/** Extended Address Space Descriptor
    Type 1, Large Item Value 0x0B

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRdExtendedAddressSpace {
  /// Large resource data header.
  /// Value = 0x8B (10001011B) – Type = 1
  /// Large item name = 0x0B
  AML_RD_HEADER  Id;

  /// Length
  UINT16         Length;

  /// Resource Type
  UINT8          ResourceType;

  /// General Flags
  UINT8          GeneralFlags;

  /// Type Specific Flags
  UINT8          TypeSpecificFlags;

  /// Revision ID
  UINT8          RevisionId;

  /// Reserved
  UINT8          Reserved;

  /// Address Space Granularity
  UINT64         AddressSpaceGranularity;

  /// Address Range Minimum
  UINT64         AddressRangeMinimum;

  /// Address Range Maximum
  UINT64         AddressRangeMaximum;

  /// Address Translation Offset
  UINT64         AddressTranslationOffset;

  /// Address Length
  UINT64         AddressLength;

  /// Type Specific Attribute
  UINT64         TypeSpecificAttribute;
} AML_RD_EXTENDED_ADDRESS_SPACE;

/** GPIO Connection Descriptor
    Type 1, Large Item Name 0x0C

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRdGpioConnection {
  /// Large resource data header.
  /// Value = 0x8C, (10001100B) – Type = 1
  /// Large item name = 0x0C
  AML_RD_HEADER  Id;

  /// Length
  UINT16         Length;

  /// Revision ID
  UINT8          RevisionId;

  /// GPIO Connection Type
  UINT8          GpioConnectionType;

  /// General Flags
  UINT16         GeneralFlags;

  /// Interrupt and IO Flags
  UINT16         InterruptIoFlags;

  /// Pin Configuration
  UINT8          PinConfiguration;

  /// Output Drive Strength
  UINT16         OutputDriveStrength;

  /// Debounce timeout
  UINT16         DebounceTimeout;

  /// Pin Table Offset
  UINT16         PinTableOffset;

  /// Resource Source Index
  UINT8          ResourceSourceIndex;

  /// Resource Source Name Offset
  UINT16         ResourceSourceNameOffset;

  /// Vendor Data Offset
  UINT16         VendorDataOffset;

  /// Vendor Data Length
  UINT16         VendorDataLength;

  /// Data
  // UINT8          Data[];
/*
  The size of the PinNumber field is not specified in the
  ACPI specification 6.3., s6.4. It occults the fields that follow.
  It has been replaced by a Data field.
  /// Pin Number
  UINT16         PinNumber[n];       // No size specified.

  /// Resource Source
  CHAR8          ResourceSource[n];  // NULL terminated string.

  /// Vendor-defined Data
  UINT16         VendorDataOffset;
*/
} AML_RD_GPIO_CONNECTION;

/** Pin Function Descriptor
    Type 1, Large Item Value 0x0D

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRdPinFunc {
  /// Large resource data header.
  /// Value = 0x8D, (10001101B) – Type = 1
  /// Large item name = 0x0D
  AML_RD_HEADER  Id;

  /// Length
  UINT16         Length;

  /// Revision ID
  UINT8          RevisionId;

  /// Flags
  UINT16         Flags;

  /// Pin Pull Configuration
  UINT8          PinPullConfiguration;

  /// Function Number
  UINT16         FunctionNumber;

  /// Pin Table Offset
  UINT16         PinTableOffset;

  /// Resource Source Index
  UINT8          ResourceSourceIndex;

  /// Resource Source Name Index
  UINT16         ResourceSourceNameIndex;

  /// Vendor Data Offset
  UINT16         VendorDataOffset;

  /// Vendor Data Length
  UINT16         VendorDataLength;

  /// Data
  // UINT8          Data[];
/*
  The size of the PinNumber field is not specified in the
  ACPI specification 6.3., s6.4. It occults the fields that follow.
  It has been replaced by a Data field.
  /// Pin Number
  UINT16        PinNumber[n];       // No size specified.

  /// Resource Source
  CHAR8         ResourceSource[n];  // NULL terminated string.

  /// Vendor-defined Data
  UINT16        VendorDataOffset;
*/
} AML_RD_PIN_FUNC;

/** Fields common to all serial bus connections.

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRdSerialBusConnectionCommon {
  /// Resource Identifier
  AML_RD_HEADER  Id;

  /// Length
  UINT16         Length;

  /// Revision ID
  UINT8          RevisionId;

  /// Resource Source Index
  UINT8          ResourceSourceIndex;

  /// Serial Bus Type
  UINT8          SerialBusType;

  /// General Flags
  UINT8          GeneralFlags;

  /// Type Specific Flags
  UINT16         TypeSpecificFlags;

  /// Type Specific Revision ID
  UINT8          TypeSpecificRevisionId;

  /// Type Data Length
  UINT16         TypeDataLength;
} AML_RD_SERIAL_BUS_CONNECTION_COMMON;


/** GenericSerialBus Connection Descriptor
    Type 1, Large Item Value 0x0E

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRdGenericSerialBusConnection {
  /// Common serial bus connection fields
  /// Byte 0: Serial Bus Type
  /// Value = 0x8E (10001110B) – Type = 1
  /// Large item name = 0x0E
  AML_RD_SERIAL_BUS_CONNECTION_COMMON  Common;

  /// Data
  // UINT8                                Data[];
/*
  The size of the TypeSpecificData field is not specified in the
  ACPI specification 6.3., s6.4. It occults the fields that follow.
  It has been replaced by a Data field.
  /// Vendor Defined Data
  UINT8                                Data[n];            // No size specified.

  /// Resource Source
  CHAR8                                ResourceSource[n];  // NULL terminated string.
*/
} AML_RD_GENERIC_SERIAL_BUS_CONNECTION;

/** I2C Serial Bus Connection Resource Descriptor
    Type 1, Large Item Value 0x0E

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRdI2cBusConnection {
  /// Common serial bus connection fields
  /// Byte 0: I2C Bus Connection Descriptor
  /// Value = 0x8E (10001110B) – Type = 1
  /// Large item name = 0x0E
  AML_RD_SERIAL_BUS_CONNECTION_COMMON  Common;

  // I2C specific fields.
  /// Connection Speed
  UINT32                               ConnectionSpeed;

  /// Slave Address
  UINT16                               SlaveAddress;

  /// Data
  // UINT8                                Data[];
/*
  The size of the Data field is not specified in the
  ACPI specification 6.3., s6.4. It occults the fields that follow.
  It has been replaced by a Data field.
  /// Vendor Defined Data
  UINT8                                Data[n];            // No size specified.

  /// Resource Source
  CHAR8                                ResourceSource[n];  // NULL terminated string.
*/
} AML_RD_I2C_SERIAL_BUS_CONNECTION;

/** SPI Serial Bus Connection Descriptor
    Type 1, Large Item Value 0x0E

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRdSpiSerialBusConnection {
  /// Common serial bus connection fields
  /// Byte 0: SPI Bus Connection Descriptor
  /// Value = 0x8E (10001110B) – Type = 1
  /// Large item name = 0x0E
  AML_RD_SERIAL_BUS_CONNECTION_COMMON  Common;

  // SPI specific fields.
  /// Connection Speed
  UINT32                               ConnectionSpeed;

  /// Data Bit Length
  UINT8                                DataBitLength;

  /// Phase
  UINT8                                Phase;

  /// Polarity
  UINT8                                Polarity;

  /// Device Selection
  UINT16                               DeviceSelection;

  /// Data
  // UINT8                                Data[];
/*
  The size of the Data field is not specified in the
  ACPI specification 6.3., s6.4. It occults the fields that follow.
  It has been replaced by a Data field.
  /// Vendor Defined Data
  UINT8                                Data[n];            // No size specified.

  /// Resource Source
  CHAR8                                ResourceSource[n];  // NULL terminated string.
*/
} AML_RD_SPI_SERIAL_BUS_CONNECTION;

/** UART Serial Bus Connection Resource Descriptor
    Type 1, Large Item Value 0x0E

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRdUartSerialBusConnection {
  /// Common serial bus connection fields
  /// Byte 0: Serial Bus Connection Descriptor
  /// Value = 0x8E (10001110B) – Type = 1
  /// Large item name = 0x0E
  AML_RD_SERIAL_BUS_CONNECTION_COMMON  Common;

  // UART specific fields.
  /// Default Baud rate
  UINT32                               DefaultBaudRate;

  /// Rx FIFO
  UINT16                               RxFifo;

  /// Tx FIFO
  UINT16                               TxFifo;

  /// Parity
  UINT8                                Parity;

  /// Serial Lines Enabled
  UINT8                                SerialLinesEnabled;

  /// Data
  // UINT8                                Data[];
/*
  The size of the Data field is not specified in the
  ACPI specification 6.3., s6.4. It occults the fields that follow.
  It has been replaced by a Data field.
  /// Vendor Defined Data
  UINT8                                Data[n];            // No size specified.

  /// Resource Source
  CHAR8                                ResourceSource[n];  // NULL terminated string.
*/
} AML_RD_UART_SERIAL_BUS_CONNECTION;

/** Pin Configuration Descriptor
    Type 1, Large Item Value 0x0F

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRdPinConf {
  /// Resource Identifier
  /// Value = 0x8F, (10001111B) – Type = 1
  /// Large item name = 0x0F
  AML_RD_HEADER  Id;

  /// Length
  UINT16         Length;

  /// Revision ID
  UINT8          RevisionId;

  /// Flags
  UINT16         Flags;

  /// Pin Configuration Type
  UINT8          PinConfType;

  /// Pin Configuration Value
  UINT32         PinConfValue;

  /// Pin Table Offset
  UINT16         PinTableOffset;

  /// Resource Source Index
  UINT8          ResourceSourceIndex;

  /// Resource Source Name Offset
  UINT16         ResourceSourceNameOffset;

  /// Vendor Data Offset
  UINT16         VendorDataOffset;

  /// Vendor Data Length
  UINT16         VendorDataLength;

  /// Data
  // UINT8          Data[];
/*
  The size of the PinNumber field is not specified in the
  ACPI specification 6.3., s6.4. It occults the fields that follow.
  It has been replaced by a Data field.
  /// Pin Number
  UINT16         PinNumber[n];       // No size specified.

  /// Resource Source
  CHAR8          ResourceSource[n];  // NULL terminated string.

  /// Vendor-defined Data
  UINT16         VendorDataOffset;
*/
} AML_RD_PIN_CONF;

/** Pin Group Descriptor
    Type 1, Large Item Value 0x10

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRdPinGroup {
  /// Resource Identifier
  /// Value = 0x90, (10010000B) – Type = 1
  /// Large item name = 0x10
  AML_RD_HEADER  Id;

  /// Length
  UINT16         Length;

  /// Revision ID
  UINT8          RevisionId;

  /// Flags
  UINT16         Flags;

  /// Pin Table Offset
  UINT16         PinTableOffset;

  /// Resource Label Offset
  UINT16         ResourceLabelOffset;

  /// Vendor Data Offset
  UINT16         VendorDataOffset;

  /// Vendor Data Length
  UINT16         VendorDataLength;

  /// Data
  // UINT8          Data[];
/*
  The size of the PinNumber field is not specified in the
  ACPI specification 6.3., s6.4. It occults the fields that follow.
  It has been replaced by a Data field.
  /// Pin Number
  UINT16         PinNumber[n];      // No size specified.

  /// Resource Label
  CHAR8          ResourceLabel[n];  // NULL terminated string.

  /// Vendor-defined Data
  UINT16         VendorDataOffset;
*/
} AML_RD_PIN_GROUP;

/** Pin Group Function Descriptor
    Type 1, Large Item Value 0x11

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRdPinGroupFunc {
  /// Resource Identifier
  /// Value = 0x91, (10010001B) – Type = 1
  /// Large item name = 0x11
  AML_RD_HEADER  Id;

  /// Length
  UINT16         Length;

  /// Revision ID
  UINT8          RevisionId;

  /// Flags
  UINT16         Flags;

  /// Function number
  UINT16         FunctionNumber;

  /// Resource Source Index
  UINT8          ResourceSourceIndex;

  /// Resource Source Name Index
  UINT16         ResourceSourceNameIndex;

  /// Resource Source Label Offset
  UINT16         ResourceSourceLabelOffset;

  /// Vendor Data Offset
  UINT16         VendorDataOffset;

  /// Vendor Data Length
  UINT16         VendorDataLength;

  /// Data
  // UINT8          Data[];
/*
  The size of the ResourceSource field is not specified in the
  ACPI specification 6.3., s6.4. It occults the fields that follow.
  It has been replaced by a Data field.
  /// Resource Source
  CHAR8          ResourceSource[n];       // NULL terminated string.

  /// Resource Source Label
  CHAR8          ResourceSourceLabel[n];  // NULL terminated string.

  /// Vendor-defined Data
  UINT16         VendorDataOffset;
*/
} AML_RD_PIN_GROUP_FUNC;

/** Pin Group Configuration Descriptor
    Type 1, Large Item Value 0x12

  @ingroup LargeResourceDataStructures
*/
typedef struct AmlRdPinGroupConf {
  /// Resource Identifier
  /// Value = 0x92, (10010001B) – Type = 1
  /// Large item name = 0x12
  AML_RD_HEADER  Id;

  /// Length
  UINT16         Length;

  /// Revision ID
  UINT8          RevisionId;

  /// Flags
  UINT16         Flags;

  /// Pin Configuration Type
  UINT8          PinConfType;

  /// Pin Configuration Value
  UINT32         PinConfValue;

  /// Resource Source Index
  UINT8          ResourceSourceIndex;

  /// Resource Source Name Offset
  UINT16         ResourceSourceNameOffset;

  /// Resource Source Label Offset
  UINT16         ResourceSourceLabelOffset;

  /// Vendor Data Offset
  UINT16         VendorDataOffset;

  /// Vendor Data Length
  UINT16         VendorDataLength;

  /// Data
  // UINT8          Data[];
/*
  The size of the ResourceSource field is not specified in the
  ACPI specification 6.3., s6.4. It occults the fields that follow.
  It has been replaced by a Data field.
  /// Resource Source
  CHAR8          ResourceSource[n];       // NULL terminated string.

  /// Resource Source Label
  CHAR8          ResourceSourceLabel[n];  // NULL terminated string.

  /// Vendor-defined Data
  UINT16         VendorDataOffset;
*/
} AML_RD_PIN_GROUP_CONF;

#pragma pack()

/**
  @defgroup ResourceDataApis Resource data APIs
  @ingroup ResourceDataLibrary
  @{
    Resource data APIs allow to manipulate/decode resource data elements.
  @}
*/

/** Check whether a resource data is of the large type.

  @ingroup ResourceDataApis

  @param  [in]  Header  Pointer to the first byte of a resource data.

  @retval TRUE  If the resource data is of the large type.
  @retval FALSE Otherwise.
**/
#define AML_RD_IS_LARGE(Header)                                               \
          ((*((CONST AML_RD_HEADER*)Header) & AML_RD_LARGE_TYPE) ==           \
             AML_RD_LARGE_TYPE)

/** Build a small resource data descriptor Id.
    The small/large bit is included in the descriptor Id,
    but the size bits are not included.

  @ingroup ResourceDataApis

  @param  [in]  Id  Descriptor Id.

  @return A descriptor Id.
**/
#define AML_RD_BUILD_SMALL_DESC_ID(Id)                                        \
          ((AML_RD_HEADER)(AML_RD_SMALL_TYPE | (((Id) & 0xF) << 3)))

/** Build a large resource data descriptor Id.
    The small/large bit is included in the descriptor Id.

  @ingroup ResourceDataApis

  @param  [in]  Id  Id of the descriptor.

  @return A descriptor Id.
**/
#define AML_RD_BUILD_LARGE_DESC_ID(Id)                                        \
          ((AML_RD_HEADER)(AML_RD_LARGE_TYPE | Id))

/** Check whether the resource data has the input descriptor Id.

  The small/large bit is included in the descriptor Id,
  but the size bits are not included for small resource data elements.

  @ingroup ResourceDataApis

  @param  [in]  Header        Pointer to the first byte of a resource data
                              element.
  @param  [in]  DescriptorId  The descriptor to check against.

  @retval TRUE    The resource data has the descriptor Id.
  @retval FALSE   Otherwise.
**/
BOOLEAN
EFIAPI
AmlRdCompareDescId (
  IN  CONST AML_RD_HEADER   * Header,
  IN        AML_RD_HEADER     DescriptorId
  );

/** Get the descriptor Id of the resource data.

  The small/large bit is included in the descriptor Id,
  but the size bits are not included for small resource data elements.

  @ingroup ResourceDataApis

  @param  [in]  Header  Pointer to the first byte of a resource data.

  @return A descriptor Id.
**/
AML_RD_HEADER
EFIAPI
AmlRdGetDescId (
  IN  CONST AML_RD_HEADER   * Header
  );

/** Get the size of a resource data element.

  If the resource data element is of the large type, the Header
  is expected to be at least 3 bytes long.

  @ingroup ResourceDataApis

  @param  [in]  Header  Pointer to the first byte of a resource data.

  @return The size of the resource data element.
**/
UINT32
EFIAPI
AmlRdGetSize (
  IN  CONST AML_RD_HEADER   * Header
  );

#endif // AML_RESOURCE_DATA_H_
