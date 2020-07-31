/** @file
  Type1 Table Generator

  Copyright (c) 2020, Puresoftware Limited. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <IndustryStandard/SmBios.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
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

#define DEFAULT_TYPE1_STRINGS              \
  "ARM   \0"            /* Manufacturer */ \
  "LX2160    \0"        /* Product Name */ \
  "B \0"                /* Version */      \
  "                    \0"  /* 20 character buffer */

#pragma pack()

STATIC ARM_TYPE1 mArmDefaultType1 = {
  {
    { /* SMBIOS_STRUCTURE Hdr */
      EFI_SMBIOS_TYPE_SYSTEM_INFORMATION,      /* UINT8 Type */
      sizeof (SMBIOS_TABLE_TYPE1),             /* UINT8 Length */
      SMBIOS_HANDLE_PI_RESERVED
    },
  },
  DEFAULT_TYPE1_STRINGS
};

/** This macro expands to a function that retrieves System
    (Type 1) Information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceArm,
  EArmObjSystemInfoType1,
  CM_ARM_SYSTEM_TYPE1_INFO
  );

/** Updates the information in the Type 1 Table.

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
AddSystemType1Info (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  * CONST CfgMgrProtocol
)
{
  EFI_STATUS                          Status = EFI_SUCCESS;
  CM_ARM_SYSTEM_TYPE1_INFO          * Type1Info;
  CHAR16                            * Manufacturer;
  CHAR16                            * ProdName;
  CHAR16                            * Version;

  ASSERT (CfgMgrProtocol != NULL);

  Status = GetEArmObjSystemInfoType1 (
      CfgMgrProtocol,
      CM_NULL_TOKEN,
      &Type1Info,
      NULL
      );
  if (EFI_ERROR (Status)) {
    DEBUG ((
          DEBUG_ERROR,
          "ERROR: SMBIOS: Failed to get Type1 table. Status = %r\n",
          Status
          ));
  }

  mArmDefaultType1.Base.Manufacturer        = 1;
  mArmDefaultType1.Base.ProductName         = 2;
  mArmDefaultType1.Base.Version             = Type1Info->Version;
  mArmDefaultType1.Base.SerialNumber        = Type1Info->SerialNumber;
  mArmDefaultType1.Base.Uuid                = Type1Info->Uuid;
  mArmDefaultType1.Base.WakeUpType          = Type1Info->WakeUpType;
  mArmDefaultType1.Base.SKUNumber           = Type1Info->SKUNumber;
  mArmDefaultType1.Base.Family              = Type1Info->Family;
  /* Update the default strings */
  Manufacturer = AllocateZeroPool ((sizeof (CHAR16)) * SMBIOS_STRING_MAX_LENGTH);
  switch (Type1Info->Manufacturer)
  {
    case 1:
    default:
              (void)UnicodeSPrint (Manufacturer, SMBIOS_STRING_MAX_LENGTH - 1, L"NXP   ", Type1Info->Manufacturer);
  }
  UnicodeStrToAsciiStr(Manufacturer, (CHAR8 *)&(mArmDefaultType1.Strings[0]));

  ProdName  = AllocateZeroPool ((sizeof (CHAR16)) * SMBIOS_STRING_MAX_LENGTH);
  switch (Type1Info->ProductName)
  {
    case 1:
              (void)UnicodeSPrint (ProdName, SMBIOS_STRING_MAX_LENGTH - 1, L"LS1046ARDB", Type1Info->ProductName);
              break;
    case 2:
              (void)UnicodeSPrint (ProdName, SMBIOS_STRING_MAX_LENGTH - 1, L"LS1046FRWY", Type1Info->ProductName);
              break;
    case 3:
    default:
              (void)UnicodeSPrint (ProdName, SMBIOS_STRING_MAX_LENGTH - 1, L"LX2160ARDB", Type1Info->ProductName);
  }
  UnicodeStrToAsciiStr(ProdName, (CHAR8 *)&(mArmDefaultType1.Strings[7]));

  Version = AllocateZeroPool ((sizeof (CHAR16)) * SMBIOS_STRING_MAX_LENGTH);
  (void)UnicodeSPrint (Version, SMBIOS_STRING_MAX_LENGTH - 1, L"B%d", Type1Info->Version);
  UnicodeStrToAsciiStr(Version, (CHAR8 *)&(mArmDefaultType1.Strings[18]));

  /* Free the resources */
  FreePool(Manufacturer);
  FreePool(ProdName);
  FreePool(Version);

  return Status;
}

/** Construct the Default Type1 table.

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
BuildType1Table (
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
  Status = AddSystemType1Info (CfgMgrProtocol);
  if (EFI_ERROR (Status)) {
    goto error_handler;
  }

  *Table = (SMBIOS_STRUCTURE*)&mArmDefaultType1;

error_handler:
  return Status;
}

/** The interface for the System Info Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR DefaultType1Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType01),
  // Generator Description
  L"SMBIOS.STD.TYPE1.GENERATOR",
  // Type
  1,
  // Build Table function
  BuildType1Table,
  // Free Resource handle
  NULL
};

/** Register the Generator with the SMBIOS Table Factory.

  @param [in]  ImageHandle  The handle to the image.
  @param [in]  SystemTable  Pointer to the System Table.

  @retval EFI_SUCCESS           The Generator is registered.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_ALREADY_STARTED   The Generator for the Table ID
                                is already registered.
**/
EFI_STATUS
EFIAPI
SmbiosDefaultType1LibConstructor (
  IN CONST EFI_HANDLE                ImageHandle,
  IN       EFI_SYSTEM_TABLE  * CONST SystemTable
  )
{
  EFI_STATUS  Status;
  Status = RegisterSmbiosTableGenerator (&DefaultType1Generator);
  DEBUG ((DEBUG_INFO, "DefaultType1  : Register Generator. Status = %r\n", Status));
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/** Deregister the Generator from the SMBIOS Table Factory.

  @param [in]  ImageHandle  The handle to the image.
  @param [in]  SystemTable  Pointer to the System Table.

  @retval EFI_SUCCESS           The Generator is deregistered.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The Generator is not registered.
**/
EFI_STATUS
EFIAPI
SmbiosDefaultType1LibDestructor (
  IN CONST EFI_HANDLE                ImageHandle,
  IN       EFI_SYSTEM_TABLE  * CONST SystemTable
  )
{
  EFI_STATUS  Status;
  Status = DeregisterSmbiosTableGenerator (&DefaultType1Generator);
  DEBUG ((DEBUG_INFO, "DefaultType1  : Deregister Generator. Status = %r\n", Status));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
