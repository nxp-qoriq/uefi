/** @file
  Type9 Table Generator

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

/**
 * Memory Device template to filled by CM's platform info
 */
SMBIOS_TABLE_TYPE9 ** mArmDefaultType9;
UINTN                 mArmTableCount;

/** This macro expands to a function that retrieves system slots
    (Type 9) Information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceArm,
  EArmObjSystemSlotType9,
  CM_ARM_SYSTEM_SLOT_TYPE9_INFO
  );

/** Free any resources allocated for constructing Type9 table

  @param [in]      This             Pointer to the table generator.
  @param [in]      SmbiosTableInfo  Pointer to the SMBIOS Table Info.
  @param [in]      CfgMgrProtocol   Pointer to the Configuration Manager
  Protocol Interface.
  @param [in, out] Table            Pointer to the SMBIOS Table.

  @retval EFI_SUCCESS           The resources were freed successfully.
  @retval EFI_INVALID_PARAMETER The table pointer is NULL or invalid.
 **/
STATIC
EFI_STATUS
FreeType9TableResources (
    IN      CONST SMBIOS_TABLE_GENERATOR                  * CONST This,
    IN      CONST CM_STD_OBJ_SMBIOS_TABLE_INFO            * CONST SmbiosTableInfo,
    IN      CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL    * CONST CfgMgrProtocol,
    IN OUT        SMBIOS_STRUCTURE                       ** CONST Table
    )
{
  ASSERT (This != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == This->GeneratorID);

  if ((Table == NULL) || (*Table == NULL)) {
    DEBUG ((DEBUG_ERROR, "ERROR: Invalid Table Pointer\n"));
    ASSERT ((Table != NULL) && (*Table != NULL));
    return EFI_INVALID_PARAMETER;
  }

  FreePool (*Table);
  *Table = NULL;
  return EFI_SUCCESS;
}

