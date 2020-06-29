/** @file
  AML Helper.

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
#include <Api/AmlHelper.h>

#include <AmlCoreInterface.h>
#include <AmlInclude.h>
#include <String/AmlString.h>

/** Compare the NameString defined by the "Name ()" ASL function,
    and stored in the NameOpNode, with the input NameString.

  An ASL NameString is expected to be NULL terminated, and can be composed
  of NameSegs that have less that 4 chars, like "DEV". "DEV" will be expanded
  as "DEV_".

  An AML NameString is not NULL terminated and is is only composed of
  4 chars long NameSegs.

  @param  [in] NameOpNode   NameOp object node defining a variable.
                            Must have an AML_NAME_OP/0 OpCode/SubOpCode.
                            NameOp object nodes are defined in ASL
                            using the "Name ()" function.
  @param  [in] AslName      ASL NameString to compare the NameOp's name with.
                            Must be NULL terminated.

  @retval TRUE If the AslName and the AmlName defined by the NameOp node
          are similar.
  @retval FALSE Otherwise.
**/
BOOLEAN
EFIAPI
AmlNameOpCompareName (
  IN  AML_OBJECT_NODE_HANDLE    NameOpNode,
  IN  CHAR8                   * AslName
  )
{
  EFI_STATUS              Status;
  AML_DATA_NODE_HANDLE    NameDataNode;

  CHAR8                 * AmlName;
  UINT32                  AmlNameSize;

  BOOLEAN                 RetVal;

  if ((NameOpNode == NULL)                                                ||
      (AmlGetNodeType ((AML_NODE_HANDLE)NameOpNode) != EAmlNodeObject)    ||
      (!AmlNodeHasOpCode (NameOpNode, AML_NAME_OP, 0))                    ||
      (AslName == NULL)) {
    ASSERT (0);
    return FALSE;
  }

  // Get the NameOp name, being in a data node
  // which is the first fixed argument (i.e. index 0).
  NameDataNode = (AML_DATA_NODE_HANDLE)AmlGetFixedArgument (
                                         NameOpNode,
                                         EAmlParseIndexTerm0
                                         );
  if ((NameDataNode == NULL)                                            ||
      (AmlGetNodeType ((AML_NODE_HANDLE)NameDataNode) != EAmlNodeData)  ||
      (!AmlNodeHasDataType (NameDataNode, EAmlNodeDataTypeNameString))) {
    ASSERT (0);
    return FALSE;
  }

  // Get the size of the name.
  Status = AmlGetDataNodeBuffer (NameDataNode, NULL, &AmlNameSize);
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return FALSE;
  }

  // Allocate memory to fetch the name.
  AmlName = AllocateZeroPool (AmlNameSize);
  if (AmlName == NULL) {
    ASSERT (0);
    return FALSE;
  }

  // Fetch the name.
  Status = AmlGetDataNodeBuffer (NameDataNode, (UINT8*)AmlName, &AmlNameSize);
  if (EFI_ERROR (Status)) {
    FreePool (AmlName);
    ASSERT (0);
    return FALSE;
  }

  // Compare the input AslName and the AmlName stored in the NameOp node.
  RetVal = CompareAmlWithAslNameString (AmlName, AslName);

  // Free the string buffer.
  FreePool (AmlName);
  return RetVal;
}

