/** @file
  Type4 Table Generator

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

#define DEFAULT_TYPE4_STRINGS                                       \
  "BGA-1156\0"                       /* socket type */              \
  "NXP\0"                            /* manufactuer */              \
  "Cortex-A57\0"                     /* processor 1 description */  \
  "Cortex-A53\0"                     /* processor 2 description */  \
  "Cortex-A72\0"                     /* processor 2 description */  \
  "0xd03\0"                          /* A53 part number */          \
  "0xd07\0"                          /* A57 part number */          \
  "0xd08\0"                          /* A72 part number */

#pragma pack()

STATIC ARM_TYPE4 mArmDefaultType4 = {
  {
    { /* SMBIOS_STRUCTURE Hdr */
      EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION,    /* UINT8 Type */
      sizeof (SMBIOS_TABLE_TYPE4),              /* UINT8 Length */
      SMBIOS_HANDLE_A72_CLUSTER
    },
  },
  DEFAULT_TYPE4_STRINGS
};


/** This macro expands to a function that retrieves Processor
    Attribute (Type 4) Information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceArm,
  EArmObjProcessorAttrInfoType4,
  CM_ARM_PROCESSOR_ATTR_TYPE4_INFO
  );

/** Updates the information in the Tyhpe 4 Table.

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
AddSystemProcessorAttrType4Info (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  * CONST CfgMgrProtocol
)
{
  EFI_STATUS                          Status = EFI_SUCCESS;
  CM_ARM_PROCESSOR_ATTR_TYPE4_INFO    * Type4Info;
  CHAR16                              * Socket;
  CHAR16                              * Manufacture;
  CHAR16                              * Version;

  ASSERT (CfgMgrProtocol != NULL);

  Status = GetEArmObjProcessorAttrInfoType4 (
      CfgMgrProtocol,
      CM_NULL_TOKEN,
      &Type4Info,
      NULL
      );
  if (EFI_ERROR (Status)) {
    DEBUG ((
          DEBUG_ERROR,
          "ERROR: SMBIOS: Failed to get Type4 table. Status = %r\n",
          Status
          ));
  }

  mArmDefaultType4.Base.Socket                    = Type4Info->Socket;
  mArmDefaultType4.Base.ProcessorType             = Type4Info->ProcessorType;
  mArmDefaultType4.Base.ProcessorFamily           = Type4Info->ProcessorFamily;
  mArmDefaultType4.Base.ProcessorManufacture      = Type4Info->ProcessorManufacture;
  mArmDefaultType4.Base.ProcessorId               = Type4Info->ProcessorId;
  mArmDefaultType4.Base.ProcessorVersion          = Type4Info->ProcessorVersion;
  mArmDefaultType4.Base.Voltage                   = Type4Info->Voltage;
  mArmDefaultType4.Base.ExternalClock             = Type4Info->ExternalClock;
  mArmDefaultType4.Base.MaxSpeed                  = Type4Info->MaxSpeed;
  mArmDefaultType4.Base.CurrentSpeed              = Type4Info->CurrentSpeed;
  mArmDefaultType4.Base.Status                    = Type4Info->Status;
  mArmDefaultType4.Base.ProcessorUpgrade          = Type4Info->ProcessorUpgrade;
  mArmDefaultType4.Base.L1CacheHandle             = Type4Info->L1CacheHandle;
  mArmDefaultType4.Base.L2CacheHandle             = Type4Info->L2CacheHandle;
  mArmDefaultType4.Base.L3CacheHandle             = Type4Info->L3CacheHandle;
  mArmDefaultType4.Base.SerialNumber              = Type4Info->SerialNumber;
  mArmDefaultType4.Base.AssetTag                  = Type4Info->AssetTag;
  mArmDefaultType4.Base.PartNumber                = Type4Info->PartNumber;
  mArmDefaultType4.Base.CoreCount                 = Type4Info->CoreCount;
  mArmDefaultType4.Base.EnabledCoreCount          = Type4Info->EnabledCoreCount;
  mArmDefaultType4.Base.ThreadCount               = Type4Info->ThreadCount;
  mArmDefaultType4.Base.ProcessorCharacteristics  = Type4Info->ProcessorCharacteristics;
  mArmDefaultType4.Base.ProcessorFamily2          = Type4Info->ProcessorFamily2;
  mArmDefaultType4.Base.CoreCount2                = Type4Info->CoreCount2;
  mArmDefaultType4.Base.EnabledCoreCount2         = Type4Info->EnabledCoreCount2;
  mArmDefaultType4.Base.ThreadCount2              = Type4Info->ThreadCount2;

  /* Update the default strings */
  Socket = AllocateZeroPool ((sizeof (CHAR16)) * SMBIOS_STRING_MAX_LENGTH);
  Manufacture = AllocateZeroPool ((sizeof (CHAR16)) * SMBIOS_STRING_MAX_LENGTH);
  Version = AllocateZeroPool ((sizeof (CHAR16)) * SMBIOS_STRING_MAX_LENGTH);

  (void)UnicodeSPrint (Socket, SMBIOS_STRING_MAX_LENGTH - 1, L"BGA-1156", Type4Info->Socket);
  (void)UnicodeSPrint (Manufacture, SMBIOS_STRING_MAX_LENGTH - 1, L"NXP", Type4Info->ProcessorManufacture);
  (void)UnicodeSPrint (Version, SMBIOS_STRING_MAX_LENGTH - 1, L"Cortex-A72", Type4Info->ProcessorVersion);

  UnicodeStrToAsciiStr(Socket, (CHAR8 *)&(mArmDefaultType4.Strings[0]));
  UnicodeStrToAsciiStr(Manufacture, (CHAR8 *)&(mArmDefaultType4.Strings[9]));
  UnicodeStrToAsciiStr(Version, (CHAR8 *)&(mArmDefaultType4.Strings[35]));

  /* Free the resources */
  FreePool(Socket);
  FreePool(Manufacture);
  FreePool(Version);

  return Status;
}

/** Construct the Default Type4 table.

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
BuildType4Table (
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

  // Update Pocessor Attributes Info
  Status = AddSystemProcessorAttrType4Info (CfgMgrProtocol);
  if (EFI_ERROR (Status)) {
    goto error_handler;
  }

  *Table = (SMBIOS_STRUCTURE*)&mArmDefaultType4;

error_handler:
  return Status;
}

/** The interface for the System Processor Attr Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR DefaultType4Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType04),
  // Generator Description
  L"SMBIOS.STD.TYPE4.GENERATOR",
  // Type
  4,
  // Build Table function
  BuildType4Table,
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
SmbiosDefaultType4LibConstructor (
  IN CONST EFI_HANDLE                ImageHandle,
  IN       EFI_SYSTEM_TABLE  * CONST SystemTable
  )
{
  EFI_STATUS  Status;
  Status = RegisterSmbiosTableGenerator (&DefaultType4Generator);
  DEBUG ((DEBUG_INFO, "DefaultType4  : Register Generator. Status = %r\n", Status));
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
SmbiosDefaultType4LibDestructor (
  IN CONST EFI_HANDLE                ImageHandle,
  IN       EFI_SYSTEM_TABLE  * CONST SystemTable
  )
{
  EFI_STATUS  Status;
  Status = DeregisterSmbiosTableGenerator (&DefaultType4Generator);
  DEBUG ((DEBUG_INFO, "DefaultType4  : Deregister Generator. Status = %r\n", Status));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
