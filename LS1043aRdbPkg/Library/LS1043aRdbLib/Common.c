/** Common.c
*
*  Copyright (c) 2015, Freescale Ltd. All rights reserved.
*  Author: Bhupesh Sharma <bhupesh.sharma@freescale.com>
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <LS1043aRdb.h>
#include <Library/DebugLib.h>

UINT32
__Div64_32 (
  IN   UINT64        *N,
  IN   UINT32        Base
  )
{
  UINT64 Rem = *N;
  UINT64 b = Base;
  UINT64 Res, d = 1;
  UINT32 High = Rem >> 32;

  /** Reduce the thing a bit first */
  Res = 0;
  if (High >= Base) {
    High /= Base;
    Res = (UINT64) High << 32;
    Rem -= (UINT64) (High*Base) << 32;
  }

  while ((UINTN)b > 0 && b < Rem) {
    b = b+b;
    d = d+d;
  }

  do {
    if (Rem >= b) {
      Rem -= b;
      Res += d;
    }
    b >>= 1;
    d >>= 1;
  } while (d);

  *N = Res;
  return Rem;
}

/*
 * PrINT32 Sizes As "Xxx KiB", "Xxx.Y KiB", "Xxx MiB", "Xxx.Y MiB",
 * Xxx GiB, Xxx.Y GiB, Etc As Needed; Allow for Optional Trailing String
 * (Like "\n")
 */
VOID
PrintSize (
  IN  UINT64 Size,
  IN  CONST INT8 *S
  )
{
  UINT64 M = 0, N;
  UINT64 F;
  static CONST INT8 Names[] = {'E', 'P', 'T', 'G', 'M', 'K'};
  UINT64 D = 10 * ARRAY_SIZE(Names);
  CHAR8 C = 0;
  UINT32 I;

  for (I = 0; I < ARRAY_SIZE(Names); I++, D -= 10) {
    if (Size >> D) {
      C = Names[I];
      break;
    }
  }

  if (!C) {
    DEBUG((EFI_D_ERROR, "%Ld Bytes,\n %a", Size, S));
    return;
  }

  N = Size >> D;
  F = Size & ((1ULL << D) - 1);

  /* if There'S A Remainder, Deal With It */
  if (F) {
    M = (10ULL * F + (1ULL << (D - 1))) >> D;

    if (M >= 10) {
           M -= 10;
           N += 1;
    }
  }

  DEBUG((EFI_D_ERROR, "%Ld", N));
  if (M) {
    DEBUG((EFI_D_ERROR, ".%Ld", M));
  }
  DEBUG((EFI_D_ERROR, " %ciB, %a ", C, S));
}
