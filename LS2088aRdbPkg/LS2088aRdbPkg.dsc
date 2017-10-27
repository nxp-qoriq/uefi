#/** LS2088A board package.
#
#    Copyright 2015-2016, Freescale semiconductor, Inc.
#    Copyright 2017 NXP
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
  PLATFORM_NAME                  = LS2088aRdbPkg
  PLATFORM_GUID                  = 60169ec4-d2b4-44f8-825e-f8684fd42e4f
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/LS2088aRdb
  SUPPORTED_ARCHITECTURES        = AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = LS2088aRdbPkg/LS2088aRdbPkg.fdf

[LibraryClasses.common.PEIM]
  PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf

[LibraryClasses.common]
  ArmLib|ArmPkg/Library/ArmLib/ArmBaseLib.inf
  ArmMmuLib|ArmPkg/Library/ArmMmuLib/ArmMmuBaseLib.inf

  ArmCpuLib|ArmPkg/Drivers/ArmCpuLib/ArmCortexAEMv8Lib/ArmCortexAEMv8Lib.inf
  ArmSmcLib|ArmPkg/Library/ArmSmcLib/ArmSmcLib.inf

  ArmGenericTimerCounterLib|ArmPkg/Library/ArmGenericTimerPhyCounterLib/ArmGenericTimerPhyCounterLib.inf
  TimerLib|ArmPkg/Library/ArmArchTimerLib/ArmArchTimerLib.inf

  LS2088aSocLib|LS2088aRdbPkg/Library/LS2088aSocLib/LS2088aSocLib.inf
  ArmTrustZoneLib|LS2088aRdbPkg/Library/ArmTrustZone/ArmTrustZone.inf
  QixisLib|LS2088aRdbPkg/Library/QixisLib/QixisLib.inf
  ArmPlatformLib|LS2088aRdbPkg/Library/LS2088aRdbLib/LS2088aRdbLib.inf
  ArmPlatformStackLib|ArmPlatformPkg/Library/ArmPlatformStackLib/ArmPlatformStackLib.inf

  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf

  FileExplorerLib|MdeModulePkg/Library/FileExplorerLib/FileExplorerLib.inf
  ImageDecoderLib|MdeModulePkg/Library/ImageDecoderLib/ImageDecoderLib.inf

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

  #
  # It is not possible to prevent the ARM compiler from inserting calls to intrinsic functions.
  # This library provides the instrinsic functions such a compiler may generate calls to.
  #
  NULL|ArmPkg/Library/CompilerIntrinsicsLib/CompilerIntrinsicsLib.inf
 

  # ARM PL310 L2 Cache Driver
#  L2X0CacheLib|ArmPlatformPkg/Drivers/PL310L2Cache/PL310L2CacheSec.inf
  # ARM PL301 Axi Driver
 # PL301AxiLib|ArmPlatformPkg/Drivers/PL301Axi/PL301Axi.inf
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
  I2cLib|LS2088aRdbPkg/Library/I2cLib/I2cLib.inf

  # Ddr Library
  DdrLib|LS2088aRdbPkg/Library/DdrLib/DdrLib.inf

  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf

  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf 
  BaseMemoryLib|ArmPkg/Library/BaseMemoryLibStm/BaseMemoryLibStm.inf

  EfiResetSystemLib|LS2088aRdbPkg/Library/ResetSystemLib/ResetSystemLib.inf

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
  RealTimeClockLib|LS2088aRdbPkg/Library/Ds3231RtcLib/Ds3231RtcLib.inf
  SemihostLib|ArmPkg/Library/SemihostLib/SemihostLib.inf

  # UART Driver
  DUartPortLib|LS2088aRdbPkg/Drivers/DUart/DUart.inf
  SerialPortLib|LS2088aRdbPkg/Library/DUartPortLib/DUartPortLib.inf

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

  NorFlashLib|LS2088aRdbPkg/Library/IfcNorFlashLib/IfcNorFlashLib.inf
  NandFlashLib|LS2088aRdbPkg/Library/IfcNandFlashLib/IfcNandFlashLib.inf
  FileSystemLib|LS2088aRdbPkg/Library/LS2088aFileSystemLib/LS2088aFileSystemLib.inf
  SdxcLib|LS2088aRdbPkg/Library/SdxcLib/SdxcLib.inf
  DspiFlashLib|LS2088aRdbPkg/Library/DspiFlashLib/DspiFlashLib.inf
  PciHostBridgeLib|LS2088aRdbPkg/Library/PciHostBridgeLib/PciHostBridgeLib.inf
