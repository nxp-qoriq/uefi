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
#include <LS2088aRdb.h>
#include <Library/IoLib.h>

#include "UsbHcd.h"

VOID
Dwc3SetFladj (
  IN  Dwc3 *Dwc3Reg,
  IN  UINT32 Val
  )
{
  DEBUG((EFI_D_INFO,"Frame length adjustment.\n"));
  MmioSetBits32((UINTN)&Dwc3Reg->GFladj, GFLADJ_30MHZ_REG_SEL |
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
		 DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG),
		 DWC3_GCTL_PRTCAPDIR(Mode));
}

EFI_STATUS
Dwc3CoreSoftReset (
  IN  Dwc3 *Dwc3Reg
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  DEBUG((EFI_D_INFO,"Controller and Core reset.\n"));

  MmioOr32 ((UINTN)&Dwc3Reg->GCtl, DWC3_GCTL_CORESOFTRESET);
  MmioOr32 ((UINTN)&Dwc3Reg->GUsb3pipectl[0], DWC3_GUSB3PIPECTL_PHYSOFTRST);
  MmioOr32 ((UINTN)&Dwc3Reg->GUsb2phycfg, DWC3_GUSB2PHYCFG_PHYSOFTRST);
  MmioClearBits32((UINTN)&Dwc3Reg->GUsb3pipectl[0], DWC3_GUSB3PIPECTL_PHYSOFTRST);
  MmioClearBits32((UINTN)&Dwc3Reg->GUsb2phycfg, DWC3_GUSB2PHYCFG_PHYSOFTRST);
  MmioClearBits32((UINTN)&Dwc3Reg->GCtl, DWC3_GCTL_CORESOFTRESET);

  return Status;
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
  if ((Revision & DWC3_GSNPSID_MASK) != 0x55330000) {
    DEBUG((EFI_D_ERROR,"This is not a DesignWare USB3 DRD Core.\n"));
    return EFI_NOT_FOUND;
  }
  Status = Dwc3CoreSoftReset(Dwc3Reg);

  Dwc3Hwparams1 = MmioRead32((UINTN)&Dwc3Reg->GHwparams1);
  Reg = MmioRead32((UINTN)&Dwc3Reg->GCtl);
  Reg &= ~DWC3_GCTL_SCALEDOWN_MASK;
  Reg &= ~DWC3_GCTL_DISSCRAMBLE;

  switch (DWC3_GHWPARAMS1_EN_PWROPT(Dwc3Hwparams1)) {
    case DWC3_GHWPARAMS1_EN_PWROPT_CLK:
      Reg &= ~DWC3_GCTL_DSBLCLKGTNG;
      break;
    default:
      DEBUG((EFI_D_ERROR,"No power optimization available.\n"));
  }

  if ((Revision & DWC3_REVISION_MASK) < 0x190a)
    Reg |= DWC3_GCTL_U2RSTECN;

  MmioWrite32 ((UINTN)&Dwc3Reg->GCtl, Reg);

  return Status;
}

EFI_STATUS
XhciCoreInit (
  IN  VOID
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  Dwc3 *Dwc3Reg = (VOID *)(USB1_REG_ADDR + DWC3_REG_OFFSET);

  DEBUG((EFI_D_INFO, "XhciCoreInit\n"));

  Status = Dwc3CoreInit(Dwc3Reg);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "Dwc3CoreInit Failed 0x%x \n", Status));
    return Status;
  }

  Dwc3SetMode(Dwc3Reg, DWC3_GCTL_PRTCAP_HOST);

  Dwc3SetFladj(Dwc3Reg, GFLADJ_30MHZ_DEFAULT);

  return Status;
}

EFI_STATUS
ApplyXhciErrata (
  IN VOID
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UsbHcdEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status =  ApplyXhciErrata();
  Status = XhciCoreInit();

  return Status;
}
