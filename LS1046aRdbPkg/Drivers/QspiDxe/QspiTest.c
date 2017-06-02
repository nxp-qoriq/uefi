/** @file  QspiTest.c

  Copyright (c) 2016 - 2017 NXP.
  All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the 
  BSD License which accompanies this distribution.  The full text of the license
  may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

 **/

#include <Common.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include "QspiDxe.h"

  EFI_STATUS
QspiTest (
    IN        QSPI_FLASH_INSTANCE   *Instance
    )
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  UINT32                                Lba = 1; //Block or Sector to Erase
  UINT8                                 *DestinationBuffer = NULL;
  UINT8                                 *SourceBuffer = NULL;
  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL   *FvbProtocol = &(Instance->FvbProtocol);
  EFI_PHYSICAL_ADDRESS                  FvbBaseAddress;
  UINT8                                 *BufferFlashAddress;
  UINT32                                Offset[] = { 1 };
  UINTN                                 BufferSize[] = { 7, 5, 1 };
  UINTN                                 Index = 0, IndexOffset = 0;
  UINTN                                 IndexBufferSize = 0;

  // Erase Block
  Status = FvbProtocol->EraseBlocks(FvbProtocol, Lba,1,EFI_LBA_LIST_TERMINATOR);
  if(EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR,"Failed to Erase QSPI Block @ Offset %p\n",
          GET_BLOCK_OFFSET(Lba)));
    return Status;
  }

  for(IndexOffset = 0; IndexOffset < ARRAY_SIZE(Offset); IndexOffset++) {
    for(IndexBufferSize = 0; 
        IndexBufferSize < ARRAY_SIZE(BufferSize);
        IndexBufferSize++) {
      DestinationBuffer = (UINT8*)AllocateZeroPool( 
          BufferSize[IndexBufferSize] * sizeof(UINT8) );
      SourceBuffer = (UINT8*)AllocateZeroPool( 
          BufferSize[IndexBufferSize] * sizeof(UINT8) );

      //Set Dummy Values to Source Buffer
      for(Index = 0; Index < BufferSize[IndexBufferSize]; Index++) {
        if (Index % 2 == 0)
          SourceBuffer[Index] = 0x61;
        else
          SourceBuffer[Index] = 0x52;
      }

      DEBUG ((DEBUG_BLKIO,"SourceBuffer to be written to QSPI start \n"));
      for (Index = 1; Index <= BufferSize[IndexBufferSize]; Index++) {
        DEBUG((DEBUG_BLKIO,"%u, ",((UINT8*)SourceBuffer)[Index-1]));
        if(0 == Index%16) DEBUG((DEBUG_BLKIO,"\n"));
      }
      DEBUG ((DEBUG_BLKIO,"\nSourceBuffer to be written to QSPI end \n"));

      Status = FvbProtocol->Write(FvbProtocol, Lba, Offset[IndexOffset],
          &(BufferSize[IndexBufferSize]), SourceBuffer);
      if(EFI_ERROR(Status)){
        DEBUG ((EFI_D_ERROR,"Failed to Write on QSPI\n"));
        goto ERROR;
      }

      FvbBaseAddress = FixedPcdGet64 (PcdFlashNvStorageVariableBase64);
      BufferFlashAddress = (UINT8*)(FvbBaseAddress + 
          ( Lba * Instance->Media.BlockSize));

      DEBUG ((DEBUG_BLKIO,"Buffer @ Flash Address %p of QSPI start \n",
            BufferFlashAddress));
      for (Index = 1; Index <= 164; Index++) {
        DEBUG((DEBUG_BLKIO,"%u, ",BufferFlashAddress[Index-1]));
        if(0 == Index%16) DEBUG((DEBUG_BLKIO,"\n"));
      }
      DEBUG ((DEBUG_BLKIO,"\nBuffer @ Flash Address of QSPI end \n"));

      Status = FvbProtocol->Read(FvbProtocol, Lba, Offset[IndexOffset],
          &(BufferSize[IndexBufferSize]), DestinationBuffer);
      if(EFI_ERROR(Status)){
        DEBUG ((EFI_D_ERROR,"Failed to Read from QSPI\n"));
        goto ERROR;
      }

      DEBUG ((DEBUG_BLKIO,"DestinationBuffer Read from QSPI start \n"));
      for (Index = 1; Index <= BufferSize[IndexBufferSize]; Index++) {
        DEBUG((DEBUG_BLKIO,"%u, ",((UINT8*)DestinationBuffer)[Index-1]));
        if(0 == Index%16) DEBUG((DEBUG_BLKIO,"\n"));
      }
      DEBUG ((DEBUG_BLKIO,"\nDestinationBuffer Read from QSPI end \n"));

      if (0 == CompareMem(DestinationBuffer,
            SourceBuffer,
            BufferSize[IndexBufferSize]))
        DEBUG ((DEBUG_BLKIO, "Qspi Test Result: PASS\n"));
      else {
        DEBUG ((EFI_D_ERROR, "Qspi Test Result: FAIL\n"));
        Status = EFI_DEVICE_ERROR;
        goto ERROR;
      }

ERROR:
      FreePool (SourceBuffer);
      FreePool (DestinationBuffer);
      if (EFI_ERROR (Status))
        break;
    }
    if (EFI_ERROR (Status))
      break;
  }
  return Status;
}
