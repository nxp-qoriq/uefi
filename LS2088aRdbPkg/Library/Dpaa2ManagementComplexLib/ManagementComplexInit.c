/** ManagementComplexInit.c
  DPAA2 Management Complex initialization

  Copyright (c) 2016, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Common.h>
#include <Library/Dpaa2ManagementComplexLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/BdsLinuxFit.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib/MemLibInternals.h>
#include <Library/TimerLib.h>
#include <Library/ArmLib.h>
#include <libfdt.h>
#include <LS2088aSocLib.h>
#include "ManagementComplex.h"
#include <Library/Dpaa2DebugLib.h>
#include <Library/Dpaa2McIo.h>
#include <Library/Dpaa2McInterfaceLib/fsl_dpmng.h>
#include <Library/Dpaa2McInterfaceLib/fsl_dprc.h>

C_ASSERT(sizeof(UINTN) == sizeof(UINT64));
C_ASSERT(sizeof(EFI_PHYSICAL_ADDRESS) == sizeof(UINT64));
C_ASSERT(sizeof(VOID *) == sizeof(UINT64));
C_ASSERT(sizeof(int) == sizeof(INT32));

/**
 * Global control block for the DPPA2 Management Complex (MC)
 */
DPAA2_MANAGEMENT_COMPLEX gManagementComplex = {
  .McBooted = FALSE,
  .McDplDeployed = FALSE,
  .McCcsrRegs = (DPAA2_MC_CCSR *)DPAA2_MC_CCSR_BASE_ADDR,
  .RootDprcMcIo = {
    .McPortal = (DPAA2_MC_COMMAND *)DPAA2_MC_PORTAL_ADDR(0x0),
    .McSendCmdLock = EFI_INITIALIZE_LOCK_VARIABLE(TPL_HIGH_LEVEL),
  },
};


static VOID
DumpRamWords(const CHAR8 *Title, VOID *Addr)
{
  UINTN I;
  UINT32 *Words = Addr;

  DPAA2_DEBUG_MSG("Dumping beginning of %a (%p):\n\t", Title, Addr);

  for (I = 0; I < 16; I++) {
    DPAA2_DEBUG_MSG_NO_PREFIX("0x%x ", Words[I]);
  }

  DPAA2_DEBUG_MSG_NO_PREFIX("\n");
}


static VOID
DumpMcCcsrRegs(
  DPAA2_MC_CCSR *McCcsrRegs)
{
  DPAA2_DEBUG_MSG("MC CCSR registers at 0x%p:\n"
	 "\tGcr1 0x%x\n"
	 "\tGsr 0x%x\n"
	 "\tSicbalr 0x%x\n"
	 "\tSicbahr 0x%x\n"
	 "\tSicapr 0x%x\n"
	 "\tMcfbalr 0x%x\n"
	 "\tMcfbahr 0x%x\n"
	 "\tMcfapr 0x%x\n"
	 "\tPsr 0x%x\n",
         McCcsrRegs,
	 McCcsrRegs->Gcr1,
	 McCcsrRegs->Gsr,
	 McCcsrRegs->Sicbalr,
	 McCcsrRegs->Sicbahr,
	 McCcsrRegs->Sicapr,
	 McCcsrRegs->Mcfbalr,
	 McCcsrRegs->Mcfbahr,
	 McCcsrRegs->Mcfapr,
	 McCcsrRegs->Psr);
}


/**
 * Allocate MC private memory block from the top of DDR
 */
static EFI_STATUS
Dpaa2McAllocatePrivateMem(DPAA2_MANAGEMENT_COMPLEX *Mc)
{
  VOID *McRamBlock;
  UINT32 McRamSize = PcdGet32(PcdDpaa2McPrivateRamSize);

  ASSERT(Mc->McPrivateMemoryBaseAddr == 0x0);
  ASSERT(McRamSize != 0 && McRamSize % MC_RAM_SIZE_ALIGNMENT == 0);

# ifdef DPAA2_USE_UEFI_ALLOCATOR_FOR_MC_MEM
  UINTN Pages = EFI_SIZE_TO_PAGES(McRamSize);

  McRamBlock = AllocateAlignedRuntimePages(Pages, MC_RAM_BASE_ADDR_ALIGNMENT);
  if (McRamBlock == NULL) {
    DPAA2_ERROR_MSG("Failed to allocate %lu pages for MC private memory block\n", Pages);
    return EFI_OUT_OF_RESOURCES;
  }

  /*
   * NOTE: For some reason we need to zero-out the allocated memory block,
   * otherwise the MC fw may crash.
   */
  (VOID)ZeroMem(McRamBlock, McRamSize);
#else
  ASSERT(McRamSize <= DPAA2_MC_RAM_SIZE);
# ifdef DPAA2_MC_IN_LOW_MEM
  McRamBlock = (VOID *)DRAM1_BASE_ADDR - DPAA2_MC_RAM_SIZE;
# else
  McRamBlock = (VOID *)DRAM2_BASE_ADDR + DDR_MEM_SIZE;
# endif
#endif

  ASSERT((UINT64)McRamBlock % MC_RAM_BASE_ADDR_ALIGNMENT == 0);
  Mc->McPrivateMemoryBaseAddr = (EFI_PHYSICAL_ADDRESS)McRamBlock;
  Mc->McPrivateMemorySize = McRamSize;
  DPAA2_INFO_MSG(
     "Allocated DPAA2 Mangement Complex private memory block (%lu MiB) at 0x%p\n",
     Mc->McPrivateMemorySize / (1024UL * 1024),
     Mc->McPrivateMemoryBaseAddr);

  return EFI_SUCCESS;
}


static VOID
Dpaa2McFreePrivateMem(DPAA2_MANAGEMENT_COMPLEX *Mc)
{
#ifdef DPAA2_USE_UEFI_ALLOCATOR_FOR_MC_MEM
  UINTN Pages = EFI_SIZE_TO_PAGES(Mc->McPrivateMemorySize);
  VOID *McRamBlock = (VOID *)Mc->McPrivateMemoryBaseAddr;

  ASSERT(McRamBlock != NULL);
  FreeAlignedPages(McRamBlock, Pages);
  DPAA2_INFO_MSG(
     "Freed DPAA2 Mangement Complex private memory block at 0x%p\n",
     McRamBlock);
#else
  /*
   * We are statically allocating the MC private memory block, so this
   * function should not be invoked:
   */
  ASSERT(FALSE);
#endif
}


/*
 * Calculates the values to be used to specify the address range
 * for the MC private DRAM block, in the MCFBALR/MCFBAHR registers.
 * It returns the highest 512MiB-aligned address within the given
 * address range, in '*aligned_base_addr', and the number of 256 MiB
 * blocks in it, in 'num_256mb_blocks'.
 */
