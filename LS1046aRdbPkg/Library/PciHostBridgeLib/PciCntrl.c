/** PciCntrl.c
 Provides the basic interfaces to be used by PCI Root Bridge IO protocol

 Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution. The full text of the license may be found 
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/ 

#include "PciHostBridge.h"
#include <Library/DebugLib.h>
#include <Base.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseMemoryLib/MemLibInternals.h>
#include <Library/BaseLib.h>

PCI_ROOT_BRIDGE_INSTANCE      *RootBridgeHead;


VOID
PciSetRegion (
	IN struct PciRegion *Reg,
	IN PciAddrT BusStart,
	IN PhysAddrT PhysStart,
	IN PciSizeT Size,
	IN UINTN Flags
) 
{
	Reg->BusStart	= BusStart;
	Reg->PhysStart = PhysStart;
	Reg->Size	= Size;
	Reg->Flags	= Flags;
}

VOID
SetLSPcieInfo (
	IN struct LsPcieInfo *PcieInfo,
	IN UINTN Num
)
{
	UINTN pciePhyAddr=0, pcieSysaddr=0;

	switch(Num){
		case 1:
			pciePhyAddr = CONFIG_SYS_PCIE1_PHYS_ADDR;
			pcieSysaddr = CONFIG_SYS_PCIE1_ADDR;
			break;
		case 2:
			pciePhyAddr = CONFIG_SYS_PCIE2_PHYS_ADDR;
			pcieSysaddr = CONFIG_SYS_PCIE2_ADDR;
			break;
		case 3:
			pciePhyAddr = CONFIG_SYS_PCIE3_PHYS_ADDR;
			pcieSysaddr = CONFIG_SYS_PCIE3_ADDR;
			break;
	}

	PcieInfo->Regs = pcieSysaddr;
	PcieInfo->Cfg0Phys = CONFIG_SYS_LS_PCIE_CFG0_PHYS_OFF + pciePhyAddr;
	PcieInfo->Cfg0Size = CONFIG_SYS_LS_PCIE_CFG0_SIZE;
	PcieInfo->Cfg1Phys = CONFIG_SYS_LS_PCIE_CFG1_PHYS_OFF + pciePhyAddr;
	PcieInfo->Cfg1Size = CONFIG_SYS_LS_PCIE_CFG1_SIZE;
	PcieInfo->MemBus = CONFIG_SYS_LS_PCIE_MEM_BUS;
	PcieInfo->MemPhys = CONFIG_SYS_LS_PCIE_MEM_PHYS_OFF + pciePhyAddr;
	PcieInfo->MemSize = CONFIG_SYS_LS_PCIE_MEM_SIZE;
	PcieInfo->IoBus = CONFIG_SYS_LS_PCIE_IO_BUS;
	PcieInfo->IoPhys = CONFIG_SYS_LS_PCIE_IO_PHYS_OFF + pciePhyAddr;
	PcieInfo->IoSize = CONFIG_SYS_LS_PCIE_IO_SIZE;
	PcieInfo->PciNum = Num;

     if (FeaturePcdGet(PcdPciDebug) == TRUE) {
      DEBUG ((EFI_D_INFO, "In SET_PCIE_INFO: %d\n", Num));
      DEBUG((EFI_D_INFO, "PciNum:%d Info CFG Values: %016llx %016llx:%016llx %016llx %016llx\n",
	     (UINT64)PcieInfo->PciNum,
	     (UINT64)PcieInfo->Regs,
	     (UINT64)PcieInfo->Cfg0Phys,
	     (UINT64)PcieInfo->Cfg0Size,
	     (UINT64)PcieInfo->Cfg1Phys,
	     (UINT64)PcieInfo->Cfg1Size));
      DEBUG((EFI_D_INFO, "Info Mem Values: %016llx:%016llx %016llx\n",
	     (UINT64)PcieInfo->MemBus,
	     (UINT64)PcieInfo->MemPhys,
	     (UINT64)PcieInfo->MemSize));
      DEBUG((EFI_D_INFO, "Info IO Values: %016llx:%016llx %016llx\n",
	     (UINT64)PcieInfo->IoBus,
	     (UINT64)PcieInfo->IoPhys,
	     (UINT64)PcieInfo->IoSize));
     }
}

//
// Implementation
//
BOOLEAN
IsPcieEnabled(
  IN UINTN PCIeBaseAddr
)
{
	UINT64 SerDes1ProtocolMap = 0x0, SerDes2ProtocolMap = 0x0;

	GetSerdesProtocolMaps(&SerDes1ProtocolMap, &SerDes2ProtocolMap);
	switch(PCIeBaseAddr){
	case CONFIG_SYS_PCIE1_ADDR:
		return(IsSerDesLaneProtocolConfigured(SerDes1ProtocolMap, SerDes2ProtocolMap, PCIE1));
	case CONFIG_SYS_PCIE2_ADDR:
		return(IsSerDesLaneProtocolConfigured(SerDes1ProtocolMap, SerDes2ProtocolMap, PCIE2));
	case CONFIG_SYS_PCIE3_ADDR:
		return(IsSerDesLaneProtocolConfigured(SerDes1ProtocolMap, SerDes2ProtocolMap, PCIE3));
	default:
		DEBUG((EFI_D_ERROR, "Device not supported\n"));
		break;
	}

	return FALSE;
}

/* PEX1/2 Misc Ports Status Register */
#define LTSSM_STATE_SHIFT	20

