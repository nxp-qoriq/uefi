/** @file

  Copyright (c) 2017 - 2019, ARM Limited. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Glossary:
    - Cm or CM   - Configuration Manager
    - Obj or OBJ - Object
    - Std or STD - Standard
**/

#ifndef ARM_NAMESPACE_OBJECTS_H_
#define ARM_NAMESPACE_OBJECTS_H_

#include <StandardNameSpaceObjects.h>

#pragma pack(1)

/** The EARM_OBJECT_ID enum describes the Object IDs
    in the ARM Namespace
*/
typedef enum ArmObjectID {
  EArmObjReserved,                     ///<  0 - Reserved
  EArmObjBootArchInfo,                 ///<  1 - Boot Architecture Info
  EArmObjCpuInfo,                      ///<  2 - CPU Info
  EArmObjPowerManagementProfileInfo,   ///<  3 - Power Management Profile Info
  EArmObjGicCInfo,                     ///<  4 - GIC CPU Interface Info
  EArmObjGicDInfo,                     ///<  5 - GIC Distributor Info
  EArmObjGicMsiFrameInfo,              ///<  6 - GIC MSI Frame Info
  EArmObjGicRedistributorInfo,         ///<  7 - GIC Redistributor Info
  EArmObjGicItsInfo,                   ///<  8 - GIC ITS Info
  EArmObjSerialConsolePortInfo,        ///<  9 - Serial Console Port Info
  EArmObjSerialDebugPortInfo,          ///< 10 - Serial Debug Port Info
  EArmObjGenericTimerInfo,             ///< 11 - Generic Timer Info
  EArmObjPlatformGTBlockInfo,          ///< 12 - Platform GT Block Info
  EArmObjGTBlockTimerFrameInfo,        ///< 13 - Generic Timer Block Frame Info
  EArmObjPlatformGenericWatchdogInfo,  ///< 14 - Platform Generic Watchdog
  EArmObjPciConfigSpaceInfo,           ///< 15 - PCI Configuration Space Info
  EArmObjHypervisorVendorIdentity,     ///< 16 - Hypervisor Vendor Id
  EArmObjFixedFeatureFlags,            ///< 17 - Fixed feature flags for FADT
  EArmObjItsGroup,                     ///< 18 - ITS Group
  EArmObjNamedComponent,               ///< 19 - Named Component
  EArmObjRootComplex,                  ///< 20 - Root Complex
  EArmObjSmmuV1SmmuV2,                 ///< 21 - SMMUv1 or SMMUv2
  EArmObjSmmuV3,                       ///< 22 - SMMUv3
  EArmObjPmcg,                         ///< 23 - PMCG
  EArmObjGicItsIdentifierArray,        ///< 24 - GIC ITS Identifier Array
  EArmObjIdMappingArray,               ///< 25 - ID Mapping Array
  EArmObjSmmuInterruptArray,           ///< 26 - SMMU Interrupt Array
  EArmObjProcHierarchyInfo,            ///< 27 - Processor Hierarchy Info
  EArmObjCacheInfo,                    ///< 28 - Cache Info
  EArmObjProcNodeIdInfo,               ///< 29 - Processor Node ID Info
  EArmObjCmRef,                        ///< 30 - CM Object Reference
  EArmObjMemoryAffinityInfo,           ///< 31 - Memory Affinity Info
  EArmObjDeviceHandleAcpi,             ///< 32 - Device Handle Acpi
  EArmObjDeviceHandlePci,              ///< 33 - Device Handle Pci
  EArmObjGenericInitiatorAffinityInfo, ///< 34 - Generic Initiator Affinity
  EArmObjSsdtFixupInfo,
  EArmObjSystemBiosInfoType0,
  EArmObjSystemInfoType1,
  EArmObjSystemChassisInfoType3,
  EArmObjProcessorAttrInfoType4,
  EArmObjCpuCacheDeviceInfoType7,
  EArmObjSystemSlotType9,
  EArmObjMemorryArrayType16,
  EArmObjMemoryDeviceType17,
  EArmObjMemoryMappedAddressType19,
  EArmObjSystemBootType32,
  EArmObjMax
} EARM_OBJECT_ID;

/** A structure that describes the
    Info required for fixing table.

    ID: EArmObjSsdtFixupInfo
*/
typedef struct {
  // This is just a sample, template can be extended.
  UINT32                    Flag;
  UINT32                    MacId;
  UINT32                    PhyId;
  UINT32                    Interrupt;
} CM_ARM_SSDT_FIXUP_INFO;