static EFI_STATUS
CalculateMcPrivateRamParams(DPAA2_MANAGEMENT_COMPLEX *Mc,
			    EFI_PHYSICAL_ADDRESS *AlignedBaseAddr,
			    UINT8 *Num256MbBlocks)
{
  UINT64 Addr;
  UINT16 NumBlocks;
  UINTN McRamSize = Mc->McPrivateMemorySize;

  if (McRamSize % MC_RAM_SIZE_ALIGNMENT != 0) {
    DPAA2_ERROR_MSG("Invalid MC private RAM size (%lu)\n", McRamSize);
    return EFI_INVALID_PARAMETER;
  }

  NumBlocks = McRamSize / MC_RAM_SIZE_ALIGNMENT;
  if (NumBlocks < 1 || NumBlocks > 0xff) {
    DPAA2_ERROR_MSG(
      "Invalid number of 256MiB blocks for MC private RAM (%lu)\n",
      NumBlocks);
    return EFI_INVALID_PARAMETER;
  }

  Addr = (Mc->McPrivateMemoryBaseAddr + McRamSize - 1) &
         MC_RAM_BASE_ADDR_ALIGNMENT_MASK;

  if (Addr < Mc->McPrivateMemoryBaseAddr) {
    DPAA2_ERROR_MSG("Bad start address for MC private RAM (0x%p)\n",
                 Mc->McPrivateMemoryBaseAddr);
    return EFI_INVALID_PARAMETER;
  }

  *AlignedBaseAddr = Addr;
  *Num256MbBlocks = NumBlocks;
  return EFI_SUCCESS;
}


/**
 * Copying an MC image (firmware, DPC or DPL) from NOR flash to DDR
 */
static VOID
McCopyImage(
  const CHAR8 *Title,
  EFI_PHYSICAL_ADDRESS ImageFlashAddr,
  UINT32 ImageSize,
  EFI_PHYSICAL_ADDRESS McRamAddr)
{
  DPAA2_DEBUG_MSG(
     "Copying %a from address 0x%p to address 0x%p (%u bytes) ...\n", Title,
     ImageFlashAddr, McRamAddr, ImageSize);

  /*
   * TODO: This is slow. It needs to be made faster, as copying the 2 MB
   * MC fw image takes more than 10 seconds, mimpacting the overall boot
   * time.
   */
  InternalMemCopyMem((VOID *)McRamAddr, (VOID *)ImageFlashAddr, ImageSize);

# if defined(DPAA2_USE_UEFI_ALLOCATOR_FOR_MC_MEM) && !defined(DPAA2_USE_CLEAN_ALL_DCACHE_LEVELS)
  CleanDcacheRange(McRamAddr, McRamAddr + ImageSize);
# endif
}


/**
   Retrieves the node offset of a given image within a FIT image

   @param[in] FitImageAddr  Address of the FIT image
   @param[in] ImageUnitName Target image name
   @param[out] NodeOffset   Pointer to area where the node offset is
                            to be returned

   @retval EFI_SUCCESS, on success
   @retval error code, on failure
 **/
static EFI_STATUS
FitImageGetNode (
  EFI_PHYSICAL_ADDRESS FitImageAddr,
  CHAR8 *ImageUnitName,
  INT32 *NodeOffset
)
{
  INT32 SubNodeOffset;
  INT32 ImagesNodeOffset;
  VOID *FitImage = (VOID *)FitImageAddr;

  ImagesNodeOffset = fdt_path_offset(FitImage, FIT_IMAGES_PATH);
  if (ImagesNodeOffset < 0) {
    DPAA2_ERROR_MSG("Can't find images parent node '%a' (%a)\n",
                    FIT_IMAGES_PATH, fdt_strerror(ImagesNodeOffset));
    return EFI_UNSUPPORTED;
  }

  SubNodeOffset = fdt_subnode_offset(FitImage, ImagesNodeOffset, ImageUnitName);
  if (SubNodeOffset < 0) {
    DPAA2_ERROR_MSG("Can't get node offset for image unit name: '%a' (%a)\n",
	            ImageUnitName, fdt_strerror(SubNodeOffset));
  }

  *NodeOffset = SubNodeOffset;
  return EFI_SUCCESS;
}


/**
 * Load the MC firmware image from NOR flash to DDR
 */
static EFI_STATUS
McLoadFirmware(DPAA2_MANAGEMENT_COMPLEX *Mc, DPAA2_MC_FW_SOURCE FlashSource)
{
  EFI_STATUS Status;
  EFI_PHYSICAL_ADDRESS RawImageFlashAddr;
  INT32 RawImageSize;
  EFI_PHYSICAL_ADDRESS McFwRamAddr;
  EFI_PHYSICAL_ADDRESS McFwFlashAddr;
  INT32 NodeOffset = 0;

  ASSERT(FlashSource == MC_IMAGES_IN_NOR_FLASH ||
         FlashSource == MC_IMAGES_IN_QSPI_FLASH);

  if (FlashSource == MC_IMAGES_IN_NOR_FLASH) {
    McFwFlashAddr = PcdGet64(PcdDpaa2McFwNorAddr);
  } else {
    McFwFlashAddr = PcdGet64(PcdDpaa2McFwQspiAddr);
  }

  DPAA2_INFO_MSG(
    "Loading MC firmware FIT image from flash address 0x%p ...\n",
    McFwFlashAddr);

  Status = FitCheckHeader(McFwFlashAddr);
  if (EFI_ERROR(Status)) {
    DPAA2_ERROR_MSG(
       "Bad FIT image header (error 0x%x) for MC firmware\n",
       Status);
    return Status;
  }

  Status = FitImageGetNode(McFwFlashAddr, "firmware", &NodeOffset);
  if (EFI_ERROR(Status)) {
    DPAA2_ERROR_MSG("Bad firmware image (missing subimage)\n");
    return Status;
  }

  Status = FitGetNodeData(McFwFlashAddr, NodeOffset, &RawImageFlashAddr,
			  &RawImageSize);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  /*
   * Load the MC FW at the beginning of the MC private DRAM block:
   */
  McFwRamAddr = Mc->McPrivateMemoryBaseAddr;
  McCopyImage("MC Firmware", RawImageFlashAddr, RawImageSize, McFwRamAddr);

  if (gDpaa2DebugFlags & DPAA2_DEBUG_DUMP_VALUES) {
    DumpRamWords("MC firmware", (VOID *)McFwRamAddr);
  }

  return EFI_SUCCESS;
}

