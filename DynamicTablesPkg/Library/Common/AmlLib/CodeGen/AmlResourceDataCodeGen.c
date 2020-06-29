/** @file
  AML Resource Data Code Generation.

  Copyright (c) 2020, ARM Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Glossary:
  - Rd or RD   - Resource Data
  - Rds or RDS - Resource Data Small
  - Rdl or RDL - Resource Data Large
**/

#include <AmlNodeDefines.h>
#include <CodeGen/AmlResourceDataCodeGen.h>

#include <AmlDefines.h>
#include <Tree/AmlNode.h>
#include <ResourceData/AmlResourceData.h>

/** Code generation for the "Interrupt ()" ASL function.

  This function creates a Resource Data element corresponding to the
  "Interrupt ()" ASL function and stores it in an AML Data Node.

  The Resource Data effectively created is an Extended Interrupt Resource
  Data. See ACPI 6.3 specification, s6.4.3.6 "Extended Interrupt Descriptor"
  for more information about Extended Interrupt Resource Data.

  This function allocates memory to create a data node. It is the caller's
  responsibility to either:
   - attach this node to an AML tree;
   - delete this node.

  @param  [in]  ResourceConsumer    The device consumes the specified interrupt
                                    or produces it for use by a child device.
  @param  [in]  EdgeTriggered       The interrupt is edge triggered or
                                    level triggered.
  @param  [in]  ActiveLow           The interrupt is active-high or active-low.
  @param  [in]  Shared              The interrupt can be shared with other
                                    devices or not (Exclusive).
  @param  [in]  IrqList             Interrupt list. Must be non-NULL.
  @param  [in]  IrqCount            Interrupt count. Must be non-zero.
  @param  [out] OutRdNode           The generated Resource Data node.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES    Could not allocate memory.
**/
EFI_STATUS
EFIAPI
AmlCodeGenInterrupt (
  IN  BOOLEAN                 ResourceConsumer,
  IN  BOOLEAN                 EdgeTriggered,
  IN  BOOLEAN                 ActiveLow,
  IN  BOOLEAN                 Shared,
  IN  UINT32                * IrqList,
  IN  UINT8                   IrqCount,
  OUT AML_DATA_NODE_HANDLE  * OutRdNode
  )
{
  EFI_STATUS                  Status;

  UINT8                     * Buffer;
  UINT16                      BufferSize;

  AML_RD_EXTENDED_INTERRUPT * RdInterrupt;
  UINT32                    * FirstInterrupt;

  if ((IrqList == NULL) ||
      (IrqCount == 0)   ||
      (OutRdNode == NULL)) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  // Allocate memory for an AML_RD_EXTENDED_INTERRUPT structure
  // + one interrupt on a UINT32.
  BufferSize = (UINT16)(sizeof (AML_RD_EXTENDED_INTERRUPT) +
                          (sizeof (UINT32) * IrqCount));
  Buffer = AllocateZeroPool (BufferSize);
  if (Buffer == NULL) {
    ASSERT (0);
    return EFI_OUT_OF_RESOURCES;
  }

  RdInterrupt = (AML_RD_EXTENDED_INTERRUPT*)Buffer;

  RdInterrupt->Id = AML_RD_BUILD_LARGE_DESC_ID (EAmlRdlIdExtendedInterrupt);
  RdInterrupt->Length = BufferSize - AML_RD_LARGE_HEADER_SIZE;
  RdInterrupt->InterruptVectorFlags = (ResourceConsumer ? BIT0 : 0) |
                                      (EdgeTriggered ? BIT1 : 0)    |
                                      (ActiveLow ? BIT2 : 0)        |
                                      (Shared ? BIT3 : 0);
  RdInterrupt->InterruptTableLength = IrqCount;

  // Get the address of the first interrupt field.
  FirstInterrupt = (UINT32*)(Buffer + sizeof (AML_RD_EXTENDED_INTERRUPT));

  // Copy the list of interrupts.
  CopyMem (FirstInterrupt, IrqList, (sizeof (UINT32) * IrqCount));

  Status = AmlCreateDataNode (
             EAmlNodeDataTypeResourceData,
             Buffer,
             BufferSize,
             OutRdNode
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    FreePool (Buffer);
  }

  return Status;
}
