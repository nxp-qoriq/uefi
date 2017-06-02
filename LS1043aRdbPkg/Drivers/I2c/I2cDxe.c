/** I2c.c
  I2c driver APIs for read, write, initialize, set speed and reset

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <I2c.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Protocol/I2cMaster.h>

EFI_STATUS
EFIAPI
SetBusFrequency (
  IN CONST EFI_I2C_MASTER_PROTOCOL   *This,
  IN OUT UINTN                       *BusClockHertz
 )
{
  VOID * BaseAddress = (VOID *)I2C0_BASE_ADDRESS;

  return (I2cSetBusSpeed(BaseAddress, *BusClockHertz));
}

EFI_STATUS
EFIAPI
Reset (
  IN CONST EFI_I2C_MASTER_PROTOCOL *This
  )
{
  VOID * BaseAddress = (VOID *)I2C0_BASE_ADDRESS;
  UINTN BusFreq = BUS_CLK;
  INT32 Ret = 0;

  I2cReset();

  Ret = I2cSetBusSpeed(BaseAddress, BusFreq);

  return Ret;
}

EFI_STATUS
EFIAPI StartRequest (
  IN CONST EFI_I2C_MASTER_PROTOCOL *This,
  IN UINTN                         SlaveAddress,
  IN EFI_I2C_REQUEST_PACKET        *RequestPacket,
  IN EFI_EVENT                     Event      OPTIONAL,
  OUT EFI_STATUS                   *I2cStatus OPTIONAL
  )
{
  UINT32 Count;
  INT32  Ret;
  UINT32 Length;
  UINT8 *Buffer = NULL;
  UINT32 Flag;

  if (RequestPacket->OperationCount <= 0) {
    DEBUG((EFI_D_ERROR," Operation count is not valid %d\n",
           RequestPacket->OperationCount));
    return EFI_INVALID_PARAMETER;
  }

  for (Count = 0; Count < RequestPacket->OperationCount; Count++) {
    Flag = RequestPacket->Operation[Count].Flags;
    Length = RequestPacket->Operation[Count].LengthInBytes;
    Buffer = RequestPacket->Operation[Count].Buffer;

    if (Length <= 0) {
      DEBUG((EFI_D_ERROR," Invalid length of buffer %d\n", Length));
      return EFI_INVALID_PARAMETER;
    }

    if (Flag == I2C_READ_FLAG) {
      Ret = I2cDataRead ((VOID *)I2C0_BASE_ADDRESS, SlaveAddress,
              0x00, sizeof(SlaveAddress)/8, Buffer, Length);
      if (Ret != EFI_SUCCESS) {
        DEBUG((EFI_D_ERROR," I2c read operation failed (error %d)\n", Ret));
        return Ret;
      }
    } else if (Flag == I2C_WRITE_FLAG) {
      Ret = I2cDataWrite ((VOID *)I2C0_BASE_ADDRESS, SlaveAddress,
              0x00, sizeof(SlaveAddress)/8, Buffer, Length);
      if (Ret != EFI_SUCCESS) {
        DEBUG((EFI_D_ERROR," I2c write operation failed (error %d)\n", Ret));
        return Ret;
      }
    } else {
      DEBUG((EFI_D_ERROR," Invalid Flag %d\n",Flag));
      return EFI_INVALID_PARAMETER;
    }
  }

  return EFI_SUCCESS;
}

CONST EFI_I2C_CONTROLLER_CAPABILITIES I2cControllerCapabilities = {
  0,
  0,
  0,
  0
};

EFI_I2C_MASTER_PROTOCOL I2c = {
  ///
  /// Set the clock frequency for the I2C bus.
  ///
  SetBusFrequency,
  ///
  /// Reset the I2C host controller.
  ///
  Reset,
  ///
  /// Start an I2C transaction in master mode on the host controller.
  ///
  StartRequest,
  ///
  /// Pointer to an EFI_I2C_CONTROLLER_CAPABILITIES data structure containing
  /// the capabilities of the I2C host controller.
  ///
  &I2cControllerCapabilities
};

/**
  The user Entry Point for I2C module. The user code starts with this function.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
InitializeI2c(
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS		Status = 0;
  UINT8		Chip = 0;

  Status = I2cBusInit(I2C0, I2C_SPEED);

  DEBUG((EFI_D_RELEASE,"Valid Chip Addresses :\n"));
  if (Status == EFI_SUCCESS) {
    for (Chip = 0; Chip < LAST_CHIP_ADDRESS; Chip++) {
      Status = I2cProbeDevices(I2C0, Chip);
      if (Status == EFI_SUCCESS)
        DEBUG((EFI_D_RELEASE,"0x%x ", Chip));
    }
    DEBUG((EFI_D_RELEASE,"\n"));

    Status = gBS->InstallMultipleProtocolInterfaces (
		&ImageHandle,
		&gEfiI2cMasterProtocolGuid, (VOID**)&I2c,
		NULL
		);
  }

  return Status;
}

