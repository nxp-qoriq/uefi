#/** LS1046A board package.
#
#    Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.
#
#    This program and the accompanying materials
#    are licensed and made available under the terms and conditions of the BSD License
#    which accompanies this distribution. The full text of the license may be found at
#    http://opensource.org/licenses/bsd-license.php
#
#    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#**/

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  #
  # Defines for default states.  These can be changed on the command line.
  # -D FLAG=VALUE
  #

[LibraryClasses.common]
  ArmLib|ArmPkg/Library/ArmLib/ArmBaseLib.inf
  ArmMmuLib|ArmPkg/Library/ArmMmuLib/ArmMmuBaseLib.inf
  ArmCpuLib|ArmPkg/Drivers/ArmCpuLib/ArmCortexAEMv8Lib/ArmCortexAEMv8Lib.inf
  ArmSmcLib|ArmPkg/Library/ArmSmcLib/ArmSmcLib.inf

  ArmGenericTimerCounterLib|ArmPkg/Library/ArmGenericTimerPhyCounterLib/ArmGenericTimerPhyCounterLib.inf
  TimerLib|ArmPkg/Library/ArmArchTimerLib/ArmArchTimerLib.inf

  LS1046aSocLib|LS1046aRdbPkg/Library/LS1046aSocLib/LS1046aSocLib.inf
  ArmTrustZoneLib|LS1046aRdbPkg/Library/ArmTrustZone/ArmTrustZone.inf
  CpldLib|LS1046aRdbPkg/Library/CpldLib/CpldLib.inf
  ArmPlatformLib|LS1046aRdbPkg/Library/LS1046aRdbLib/LS1046aRdbLib.inf
  ArmPlatformStackLib|ArmPlatformPkg/Library/ArmPlatformStackLib/ArmPlatformStackLib.inf
  Dpaa1Lib|LS1046aRdbPkg/Library/Dpaa1Lib/Dpaa1Lib.inf

  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf

  FileExplorerLib|MdeModulePkg/Library/FileExplorerLib/FileExplorerLib.inf

  BootLogoLib|MdeModulePkg/Library/BootLogoLib/BootLogoLib.inf
  PlatformBootManagerLib|ArmPkg/Library/PlatformBootManagerLib/PlatformBootManagerLib.inf

  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf
  CustomizedDisplayLib|MdeModulePkg/Library/CustomizedDisplayLib/CustomizedDisplayLib.inf

# Networking Requirements
  NetLib|MdeModulePkg/Library/DxeNetLib/DxeNetLib.inf
  DpcLib|MdeModulePkg/Library/DxeDpcLib/DxeDpcLib.inf
  UdpIoLib|MdeModulePkg/Library/DxeUdpIoLib/DxeUdpIoLib.inf
  IpIoLib|MdeModulePkg/Library/DxeIpIoLib/DxeIpIoLib.inf


  # ARM PL310 L2 Cache Driver
  L2X0CacheLib|ArmPlatformPkg/Drivers/PL310L2Cache/PL310L2CacheSec.inf
  # ARM PL301 Axi Driver
  PL301AxiLib|ArmPlatformPkg/Drivers/PL301Axi/PL301Axi.inf
  # ARM GIC400 General Interrupt Driver
  ArmGicLib|ArmPkg/Drivers/ArmGic/ArmGicLib.inf
  ArmGicArchLib|ArmPkg/Library/ArmGicArchLib/ArmGicArchLib.inf

!if $(TARGET) == RELEASE
  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  UncachedMemoryAllocationLib|ArmPkg/Library/UncachedMemoryAllocationLib/UncachedMemoryAllocationLib.inf
!else
  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  UncachedMemoryAllocationLib|ArmPkg/Library/UncachedMemoryAllocationLib/UncachedMemoryAllocationLib.inf
