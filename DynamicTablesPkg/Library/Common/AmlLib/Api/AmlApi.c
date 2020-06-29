/** @file
  AML Api.

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
#include <String/AmlString.h>

/** Update the name of a DeviceOp object node.

  @param  [in] DeviceOpNode   Object node representing a Device.
                              Must have an OpCode=AML_NAME_OP, SubOpCode=0.
                              OpCode/SubOpCode.
                              DeviceOp object nodes are defined in ASL
                              using the "Device ()" function.
  @param  [in] NewNameString  The new Device's name.
                              Must be a NameString,
                              e.g.: "DEV0", "DV15.DEV0", etc.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
**/
EFI_STATUS
EFIAPI
AmlDeviceOpUpdateName (
  IN  AML_OBJECT_NODE_HANDLE    DeviceOpNode,
  IN  CHAR8                   * NewNameString
  )
{
  EFI_STATUS            Status;

  AML_DATA_NODE_HANDLE  DeviceNameDataNode;
  UINT32                NewNameStringSize;

  // Check the input node is an object node.
  if ((DeviceOpNode == NULL)                                              ||
      (AmlGetNodeType ((AML_NODE_HANDLE)DeviceOpNode) != EAmlNodeObject)  ||
      (!AmlNodeHasOpCode (DeviceOpNode, AML_EXT_OP, AML_EXT_DEVICE_OP))   ||
      (NewNameString == NULL)) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  // Get the Device's name, being a data node
  // which is the 1st fixed argument (i.e. index 0).
  DeviceNameDataNode = (AML_DATA_NODE_HANDLE)AmlGetFixedArgument (
                                               DeviceOpNode,
                                               EAmlParseIndexTerm0
                                               );
  if ((DeviceNameDataNode == NULL)                                            ||
      (AmlGetNodeType ((AML_NODE_HANDLE)DeviceNameDataNode) != EAmlNodeData)  ||
      (!AmlNodeHasDataType (DeviceNameDataNode, EAmlNodeDataTypeNameString))) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  Status = AmlGetNameStringSize (NewNameString, &NewNameStringSize);
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  // Update the Device's name node.
  Status = AmlUpdateDataNode (
             DeviceNameDataNode,
             EAmlNodeDataTypeNameString,
             (UINT8*)NewNameString,
             NewNameStringSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
  }

  return Status;
}

/** Update the integer value contained in a "_UID" object.

  The "_UID" object must be stored in a NameOp object node.

  @param  [in] NameOpUidNode  NameOp object node defining a "_UID" object.
                              Must have an OpCode=AML_NAME_OP, SubOpCode=0.
                              NameOp object nodes are defined in ASL
                              using the "Name ()" function.
  @param  [in] NewInt         New Integer value to assign to _UID.
                              Must be a UINT64.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
**/
EFI_STATUS
EFIAPI
AmlNameOpUidUpdateValue (
  IN  AML_OBJECT_NODE_HANDLE  NameOpUidNode,
  IN  UINT64                  NewInt
  )
{
  EFI_STATUS              Status;

  AML_OBJECT_NODE_HANDLE  IntegerOpNode;

  // Check the input node is an object node.
  if ((NameOpUidNode == NULL)                                             ||
      (AmlGetNodeType ((AML_NODE_HANDLE)NameOpUidNode) != EAmlNodeObject) ||
      (!AmlNodeHasOpCode (NameOpUidNode, AML_NAME_OP, 0))                 ||
      (!AmlNameOpCompareName (NameOpUidNode, "_UID"))) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  // Get the _UID value, being an Integer object node:
  // it must have an Integer OpCode (Byte/Word/DWord/QWord)
  // which is the 2nd fixed argument (i.e. index 1).
  // This can also be a ZeroOp or OneOp node.
  IntegerOpNode = (AML_OBJECT_NODE_HANDLE)AmlGetFixedArgument (
                                            NameOpUidNode,
                                            EAmlParseIndexTerm1
                                            );
  if ((IntegerOpNode == NULL)  ||
      (AmlGetNodeType ((AML_NODE_HANDLE)IntegerOpNode) != EAmlNodeObject)) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  // Update the Integer value of the "_UID" object.
  Status = AmlUpdateInteger (IntegerOpNode, NewInt);
  if (EFI_ERROR (Status)) {
    ASSERT (0);
  }

  return Status;
}