/** Add a Resource Data node at the end of the list containing
    the input RdNode.

  The new Resource Data node is added at the end of the list of Resource Data
  nodes, but before the End Tag.
  If no End Tag is found, the function returns an error.

  @param  [in]  CurrRdNode  Add the new Resource Data node at the end of
                            the list containing this node.
  @param  [in]  NewRdNode   The new Resource Data node to add.

  @retval  EFI_SUCCESS            The function completed successfully.
  @retval  EFI_INVALID_PARAMETER  Invalid parameter.
**/
EFI_STATUS
EFIAPI
AmlAppendRdNode (
  AML_DATA_NODE_HANDLE  CurrRdNode,
  AML_DATA_NODE_HANDLE  NewRdNode
  )
{
  EFI_STATUS      Status;

  if ((CurrRdNode == NULL)                                              ||
      (AmlGetNodeType ((AML_NODE_HANDLE)CurrRdNode) != EAmlNodeData)    ||
      (!AmlNodeHasDataType (CurrRdNode, EAmlNodeDataTypeResourceData))  ||
      (NewRdNode == NULL)                                               ||
      (AmlGetNodeType ((AML_NODE_HANDLE)NewRdNode) != EAmlNodeData)     ||
      (!AmlNodeHasDataType (NewRdNode, EAmlNodeDataTypeResourceData))) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  // Iterate through the Resource Data nodes to find the End Tag.
  while (TRUE) {
    // If the Resource Data is an End Tag,
    // add the new node before and return.
    if (AmlNodeHasRdDataType (
          CurrRdNode,
          AML_RD_BUILD_SMALL_DESC_ID (EAmlRdsIdEndTag)
          )) {
      Status = AmlVarListAddBefore (
                 (AML_NODE_HANDLE)CurrRdNode,
                 (AML_NODE_HANDLE)NewRdNode)
                 ;
      if (EFI_ERROR (Status)) {
        ASSERT (0);
      }
      return Status;
    }

    // Get the next Resource Data node.
    // If this was the last node and no End Tag was found, return error.
    // It is possible to have only one Resource Data in a BufferOp,
    // but it should not be possible to add a new Resource Data in the list
    // in this case.
    CurrRdNode = (AML_DATA_NODE_HANDLE)AmlGetSiblingVariableArgument (
                                         (AML_NODE_HANDLE)CurrRdNode
                                         );
    if ((CurrRdNode == NULL)                                              ||
        (AmlGetNodeType ((AML_NODE_HANDLE)CurrRdNode) != EAmlNodeData)    ||
        (!AmlNodeHasDataType (CurrRdNode, EAmlNodeDataTypeResourceData))) {
      ASSERT (0);
      return EFI_INVALID_PARAMETER;
    }
  } // while
}

/** Check whether ObjectNode has the input OpCode/SubOpcode couple.

  @param  [in]  ObjectNode  Pointer to an object node.
  @param  [in]  OpCode      OpCode to check
  @param  [in]  SubOpCode   SubOpCode to check

  @retval TRUE    The node is an object node and
                  the Opcode and SubOpCode match.
  @retval FALSE   Otherwise.
**/
BOOLEAN
EFIAPI
AmlNodeHasOpCode (
  IN  AML_OBJECT_NODE_HANDLE    ObjectNode,
  IN  UINT8                     OpCode,
  IN  UINT8                     SubOpCode
  )
{
  EFI_STATUS    Status;
  UINT8         NodeOpCode;
  UINT8         NodeSubOpCode;

  // Get the Node information.
  Status = AmlGetObjectNodeInfo (
             ObjectNode,
             &NodeOpCode,
             &NodeSubOpCode,
             NULL,
             NULL
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return FALSE;
  }

  // Check the OpCode and SubOpCode.
  if ((OpCode != NodeOpCode)  ||
      (SubOpCode != NodeSubOpCode)) {
    return FALSE;
  }

  return TRUE;
}

/** Check whether DataNode has the input DataType.

  @param  [in]  DataNode   Pointer to a data node.
  @param  [in]  DataType   DataType to check.

  @retval TRUE    The node is a data node and
                  the DataType match.
  @retval FALSE   Otherwise.
**/
BOOLEAN
EFIAPI
AmlNodeHasDataType (
  IN  AML_DATA_NODE_HANDLE    DataNode,
  IN  EAML_NODE_DATA_TYPE     DataType
  )
{
  EFI_STATUS            Status;
  EAML_NODE_DATA_TYPE   NodeDataType;

  // Get the data type.
  Status = AmlGetNodeDataType (DataNode, &NodeDataType);
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return FALSE;
  }

  // Check the data type.
  if (NodeDataType != DataType) {
    return FALSE;
  }

  return TRUE;
}

/** Check whether RdNode has the input RdDataType.

  @param  [in]  RdNode      Pointer to a data node.
  @param  [in]  RdDataType  DataType to check.

  @retval TRUE    The node is a Resource Data node and
                  the RdDataType match.
  @retval FALSE   Otherwise.
**/
BOOLEAN
EFIAPI
AmlNodeHasRdDataType (
  IN  AML_DATA_NODE_HANDLE    RdNode,
  IN  AML_RD_HEADER           RdDataType
  )
{
  EFI_STATUS      Status;
  AML_RD_HEADER   NodeRdDataType;

  // Get the resource data type.
  Status = AmlGetResourceDataType (
             RdNode,
             &NodeRdDataType
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return FALSE;
  }

  // Check the RdDataType.
  return AmlRdCompareDescId (&NodeRdDataType, RdDataType);
}
