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
 * This array of strings is platfporm specific, Idea here is that
 * this need to be created (as of not hard coed) based on platform
 * dynamically here in this generator itself.
 * Note - TODO: 1.  Once we fetch all the information from CM and prepare
 *                  whole tamplete for type16 including default strings as
 *                  "DEFAULT_TYPE16_STRINGS"
 *              2.  Add this record using SMBIOS protocol.
 *              3.  Now if generators realize that fact that the record
 *                  need update w.r.t to strings then, get the record back
 *                  and update it.
 */
#define DEFAULT_TYPE9_STRINGS           \
  "PCIe-Slot1\0"                        \
  "PCIe-Slot2\0"

/**
 * Memory Device template to filled by CM's platform info
 */
STATIC ARM_TYPE9 mArmDefaultType9 = {
  {
    { /* Header */
      SMBIOS_TYPE_SYSTEM_SLOTS,        /* UINT8 Type */
      sizeof (SMBIOS_TABLE_TYPE9),     /* UINT8 Length */
      SMBIOS_HANDLE_PI_RESERVED        /* SMBIOS_HANDLE */
    },
  },
  DEFAULT_TYPE9_STRINGS
};

#pragma pack()

/** This macro expands to a function that retrieves system slots
    (Type 9) Information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceArm,
  EArmObjSystemSlotType9,
  CM_ARM_SYSTEM_SLOT_TYPE9_INFO
  );

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
          "ERROR: SMBIOS: Failed to get Type17 table. Status = %r\n",
          Status
          ));
  }

  mArmDefaultType9.Base.SlotDesignation      = Type9Info->SlotDesignation;
  mArmDefaultType9.Base.SlotType             = Type9Info->SlotType;
  mArmDefaultType9.Base.SlotDataBusWidth     = Type9Info->SlotDataBusWidth;
  mArmDefaultType9.Base.CurrentUsage         = Type9Info->CurrentUsage;
  mArmDefaultType9.Base.SlotLength           = Type9Info->SlotLength;
  mArmDefaultType9.Base.SlotID               = Type9Info->SlotID;
  mArmDefaultType9.Base.SlotCharacteristics1 = Type9Info->SlotCharacteristics1;
  mArmDefaultType9.Base.SlotCharacteristics2 = Type9Info->SlotCharacteristics2;
  mArmDefaultType9.Base.SegmentGroupNum      = Type9Info->SegmentGroupNum;
  mArmDefaultType9.Base.BusNum               = Type9Info->BusNum;
  mArmDefaultType9.Base.DevFuncNum           = Type9Info->DevFuncNum;

  /* Update the default strings */
  SlotDesignation = AllocateZeroPool ((sizeof (CHAR16)) * SMBIOS_STRING_MAX_LENGTH);
  (void)UnicodeSPrint (SlotDesignation, SMBIOS_STRING_MAX_LENGTH - 1, L"PCIE-SLOT%d", Type9Info->SlotID);
  UnicodeStrToAsciiStr(SlotDesignation, (CHAR8 *)&(mArmDefaultType9.Strings[0]));

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
BuildType9Table (
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

  // Update Slot Info
  Status = AddSystemSlotType9Info (CfgMgrProtocol);
  if (EFI_ERROR (Status)) {
    goto error_handler;
  }

  *Table = (SMBIOS_STRUCTURE*)&mArmDefaultType9;

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
  BuildType9Table,
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