!if $(BOOT_VIA_QSPI_FLASH) == TRUE
  QspiFlashLib|LS2088aRdbPkg/Library/QspiFlashLib/QspiFlashLib.inf
!endif

  # DPAA2 Ethernet
  Dpaa2EthernetMacLib|LS2088aRdbPkg/Library/Dpaa2EthernetMacLib/Dpaa2EthernetMacLib.inf
  Dpaa2EthernetPhyLib|LS2088aRdbPkg/Library/Dpaa2EthernetPhyLib/Dpaa2EthernetPhyLib.inf
  Dpaa2BoardSpecificLib|LS2088aRdbPkg/Library/Dpaa2BoardSpecificLib/Dpaa2BoardSpecificLib.inf
  Dpaa2ManagementComplexLib|LS2088aRdbPkg/Library/Dpaa2ManagementComplexLib/Dpaa2ManagementComplexLib.inf
  Dpaa2McInterfaceLib|LS2088aRdbPkg/Library/Dpaa2McInterfaceLib/Dpaa2McInterfaceLib.inf


  #
  # Allow dynamic PCDs
  #
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf

  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf

  UsbHcdLibrary|LS2088aRdbPkg/Library/UsbHcdLibrary/UsbHcd.inf

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
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  ReportStatusCodeLib|IntelFrameworkModulePkg/Library/DxeReportStatusCodeLibFramework/DxeReportStatusCodeLib.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  PeCoffLib|EmbeddedPkg/Library/DxeHobPeCoffLib/DxeHobPeCoffLib.inf

[LibraryClasses.ARM]
  #
  # It is not possible to prevent the ARM compiler for generic intrinsic functions.
  # This library provides the instrinsic functions generate by a given compiler.
  # [LibraryClasses.ARM] and NULL mean link this library into all ARM images.
  #
  NULL|ArmPkg/Library/CompilerIntrinsicsLib/CompilerIntrinsicsLib.inf

[BuildOptions]
  XCODE:*_*_ARM_PLATFORM_FLAGS == -arch armv7

  GCC:*_*_ARM_PLATFORM_FLAGS == -march=armv7-a
  GCC:*_*_AARCH64_CC_FLAGS = -mcmodel=small

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

[PcdsDynamicHii.common.DEFAULT]
  gEfiMdePkgTokenSpaceGuid.PcdPlatformBootTimeOut|L"Timeout"|gEfiGlobalVariableGuid|0x0|5
  
[PcdsFixedAtBuild.common]
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x2000
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxAuthVariableSize|0x2800

  gEfiMdeModulePkgTokenSpaceGuid.PcdResetOnMemoryTypeInformationChange|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdBootManagerMenuFile|{ 0x21, 0xaa, 0x2c, 0x46, 0x14, 0x76, 0x03, 0x45, 0x83, 0x6e, 0x8a, 0xb6, 0xf4, 0x66, 0x23, 0x31 }
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdShellFile|{ 0x83, 0xA5, 0x04, 0x7C, 0x3E, 0x9E, 0x1C, 0x4F, 0xAD, 0x65, 0xE0, 0x52, 0x68, 0xD0, 0xB4, 0xD1 }

  gArmPlatformTokenSpaceGuid.PcdFirmwareVendor|"LS2088a RDB board"
  gEmbeddedTokenSpaceGuid.PcdEmbeddedPrompt|"LS2088a"

  gArmPlatformTokenSpaceGuid.PcdCoreCount|1 # Only one core
  
  #
  # NV Storage PCDs.
  #
  gArmTokenSpaceGuid.PcdVFPEnabled|1
  
!if $(BOOT_VIA_QSPI_FLASH) == TRUE
  gArmPlatformTokenSpaceGuid.PcdCounterFrequency|20000000 #20Mhz
  gLS2088aRdbTokenSpaceGuid.PcdFlashDeviceBase64|0x20000000
  #
  # NV Storage PCDs.
  #
  gLS2088aRdbTokenSpaceGuid.PcdFlashReservedRegionBase64|0x20000000
