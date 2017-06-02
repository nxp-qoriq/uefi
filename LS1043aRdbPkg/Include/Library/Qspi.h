/** @Qspi.h
  Header Defining The Qspi Flash Controller Constants (Base Addresses, Sizes,
  Flags), Function Prototype, Structures etc

  Based on NOR flash access APIs used in ArmPlatformPkg/Drivers/NorFlashDxe/NorFlashDxe.h

  Copyright (c) 2011 - 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2016 - 2017 NXP.
  All rights reserved.

  This Program And The Accompanying Materials
  Are Licensed And Made Available Under The Terms And Conditions Of The BSD
  License Which Accompanies This Distribution. The Full Text Of The License
  May Be Found At
  http://Opensource.Org/Licenses/Bsd-License.Php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __QSPI_H__
#define __QSPI_H__

#include <Uefi.h>

typedef struct {
  UINTN       DeviceBaseAddress;    // Start address of the Device Base Address (DBA)
  UINTN       RegionBaseAddress;    // Start address of one single region
  UINTN       Size;
  UINTN       BlockSize;
} QSPI_FLASH_DESCRIPTION;

/* Function Prototypes */
/**
  This API detect the slave device connected to qspi controller

  @retval EFI_SUCCESS           Slave device is attached to QSPI Controller.
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory.
**/

EFI_STATUS
QspiPlatformGetDevices (
  OUT QSPI_FLASH_DESCRIPTION   **QspiDevices,
  OUT UINT32                  *Count
  );

EFI_STATUS
QspiPlatformFlashGetAttributes (
  OUT QSPI_FLASH_DESCRIPTION  *QspiDevices,
  IN UINT32                  Count
  );

/**
  This API detect the slave device connected to qspi controller and
  initializes slave device structure.

  @retval EFI_SUCCESS       	Slave device is attached to QSPI Controller.
  @retval EFI_OUT_OF_RESOURCES	Failed to allocate memory.
**/
EFI_STATUS
QspiDetect(
  VOID
  );

/**
  This API read the length bytes of data starting from specified offset
  from connected flash memory device to an input memory buffer

  @param[in]  Offset    	Offset of Flash memory to start with.
  @param[in]  Len	 	Length of data to be read.
  @param[out] Buf		Pointer to memory buffer to keep read data.

  @retval EFI_SUCCESS       	Read operation is successful.
  @retval EFI_INVALID_PARAMETER 	Input Parameter is invalid.
**/
EFI_STATUS
QspiFlashRead (
  IN  UINT64 Offset,
  IN  UINT64 Len,
  OUT VOID *Buf
  );

EFI_STATUS
QspiFlashWrite (
  IN     UINT64     Offset,
  IN     UINT64     BufferSizeInBytes,
  IN     CONST VOID *Buffer
);

/**
  This API write the length bytes of data from inputted memory
  buffer to connected flash memory device starting from specified offset.

  @param[in]  Offset    	Offset of Flash memory to start with.
  @param[in]  Len	 	Length of data to write.
  @param[in]  Buf		Pointer to memory buffer to write data from.

  @retval EFI_SUCCESS       	Write operation is successful.
  @retval EFI_INVALID_PARAMETER 	Input Parameter is invalid.
**/
EFI_STATUS
QspiFlashAlignedWrite (
  IN  UINT64 Offset,
  IN  UINT64 Len,
  IN  CONST VOID *Buf
  );

/**
  This API erase length bytes of flash memory device starting from
  input offset value.

  @param[in]  Offset    	Offset of Flash memory to start with.
  @param[in]  Len	 	Length of flash memory need to erase.

  @retval EFI_SUCCESS       	Erase operation is successful.
  @retval EFI_INVALID_PARAMETER 	Input Parameter is invalid.
**/
EFI_STATUS
QspiFlashErase (
  IN  UINT64 Offset,
  IN  UINT64 Len
  );

/**
  This API free the global Qspi flash memory structure(QspiFlash).
**/
VOID
QspiFlashFree (
  VOID
  );

/**
* Relocate the Allocated Runtime data when exiting boot services
**/
VOID
QspiFlashRelocate (
  VOID
  );

#endif