/** A structure that describes the
    Processor Cache (Type 7) table.

    ID: EArmObjCpuCacheDeviceInfoType7
*/
typedef struct {
  SMBIOS_TABLE_STRING       SocketDesignation;
  UINT16                    CacheConfiguration;
  UINT16                    MaximumCacheSize;
  UINT16                    InstalledSize;
  CACHE_SRAM_TYPE_DATA      SupportedSRAMType;
  CACHE_SRAM_TYPE_DATA      CurrentSRAMType;
  UINT8                     CacheSpeed;
  UINT8                     ErrorCorrectionType;            ///< The enumeration value from CACHE_ERROR_TYPE_DATA.
  UINT8                     SystemCacheType;                ///< The enumeration value from CACHE_TYPE_DATA.
  UINT8                     Associativity;                  ///< The enumeration value from CACHE_ASSOCIATIVITY_DATA.
  UINT32                    MaximumCacheSize2;
  UINT32                    InstalledSize2;
} CM_ARM_CPU_CACHE_DEVICE_TYPE7_INFO;

/** A structure that describes the
    Processor Info (Type 4) table.

    ID: EArmObjProcessorAttrInfoType4
*/
typedef struct {
  SMBIOS_TABLE_STRING   Socket;
  UINT8                 ProcessorType;          ///< The enumeration value from PROCESSOR_TYPE_DATA.
  UINT8                 ProcessorFamily;        ///< The enumeration value from PROCESSOR_FAMILY_DATA.
  SMBIOS_TABLE_STRING   ProcessorManufacture;
  PROCESSOR_ID_DATA     ProcessorId;
  SMBIOS_TABLE_STRING   ProcessorVersion;
  PROCESSOR_VOLTAGE     Voltage;
  UINT16                ExternalClock;
  UINT16                MaxSpeed;
  UINT16                CurrentSpeed;
  UINT8                 Status;
  UINT8                 ProcessorUpgrade;      ///< The enumeration value from PROCESSOR_UPGRADE.
  UINT16                L1CacheHandle;
  UINT16                L2CacheHandle;
  UINT16                L3CacheHandle;
  SMBIOS_TABLE_STRING   SerialNumber;
  SMBIOS_TABLE_STRING   AssetTag;
  SMBIOS_TABLE_STRING   PartNumber;
  UINT8                 CoreCount;
  UINT8                 EnabledCoreCount;
  UINT8                 ThreadCount;
  UINT16                ProcessorCharacteristics;
  UINT16                ProcessorFamily2;
  UINT16                CoreCount2;
  UINT16                EnabledCoreCount2;
  UINT16                ThreadCount2;
} CM_ARM_PROCESSOR_ATTR_TYPE4_INFO;

/** A structure that describes the
    System Info (Type 1) table.

    ID: EArmObjSystemInfoType1
*/
typedef struct {
  SMBIOS_TABLE_STRING     Manufacturer;
  SMBIOS_TABLE_STRING     ProductName;
  SMBIOS_TABLE_STRING     Version;
  SMBIOS_TABLE_STRING     SerialNumber;
  GUID                    Uuid;
  UINT8                   WakeUpType;           ///< The enumeration value from MISC_SYSTEM_WAKEUP_TYPE.
  SMBIOS_TABLE_STRING     SKUNumber;
  SMBIOS_TABLE_STRING     Family;
} CM_ARM_SYSTEM_TYPE1_INFO;

/** A structure that describes the
    System Chassis (Type 3) table.

    ID: EArmObjSystemChassisInfoType3
*/
typedef struct {
  SMBIOS_TABLE_STRING         Manufacturer;
  UINT8                       Type;
  SMBIOS_TABLE_STRING         Version;
  SMBIOS_TABLE_STRING         SerialNumber;
  SMBIOS_TABLE_STRING         AssetTag;
  UINT8                       BootupState;            ///< The enumeration value from MISC_CHASSIS_STATE.
  UINT8                       PowerSupplyState;       ///< The enumeration value from MISC_CHASSIS_STATE.
  UINT8                       ThermalState;           ///< The enumeration value from MISC_CHASSIS_STATE.
  UINT8                       SecurityStatus;         ///< The enumeration value from MISC_CHASSIS_SECURITY_STATE.
  UINT8                       OemDefined[4];
  UINT8                       Height;
  UINT8                       NumberofPowerCords;
  UINT8                       ContainedElementCount;
  UINT8                       ContainedElementRecordLength;
  CONTAINED_ELEMENT           ContainedElements[1];
} CM_ARM_SYSTEM_CHASSIS_TYPE3_INFO;

/** A structure that describes the
    System Bios (Type 0) table.

    ID: EArmObjSystemSlotType9
*/
typedef struct {
  SMBIOS_TABLE_STRING       Vendor;
  SMBIOS_TABLE_STRING       BiosVersion;
  UINT16                    BiosSegment;
  SMBIOS_TABLE_STRING       BiosReleaseDate;
  UINT8                     BiosSize;
  MISC_BIOS_CHARACTERISTICS BiosCharacteristics;
  UINT8                     BIOSCharacteristicsExtensionBytes[2];
  UINT8                     SystemBiosMajorRelease;
  UINT8                     SystemBiosMinorRelease;
  UINT8                     EmbeddedControllerFirmwareMajorRelease;
  UINT8                     EmbeddedControllerFirmwareMinorRelease;
  EXTENDED_BIOS_ROM_SIZE    ExtendedBiosSize;
} CM_ARM_SYSTEM_BIOS_TYPE0_INFO;