!else
  gArmPlatformTokenSpaceGuid.PcdCounterFrequency|25000000 #25Mhz
  gLS2088aRdbTokenSpaceGuid.PcdFlashDeviceBase64|0x580000000
  #
  # NV Storage PCDs.
  #
  gLS2088aRdbTokenSpaceGuid.PcdFlashReservedRegionBase64|0x580000000
!endif

  ## SP805 Watchdog - Watchdog at Platform clk/2
  gArmPlatformTokenSpaceGuid.PcdSP805WatchdogBase|0x0C000000

!if $(BOOT_VIA_QSPI_FLASH) == TRUE
  gArmPlatformTokenSpaceGuid.PcdSP805WatchdogClockFrequencyInHz|200000000 #250MHz
!else
  gArmPlatformTokenSpaceGuid.PcdSP805WatchdogClockFrequencyInHz|266666666 #266MHz
!endif

  gEfiMdePkgTokenSpaceGuid.PcdMaximumUnicodeStringLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdMaximumAsciiStringLength|2000000
  gEfiMdePkgTokenSpaceGuid.PcdMaximumLinkedListLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdSpinLockTimeout|10000000
  gEfiMdePkgTokenSpaceGuid.PcdDebugClearMemoryValue|0xAF
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|1
  gEfiMdePkgTokenSpaceGuid.PcdPostCodePropertyMask|0
  gEfiMdePkgTokenSpaceGuid.PcdUefiLibMaxPrintBufferSize|320

!if $(BOOT_VIA_QSPI_FLASH) == TRUE
  gLS2088aRdbTokenSpaceGuid.PcdFdNorBaseAddress|0x20100000
  gLS2088aRdbTokenSpaceGuid.PcdBootMode|0x2
!else
  gLS2088aRdbTokenSpaceGuid.PcdFdNorBaseAddress|0x30100000
  gLS2088aRdbTokenSpaceGuid.PcdBootMode|0x0
!endif

!if $(TARGET) == RELEASE
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x27
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x81000000 #0x8000000F #Print almost everything
!else
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2F
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x81000044 #0x8000000F #Print almost everything
!endif

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
  gLS2088aRdbTokenSpaceGuid.PcdKludgeMapPciMmioAsCached|FALSE
  gLS2088aRdbTokenSpaceGuid.PcdPciBusMin|0
  gLS2088aRdbTokenSpaceGuid.PcdPciBusMax|255
  gLS2088aRdbTokenSpaceGuid.PcdPciIoBase|0x00010000
  gLS2088aRdbTokenSpaceGuid.PcdPciIoSize|0x00010000      # 64k
  gLS2088aRdbTokenSpaceGuid.PcdPci1Mmio64Base|0x2040000000
  gLS2088aRdbTokenSpaceGuid.PcdPci2Mmio64Base|0x2840000000
  gLS2088aRdbTokenSpaceGuid.PcdPci3Mmio64Base|0x3040000000
  gLS2088aRdbTokenSpaceGuid.PcdPci4Mmio64Base|0x3840000000
  gLS2088aRdbTokenSpaceGuid.PcdPciMmio64Size|0x40000000
  gLS2088aRdbTokenSpaceGuid.PcdPci1ExpressBaseAddress|0x2000000000
  gLS2088aRdbTokenSpaceGuid.PcdPci2ExpressBaseAddress|0x2800000000
  gLS2088aRdbTokenSpaceGuid.PcdPci3ExpressBaseAddress|0x3000000000
  gLS2088aRdbTokenSpaceGuid.PcdPci4ExpressBaseAddress|0x3800000000
  gLS2088aRdbTokenSpaceGuid.PcdPciDebug|FALSE

  #
  # Erratum Pcds
  #
  gLS2088aRdbTokenSpaceGuid.PcdSataErratumA008402|TRUE
  gLS2088aRdbTokenSpaceGuid.PcdSataErratumA009185|TRUE

  #
  # ARM Primecells
  #

  ## Serial Terminal
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x21c0600
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultBaudRate|115200

  gEfiMdePkgTokenSpaceGuid.PcdDefaultTerminalType|4

  #
  # ARM General Interrupt Controller
  gArmTokenSpaceGuid.PcdGicDistributorBase|0x6000000
  gArmTokenSpaceGuid.PcdGicRedistributorsBase|0x6100000

  #
  # ARM L2x0 PCDs
  gArmTokenSpaceGuid.PcdL2x0ControllerBase|0x10900000

  #
  # LS2088a board Specific PCDs
  # XX (DRAM - Region 1 2GB)
  # (NOR - IFC Region 1 512MB)
  gArmTokenSpaceGuid.PcdSystemMemoryBase|0x00A0000000 # Actual base + 512MB
  gArmTokenSpaceGuid.PcdSystemMemorySize|0x0060000000 # 2G - 512MB
  gLS2088aRdbTokenSpaceGuid.PcdSystemMemoryExBase|0x8080000000 #Extended System Memory Base
  gLS2088aRdbTokenSpaceGuid.PcdSystemMemoryExSize|0x0380000000 #14GB Extended System Memory Size

  #
  # LS2088a Soc Specific PCDs
  #
  gArmPlatformTokenSpaceGuid.PcdCounterFrequencyReal|TRUE
  gArmPlatformTokenSpaceGuid.PcdCsuInitialize|TRUE
  gArmPlatformTokenSpaceGuid.PcdTzc380Initialize|TRUE
  gArmPlatformTokenSpaceGuid.PcdTzpcBp147Initialize|TRUE
  gArmPlatformTokenSpaceGuid.PcdCci400Initialize|TRUE
  gArmPlatformTokenSpaceGuid.PcdClockInitialize|TRUE

  #
  # LS2088a SoC specific DPAA2 PCDs
  #

  # We need to alter PcdDpaa2McFwSrc and PcdDpaa2CortinaFwSrc values for
  # QSPI boot case. Note that the two possible values of these PCDs are:
  # - 0x01: NOR flash (value selected by default in the .dec file)
  # - 0x02: QSPI flash
