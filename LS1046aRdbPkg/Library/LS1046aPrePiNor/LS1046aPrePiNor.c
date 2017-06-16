/** @LS1046aPrePiNor.c
#
#  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
**/


#include <Ddr.h>
#include <Library/ArmLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <LS1046aRdb.h>
#include <LS1046aSocLib.h>
#include <Library/SerialPortLib.h>

#define DCSR_SATA_ERRATA     0x20140520
#define SERDES2_SATA_ERRATA  0x01eb1300

#define DCSR_SDHC_ERRATA            0x20000000
#define DCSR_DCFG_SDHC_ERRATA       (DCSR_SDHC_ERRATA + 0x00140000)
#define DCSR_PORCR1_SDHC_ERRATA     0x0

UINTN mGlobalVariableBase = 0;

VOID CopyImage(UINT8* Dest, UINT8* Src, UINTN Size)
{
  UINTN Count;

  for(Count = 0; Count < Size; Count++)
    Dest[Count] = Src[Count];
}

STATIC VOID ErratumA010539 (VOID)
{
  // Impact : SDHC_VS is pin muxed with SPI_* pins when source of RCW is QSPI
  // Standard and high speed modes are supported for SD card connected to
  // eSDHC interface.SDR modes(Ulta high speed) that require 1.8 V and GPIO
  // pins releated to SPI don't work
  // Workaround : Read DCFG_CCSR_PORSR1,clear RCW_SRC(bit 0:8) and write
  // updated value to 0x20140000.
  struct CcsrGur *GurBase = (VOID *)(GUTS_ADDR);
  UINT32 Temp = 0;

  Temp = MmioReadBe32((UINTN)&GurBase->porsr1);
  Temp &= ~FSL_CHASSIS2_CCSR_PORSR1_RCW_MASK;
  MmioWriteBe32((UINTN)(DCSR_DCFG_SDHC_ERRATA + DCSR_PORCR1_SDHC_ERRATA), Temp);
  MmioWriteBe32((UINTN)0x015701A8, 0xFFFFFFFF);

}

STATIC VOID ErratumSata (VOID)
{
  // Impact : The SATA controller does not detect some hard drives reliably with
  // the default SerDes register setting.
  // Workaround : write value 0x80104e20 to 0x1eb1300 (serdes 2)
  if (PcdGetBool(PcdSataErratumA010554)) {
    MmioWriteBe32((UINTN)SERDES2_SATA_ERRATA, 0x80104e20);
  }

  // Impact : Device may see false CRC errors causing unreliable SATA operation.
  // Workaround : write 0x80000000 to the address 0x20140520 (dcsr).
  if (PcdGetBool(PcdSataErratumA010635)) {
      MmioWrite32((UINTN)DCSR_SATA_ERRATA, 0x80000000);
  }
}

STATIC VOID ErratumA008550Pre (VOID)
{
  struct CcsrCci400 *Base = (struct CcsrCci400 *)CONFIG_SYS_CCI400_ADDR;
  struct CcsrDdr *Ddr = (VOID *)CONFIG_SYS_FSL_DDR_ADDR;

  /* disables barrier transactions to DDRC from CCI400 */
  MmioWrite32((UINTN)&Base->ctrl_ord, CCI400_CTRLORD_TERM_BARRIER);

  /* disable re-ordering in DDR Controller */
  MmioWriteBe32((UINTN)&Ddr->Eor,
	DDR_EOR_READ_REOD_DIS | DDR_EOR_WRITE_REOD_DIS);
}