!endif

  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf

  # I2c Library
  I2cLib|LS1046aRdbPkg/Library/I2cLib/I2cLib.inf

  # Ddr Library
  DdrLib|LS1046aRdbPkg/Library/DdrLib/DdrLib.inf

  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf

  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  BaseMemoryLib|ArmPkg/Library/BaseMemoryLibStm/BaseMemoryLibStm.inf

  EfiResetSystemLib|LS1046aRdbPkg/Library/ResetSystemLib/ResetSystemLib.inf

  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf

  EfiFileLib|EmbeddedPkg/Library/EfiFileLib/EfiFileLib.inf

  PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf

  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf

  PeCoffExtraActionLib|ArmPkg/Library/DebugPeCoffExtraActionLib/DebugPeCoffExtraActionLib.inf

  CacheMaintenanceLib|ArmPkg/Library/ArmCacheMaintenanceLib/ArmCacheMaintenanceLib.inf
  DefaultExceptionHandlerLib|ArmPkg/Library/DefaultExceptionHandlerLib/DefaultExceptionHandlerLib.inf
  CpuExceptionHandlerLib|ArmPkg/Library/ArmExceptionLib/ArmExceptionLib.inf
  PrePiLib|EmbeddedPkg/Library/PrePiLib/PrePiLib.inf
  RealTimeClockLib|LS1046aRdbPkg/Library/Pcf2129RtcLib/Pcf2129RtcLib.inf
  SemihostLib|ArmPkg/Library/SemihostLib/SemihostLib.inf

  # UART Driver
  DUartPortLib|LS1046aRdbPkg/Drivers/DUart/DUart.inf
  SerialPortLib|LS1046aRdbPkg/Library/DUartPortLib/DUartPortLib.inf

  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf

  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf

  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf

  NorFlashLib|LS1046aRdbPkg/Library/IfcNorFlashLib/IfcNorFlashLib.inf
  NandFlashLib|LS1046aRdbPkg/Library/IfcNandFlashLib/IfcNandFlashLib.inf
  FileSystemLib|LS1046aRdbPkg/Library/LS1046aFileSystemLib/LS1046aFileSystemLib.inf
  DspiFlashLib|LS1046aRdbPkg/Library/DspiFlashLib/DspiFlashLib.inf
  PciHostBridgeLib|LS1046aRdbPkg/Library/PciHostBridgeLib/PciHostBridgeLib.inf
  SdxcLib|LS1046aRdbPkg/Library/SdxcLib/SdxcLib.inf
!if $(BOOT_VIA_QSPI_FLASH) == TRUE
  QspiFlashLib|LS1046aRdbPkg/Library/QspiFlashLib/QspiFlashLib.inf
!endif

  # DPAA1 Ethernet
  Dpaa1EthernetMacLib|LS1046aRdbPkg/Library/Dpaa1EthernetMacLib/Dpaa1EthernetMacLib.inf
  Dpaa1EthernetPhyLib|LS1046aRdbPkg/Library/Dpaa1EthernetPhyLib/Dpaa1EthernetPhyLib.inf
  Dpaa1BoardSpecificLib|LS1046aRdbPkg/Library/Dpaa1BoardSpecificLib/Dpaa1BoardSpecificLib.inf

  #
  # Allow dynamic PCDs
  #
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf

  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf

  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf

  CpuLib|MdePkg/Library/BaseCpuLib/BaseCpuLib.inf

  ArmDisassemblerLib|ArmPkg/Library/ArmDisassemblerLib/ArmDisassemblerLib.inf
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
  DmaLib|ArmPkg/Library/ArmDmaLib/ArmDmaLib.inf

  BdsLib|ArmPkg/Library/BdsLib/BdsLib.inf
  FdtLib|EmbeddedPkg/Library/FdtLib/FdtLib.inf

  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf

  NetLib|MdeModulePkg/Library/DxeNetLib/DxeNetLib.inf
  HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf

  BcfgCommandLib|ShellPkg/Library/UefiShellBcfgCommandLib/UefiShellBcfgCommandLib.inf

  TpmMeasurementLib|MdeModulePkg/Library/TpmMeasurementLibNull/TpmMeasurementLibNull.inf
  AuthVariableLib|MdeModulePkg/Library/AuthVariableLibNull/AuthVariableLibNull.inf
  VarCheckLib|MdeModulePkg/Library/VarCheckLib/VarCheckLib.inf

  # Gpio Library
  GpioLib|LS1046aRdbPkg/Library/GpioLib/GpioLib.inf

  SecureMonRngLib|LS1046aRdbPkg/Library/SecureMonRngLib/SecureMonRngLib.inf

[LibraryClasses.common.SEC]
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  ReportStatusCodeLib|IntelFrameworkModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  ExtractGuidedSectionLib|EmbeddedPkg/Library/PrePiExtractGuidedSectionLib/PrePiExtractGuidedSectionLib.inf
  LzmaDecompressLib|IntelFrameworkModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf

  # Temp work around for Movt relocation issue.
  #PeCoffLib|ArmPkg/Library/BasePeCoffLib/BasePeCoffLib.inf
  PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf

  HobLib|EmbeddedPkg/Library/PrePiHobLib/PrePiHobLib.inf
  PrePiHobListPointerLib|ArmPlatformPkg/Library/PrePiHobListPointerLib/PrePiHobListPointerLib.inf
  MemoryAllocationLib|EmbeddedPkg/Library/PrePiMemoryAllocationLib/PrePiMemoryAllocationLib.inf
  PerformanceLib|MdeModulePkg/Library/PeiPerformanceLib/PeiPerformanceLib.inf
  PlatformPeiLib|ArmPlatformPkg/PlatformPei/PlatformPeiLib.inf
  MemoryInitPeiLib|ArmPlatformPkg/MemoryInitPei/MemoryInitPeiLib.inf

  # 1/123 faster than Stm or Vstm version
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf

  # Uncomment to turn on GDB stub in SEC.
  #DebugAgentLib|EmbeddedPkg/Library/GdbDebugAgent/GdbDebugAgent.inf

