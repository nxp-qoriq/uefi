/** PciRootBridge.h
 The Header file of the Pci Root Bridge driver 

 Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution. The full text of the license may be found 
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/ 

#ifndef _PCI_LS_H_
#define _PCI_LS_H_

#include <Library/IoLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/DevicePath.h>

#define MAX_PCI_REGIONS         7



typedef UINT64 PciAddrT;
typedef UINT64 PciSizeT;
typedef UINT64 PhysAddrT;
typedef UINT64 PhysSizeT;

typedef INTN PciDevT;

struct PciRegion {
	PciAddrT BusStart;	/* Start on the bus */
	PhysAddrT PhysStart;	/* Start in physical address space */
	PciSizeT Size;	/* Size */
	UINT64 Flags;	/* Resource flags */

	PciAddrT BusLower;
};

struct LsPcieInfo {
	UINT64 Regs;
	INT32 PciNum;
	UINT64 Cfg0Phys;
	UINT64 Cfg0Size;
	UINT64 Cfg1Phys;
	UINT64 Cfg1Size;
	UINT64 MemBus;
	UINT64 MemPhys;
	UINT64 MemSize;
	UINT64 IoBus;
	UINT64 IoPhys;
	UINT64 IoSize;
};

struct LsPcie {
	INTN Idx;
	VOID *Dbi;
	UINT32 *VaCfg0;
	UINT32 *VaCfg1;
};

struct PciDeviceId {
	UINTN Vendor, Device;		/* Vendor and device ID or PCI_ANY_ID */
};

struct PciConfigTable {
	UINTN Vendor, Device;		/* Vendor and device ID or PCI_ANY_ID */
	UINTN Class;			/* Class ID, or  PCI_ANY_ID */
	UINTN Bus;			/* Bus number, or PCI_ANY_ID */
	UINTN Dev;			/* Device number, or PCI_ANY_ID */
	UINTN Func;			/* Function number, or PCI_ANY_ID */

	UINTN Priv[3];
};

#define ACPI_SPECFLAG_PREFETCHABLE      0x06
#define EFI_RESOURCE_NONEXISTENT        0xFFFFFFFFFFFFFFFFULL
#define EFI_RESOURCE_LESS               0xFFFFFFFFFFFFFFFEULL

typedef struct {
  ACPI_HID_DEVICE_PATH          AcpiDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      End;
} EFI_PCI_ROOT_BRIDGE_DEVICE_PATH;

typedef enum {
	ResTypeIo = 0,
	ResTypeMem32,
	ResTypePMem32,
	ResTypeMem64,
	ResTypePMem64,
	ResTypeMax
} PCI_RESOURCE_TYPE;


typedef struct {
	UINT64  Base;
	UINT64  Length;
	UINT64  Alignment;
} PCI_RESOURCE_ALLOC;


typedef struct {
  UINTN                 Signature;
  LIST_ENTRY             Link;
  EFI_HANDLE             Handle;
  UINT64                 RootBridgeAttrib;
  UINT64                 Attributes;
  UINT64                 Supports;
  UINTN                  BusStart;
  UINTN                  BusLength;
  UINT64                 MemAllocAttributes;
  PCI_RESOURCE_ALLOC     ResAlloc[ResTypeMax];
  UINT64                PciBaseAddress64;
  UINT64                PciMemBase64;
  struct LsPcieInfo	*Info;
  struct LsPcie		*Pcie;
  struct PCI_ROOT_BRIDGE_INSTANCE *Next;
  INTN			FirstBusno;
  INTN			LastBusno;
  INTN			CurrentBusno;
  UINTN			*CfgAddr;
  CHAR8			*CfgData;
  INTN			IndirectType;
  struct PciRegion	Regions[MAX_PCI_REGIONS];
  INTN			RegionCnt;
  struct PciConfigTable	*ConfigTable;

  struct PciRegion *PciMem, *PciIo, *PciPrefetch;

  EFI_PCI_ROOT_BRIDGE_DEVICE_PATH         DevicePath;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL         Io;

} PCI_ROOT_BRIDGE_INSTANCE;

#endif
