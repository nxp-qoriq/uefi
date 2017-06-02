/** @file

 Copyright (c) 2011-2012, ARM Ltd. All rights reserved.<BR>
 Copyright (c) 2016, Freescale Semiconductor. All rights reserved.<BR>
 
This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

 **/

#ifndef _NORFLASHLIB_H_
#define _NORFLASHLIB_H_

typedef struct {
  UINTN       DeviceBaseAddress;   // Start address of the Device Base Address (DBA)
  UINTN       RegionBaseAddress;   // Start address of one single region
  UINTN       Size;
  UINTN       BlockSize;
  EFI_GUID    Guid;
  UINTN       MultiByteWordCount;  // Maximum Word count that can be written to Nor Flash in multi byte write
  UINTN      WordWriteTimeOut;    // single byte/word timeout usec
  UINTN      BufferWriteTimeOut;  // buffer write timeout usec
  UINTN      BlockEraseTimeOut;   // block erase timeout usec
  UINTN      ChipEraseTimeOut;    // chip erase timeout usec
} NOR_FLASH_DESCRIPTION;

EFI_STATUS
NorFlashPlatformGetDevices (
  OUT NOR_FLASH_DESCRIPTION   **NorFlashDescriptions,
  OUT UINT32                  *Count
  );

#endif /* _NORFLASHLIB_H_ */
