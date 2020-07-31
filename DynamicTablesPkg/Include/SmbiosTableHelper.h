/** @file
  SMBIOS struct information

  Copyright (c) 2020 Puresoftware Limited. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

 **/

#include <IndustryStandard/SmBios.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/Smbios.h>

/**
 * Type definition and contents of the default SMBIOS table.
 * This table covers only the minimum structures required by
 * the SMBIOS specification (section 6.2, version 3.0)
 */
#pragma pack(1)

typedef struct {
  SMBIOS_TABLE_TYPE0 Base;
  UINT8              Strings [];
} ARM_TYPE0;

typedef struct {
  SMBIOS_TABLE_TYPE1 Base;
  UINT8              Strings [];
} ARM_TYPE1;

typedef struct {
  SMBIOS_TABLE_TYPE3 Base;
  UINT8              Strings [];
} ARM_TYPE3;

typedef struct {
  SMBIOS_TABLE_TYPE4 Base;
  UINT8              Strings [];
} ARM_TYPE4;

typedef struct {
  SMBIOS_TABLE_TYPE7 Base;
  UINT8              Strings [];
} ARM_TYPE7;

typedef struct {
  SMBIOS_TABLE_TYPE9 Base;
  UINT8              Strings [];
} ARM_TYPE9;

typedef struct {
  SMBIOS_TABLE_TYPE16 Base;
  UINT8               Strings [];
} ARM_TYPE16;

typedef struct {
  SMBIOS_TABLE_TYPE17 Base;
  UINT8               Strings [];
} ARM_TYPE17;

typedef struct {
  SMBIOS_TABLE_TYPE19 Base;
  UINT8               Strings [];
} ARM_TYPE19;

typedef struct {
  SMBIOS_TABLE_TYPE32 Base;
  UINT8               Strings [];
} ARM_TYPE32;

/**
 * SMBIOS tables often reference each other using
 * fixed constants, define a list of these constants
 * for our hardcoded tables
 */
typedef enum  {
  SMBIOS_HANDLE_A57_L1I = 0x1000,
  SMBIOS_HANDLE_A57_L1D,
  SMBIOS_HANDLE_A57_L2,
  SMBIOS_HANDLE_A53_L1I,
  SMBIOS_HANDLE_A53_L1D,
  SMBIOS_HANDLE_A53_L2,
  SMBIOS_HANDLE_MOTHERBOARD,
  SMBIOS_HANDLE_CHASSIS,
  SMBIOS_HANDLE_A72_CLUSTER,
  SMBIOS_HANDLE_A57_CLUSTER,
  SMBIOS_HANDLE_A53_CLUSTER,
  SMBIOS_HANDLE_MEMORY,
  SMBIOS_HANDLE_DIMM
} SMBIOS_REFRENCE_HANDLES;