/** A structure that describes
    SMBIOS Type32 table.

    ID: EArmObjSystemBootType32
 */
typedef struct CmArmSystemBootType32Info {
  UINT8   BootStatus;
} CM_ARM_SYSTEM_BOOT_TYPE32_INFO;

/** A structure that describes the
    SMBIOS Type19 table.

    ID: EArmObjMemoryMappedAddressType19
*/
typedef struct CmArmMemoryMappedAddressType19Info {
  UINT32    StartingAddress;
  UINT32    EndingAddress;
  UINT16    MemoryArrayHandle;
  UINT8     PartitionWidth;
  UINT64    ExtendedStartingAddress;
  UINT64    ExtendedEndingAddress;
}CM_ARM_MEM_MAP_ADDRESS_TYPE19_INFO;

/** A structure that describes the
    Memory Device (Type 17) table.

    ID: EArmObjMemoryDeviceType17
*/
typedef struct {
  UINT16                     MemoryArrayHandle;
  UINT16                     MemoryErrorInformationHandle;
  UINT16                     TotalWidth;
  UINT16                     DataWidth;
  UINT16                     Size;
  UINT8                      FormFactor;         ///< The enumeration value from MEMORY_FORM_FACTOR.
  UINT8                      DeviceSet;
  SMBIOS_TABLE_STRING        DeviceLocator;
  SMBIOS_TABLE_STRING        BankLocator;
  UINT8                      MemoryType;         ///< The enumeration value from MEMORY_DEVICE_TYPE.
  MEMORY_DEVICE_TYPE_DETAIL  TypeDetail;
  UINT16                     Speed;
  SMBIOS_TABLE_STRING        Manufacturer;
  SMBIOS_TABLE_STRING        SerialNumber;
  SMBIOS_TABLE_STRING        AssetTag;
  SMBIOS_TABLE_STRING        PartNumber;
  UINT8                      Attributes;
  UINT32                     ExtendedSize;
  UINT16                     ConfiguredMemoryClockSpeed;
  UINT16                     MinimumVoltage;
  UINT16                     MaximumVoltage;
  UINT16                     ConfiguredVoltage;
  UINT8                      MemoryTechnology;   ///< The enumeration value from MEMORY_DEVICE_TECHNOLOGY
  MEMORY_DEVICE_OPERATING_MODE_CAPABILITY   MemoryOperatingModeCapability;
  SMBIOS_TABLE_STRING        FirwareVersion;
  UINT16                     ModuleManufacturerID;
  UINT16                     ModuleProductID;
  UINT16                     MemorySubsystemControllerManufacturerID;
  UINT16                     MemorySubsystemControllerProductID;
  UINT64                     NonVolatileSize;
  UINT64                     VolatileSize;
  UINT64                     CacheSize;
  UINT64                     LogicalSize;
  UINT32                     ExtendedSpeed;
  UINT32                     ExtendedConfiguredMemorySpeed;
} CM_ARM_MEMORY_DEVICE_TYPE17_INFO;

/** A structure that describes the
    SMBIOS Type16 table.

    ID: EArmObjMemorryArrayType16
 */
typedef struct CmArmMemoryArrayType16Info {
  UINT8   Location;                     ///< The enumeration value from MEMORY_ARRAY_LOCATION.
  UINT8   Use;                          ///< The enumeration value from MEMORY_ARRAY_USE.
  UINT8   MemoryErrorCorrection;        ///< The enumeration value from MEMORY_ERROR_CORRECTION.
  UINT32  MaximumCapacity;
  UINT16  MemoryErrorInformationHandle;
  UINT16  NumberOfMemoryDevices;
} CM_ARM_MEMORY_ARRAY_TYPE16_INFO;

/** A structure that describes the
    System Slot (Type 9) table.

    ID: EArmObjSystemSlotType9
*/
typedef struct {
  SMBIOS_TABLE_STRING         SlotDesignation;
  UINT8                       SlotType;                 ///< The enumeration value from MISC_SLOT_TYPE.
  UINT8                       SlotDataBusWidth;         ///< The enumeration value from MISC_SLOT_DATA_BUS_WIDTH.
  UINT8                       CurrentUsage;             ///< The enumeration value from MISC_SLOT_USAGE.
  UINT8                       SlotLength;               ///< The enumeration value from MISC_SLOT_LENGTH.
  UINT16                      SlotID;
  MISC_SLOT_CHARACTERISTICS1  SlotCharacteristics1;
  MISC_SLOT_CHARACTERISTICS2  SlotCharacteristics2;
  UINT16                      SegmentGroupNum;
  UINT8                       BusNum;
  UINT8                       DevFuncNum;
} CM_ARM_SYSTEM_SLOT_TYPE9_INFO;

