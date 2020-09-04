## @file
#  Dsc include file for Dynamic Tables Framework.
#
#  Copyright (c) 2017 - 2019, ARM Limited. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]

[BuildOptions]
  *_*_*_ASL_FLAGS          = -tc -li -so
  *_*_*_PLATFORM_FLAGS     = -I$(BIN_DIR)/Platform/NXP/$(DYNAMIC_ACPI_PLATFORM)/PlatformASLTablesLib/PlatformASLTablesLib/OUTPUT

[LibraryClasses.common]
  TableHelperLib|DynamicTablesPkg/Library/Common/TableHelperLib/TableHelperLib.inf
  AmlLib|DynamicTablesPkg/Library/Common/AmlLib/AmlLib.inf

[Components.common]
  #
  # Dynamic Table Factory Dxe
  #
  DynamicTablesPkg/Drivers/DynamicTableFactoryDxe/DynamicTableFactoryDxe.inf {
    <LibraryClasses>
      NULL|DynamicTablesPkg/Library/Acpi/Arm/AcpiDbg2LibArm/AcpiDbg2LibArm.inf
      NULL|DynamicTablesPkg/Library/Acpi/Arm/AcpiFadtLibArm/AcpiFadtLibArm.inf
      NULL|DynamicTablesPkg/Library/Acpi/Arm/AcpiGtdtLibArm/AcpiGtdtLibArm.inf
      NULL|DynamicTablesPkg/Library/Acpi/Arm/AcpiIortLibArm/AcpiIortLibArm.inf
      NULL|DynamicTablesPkg/Library/Acpi/Arm/AcpiMadtLibArm/AcpiMadtLibArm.inf
      NULL|DynamicTablesPkg/Library/Acpi/Arm/AcpiMcfgLibArm/AcpiMcfgLibArm.inf
      NULL|DynamicTablesPkg/Library/Acpi/Arm/AcpiPpttLibArm/AcpiPpttLibArm.inf
      NULL|DynamicTablesPkg/Library/Acpi/Arm/AcpiRawLibArm/AcpiRawLibArm.inf
      NULL|DynamicTablesPkg/Library/Acpi/Arm/AcpiSpcrLibArm/AcpiSpcrLibArm.inf
      NULL|DynamicTablesPkg/Library/Acpi/Arm/AcpiSratLibArm/AcpiSratLibArm.inf

      # SMBIOS
      NULL|DynamicTablesPkg/Library/Smbios/Arm/DefaultType0LibArm/DefaultType0LibArm.inf
      NULL|DynamicTablesPkg/Library/Smbios/Arm/DefaultType1LibArm/DefaultType1LibArm.inf
      NULL|DynamicTablesPkg/Library/Smbios/Arm/DefaultType3LibArm/DefaultType3LibArm.inf
      NULL|DynamicTablesPkg/Library/Smbios/Arm/DefaultType4LibArm/DefaultType4LibArm.inf
      NULL|DynamicTablesPkg/Library/Smbios/Arm/DefaultType7LibArm/DefaultType7LibArm.inf
      NULL|DynamicTablesPkg/Library/Smbios/Arm/DefaultType9LibArm/DefaultType9LibArm.inf
      NULL|DynamicTablesPkg/Library/Smbios/Arm/DefaultType16LibArm/DefaultType16LibArm.inf
      NULL|DynamicTablesPkg/Library/Smbios/Arm/DefaultType17LibArm/DefaultType17LibArm.inf
      NULL|DynamicTablesPkg/Library/Smbios/Arm/DefaultType19LibArm/DefaultType19LibArm.inf
      NULL|DynamicTablesPkg/Library/Smbios/Arm/DefaultType32LibArm/DefaultType32LibArm.inf

  }

  # SSDT generator need reference to the platforms DSDT hex file
  DynamicTablesPkg/Library/Acpi/Nxp/AcpiSsdtLib/AcpiSsdtLib.inf {
    <LibraryClasses>
      NULL|Platform/NXP/LX2160aRdbPkg/PlatformASLTablesLib/PlatformASLTablesLib.inf
      NULL|Platform/NXP/LS1046aRdbPkg/PlatformASLTablesLib/PlatformASLTablesLib.inf
      NULL|Platform/NXP/LS1046aFrwyPkg/PlatformASLTablesLib/PlatformASLTablesLib.inf
  }

  #
  # Dynamic Tables Manager Dxe
  #
  DynamicTablesPkg/Drivers/DynamicTableManagerDxe/DynamicTableManagerDxe.inf

  #
  DynamicTablesPkg/Library/Common/TableHelperLib/TableHelperLib.inf
  #

  #
  DynamicTablesPkg/Library/Common/AmlLib/AmlLib.inf
  #

  # DSDT of different platforms
  Platform/NXP/LX2160aRdbPkg/PlatformASLTablesLib/PlatformASLTablesLib.inf
  Platform/NXP/LS1046aRdbPkg/PlatformASLTablesLib/PlatformASLTablesLib.inf
  Platform/NXP/LS1046aFrwyPkg/PlatformASLTablesLib/PlatformASLTablesLib.inf