/**
   Do a fixup of a given 32-bit property in a node of an FDT image, given the
   node's offset.

   @param[in] FdtImageAddr  Address of the FDT image
   @param[in] NodeOffset    Node's offset
   @param[in] Property      Property name
   @param[in] Value         Property value

   @retval EFI_SUCCESS, on success
   @retval error code, on failure
 **/
static EFI_STATUS
FdtImageFixupPropU32(
  EFI_PHYSICAL_ADDRESS FdtImageAddr,
  INT32 NodeOffset,
  CHAR8* Property,
  UINT32 Value)
{
  INT32 FdtError;
  fdt32_t FdtValue;
  VOID *FdtImage = (VOID *)FdtImageAddr;

  FdtValue = cpu_to_fdt32(Value);
  FdtError = fdt_setprop(FdtImage, NodeOffset, Property, &FdtValue,
                         sizeof(FdtValue));
  if (FdtError != 0) {
    DPAA2_ERROR_MSG(
       "Can't set property '%a' for node offset 0x%x (%a)\n",
       Property, NodeOffset, fdt_strerror(FdtError));
    return EFI_UNSUPPORTED;
  }

  if (gDpaa2DebugFlags & DPAA2_DEBUG_EXTRA_CHECKS) {
    INT32 PropSize;
    const UINT32 *PropValuePtr = fdt_getprop(FdtImage, NodeOffset, Property, &PropSize);

    ASSERT(FdtError == 0);
    ASSERT(PropSize == sizeof(FdtValue));
    ASSERT(*PropValuePtr == FdtValue);
  }

  return EFI_SUCCESS;
}


/**
   Do a fixup of a given string property in a node of an FDT image, given the
   node's offset.

   @param[in] FdtImageAddr  Address of the FDT image
   @param[in] NodeOffset    Node's offset
   @param[in] Property      Property name
   @param[in] Value         Property value

   @retval EFI_SUCCESS, on success
   @retval error code, on failure
 **/
static EFI_STATUS
FdtImageFixupPropString(
  EFI_PHYSICAL_ADDRESS FdtImageAddr,
  INT32 NodeOffset,
  const CHAR8* Property,
  const CHAR8* Value)
{
  INT32 FdtError;
  VOID *FdtImage = (VOID *)FdtImageAddr;

  FdtError = fdt_setprop(FdtImage, NodeOffset, Property, Value,
                         strlen(Value) + 1);
  if (FdtError != 0) {
    DPAA2_ERROR_MSG(
       "Can't set property '%a' for node offset 0x%x (%a)\n",
       Property, NodeOffset, fdt_strerror(FdtError));
    return EFI_UNSUPPORTED;
  }

  if (gDpaa2DebugFlags & DPAA2_DEBUG_EXTRA_CHECKS) {
    INT32 PropSize;
    const CHAR8 *PropValuePtr = fdt_getprop(FdtImage, NodeOffset, Property, &PropSize);

    ASSERT(FdtError == 0);
    ASSERT(PropSize == strlen(Value) + 1);
    ASSERT(memcmp(PropValuePtr, Value, PropSize) == 0);
  }

  return EFI_SUCCESS;
}


/**
 * Fix up MC log level in the DPC blob (FDT)
 */
static EFI_STATUS
McFixupDpcMcLogLevel(DPAA2_MANAGEMENT_COMPLEX *Mc,
                     EFI_PHYSICAL_ADDRESS DpcRamAddr,
                     DPAA2_MC_LOG_LEVEL McLogLevel)
{
  static const CHAR8 *const McLogLevelStrings[] = {
    [MC_LOG_LEVEL_DEBUG] =    "LOG_LEVEL_DEBUG",
    [MC_LOG_LEVEL_INFO] =     "LOG_LEVEL_INFO",
    [MC_LOG_LEVEL_WARNING] =  "LOG_LEVEL_WARNING",
    [MC_LOG_LEVEL_ERROR] =    "LOG_LEVEL_ERROR",
    [MC_LOG_LEVEL_CRITICAL] = "LOG_LEVEL_CRITICAL",
    [MC_LOG_LEVEL_ASSERT] =   "LOG_LEVEL_ASSERT",
  };

  EFI_STATUS Status;
  INT32 NodeOffset;
  VOID *DpcBlob = (VOID *)DpcRamAddr;

  ASSERT(McLogLevel >= MC_LOG_LEVEL_DEBUG &&
         McLogLevel <= MC_LOG_LEVEL_ASSERT);

  NodeOffset = fdt_path_offset(DpcBlob, "/mc_general/log");
  if (NodeOffset < 0) {
    DPAA2_ERROR_MSG("DPC /mc_general/log node is missing\n");
    return EFI_INVALID_PARAMETER;
  }

  Status = FdtImageFixupPropString(DpcRamAddr,
                                   NodeOffset,
                                   "mode",
                                   "LOG_MODE_ON");
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = FdtImageFixupPropString(DpcRamAddr,
                                   NodeOffset,
                                   "level",
                                   McLogLevelStrings[McLogLevel]);
  if (EFI_ERROR(Status)) {
      return Status;
  }

  if (gDpaa2DebugFlags & DPAA2_DEBUG_EXTRA_CHECKS) {
    INT32 PropSize;
    const CHAR8 *McLogModeStr;
    const CHAR8 *McLogLevelStr;

    McLogModeStr = fdt_getprop(DpcBlob, NodeOffset, "mode", &PropSize);
    ASSERT(McLogModeStr != NULL && PropSize > 1);
    DPAA2_DEBUG_MSG("MC log mode fixed up in DPC to '%a'\n", McLogModeStr);

    McLogLevelStr = fdt_getprop(DpcBlob, NodeOffset, "level", &PropSize);
    ASSERT(McLogLevelStr != NULL && PropSize > 1);
    DPAA2_DEBUG_MSG("MC log level fixed up in DPC to '%a'\n", McLogLevelStr);
  }

  return EFI_SUCCESS;
}


/**
 * Fix up DPC blob (FDT)
 */