!if $(BOOT_VIA_QSPI_FLASH) == TRUE
  gLS2088aRdbTokenSpaceGuid.PcdDpaa2McFwSrc|0x02
  gLS2088aRdbTokenSpaceGuid.PcdDpaa2CortinaFwSrc|0x02
!endif

  gLS2088aRdbTokenSpaceGuid.PcdDisableMcLogging|TRUE
  gLS2088aRdbTokenSpaceGuid.PcdDpaa2McLogLevel|0x02
  gLS2088aRdbTokenSpaceGuid.PcdDpaa2DebugFlags|0x0

  #
  # Enable all DMPACs by default
  #
  gLS2088aRdbTokenSpaceGuid.PcdDpaa2UsedDpmacsMask|0xff

  #
  # Sdxc specific PCDs
  #
  gArmPlatformTokenSpaceGuid.PcdSdxcDmaSupported|TRUE

  #
  # IP Testing specific PCDs
  #
  gLS2088aRdbTokenSpaceGuid.PcdDspiTest|FALSE
  gLS2088aRdbTokenSpaceGuid.PcdNandTest|FALSE
  gLS2088aRdbTokenSpaceGuid.PcdNorTest|FALSE
!if $(BOOT_VIA_QSPI_FLASH) == TRUE
  gLS2088aRdbTokenSpaceGuid.PcdQspiTest|FALSE
!endif

  #
  # PPA specific PCDs
  #
  gArmPlatformTokenSpaceGuid.PcdPpaNorBaseAddr|0x580400000
  gLS2088aRdbTokenSpaceGuid.PcdPpaQspiBaseAddr|0x20400000
  gArmPlatformTokenSpaceGuid.PcdPpaDdrOffsetAddr|0x8000000 # (128MB) calculated from top of DDR

  #
  # System Clock specific PCDs
  #
!if $(BOOT_VIA_QSPI_FLASH) == TRUE
  gLS2088aRdbTokenSpaceGuid.PcdSysClk|100000000
!else
  gLS2088aRdbTokenSpaceGuid.PcdSysClk|133333333