INTN
PcieLinkState (
	IN struct LsPcie *Pcie
)
{
	UINT32 State;
	State = MmioRead32(((UINTN)(Pcie->Dbi) + LS_PCIE_LUT_BASE + LS_PCIE_LUT_DBG) &
		LS_LTSSM_STATE_MASK);
	if (State < LS_LTSSM_PCIE_L0) {
		DEBUG((EFI_D_ERROR," Pcie Link error. LTSSM=0x%2x\n",
		       State));
		return 0;
	}
	return 1;
}

INTN
PcieLinkUp (
	IN struct LsPcie *Pcie
)	
{
	INTN State;
	UINT32 Cap;

	State = PcieLinkState(Pcie);
	if (State)
		return State;

	/* Try to download speed to gen1 */
	Cap = MmioRead32((UINTN)(Pcie->Dbi) + LS_PCIE_LINK_CAP);
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_LINK_CAP, (UINT32)(Cap & (~LS_PCIE_LINK_SPEED_MASK)) | 1);
	State = PcieLinkState(Pcie);
	if (State)
		return State;

	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_LINK_CAP, Cap);

	return 0;
}

VOID
PcieCfg0SetBusdev (
	IN struct LsPcie *Pcie,
	IN UINT32 BusDev
)
{
	MmioWrite32((UINTN)(Pcie->Dbi + LS_PCIE_ATU_VIEWPORT), 
		    LS_PCIE_ATU_REGION_OUTBOUND | LS_PCIE_ATU_REGION_INDEX0);
	MmioWrite32((UINTN)(Pcie->Dbi + LS_PCIE_ATU_LOWER_TARGET), BusDev);
}

VOID
PcieCfg1SetBusdev (
	IN struct LsPcie *Pcie,
	IN UINT32 BusDev
)
{
	MmioWrite32((UINTN)(Pcie->Dbi + LS_PCIE_ATU_VIEWPORT), 
		    LS_PCIE_ATU_REGION_OUTBOUND | LS_PCIE_ATU_REGION_INDEX1);
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_ATU_LOWER_TARGET, BusDev);
}

VOID
PcieIatuInboundSet (
	IN struct LsPcie *Pcie,
	IN UINT32 Idx,
	IN UINT32 Bar,
	IN UINT64 Phys
)
{
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_ATU_VIEWPORT, (UINT32)(LS_PCIE_ATU_REGION_INBOUND | Idx));
	
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_ATU_LOWER_BASE, Phys);
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_ATU_UPPER_BASE, (UINT32)(Phys >> 32));
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_ATU_LOWER_TARGET, (UINT32)Phys);
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_ATU_UPPER_TARGET, (UINT32)(Phys >> 32));
	
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_ATU_CR1, LS_PCIE_ATU_TYPE_MEM);
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_ATU_CR2, LS_PCIE_ATU_ENABLE);
}


VOID
PcieIatuOutboundSet (
	IN struct LsPcie *Pcie,
	IN UINT32 Idx,
	IN UINT32 Type,
	IN UINT64 Phys,
	IN UINT64 BusAddr,
	IN UINT64 Size
)
{
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_ATU_VIEWPORT, (UINT32)(LS_PCIE_ATU_REGION_OUTBOUND | Idx));
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_ATU_LOWER_BASE, Phys);
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_ATU_UPPER_BASE, (UINT32)(Phys >> 32));
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_ATU_LIMIT, (UINT32)(Phys + Size -1));
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_ATU_LOWER_TARGET, (UINT32)BusAddr);
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_ATU_UPPER_TARGET, (UINT32)(BusAddr >> 32));
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_ATU_CR1, (UINT32)Type);
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_ATU_CR2, LS_PCIE_ATU_ENABLE);
}

