/** @file
  Type7 Table Generator

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

#define DEFAULT_TYPE7_STRINGS                      \
  "L1 Instruction\0"                 /* L1I  */    \
  "L1 Data\0"                        /* L1D  */    \
  "L2\0"                             /* L2   */

STATIC ARM_TYPE7 mArmDefaultType7 = {
  {
    { /* SMBIOS_STRUCTURE Hdr */
      EFI_SMBIOS_TYPE_CACHE_INFORMATION,       /* UINT8 Type */
      sizeof (SMBIOS_TABLE_TYPE7),             /* UINT8 Length */
      SMBIOS_HANDLE_A57_L1I
    },
  },
  DEFAULT_TYPE7_STRINGS
};

#pragma pack()

/** This macro expands to a function that retrieves System
    CPU cache (Type 7) Information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceArm,
  EArmObjCpuCacheDeviceInfoType7,
  CM_ARM_CPU_CACHE_DEVICE_TYPE7_INFO
  );

/** Updates the information in the Tyhpe 7 Table.

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
AddCpuCacheDeviceType7Info (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  * CONST CfgMgrProtocol
)
{
  EFI_STATUS                              Status = EFI_SUCCESS;
  CM_ARM_CPU_CACHE_DEVICE_TYPE7_INFO    * Type7Info;
  CHAR16                                * SocketDesignation;

  ASSERT (CfgMgrProtocol != NULL);

  Status = GetEArmObjCpuCacheDeviceInfoType7 (
      CfgMgrProtocol,
      CM_NULL_TOKEN,
      &Type7Info,
      NULL
      );
  if (EFI_ERROR (Status)) {
    DEBUG ((
          DEBUG_ERROR,
          "ERROR: SMBIOS: Failed to get Type7 table. Status = %r\n",
          Status
          ));
  }

  mArmDefaultType7.Base.CacheConfiguration      = Type7Info->CacheConfiguration;
  mArmDefaultType7.Base.MaximumCacheSize        = Type7Info->MaximumCacheSize;
  mArmDefaultType7.Base.InstalledSize           = Type7Info->InstalledSize;
  mArmDefaultType7.Base.SupportedSRAMType       = Type7Info->SupportedSRAMType;
  mArmDefaultType7.Base.CurrentSRAMType         = Type7Info->CurrentSRAMType;
  mArmDefaultType7.Base.CacheSpeed              = Type7Info->CacheSpeed;
  mArmDefaultType7.Base.ErrorCorrectionType     = Type7Info->ErrorCorrectionType;
  mArmDefaultType7.Base.SystemCacheType         = Type7Info->SystemCacheType;
  mArmDefaultType7.Base.Associativity           = Type7Info->Associativity;
  mArmDefaultType7.Base.MaximumCacheSize2       = Type7Info->MaximumCacheSize2;
  mArmDefaultType7.Base.InstalledSize2          = Type7Info->InstalledSize2;
  mArmDefaultType7.Base.SocketDesignation       = Type7Info->SocketDesignation;
  /* Update the default strings */
  SocketDesignation = AllocateZeroPool ((sizeof (CHAR16)) * SMBIOS_STRING_MAX_LENGTH);
  (void)UnicodeSPrint (SocketDesignation, SMBIOS_STRING_MAX_LENGTH - 1, L"L%d Instruction", Type7Info->SocketDesignation);
  UnicodeStrToAsciiStr(SocketDesignation, (CHAR8 *)&(mArmDefaultType7.Strings[0]));
  /* Free the resources */
  FreePool(SocketDesignation);

  return Status;
}

/** Construct the Default Type7 table.

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
BuildType7Table (
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

  // Update cpu cache Info
  Status = AddCpuCacheDeviceType7Info (CfgMgrProtocol);
  if (EFI_ERROR (Status)) {
    goto error_handler;
  }

  *Table = (SMBIOS_STRUCTURE*)&mArmDefaultType7;

error_handler:
  return Status;
}

/** The interface for the CPU Cache devices in System Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR DefaultType7Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType07),
  // Generator Description
  L"SMBIOS.STD.TYPE7.GENERATOR",
  // Type
  7,
  // Build Table function
  BuildType7Table,
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
SmbiosDefaultType7LibConstructor (
  IN CONST EFI_HANDLE                ImageHandle,
  IN       EFI_SYSTEM_TABLE  * CONST SystemTable
  )
{
  EFI_STATUS  Status;
  Status = RegisterSmbiosTableGenerator (&DefaultType7Generator);
  DEBUG ((DEBUG_INFO, "DefaultType7  : Register Generator. Status = %r\n", Status));
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
SmbiosDefaultType7LibDestructor (
  IN CONST EFI_HANDLE                ImageHandle,
  IN       EFI_SYSTEM_TABLE  * CONST SystemTable
  )
{
  EFI_STATUS  Status;
  Status = DeregisterSmbiosTableGenerator (&DefaultType7Generator);
  DEBUG ((DEBUG_INFO, "DefaultType7  : Deregister Generator. Status = %r\n", Status));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
