#/** LS1043A board package.
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
  PLATFORM_NAME                  = LS1043aRdbPkg
  PLATFORM_GUID                  = 60169ec4-d2b4-44f8-825e-f8684fd42e4f
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/LS1043aRdb
  SUPPORTED_ARCHITECTURES        = AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = LS1043aRdbPkg/LS1043aRdbPkgFatXipBoot.fdf

[PcdsFixedAtBuild.common]
!if $(BOOT_VIA_QSPI_FLASH) == TRUE
  gLS1043aRdbTokenSpaceGuid.PcdFdNorBaseAddress|0x40100000
  gLS1043aRdbTokenSpaceGuid.PcdBootMode|0x3
!else
  gLS1043aRdbTokenSpaceGuid.PcdFdNorBaseAddress|0x60100000
  gLS1043aRdbTokenSpaceGuid.PcdBootMode|0x0
!endif

[LibraryClasses.common.PEIM]
  PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf

[Components.common]
  LS1043aRdbPkg/Library/LS1043aPrePiNor/LS1043aPrePiNor.inf

!include LS1043aRdbPkg.dsc

#
# FAT filesystem + GPT/MBR partitioning
#
  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  FatPkg/FatPei/FatPei.inf
  FatPkg/EnhancedFatDxe/Fat.inf