static EFI_STATUS
McFixupDpc(DPAA2_MANAGEMENT_COMPLEX *Mc,
           EFI_PHYSICAL_ADDRESS DpcRamAddr)
{
  INT32 NodeOffset;
  INT32 FdtError;
  EFI_STATUS Status;
  VOID *DpcBlob = (VOID *)DpcRamAddr;
  UINT32 Dpaa2StreamIdStart = PcdGet32(PcdDpaa2StreamIdStart);
  UINT32 Dpaa2StreamIdEnd = PcdGet32(PcdDpaa2StreamIdEnd);
  DPAA2_MC_LOG_LEVEL McLogLevel = (DPAA2_MC_LOG_LEVEL)PcdGet8(PcdDpaa2McLogLevel);

  ASSERT(Dpaa2StreamIdEnd > Dpaa2StreamIdStart);
  DPAA2_DEBUG_MSG("Fixing up DPC ...\n");

  /*
   * Delete any existing ICID pools
   */

  NodeOffset = fdt_path_offset(DpcBlob, "/resources/icid_pools");
  if (NodeOffset < 0) {
    DPAA2_ERROR_MSG("DPC /resources/icid_pools node is missing\n");
    return EFI_INVALID_PARAMETER;
  }

  FdtError = fdt_del_node(DpcBlob, NodeOffset);
  if (FdtError < 0) {
    DPAA2_WARN_MSG("Could not delete ICID pool (error %a)\n",
                fdt_strerror(FdtError));
  }

  /*
   * Add a new pool
   */

  NodeOffset = fdt_path_offset(DpcBlob, "/resources");
  if (NodeOffset < 0) {
    DPAA2_ERROR_MSG("DPC /resources node is missing\n");
    return EFI_INVALID_PARAMETER;
  }

  NodeOffset = fdt_add_subnode(DpcBlob, NodeOffset, "icid_pools");
  if (NodeOffset < 0) {
    DPAA2_ERROR_MSG("Can't add node icid_pools\n");
    return EFI_INVALID_PARAMETER;
  }

  NodeOffset = fdt_add_subnode(DpcBlob, NodeOffset, "icid_pool@0");
  if (NodeOffset < 0) {
    DPAA2_ERROR_MSG("Can't add node icid_pool@0\n");
    return EFI_INVALID_PARAMETER;
  }

  Status = FdtImageFixupPropU32(DpcRamAddr,
                                NodeOffset,
			        "base_icid",
                                Dpaa2StreamIdStart);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = FdtImageFixupPropU32(DpcRamAddr,
                                NodeOffset,
			        "num",
			        Dpaa2StreamIdEnd - Dpaa2StreamIdStart + 1);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (gDpaa2DebugFlags & DPAA2_DEBUG_EXTRA_CHECKS) {
    INT32 PropSize;
    const UINT32 *BaseIcid;
    const UINT32 *Num;

    BaseIcid = fdt_getprop(DpcBlob, NodeOffset, "base_icid", &PropSize);
    ASSERT(BaseIcid != NULL && PropSize == sizeof(UINT32));
    DPAA2_DEBUG_MSG("MC base ICID fixed up in DPC to 0x%x\n", fdt32_to_cpu(*BaseIcid));

    Num = fdt_getprop(DpcBlob, NodeOffset, "num", &PropSize);
    ASSERT(Num != NULL && PropSize == sizeof(UINT32));
    DPAA2_DEBUG_MSG("MC num ICIDs fixed up in DPC to %u\n", fdt32_to_cpu(*Num));
  }

  if (McLogLevel != MC_LOG_LEVEL_DPC_DEFAULT) {
    Status = McFixupDpcMcLogLevel(Mc, DpcRamAddr, McLogLevel);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

# ifdef DPAA2_USE_UEFI_ALLOCATOR_FOR_MC_MEM
  CleanDcacheRange(DpcRamAddr, DpcRamAddr + fdt_totalsize(DpcBlob));
# endif

  return EFI_SUCCESS;
}


/**
 * Load the MC DPC blob to the MC private DRAM block:
 */
static EFI_STATUS
McLoadDpc(DPAA2_MANAGEMENT_COMPLEX *Mc, DPAA2_MC_FW_SOURCE FlashSource)
{
  EFI_STATUS Status;
  UINT32 DpcMcDramOffset = PcdGet32(PcdDpaa2McDpcMcDramOffset);
  UINT32 DpcMaxLen = PcdGet32(PcdDpaa2McDpcMaxLen);
  UINT32 DpcSize;
  EFI_PHYSICAL_ADDRESS DpcRamAddr;
  EFI_PHYSICAL_ADDRESS DpcFlashAddr;

  ASSERT(DpcMcDramOffset < Mc->McPrivateMemorySize);
  ASSERT(DpcMaxLen != 0);
  ASSERT(DpcMcDramOffset + DpcMaxLen <= Mc->McPrivateMemorySize);
  ASSERT(FlashSource == MC_IMAGES_IN_NOR_FLASH ||
         FlashSource == MC_IMAGES_IN_QSPI_FLASH);

  if (FlashSource == MC_IMAGES_IN_NOR_FLASH) {
    DpcFlashAddr = PcdGet64(PcdDpaa2McDpcNorAddr);
  } else {
    DpcFlashAddr = PcdGet64(PcdDpaa2McDpcQspiAddr);
  }

  DPAA2_INFO_MSG("Loading MC DPC FDT blob from flash address 0x%p ...\n",
              DpcFlashAddr);

  if (DpcMcDramOffset & 0x3) {
    DPAA2_ERROR_MSG("Invalid MC DRAM DPC offset: 0x%x\n", DpcMcDramOffset);
    return EFI_INVALID_PARAMETER;
  }

  if(fdt_check_header((VOID*)DpcFlashAddr)) {
    DPAA2_ERROR_MSG("Bad FDT header for DPC blob\n");
    return EFI_UNSUPPORTED;
  }

  DpcSize = fdt_totalsize(DpcFlashAddr);
  if (DpcSize > DpcMaxLen) {
    DPAA2_ERROR_MSG("Bad DPC blob image (too large: %u)\n", DpcSize);
    return EFI_INVALID_PARAMETER;
  }

  DpcRamAddr = Mc->McPrivateMemoryBaseAddr + DpcMcDramOffset;
  McCopyImage("MC DPC blob", DpcFlashAddr, DpcSize, DpcRamAddr);

  Status = McFixupDpc(Mc, DpcRamAddr);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (gDpaa2DebugFlags & DPAA2_DEBUG_DUMP_VALUES) {
    DumpRamWords("DPC", (VOID *)DpcRamAddr);
  }

  return EFI_SUCCESS;
}


static VOID
DumpMcLogFragment(
  DPAA2_MANAGEMENT_COMPLEX *Mc,
  UINT32 McLogPrintStartOffset,
  UINT32 McLogPrintEndOffset)
{
  const CHAR8 *McLogPrintStart;
  const CHAR8 *McLogPrintEnd;
  const CHAR8 *McLogPrintCursor;
  DPAA2_MC_LOG_HEADER *McLog = Mc->McLog;
  BOOLEAN OldMcLogWraparoundFlag = Mc->McLogLastWraparoundFlag;
  UINT32 NewMcLogCursorOffset = McLog->BufStart + (McLog->LastByte & ~BIT(31));
  BOOLEAN NewMcLogWraparoundFlag = ((McLog->LastByte & BIT(31)) != 0);

# ifdef DPAA2_USE_UEFI_ALLOCATOR_FOR_MC_MEM
  /*
   * Purge Cache to ensure that we get the latest log data written by the MC:
   */
  CleanDcacheRange((UINT64)McLog, (UINT64)(McLog + sizeof(*McLog)));
  CleanDcacheRange((UINT64)Mc->McLogBufferStartPtr, (UINT64)Mc->McLogBufferEnd);
# endif

  if ((McLog->MagicWord & MC_LOG_MAGIC_WORD_PREFIX_MASK) !=
      MC_LOG_MAGIC_WORD_PREFIX_MASK) {
    DPAA2_ERROR_MSG("Invalid MC log header (bad magic word: 0x%x)\n",
                McLog->MagicWord);
    return;
  }

  DPAA2_DEBUG_MSG("*** Begining of MC log fragment dump ***\n");
  ASSERT(McLogPrintStartOffset >= McLog->BufStart &&
         McLogPrintStartOffset < McLog->BufStart + McLog->BufLength);
  ASSERT(McLogPrintEndOffset >= McLog->BufStart &&
         McLogPrintEndOffset < McLog->BufStart + McLog->BufLength);

  if (McLogPrintStartOffset == McLogPrintEndOffset &&
      OldMcLogWraparoundFlag == NewMcLogWraparoundFlag) {
    return;
  }

  Mc->McLogLastWraparoundFlag = NewMcLogWraparoundFlag;

  if (OldMcLogWraparoundFlag != NewMcLogWraparoundFlag) {
    /*
     * A new log buffer wraparound happened.
     */
    McLogPrintStart = Mc->McLogBufferStartPtr;
  } else {
    McLogPrintStart = (CHAR8 *)(Mc->McPrivateMemoryBaseAddr +
                                McLogPrintStartOffset);
  }

  McLogPrintEnd = (CHAR8 *)(Mc->McPrivateMemoryBaseAddr +
                            McLogPrintEndOffset);

  if (NewMcLogCursorOffset != McLogPrintEndOffset) {
    /*
     * If new messages were written to the log recently,
     * include them in the tail print out:
     */
    McLogPrintEnd = (CHAR8 *)(Mc->McPrivateMemoryBaseAddr +
                              NewMcLogCursorOffset);
  }

  if (McLogPrintEnd == McLogPrintStart) {
    DPAA2_INFO_MSG_NO_PREFIX("%c", *McLogPrintStart);
    McLogPrintCursor = McLogPrintStart + 1;
    if (McLogPrintCursor == Mc->McLogBufferEnd) {
      McLogPrintCursor = Mc->McLogBufferStartPtr;
    }
  } else {
    McLogPrintCursor = McLogPrintStart;
  }

  do {
    if (*McLogPrintCursor == '\n') {
      /*
       * We need this special case, because of a shortcoming in underlying
       * UEFI printing code.
       */
      DPAA2_INFO_MSG_NO_PREFIX("\n");
    } else {
      DPAA2_INFO_MSG_NO_PREFIX("%c", *McLogPrintCursor);
    }

    McLogPrintCursor ++;
    if (McLogPrintCursor == Mc->McLogBufferEnd) {
      McLogPrintCursor = Mc->McLogBufferStartPtr;
    }
  } while (McLogPrintCursor != McLogPrintEnd);

  DPAA2_DEBUG_MSG("*** End of MC log fragment dump ***\n");
}


/**
 * Dump a given number of lines from the tail of the MC log
 */
VOID DumpMcLogTail(DPAA2_MANAGEMENT_COMPLEX *Mc, UINTN NumTailLines)
{
  DPAA2_MC_LOG_HEADER *const McLog = Mc->McLog;
  const UINT32 McLogCursorOffset = McLog->BufStart + (McLog->LastByte & ~BIT(31));
  const BOOLEAN McLogWraparoundFlag = ((McLog->LastByte & BIT(31)) != 0);
  const CHAR8 *const McLogPrintEnd = (CHAR8 *)(Mc->McPrivateMemoryBaseAddr +
                                               McLogCursorOffset);
  const CHAR8 *McLogPrintCursor = McLogPrintEnd - 1;
  const CHAR8 *OldMcLogPrintCursor = NULL;
  UINTN TextLinesCount = 0;
  UINT32 McLogPrintStartOffset;
  UINT32 McLogPrintEndOffset;

  ASSERT(NumTailLines > 0);
  ASSERT(McLogPrintEnd >= Mc->McLogBufferStartPtr);
  ASSERT(McLogPrintEnd <= Mc->McLogBufferEnd);
  if (McLogPrintEnd == Mc->McLogBufferStartPtr) {
    if (McLogWraparoundFlag == Mc->McLogLastWraparoundFlag)  {
      return;
    }

    McLogPrintCursor = Mc->McLogBufferEnd - 1;
  } else {
    McLogPrintCursor = McLogPrintEnd - 1;
  }

  /*
   * Calculate Start offset, traversing the MC log buffer backwards,
   * counting text lines:
   */
  ASSERT(McLogPrintCursor != McLogPrintEnd);
  do {
    ASSERT(McLogPrintCursor >= Mc->McLogBufferStartPtr);
    ASSERT(McLogPrintCursor < Mc->McLogBufferEnd);
    if (*McLogPrintCursor == '\n') {
      TextLinesCount ++;
      if (TextLinesCount == NumTailLines + 1) {
        /*
         * We need to count one more line, to include the first line of
         * wanted the log tail
         */
        break;
      }
    }

    OldMcLogPrintCursor = McLogPrintCursor;
    if (McLogPrintCursor == Mc->McLogBufferStartPtr)  {
      if (McLogWraparoundFlag == Mc->McLogLastWraparoundFlag) {
        break;
      }

      McLogPrintCursor = Mc->McLogBufferEnd - 1;
    } else {
      McLogPrintCursor --;
    }
  } while (McLogPrintCursor != McLogPrintEnd);

  /*
   * OldMcLogPrintCursor points to the beginning of the first line to print:
   */
  ASSERT(OldMcLogPrintCursor >= Mc->McLogBufferStartPtr);
  ASSERT(OldMcLogPrintCursor < Mc->McLogBufferEnd);
  McLogPrintStartOffset = McLog->BufStart +
                         (OldMcLogPrintCursor - Mc->McLogBufferStartPtr);
  McLogPrintEndOffset = McLog->BufStart +
                        (McLogPrintEnd - Mc->McLogBufferStartPtr);

  DPAA2_DEBUG_MSG("Dumping up to last %u lines of MC log at address 0x%p:\n",
               NumTailLines, Mc->McLog);
  DumpMcLogFragment(Mc, McLogPrintStartOffset, McLogPrintEndOffset);
}


static EFI_STATUS
WaitForMc(
  DPAA2_MANAGEMENT_COMPLEX *Mc,
  BOOLEAN BootingMc,
  UINT32 *FinalRegGsr)
{
  UINT32 RegGsr;
  UINT32 McFwBootStatus;
  UINT32 TimeoutMs = PcdGet32(PcdDpaa2McBootTimeoutMs);
  DPAA2_MC_CCSR *McCcsrRegs = Mc->McCcsrRegs;

  ASSERT(TimeoutMs > 0);
  ArmDataMemoryBarrier();
  for ( ; ; ) {
    MicroSecondDelay(1000);
    RegGsr = MmioRead32((UINTN)&McCcsrRegs->Gsr);
    McFwBootStatus = (RegGsr & GSR_FS_MASK);
    if (McFwBootStatus & 0x1) {
      break;
    }

    TimeoutMs--;
    if (TimeoutMs == 0) {
      break;
    }
  }

  if (TimeoutMs == 0) {
    DPAA2_ERROR_MSG("Timeout booting MC\n");
    return EFI_TIMEOUT;
  }

  if (McFwBootStatus != 0x1) {
    DPAA2_WARN_MSG("Firmware returned an error (GSR: 0x%x)\n", RegGsr);
  } else {
    DPAA2_INFO_MSG_NO_PREFIX("SUCCESS\n");
  }

  *FinalRegGsr = RegGsr;
  return EFI_SUCCESS;
}


/**
 * Initialize MC log state variables
 */
static VOID
InitMcLogVars(DPAA2_MANAGEMENT_COMPLEX *Mc)
{
  UINT32 McLogMcDramOffset;
  DPAA2_MC_LOG_HEADER *McLog;

  ASSERT(Mc->McBooted);

  /*
   * Get MC log offset relative to the MC private memory:
   */
  McLogMcDramOffset = PcdGet32(PcdDpaa2McLogMcDramOffset);
  ASSERT(McLogMcDramOffset < Mc->McPrivateMemorySize);

  Mc->McLog = (DPAA2_MC_LOG_HEADER *)(Mc->McPrivateMemoryBaseAddr +
                                      McLogMcDramOffset);
  /*
   * Purge Cache to ensure that we get the latest log data written by the MC:
   */
  McLog = Mc->McLog;
# ifdef DPAA2_USE_UEFI_ALLOCATOR_FOR_MC_MEM
  CleanDcacheRange((UINT64)McLog, (UINT64)(McLog + sizeof(*McLog)));
# endif

  /*
   * Check the MC header (which is initialized by the MC firmware when it
   * is booted:
   */
  if ((McLog->MagicWord & MC_LOG_MAGIC_WORD_PREFIX_MASK) !=
      MC_LOG_MAGIC_WORD_PREFIX_MASK) {
    DPAA2_ERROR_MSG("Invalid MC log header (bad magic word: 0x%x)\n",
                McLog->MagicWord);
    return;
  }

  ASSERT(McLog->BufStart >= McLogMcDramOffset + sizeof(DPAA2_MC_LOG_HEADER));
  ASSERT(McLog->BufLength != 0);
  ASSERT((McLog->LastByte & ~BIT(31)) < McLog->BufLength);

  Mc->McLogLastWraparoundFlag = ((McLog->LastByte & BIT(31)) != 0);

  /*
   * Get the MC's log header immutable fields (which are initialized
   * during MC booting):
   */
  Mc->McLogBufferStartPtr = (CHAR8 *)(Mc->McPrivateMemoryBaseAddr +
                                      McLog->BufStart);
  Mc->McLogBufferEnd = Mc->McLogBufferStartPtr + McLog->BufLength;
  ASSERT(Mc->McLogBufferEnd > Mc->McLogBufferStartPtr); /* to catch overflow */
  DPAA2_DEBUG_MSG("MC log buffer at address 0x%p, size %u bytes\n",
               Mc->McLogBufferStartPtr, McLog->BufLength);
}


/**
   Initializes the DPAA2 Management Complex (MC) module.

   It loads the MC firmware and boots the MC module.

   @param[in] None

   @retval EFI_SUCCESS, on success
   @retval error code, on failure
 **/
EFI_STATUS
Dpaa2McInit(VOID)
{
  EFI_STATUS Status = 0;
  INT32 McFlibError;
  DPAA2_MANAGEMENT_COMPLEX *Mc = &gManagementComplex;
  DPAA2_MC_CCSR *McCcsrRegs = Mc->McCcsrRegs;
  DPAA2_MC_IO *RootDprcMcIo = &Mc->RootDprcMcIo;
  UINT32 RegGsr = 0;
  UINT32 RegMcfbalr;
  struct mc_version McVerInfo;
  UINT64 McRamAlignedBaseAddr = 0;
  UINT8 McRamNum256mbBlocks = 0;
  INT32 ContainerId;
  DPAA2_MC_FW_SOURCE McFwSrc = PcdGet8(PcdDpaa2McFwSrc);
  BOOLEAN McCoreReleased = FALSE;

  if (McFwSrc != MC_IMAGES_IN_NOR_FLASH && McFwSrc != MC_IMAGES_IN_QSPI_FLASH) {
    DPAA2_ERROR_MSG("Storage media '%d' for MC firmware images not supported\n",
                 McFwSrc);
    return EFI_INVALID_PARAMETER;
  }

  Status = Dpaa2McAllocatePrivateMem(Mc);
  if (EFI_ERROR(Status)) {
    goto Out;
  }

  Status = CalculateMcPrivateRamParams(Mc, &McRamAlignedBaseAddr,
                                       &McRamNum256mbBlocks);
  if (EFI_ERROR(Status)) {
    goto Out;
  }

  /*
   * Management Complex cores should be held at reset out of POR.
   * Make sure we held at reset the MC cores again by setting GCR1 to 0.
   * After we load the MC firmware, we will start the MC by deasserting
   * the reset bit for its core 0, and by deasserting the reset bits for
   * Command Portal Managers.
   */
  MmioWrite32((UINTN)&McCcsrRegs->Gcr1, 0);
  ArmDataMemoryBarrier();

  /*
   * Load MC firmware to DRAM:
   */
  Status = McLoadFirmware(Mc, McFwSrc);
  if (EFI_ERROR(Status)) {
    goto Out;
  }

  /*
   * Load MC DPC to DRAM:
   */
  Status = McLoadDpc(Mc, McFwSrc);
  if (EFI_ERROR(Status)) {
    goto Out;
  }

  /*
   * Tell MC what is the address range of the DRAM block assigned to it:
   */
  RegMcfbalr = (UINT32)McRamAlignedBaseAddr | (McRamNum256mbBlocks - 1);
  MmioWrite32((UINTN)&McCcsrRegs->Mcfbalr, RegMcfbalr);
  MmioWrite32((UINTN)&McCcsrRegs->Mcfbahr, (UINT32)(McRamAlignedBaseAddr >> 32));
  MmioWrite32((UINTN)&McCcsrRegs->Mcfapr, BYPASS_AMQ_MASK);

  /*
   * Tell the MC that we want delayed DPL deployment.
   */
  MmioWrite32((UINTN)&McCcsrRegs->Gsr, 0xDD00);

  if (gDpaa2DebugFlags & DPAA2_DEBUG_DUMP_VALUES) {
    DumpMcCcsrRegs(McCcsrRegs);
  }

# if defined(DPAA2_USE_UEFI_ALLOCATOR_FOR_MC_MEM) && defined(DPAA2_USE_CLEAN_ALL_DCACHE_LEVELS)
  CleanAllDcacheLevels();
# endif

  DPAA2_INFO_MSG("Booting Management Complex ... ");

  /*
   * Deassert reset and release MC core 0 to run
   */
  MmioWrite32((UINTN)&McCcsrRegs->Gcr1, GCR1_P1_DE_RST | GCR1_M_ALL_DE_RST);
  McCoreReleased = TRUE;
  /*
   * Wait for MC to finish booting:
   *
   * TODO: This could be removed from here, and use this time to do
   * something more productive in the meantime than just polling the MC
   */
  Status = WaitForMc(Mc, TRUE, &RegGsr);
  if (EFI_ERROR(Status)) {
    goto Out;
  }

  Mc->McBooted = TRUE;
  InitMcLogVars(Mc);

  if (gDpaa2DebugFlags & DPAA2_DEBUG_DUMP_MC_LOG_FRAGMENT) {
    DumpMcLogTail(Mc, 100);
  }

  /*
   * Check that the MC firmware is responding portal commands:
   */
  DPAA2_DEBUG_MSG(
    "Checking access to MC portal of root DPRC (portal physical addr 0x%p)\n",
    RootDprcMcIo->McPortal);

  McFlibError = mc_get_version(RootDprcMcIo, MC_CMD_NO_FLAGS, &McVerInfo);
  if (McFlibError != 0) {
    DPAA2_ERROR_MSG("Firmware version check failed (error %d)\n", McFlibError);
    Status = EFI_DEVICE_ERROR;
    goto Out;
  }

  if (MC_VERSION(McVerInfo.major, McVerInfo.minor) < MC_VERSION(MC_VER_MAJOR, MC_VER_MINOR)) {
    DPAA2_ERROR_MSG("Firmware version %d.%d not supported. Need >=%d.%d\n",
                    McVerInfo.major, McVerInfo.minor, MC_VER_MAJOR, MC_VER_MINOR);
    Status = EFI_DEVICE_ERROR;
    goto Out;
  }

  DPAA2_INFO_MSG(
    "Management Complex booted (version: %d.%d.%d, boot status: 0x%x)\n",
    McVerInfo.major, McVerInfo.minor, McVerInfo.revision,
    RegGsr & GSR_FS_MASK);

  /*
   * Open root container:
   */

  McFlibError = dprc_get_container_id(RootDprcMcIo, MC_CMD_NO_FLAGS, &ContainerId);
  if (McFlibError != 0) {
    DPAA2_ERROR_MSG("dprc_get_container_id() failed for root DPRC (error %d)\n",
                    McFlibError);
    Status = EFI_DEVICE_ERROR;
    goto Out;
  }

  McFlibError = dprc_open(RootDprcMcIo, MC_CMD_NO_FLAGS, ContainerId,
                          &Mc->RootDprcHandle);
  if (McFlibError != 0) {
    DPAA2_ERROR_MSG("dprc_open() failed for root DPRC (error %d)\n",
                    McFlibError);
    Status = EFI_DEVICE_ERROR;
    goto Out;
  }

  ASSERT(Mc->RootDprcHandle != 0);
  Status = EFI_SUCCESS;

Out:
  if (EFI_ERROR(Status)) {
    if (McCoreReleased) {
      /*
       * Make sure we hold at reset the MC cores again by setting GCR1 to 0.
       */
      MmioWrite32((UINTN)&McCcsrRegs->Gcr1, 0);
    }

    if (Mc->McPrivateMemoryBaseAddr != 0x0) {
      Dpaa2McFreePrivateMem(Mc);
    }
  }

  Mc->McBootStatus = Status;
  return Status;
}


/**
   Cleanup MC state before booting the OS

   @param[in] None

 **/
VOID
Dpaa2McExit(VOID)
{
  INT32 McFlibError;
  DPAA2_MANAGEMENT_COMPLEX *Mc = &gManagementComplex;
  DPAA2_MC_IO *RootDprcMcIo = &Mc->RootDprcMcIo;

  /*
   * Close Root DPRC:
   */
  McFlibError = dprc_close(RootDprcMcIo, MC_CMD_NO_FLAGS, Mc->RootDprcHandle);
  if (McFlibError != 0) {
    DPAA2_ERROR_MSG("dprc_close() failed for root DPRC (error %d)\n",
                    McFlibError);
  }
}


/**
 * Load the MC DPL blob to the MC private DRAM block:
 */
static EFI_STATUS
McLoadDpl(DPAA2_MANAGEMENT_COMPLEX *Mc, DPAA2_MC_FW_SOURCE FlashSource)
{
  UINT32 DplMcDramOffset = PcdGet32(PcdDpaa2McDplMcDramOffset);
  UINT32 DplMaxLen = PcdGet32(PcdDpaa2McDplMaxLen);
  UINT32 DplSize;
  EFI_PHYSICAL_ADDRESS DplRamAddr;
  EFI_PHYSICAL_ADDRESS DplFlashAddr;

  ASSERT(DplMcDramOffset < Mc->McPrivateMemorySize);
  ASSERT(DplMaxLen != 0);
  ASSERT(DplMcDramOffset + DplMaxLen <= Mc->McPrivateMemorySize);
  ASSERT(FlashSource == MC_IMAGES_IN_NOR_FLASH ||
         FlashSource == MC_IMAGES_IN_QSPI_FLASH);

  if (FlashSource == MC_IMAGES_IN_NOR_FLASH) {
    DplFlashAddr = PcdGet64(PcdDpaa2McDplNorAddr);
  } else {
    DplFlashAddr = PcdGet64(PcdDpaa2McDplQspiAddr);
  }

  DPAA2_INFO_MSG("Loading MC DPL FDT blob from flash address 0x%p ...\n",
              DplFlashAddr);

  if (DplMcDramOffset & 0x3) {
    DPAA2_ERROR_MSG("Invalid MC DRAM DPL offset: 0x%x\n", DplMcDramOffset);
    return EFI_INVALID_PARAMETER;
  }

  if(fdt_check_header((VOID*)DplFlashAddr)) {
    DPAA2_ERROR_MSG("Bad FDT header for DPL blob\n");
    return EFI_UNSUPPORTED;
  }

  DplSize = fdt_totalsize(DplFlashAddr);
  if (DplSize > DplMaxLen) {
    DPAA2_ERROR_MSG("Bad DPL blob image (too large: %u)\n", DplSize);
    return EFI_INVALID_PARAMETER;
  }

  DplRamAddr = Mc->McPrivateMemoryBaseAddr + DplMcDramOffset;
  McCopyImage("MC DPL blob", DplFlashAddr, DplSize, DplRamAddr);

  if (gDpaa2DebugFlags & DPAA2_DEBUG_DUMP_VALUES) {
    DumpRamWords("DPL", (VOID *)DplRamAddr);
  }

  return EFI_SUCCESS;
}


static
VOID
DumpRootDprc(VOID)
{
  INT32 McFlibError;
  INT32 ContainerId;
  UINT16 RootDprcHandle;
  DPAA2_MANAGEMENT_COMPLEX *Mc = &gManagementComplex;
  DPAA2_MC_IO *RootDprcMcIo = &Mc->RootDprcMcIo;
  INT32 NumChildObjects;
  INT32 I;
  struct dprc_attributes DprcAttr;

  McFlibError = dprc_get_container_id(RootDprcMcIo, MC_CMD_NO_FLAGS, &ContainerId);
  if (McFlibError != 0) {
    DPAA2_ERROR_MSG("dprc_get_container_id() failed for root DPRC (error %d)\n",
                    McFlibError);
    return;
  }

  DPAA2_INFO_MSG("Opening dprc.%d ...\n", ContainerId);
  McFlibError = dprc_open(RootDprcMcIo, MC_CMD_NO_FLAGS, ContainerId,
                          &RootDprcHandle);
  if (McFlibError != 0) {
    DPAA2_ERROR_MSG("dprc_open() failed for root DPRC (error %d)\n",
                    McFlibError);
    return;
  }

  ASSERT(RootDprcHandle != 0);

  McFlibError = dprc_get_attributes(RootDprcMcIo, 0, RootDprcHandle, &DprcAttr);
  if (McFlibError != 0) {
    DPAA2_ERROR_MSG("dprc_get_attributes() failed for root DPRC (error %d)\n",
                    McFlibError);
    goto CommonExit;
  }

  ASSERT(ContainerId == DprcAttr.container_id);
  DPAA2_INFO_MSG("DPRC attributes:\n"
                 "\tcontainer id: %d\n"
		"\ticid: %u\n"
		"\tportal id: %d\n"
		"\tversion: %u.%u\n"
		"\tdprc options: 0x%lx\n",
		DprcAttr.container_id,
		DprcAttr.icid,
		DprcAttr.portal_id,
		DprcAttr.version.major,
		DprcAttr.version.minor,
		DprcAttr.options);

  McFlibError = dprc_get_obj_count(RootDprcMcIo,
				   0,
				   RootDprcHandle,
				   &NumChildObjects);
  if (McFlibError != 0) {
    DPAA2_ERROR_MSG("dprc_get_obj_count() failed for root DPRC (error %d)\n",
                    McFlibError);
    goto CommonExit;
  }

  for (I = 0; I < NumChildObjects; I++) {
    struct dprc_obj_desc ObjDesc;

    ZeroMem(&ObjDesc, sizeof ObjDesc);
    McFlibError = dprc_get_obj(RootDprcMcIo,
                               0,
                               RootDprcHandle,
                               I, &ObjDesc);

    if (McFlibError != 0) {
      DPAA2_ERROR_MSG("dprc_get_obj() failed for root DPRC (error %d)\n",
                      McFlibError);
      goto CommonExit;
    }

    DPAA2_INFO_MSG("*** Discovered object: type %a, id %d\n",
		   ObjDesc.type, ObjDesc.id);
  }

CommonExit:
  DPAA2_INFO_MSG("Closing dprc.%d ...\n", ContainerId);
  McFlibError = dprc_close(RootDprcMcIo, MC_CMD_NO_FLAGS, RootDprcHandle);
  if (McFlibError != 0) {
    DPAA2_ERROR_MSG("dprc_close() failed for root DPRC (error %d)\n",
                    McFlibError);
  }
}


/**
   Deploy the MC data path layout (DPL)

   @param[in] None

   @retval EFI_SUCCESS, on success
   @retval error code, on failure
 **/
EFI_STATUS
Dpaa2McDeployDpl(VOID)
{
  EFI_STATUS Status;
  UINT32 RegGsr;
  DPAA2_MANAGEMENT_COMPLEX *Mc = &gManagementComplex;
  DPAA2_MC_CCSR *McCcsrRegs = Mc->McCcsrRegs;
  DPAA2_MC_FW_SOURCE McFwSrc = PcdGet8(PcdDpaa2McFwSrc);

  ASSERT(Mc->McBooted);
  ASSERT(!Mc->McDplDeployed);

  if (gDpaa2DebugFlags & DPAA2_DEBUG_DUMP_MC_LOG_FRAGMENT) {
    DumpMcLogTail(Mc, 10);
  }

  /*
   * Load MC DPL to DRAM:
   */
  Status = McLoadDpl(Mc, McFwSrc);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  /*
   * Tell the MC to deploy the DPL:
   */
  MmioWrite32((UINTN)&McCcsrRegs->Gsr, 0x0);

  /*
   * Wait for MC to deploy DPL:
   */
  DPAA2_INFO_MSG("Deploying data path layout ... ");
  Status = WaitForMc(Mc, FALSE, &RegGsr);
  if (EFI_ERROR(Status)) {
    DumpMcLogTail(Mc, 10);
    return Status;
  }

  Mc->McDplDeployed = TRUE;
  if (gDpaa2DebugFlags & DPAA2_DEBUG_DUMP_MC_LOG_FRAGMENT) {
    DumpMcLogTail(Mc, 100);
  }

  if (gDpaa2DebugFlags & DPAA2_DEBUG_DUMP_ROOT_DPRC) {
    DumpRootDprc();
  }

  return EFI_SUCCESS;
}

