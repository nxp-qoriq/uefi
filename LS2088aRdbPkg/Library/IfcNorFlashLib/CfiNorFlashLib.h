/** @CfiNorFlashLib.h

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __CFI_NOR_FLASH_LIB_H__
#define __CFI_NOR_FLASH_LIB_H__

#include <Base.h>
#include <PiDxe.h>

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/NorFlashLib.h>
#include <Library/UefiLib.h>

EFI_STATUS
CfiNorFlashFlashGetAttributes (
  OUT NOR_FLASH_DESCRIPTION  *NorFlashDevices,
  IN UINT32                  Index
  );

EFI_STATUS
NorFlashReadCfiData (
  IN  UINTN                   DeviceBaseAddress,
  IN  UINTN                   CFI_Offset,
  IN  UINT32                  NumberOfBytes,
  OUT UINT32                  *Data
  );

#endif /* __CFI_NOR_FLASH_LIB_H__ */
