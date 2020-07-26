
/** @file
  Type32 Table Generator

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

#define DEFAULT_TYPE32_STRINGS                      \
  "\0"                               /* nothing */

STATIC ARM_TYPE32 mArmDefaultType32 = {
  {
    { /* SMBIOS_STRUCTURE Hdr */
      EFI_SMBIOS_TYPE_SYSTEM_BOOT_INFORMATION, /* UINT8 Type */
      sizeof (SMBIOS_TABLE_TYPE32),            /* UINT8 Length */
      SMBIOS_HANDLE_PI_RESERVED
    },
  },
  DEFAULT_TYPE32_STRINGS
};

#pragma pack()

/** This macro expands to a function that retrieves System boot
    (Type 32) Information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceArm,
  EArmObjSystemBootType32,
  CM_ARM_SYSTEM_BOOT_TYPE32_INFO
  );

/** Updates the information in the Tyhpe 32 Table.

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
AddSystemBootType32Info (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  * CONST CfgMgrProtocol
)
{
  EFI_STATUS                          Status = EFI_SUCCESS;
  CM_ARM_SYSTEM_BOOT_TYPE32_INFO    * Type32Info;

  ASSERT (CfgMgrProtocol != NULL);

  Status = GetEArmObjSystemBootType32 (
      CfgMgrProtocol,
      CM_NULL_TOKEN,
      &Type32Info,
      NULL
      );
  if (EFI_ERROR (Status)) {
    DEBUG ((
          DEBUG_ERROR,
          "ERROR: SMBIOS: Failed to get Type32 table. Status = %r\n",
          Status
          ));
  }

  mArmDefaultType32.Base.BootStatus  = Type32Info->BootStatus;

  return Status;
}

/** Construct the Default Type32 table.

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
BuildType32Table (
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
  Status = AddSystemBootType32Info (CfgMgrProtocol);
  if (EFI_ERROR (Status)) {
    goto error_handler;
  }

  *Table = (SMBIOS_STRUCTURE*)&mArmDefaultType32;

error_handler:
  return Status;
}

/** The interface for the System Boot Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR DefaultType32Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType32),
  // Generator Description
  L"SMBIOS.STD.TYPE32.GENERATOR",
  // Type
  32,
  // Build Table function
  BuildType32Table,
  // Free Resource handle
  NULL
};

/** Register the Generator with the ACPI Table Factory.

  @param [in]  ImageHandle  The handle to the image.
  @param [in]  SystemTable  Pointer to the System Table.

  @retval EFI_SUCCESS           The Generator is registered.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_ALREADY_STARTED   The Generator for the Table ID
                                is already registered.
**/
EFI_STATUS
EFIAPI
SmbiosDefaultType32LibConstructor (
  IN CONST EFI_HANDLE                ImageHandle,
  IN       EFI_SYSTEM_TABLE  * CONST SystemTable
  )
{
  EFI_STATUS  Status;
  Status = RegisterSmbiosTableGenerator (&DefaultType32Generator);
  DEBUG ((DEBUG_ERROR, "DefaultType32  : Register Generator. Status = %r\n", Status));
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/** Deregister the Generator from the ACPI Table Factory.

  @param [in]  ImageHandle  The handle to the image.
  @param [in]  SystemTable  Pointer to the System Table.

  @retval EFI_SUCCESS           The Generator is deregistered.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The Generator is not registered.
**/
EFI_STATUS
EFIAPI
SmbiosDefaultType32LibDestructor (
  IN CONST EFI_HANDLE                ImageHandle,
  IN       EFI_SYSTEM_TABLE  * CONST SystemTable
  )
{
  EFI_STATUS  Status;
  Status = DeregisterSmbiosTableGenerator (&DefaultType32Generator);
  DEBUG ((DEBUG_INFO, "DefaultType32  : Deregister Generator. Status = %r\n", Status));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
