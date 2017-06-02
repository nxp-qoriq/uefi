/** Duart.c
  Duart driver

  Based on Serial I/O Port Implementation available in PL011Uart.c

  Copyright (c) 2008 - 2010, Apple Inc. All rights reserved.<BR>
  Copyright (c) 2011 - 2013, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 **/

#include <LS1046aSocLib.h>

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>

#include <DUart.h>

//
// EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE and
// EFI_SERIAL_DATA_TERMINAL_READY are the only control bits
// that is not supported.
STATIC CONST UINT32 mInvalidControlBits = (EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE | \
					   EFI_SERIAL_DATA_TERMINAL_READY);

UINT32
CalculateBaudDivisor (
  OUT UINT64 *BaudRate
  )
{
	struct SysInfo SocSysInfo;
	UINTN DUartClk;

	GetSysInfo(&SocSysInfo);
	DUartClk = (SocSysInfo.FreqSystemBus)/2;

	return ((DUartClk)/(*BaudRate * 16));
}

/*
   Initialise the serial port to the specified settings.
   All unspecified settings will be set to the default values.

   @return    Always return EFI_SUCCESS or EFI_INVALID_PARAMETER.

 **/
RETURN_STATUS
EFIAPI
DuartInitializePort (
  IN OUT UINTN UartBase,
  IN OUT UINT64 *BaudRate
  )
{
	UINT32	BaudDivisor;

	BaudDivisor = CalculateBaudDivisor(BaudRate);

	while (!(MmioRead8(UartBase + ULSR) & DUART_LSR_TEMT))
	;

	MmioWrite8(UartBase + UIER, 0x1);
	MmioWrite8(UartBase + ULCR, DUART_LCR_BKSE | DUART_LCRVAL);
	MmioWrite8(UartBase + UDLB, 0);
	MmioWrite8(UartBase + UDMB, 0);
	MmioWrite8(UartBase + ULCR, DUART_LCRVAL);
	MmioWrite8(UartBase + UMCR, DUART_MCRVAL);
	MmioWrite8(UartBase + UFCR, DUART_FCRVAL);
	MmioWrite8(UartBase + ULCR, DUART_LCR_BKSE | DUART_LCRVAL);
	MmioWrite8(UartBase + UDLB, BaudDivisor & 0xff);
	MmioWrite8(UartBase + UDMB, (BaudDivisor >> 8) & 0xff);
	MmioWrite8(UartBase + ULCR, DUART_LCRVAL);
	return RETURN_SUCCESS;
}

/**
  Write data to serial device.

  @param  Buffer           Point of data buffer which need to be written.
  @param  NumberOfBytes    Number of output bytes which are cached in Buffer.

  @retval 0                Write data failed.
  @retval !0               Actual number of bytes written to serial device.

 **/
UINTN
EFIAPI
DuartWrite (
  IN  UINTN    UartBase,
  IN UINT8     *Buffer,
  IN UINTN     NumberOfBytes
 )
{
	UINT8* CONST Final = &Buffer[NumberOfBytes];

	while (Buffer < Final) {
		while ((MmioRead8(UartBase + ULSR) & DUART_LSR_THRE) == 0)
			                ;
		        MmioWrite8(UartBase + UTHR, *Buffer++);
	}

	return NumberOfBytes;
}

/**
  Read data from serial device and save the data in buffer.

  @param  Buffer           Point of data buffer which need to be written.
  @param  NumberOfBytes    Number of output bytes which are cached in Buffer.

  @retval 0                Read data failed.
  @retval !0               Actual number of bytes read from serial device.

 **/
UINTN
EFIAPI
DuartRead (
  IN UINTN UartBase,
  OUT UINT8 *Buffer,
  IN UINTN NumberOfBytes
  )
{
	UINTN   Count;

	for (Count = 0; Count < NumberOfBytes; Count++, Buffer++) {
		/*
		 * Loop while waiting for a new char(s) to arrive in the
		 * RxFIFO
		 */
		while ((MmioRead8(UartBase + ULSR) & DUART_LSR_DR) == 0);

		*Buffer = MmioRead8(UartBase + URBR);
	}

	return NumberOfBytes;
}

/**
  Check to see if any data is available to be read from the debug device.

  @retval EFI_SUCCESS       At least one byte of data is available to be read
  @retval EFI_NOT_READY     No data is available to be read
  @retval EFI_DEVICE_ERROR  The serial device is not functioning properly

 **/
BOOLEAN
EFIAPI
DuartPoll (
  IN UINTN UartBase
  )
{
	return ((MmioRead8 (UartBase + ULSR) & DUART_LSR_DR) != 0);
}

