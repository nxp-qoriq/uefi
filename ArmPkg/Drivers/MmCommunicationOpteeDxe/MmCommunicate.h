/** @file

  Copyright 2020 NXP

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _OPTEE_MM_COMMUNICATE_H_
#define _OPTEE_MM_COMMUNICATE_H_

#include <Uefi.h>

#include <Protocol/MmCommunication.h>

#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/OpteeLib.h>

#include <IndustryStandard/ArmStdSmc.h>

#define MM_MAJOR_VER_MASK        0xEFFF0000
#define MM_MINOR_VER_MASK        0x0000FFFF
#define MM_MAJOR_VER_SHIFT       16

#define MM_MAJOR_VER(x) (((x) & MM_MAJOR_VER_MASK) >> MM_MAJOR_VER_SHIFT)
#define MM_MINOR_VER(x) ((x) & MM_MINOR_VER_MASK)

#define MM_CALLER_MAJOR_VER      0x1UL
#define MM_CALLER_MINOR_VER      0x0

#define OPTEE_TA_MM_UUID \
  { 0xed32d533, 0x99e6, 0x4209, { 0x9c, 0xc0, 0x2d, 0x72, 0xcd, 0xd9, 0x98, 0xa7 } }

#define OPTEE_TA_MM_FUNC_COMMUNICATE    0

#define OPTEE_MM_SESSION_SIGNATURE    SIGNATURE_32('O', 'T', 'M', 'M')

typedef struct _OPTEE_MM_SESSION      OPTEE_MM_SESSION;

struct _OPTEE_MM_SESSION {
  UINT32                            Signature;
  EFI_HANDLE                        Handle;
  EFI_MM_COMMUNICATION_PROTOCOL     Mm;
  UINT32                            Session;
};

#define OPTEE_MM_SESSION_FROM_MM_COMMUNICATION_PROTOCOL_THIS(a) \
  CR(                             \
    a,                            \
    OPTEE_MM_SESSION,             \
    Mm,                           \
    OPTEE_MM_SESSION_SIGNATURE    \
    )

extern EFI_GUID MmUuid;

#endif /* _OPTEE_MM_COMMUNICATE_H_ */