/** A structure that describes the
    ARM Boot Architecture flags.

    ID: EArmObjBootArchInfo
*/
typedef struct CmArmBootArchInfo {
  /** This is the ARM_BOOT_ARCH flags field of the FADT Table
      described in the ACPI Table Specification.
  */
  UINT32  BootArchFlags;
} CM_ARM_BOOT_ARCH_INFO;

typedef struct CmArmCpuInfo {
  // Reserved for use when SMBIOS tables are implemented
} CM_ARM_CPU_INFO;

/** A structure that describes the
    Power Management Profile Information for the Platform.

    ID: EArmObjPowerManagementProfileInfo
*/
typedef struct CmArmPowerManagementProfileInfo {
  /** This is the Preferred_PM_Profile field of the FADT Table
      described in the ACPI Specification
  */
  UINT8  PowerManagementProfile;
} CM_ARM_POWER_MANAGEMENT_PROFILE_INFO;

/** A structure that describes the
    GIC CPU Interface for the Platform.

    ID: EArmObjGicCInfo
*/
typedef struct CmArmGicCInfo {
  /// The GIC CPU Interface number.
  UINT32  CPUInterfaceNumber;

  /** The ACPI Processor UID. This must match the
      _UID of the CPU Device object information described
      in the DSDT/SSDT for the CPU.
  */
  UINT32  AcpiProcessorUid;

  /** The flags field as described by the GICC structure
      in the ACPI Specification.
  */
  UINT32  Flags;

  /** The parking protocol version field as described by
    the GICC structure in the ACPI Specification.
  */
  UINT32  ParkingProtocolVersion;

  /** The Performance Interrupt field as described by
      the GICC structure in the ACPI Specification.
  */
  UINT32  PerformanceInterruptGsiv;

  /** The CPU Parked address field as described by
      the GICC structure in the ACPI Specification.
  */
  UINT64  ParkedAddress;

  /** The base address for the GIC CPU Interface
      as described by the GICC structure in the
      ACPI Specification.
  */
  UINT64  PhysicalBaseAddress;

  /** The base address for GICV interface
      as described by the GICC structure in the
      ACPI Specification.
  */
  UINT64  GICV;

  /** The base address for GICH interface
      as described by the GICC structure in the
      ACPI Specification.
  */
  UINT64  GICH;

  /** The GICV maintenance interrupt
      as described by the GICC structure in the
      ACPI Specification.
  */
  UINT32  VGICMaintenanceInterrupt;

  /** The base address for GICR interface
      as described by the GICC structure in the
      ACPI Specification.
  */
  UINT64  GICRBaseAddress;

  /** The MPIDR for the CPU
      as described by the GICC structure in the
      ACPI Specification.
  */
  UINT64  MPIDR;

  /** The Processor Power Efficiency class
      as described by the GICC structure in the
      ACPI Specification.
  */
  UINT8   ProcessorPowerEfficiencyClass;

  /** Statistical Profiling Extension buffer overflow GSIV. Zero if
      unsupported by this processor. This field was introduced in
      ACPI 6.3 (MADT revision 5) and is therefore ignored when
      generating MADT revision 4 or lower.
  */
  UINT16  SpeOverflowInterrupt;

  /** The proximity domain to which the logical processor belongs.
      This field is used to populate the GICC affinity structure
      in the SRAT table.
  */
  UINT32  ProximityDomain;

  /** The clock domain to which the logical processor belongs.
      This field is used to populate the GICC affinity structure
      in the SRAT table.
  */
  UINT32  ClockDomain;

  /** The GICC Affinity flags field as described by the GICC Affinity structure
      in the SRAT table.
  */
  UINT32  AffinityFlags;
} CM_ARM_GICC_INFO;

/** A structure that describes the
    GIC Distributor information for the Platform.

    ID: EArmObjGicDInfo
*/
typedef struct CmArmGicDInfo {
  /// The Physical Base address for the GIC Distributor.
  UINT64  PhysicalBaseAddress;

  /** The global system interrupt
      number where this GIC Distributor's
      interrupt inputs start.
  */
  UINT32  SystemVectorBase;

  /** The GIC version as described
      by the GICD structure in the
      ACPI Specification.
  */
  UINT8   GicVersion;
} CM_ARM_GICD_INFO;

/** A structure that describes the
    GIC MSI Frame information for the Platform.

    ID: EArmObjGicMsiFrameInfo
*/
typedef struct CmArmGicMsiFrameInfo {
  /// The GIC MSI Frame ID
  UINT32  GicMsiFrameId;

  /// The Physical base address for the MSI Frame
  UINT64  PhysicalBaseAddress;

  /** The GIC MSI Frame flags
      as described by the GIC MSI frame
      structure in the ACPI Specification.
  */
  UINT32  Flags;

  /// SPI Count used by this frame
  UINT16  SPICount;

  /// SPI Base used by this frame
  UINT16  SPIBase;
} CM_ARM_GIC_MSI_FRAME_INFO;

