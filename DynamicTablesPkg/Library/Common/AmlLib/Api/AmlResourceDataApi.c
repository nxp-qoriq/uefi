/** @file
  AML Update Resource Data.

  Copyright (c) 2020, ARM Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

/* Even though this file has access to the internal Node definition,
   i.e. AML_ROOT_NODE, AML_OBJECT_NODE, etc. Only the external node
   handle types should be used, i.e. AML_NODE_HANDLE, AML_ROOT_NODE_HANDLE,
   etc.
   Indeed, the functions in the "Api" folder should be implemented only
   using the "safe" functions available in the "Include" folder. This
   makes the functions available in the "Api" folder easy to export.
*/
#include <AmlNodeDefines.h>

#include <AmlCoreInterface.h>
#include <AmlInclude.h>
#include <Api/AmlHelper.h>
#include <CodeGen/AmlResourceDataCodeGen.h>

/** Update the first interrupt of an Interrupt resource data node.

  The flags of the Interrupt resource data are left unchanged.

  The InterruptRdNode corresponds to the Resource Data created by the
  "Interrupt ()" ASL macro. It is an Extended Interrupt Resource Data.
  See ACPI 6.3 specification, s6.4.3.6 "Extended Interrupt Descriptor"
  for more information about Extended Interrupt Resource Data.

  @param  [in]  InterruptRdNode   Pointer to the an extended interrupt
                                  resource data node.
  @param  [in]  Irq               Interrupt value to update.

  @retval  EFI_SUCCESS            The function completed successfully.
  @retval  EFI_INVALID_PARAMETER  Invalid parameter.
  @retval  EFI_OUT_OF_RESOURCES   Out of resources.
**/
EFI_STATUS
EFIAPI
AmlUpdateRdInterrupt (
  IN  AML_DATA_NODE_HANDLE    InterruptRdNode,
  IN  UINT32                  Irq
  )
{
  EFI_STATUS                    Status;
  UINT32                      * FirstInterrupt;
  UINT8                       * QueryBuffer;
  UINT32                        QueryBufferSize;

  if ((InterruptRdNode == NULL)                                           ||
      (AmlGetNodeType ((AML_NODE_HANDLE)InterruptRdNode) != EAmlNodeData) ||
      (!AmlNodeHasDataType (
          InterruptRdNode,
          EAmlNodeDataTypeResourceData))                                  ||
      (!AmlNodeHasRdDataType (
          InterruptRdNode,
          AML_RD_BUILD_LARGE_DESC_ID (EAmlRdlIdExtendedInterrupt)))) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  QueryBuffer = NULL;

  // Get the size of the InterruptRdNode buffer.
  Status = AmlGetDataNodeBuffer (
             InterruptRdNode,
             NULL,
             &QueryBufferSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  // Cf. AML_RD_EXTENDED_INTERRUPT structure,
  // the interrupts are stored on UINT32 values.
  // Check the Buffer is large enough.
  if (QueryBufferSize <
        (sizeof (AML_RD_EXTENDED_INTERRUPT) + sizeof (UINT32))) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  // Allocate a buffer to fetch the data.
  QueryBuffer = AllocatePool (QueryBufferSize);
  if (QueryBuffer == NULL) {
    ASSERT (0);
    return EFI_OUT_OF_RESOURCES;
  }

  // Get the data.
  Status = AmlGetDataNodeBuffer (
             InterruptRdNode,
             QueryBuffer,
             &QueryBufferSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    goto error_handler;
  }

  // Get the address of the first interrupt field.
  FirstInterrupt = (UINT32*)(QueryBuffer +
                                sizeof(AML_RD_EXTENDED_INTERRUPT));

  *FirstInterrupt = Irq;

  // Update the InterruptRdNode buffer.
  Status = AmlUpdateDataNode (
             InterruptRdNode,
             EAmlNodeDataTypeResourceData,
             QueryBuffer,
             QueryBufferSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
  }

error_handler:
  if (QueryBuffer != NULL) {
    FreePool (QueryBuffer);
  }
  return Status;
}

/** Update the interrupt list of an interrupt resource data node.

  The InterruptRdNode corresponds to the Resource Data created by the
  "Interrupt ()" ASL function. It is an Extended Interrupt Resource Data.
  See ACPI 6.3 specification, s6.4.3.6 "Extended Interrupt Descriptor"
  for more information about Extended Interrupt Resource Data.

  @param  [in]  InterruptRdNode   Pointer to the an extended interrupt
                                  resource data node.
  @param  [in]  ResourceConsumer    The device consumes the specified interrupt
                                    or produces it for use by a child device.
  @param  [in]  EdgeTriggered       The interrupt is edge triggered or
                                    level triggered.
  @param  [in]  ActiveLow           The interrupt is active-high or active-low.
  @param  [in]  Shared              The interrupt can be shared with other
                                    devices or not (Exclusive).
  @param  [in]  IrqList           Interrupt list. Must be non-NULL.
  @param  [in]  IrqCount          Interrupt count. Must be non-zero.


  @retval  EFI_SUCCESS            The function completed successfully.
  @retval  EFI_INVALID_PARAMETER  Invalid parameter.
  @retval  EFI_OUT_OF_RESOURCES   Out of resources.
**/
EFI_STATUS
EFIAPI
AmlUpdateRdInterruptEx (
  IN  AML_DATA_NODE_HANDLE    InterruptRdNode,
  IN  BOOLEAN                 ResourceConsumer,
  IN  BOOLEAN                 EdgeTriggered,
  IN  BOOLEAN                 ActiveLow,
  IN  BOOLEAN                 Shared,
  IN  UINT32                * IrqList,
  IN  UINT8                   IrqCount
  )
{
  EFI_STATUS                    Status;

  AML_RD_EXTENDED_INTERRUPT   * RdInterrupt;
  UINT32                      * FirstInterrupt;
  UINT8                       * UpdateBuffer;
  UINT16                        UpdateBufferSize;

  if ((InterruptRdNode == NULL)                                           ||
      (AmlGetNodeType ((AML_NODE_HANDLE)InterruptRdNode) != EAmlNodeData) ||
      (!AmlNodeHasDataType (
          InterruptRdNode,
          EAmlNodeDataTypeResourceData))                                  ||
      (!AmlNodeHasRdDataType (
          InterruptRdNode,
          AML_RD_BUILD_LARGE_DESC_ID (EAmlRdlIdExtendedInterrupt)))       ||
      (IrqList == NULL)                                                   ||
      (IrqCount == 0)) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  UpdateBuffer = NULL;
  UpdateBufferSize = sizeof (AML_RD_EXTENDED_INTERRUPT) +
                       (sizeof (UINT32) * IrqCount);

  // Allocate a buffer to update the data.
  UpdateBuffer = AllocatePool (UpdateBufferSize);
  if (UpdateBuffer == NULL) {
    ASSERT (0);
    return EFI_OUT_OF_RESOURCES;
  }

 // Update the Resource Data information (structure size, interrupt count).
  RdInterrupt = (AML_RD_EXTENDED_INTERRUPT*)UpdateBuffer;
  RdInterrupt->Id = AML_RD_BUILD_LARGE_DESC_ID (EAmlRdlIdExtendedInterrupt);
  RdInterrupt->Length = UpdateBufferSize - AML_RD_LARGE_HEADER_SIZE;
  RdInterrupt->InterruptTableLength = IrqCount;

  RdInterrupt->InterruptVectorFlags = (ResourceConsumer ? BIT0 : 0) |
                                      (EdgeTriggered ? BIT1 : 0)    |
                                      (ActiveLow ? BIT2 : 0)        |
                                      (Shared ? BIT3 : 0);

  // Get the address of the first interrupt field.
  FirstInterrupt = (UINT32*)(UpdateBuffer +
                                sizeof(AML_RD_EXTENDED_INTERRUPT));

  // Copy the input list of interrupts.
  CopyMem (FirstInterrupt, IrqList, (sizeof (UINT32) * IrqCount));

  // Update the InterruptRdNode buffer.
  Status = AmlUpdateDataNode (
             InterruptRdNode,
             EAmlNodeDataTypeResourceData,
             UpdateBuffer,
             UpdateBufferSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
  }

  // Cleanup
  FreePool (UpdateBuffer);

  return Status;
}

/** Update the base address and length of a QWord resource data node.

  @param  [in] QWordRdNode         Pointer a QWord resource data
                                   node.
  @param  [in] BaseAddress         Base address.
  @param  [in] BaseAddressLength   Base address length.

  @retval  EFI_SUCCESS            The function completed successfully.
  @retval  EFI_INVALID_PARAMETER  Invalid parameter.
  @retval  EFI_OUT_OF_RESOURCES   Out of resources.
**/
EFI_STATUS
EFIAPI
AmlUpdateRdQWord (
  IN  AML_DATA_NODE_HANDLE  QWordRdNode,
  IN  UINT64                BaseAddress,
  IN  UINT64                BaseAddressLength
  )
{
  EFI_STATUS                    Status;
  AML_RD_QWORD_ADDRESS_SPACE  * RdQWord;

  UINT8                       * QueryBuffer;
  UINT32                        QueryBufferSize;

  if ((QWordRdNode == NULL)                                             ||
      (AmlGetNodeType ((AML_NODE_HANDLE)QWordRdNode) != EAmlNodeData)   ||
      (!AmlNodeHasDataType (QWordRdNode, EAmlNodeDataTypeResourceData)) ||
      (!AmlNodeHasRdDataType (
          QWordRdNode,
          AML_RD_BUILD_LARGE_DESC_ID (EAmlRdlIdQwordAddressSpace)))) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  // Get the size of the QWordRdNode's buffer.
  Status = AmlGetDataNodeBuffer (
             QWordRdNode,
             NULL,
             &QueryBufferSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  // Allocate a buffer to fetch the data.
  QueryBuffer = AllocatePool (QueryBufferSize);
  if (QueryBuffer == NULL) {
    ASSERT (0);
    return EFI_OUT_OF_RESOURCES;
  }

  // Get the data.
  Status = AmlGetDataNodeBuffer (
             QWordRdNode,
             QueryBuffer,
             &QueryBufferSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    goto error_handler;
  }

  RdQWord = (AML_RD_QWORD_ADDRESS_SPACE*)QueryBuffer;

  // Update the Base Address and Length.
  RdQWord->AddressRangeMinimum = BaseAddress;
  RdQWord->AddressRangeMaximum = BaseAddress + BaseAddressLength - 1;
  RdQWord->AddressLength = BaseAddressLength;

  // Update Base Address Resource Data node.
  Status = AmlUpdateDataNode (
             QWordRdNode,
             EAmlNodeDataTypeResourceData,
             QueryBuffer,
             QueryBufferSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
  }

error_handler:
  if (QueryBuffer != NULL) {
    FreePool (QueryBuffer);
  }
  return Status;
}

/** Add an Interrupt Resource Data node.

  This function creates a Resource Data element corresponding to the
  "Interrupt ()" ASL function, stores it in an AML Data Node.

  It then adds it after the input CurrRdNode in the list of resource data
  element.

  The Resource Data effectively created is an Extended Interrupt Resource
  Data. See ACPI 6.3 specification, s6.4.3.6 "Extended Interrupt Descriptor"
  for more information about Extended Interrupt Resource Data.

  The Extended Interrupt contains one single interrupt.

  This function allocates memory to create a data node. It is the caller's
  responsibility to either:
   - attach this node to an AML tree;
   - delete this node.

  @param  [in]  NameOpCrsNode    NameOp object node defining a "_CRS" object.
                                 Must have an OpCode=AML_NAME_OP, SubOpCode=0.
                                 NameOp object nodes are defined in ASL
                                 using the "Name ()" function.
  @param  [in]  ResourceConsumer The device consumes the specified interrupt
                                 or produces it for use by a child device.
  @param  [in]  EdgeTriggered    The interrupt is edge triggered or
                                 level triggered.
  @param  [in]  ActiveLow        The interrupt is active-high or active-low.
  @param  [in]  Shared           The interrupt can be shared with other
                                 devices or not (Exclusive).
  @param  [in]  IrqList          Interrupt list. Must be non-NULL.
  @param  [in]  IrqCount         Interrupt count. Must be non-zero.


  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES    Could not allocate memory.
**/
EFI_STATUS
EFIAPI
AmlNameOpCrsAddRdInterrupt (
  IN  AML_OBJECT_NODE_HANDLE  NameOpCrsNode,
  IN  BOOLEAN                 ResourceConsumer,
  IN  BOOLEAN                 EdgeTriggered,
  IN  BOOLEAN                 ActiveLow,
  IN  BOOLEAN                 Shared,
  IN  UINT32                * IrqList,
  IN  UINT8                   IrqCount
  )
{
  EFI_STATUS              Status;
  EFI_STATUS              Status1;

  AML_DATA_NODE_HANDLE    FirstRdElement;
  AML_DATA_NODE_HANDLE    NewRdNode;

  if ((IrqList == NULL)                                                   ||
      (IrqCount == 0)                                                     ||
      (NameOpCrsNode == NULL)                                             ||
      (AmlGetNodeType ((AML_NODE_HANDLE)NameOpCrsNode) != EAmlNodeObject) ||
      (!AmlNodeHasOpCode (NameOpCrsNode, AML_NAME_OP, 0))                 ||
      (!AmlNameOpCompareName (NameOpCrsNode, "_CRS"))) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  Status = AmlNameOpCrsGetFirstRdNode (NameOpCrsNode, &FirstRdElement);
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  // Generate the Extended Interrupt Resource Data node.
  Status = AmlCodeGenInterrupt (
             ResourceConsumer,
             EdgeTriggered,
             ActiveLow,
             Shared,
             IrqList,
             IrqCount,
             &NewRdNode
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  // Add the generated node at the last position,
  // but before the "End Tag" Resource Data.
  Status = AmlAppendRdNode (FirstRdElement, NewRdNode);
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    Status1 = AmlDeleteTree ((AML_NODE_HANDLE)NewRdNode);
    if (EFI_ERROR (Status1)) {
      ASSERT (0);
    }
  }

  return Status;
}