[LibraryClasses.common.PEI_CORE]
  PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  ReportStatusCodeLib|IntelFrameworkModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf

[LibraryClasses.common.DXE_CORE]
  HobLib|MdePkg/Library/DxeCoreHobLib/DxeCoreHobLib.inf
  MemoryAllocationLib|MdeModulePkg/Library/DxeCoreMemoryAllocationLib/DxeCoreMemoryAllocationLib.inf
  DxeCoreEntryPoint|MdePkg/Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
  ReportStatusCodeLib|IntelFrameworkModulePkg/Library/DxeReportStatusCodeLibFramework/DxeReportStatusCodeLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  PeCoffLib|EmbeddedPkg/Library/DxeHobPeCoffLib/DxeHobPeCoffLib.inf

  PerformanceLib|MdeModulePkg/Library/DxeCorePerformanceLib/DxeCorePerformanceLib.inf

[LibraryClasses.common.DXE_DRIVER]
  ReportStatusCodeLib|IntelFrameworkModulePkg/Library/DxeReportStatusCodeLibFramework/DxeReportStatusCodeLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  SecurityManagementLib|MdeModulePkg/Library/DxeSecurityManagementLib/DxeSecurityManagementLib.inf
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  MemoryInitPeiLib|ArmPlatformPkg/MemoryInitPei/MemoryInitPeiLib.inf

[LibraryClasses.common.UEFI_APPLICATION]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  ReportStatusCodeLib|IntelFrameworkModulePkg/Library/DxeReportStatusCodeLibFramework/DxeReportStatusCodeLib.inf
  UefiDecompressLib|IntelFrameworkModulePkg/Library/BaseUefiTianoCustomDecompressLib/BaseUefiTianoCustomDecompressLib.inf
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf

[LibraryClasses.common.UEFI_DRIVER]
  ReportStatusCodeLib|IntelFrameworkModulePkg/Library/DxeReportStatusCodeLibFramework/DxeReportStatusCodeLib.inf
  UefiDecompressLib|IntelFrameworkModulePkg/Library/BaseUefiTianoCustomDecompressLib/BaseUefiTianoCustomDecompressLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  ReportStatusCodeLib|IntelFrameworkModulePkg/Library/DxeReportStatusCodeLibFramework/DxeReportStatusCodeLib.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  PeCoffLib|EmbeddedPkg/Library/DxeHobPeCoffLib/DxeHobPeCoffLib.inf

[LibraryClasses.AARCH64]
  #
  # It is not possible to prevent the ARM compiler for generic intrinsic functions.
  # This library provides the instrinsic functions generate by a given compiler.
  # [LibraryClasses.ARM] and NULL mean link this library into all ARM images.
  #
  NULL|ArmPkg/Library/CompilerIntrinsicsLib/CompilerIntrinsicsLib.inf

[BuildOptions]
  XCODE:*_*_ARM_PLATFORM_FLAGS == -arch armv7

  GCC:*_*_ARM_PLATFORM_FLAGS == -march=armv7-a
 # GCC:*_GCC49_AARCH64_CC_FLAGS = -mcmodel=small

  RVCT:*_*_ARM_PLATFORM_FLAGS == --cpu cortex-a9

[BuildOptions.common.EDKII.DXE_RUNTIME_DRIVER]
  GCC:*_*_ARM_DLINK_FLAGS = -z common-page-size=0x1000
  GCC:*_*_AARCH64_DLINK_FLAGS = -z common-page-size=0x10000

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsFeatureFlag.common]
  ## If TRUE, Graphics Output Protocol will be installed on virtual handle created by ConsplitterDxe.
  #  It could be set FALSE to save size.
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutGopSupport|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutUgaSupport|FALSE
  gEfiMdePkgTokenSpaceGuid.PcdComponentNameDisable|TRUE
  gEfiMdePkgTokenSpaceGuid.PcdDriverDiagnosticsDisable|TRUE
  gEfiMdePkgTokenSpaceGuid.PcdComponentName2Disable|TRUE
  gEfiMdePkgTokenSpaceGuid.PcdDriverDiagnostics2Disable|TRUE

  #
  # Control what commands are supported from the UI
  # Turn these on and off to add features or save size
  #
  gEmbeddedTokenSpaceGuid.PcdEmbeddedMacBoot|TRUE
  gEmbeddedTokenSpaceGuid.PcdEmbeddedDirCmd|TRUE
  gEmbeddedTokenSpaceGuid.PcdEmbeddedHobCmd|TRUE
  gEmbeddedTokenSpaceGuid.PcdEmbeddedHwDebugCmd|TRUE
  gEmbeddedTokenSpaceGuid.PcdEmbeddedPciDebugCmd|TRUE
  gEmbeddedTokenSpaceGuid.PcdEmbeddedIoEnable|FALSE
  gEmbeddedTokenSpaceGuid.PcdEmbeddedScriptCmd|FALSE

  gEmbeddedTokenSpaceGuid.PcdCacheEnable|TRUE

  # Use the Vector Table location in CpuDxe. We will not copy the Vector Table at PcdCpuVectorBaseAddress
  gArmTokenSpaceGuid.PcdRelocateVectorTable|FALSE

  gEmbeddedTokenSpaceGuid.PcdPrePiProduceMemoryTypeInformationHob|TRUE
  gArmTokenSpaceGuid.PcdCpuDxeProduceDebugSupport|FALSE

  gEfiMdeModulePkgTokenSpaceGuid.PcdTurnOffUsbLegacySupport|TRUE