/**

  Assert or deassert the control signals on a serial port.
  The following control signals are set according their bit settings :
  . Request to Send
  . Data Terminal Ready

  @param[in]  UartBase  UART registers base address
  @param[in]  Control   The following bits are taken into account :
                        . EFI_SERIAL_REQUEST_TO_SEND : assert/deassert the
                          "Request To Send" control signal if this bit is
                          equal to one/zero.
                        . EFI_SERIAL_DATA_TERMINAL_READY : assert/deassert
                          the "Data Terminal Ready" control signal if this
                          bit is equal to one/zero.
                        . EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE : enable/disable
                          the hardware loopback if this bit is equal to
                          one/zero.
                        . EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE : not supported.
                        . EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE : enable/
                          disable the hardware flow control based on CTS (Clear
                          To Send) and RTS (Ready To Send) control signals.

  @retval  RETURN_SUCCESS      The new control bits were set on the device.
  @retval  RETURN_UNSUPPORTED  The device does not support this operation.

**/
RETURN_STATUS
EFIAPI
DuartSetControl (
    IN UINTN   UartBase,
    IN UINT32  Control
  )
{
  UINT32  McrBits;

  if (Control & (mInvalidControlBits)) {
    return RETURN_UNSUPPORTED;
  }

  McrBits = MmioRead8 (UartBase + UMCR);

  if (Control & EFI_SERIAL_REQUEST_TO_SEND) {
    McrBits |= DUART_MCR_RTS;
  } else {
    McrBits &= ~DUART_MCR_RTS;
  }

  if (Control & EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE) {
    McrBits |= DUART_MCR_LOOP;
  } else {
    McrBits &= ~DUART_MCR_LOOP;
  }

  if (Control & EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE) {
    McrBits |= DUART_MCR_AFE;
  } else {
    McrBits &= ~DUART_MCR_AFE;
  }

  MmioWrite32 (UartBase + UMCR, McrBits);

  return RETURN_SUCCESS;
}

/**

  Retrieve the status of the control bits on a serial device.

  @param[in]   UartBase  UART registers base address
  @param[out]  Control   Status of the control bits on a serial device :

                         . EFI_SERIAL_DATA_CLEAR_TO_SEND,
                           EFI_SERIAL_DATA_SET_READY,
                           EFI_SERIAL_RING_INDICATE,
                           EFI_SERIAL_CARRIER_DETECT,
                           EFI_SERIAL_REQUEST_TO_SEND,
                           EFI_SERIAL_DATA_TERMINAL_READY
                           are all related to the DTE (Data Terminal Equipment)
                           and DCE (Data Communication Equipment) modes of
                           operation of the serial device.
                         . EFI_SERIAL_INPUT_BUFFER_EMPTY : equal to one if the
                           receive buffer is empty, 0 otherwise.
                         . EFI_SERIAL_OUTPUT_BUFFER_EMPTY : equal to one if the
                           transmit buffer is empty, 0 otherwise.
                         . EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE : equal to one if
                           the hardware loopback is enabled (the ouput feeds the
                           receive buffer), 0 otherwise.
                         . EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE : equal to one if
                           a loopback is accomplished by software, 0 otherwise.
                         . EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE : equal to
                           one if the hardware flow control based on CTS (Clear
                           To Send) and RTS (Ready To Send) control signals is
                           enabled, 0 otherwise.

  @retval RETURN_SUCCESS  The control bits were read from the serial device.

**/
RETURN_STATUS
EFIAPI
DuartGetControl (
    IN UINTN     UartBase,
    OUT UINT32  *Control
  )
{
  UINT32      MsrRegister;
  UINT32      McrRegister;
  UINT32      LsrRegister;

  MsrRegister = MmioRead8 (UartBase + UMSR);
  McrRegister = MmioRead8 (UartBase + UMCR);
  LsrRegister = MmioRead8 (UartBase + ULSR);

  *Control = 0;

  if ((MsrRegister & DUART_MSR_CTS) == DUART_MSR_CTS) {
    *Control |= EFI_SERIAL_CLEAR_TO_SEND;
  }

  if ((McrRegister & DUART_MCR_RTS) == DUART_MCR_RTS) {
    *Control |= EFI_SERIAL_REQUEST_TO_SEND;
  }

  if ((LsrRegister & DUART_LSR_TEMT) == DUART_LSR_TEMT) {
    *Control |= EFI_SERIAL_OUTPUT_BUFFER_EMPTY;
  }

  if ((McrRegister & DUART_MCR_AFE) == DUART_MCR_AFE) {
    *Control |= EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE;
  }

  if ((McrRegister & DUART_MCR_LOOP) == DUART_MCR_LOOP) {
    *Control |= EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE;
  }

  return RETURN_SUCCESS;
}