/** A structure that describes the
    GIC Redistributor information for the Platform.

    ID: EArmObjGicRedistributorInfo
*/
typedef struct CmArmGicRedistInfo {
  /** The physical address of a page range
      containing all GIC Redistributors.
  */
  UINT64  DiscoveryRangeBaseAddress;

  /// Length of the GIC Redistributor Discovery page range
  UINT32  DiscoveryRangeLength;
} CM_ARM_GIC_REDIST_INFO;

/** A structure that describes the
    GIC Interrupt Translation Service information for the Platform.

    ID: EArmObjGicItsInfo
*/
typedef struct CmArmGicItsInfo {
  /// The GIC ITS ID
  UINT32  GicItsId;

  /// The physical address for the Interrupt Translation Service
  UINT64  PhysicalBaseAddress;

  /** The proximity domain to which the logical processor belongs.
      This field is used to populate the GIC ITS affinity structure
      in the SRAT table.
  */
  UINT32  ProximityDomain;
} CM_ARM_GIC_ITS_INFO;

/** A structure that describes the
    Serial Port information for the Platform.

    ID: EArmObjSerialConsolePortInfo or
        EArmObjSerialDebugPortInfo
*/
typedef struct CmArmSerialPortInfo {
  /// The physical base address for the serial port
  UINT64  BaseAddress;

  /// The serial port interrupt
  UINT32  Interrupt;

  /// The serial port baud rate
  UINT64  BaudRate;

  /// The serial port clock
  UINT32  Clock;

  /// Serial Port subtype
  UINT16  PortSubtype;
} CM_ARM_SERIAL_PORT_INFO;

/** A structure that describes the
    Generic Timer information for the Platform.

    ID: EArmObjGenericTimerInfo
*/
typedef struct CmArmGenericTimerInfo {
  /// The physical base address for the counter control frame
  UINT64  CounterControlBaseAddress;

  /// The physical base address for the counter read frame
  UINT64  CounterReadBaseAddress;

  /// The secure PL1 timer interrupt
  UINT32  SecurePL1TimerGSIV;

  /// The secure PL1 timer flags
  UINT32  SecurePL1TimerFlags;

  /// The non-secure PL1 timer interrupt
  UINT32  NonSecurePL1TimerGSIV;

  /// The non-secure PL1 timer flags
  UINT32  NonSecurePL1TimerFlags;

  /// The virtual timer interrupt
  UINT32  VirtualTimerGSIV;

  /// The virtual timer flags
  UINT32  VirtualTimerFlags;

  /// The non-secure PL2 timer interrupt
  UINT32  NonSecurePL2TimerGSIV;

  /// The non-secure PL2 timer flags
  UINT32  NonSecurePL2TimerFlags;

  /// GSIV for the virtual EL2 timer
  UINT32  VirtualPL2TimerGSIV;

  /// Flags for the virtual EL2 timer
  UINT32  VirtualPL2TimerFlags;
} CM_ARM_GENERIC_TIMER_INFO;

/** A structure that describes the
    Platform Generic Block Timer Frame information for the Platform.

    ID: EArmObjGTBlockTimerFrameInfo
*/
typedef struct CmArmGTBlockTimerFrameInfo {
  /// The Generic Timer frame number
  UINT8   FrameNumber;

  /// The physical base address for the CntBase block
  UINT64  PhysicalAddressCntBase;

  /// The physical base address for the CntEL0Base block
  UINT64  PhysicalAddressCntEL0Base;

  /// The physical timer interrupt
  UINT32  PhysicalTimerGSIV;

  /** The physical timer flags as described by the GT Block
      Timer frame Structure in the ACPI Specification.
  */
  UINT32  PhysicalTimerFlags;

  /// The virtual timer interrupt
  UINT32  VirtualTimerGSIV;

  /** The virtual timer flags as described by the GT Block
      Timer frame Structure in the ACPI Specification.
  */
  UINT32  VirtualTimerFlags;

  /** The common timer flags as described by the GT Block
      Timer frame Structure in the ACPI Specification.
  */
  UINT32  CommonFlags;
} CM_ARM_GTBLOCK_TIMER_FRAME_INFO;

/** A structure that describes the
    Platform Generic Block Timer information for the Platform.

    ID: EArmObjPlatformGTBlockInfo
*/
typedef struct CmArmGTBlockInfo {
  /// The physical base address for the GT Block Timer structure
  UINT64            GTBlockPhysicalAddress;

  /// The number of timer frames implemented in the GT Block
  UINT32            GTBlockTimerFrameCount;

  /// Reference token for the GT Block timer frame list
  CM_OBJECT_TOKEN   GTBlockTimerFrameToken;
} CM_ARM_GTBLOCK_INFO;