[PcdsDynamicDefault.common]
  #
  # Set video resolution for boot options and for text setup.
  # PlatformDxe can set the former at runtime.
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoHorizontalResolution|800
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoVerticalResolution|600
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoHorizontalResolution|640
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoVerticalResolution|480

  #
  # ARM General Interrupt Controller
  gArmTokenSpaceGuid.PcdGicDistributorBase|0x1410000
  gArmTokenSpaceGuid.PcdGicInterruptInterfaceBase|0x1420000

[PcdsDynamicHii.common.DEFAULT]
  gEfiMdePkgTokenSpaceGuid.PcdPlatformBootTimeOut|L"Timeout"|gEfiGlobalVariableGuid|0x0|10

[PcdsFixedAtBuild.common]
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x2000
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxAuthVariableSize|0x2800

  gEfiMdeModulePkgTokenSpaceGuid.PcdResetOnMemoryTypeInformationChange|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdBootManagerMenuFile|{ 0x21, 0xaa, 0x2c, 0x46, 0x14, 0x76, 0x03, 0x45, 0x83, 0x6e, 0x8a, 0xb6, 0xf4, 0x66, 0x23, 0x31 }
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdShellFile|{ 0x83, 0xA5, 0x04, 0x7C, 0x3E, 0x9E, 0x1C, 0x4F, 0xAD, 0x65, 0xE0, 0x52, 0x68, 0xD0, 0xB4, 0xD1 }

  gArmPlatformTokenSpaceGuid.PcdFirmwareVendor|"LS1046a RDB board"
  gEmbeddedTokenSpaceGuid.PcdEmbeddedPrompt|"LS1046a"

  gArmPlatformTokenSpaceGuid.PcdCoreCount|1 # Only one core
  gArmPlatformTokenSpaceGuid.PcdCounterFrequency|12000000 #12Mhz

  #
  # NV Storage PCDs.
  #
  gArmTokenSpaceGuid.PcdVFPEnabled|1
!if $(BOOT_VIA_QSPI_FLASH) == TRUE
  gLS1046aRdbTokenSpaceGuid.PcdFlashDeviceBase64|0x040000000
  gLS1046aRdbTokenSpaceGuid.PcdFlashReservedRegionBase64|0x040300000
!else
  gLS1046aRdbTokenSpaceGuid.PcdFlashDeviceBase64|0x060000000
  gLS1046aRdbTokenSpaceGuid.PcdFlashReservedRegionBase64|0x60200000
!endif

  gEfiMdePkgTokenSpaceGuid.PcdMaximumUnicodeStringLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdMaximumAsciiStringLength|2000000
  gEfiMdePkgTokenSpaceGuid.PcdMaximumLinkedListLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdSpinLockTimeout|10000000
  gEfiMdePkgTokenSpaceGuid.PcdDebugClearMemoryValue|0xAF
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|1
  #gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|0 # turn off for now
  gEfiMdePkgTokenSpaceGuid.PcdPostCodePropertyMask|0
  gEfiMdePkgTokenSpaceGuid.PcdUefiLibMaxPrintBufferSize|320

# DEBUG_ASSERT_ENABLED       0x01
# DEBUG_PRINT_ENABLED        0x02
# DEBUG_CODE_ENABLED         0x04
# CLEAR_MEMORY_ENABLED       0x08
# ASSERT_BREAKPOINT_ENABLED  0x10
# ASSERT_DEADLOOP_ENABLED    0x20
!if $(TARGET) == RELEASE
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x27
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x81000000 #0x8000000F #Print almost everything
!else
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2F
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x81000044 #0x8000000F #Print almost everything
!endif

