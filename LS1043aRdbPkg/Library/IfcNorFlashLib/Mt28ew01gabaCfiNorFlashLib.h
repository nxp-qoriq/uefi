/** @Mt28ew01gabaCfiNorFlashLib.h

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __MT28EW01GABA_CFI_FLASH_H__
#define __MT28EW01GABA_CFI_FLASH_H__

#include <Base.h>
#include <PiDxe.h>

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/NorFlashLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeLib.h>

#define NOR_FLASH_ERASE_RETRY			10

/*
 * Values for the width of the port
 */
#define FLASH_CFI_8BIT		0x01
#define FLASH_CFI_16BIT		0x02
#define FLASH_CFI_32BIT		0x04
#define FLASH_CFI_64BIT		0x08

typedef UINT16 FLASH_DATA;

VOID FlashWrite (FLASH_DATA Val, UINTN Addr);
FLASH_DATA FlashRead (UINTN Addr);

VOID FlashWrite_Data (FLASH_DATA Val, UINTN Addr);
FLASH_DATA FlashRead_Data (UINTN Addr);

#define CREATE_BYTE_OFFSET(OffsetAddr)		((sizeof(FLASH_DATA)) * (OffsetAddr))
#define CREATE_NOR_ADDRESS(BaseAddr,OffsetAddr)	((BaseAddr) + (OffsetAddr))
#define FLASH_READ(Addr)			FlashRead((Addr))
#define FLASH_WRITE(Addr, Val)		        FlashWrite((Val), (Addr))
#define FLASH_READ_DATA(Addr)		        FlashRead_Data((Addr))
#define FLASH_WRITE_DATA(Addr, Val)		FlashWrite_Data((Val), (Addr))

#define SEND_NOR_COMMAND(BaseAddr,Offset,Cmd)	FLASH_WRITE(CREATE_NOR_ADDRESS(BaseAddr,CREATE_BYTE_OFFSET(Offset)), (Cmd))

#define GET_NOR_BLOCK_ADDRESS(BaseAddr,Lba,LbaSize)	( BaseAddr + (UINTN)((Lba) * LbaSize) )

// Device Commands for MT28EW01GABA  NOR flash

// Unlock Address
#define MT28EW01GABA_CMD_UNLOCK_1_ADDR             0x555
#define MT28EW01GABA_CMD_UNLOCK_2_ADDR             0x2AA

// Write Buffer related
#define MT28EW01GABA_BUFFER_SIZE_IN_BYTES     	  (512 * 4)
#define MT28EW01GABA_MAX_BUFFER_SIZE_IN_WORDS     (MT28EW01GABA_BUFFER_SIZE_IN_BYTES / 4)
#define BOUNDARY_OF_32_WORDS                      0x7F
#define MAX_BUFFERED_PROG_ITERATIONS              10000000

// CFI Related
// CFI Data "QRY"
#define CFI_QRY_Q                                 0x51
#define CFI_QRY_R                                 0x52
#define CFI_QRY_Y                                 0x59
#define CFI_QRY                                   0x515259

#define MT28EW01GABA_ENTER_CFI_QUERY_MODE_ADDR     0x0055
#define MT28EW01GABA_ENTER_CFI_QUERY_MODE_CMD      0x0098

#define MT28EW01GABA_CFI_QUERY_UNIQUE_QRY_STRING    0x10

#define MT28EW01GABA_CFI_VENDOR_ID_MAJOR_ADDR		0x43
#define MT28EW01GABA_CFI_VENDOR_ID_MAJOR		0x0031
#define MT28EW01GABA_CFI_VENDOR_ID_MINOR_ADDR		0x44
#define MT28EW01GABA_CFI_VENDOR_ID_MINOR		0x0033

// Offsets for CFI queries
#define MT28EW01GABA_CFI_QUERY_TYP_TIMEOUT_WORD_WRITE        0x1F
#define MT28EW01GABA_CFI_QUERY_TYP_TIMEOUT_MAX_BUFFER_WRITE  0x20
#define MT28EW01GABA_CFI_QUERY_TYP_TIMEOUT_BLOCK_ERASE       0x21
#define MT28EW01GABA_CFI_QUERY_TYP_TIMEOUT_CHIP_ERASE        0x22
#define MT28EW01GABA_CFI_QUERY_MAX_TIMEOUT_WORD_WRITE        0x23
#define MT28EW01GABA_CFI_QUERY_MAX_TIMEOUT_MAX_BUFFER_WRITE  0x24
#define MT28EW01GABA_CFI_QUERY_MAX_TIMEOUT_BLOCK_ERASE       0x25
#define MT28EW01GABA_CFI_QUERY_MAX_TIMEOUT_CHIP_ERASE        0x26

