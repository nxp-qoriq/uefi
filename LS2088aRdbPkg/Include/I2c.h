/** @I2c.h
  Header defining the constant, base address amd function for I2C controller

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __I2C_H___
#define __I2C_H__

#include <Uefi.h>
#include "LS2088aRdb.h"
#include "LS2088aSocLib.h"
#include <Library/TimerLib.h>
#include "Common.h"

#define I2C1	0
#define I2C2	1
#define I2C3	2
#define I2C4	3

///
/// Define the I2C flags
///
/// I2C read operation when set
#define I2C_READ_FLAG               0x1
#define I2C_WRITE_FLAG              0x2

#define I2C_SPEED		100000

#define EEPROM0_ADDRESS	0x36
#define EEPROM1_ADDRESS	0x37

#define LAST_CHIP_ADDRESS	0x80

#define I2C_CR_IIEN    	(1 << 6)
#define I2C_CR_MSTA    	(1 << 5)
#define I2C_CR_MTX     	(1 << 4)
#define I2C_CR_TX_NO_AK	(1 << 3)
#define I2C_CR_RSTA    	(1 << 2)

#define I2C_SR_ICF     	(1 << 7)
#define I2C_SR_IBB     	(1 << 5)
#define I2C_SR_IAL     	(1 << 4)
#define I2C_SR_IIF     	(1 << 1)
#define I2C_SR_RX_NO_AK       (1 << 0)

#define I2C_CR_IEN     	(0 << 7)
#define I2C_CR_IDIS    	(1 << 7)
#define I2C_SR_IIF_CLEAR      (1 << 1)


#define BUS_IDLE (0 | (I2C_SR_IBB << 8))
#define BUS_BUSY (I2C_SR_IBB | (I2C_SR_IBB << 8))
#define IIF (I2C_SR_IIF | (I2C_SR_IIF << 8))

/**
  Record defining i2c registers
**/

struct I2cRegs {
  UINT8     I2cAdr;
  UINT8     I2cFdr;
  UINT8     I2cCr;
  UINT8     I2cSr;
  UINT8     I2cDr;
};

/**
  Function to initialize i2c bus

  @param   I2c			parameter defining I2c controller no
  @param   Speed			I2C speed to be set

  @retval  EFI_INVALID_PARAMETER	Input parametr I2c was invalid
  @retval  EFI_SUCCESS		I2c was initialized successfully

**/
EFI_STATUS
EFIAPI
I2cBusInit (
  IN   INT16		I2c,
  IN   UINT32        Speed
  );

/**
  Function to read data usin i2c

  @param   Base			A pointer to the base address of I2c Controller
  @param   Chip			Address of slave device from where data to be read
  @param   Offset			Offset of slave memory
  @param   Alen			Address length of slave
  @param   Buffer			A pointer to the destination buffer for the data
  @param   Len			Length of data to be read

  @retval  EFI_NOT_READY		Arbitration lost
  @retval  EFI_TIMEOUT		Failed to initialize data transfer in predefined time
  @retval  EFI_NOT_FOUND		ACK was not recieved
  @retval  EFI_SUCCESS		Read was successful

**/
EFI_STATUS
I2cDataRead (
  IN   VOID 		*Base,
  IN   UINT8 		Chip,
  IN   UINT32 	Offset,
  IN   UINT32 	Alen,
  OUT  UINT8 		*Buffer,
  IN   UINT32 	Len
  );

/**
  Function to set I2c bus speed

  @param   BaseAddress		Base address of I2c controller
  @param   Speed			value to be set

**/
EFI_STATUS
EFIAPI
I2cSetBusSpeed (
  IN   VOID		*BaseAddress,
  IN   UINT32		Speed
  );

/**
  Function to stop transaction on i2c bus

  @param   I2cRegs		Pointer to i2c registers

  @retval  EFI_NOT_READY    Arbitration was lost
  @retval  EFI_TIMEOUT      Timeout occured
  @retval  EFI_SUCCESS	Stop operation was successful

**/
EFI_STATUS
I2cStop (
  IN   struct I2cRegs       *I2cRegs
  );

/**
  Function to initiate data transfer on i2c bus

  @param   I2cRegs   Pointer to i2c base registers
  @param   Chip      Chip Address
  @param   Offset    Slave memory's offset
  @param   Alen      length of chip address

  @retval  EFI_NOT_READY           Arbitration lost
  @retval  EFI_TIMEOUT             Failed to initialize data transfer in predefined time
  @retval  EFI_NOT_FOUND           ACK was not recieved
  @retval  EFI_SUCCESS             Read was successful

**/
EFI_STATUS
I2cBusInitTransfer (
  IN   struct I2cRegs       *I2cRegs,
  IN   UINT8                Chip,
  IN   UINT32               Offset,
  IN   INT32                Alen
  );


/**
  Function to write data using i2c bus

  @param   Base                    Pointer to the base address of I2c Controller
  @param   Chip                    Address of slave device where data to be written
  @param   Offset                  Offset of slave memory
  @param   Alen                    Address length of slave
  @param   Buffer                  A pointer to the source buffer for the data
  @param   Len                     Length of data to be write

  @retval  EFI_NOT_READY           Arbitration lost
  @retval  EFI_TIMEOUT             Failed to initialize data transfer in predefined time
  @retval  EFI_NOT_FOUND           ACK was not recieved
  @retval  EFI_SUCCESS             Read was successful

**/
EFI_STATUS
I2cDataWrite (
  IN   VOID          *Base,
  IN   UINT8         Chip,
  IN   UINT32        Offset,
  IN   INT32         Alen,
  OUT  UINT8         *Buffer,
  IN   INT32         Len
  );


/**
  Function to reset I2c

**/

VOID
I2cReset (
  VOID
  );

EFI_STATUS
EFIAPI
I2cProbeDevices (
  IN   INT16		I2c,
  IN   UINT8		Chip
  );

#endif