#  DEBUG_INIT      0x00000001  // Initialization
#  DEBUG_WARN      0x00000002  // Warnings
#  DEBUG_LOAD      0x00000004  // Load events
#  DEBUG_FS        0x00000008  // EFI File system
#  DEBUG_POOL      0x00000010  // Alloc & Free's
#  DEBUG_PAGE      0x00000020  // Alloc & Free's
#  DEBUG_INFO      0x00000040  // Verbose
#  DEBUG_DISPATCH  0x00000080  // PEI/DXE Dispatchers
#  DEBUG_VARIABLE  0x00000100  // Variable
#  DEBUG_BM        0x00000400  // Boot Manager
#  DEBUG_BLKIO     0x00001000  // BlkIo Driver
#  DEBUG_NET       0x00004000  // SNI Driver
#  DEBUG_UNDI      0x00010000  // UNDI Driver
#  DEBUG_LOADFILE  0x00020000  // UNDI Driver
#  DEBUG_EVENT     0x00080000  // Event messages
#  DEBUG_ERROR     0x80000000  // Error

  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x07

  gEmbeddedTokenSpaceGuid.PcdEmbeddedAutomaticBootCommand|""
  gEmbeddedTokenSpaceGuid.PcdEmbeddedDefaultTextColor|0x07
  gEmbeddedTokenSpaceGuid.PcdEmbeddedMemVariableStoreSize|0x10000

#
# Optional feature to help prevent EFI memory map fragments
# Turned on and off via: PcdPrePiProduceMemoryTypeInformationHob
# Values are in EFI Pages (4K). DXE Core will make sure that
# at least this much of each type of memory can be allocated
# from a single memory range. This way you only end up with
# maximum of two fragements for each type in the memory map
# (the memory used, and the free memory that was prereserved
# but not used).
#
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiACPIReclaimMemory|0
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiACPIMemoryNVS|0
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiReservedMemoryType|0
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiRuntimeServicesData|80
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiRuntimeServicesCode|40
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiBootServicesCode|400
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiBootServicesData|3000
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiLoaderCode|10
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiLoaderData|0

 # PCI PCDs
 #
  gLS1046aRdbTokenSpaceGuid.PcdKludgeMapPciMmioAsCached|FALSE
  gLS1046aRdbTokenSpaceGuid.PcdPciBusMin|0
  gLS1046aRdbTokenSpaceGuid.PcdPciBusMax|255
  gLS1046aRdbTokenSpaceGuid.PcdPciIoBase|0x00010000
  gLS1046aRdbTokenSpaceGuid.PcdPciIoSize|0x00010000      # 64k
  gLS1046aRdbTokenSpaceGuid.PcdPci1Mmio64Base|0x4040000000
  gLS1046aRdbTokenSpaceGuid.PcdPci2Mmio64Base|0x4840000000
  gLS1046aRdbTokenSpaceGuid.PcdPci3Mmio64Base|0x5040000000
  gLS1046aRdbTokenSpaceGuid.PcdPciMmio64Size|0x40000000
  gLS1046aRdbTokenSpaceGuid.PcdPci1ExpressBaseAddress|0x4000000000
  gLS1046aRdbTokenSpaceGuid.PcdPci2ExpressBaseAddress|0x4800000000
  gLS1046aRdbTokenSpaceGuid.PcdPci3ExpressBaseAddress|0x5000000000
  gLS1046aRdbTokenSpaceGuid.PcdPciDebug|FALSE

  #
  # ARM Primecells
  #

  ## Serial Terminal
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x21c0500
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultBaudRate|115200

  gEfiMdePkgTokenSpaceGuid.PcdDefaultTerminalType|4

  #
  # ARM L2x0 PCDs
  gArmTokenSpaceGuid.PcdL2x0ControllerBase|0x10900000

  #
  # LS1046a board Specific PCDs
  # XX (DRAM - Region 1 2GB)
  # (NOR - IFC Region 1 512MB)
  gArmTokenSpaceGuid.PcdSystemMemoryBase|0x80000000
  gArmTokenSpaceGuid.PcdSystemMemorySize|0x80000000

  #
  # LS1046a Soc Specific PCDs
  #
  gArmPlatformTokenSpaceGuid.PcdCounterFrequencyReal|TRUE
  gArmPlatformTokenSpaceGuid.PcdCsuInitialize|TRUE
  gArmPlatformTokenSpaceGuid.PcdTzc380Initialize|TRUE
  gArmPlatformTokenSpaceGuid.PcdCci400Initialize|TRUE
  gArmPlatformTokenSpaceGuid.PcdClockInitialize|TRUE

  #
  # LS1046a SoC specific DPAA1 PCDs
  #
  gLS1046aRdbTokenSpaceGuid.PcdDpaa1DebugFlags|0x00

  # Enable MEMACS 1, 2, 3, 4, 5, 6, 9
  #
  gLS1046aRdbTokenSpaceGuid.PcdDpaa1UsedMemacsMask|0x13F
  
!if $(BOOT_VIA_QSPI_FLASH) == TRUE
  gLS1046aRdbTokenSpaceGuid.PcdFManFwSrc|2
!endif

  #
  # SD Specific PCDs
  #
  gLS1046aRdbTokenSpaceGuid.PcdSdxcDmaSupported|TRUE

  #
  # IP Testing specific PCDs
  #
  gLS1046aRdbTokenSpaceGuid.PcdDspiTest|FALSE