/** Get the first Resource Data element contained in a "_CRS" object.

  In the following ASL code, the function will return the Resource Data
  node corresponding to the "QWordMemory ()" ASL macro.
  Name (_CRS, ResourceTemplate() {
      QWordMemory (...) {...},
      Interrupt (...) {...}
    }
  )

  Note:
   - The "_CRS" object must be declared using ASL "Name (Declare Named Object)".
   - "_CRS" declared using ASL "Method (Declare Control Method)" is not
     supported.

  @param  [in] NameOpCrsNode  NameOp object node defining a "_CRS" object.
                              Must have an OpCode=AML_NAME_OP, SubOpCode=0.
                              NameOp object nodes are defined in ASL
                              using the "Name ()" function.
  @param  [out] OutRdNode     Pointer to the first Resource Data element of
                              the "_CRS" object. A Resource Data element
                              is stored in a data node.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
**/
EFI_STATUS
EFIAPI
AmlNameOpCrsGetFirstRdNode (
  IN  AML_OBJECT_NODE_HANDLE   NameOpCrsNode,
  OUT AML_DATA_NODE_HANDLE   * OutRdNode
  )
{
  AML_OBJECT_NODE_HANDLE  BufferOpNode;
  AML_DATA_NODE_HANDLE    FirstRdNode;

  if ((NameOpCrsNode == NULL)                                              ||
      (AmlGetNodeType ((AML_NODE_HANDLE)NameOpCrsNode) != EAmlNodeObject)  ||
      (!AmlNodeHasOpCode (NameOpCrsNode, AML_NAME_OP, 0))                  ||
      (!AmlNameOpCompareName (NameOpCrsNode, "_CRS"))                      ||
      (OutRdNode == NULL)) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  *OutRdNode = NULL;

  // Get the _CRS value which is represented as a BufferOp object node
  // which is the 2nd fixed argument (i.e. index 1).
  BufferOpNode = (AML_OBJECT_NODE_HANDLE)AmlGetFixedArgument (
                                           NameOpCrsNode,
                                           EAmlParseIndexTerm1
                                           );
  if ((BufferOpNode == NULL)                                             ||
      (AmlGetNodeType ((AML_NODE_HANDLE)BufferOpNode) != EAmlNodeObject) ||
      (!AmlNodeHasOpCode (BufferOpNode, AML_BUFFER_OP, 0))) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  // Get the first Resource data node in the variable list of
  // argument of the BufferOp node.
  FirstRdNode = (AML_DATA_NODE_HANDLE)AmlGetNextVariableArgument (
                                        (AML_NODE_HANDLE)BufferOpNode,
                                        NULL
                                        );
  if ((FirstRdNode == NULL)                                            ||
      (AmlGetNodeType ((AML_NODE_HANDLE)FirstRdNode) != EAmlNodeData)  ||
      (!AmlNodeHasDataType (FirstRdNode, EAmlNodeDataTypeResourceData))) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  *OutRdNode = FirstRdNode;
  return EFI_SUCCESS;
}

/** Get the Resource Data element following the CurrRdNode Resource Data.

  In the following ASL code, if CurrRdNode corresponds to the first
  "QWordMemory ()" ASL macro, the function will return the Resource Data
  node corresponding to the "Interrupt ()" ASL macro.
  Name (_CRS, ResourceTemplate() {
      QwordMemory (...) {...},
      Interrupt (...) {...}
    }
  )

  The CurrRdNode Resource Data node must be defined in an object named "_CRS"
  and defined by a "Name ()" ASL function.

  @param  [in]  CurrRdNode   Pointer to the current Resource Data element of
                             the "_CRS" object.
  @param  [out] OutRdNode    Pointer to the Resource Data element following
                             the CurrRdNode.
                             Contain a NULL pointer if CurrRdNode is the
                             last Resource Data element in the list.
                             The "End Tag" is not considered as a resource
                             data element and is not returned.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
**/
EFI_STATUS
EFIAPI
AmlNameOpCrsGetNextRdNode (
  IN  AML_DATA_NODE_HANDLE    CurrRdNode,
  OUT AML_DATA_NODE_HANDLE  * OutRdNode
  )
{
  AML_OBJECT_NODE_HANDLE     NameOpCrsNode;
  AML_OBJECT_NODE_HANDLE     BufferOpNode;

  if ((CurrRdNode == NULL)                                              ||
      (AmlGetNodeType ((AML_NODE_HANDLE)CurrRdNode) != EAmlNodeData)    ||
      (!AmlNodeHasDataType (CurrRdNode, EAmlNodeDataTypeResourceData))  ||
      (OutRdNode == NULL)) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  *OutRdNode = NULL;

  // The parent of the CurrRdNode must be a BufferOp node.
  BufferOpNode = (AML_OBJECT_NODE_HANDLE)AmlGetParent (
                                           (AML_NODE_HANDLE)CurrRdNode
                                           );
  if ((BufferOpNode == NULL)  ||
      (!AmlNodeHasOpCode (BufferOpNode, AML_BUFFER_OP, 0))) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  // The parent of the BufferOpNode must be a NameOp node.
  NameOpCrsNode = (AML_OBJECT_NODE_HANDLE)AmlGetParent (
                                            (AML_NODE_HANDLE)BufferOpNode
                                            );
  if ((NameOpCrsNode == NULL)                             ||
      (!AmlNodeHasOpCode (NameOpCrsNode, AML_NAME_OP, 0)) ||
      (!AmlNameOpCompareName (NameOpCrsNode, "_CRS"))) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  *OutRdNode = (AML_DATA_NODE_HANDLE)AmlGetNextVariableArgument (
                                       (AML_NODE_HANDLE)BufferOpNode,
                                       (AML_NODE_HANDLE)CurrRdNode
                                       );

  // If the Resource Data is an End Tag, return NULL.
  if (AmlNodeHasRdDataType (
        *OutRdNode,
        AML_RD_BUILD_SMALL_DESC_ID (EAmlRdsIdEndTag)
        )) {
    *OutRdNode = NULL;
  }

  return EFI_SUCCESS;
}
