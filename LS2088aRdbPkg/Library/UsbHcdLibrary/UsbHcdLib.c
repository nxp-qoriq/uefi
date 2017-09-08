/** @file

  Copyright 2017 NXP 

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include  <Library/UsbHcd.h>

VOID
XhciSetBeatBurstLength (
  IN  UINTN  UsbReg
  )
{
  Dwc3 *Dwc3Reg;
  DEBUG((EFI_D_INFO, "XhciSetBeatBurstLength\n"));
  
  Dwc3Reg = (VOID *)(UsbReg + USB_REG_OFFSET);

  MmioClearSet32((UINTN)&Dwc3Reg->GSbuscfg0, USB_ENABLE_BEAT_BURST_MASK,
                                           USB_ENABLE_BEAT_BURST);
  MmioSetBits32((UINTN)&Dwc3Reg->GSbuscfg1, USB_SET_BEAT_BURST_LIMIT);

  return;
}

VOID
Dwc3SetFladj (
  IN  Dwc3 *Dwc3Reg,
  IN  UINT32 Val
  )
{
  DEBUG((EFI_D_INFO,"Frame length adjustment.\n"));
  MmioSetBits32((UINTN)&Dwc3Reg->GFLAdj, GFLADJ_30MHZ_REG_SEL |
                        GFLADJ_30MHZ(Val));
}

VOID
Dwc3SetMode (
  IN   Dwc3 *Dwc3Reg,
  IN   UINT32 Mode
  )
{
  DEBUG((EFI_D_INFO,"Configure controller in host mode.\n"));
  MmioClearSet32((UINTN)&Dwc3Reg->GCtl,
		 USB_GCTL_PRTCAPDIR(USB_GCTL_PRTCAP_OTG),
		 USB_GCTL_PRTCAPDIR(Mode));
}

VOID
Dwc3CoreSoftReset (
  IN  Dwc3 *Dwc3Reg
  )
{
  DEBUG((EFI_D_INFO,"Controller and Core reset.\n"));

  MmioOr32 ((UINTN)&Dwc3Reg->GCtl, USB_GCTL_CORESOFTRESET);
  MmioOr32 ((UINTN)&Dwc3Reg->GUsb3pipectl[0], USB_GUSB3PIPECTL_PHYSOFTRST);
  MmioOr32 ((UINTN)&Dwc3Reg->GUsb2phycfg, USB_GUSB2PHYCFG_PHYSOFTRST);
  MmioClearBits32((UINTN)&Dwc3Reg->GUsb3pipectl[0], USB_GUSB3PIPECTL_PHYSOFTRST);
  MmioClearBits32((UINTN)&Dwc3Reg->GUsb2phycfg, USB_GUSB2PHYCFG_PHYSOFTRST);
  MmioClearBits32((UINTN)&Dwc3Reg->GCtl, USB_GCTL_CORESOFTRESET);

  return;
}

EFI_STATUS
Dwc3CoreInit (
  IN  Dwc3 *Dwc3Reg
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 Revision;
  UINT32 Reg;
  UINTN Dwc3Hwparams1;

  DEBUG((EFI_D_INFO,"USB Core init.\n"));

  Revision = MmioRead32((UINTN)&Dwc3Reg->GSnpsid);
  /* This should read as U3 followed by revision number */
  if ((Revision & USB_GSNPSID_MASK) != 0x55330000) {
    DEBUG((EFI_D_ERROR,"This is not a DesignWare USB3 DRD Core.\n"));
    return EFI_NOT_FOUND;
  }
  Dwc3CoreSoftReset(Dwc3Reg);

  Reg = MmioRead32((UINTN)&Dwc3Reg->GCtl);
  Reg &= ~USB_GCTL_SCALEDOWN_MASK;
  Reg &= ~USB_GCTL_DISSCRAMBLE;

  Dwc3Hwparams1 = MmioRead32((UINTN)&Dwc3Reg->GHwparams1);
  switch (USB_GHWPARAMS1_EN_PWROPT(Dwc3Hwparams1)) {
    case USB_GHWPARAMS1_EN_PWROPT_CLK:
      Reg &= ~USB_GCTL_DSBLCLKGTNG;
      break;
    default:
      DEBUG((EFI_D_ERROR,"No power optimization available.\n"));
  }

  if ((Revision & USB_REVISION_MASK) < 0x190a)
    Reg |= USB_GCTL_U2RSTECN;

  MmioWrite32 ((UINTN)&Dwc3Reg->GCtl, Reg);

  return Status;
}

EFI_STATUS
XhciCoreInit (
  IN  UINTN  UsbReg
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  Dwc3 *Dwc3Reg = (VOID *)(UsbReg + USB_REG_OFFSET);

  DEBUG((EFI_D_INFO, "XhciCoreInit\n"));

  Status = Dwc3CoreInit(Dwc3Reg);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "Dwc3CoreInit Failed for controller 0x%x (0x%x) \n",
                  UsbReg, Status));
    return Status;
  }

  Dwc3SetMode(Dwc3Reg, USB_GCTL_PRTCAP_HOST);

  Dwc3SetFladj(Dwc3Reg, GFLADJ_30MHZ_DEFAULT);

  return Status;
}

EFI_STATUS
EFIAPI
InitializeUsbController (
  IN  UINTN  UsbReg
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = XhciCoreInit(UsbReg);

  /* Change beat burst and outstanding pipelined transfers requests */
  XhciSetBeatBurstLength(UsbReg);

  return Status;
}