!if $(BOOT_VIA_QSPI_FLASH) == TRUE
  gLS1046aRdbTokenSpaceGuid.PcdQspiTest|FALSE
  gLS1046aRdbTokenSpaceGuid.PcdQspiSwap|FALSE
!endif
  gLS1046aRdbTokenSpaceGuid.PcdDdrBistTest|FALSE

  #
  # PPA specific PCDs
  #
  gLS1046aRdbTokenSpaceGuid.PcdPpaNorBaseAddr|0x60400000
  gLS1046aRdbTokenSpaceGuid.PcdPpaQspiBaseAddr|0x44400000
  gLS1046aRdbTokenSpaceGuid.PcdPpaFwSize|0x8000000 # (128MB)
  gLS1046aRdbTokenSpaceGuid.PcdPpaImageSize|0x100000

  # Size of the region used by UEFI in permanent memory (Reserved 16MB)
  gArmPlatformTokenSpaceGuid.PcdSystemMemoryUefiRegionSize|0x02000000

  # Size of the region reserved for fixed address allocations (Reserved 32MB)
  gArmTokenSpaceGuid.PcdArmLinuxKernelMaxOffset|0x08000000
  gArmTokenSpaceGuid.PcdArmLinuxFdtMaxOffset|0x08000000
  gArmTokenSpaceGuid.PcdArmLinuxFdtAlignment|0x0

  gArmTokenSpaceGuid.PcdCpuVectorBaseAddress|0x94A00000
  gArmTokenSpaceGuid.PcdCpuResetAddress|0x94A00000

  # Timer
  gArmTokenSpaceGuid.PcdArmArchTimerFreqInHz|0
  gEmbeddedTokenSpaceGuid.PcdTimerPeriod|10000 # expressed in 100ns units, 100,000 x 100 ns = 10,000,000 ns = 10 ms
  gArmPlatformTokenSpaceGuid.PcdArmArchTimerVirtIntrNum|11 # Virtual PPI
  gArmPlatformTokenSpaceGuid.PcdArmArchTimerHypIntrNum|10 # Hypervisor PPI
  gArmPlatformTokenSpaceGuid.PcdArmArchTimerSecIntrNum|13 # Physical Secure PPI
  gArmPlatformTokenSpaceGuid.PcdArmArchTimerIntrNum|14 # Physical Non-Secure PPI
  gEmbeddedTokenSpaceGuid.PcdMetronomeTickPeriod|1000
  #gEmbeddedTokenSpaceGuid.PcdEmbeddedPerformanceCounterPeriodInNanoseconds|77
  #gEmbeddedTokenSpaceGuid.PcdEmbeddedPerformanceCounterFrequencyInHz|13000000

  # We want to use the Shell Libraries but don't want it to initialise
  # automatically. We initialise the libraries when the command is called by the
  # Shell.
  gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE

  #
  # ARM Pcds
  #
  gArmTokenSpaceGuid.PcdArmUncachedMemoryMask|0x0000000040000000

  gArmPlatformTokenSpaceGuid.PcdDefaultBootDescription|L"EFI Linux from NOR flash"
  gArmPlatformTokenSpaceGuid.PcdDefaultBootDevicePath|L"MemoryMapped(0x0,0x61100000,0x6111FFFF)"
!if $(BOOT_VIA_QSPI_FLASH) == TRUE
  gEmbeddedTokenSpaceGuid.PcdFdtDevicePaths|L"MemoryMapped(0x0,0x40F00000 ,0x40FFFFFF)/fsl-ls1046a-rdb.dtb"
!else
  gEmbeddedTokenSpaceGuid.PcdFdtDevicePaths|L"MemoryMapped(0x0,0x61000000,0x610FFFFF)/fsl-ls1046a-rdb.dtb"
!endif

  gArmPlatformTokenSpaceGuid.PcdDefaultBootArgument|L"MemoryMapped(0x0,0x61120000,0x61AFFFFF) -f \"MemoryMapped(0x0,0x61C00000,0x638FFFFF)\" -c \"console=ttyS0,115200 root=/dev/ram0 earlycon=uart8250,0x21c0500\""

  # PPA
  gLS1046aRdbTokenSpaceGuid.PcdPpaFitConfiguration|"config@1"

  # Use the serial console for both ConIn & ConOut
  gArmPlatformTokenSpaceGuid.PcdDefaultConOutPaths|L"VenHw(D3987D4B-971A-435F-8CAF-4967EB627241)/Uart(115200,8,N,1)/VenPcAnsi();"
  gArmPlatformTokenSpaceGuid.PcdDefaultConInPaths|L"VenHw(D3987D4B-971A-435F-8CAF-4967EB627241)/Uart(115200,8,N,1)/VenPcAnsi()"

  gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
  gEfiMdePkgTokenSpaceGuid.PcdUefiLibMaxPrintBufferSize|16000
!ifdef $(NO_SHELL_PROFILES)
  gEfiShellPkgTokenSpaceGuid.PcdShellProfileMask|0x00