/** Updates the information in the Type 9 Table.

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
AddSystemSlotType9Info (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  * CONST CfgMgrProtocol
  )
{
  EFI_STATUS                          Status = EFI_SUCCESS;
  UINT32                              TotalObjCount;
  CHAR16                            * SlotDesignation;
  CM_ARM_SYSTEM_SLOT_TYPE9_INFO     * Type9Info;
  UINT32                              Cntr;
  UINT32                              Indx;
  UINT32                              SlotDesignationStrLen;

  ASSERT (CfgMgrProtocol != NULL);

  Status = GetEArmObjSystemSlotType9 (
      CfgMgrProtocol,
      CM_NULL_TOKEN,
      &Type9Info,
      &TotalObjCount
      );
  if (EFI_ERROR (Status)) {
    DEBUG ((
          DEBUG_ERROR,
          "ERROR: SMBIOS: Failed to get Type9 table. Status = %r\n",
          Status
          ));
  }

  mArmTableCount  = TotalObjCount;
  mArmDefaultType9 = (SMBIOS_TABLE_TYPE9 **) AllocateZeroPool (sizeof(SMBIOS_TABLE_TYPE9*) * mArmTableCount);
  if (mArmDefaultType9 == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG ((
          DEBUG_ERROR,
          "ERROR: SMBIOS: Failed to get Type9 table resources . Status = %r\n",
          Status
          ));
    return Status;
  }
  for (Cntr = 1; Cntr <= TotalObjCount; Cntr++) {
    Type9Info = NULL;
    Indx = Cntr - 1;
    Status = GetEArmObjSystemSlotType9 (
        CfgMgrProtocol,
        Cntr,
        &Type9Info,
        NULL
        );
    if (EFI_ERROR (Status)) {
      DEBUG ((
            DEBUG_ERROR,
            "ERROR: SMBIOS: Failed to get Type9 table at index %d. Status = %r\n",
            Indx,
            Status
            ));
    }

    SlotDesignation = AllocateZeroPool ((sizeof (CHAR16)) * SMBIOS_STRING_MAX_LENGTH);
    switch (Type9Info->SlotDesignation)
    {
      case 1:
        SlotDesignationStrLen =
          UnicodeSPrint (SlotDesignation, SMBIOS_STRING_MAX_LENGTH - 1, L"Mini-Pcie", Type9Info->SlotDesignation);
        break;
      case 2:
        SlotDesignationStrLen =
          UnicodeSPrint (SlotDesignation, SMBIOS_STRING_MAX_LENGTH - 1, L"PCIe-Slot%d", (Type9Info->SlotDesignation-1));
        break;
      case 3:
        SlotDesignationStrLen =
          UnicodeSPrint (SlotDesignation, SMBIOS_STRING_MAX_LENGTH - 1, L"PCIe-Slot%d", (Type9Info->SlotDesignation-1));
        break;
      case 4:
      default:
        SlotDesignationStrLen =
          UnicodeSPrint (SlotDesignation, SMBIOS_STRING_MAX_LENGTH - 1, L"PCIe M2_TYPE_E (WiFi)", Type9Info->SlotDesignation);
        break;
    }

    *(mArmDefaultType9+Indx) = AllocateZeroPool(sizeof (SMBIOS_TABLE_TYPE9) + SlotDesignationStrLen + 1 + 1);

    (*(mArmDefaultType9+Indx))->SlotDesignation      = 1;
    (*(mArmDefaultType9+Indx))->SlotType             = Type9Info->SlotType;
    (*(mArmDefaultType9+Indx))->SlotDataBusWidth     = Type9Info->SlotDataBusWidth;
    (*(mArmDefaultType9+Indx))->CurrentUsage         = Type9Info->CurrentUsage;
    (*(mArmDefaultType9+Indx))->SlotLength           = Type9Info->SlotLength;
    (*(mArmDefaultType9+Indx))->SlotID               = Type9Info->SlotID;
    (*(mArmDefaultType9+Indx))->SlotCharacteristics1 = Type9Info->SlotCharacteristics1;
    (*(mArmDefaultType9+Indx))->SlotCharacteristics2 = Type9Info->SlotCharacteristics2;
    (*(mArmDefaultType9+Indx))->SegmentGroupNum      = Type9Info->SegmentGroupNum;
    (*(mArmDefaultType9+Indx))->BusNum               = Type9Info->BusNum;
    (*(mArmDefaultType9+Indx))->DevFuncNum           = Type9Info->DevFuncNum;

    (*(mArmDefaultType9+Indx))->Hdr.Type    = SMBIOS_TYPE_SYSTEM_SLOTS;
    (*(mArmDefaultType9+Indx))->Hdr.Length  = sizeof (SMBIOS_TABLE_TYPE9);
    (*(mArmDefaultType9+Indx))->Hdr.Handle  = SMBIOS_HANDLE_PI_RESERVED;

    UnicodeStrToAsciiStr(SlotDesignation, (CHAR8 *)((*(mArmDefaultType9+Indx)) + 1));
    FreePool(SlotDesignation);
  }

  return Status;
}

/** Construct the Default Type9 table.

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
BuildType9TableEx (
  IN  CONST SMBIOS_TABLE_GENERATOR                  *       This,
  IN        CM_STD_OBJ_SMBIOS_TABLE_INFO            * CONST SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL    * CONST CfgMgrProtocol,
  OUT       SMBIOS_STRUCTURE                      ***       Table,
  OUT       UINTN                                   * CONST TableCount
  )
{
  EFI_STATUS  Status;

  ASSERT (This != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == This->GeneratorID);

  *Table = NULL;

  // Update Slot Info
  Status = AddSystemSlotType9Info (CfgMgrProtocol);
  if (EFI_ERROR (Status)) {
    goto error_handler;
  }

  *Table = (SMBIOS_STRUCTURE**)mArmDefaultType9;
  *TableCount = mArmTableCount;

error_handler:
  return Status;
}

/** The interface for the SMBIOS Table9 Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR DefaultType9Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType09),
  // Generator Description
  L"SMBIOS.STD.TYPE9.GENERATOR",
  // Type
  9,
  // Build Table function
  NULL,
  // Extended Build Table function
  BuildType9TableEx,
  // Free Resource handle
  FreeType9TableResources
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
SmbiosDefaultType9LibConstructor (
  IN CONST EFI_HANDLE                ImageHandle,
  IN       EFI_SYSTEM_TABLE  * CONST SystemTable
  )
{
  EFI_STATUS  Status;
  Status = RegisterSmbiosTableGenerator (&DefaultType9Generator);
  DEBUG ((DEBUG_INFO, "DefaultType9  : Register Generator. Status = %r\n", Status));
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
SmbiosDefaultType9LibDestructor (
  IN CONST EFI_HANDLE                ImageHandle,
  IN       EFI_SYSTEM_TABLE  * CONST SystemTable
  )
{
  EFI_STATUS  Status;
  Status = DeregisterSmbiosTableGenerator (&DefaultType9Generator);
  DEBUG ((DEBUG_INFO, "DefaultType9  : Deregister Generator. Status = %r\n", Status));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
