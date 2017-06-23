/** @file
  Main Header file for the RNG DXE driver

  Copyright 2017 NXP.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __RNG_DXE_H
#define __RNG_DXE_H

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseMemoryLib/MemLibInternals.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Rng.h>

EFI_STATUS 
GetInfo( 
    IN     EFI_RNG_PROTOCOL      *This, 
    IN OUT UINTN                 *RNGAlgorithmListSize,
    OUT    EFI_RNG_ALGORITHM    *RNGAlgorithmList 
    );

EFI_STATUS 
GetRNG (
    IN  EFI_RNG_PROTOCOL      *This, 
    IN  EFI_RNG_ALGORITHM     *RNGAlgorithm, 
    IN  UINTN                  RNGValueLength, 
    OUT UINT8                 *RNGValue 
  );

#endif
