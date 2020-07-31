/** @file
  Type17 Table Generator

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

#define DEFAULT_TYPE17_STRINGS                           \
  "SIDE-0\0"                     /* location */          \
  "BANK-0\0"                     /* bank description */

/**
 * Memory Device template to filled by CM's platform info
 */
STATIC ARM_TYPE17 mArmDefaultType17 = {
  {
    { /* Header */
      SMBIOS_TYPE_MEMORY_DEVICE,        /* UINT8 Type */
      sizeof (SMBIOS_TABLE_TYPE17),     /* UINT8 Length */
      SMBIOS_HANDLE_DIMM                /* SMBIOS_HANDLE */
    },
  },
  DEFAULT_TYPE17_STRINGS
};

#pragma pack()

/** This macro expands to a function that retrieves Memory Device
    (Type 17) Information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceArm,
  EArmObjMemoryDeviceType17,
  CM_ARM_MEMORY_DEVICE_TYPE17_INFO
  );

/** Updates the information in the Tyhpe 17 Table.

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
AddMemoryDeviceType16Info (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  * CONST CfgMgrProtocol
)
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  CM_ARM_MEMORY_DEVICE_TYPE17_INFO    * Type17Info;
  CHAR16                              * DeviceLocator;
  CHAR16                              * BankLocator;

  ASSERT (CfgMgrProtocol != NULL);

  Status = GetEArmObjMemoryDeviceType17 (
      CfgMgrProtocol,
      CM_NULL_TOKEN,
      &Type17Info,
      NULL
      );
  if (EFI_ERROR (Status)) {
    DEBUG ((
          DEBUG_ERROR,
          "ERROR: SMBIOS: Failed to get Type17 table. Status = %r\n",
          Status
          ));
  }

  mArmDefaultType17.Base.MemoryArrayHandle             = Type17Info->MemoryArrayHandle;
  mArmDefaultType17.Base.MemoryErrorInformationHandle  = Type17Info->MemoryErrorInformationHandle;
  mArmDefaultType17.Base.TotalWidth                    = Type17Info->TotalWidth;
  mArmDefaultType17.Base.DataWidth                     = Type17Info->DataWidth;
  mArmDefaultType17.Base.Size                          = Type17Info->Size;
  mArmDefaultType17.Base.FormFactor                    = Type17Info->FormFactor;
  mArmDefaultType17.Base.DeviceSet                     = Type17Info->DeviceSet;
  mArmDefaultType17.Base.DeviceLocator                 = Type17Info->DeviceLocator;
  mArmDefaultType17.Base.BankLocator                   = Type17Info->BankLocator;
  mArmDefaultType17.Base.MemoryType                    = Type17Info->MemoryType;
  mArmDefaultType17.Base.TypeDetail                    = Type17Info->TypeDetail;
  mArmDefaultType17.Base.Speed                         = Type17Info->Speed;
  mArmDefaultType17.Base.Manufacturer                  = Type17Info->Manufacturer;
  mArmDefaultType17.Base.SerialNumber                  = Type17Info->SerialNumber;
  mArmDefaultType17.Base.AssetTag                      = Type17Info->AssetTag;
  mArmDefaultType17.Base.PartNumber                    = Type17Info->PartNumber;
  mArmDefaultType17.Base.MemoryTechnology              = Type17Info->MemoryTechnology;

  /* Update the default strings */
  DeviceLocator = AllocateZeroPool ((sizeof (CHAR16)) * SMBIOS_STRING_MAX_LENGTH);
  BankLocator   = AllocateZeroPool ((sizeof (CHAR16)) * SMBIOS_STRING_MAX_LENGTH);
  (void)UnicodeSPrint (DeviceLocator, SMBIOS_STRING_MAX_LENGTH - 1, L"SIDE-%d", Type17Info->DeviceLocator);
  (void)UnicodeSPrint (BankLocator, SMBIOS_STRING_MAX_LENGTH - 1, L"BANK-%d", Type17Info->BankLocator);
  UnicodeStrToAsciiStr(DeviceLocator, (CHAR8 *)&(mArmDefaultType17.Strings[0]));
  UnicodeStrToAsciiStr(BankLocator, (CHAR8 *)&(mArmDefaultType17.Strings[7]));
  /* Free the resources */
  FreePool(DeviceLocator);
  FreePool(BankLocator);

  return Status;
}

/** Construct the Default Type17 table.

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
BuildType17Table (
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
  Status = AddMemoryDeviceType16Info (CfgMgrProtocol);
  if (EFI_ERROR (Status)) {
    goto error_handler;
  }

  *Table = (SMBIOS_STRUCTURE*)&mArmDefaultType17;

error_handler:
  return Status;
}

/** The interface for the SMBIOS Table17 Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR DefaultType17Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType17),
  // Generator Description
  L"SMBIOS.STD.TYPE17.GENERATOR",
  // Type
  17,
  // Build Table function
  BuildType17Table,
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
SmbiosDefaultType17LibConstructor (
  IN CONST EFI_HANDLE                ImageHandle,
  IN       EFI_SYSTEM_TABLE  * CONST SystemTable
  )
{
  EFI_STATUS  Status;
  Status = RegisterSmbiosTableGenerator (&DefaultType17Generator);
  DEBUG ((DEBUG_INFO, "DefaultType17  : Register Generator. Status = %r\n", Status));
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
SmbiosDefaultType17LibDestructor (
  IN CONST EFI_HANDLE                ImageHandle,
  IN       EFI_SYSTEM_TABLE  * CONST SystemTable
  )
{
  EFI_STATUS  Status;
  Status = DeregisterSmbiosTableGenerator (&DefaultType17Generator);
  DEBUG ((DEBUG_INFO, "DefaultType17  : Deregister Generator. Status = %r\n", Status));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