STATIC VOID ErratumA008550Post(VOID)
{
  struct CcsrCci400 *Base = (struct CcsrCci400 *)CONFIG_SYS_CCI400_ADDR;
  struct CcsrDdr *Ddr = (VOID *)CONFIG_SYS_FSL_DDR_ADDR;
  UINT32 Temp = 0;

  /* enable barrier transactions to DDRC from CCI400 */
  MmioWrite32((UINTN)&Base->ctrl_ord, CCI400_CTRLORD_EN_BARRIER);

  /* enable re-ordering in DDR Controller */
  Temp = MmioReadBe32((UINTN)&Ddr->Eor);
  Temp &= ~(DDR_EOR_READ_REOD_DIS | DDR_EOR_WRITE_REOD_DIS);
  MmioWriteBe32((UINTN)&Ddr->Eor, Temp);
}
/*
* A-009008: USB High Speed (HS) eye height adjustment
* Affects: USB
* Description: USB HS eye diagram fails with the default value at many corners, particularly at a high
* temperature (105°C).
* Impact: USB HS eye diagram may fail using the default value.
*/
STATIC VOID ErratumA009008 (VOID)
{
  struct CcsrScfg *Scfg = (VOID *)SCFG_BASE_ADDR;
  UINT32 Value = MmioReadBe32((UINTN)&Scfg->usb1prm1cr);
  Value &= ~(0xF << 6);
  MmioWriteBe32((UINTN)&Scfg->usb1prm1cr, Value|(USB_TXVREFTUNE << 6));
  Value = MmioReadBe32((UINTN)&Scfg->usb2prm1cr);
  Value &= ~(0xF << 6);
  MmioWriteBe32((UINTN)&Scfg->usb2prm1cr, Value|(USB_TXVREFTUNE << 6));
  Value = MmioReadBe32((UINTN)&Scfg->usb3prm1cr);
  Value &= ~(0xF << 6);
  MmioWriteBe32((UINTN)&Scfg->usb3prm1cr, Value|(USB_TXVREFTUNE << 6));
}
/*
* A-009798: USB high speed squelch threshold adjustment
* Affects: USB
* Description: The default setting for USB high speed squelch threshold results in a threshold close to or
* lower than 100mV. This leads to a receiver compliance test failure for a 100mV threshold.
* Impact: If the errata is not applied, only the USB high speed receiver sensitivity compliance test fails,
* however USB data continues to transfer.
*/
STATIC VOID ErratumA009798 (VOID)
{
  struct CcsrScfg *Scfg = (VOID *)SCFG_BASE_ADDR;
  UINT32 Value = MmioReadBe32((UINTN)&Scfg->usb1prm1cr);
  MmioWriteBe32((UINTN)&Scfg->usb1prm1cr, Value & USB_SQRXTUNE);
  Value = MmioReadBe32((UINTN)&Scfg->usb2prm1cr);
  MmioWriteBe32((UINTN)&Scfg->usb2prm1cr, Value & USB_SQRXTUNE);
  Value = MmioReadBe32((UINTN)&Scfg->usb3prm1cr);
  MmioWriteBe32((UINTN)&Scfg->usb3prm1cr, Value & USB_SQRXTUNE);
}
/*
* A-008997: USB3 LFPS peak-to-peak differential output voltage adjustment settings
* Affects: USB
* Description: Low Frequency Periodic Signaling (LFPS) peak-to-peak differential output voltage test
* compliance fails using default transmitter settings. Software is required to change the
* transmitter signal swings to pass compliance tests.
* Impact: LFPS peak-to-peak differential output voltage compliance test fails.
*/
STATIC VOID ErratumA008997 (VOID)
{
  struct CcsrScfg *Scfg = (VOID *)SCFG_BASE_ADDR;
  UINT32 Value = MmioReadBe32((UINTN)&Scfg->usb1prm2cr);
  Value &= ~(0x7F << 9);
  MmioWriteBe32((UINTN)&Scfg->usb1prm2cr, Value | (USB_PCSTXSWINGFULL << 9));
  Value = MmioReadBe32((UINTN)&Scfg->usb2prm2cr);
  Value &= ~(0x7F << 9);
  MmioWriteBe32((UINTN)&Scfg->usb2prm2cr, Value | (USB_PCSTXSWINGFULL << 9));
  Value = MmioReadBe32((UINTN)&Scfg->usb3prm2cr);
  Value &= ~(0x7F << 9);
  MmioWriteBe32((UINTN)&Scfg->usb3prm2cr, Value | (USB_PCSTXSWINGFULL << 9));
}
/*
* A-009007: USB3PHY observing intermittent failure in receive compliance tests at
* higher jitter frequency using default register values
* Affects: USB
* Description: Receive compliance tests may fail intermittently at high jitter frequencies using default register
* values.
* Impact: Receive compliance test fails at default register setting.
*/
STATIC VOID ErratumA009007 (VOID)
{
  struct CcsrUsbPhySs *UsbPhy = (VOID *)USB_PHY1;
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)&UsbPhy->Lane0RxOvrdInHi, USB_PHY_RX_EQ_VAL_1);
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)&UsbPhy->Lane0RxOvrdInHi, USB_PHY_RX_EQ_VAL_2);
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)&UsbPhy->Lane0RxOvrdInHi, USB_PHY_RX_EQ_VAL_3);
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)&UsbPhy->Lane0RxOvrdInHi, USB_PHY_RX_EQ_VAL_4);
  UsbPhy = (VOID *)USB_PHY2;
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)&UsbPhy->Lane0RxOvrdInHi, USB_PHY_RX_EQ_VAL_1);
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)&UsbPhy->Lane0RxOvrdInHi, USB_PHY_RX_EQ_VAL_2);
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)&UsbPhy->Lane0RxOvrdInHi, USB_PHY_RX_EQ_VAL_3);
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)&UsbPhy->Lane0RxOvrdInHi, USB_PHY_RX_EQ_VAL_4);
  UsbPhy = (VOID *)USB_PHY3;
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)&UsbPhy->Lane0RxOvrdInHi, USB_PHY_RX_EQ_VAL_1);
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)&UsbPhy->Lane0RxOvrdInHi, USB_PHY_RX_EQ_VAL_2);
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)&UsbPhy->Lane0RxOvrdInHi, USB_PHY_RX_EQ_VAL_3);
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)&UsbPhy->Lane0RxOvrdInHi, USB_PHY_RX_EQ_VAL_4);
}

