/** @file
  Type3 Table Generator

  Copyright (c) 2020, Puresoftware Limited. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <IndustryStandard/SmBios.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <PiDxe.h>
#include <Protocol/Smbios.h>

// Module specific include files.
#include <ConfigurationManagerObject.h>
#include <ConfigurationManagerHelper.h>
#include <Library/TableHelperLib.h>
#include <Protocol/ConfigurationManagerProtocol.h>

#include <Protocol/DynamicTableFactoryProtocol.h>
#include <SmbiosTableGenerator.h>
#include <SmbiosTableHelper.h>

#define DEFAULT_TYPE3_STRINGS                           \
  "NXP\0"                            /* Manufacturer */ \
  "1U 19 rackmount\0"                /* Version */      \
  "Serial Not Set\0"                 /* Serial  */

#pragma pack()

STATIC ARM_TYPE3 mArmDefaultType3 = {
  {
    { /* SMBIOS_STRUCTURE Hdr */
      EFI_SMBIOS_TYPE_SYSTEM_ENCLOSURE, /* UINT8 Type */
      sizeof (SMBIOS_TABLE_TYPE3),      /* UINT8 Length */
      SMBIOS_HANDLE_CHASSIS
    },
  },
  DEFAULT_TYPE3_STRINGS
};

/** This macro expands to a function that retrieves System chassis
    (Type 3) Information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceArm,
  EArmObjSystemChassisInfoType3,
  CM_ARM_SYSTEM_CHASSIS_TYPE3_INFO
  );

/** Updates the information in the Type 3 Table.

  @param [in]  CfgMgrProtocol Pointer to the Configuration Manager
                              Protocol Interface.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object was not found.
  @retval EFI_BAD_BUFFER_SIZE   The size returned by the Configuration
                                Manager is less than the Object size for the
                                requested object.
**/
STATIC
EFI_STATUS
EFIAPI
AddSystemChassisType3Info (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  * CONST CfgMgrProtocol
)
{
  EFI_STATUS                          Status = EFI_SUCCESS;
  CM_ARM_SYSTEM_CHASSIS_TYPE3_INFO  * Type3Info;

  ASSERT (CfgMgrProtocol != NULL);

  Status = GetEArmObjSystemChassisInfoType3 (
      CfgMgrProtocol,
      CM_NULL_TOKEN,
      &Type3Info,
      NULL
      );
  if (EFI_ERROR (Status)) {
    DEBUG ((
          DEBUG_ERROR,
          "ERROR: SMBIOS: Failed to get Type3 table. Status = %r\n",
          Status
          ));
  }

  mArmDefaultType3.Base.Manufacturer                  = Type3Info->Manufacturer;
  mArmDefaultType3.Base.Type                          = Type3Info->Type;
  mArmDefaultType3.Base.Version                       = Type3Info->Version;
  mArmDefaultType3.Base.SerialNumber                  = Type3Info->SerialNumber;
  mArmDefaultType3.Base.AssetTag                      = Type3Info->AssetTag;
  mArmDefaultType3.Base.BootupState                   = Type3Info->BootupState;
  mArmDefaultType3.Base.PowerSupplyState              = Type3Info->PowerSupplyState;
  mArmDefaultType3.Base.ThermalState                  = Type3Info->ThermalState;
  mArmDefaultType3.Base.SecurityStatus                = Type3Info->SecurityStatus;
  mArmDefaultType3.Base.OemDefined[0]                 = Type3Info->OemDefined[0];
  mArmDefaultType3.Base.OemDefined[1]                 = Type3Info->OemDefined[1];
  mArmDefaultType3.Base.OemDefined[2]                 = Type3Info->OemDefined[2];
  mArmDefaultType3.Base.OemDefined[3]                 = Type3Info->OemDefined[3];
  mArmDefaultType3.Base.Height                        = Type3Info->Height;
  mArmDefaultType3.Base.NumberofPowerCords            = Type3Info->NumberofPowerCords;
  mArmDefaultType3.Base.ContainedElementCount         = Type3Info->ContainedElementCount;
  mArmDefaultType3.Base.ContainedElementRecordLength  = Type3Info->ContainedElementRecordLength;

  return Status;
}

/** Construct the Default Type3 table.

  This function invokes the Configuration Manager protocol interface
  to get the required hardware information for generating the SMBIOS
  table.

  If this function allocates any resources then they must be freed
  in the FreeXXXXTableResources function.

  @param [in]  This             Pointer to the table generator.
  @param [in]  SmbiosTableInfo  Pointer to the ACPI Table Info.
  @param [in]  CfgMgrProtocol   Pointer to the Configuration Manager
                                Protocol Interface.
  @param [out] Table            Pointer to the constructed SMBIOS Table.

  @retval EFI_SUCCESS           Table generated successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object was not found.
  @retval EFI_BAD_BUFFER_SIZE   The size returned by the Configuration
                                Manager is less than the Object size for the
                                requested object.
**/
STATIC
EFI_STATUS
EFIAPI
BuildType3Table (
  IN  CONST SMBIOS_TABLE_GENERATOR                  *       This,
  IN        CM_STD_OBJ_SMBIOS_TABLE_INFO            * CONST SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL    * CONST CfgMgrProtocol,
  OUT       SMBIOS_STRUCTURE                       **       Table
  )
{
  EFI_STATUS  Status;

  ASSERT (This != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == This->GeneratorID);

  *Table = NULL;

  // Update BootArch Info
  Status = AddSystemChassisType3Info (CfgMgrProtocol);
  if (EFI_ERROR (Status)) {
    goto error_handler;
  }

  *Table = (SMBIOS_STRUCTURE*)&mArmDefaultType3;

error_handler:
  return Status;
}

/** The interface for the System Boot Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR DefaultType3Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType03),
  // Generator Description
  L"SMBIOS.STD.TYPE3.GENERATOR",
  // Type
  3,
  // Build Table function
  BuildType3Table,
  // Free Resource handle
  NULL
};

/** Register the Generator with the BIOS Table Factory.

  @param [in]  ImageHandle  The handle to the image.
  @param [in]  SystemTable  Pointer to the System Table.

  @retval EFI_SUCCESS           The Generator is registered.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_ALREADY_STARTED   The Generator for the Table ID
                                is already registered.
**/
EFI_STATUS
EFIAPI
SmbiosDefaultType3LibConstructor (
  IN CONST EFI_HANDLE                ImageHandle,
  IN       EFI_SYSTEM_TABLE  * CONST SystemTable
  )
{
  EFI_STATUS  Status;
  Status = RegisterSmbiosTableGenerator (&DefaultType3Generator);
  DEBUG ((DEBUG_INFO, "DefaultType3  : Register Generator. Status = %r\n", Status));
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/** Deregister the Generator from the BIOS Table Factory.

  @param [in]  ImageHandle  The handle to the image.
  @param [in]  SystemTable  Pointer to the System Table.

  @retval EFI_SUCCESS           The Generator is deregistered.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The Generator is not registered.
**/
EFI_STATUS
EFIAPI
SmbiosDefaultType3LibDestructor (
  IN CONST EFI_HANDLE                ImageHandle,
  IN       EFI_SYSTEM_TABLE  * CONST SystemTable
  )
{
  EFI_STATUS  Status;
  Status = DeregisterSmbiosTableGenerator (&DefaultType3Generator);
  DEBUG ((DEBUG_INFO, "DefaultType3  : Deregister Generator. Status = %r\n", Status));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