!endif #$(NO_SHELL_PROFILES)

  gLS1046aRdbTokenSpaceGuid.PcdOcramStackBase|0x10010000

  # Soc Specific Erratum Pcds
  gLS1046aRdbTokenSpaceGuid.PcdDdrErratumA008550|TRUE
  gLS1046aRdbTokenSpaceGuid.PcdDdrErratumA009660|FALSE
  gLS1046aRdbTokenSpaceGuid.PcdDdrErratumA009663|FALSE
  gLS1046aRdbTokenSpaceGuid.PcdDdrErratumA009442|FALSE

  gLS1046aRdbTokenSpaceGuid.PcdUsbErratumA009008|TRUE
  gLS1046aRdbTokenSpaceGuid.PcdUsbErratumA009798|TRUE
  gLS1046aRdbTokenSpaceGuid.PcdUsbErratumA008997|TRUE
  gLS1046aRdbTokenSpaceGuid.PcdUsbErratumA009007|TRUE

  # IP Specific Errata Pcds
  gLS1046aRdbTokenSpaceGuid.PcdSataErratumA009185|TRUE
  gLS1046aRdbTokenSpaceGuid.PcdSataErratumA010554|TRUE
  gLS1046aRdbTokenSpaceGuid.PcdSataErratumA010635|TRUE
  gLS1046aRdbTokenSpaceGuid.PcdSataErratumA008402|TRUE
  gLS1046aRdbTokenSpaceGuid.PcdSdhcErratumA010539|TRUE

################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################
[Components.common]
  #
  # SEC
  #
  ArmPlatformPkg/PrePi/PeiUniCore.inf

  #
  # DXE
  #
  MdeModulePkg/Core/Dxe/DxeMain.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/DxeCrc32GuidedSectionExtractLib/DxeCrc32GuidedSectionExtractLib.inf
      NULL|EmbeddedPkg/Library/LzmaHobCustomDecompressLib/LzmaHobCustomDecompressLib.inf
  }
  MdeModulePkg/Universal/PCD/Dxe/Pcd.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }

  #
  # Architectural Protocols
  #
  ArmPkg/Drivers/CpuDxe/CpuDxe.inf
  MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf
  MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf
  MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/VarCheckUefiLib/VarCheckUefiLib.inf
  }
  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteDxe.inf
  EmbeddedPkg/EmbeddedMonotonicCounter/EmbeddedMonotonicCounter.inf
  EmbeddedPkg/ResetRuntimeDxe/ResetRuntimeDxe.inf
  EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf

  # FDT installation
  EmbeddedPkg/Drivers/FdtPlatformDxe/FdtPlatformDxe.inf

  MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  MdeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf
  MdeModulePkg/Universal/SerialDxe/SerialDxe.inf

  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf

  EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf
  ArmPkg/Drivers/TimerDxe/TimerDxe.inf
  LS1046aRdbPkg/Drivers/LS1046aWatchDog/LS1046aWatchDogDxe.inf
  ArmPkg/Drivers/ArmGic/ArmGicDxe.inf

  EmbeddedPkg/SimpleTextInOutSerial/SimpleTextInOutSerial.inf

  #
  # PPA
  #
  LS1046aRdbPkg/Drivers/PpaInitDxe/PpaInitDxe.inf

  #
  # Semi-hosting filesystem
  #
  ArmPkg/Filesystem/SemihostFs/SemihostFs.inf

  #
  # FAT filesystem + GPT/MBR partitioning
  #
  MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf

  #
  # Dspi
#  LS1046aRdbPkg/Drivers/DspiDxe/DspiDxe.inf

!if $(BOOT_VIA_QSPI_FLASH) != TRUE
  #
  # Nor Flash
  LS1046aRdbPkg/Drivers/NorFlashDxe/NorFlashDxe.inf

  #
  # Nand Flash
  LS1046aRdbPkg/Drivers/NandFlashDxe/NandFlashDxe.inf

  #
  # File System
  LS1046aRdbPkg/Drivers/LS1046aFileSystemDxe/LS1046aFileSystemDxe.inf
!endif

  #
  # MMC/SD
  LS1046aRdbPkg/Drivers/MmcDxe/MmcDxe.inf

  #
  # I2C
  #
  LS1046aRdbPkg/Drivers/I2c/I2cDxe.inf

  # PCI Dxe Driver
  #
  MdeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf
  LS1046aRdbPkg/Drivers/PciHostBridgeDxe/PciHostBridgeDxe.inf

!if $(BOOT_VIA_QSPI_FLASH) == TRUE
  #
  # Qspi
  LS1046aRdbPkg/Drivers/QspiDxe/QspiDxe.inf