!endif
  
  gLS2088aRdbTokenSpaceGuid.PcdDdrClk|133333333

  #
  # Erratum Pcds
  #
  gArmPlatformTokenSpaceGuid.PcdI2cErratumA009203|TRUE


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
  gArmPlatformTokenSpaceGuid.PcdDefaultBootDevicePath|L"MemoryMapped(0x0,0x581100000,0x5811FFFFF)"

!if $(EARLY_PRINTK) == TRUE
  gArmPlatformTokenSpaceGuid.PcdDefaultBootArgument|L"MemoryMapped(0x0,0x581200000,0x581DFFFFF) -f \"MemoryMapped(0x0,0x581F00000,0x583FFFFFF)\" -c \"console=ttyS1,115200 root=/dev/ram0 earlyprintk=uart8250-8bit,0x21c0600,115200 ramdisk_size=0x2000000 default_hugepagesz=2m hugepagesz=2m hugepages=16\""
!else
  gArmPlatformTokenSpaceGuid.PcdDefaultBootArgument|L"MemoryMapped(0x0,0x581200000,0x581DFFFFF) -f \"MemoryMapped(0x0,0x581F00000,0x583FFFFFF)\" -c \"console=ttyS1,115200 root=/dev/ram0 earlycon=uart8250,mmio,0x21c0600 ramdisk_size=0x2000000 default_hugepagesz=2m hugepagesz=2m hugepages=256\""
!endif

!if $(BOOT_VIA_QSPI_FLASH) == TRUE
  gEmbeddedTokenSpaceGuid.PcdFdtDevicePaths|L"MemoryMapped(0x0,0x20F00000,0x20FFFFFF)/fsl-ls2088a-rdb.dtb"
  gArmPlatformTokenSpaceGuid.PcdDefaultBootArgument|L" " # Leave as empty string as we will use tftpboot in case of QSPI_BOOT
!else
  gEmbeddedTokenSpaceGuid.PcdFdtDevicePaths|L"MemoryMapped(0x0,0x580F00000,0x580FFFFFF)/fsl-ls2088a-rdb.dtb"
!endif

  # PPA
  gLS2088aRdbTokenSpaceGuid.PcdPpaFitConfiguration|"config@1"

  # Use the serial console for both ConIn & ConOut
  gArmPlatformTokenSpaceGuid.PcdDefaultConOutPaths|L"VenHw(D3987D4B-971A-435F-8CAF-4967EB627241)/Uart(115200,8,N,1)/VenPcAnsi();"
  gArmPlatformTokenSpaceGuid.PcdDefaultConInPaths|L"VenHw(D3987D4B-971A-435F-8CAF-4967EB627241)/Uart(115200,8,N,1)/VenPcAnsi()"

  gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
  gEfiMdePkgTokenSpaceGuid.PcdUefiLibMaxPrintBufferSize|16000
!ifdef $(NO_SHELL_PROFILES)
  gEfiShellPkgTokenSpaceGuid.PcdShellProfileMask|0x00
!endif #$(NO_SHELL_PROFILES)

  gLS2088aRdbTokenSpaceGuid.PcdOcramStackBase|0x18010000
  gLS2088aRdbTokenSpaceGuid.PcdArmErrata828024|1
  gLS2088aRdbTokenSpaceGuid.PcdArmErrata826974|1
  gLS2088aRdbTokenSpaceGuid.PcdArmErrata833471|1
  gLS2088aRdbTokenSpaceGuid.PcdArmErrata829520|1

  gLS2088aRdbTokenSpaceGuid.PcdUsbErratumA009007|TRUE

################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################
[Components.common]
  #
  # SEC
  #
  LS2088aRdbPkg/Library/LS2088aPrePi/PeiMPCore.inf

  LS2088aRdbPkg/Library/LS2088aPrePiNor/LS2088aPrePiNor.inf

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
!if $(BOOT_VIA_QSPI_FLASH) == TRUE
  MdeModulePkg/Universal/Variable/EmuRuntimeDxe/EmuVariableRuntimeDxe.inf
!endif
!if $(BOOT_VIA_QSPI_FLASH) == FALSE
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/VarCheckUefiLib/VarCheckUefiLib.inf
  }
