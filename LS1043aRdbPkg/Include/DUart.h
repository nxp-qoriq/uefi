/** DUart.h
*  Header defining the DUART constants (Base addresses, sizes, flags)
*
*  Based on Serial I/O Port library headers available in PL011Uart.h
*
*  Copyright (c) 2011-2012, ARM Limited. All rights reserved.
*  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.
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

#ifndef __DUART_H__
#define __DUART_H__

#include <Uefi.h>
#include <Protocol/SerialIo.h>

/* Register Definitions */


// FIFO Control Register
#define DUART_FCR_FIFO_EN	0x01 /* Fifo enable */
#define DUART_FCR_CLEAR_RCVR	0x02 /* Clear the RCVR FIFO */
#define DUART_FCR_CLEAR_XMIT	0x04 /* Clear the XMIT FIFO */
#define DUART_FCR_DMA_SELECT	0x08 /* For DMA applications */
#define DUART_FCR_TRIGGER_MASK	0xC0 /* Mask for the FIFO trigger range */
#define DUART_FCR_TRIGGER_1	0x00 /* Mask for trigger set at 1 */
#define DUART_FCR_TRIGGER_4	0x40 /* Mask for trigger set at 4 */
#define DUART_FCR_TRIGGER_8	0x80 /* Mask for trigger set at 8 */
#define DUART_FCR_TRIGGER_14	0xC0 /* Mask for trigger set at 14 */

#define DUART_FCR_RXSR		0x02 /* Receiver soft reset */
#define DUART_FCR_TXSR		0x04 /* Transmitter soft reset */

// Modem Control Register
#define DUART_MCR_DTR		0x01 /* DTR   */
#define DUART_MCR_RTS		0x02 /* RTS   */
#define DUART_MCR_OUT1		0x04 /* Out 1 */
#define DUART_MCR_OUT2		0x08 /* Out 2 */
#define DUART_MCR_LOOP		0x10 /* Enable loopback test mode */
#define DUART_MCR_AFE		0x20 /* AFE (Auto Flow Control) */

#define DUART_MCR_DMA_EN	0x04
#define DUART_MCR_TX_DFR	0x08


// Line Control Register
/*
* Note: if the word length is 5 bits (DUART_LCR_WLEN5), then setting
* DUART_LCR_STOP will select 1.5 stop bits, not 2 stop bits.
*/
#define DUART_LCR_WLS_MSK	0x03 /* character length select mask */
#define DUART_LCR_WLS_5		0x00 /* 5 bit character length */
#define DUART_LCR_WLS_6		0x01 /* 6 bit character length */
#define DUART_LCR_WLS_7		0x02 /* 7 bit character length */
#define DUART_LCR_WLS_8		0x03 /* 8 bit character length */
#define DUART_LCR_STB		0x04 /* # stop Bits, off=1, on=1.5 or 2) */
#define DUART_LCR_PEN		0x08 /* Parity eneble */
#define DUART_LCR_EPS		0x10 /* Even Parity Select */
#define DUART_LCR_STKP		0x20 /* Stick Parity */
#define DUART_LCR_SBRK		0x40 /* Set Break */
#define DUART_LCR_BKSE		0x80 /* Bank select enable */
#define DUART_LCR_DLAB		0x80 /* Divisor latch access bit */

// Line Status Register
#define DUART_LSR_DR	0x01		/* Data ready */
#define DUART_LSR_OE	0x02		/* Overrun */
#define DUART_LSR_PE	0x04		/* Parity error */
#define DUART_LSR_FE	0x08		/* Framing error */
#define DUART_LSR_BI	0x10		/* Break */
#define DUART_LSR_THRE	0x20		/* Xmit holding register empty */
#define DUART_LSR_TEMT	0x40		/* Xmitter empty */
#define DUART_LSR_ERR	0x80		/* Error */

//#define DUART_MSR_DCD	0x80		/* Data Carrier Detect */
//#define DUART_MSR_RI	0x40		/* Ring Indicator */
//#define DUART_MSR_DSR	0x20		/* Data Set Ready */
//#define DUART_MSR_CTS	0x10		/* Clear to Send */
//#define DUART_MSR_DDCD	0x08		/* Delta DCD */
//#define DUART_MSR_TERI	0x04		/* Trailing edge ring indicator */
//#define DUART_MSR_DDSR	0x02		/* Delta DSR */
#define DUART_MSR_DCTS	0x01		/* Delta CTS */
#define DUART_MSR_DDSR	0x02		/* Reserved */
#define DUART_MSR_TERI	0x04		/* Reserved */
#define DUART_MSR_DDCD	0x08		/* Reserved */
#define DUART_MSR_CTS	0x10		/* Clear to Send */
#define DUART_MSR_DSR	0x20		/* Reserved */
#define DUART_MSR_RI	0x40		/* Reserved */
#define DUART_MSR_DCD	0x80		/* Reserved */

// Interrupt Identification Register
#define DUART_IIR_NO_INT	0x01	/* No interrupts pending */
#define DUART_IIR_ID	0x06	/* Mask for the interrupt ID */

#define DUART_IIR_MSI	0x00	/* Modem status interrupt */
#define DUART_IIR_THRI	0x02	/* Transmitter holding register empty */
#define DUART_IIR_RDI	0x04	/* Receiver data interrupt */
#define DUART_IIR_RLSI	0x06	/* Receiver line status interrupt */

//  Interrupt Enable Register
#define DUART_IER_MSI	0x08	/* Enable Modem status interrupt */
#define DUART_IER_RLSI	0x04	/* Enable receiver line status interrupt */
#define DUART_IER_THRI	0x02	/* Enable Transmitter holding register int. */
#define DUART_IER_RDI	0x01	/* Enable receiver data interrupt */

// LCR defaults
#define DUART_LCR_8N1	0x03

#define DUART_LCRVAL DUART_LCR_8N1                /* 8 data, 1 stop, no parity */
#define DUART_MCRVAL (DUART_MCR_DTR | \
                     DUART_MCR_RTS)              /* RTS/DTR */
#define DUART_FCRVAL (DUART_FCR_FIFO_EN | \
                     DUART_FCR_RXSR |    \
                     DUART_FCR_TXSR)             /* Clear & enable FIFOs */


#define URBR		0x0
#define UTHR		0x0
#define UDLB	 	0x0	 
#define UDMB	 	0x1	 
#define UIER	 	0x1 
#define UIIR	 	0x2 
#define UFCR	 	0x2 
#define UAFR	 	0x2 
#define ULCR	 	0x3 
#define UMCR	 	0x4
#define ULSR		0x5
#define UMSR		0x6
#define USCR		0x7
#define UDSR		0x10

/*

  Programmed hardware of Serial port.

  @return    Always return EFI_UNSUPPORTED.

*/

RETURN_STATUS
EFIAPI
DuartInitializePort (
  IN OUT UINTN               UartBase,
  IN OUT UINT64              *BaudRate
  );


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
  IN  UINTN       UartBase,
  IN  UINT8       *Buffer,
  IN  UINTN       NumberOfBytes
  );

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
  IN  UINTN       UartBase,
  OUT UINT8       *Buffer,
  IN  UINTN       NumberOfBytes
  );

/**
  Check to see if any data is available to be read from the debug device.

  @retval EFI_SUCCESS       At least one byte of data is available to be read
  @retval EFI_NOT_READY     No data is available to be read
  @retval EFI_DEVICE_ERROR  The serial device is not functioning properly

**/
BOOLEAN
EFIAPI
DuartPoll (
  IN  UINTN       UartBase
  );

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
  );

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
  );

#endif /* __DUART_H__ */
