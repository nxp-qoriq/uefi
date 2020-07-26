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
      NULL|DynamicTablesPkg/Library/Smbios/Arm/DefaultType9LibArm/DefaultType9LibArm.inf
      NULL|DynamicTablesPkg/Library/Smbios/Arm/DefaultType16LibArm/DefaultType16LibArm.inf
      NULL|DynamicTablesPkg/Library/Smbios/Arm/DefaultType17LibArm/DefaultType17LibArm.inf
      NULL|DynamicTablesPkg/Library/Smbios/Arm/DefaultType19LibArm/DefaultType19LibArm.inf
      NULL|DynamicTablesPkg/Library/Smbios/Arm/DefaultType32LibArm/DefaultType32LibArm.inf
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