EFI_STATUS
PciSkipDev (
  IN PCI_ROOT_BRIDGE_INSTANCE      *PrivateData,
  IN UINT32 Dev
)
{
	/* Do not skip controller */
	return EFI_SUCCESS;
}

EFI_STATUS
PcieAddrValid (
  IN PCI_ROOT_BRIDGE_INSTANCE      *PrivateData,
  IN UINT32 Dev
)
{
	if ((((Dev >> 19)) & 0x1f) > 0)
		return EFI_DEVICE_ERROR;

	/* No support for multi-function */
	if ((((Dev >> 16)) & 0x07) > 0)
		return EFI_DEVICE_ERROR;

	/* Controller does not support multi-function in RC mode */
	if (((((Dev) >> 24) & 0xff) == PrivateData->FirstBusno) && ((((Dev) >> 16) & 0x7) > 0))
		return EFI_DEVICE_ERROR;

	return 0;
}

EFI_STATUS
PcieReadConfig (
  IN PCI_ROOT_BRIDGE_INSTANCE      *PrivateData,
  IN UINT32 Dev,
  IN INT32 Where,
  IN OUT UINT32 *Val
)
{
	struct LsPcie *Pcie = PrivateData->Pcie;
	UINT32 BusDev, *Addr;
	*Val = 0x00000000;

	if (PcieAddrValid(PrivateData, Dev)) {
		*Val = 0xffffffff;
		return EFI_DEVICE_ERROR;
	}

	if ((((Dev) >> 24) & 0xff) == PrivateData->FirstBusno) {
		Addr = Pcie->Dbi + Where;
	} else {
		BusDev = Dev;

		if ((((Dev) >> 24) & 0xff) == PrivateData->FirstBusno + 1) {
			PcieCfg0SetBusdev(Pcie, BusDev);
			Addr = (VOID *)Pcie->VaCfg0 + Where;
		} else {
			PcieCfg1SetBusdev(Pcie, BusDev);
			Addr = (VOID *)Pcie->VaCfg1 + Where;
		}
	}

	*Val = MmioRead32((UINTN)Addr);
	return 0;
}

EFI_STATUS
PcieReadConfigByte (
  IN PCI_ROOT_BRIDGE_INSTANCE      *PrivateData,
  IN UINT32 Dev,
  IN INT32 Offset,
  IN OUT UINT8 *Val
)
{
  UINT32 Val32;

  if (EFI_ERROR(PcieReadConfig(PrivateData, Dev, Offset & 0xfc, &Val32))) {
    *Val = -1;
    return -1;
  }

  *Val = (Val32 >> ((Offset & (INT32)0x03) * 8));

  return EFI_SUCCESS;
}

EFI_STATUS
PcieReadConfigWord (
  IN PCI_ROOT_BRIDGE_INSTANCE      *PrivateData,
  IN UINT32 Dev,
  IN INT32 Offset,
  IN OUT UINT16 *Val
)
{
  UINT32 Val32;

  if (EFI_ERROR(PcieReadConfig(PrivateData, Dev, Offset & 0xfc, &Val32))) {
    *Val = -1;
    return -1;
  }

  *Val = (UINT16)(Val32 >> ((Offset & (INT32)0x02) * 8));

  return EFI_SUCCESS;
}

EFI_STATUS
PcieWriteConfigByte (
  IN PCI_ROOT_BRIDGE_INSTANCE      *PrivateData,
  IN UINT32 Dev,
  IN INT32 Offset,
  OUT UINT8 Val
)
{
  UINT32 Val32, Mask, Ldata, Shift;
  
  if (EFI_ERROR(PcieReadConfig(PrivateData, Dev, Offset & 0xfc, &Val32)))
    return -1;

  Shift = ((Offset & (INT32)0x03) * 8);
  Ldata = (((UINT32)Val) & 0x000000ff) << Shift;
  Mask = 0x000000ff << Shift;
  Val32 = (Val32 & ~Mask) | Ldata;

  if (EFI_ERROR(PcieWriteConfig(PrivateData, Dev, Offset & 0xfc, Val32)))
    return -1;

  return EFI_SUCCESS;
}

