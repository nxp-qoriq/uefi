/** LS2088aRdbMem.c
*
*  RDB memory specific Library for LS2088A SoC.
*
*  Based on BeagleBoardPkg/Library/BeagleBoardLib/BeagleBoardMem.c
*
*  Copyright (c) 2011, ARM Limited. All rights reserved.
*  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution. The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IoLib.h>

#include <LS2088aRdb.h>

#define MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS          20

/**
  Return the Virtual Memory Map of your platform

  This Virtual Memory Map is used by MemoryInitPei Module to initialize the MMU on your platform.

  @param[out]   VirtualMemoryMap    Array of ARM_MEMORY_REGION_DESCRIPTOR describing a Physical-to-
                                    Virtual Memory mapping. This array must be ended by a zero-filled
                                    entry

**/
VOID
ArmPlatformGetVirtualMemoryMap (
  IN ARM_MEMORY_REGION_DESCRIPTOR** VirtualMemoryMap
  )
{
  ARM_MEMORY_REGION_ATTRIBUTES  CacheAttributes;
  UINTN                         Index = 0;
  ARM_MEMORY_REGION_DESCRIPTOR  *VirtualMemoryTable;

  ASSERT(VirtualMemoryMap != NULL);

  VirtualMemoryTable = (ARM_MEMORY_REGION_DESCRIPTOR*)AllocatePages(EFI_SIZE_TO_PAGES (sizeof(ARM_MEMORY_REGION_DESCRIPTOR) * MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS));
  if (VirtualMemoryTable == NULL) {
    return;
  }

  if (FeaturePcdGet(PcdCacheEnable) == TRUE) {
    CacheAttributes = DDR_ATTRIBUTES_CACHED;
  } else {
    CacheAttributes = DDR_ATTRIBUTES_UNCACHED;
  }

  // DRAM1 (Must be 1st entry)
  VirtualMemoryTable[Index].PhysicalBase = DRAM1_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = DRAM1_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = DRAM1_SIZE;
  VirtualMemoryTable[Index].Attributes   = CacheAttributes;

  // Secure BootROM
  VirtualMemoryTable[++Index].PhysicalBase = SECURE_BOOTROM_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = SECURE_BOOTROM_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = SECURE_BOOTROM_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED;

  // Extended BootROM
  VirtualMemoryTable[++Index].PhysicalBase = EXT_BOOTROM_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = EXT_BOOTROM_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = EXT_BOOTROM_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED;

  // CCSR Space
  VirtualMemoryTable[++Index].PhysicalBase = CCSR_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = CCSR_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = CCSR_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // OCRAM Space
  VirtualMemoryTable[++Index].PhysicalBase = OCRAM_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = OCRAM_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = OCRAM_SIZE;
  VirtualMemoryTable[Index].Attributes   = DDR_ATTRIBUTES_UNCACHED;

  // IFC region 1
  VirtualMemoryTable[++Index].PhysicalBase = IFC_REGION1_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = IFC_REGION1_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = IFC_REGION1_BASE_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // IFC region 2
  VirtualMemoryTable[++Index].PhysicalBase = IFC_REGION2_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = IFC_REGION2_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = IFC_REGION2_BASE_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // QSPI region 1
  VirtualMemoryTable[++Index].PhysicalBase = QSPI_REGION1_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = QSPI_REGION1_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = QSPI_REGION1_BASE_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED;

  // QSPI region 2
  VirtualMemoryTable[++Index].PhysicalBase = QSPI_REGION2_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = QSPI_REGION2_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = QSPI_REGION2_BASE_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED;

# ifdef DPAA2_USE_UEFI_ALLOCATOR_FOR_MC_MEM
  // DRAM2
  VirtualMemoryTable[++Index].PhysicalBase = DRAM2_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = DRAM2_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = DRAM2_SIZE;
  VirtualMemoryTable[Index].Attributes   = CacheAttributes;
# else
  // DRAM2
  VirtualMemoryTable[++Index].PhysicalBase = DRAM2_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = DRAM2_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = DDR_MEM_SIZE;
  VirtualMemoryTable[Index].Attributes   = CacheAttributes;

  // MC private DRAM
# ifdef DPAA2_MC_IN_LOW_MEM
  VirtualMemoryTable[++Index].PhysicalBase = DRAM1_BASE_ADDR - DPAA2_MC_RAM_SIZE;
  VirtualMemoryTable[Index].VirtualBase  = DRAM1_BASE_ADDR - DPAA2_MC_RAM_SIZE;
# else
  VirtualMemoryTable[++Index].PhysicalBase = DRAM2_BASE_ADDR + DDR_MEM_SIZE;
  VirtualMemoryTable[Index].VirtualBase  = DRAM2_BASE_ADDR + DDR_MEM_SIZE;
# endif
  VirtualMemoryTable[Index].Length       = DPAA2_MC_RAM_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;
# endif // DPAA2_USE_UEFI_ALLOCATOR_FOR_MC_MEM

  // PCIe1
  VirtualMemoryTable[++Index].PhysicalBase = PCI_EXP1_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = PCI_EXP1_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = PCI_EXP1_BASE_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // PCIe2
  VirtualMemoryTable[++Index].PhysicalBase = PCI_EXP2_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = PCI_EXP2_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = PCI_EXP2_BASE_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // PCIe3
  VirtualMemoryTable[++Index].PhysicalBase = PCI_EXP3_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = PCI_EXP3_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = PCI_EXP3_BASE_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // PCIe4
  VirtualMemoryTable[++Index].PhysicalBase = PCI_EXP4_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = PCI_EXP4_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = PCI_EXP4_BASE_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // DPAA2 MC Portals region
  VirtualMemoryTable[++Index].PhysicalBase = DPAA2_MC_PORTALS_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = DPAA2_MC_PORTALS_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = DPAA2_MC_PORTALS_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // DPAA2 NI Portals region
  VirtualMemoryTable[++Index].PhysicalBase = DPAA2_NI_PORTALS_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = DPAA2_NI_PORTALS_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = DPAA2_NI_PORTALS_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // DPAA2 QBMAN Portals - cache enabled region
  VirtualMemoryTable[++Index].PhysicalBase = DPAA2_QBMAN_PORTALS_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = DPAA2_QBMAN_PORTALS_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = DPAA2_QBMAN_PORTALS_CACHE_ENABLED_SIZE;
  VirtualMemoryTable[Index].Attributes   = CacheAttributes;

  // DPAA2 QBMAN Portals - cache inhibited region
  VirtualMemoryTable[++Index].PhysicalBase = DPAA2_QBMAN_PORTALS_BASE_ADDR + DPAA2_QBMAN_PORTALS_CACHE_ENABLED_SIZE;
  VirtualMemoryTable[Index].VirtualBase  = DPAA2_QBMAN_PORTALS_BASE_ADDR + DPAA2_QBMAN_PORTALS_CACHE_ENABLED_SIZE;
  VirtualMemoryTable[Index].Length       = DPAA2_QBMAN_PORTALS_SIZE - DPAA2_QBMAN_PORTALS_CACHE_ENABLED_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // End of Table
  VirtualMemoryTable[++Index].PhysicalBase = 0;
  VirtualMemoryTable[Index].VirtualBase  = 0;
  VirtualMemoryTable[Index].Length       = 0;
  VirtualMemoryTable[Index].Attributes   = (ARM_MEMORY_REGION_ATTRIBUTES)0;

  ASSERT((Index + 1) <= MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS);

  *VirtualMemoryMap = VirtualMemoryTable;
}
