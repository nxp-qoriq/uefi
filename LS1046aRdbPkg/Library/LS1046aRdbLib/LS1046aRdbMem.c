/** LS1046aRdbMem.c
*
*  RDB memory specific Library for LS1046A SoC.
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

#include <LS1046aRdb.h>

#define MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS          25

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
  VirtualMemoryTable[Index].PhysicalBase = CONFIG_DRAM1_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = CONFIG_DRAM1_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = CONFIG_DRAM1_SIZE;
  VirtualMemoryTable[Index].Attributes   = CacheAttributes;

  // Secure BootROM
  VirtualMemoryTable[++Index].PhysicalBase = CONFIG_SECURE_BOOTROM_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = CONFIG_SECURE_BOOTROM_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = CONFIG_SECURE_BOOTROM_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED;

  // Extended BootROM
  VirtualMemoryTable[++Index].PhysicalBase = CONFIG_EXT_BOOTROM_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = CONFIG_EXT_BOOTROM_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = CONFIG_EXT_BOOTROM_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED;

  // CCSR Space
  VirtualMemoryTable[++Index].PhysicalBase = CONFIG_CCSR_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = CONFIG_CCSR_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = CONFIG_CCSR_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // OCRAM1 Space
  VirtualMemoryTable[++Index].PhysicalBase = CONFIG_OCRAM1_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = CONFIG_OCRAM1_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = CONFIG_OCRAM1_SIZE;
  VirtualMemoryTable[Index].Attributes   = CacheAttributes;
  
  // OCRAM2 Space
  VirtualMemoryTable[++Index].PhysicalBase = CONFIG_OCRAM2_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = CONFIG_OCRAM2_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = CONFIG_OCRAM2_SIZE;
  VirtualMemoryTable[Index].Attributes   = CacheAttributes;

  // FIXME: To Add QSPI

  // IFC region 1
  /*
  * A-009241   : Unaligned write transactions to IFC may result in corruption of data          *
  * Affects    : IFC                                                                           *
  * Description: 16 byte unaligned write from system bus to IFC may result in extra unintended *
  *              writes on external IFC interface that can corrupt data on external flash.     *
  * Impact     : Data corruption on external flash may happen in case of unaligned writes to   *
  *              IFC memory space.                                                             *
  * Workaround: Following are the workarounds:                                                 *
  *             • For write transactions from core, IFC interface memories (including IFC SRAM)*
  *                should be configured as “device type" memory in MMU.                        *
  *             • For write transactions from non-core masters (like system DMA), the address  *
  *                should be 16 byte aligned and the data size should be multiple of 16 bytes. *
  */
  VirtualMemoryTable[++Index].PhysicalBase = CONFIG_IFC_REGION1_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = CONFIG_IFC_REGION1_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = CONFIG_IFC_REGION1_BASE_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE/*CacheAttributes*/;

  // QMAN SWP
  VirtualMemoryTable[++Index].PhysicalBase = CONFIG_QMAN_SWP_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = CONFIG_QMAN_SWP_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = CONFIG_QMAN_SWP_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED;

  // BMAN SWP
  VirtualMemoryTable[++Index].PhysicalBase = CONFIG_BMAN_SWP_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = CONFIG_BMAN_SWP_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = CONFIG_BMAN_SWP_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED;

  // IFC region 2
  VirtualMemoryTable[++Index].PhysicalBase = CONFIG_IFC_REGION2_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = CONFIG_IFC_REGION2_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = CONFIG_IFC_REGION2_BASE_SIZE;
  VirtualMemoryTable[Index].Attributes   = CacheAttributes;

  // QSPI region
  VirtualMemoryTable[++Index].PhysicalBase = QSPI_REGION_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = QSPI_REGION_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = QSPI_REGION_BASE_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED;

  // DRAM2
  VirtualMemoryTable[++Index].PhysicalBase = CONFIG_DRAM2_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = CONFIG_DRAM2_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = CONFIG_DRAM2_SIZE;
  VirtualMemoryTable[Index].Attributes   = CacheAttributes;

  // PCIe1
  VirtualMemoryTable[++Index].PhysicalBase = CONFIG_PCI_EXP1_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = CONFIG_PCI_EXP1_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = CONFIG_PCI_EXP1_BASE_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // PCIe2
  VirtualMemoryTable[++Index].PhysicalBase = CONFIG_PCI_EXP2_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = CONFIG_PCI_EXP2_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = CONFIG_PCI_EXP2_BASE_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // PCIe3
  VirtualMemoryTable[++Index].PhysicalBase = CONFIG_PCI_EXP3_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = CONFIG_PCI_EXP3_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = CONFIG_PCI_EXP3_BASE_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // DRAM3
  VirtualMemoryTable[++Index].PhysicalBase = CONFIG_DRAM3_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = CONFIG_DRAM3_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = CONFIG_DRAM3_SIZE;
  VirtualMemoryTable[Index].Attributes   = CacheAttributes;

  // DCSR
  VirtualMemoryTable[++Index].PhysicalBase = CONFIG_DCSR_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = CONFIG_DCSR_SIZE;
  VirtualMemoryTable[Index].Length       = CONFIG_DCSR_SIZE;
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;
 
  // End of Table
  VirtualMemoryTable[++Index].PhysicalBase = 0;
  VirtualMemoryTable[Index].VirtualBase  = 0;
  VirtualMemoryTable[Index].Length       = 0;
  VirtualMemoryTable[Index].Attributes   = (ARM_MEMORY_REGION_ATTRIBUTES)0;

  ASSERT((Index + 1) <= MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS);

  *VirtualMemoryMap = VirtualMemoryTable;
}