EFI_STATUS
PcieWriteConfigWord (
  IN PCI_ROOT_BRIDGE_INSTANCE      *PrivateData,
  IN UINT32 Dev,
  IN INT32 Offset,
  OUT UINT16 Val
)
{
  UINT32 Val32, Mask, Ldata, Shift;
  
  if (EFI_ERROR(PcieReadConfig(PrivateData, Dev, Offset & 0xfc, &Val32)))
    return -1;

  Shift = ((Offset & (INT32)0x02) * 8);
  Ldata = (((UINT32)Val) & 0x0000ffff) << Shift;
  Mask = 0x0000ffff << Shift;
  Val32 = (Val32 & ~Mask) | Ldata;

  if (EFI_ERROR(PcieWriteConfig(PrivateData, Dev, Offset & 0xfc, Val32)))
    return -1;

  return EFI_SUCCESS;
}

EFI_STATUS
PcieWriteConfig (
  IN PCI_ROOT_BRIDGE_INSTANCE      *PrivateData,
  IN UINT32 Dev,
  IN INT32 Where,
  IN UINT32 Val
)
{
	struct LsPcie *Pcie = PrivateData->Pcie;
	UINT32 BusDev, *Addr;

	if (PcieAddrValid(PrivateData, Dev)) {
		return EFI_DEVICE_ERROR;
	}

	if ((((Dev) >> 24) & 0xff) == PrivateData->FirstBusno) {
		Addr = Pcie->Dbi + (Where & ~0x3);
	} else {
		BusDev = Dev;

		if ((((Dev) >> 24) & 0xff) == PrivateData->FirstBusno + 1) {
			PcieCfg0SetBusdev(Pcie, BusDev);
			Addr = (VOID *)Pcie->VaCfg0 + (Where & ~0x3);
		} else {
			PcieCfg1SetBusdev(Pcie, BusDev);
			Addr = (VOID *)Pcie->VaCfg1 + (Where & ~0x3);
		}
	}

	MmioWrite32((UINTN)Addr, (UINT32)Val);
	return EFI_SUCCESS;
}

VOID
PcieSetupAtuInbound (
	IN struct LsPcie *Pcie,
	IN struct LsPcieInfo *Info
)
{
	UINT32 ValIn = 0x00000000;
	UINT32 ValOut = 0x28282828;
	UINT32 *Addr = (UINT32 *)0x80080000;

	MmioWrite32((UINTN)Addr, (UINT32)ValOut);
	DEBUG((EFI_D_INFO,"\nValue: %08lx written on Addr: %08lx\n", ValOut, Addr));
	ValIn = MmioRead32((UINTN)Addr);
	DEBUG((EFI_D_INFO, "Value Read: %08lx from Address: %llx\n", ValIn, Addr));
	/* ATU 2 : OUTBOUND : MEM */
	PcieIatuInboundSet(Pcie, LS_PCIE_ATU_REGION_INDEX2,
				  1,
				  0x80080000);

	Addr = (VOID *)Pcie->VaCfg0 + 0x40000000;
	ValIn = MmioRead32((UINTN)Addr);
	DEBUG((EFI_D_INFO, "Inbound: Value Read: %08lx from Address: %llx\n", ValIn, Addr));
	Addr = (VOID *)Pcie->VaCfg0 + 0x00000000;
	ValIn = MmioRead32((UINTN)Addr);
	DEBUG((EFI_D_INFO, "Inbound: Value Read: %08lx from Address: %llx\n", ValIn, Addr));
        Addr = (UINT32 *)0x80080000;	
	ValIn = MmioRead32((UINTN)Addr);
	DEBUG((EFI_D_INFO, "Inbound: Value Read: %08lx from Address: %llx\n", ValIn, Addr));

}