VOID CEntryPoint(
  UINTN	UefiMemoryBase,
  UINTN 	UefiNorBase,
  UINTN	UefiMemorySize
  )
{
  struct CcsrScfg *Scfg = (VOID *)SCFG_BASE_ADDR;
  VOID	(*PrePiStart)(VOID);

  /* Make SEC and USB reads and writes snoopable */
  MmioSetBitsBe32((UINTN)&Scfg->snpcnfgcr, CCSR_SCFG_SNPCNFGCR_SECRDSNP |
		     CCSR_SCFG_SNPCNFGCR_SECWRSNP | CCSR_SCFG_SNPCNFGCR_USB1RDSNP |
		     CCSR_SCFG_SNPCNFGCR_USB1WRSNP | CCSR_SCFG_SNPCNFGCR_USB2RDSNP |
		     CCSR_SCFG_SNPCNFGCR_USB2WRSNP | CCSR_SCFG_SNPCNFGCR_USB3RDSNP |
		     CCSR_SCFG_SNPCNFGCR_USB3WRSNP | CCSR_SCFG_SNPCNFGCR_SATARDSNP |
		     CCSR_SCFG_SNPCNFGCR_SATAWRSNP);

  if (PcdGetBool(PcdDdrErratumA008550))
    ErratumA008550Pre();

  if (PcdGetBool(PcdUsbErratumA009008))
    ErratumA009008();
  if (PcdGetBool(PcdUsbErratumA009798))
    ErratumA009798();
  if (PcdGetBool(PcdUsbErratumA008997))
    ErratumA008997();
  if (PcdGetBool(PcdUsbErratumA009007))
    ErratumA009007();

  ErratumSata();

  if (PcdGetBool(PcdSdhcErratumA010539))
    ErratumA010539();

  // Data Cache enabled on Primary core when MMU is enabled.
  ArmDisableDataCache ();
  // Invalidate Data cache
  //ArmInvalidateDataCache ();
  // Invalidate instruction cache
  ArmInvalidateInstructionCache ();
  // Enable Instruction Caches on all cores.
  ArmEnableInstructionCache ();

  SerialPortInitialize ();

  TimerInit();
  DramInit();

  if (PcdGetBool(PcdDdrErratumA008550))
    ErratumA008550Post();

  CopyImage((VOID*)UefiMemoryBase, (VOID*)UefiNorBase, UefiMemorySize);

  PrePiStart = (VOID (*)())((UINT64)PcdGet64(PcdFvBaseAddress));
  PrePiStart();
}