!endif

  #
  # RNG Driver
  #
  LS1046aRdbPkg/Drivers/RngDxe/RngDxe.inf

  #
  # Networking stack
  #
  MdeModulePkg/Universal/Network/SnpDxe/SnpDxe.inf
  MdeModulePkg/Universal/Network/DpcDxe/DpcDxe.inf
  MdeModulePkg/Universal/Network/MnpDxe/MnpDxe.inf
  MdeModulePkg/Universal/Network/VlanConfigDxe/VlanConfigDxe.inf
  MdeModulePkg/Universal/Network/ArpDxe/ArpDxe.inf
  MdeModulePkg/Universal/Network/Dhcp4Dxe/Dhcp4Dxe.inf
  MdeModulePkg/Universal/Network/Ip4Dxe/Ip4Dxe.inf
  MdeModulePkg/Universal/Network/Mtftp4Dxe/Mtftp4Dxe.inf
  MdeModulePkg/Universal/Network/Udp4Dxe/Udp4Dxe.inf
#  MdeModulePkg/Universal/Network/Tcp4Dxe/Tcp4Dxe.inf
  MdeModulePkg/Universal/Network/UefiPxeBcDxe/UefiPxeBcDxe.inf
  MdeModulePkg/Universal/Network/IScsiDxe/IScsiDxe.inf
  MdeModulePkg/Bus/Pci/UhciDxe/UhciDxe.inf
!if $(NETWORK_IP6_ENABLE) == TRUE
  NetworkPkg/Ip6Dxe/Ip6Dxe.inf
  NetworkPkg/TcpDxe/TcpDxe.inf
  NetworkPkg/Udp6Dxe/Udp6Dxe.inf
  NetworkPkg/Dhcp6Dxe/Dhcp6Dxe.inf
  NetworkPkg/Mtftp6Dxe/Mtftp6Dxe.inf
  NetworkPkg/UefiPxeBcDxe/UefiPxeBcDxe.inf
!else
  MdeModulePkg/Universal/Network/Tcp4Dxe/Tcp4Dxe.inf
!endif

  #
  # Intel E1000 driver
  LS1046aRdbPkg/Drivers/LanIntelE1000Dxe/LanIntelE1000Dxe.inf

  # PCIe emulation support for SATA and USB
  #
  LS1046aRdbPkg/PciEmulation/PciEmulation.inf
  
  #
  # SATA
  #
  LS1046aRdbPkg/Drivers/SataControllerDxe/SataControllerDxe.inf
  MdeModulePkg/Bus/Ata/AtaAtapiPassThru/AtaAtapiPassThru.inf
  MdeModulePkg/Bus/Ata/AtaBusDxe/AtaBusDxe.inf

  #
  # DPAA1 Ethernet driver
  LS1046aRdbPkg/Drivers/Dpaa1EthernetDxe/Dpaa1EthernetDxe.inf
  
  #
  # Bds
  #
  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
  MdeModulePkg/Universal/DisplayEngineDxe/DisplayEngineDxe.inf
  MdeModulePkg/Universal/SetupBrowserDxe/SetupBrowserDxe.inf
  MdeModulePkg/Universal/BdsDxe/BdsDxe.inf
  MdeModulePkg/Application/UiApp/UiApp.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/DeviceManagerUiLib/DeviceManagerUiLib.inf
      NULL|MdeModulePkg/Library/BootManagerUiLib/BootManagerUiLib.inf
      NULL|MdeModulePkg/Library/BootMaintenanceManagerUiLib/BootMaintenanceManagerUiLib.inf
  }

  MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf
  # Legacy Linux Loader
  ArmPkg/Application/LinuxLoader/LinuxLoader.inf

  #
  # Example Application
  #
  MdeModulePkg/Application/HelloWorld/HelloWorld.inf

  ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
  ShellPkg/Library/UefiShellLevel2CommandsLib/UefiShellLevel2CommandsLib.inf
  ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf

  ShellPkg/Library/UefiDpLib/UefiDpLib.inf {
    <LibraryClasses>
      TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
      PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
      DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  }

ShellPkg/Application/Shell/Shell.inf {
    <LibraryClasses>
      NULL|ShellPkg/Library/UefiShellLevel2CommandsLib/UefiShellLevel2CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel1CommandsLib/UefiShellLevel1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.inf
!ifndef $(NO_SHELL_PROFILES)
      NULL|ShellPkg/Library/UefiShellDriver1CommandsLib/UefiShellDriver1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellInstall1CommandsLib/UefiShellInstall1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellNetwork1CommandsLib/UefiShellNetwork1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellTftpCommandLib/UefiShellTftpCommandLib.inf
!ifdef $(INCLUDE_DP)
      NULL|ShellPkg/Library/UefiDpLib/UefiDpLib.inf
!endif #$(INCLUDE_DP)
!ifdef $(INCLUDE_TFTP_COMMAND)
      NULL|ShellPkg/Library/UefiShellTftpCommandLib/UefiShellTftpCommandLib.inf
!endif #$(INCLUDE_TFTP_COMMAND)
!endif #$(NO_SHELL_PROFILES)
      }

  ##