!endif
  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteDxe.inf
  EmbeddedPkg/EmbeddedMonotonicCounter/EmbeddedMonotonicCounter.inf

  # FDT installation
  EmbeddedPkg/Drivers/FdtPlatformDxe/FdtPlatformDxe.inf

  MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  MdeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf
  MdeModulePkg/Universal/SerialDxe/SerialDxe.inf

  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf

  EmbeddedPkg/ResetRuntimeDxe/ResetRuntimeDxe.inf
  EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf
  EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf
  ArmPkg/Drivers/TimerDxe/TimerDxe.inf
  ArmPkg/Drivers/ArmGic/ArmGicDxe.inf

  ArmPlatformPkg/Drivers/SP805WatchdogDxe/SP805WatchdogDxe.inf
  EmbeddedPkg/SimpleTextInOutSerial/SimpleTextInOutSerial.inf

  #
  # PPA
  #
  LS2088aRdbPkg/Drivers/PpaInitDxe/PpaInitDxe.inf

  #
  # Semi-hosting filesystem
  #
  ArmPkg/Filesystem/SemihostFs/SemihostFs.inf

  #
  # FAT filesystem + GPT/MBR partitioning
  #
  MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf
  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  FatPkg/FatPei/FatPei.inf
  FatPkg/EnhancedFatDxe/Fat.inf

!if $(BOOT_VIA_QSPI_FLASH) != TRUE
  #
  # Nor Flash
  LS2088aRdbPkg/Drivers/NorFlashDxe/NorFlashDxe.inf

  # NAND Flash
  LS2088aRdbPkg/Drivers/NandFlashDxe/NandFlashDxe.inf
!endif

  #
  # File System
  #LS2088aRdbPkg/Drivers/LS2088aFileSystemDxe/LS2088aFileSystemDxe.inf

  #
  # MMC/SD
  LS2088aRdbPkg/Drivers/MmcDxe/MmcDxe.inf

  #
  # I2C
  #
  LS2088aRdbPkg/Drivers/I2c/I2cDxe.inf

  # PCI Dxe Driver
  #
  MdeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf
  LS2088aRdbPkg/Drivers/PciHostBridgeDxe/PciHostBridgeDxe.inf

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
  MdeModulePkg/Universal/Network/UefiPxeBcDxe/UefiPxeBcDxe.inf
  MdeModulePkg/Universal/Network/IScsiDxe/IScsiDxe.inf
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
  LS2088aRdbPkg/Drivers/LanIntelE1000Dxe/LanIntelE1000Dxe.inf

  # PCIe emulation support for SATA and USB
  #
  LS2088aRdbPkg/PciEmulation/PciEmulation.inf

  # SATA support
  LS2088aRdbPkg/Drivers/SataControllerDxe/SataControllerDxe.inf
  MdeModulePkg/Bus/Ata/AtaAtapiPassThru/AtaAtapiPassThru.inf
  MdeModulePkg/Bus/Ata/AtaBusDxe/AtaBusDxe.inf
  #
  # Usb Support
  #
  MdeModulePkg/Bus/Pci/UhciDxe/UhciDxe.inf
  MdeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
  MdeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf
  MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf

  #
  # Dspi
  LS2088aRdbPkg/Drivers/DspiDxe/DspiDxe.inf

!if $(BOOT_VIA_QSPI_FLASH) == FALSE
  #
  # DPAA2 Ethernet driver
  LS2088aRdbPkg/Drivers/Dpaa2EthernetDxe/Dpaa2EthernetDxe.inf
!endif

!if $(BOOT_VIA_QSPI_FLASH) == TRUE
  #
  # Qspi
  LS2088aRdbPkg/Drivers/QspiDxe/QspiDxe.inf
!endif

  #
  # Prefetch information
  LS2088aRdbPkg/Drivers/ShellCommand/PreFetch/PreFetch.inf

  #
  # Bds
  #
  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
  MdeModulePkg/Universal/DisplayEngineDxe/DisplayEngineDxe.inf
  MdeModulePkg/Universal/SetupBrowserDxe/SetupBrowserDxe.inf
  MdeModulePkg/Universal/BdsDxe/BdsDxe.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/BmpImageDecoderLib/BmpImageDecoderLib.inf
  }
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