VOID
PcieSetupAtu (
	IN struct LsPcie *Pcie,
	IN struct LsPcieInfo *Info
)
{
	 /* ATU 0 : OUTBOUND : CFG0 */
	PcieIatuOutboundSet(Pcie, LS_PCIE_ATU_REGION_INDEX0,
				  LS_PCIE_ATU_TYPE_CFG0,
				  Info->Cfg0Phys,
				  0,
				  Info->Cfg0Size);

	/* ATU 1 : OUTBOUND : CFG1 */
	PcieIatuOutboundSet(Pcie, LS_PCIE_ATU_REGION_INDEX1,
				  LS_PCIE_ATU_TYPE_CFG1,
				  Info->Cfg1Phys,
				  0,
				  Info->Cfg1Size);
	/* ATU 2 : OUTBOUND : MEM */
	PcieIatuOutboundSet(Pcie, LS_PCIE_ATU_REGION_INDEX2,
				  LS_PCIE_ATU_TYPE_MEM,
				  Info->MemPhys,
				  Info->MemBus,
				  //0x10000000,
				  Info->MemSize);
	/* ATU 3 : OUTBOUND : IO */
	PcieIatuOutboundSet(Pcie, LS_PCIE_ATU_REGION_INDEX3,
				  LS_PCIE_ATU_TYPE_IO,
				  Info->IoPhys,
				  Info->IoBus,
				  Info->IoSize);

       if (FeaturePcdGet(PcdPciDebug) == TRUE) {
          INTN Cnt;
          UINTN AddrTemp;
 	  for (Cnt = 0; Cnt <= LS_PCIE_ATU_REGION_INDEX3; Cnt++) {
		MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_ATU_VIEWPORT,
			    (UINT32)(LS_PCIE_ATU_REGION_OUTBOUND | Cnt));
		DEBUG((EFI_D_INFO,"iATU%d:\n", Cnt));
		AddrTemp = (UINTN)((UINTN)Pcie->Dbi + LS_PCIE_ATU_VIEWPORT);
		DEBUG((EFI_D_INFO,"iATU%d VIEWPORT REG Addr:%08lx Val:%08lx\n",
		       Cnt, AddrTemp, MmioRead32(AddrTemp)));
		DEBUG((EFI_D_INFO,"iATU%d VIEWPORT REG:%08lx\n", Cnt,
		       MmioRead32((UINTN)Pcie->Dbi + LS_PCIE_ATU_VIEWPORT)));
		DEBUG((EFI_D_INFO,"\tLOWER PHYS 0x%08x\n",
		      MmioRead32((UINTN)Pcie->Dbi + LS_PCIE_ATU_LOWER_BASE)));
		DEBUG((EFI_D_INFO,"\tUPPER PHYS 0x%08x\n",
		      MmioRead32((UINTN)Pcie->Dbi + LS_PCIE_ATU_UPPER_BASE)));
		DEBUG((EFI_D_INFO,"\tLOWER BUS  0x%08x\n",
		      MmioRead32((UINTN)Pcie->Dbi + LS_PCIE_ATU_LOWER_TARGET)));
		DEBUG((EFI_D_INFO,"\tUPPER BUS  0x%08x\n",
		      MmioRead32((UINTN)Pcie->Dbi + LS_PCIE_ATU_UPPER_TARGET)));
		DEBUG((EFI_D_INFO,"\tLIMIT      0x%08x\n",
		      MmioRead32((UINTN)Pcie->Dbi + LS_PCIE_ATU_LIMIT)));
		DEBUG((EFI_D_INFO,"\tCR1        0x%08x\n",
		      MmioRead32((UINTN)Pcie->Dbi + LS_PCIE_ATU_CR1)));
		DEBUG((EFI_D_INFO,"\tCR2        0x%08x\n",
		      MmioRead32((UINTN)Pcie->Dbi + LS_PCIE_ATU_CR2)));
	}
      }
}

VOID
PcieSetupCntrl (
  IN PCI_ROOT_BRIDGE_INSTANCE      *PrivateData,
  IN struct LsPcie 		*Pcie,
  IN struct LsPcieInfo 		*Info
)
{
	UINTN Dev = ((PrivateData->FirstBusno) << 16 | (0) << 11 | (0) << 8);

        if (FeaturePcdGet(PcdPciDebug) == TRUE)
	  DEBUG((EFI_D_INFO, "Going to SetUp IATU\n\n"));
	PcieSetupAtu(Pcie, Info);

	PcieWriteConfig(PrivateData, Dev, LS_PCI_BASE_ADDRESS_0, 0);

	/* program correct class for RC */
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_DBI_RO_WR_EN, (UINT32)1);
	PcieWriteConfigWord(PrivateData, Dev, LS_PCI_CLASS_DEVICE,
			    PCI_CLASS_BRIDGE_PCI);
	MmioWrite32((UINTN)Pcie->Dbi + LS_PCIE_DBI_RO_WR_EN, (UINT32)0);
}