/** A structure that describes the
    SBSA Generic Watchdog information for the Platform.

    ID: EArmObjPlatformGenericWatchdogInfo
*/
typedef struct CmArmGenericWatchdogInfo {
  /// The physical base address of the SBSA Watchdog control frame
  UINT64  ControlFrameAddress;

  /// The physical base address of the SBSA Watchdog refresh frame
  UINT64  RefreshFrameAddress;

  /// The watchdog interrupt
  UINT32  TimerGSIV;

  /** The flags for the watchdog as described by the SBSA watchdog
      structure in the ACPI specification.
  */
  UINT32  Flags;
} CM_ARM_GENERIC_WATCHDOG_INFO;

/** A structure that describes the
    PCI Configuration Space information for the Platform.

    ID: EArmObjPciConfigSpaceInfo
*/
typedef struct CmArmPciConfigSpaceInfo {
  /// The physical base address for the PCI segment
  UINT64  BaseAddress;

  /// The PCI segment group number
  UINT16  PciSegmentGroupNumber;

  /// The start bus number
  UINT8   StartBusNumber;

  /// The end bus number
  UINT8   EndBusNumber;
} CM_ARM_PCI_CONFIG_SPACE_INFO;

/** A structure that describes the
    Hypervisor Vendor ID information for the Platform.

    ID: EArmObjHypervisorVendorIdentity
*/
typedef struct CmArmHypervisorVendorId {
  /// The hypervisor Vendor ID
  UINT64  HypervisorVendorId;
} CM_ARM_HYPERVISOR_VENDOR_ID;

/** A structure that describes the
    Fixed feature flags for the Platform.

    ID: EArmObjFixedFeatureFlags
*/
typedef struct CmArmFixedFeatureFlags {
  /// The Fixed feature flags
  UINT32  Flags;
} CM_ARM_FIXED_FEATURE_FLAGS;

/** A structure that describes the
    ITS Group node for the Platform.

    ID: EArmObjItsGroup
*/
typedef struct CmArmItsGroupNode {
  /// An unique token used to identify this object
  CM_OBJECT_TOKEN   Token;
  /// The number of ITS identifiers in the ITS node
  UINT32            ItsIdCount;
  /// Reference token for the ITS identifier array
  CM_OBJECT_TOKEN   ItsIdToken;
} CM_ARM_ITS_GROUP_NODE;

/** A structure that describes the
    GIC ITS Identifiers for an ITS Group node.

    ID: EArmObjGicItsIdentifierArray
*/
typedef struct CmArmGicItsIdentifier {
  /// The ITS Identifier
  UINT32  ItsId;
} CM_ARM_ITS_IDENTIFIER;

/** A structure that describes the
    Named component node for the Platform.

    ID: EArmObjNamedComponent
*/
typedef struct CmArmNamedComponentNode {
  /// An unique token used to identify this object
  CM_OBJECT_TOKEN   Token;
  /// Number of ID mappings
  UINT32            IdMappingCount;
  /// Reference token for the ID mapping array
  CM_OBJECT_TOKEN   IdMappingToken;

  /// Flags for the named component
  UINT32            Flags;

  /// Memory access properties : Cache coherent attributes
  UINT32            CacheCoherent;
  /// Memory access properties : Allocation hints
  UINT8             AllocationHints;
  /// Memory access properties : Memory access flags
  UINT8             MemoryAccessFlags;

  /// Memory access properties : Address size limit
  UINT8             AddressSizeLimit;
  /** ASCII Null terminated string with the full path to
      the entry in the namespace for this object.
  */
  CHAR8*            ObjectName;
} CM_ARM_NAMED_COMPONENT_NODE;

/** A structure that describes the
    Root complex node for the Platform.

    ID: EArmObjRootComplex
*/
typedef struct CmArmRootComplexNode {
  /// An unique token used to identify this object
  CM_OBJECT_TOKEN   Token;
  /// Number of ID mappings
  UINT32            IdMappingCount;
  /// Reference token for the ID mapping array
  CM_OBJECT_TOKEN   IdMappingToken;

  /// Memory access properties : Cache coherent attributes
  UINT32            CacheCoherent;
  /// Memory access properties : Allocation hints
  UINT8             AllocationHints;
  /// Memory access properties : Memory access flags
  UINT8             MemoryAccessFlags;

  /// ATS attributes
  UINT32            AtsAttribute;
  /// PCI segment number
  UINT32            PciSegmentNumber;
  /// Memory address size limit
  UINT8             MemoryAddressSize;
} CM_ARM_ROOT_COMPLEX_NODE;