#define MT28EW01GABA_CFI_QUERY_BLOCK_SIZE          0x2F
#define MT28EW01GABA_CFI_QUERY_DEVICE_SIZE         0x27

#define MT28EW01GABA_CFI_QUERY_MAX_NUM_BYTES_WRITE         0x2A

// RESET Command
#define MT28EW01GABA_CMD_RESET_FIRST               0xAA
#define MT28EW01GABA_CMD_RESET_SECOND              0x55
#define MT28EW01GABA_CMD_RESET                     0xF0

// READ Command

// Manufacturer ID
#define MT28EW01GABA_CMD_READ_M_ID_FIRST           0xAA
#define MT28EW01GABA_CMD_READ_M_ID_SECOND          0x55
#define MT28EW01GABA_CMD_READ_M_ID_THIRD           0x90
#define MT28EW01GABA_CMD_READ_M_ID_FOURTH          0x01

// Device ID
#define MT28EW01GABA_CMD_READ_D_ID_FIRST           0xAA
#define MT28EW01GABA_CMD_READ_D_ID_SECOND          0x55
#define MT28EW01GABA_CMD_READ_D_ID_THIRD           0x90
#define MT28EW01GABA_CMD_READ_D_ID_FOURTH          0x7E
#define MT28EW01GABA_CMD_READ_D_ID_FIFTH           0x13
#define MT28EW01GABA_CMD_READ_D_ID_SIXTH           0x00

/* WRITE Commands */

// PROGRAM Commands
#define MT28EW01GABA_CMD_PROGRAM_FIRST             0xAA
#define MT28EW01GABA_CMD_PROGRAM_SECOND            0x55
#define MT28EW01GABA_CMD_PROGRAM_THIRD             0xA0

// Write Buffer Commands
#define MT28EW01GABA_CMD_WRITE_TO_BUFFER_FIRST     0xAA
#define MT28EW01GABA_CMD_WRITE_TO_BUFFER_SECOND    0x55
#define MT28EW01GABA_CMD_WRITE_TO_BUFFER_THIRD     0x25
#define MT28EW01GABA_CMD_WRITE_TO_BUFFER_CONFIRM   0x29

/* ERASE Commands */

// Chip Erase Commands
#define MT28EW01GABA_CMD_CHIP_ERASE_FIRST          0xAA
#define MT28EW01GABA_CMD_CHIP_ERASE_SECOND         0x55
#define MT28EW01GABA_CMD_CHIP_ERASE_THIRD          0x80
#define MT28EW01GABA_CMD_CHIP_ERASE_FOURTH         0xAA
#define MT28EW01GABA_CMD_CHIP_ERASE_FIFTH          0x55
#define MT28EW01GABA_CMD_CHIP_ERASE_SIXTH          0x10

// Sector Erase Commands
#define MT28EW01GABA_CMD_SECTOR_ERASE_FIRST        0xAA
#define MT28EW01GABA_CMD_SECTOR_ERASE_SECOND       0x55
#define MT28EW01GABA_CMD_SECTOR_ERASE_THIRD        0x80
#define MT28EW01GABA_CMD_SECTOR_ERASE_FOURTH       0xAA
#define MT28EW01GABA_CMD_SECTOR_ERASE_FIFTH        0x55
#define MT28EW01GABA_CMD_SECTOR_ERASE_SIXTH        0x30

// SUSPEND Commands
#define MT28EW01GABA_CMD_PROGRAM_OR_ERASE_SUSPEND  0xB0
#define MT28EW01GABA_CMD_PROGRAM_OR_ERASE_RESUME   0x30

// BLOCK LOCKING / UNLOCKING Commands
// FIXME: To be Added

// PROTECTION Commands
// FIXME: To be Added

#endif /* __MT28EW01GABA_CFI_FLASH_H__ */