/** A structure that describes the
    SMMUv1 or SMMUv2 node for the Platform.

    ID: EArmObjSmmuV1SmmuV2
*/
typedef struct CmArmSmmuV1SmmuV2Node {
  /// An unique token used to identify this object
  CM_OBJECT_TOKEN   Token;
  /// Number of ID mappings
  UINT32            IdMappingCount;
  /// Reference token for the ID mapping array
  CM_OBJECT_TOKEN   IdMappingToken;

  /// SMMU Base Address
  UINT64            BaseAddress;
  /// Length of the memory range covered by the SMMU
  UINT64            Span;
  /// SMMU Model
  UINT32            Model;
  /// SMMU flags
  UINT32            Flags;

  /// Number of context interrupts
  UINT32            ContextInterruptCount;
  /// Reference token for the context interrupt array
  CM_OBJECT_TOKEN   ContextInterruptToken;

  /// Number of PMU interrupts
  UINT32            PmuInterruptCount;
  /// Reference token for the PMU interrupt array
  CM_OBJECT_TOKEN   PmuInterruptToken;

  /// GSIV of the SMMU_NSgIrpt interrupt
  UINT32            SMMU_NSgIrpt;
  /// SMMU_NSgIrpt interrupt flags
  UINT32            SMMU_NSgIrptFlags;
  /// GSIV of the SMMU_NSgCfgIrpt interrupt
  UINT32            SMMU_NSgCfgIrpt;
  /// SMMU_NSgCfgIrpt interrupt flags
  UINT32            SMMU_NSgCfgIrptFlags;
} CM_ARM_SMMUV1_SMMUV2_NODE;

/** A structure that describes the
    SMMUv3 node for the Platform.

    ID: EArmObjSmmuV3
*/
typedef struct CmArmSmmuV3Node {
  /// An unique token used to identify this object
  CM_OBJECT_TOKEN   Token;
  /// Number of ID mappings
  UINT32            IdMappingCount;
  /// Reference token for the ID mapping array
  CM_OBJECT_TOKEN   IdMappingToken;

  /// SMMU Base Address
  UINT64    BaseAddress;
  /// SMMU flags
  UINT32            Flags;
  /// VATOS address
  UINT64            VatosAddress;
  /// Model
  UINT32            Model;
  /// GSIV of the Event interrupt if SPI based
  UINT32            EventInterrupt;
  /// PRI Interrupt if SPI based
  UINT32            PriInterrupt;
  /// GERR interrupt if GSIV based
  UINT32            GerrInterrupt;
  /// Sync interrupt if GSIV based
  UINT32            SyncInterrupt;

  /// Proximity domain flag
  UINT32            ProximityDomain;
  /// Index into the array of ID mapping
  UINT32            DeviceIdMappingIndex;
} CM_ARM_SMMUV3_NODE;

/** A structure that describes the
    PMCG node for the Platform.

    ID: EArmObjPmcg
*/
typedef struct CmArmPmcgNode {
  /// An unique token used to identify this object
  CM_OBJECT_TOKEN   Token;
  /// Number of ID mappings
  UINT32            IdMappingCount;
  /// Reference token for the ID mapping array
  CM_OBJECT_TOKEN   IdMappingToken;

  /// Base Address for performance monitor counter group
  UINT64            BaseAddress;
  /// GSIV for the Overflow interrupt
  UINT32            OverflowInterrupt;
  /// Page 1 Base address
  UINT64            Page1BaseAddress;

  /// Reference token for the IORT node associated with this node
  CM_OBJECT_TOKEN   ReferenceToken;
} CM_ARM_PMCG_NODE;

/** A structure that describes the
    ID Mappings for the Platform.

    ID: EArmObjIdMappingArray
*/
typedef struct CmArmIdMapping {
  /// Input base
  UINT32           InputBase;
  /// Number of input IDs
  UINT32           NumIds;
  /// Output Base
  UINT32           OutputBase;
  /// Reference token for the output node
  CM_OBJECT_TOKEN  OutputReferenceToken;
  /// Flags
  UINT32    Flags;
} CM_ARM_ID_MAPPING;

/** A structure that describes the
    SMMU interrupts for the Platform.

    ID: EArmObjSmmuInterruptArray
*/
typedef struct CmArmSmmuInterrupt {
  /// Interrupt number
  UINT32    Interrupt;

  /// Flags
  UINT32    Flags;
} CM_ARM_SMMU_INTERRUPT;

/** A structure that describes the Processor Hierarchy Node (Type 0) in PPTT

    ID: EArmObjProcHierarchyInfo
*/
typedef struct CmArmProcHierarchyInfo {
  /// A unique token used to identify this object
  CM_OBJECT_TOKEN   Token;
  /// Processor structure flags (ACPI 6.3 - January 2019, PPTT, Table 5-155)
  UINT32            Flags;
  /// Token for the parent CM_ARM_PROC_HIERARCHY_INFO object in the processor
  /// topology. A value of CM_NULL_TOKEN means this node has no parent.
  CM_OBJECT_TOKEN   ParentToken;
  /// Token of the associated CM_ARM_GICC_INFO object which has the
  /// corresponding ACPI Processor ID. A value of CM_NULL_TOKEN means this
  /// node represents a group of associated processors and it does not have an
  /// associated GIC CPU interface.
  CM_OBJECT_TOKEN   GicCToken;
  /// Number of resources private to this Node
  UINT32            NoOfPrivateResources;
  /// Token of the array which contains references to the resources private to
  /// this CM_ARM_PROC_HIERARCHY_INFO instance. This field is ignored if
  /// the NoOfPrivateResources is 0, in which case it is recomended to set
  /// this field to CM_NULL_TOKEN.
  CM_OBJECT_TOKEN   PrivateResourcesArrayToken;
} CM_ARM_PROC_HIERARCHY_INFO;

/** A structure that describes the Cache Type Structure (Type 1) in PPTT

    ID: EArmObjCacheInfo
*/
typedef struct CmArmCacheInfo {
  /// A unique token used to identify this object
  CM_OBJECT_TOKEN   Token;
  /// Reference token for the next level of cache that is private to the same
  /// CM_ARM_PROC_HIERARCHY_INFO instance. A value of CM_NULL_TOKEN means this
  /// entry represents the last cache level appropriate to the processor
  /// hierarchy node structures using this entry.
  CM_OBJECT_TOKEN   NextLevelOfCacheToken;
  /// Size of the cache in bytes
  UINT32            Size;
  /// Number of sets in the cache
  UINT32            NumberOfSets;
  /// Integer number of ways. The maximum associativity supported by
  /// ACPI Cache type structure is limited to MAX_UINT8. However,
  /// the maximum number of ways supported by the architecture is
  /// PPTT_ARM_CCIDX_CACHE_ASSOCIATIVITY_MAX. Therfore this field
  /// is 32-bit wide.
  UINT32            Associativity;
  /// Cache attributes (ACPI 6.3 - January 2019, PPTT, Table 5-156)
  UINT8             Attributes;
  /// Line size in bytes
  UINT16            LineSize;
} CM_ARM_CACHE_INFO;

/** A structure that describes the ID Structure (Type 2) in PPTT

    ID: EArmObjProcNodeIdInfo
*/
typedef struct CmArmProcNodeIdInfo {
  /// A unique token used to identify this object
  CM_OBJECT_TOKEN   Token;
  // Vendor ID (as described in ACPI ID registry)
  UINT32            VendorId;
  /// First level unique node ID
  UINT64            Level1Id;
  /// Second level unique node ID
  UINT64            Level2Id;
  /// Major revision of the node
  UINT16            MajorRev;
  /// Minor revision of the node
  UINT16            MinorRev;
  /// Spin revision of the node
  UINT16            SpinRev;
} CM_ARM_PROC_NODE_ID_INFO;

/** A structure that describes a reference to another Configuration Manager
    object.

    This is useful for creating an array of reference tokens. The framework
    can then query the configuration manager for these arrays using the
    object ID EArmObjCmRef.

    This can be used is to represent one-to-many relationships between objects.

    ID: EArmObjCmRef
*/
typedef struct CmArmObjRef {
  /// Token of the CM object being referenced
  CM_OBJECT_TOKEN   ReferenceToken;
} CM_ARM_OBJ_REF;

/** A structure that describes the Memory Affinity Structure (Type 1) in SRAT

    ID: EArmObjMemoryAffinityInfo
*/
typedef struct CmArmMemoryAffinityInfo {
  /// The proximity domain to which the "range of memory" belongs.
  UINT32            ProximityDomain;

  /// Base Address
  UINT64            BaseAddress;

  /// Length
  UINT64            Length;

  /// Flags
  UINT32            Flags;
} CM_ARM_MEMORY_AFFINITY_INFO;

/** A structure that describes the ACPI Device Handle (Type 0) in the
    Generic Initiator Affinity structure in SRAT

    ID: EArmObjDeviceHandleAcpi
*/
typedef struct CmArmDeviceHandleAcpi {
  /// Hardware ID
  UINT64  Hid;

  /// Unique Id
  UINT32  Uid;
} CM_ARM_DEVICE_HANDLE_ACPI;

/** A structure that describes the PCI Device Handle (Type 1) in the
    Generic Initiator Affinity structure in SRAT

    ID: EArmObjDeviceHandlePci
*/
typedef struct CmArmDeviceHandlePci {
  /// PCI Segment Number
  UINT16  SegmentNumber;

  /// PCI Bus Number - Max 256 busses (Bits 15:8 of BDF)
  UINT8  BusNumber;

  /// PCI Device Mumber - Max 32 devices (Bits 7:3 of BDF)
  UINT8   DeviceNumber;

  /// PCI Function Number - Max 8 functions (Bits 2:0 of BDF)
  UINT8   FunctionNumber;
} CM_ARM_DEVICE_HANDLE_PCI;

/** A structure that describes the Generic Initiator Affinity structure in SRAT

    ID: EArmObjGenericInitiatorAffinityInfo
*/
typedef struct CmArmGenericInitiatorAffinityInfo {
  /// The proximity domain to which the generic initiator belongs.
  UINT32            ProximityDomain;

  /// Flags
  UINT32            Flags;

  /// Device Handle Type
  UINT8             DeviceHandleType;

  /// Reference Token for the Device Handle
  CM_OBJECT_TOKEN   DeviceHandleToken;
} CM_ARM_GENERIC_INITIATOR_AFFINITY_INFO;

#pragma pack()

#endif // ARM_NAMESPACE_OBJECTS_H_
