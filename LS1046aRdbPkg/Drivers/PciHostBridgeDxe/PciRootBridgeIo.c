/** PciRootBridgeIo.c
 PCI Root Bridge Io Protocol implementation

 Based on PCI RootBridge implementation in
 ArmPlatformPkg/ArmJunoPkg/Drivers/PciHostBridgeDxe/PciRootBridge.c

 Copyright (c) 2011-2015, ARM Ltd. All rights reserved.
 Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution. The full text of the license may be found 
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/ 

#include <PciHostBridge.h>


typedef struct {
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR     SpaceDesp[ResTypeMax+1];
  EFI_ACPI_END_TAG_DESCRIPTOR           EndDesp;
} RESOURCE_CONFIGURATION;

RESOURCE_CONFIGURATION Configuration = {
   {{ACPI_ADDRESS_SPACE_DESCRIPTOR, 0x2B, ACPI_ADDRESS_SPACE_TYPE_IO , 0, 0, 0, 0, 0, 0, 0},
    {ACPI_ADDRESS_SPACE_DESCRIPTOR, 0x2B, ACPI_ADDRESS_SPACE_TYPE_MEM, 0, 0, 32, 0, 0, 0, 0},
    {ACPI_ADDRESS_SPACE_DESCRIPTOR, 0x2B, ACPI_ADDRESS_SPACE_TYPE_MEM, 0, 6, 32, 0, 0, 0, 0},
    {ACPI_ADDRESS_SPACE_DESCRIPTOR, 0x2B, ACPI_ADDRESS_SPACE_TYPE_MEM, 0, 0, 64, 0, 0, 0, 0},
    {ACPI_ADDRESS_SPACE_DESCRIPTOR, 0x2B, ACPI_ADDRESS_SPACE_TYPE_MEM, 0, 6, 64, 0, 0, 0, 0},
    {ACPI_ADDRESS_SPACE_DESCRIPTOR, 0x2B, ACPI_ADDRESS_SPACE_TYPE_BUS, 0, 0, 0, 0, 255, 0, 255}},
    {ACPI_END_TAG_DESCRIPTOR, 0}
};

//
// Memory Controller Pci Root Bridge Io Module Variables
//
EFI_METRONOME_ARCH_PROTOCOL *mMetronome;

//
// Protocol Member Function Prototypes
//

/**
   Polls an address in memory mapped I/O space until an exit condition is met, or 
   a timeout occurs. 

   This function provides a standard way to poll a PCI memory location. A PCI memory read
   operation is performed at the PCI memory address specified by Address for the width specified
   by Width. The result of this PCI memory read operation is stored in Result. This PCI memory
   read operation is repeated until either a timeout of Delay 100 ns units has expired, or (Result &
   Mask) is equal to Value.

   @param[in]   This      A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width     Signifies the width of the memory operations.
   @param[in]   Address   The base address of the memory operations. The caller is
                          responsible for aligning Address if required.
   @param[in]   Mask      Mask used for the polling criteria. Bytes above Width in Mask
                          are ignored. The bits in the bytes below Width which are zero in
                          Mask are ignored when polling the memory address.
   @param[in]   Value     The comparison value used for the polling exit criteria.
   @param[in]   Delay     The number of 100 ns units to poll. Note that timer available may
                          be of poorer granularity.
   @param[out]  Result    Pointer to the last value read from the memory location.
   
   @retval EFI_SUCCESS            The last data returned from the access matched the poll exit criteria.
   @retval EFI_INVALID_PARAMETER  Width is invalid.
   @retval EFI_INVALID_PARAMETER  Result is NULL.
   @retval EFI_TIMEOUT            Delay expired before a match occurred.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoPollMem ( 
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN  UINT64                                 Address,
  IN  UINT64                                 Mask,
  IN  UINT64                                 Value,
  IN  UINT64                                 Delay,
  OUT UINT64                                 *Result
  );
  
/**
   Reads from the I/O space of a PCI Root Bridge. Returns when either the polling exit criteria is
   satisfied or after a defined duration.

   This function provides a standard way to poll a PCI I/O location. A PCI I/O read operation is
   performed at the PCI I/O address specified by Address for the width specified by Width.
   The result of this PCI I/O read operation is stored in Result. This PCI I/O read operation is
   repeated until either a timeout of Delay 100 ns units has expired, or (Result & Mask) is equal
   to Value.

   @param[in] This      A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in] Width     Signifies the width of the I/O operations.
   @param[in] Address   The base address of the I/O operations. The caller is responsible
                        for aligning Address if required.
   @param[in] Mask      Mask used for the polling criteria. Bytes above Width in Mask
                        are ignored. The bits in the bytes below Width which are zero in
                        Mask are ignored when polling the I/O address.
   @param[in] Value     The comparison value used for the polling exit criteria.
   @param[in] Delay     The number of 100 ns units to poll. Note that timer available may
                        be of poorer granularity.
   @param[out] Result   Pointer to the last value read from the memory location.
   
   @retval EFI_SUCCESS            The last data returned from the access matched the poll exit criteria.
   @retval EFI_INVALID_PARAMETER  Width is invalid.
   @retval EFI_INVALID_PARAMETER  Result is NULL.
   @retval EFI_TIMEOUT            Delay expired before a match occurred.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoPollIo ( 
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN  UINT64                                 Address,
  IN  UINT64                                 Mask,
  IN  UINT64                                 Value,
  IN  UINT64                                 Delay,
  OUT UINT64                                 *Result
  );
  
/**
   Enables a PCI driver to access PCI controller registers in the PCI root bridge memory space.

   The Mem.Read(), and Mem.Write() functions enable a driver to access PCI controller
   registers in the PCI root bridge memory space.
   The memory operations are carried out exactly as requested. The caller is responsible for satisfying
   any alignment and memory width restrictions that a PCI Root Bridge on a platform might require.

   @param[in]   This      A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width     Signifies the width of the memory operation.
   @param[in]   Address   The base address of the memory operation. The caller is
                          responsible for aligning the Address if required.
   @param[in]   Count     The number of memory operations to perform. Bytes moved is
                          Width size * Count, starting at Address.
   @param[out]  Buffer    For read operations, the destination buffer to store the results. For
                          write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS            The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoMemRead (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  OUT    VOID                                   *Buffer
  );

/**
   Enables a PCI driver to access PCI controller registers in the PCI root bridge memory space.

   The Mem.Read(), and Mem.Write() functions enable a driver to access PCI controller
   registers in the PCI root bridge memory space.
   The memory operations are carried out exactly as requested. The caller is responsible for satisfying
   any alignment and memory width restrictions that a PCI Root Bridge on a platform might require.

   @param[in]   This      A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width     Signifies the width of the memory operation.
   @param[in]   Address   The base address of the memory operation. The caller is
                          responsible for aligning the Address if required.
   @param[in]   Count     The number of memory operations to perform. Bytes moved is
                          Width size * Count, starting at Address.
   @param[in]   Buffer    For read operations, the destination buffer to store the results. For
                          write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS            The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.
**/
EFI_STATUS
EFIAPI
RootBridgeIoMemWrite (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN     VOID                                   *Buffer
  );

/**
   Enables a PCI driver to access PCI controller registers in the PCI root bridge I/O space.

   @param[in]   This        A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width       Signifies the width of the memory operations.
   @param[in]   UserAddress The base address of the I/O operation. The caller is responsible for
                            aligning the Address if required.
   @param[in]   Count       The number of I/O operations to perform. Bytes moved is Width
                            size * Count, starting at Address.
   @param[out]  UserBuffer  For read operations, the destination buffer to store the results. For
                            write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS              The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER    Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER    Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoIoRead (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 UserAddress,
  IN     UINTN                                  Count,
  OUT    VOID                                   *UserBuffer
  );

/**
   Enables a PCI driver to access PCI controller registers in the PCI root bridge I/O space.

   @param[in]   This        A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width       Signifies the width of the memory operations.
   @param[in]   UserAddress The base address of the I/O operation. The caller is responsible for
                            aligning the Address if required.
   @param[in]   Count       The number of I/O operations to perform. Bytes moved is Width
                            size * Count, starting at Address.
   @param[in]   UserBuffer  For read operations, the destination buffer to store the results. For
                            write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS              The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER    Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER    Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoIoWrite (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 UserAddress,
  IN     UINTN                                  Count,
  IN     VOID                                   *UserBuffer
  );

/**
   Enables a PCI driver to copy one region of PCI root bridge memory space to another region of PCI
   root bridge memory space.

   The CopyMem() function enables a PCI driver to copy one region of PCI root bridge memory
   space to another region of PCI root bridge memory space. This is especially useful for video scroll
   operation on a memory mapped video buffer.
   The memory operations are carried out exactly as requested. The caller is responsible for satisfying
   any alignment and memory width restrictions that a PCI root bridge on a platform might require.

   @param[in] This        A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL instance.
   @param[in] Width       Signifies the width of the memory operations.
   @param[in] DestAddress The destination address of the memory operation. The caller is
                          responsible for aligning the DestAddress if required.
   @param[in] SrcAddress  The source address of the memory operation. The caller is
                          responsible for aligning the SrcAddress if required.
   @param[in] Count       The number of memory operations to perform. Bytes moved is
                          Width size * Count, starting at DestAddress and SrcAddress.
   
   @retval  EFI_SUCCESS             The data was copied from one memory region to another memory region.
   @retval  EFI_INVALID_PARAMETER   Width is invalid for this PCI root bridge.
   @retval  EFI_OUT_OF_RESOURCES    The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoCopyMem (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 DestAddress,
  IN     UINT64                                 SrcAddress,
  IN     UINTN                                  Count
  );

/**
   Enables a PCI driver to access PCI controller registers in a PCI root bridge's configuration space.

   The Pci.Read() and Pci.Write() functions enable a driver to access PCI configuration
   registers for a PCI controller.
   The PCI Configuration operations are carried out exactly as requested. The caller is responsible for
   any alignment and PCI configuration width issues that a PCI Root Bridge on a platform might
   require.

   @param[in]   This      A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width     Signifies the width of the memory operations.
   @param[in]   Address   The address within the PCI configuration space for the PCI controller.
   @param[in]   Count     The number of PCI configuration operations to perform. Bytes
                          moved is Width size * Count, starting at Address.
   @param[out]  Buffer    For read operations, the destination buffer to store the results. For
                          write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS            The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoPciRead (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  OUT    VOID                                   *Buffer
  );

/**
   Enables a PCI driver to access PCI controller registers in a PCI root bridge's configuration space.

   The Pci.Read() and Pci.Write() functions enable a driver to access PCI configuration
   registers for a PCI controller.
   The PCI Configuration operations are carried out exactly as requested. The caller is responsible for
   any alignment and PCI configuration width issues that a PCI Root Bridge on a platform might
   require.

   @param[in]   This      A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width     Signifies the width of the memory operations.
   @param[in]   Address   The address within the PCI configuration space for the PCI controller.
   @param[in]   Count     The number of PCI configuration operations to perform. Bytes
                          moved is Width size * Count, starting at Address.
   @param[in]   Buffer    For read operations, the destination buffer to store the results. For
                          write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS            The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoPciWrite (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN     VOID                                   *Buffer
  );

/**
   Provides the PCI controller-specific addresses required to access system memory from a
   DMA bus master.

   The Map() function provides the PCI controller specific addresses needed to access system
   memory. This function is used to map system memory for PCI bus master DMA accesses.

   @param[in]       This            A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]       Operation       Indicates if the bus master is going to read or write to system memory.
   @param[in]       HostAddress     The system memory address to map to the PCI controller.
   @param[in, out]  NumberOfBytes   On input the number of bytes to map. On output the number of bytes that were mapped.
   @param[out]      DeviceAddress   The resulting map address for the bus master PCI controller to use
                                    to access the system memory's HostAddress.
   @param[out]      Mapping         The value to pass to Unmap() when the bus master DMA operation is complete.
   
   @retval EFI_SUCCESS            The range was mapped for the returned NumberOfBytes.
   @retval EFI_INVALID_PARAMETER  Operation is invalid.
   @retval EFI_INVALID_PARAMETER  HostAddress is NULL.
   @retval EFI_INVALID_PARAMETER  NumberOfBytes is NULL.
   @retval EFI_INVALID_PARAMETER  DeviceAddress is NULL.
   @retval EFI_INVALID_PARAMETER  Mapping is NULL.
   @retval EFI_UNSUPPORTED        The HostAddress cannot be mapped as a common buffer.
   @retval EFI_DEVICE_ERROR       The system hardware could not map the requested address.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoMap (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL            *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION  Operation,
  IN     VOID                                       *HostAddress,
  IN OUT UINTN                                      *NumberOfBytes,
  OUT    EFI_PHYSICAL_ADDRESS                       *DeviceAddress,
  OUT    VOID                                       **Mapping
  );

/**
   Completes the Map() operation and releases any corresponding resources.

   The Unmap() function completes the Map() operation and releases any corresponding resources.
   If the operation was an EfiPciOperationBusMasterWrite or
   EfiPciOperationBusMasterWrite64, the data is committed to the target system memory.
   Any resources used for the mapping are freed.  

   @param[in] This      A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in] Mapping   The mapping value returned from Map().
   
   @retval EFI_SUCCESS            The range was unmapped.
   @retval EFI_INVALID_PARAMETER  Mapping is not a value that was returned by Map().
   @retval EFI_DEVICE_ERROR       The data was not committed to the target system memory.

**/
EFI_STATUS
EFIAPI
RootBridgeIoUnmap (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  VOID                             *Mapping
  );

/**
   Allocates pages that are suitable for an EfiPciOperationBusMasterCommonBuffer or
   EfiPciOperationBusMasterCommonBuffer64 mapping.
  
   @param This        A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param Type        This parameter is not used and must be ignored.
   @param MemoryType  The type of memory to allocate, EfiBootServicesData or EfiRuntimeServicesData.
   @param Pages       The number of pages to allocate.
   @param HostAddress A pointer to store the base system memory address of the allocated range.
   @param Attributes  The requested bit mask of attributes for the allocated range. Only
                      the attributes EFI_PCI_ATTRIBUTE_MEMORY_WRITE_COMBINE, EFI_PCI_ATTRIBUTE_MEMORY_CACHED, 
                      and EFI_PCI_ATTRIBUTE_DUAL_ADDRESS_CYCLE may be used with this function.
   
   @retval EFI_SUCCESS            The requested memory pages were allocated.
   @retval EFI_INVALID_PARAMETER  MemoryType is invalid.
   @retval EFI_INVALID_PARAMETER  HostAddress is NULL.
   @retval EFI_UNSUPPORTED        Attributes is unsupported. The only legal attribute bits are
                                  MEMORY_WRITE_COMBINE, MEMORY_CACHED, and DUAL_ADDRESS_CYCLE.
   @retval EFI_OUT_OF_RESOURCES   The memory pages could not be allocated.

**/
EFI_STATUS
EFIAPI
RootBridgeIoAllocateBuffer (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  EFI_ALLOCATE_TYPE                Type,
  IN  EFI_MEMORY_TYPE                  MemoryType,
  IN  UINTN                            Pages,
  OUT VOID                             **HostAddress,
  IN  UINT64                           Attributes
  );

/**
   Frees memory that was allocated with AllocateBuffer().

   The FreeBuffer() function frees memory that was allocated with AllocateBuffer().

   @param This        A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param Pages       The number of pages to free.
   @param HostAddress The base system memory address of the allocated range.
   
   @retval EFI_SUCCESS            The requested memory pages were freed.
   @retval EFI_INVALID_PARAMETER  The memory range specified by HostAddress and Pages
                                  was not allocated with AllocateBuffer().

**/
EFI_STATUS
EFIAPI
RootBridgeIoFreeBuffer (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  UINTN                            Pages,
  OUT VOID                             *HostAddress
  );

/**
   Flushes all PCI posted write transactions from a PCI host bridge to system memory.

   The Flush() function flushes any PCI posted write transactions from a PCI host bridge to system
   memory. Posted write transactions are generated by PCI bus masters when they perform write
   transactions to target addresses in system memory.
   This function does not flush posted write transactions from any PCI bridges. A PCI controller
   specific action must be taken to guarantee that the posted write transactions have been flushed from
   the PCI controller and from all the PCI bridges into the PCI host bridge. This is typically done with
   a PCI read transaction from the PCI controller prior to calling Flush().

   @param This        A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   
   @retval EFI_SUCCESS        The PCI posted write transactions were flushed from the PCI host
                              bridge to system memory.
   @retval EFI_DEVICE_ERROR   The PCI posted write transactions were not flushed from the PCI
                              host bridge due to a hardware error.

**/
EFI_STATUS
EFIAPI
RootBridgeIoFlush (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This
  );

/**
   Gets the attributes that a PCI root bridge supports setting with SetAttributes(), and the
   attributes that a PCI root bridge is currently using.  

   The GetAttributes() function returns the mask of attributes that this PCI root bridge supports
   and the mask of attributes that the PCI root bridge is currently using.

   @param This        A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param Supported   A pointer to the mask of attributes that this PCI root bridge
                      supports setting with SetAttributes().
   @param Attributes  A pointer to the mask of attributes that this PCI root bridge is
                      currently using.
   
   @retval  EFI_SUCCESS           If Supports is not NULL, then the attributes that the PCI root
                                  bridge supports is returned in Supports. If Attributes is
                                  not NULL, then the attributes that the PCI root bridge is currently
                                  using is returned in Attributes.
   @retval  EFI_INVALID_PARAMETER Both Supports and Attributes are NULL.

**/
EFI_STATUS
EFIAPI
RootBridgeIoGetAttributes (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  OUT UINT64                           *Supported,
  OUT UINT64                           *Attributes
  );

/**
   Sets attributes for a resource range on a PCI root bridge.

   The SetAttributes() function sets the attributes specified in Attributes for the PCI root
   bridge on the resource range specified by ResourceBase and ResourceLength. Since the
   granularity of setting these attributes may vary from resource type to resource type, and from
   platform to platform, the actual resource range and the one passed in by the caller may differ. As a
   result, this function may set the attributes specified by Attributes on a larger resource range
   than the caller requested. The actual range is returned in ResourceBase and
   ResourceLength. The caller is responsible for verifying that the actual range for which the
   attributes were set is acceptable.

   @param[in]       This            A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]       Attributes      The mask of attributes to set. If the attribute bit
                                    MEMORY_WRITE_COMBINE, MEMORY_CACHED, or
                                    MEMORY_DISABLE is set, then the resource range is specified by
                                    ResourceBase and ResourceLength. If
                                    MEMORY_WRITE_COMBINE, MEMORY_CACHED, and
                                    MEMORY_DISABLE are not set, then ResourceBase and
                                    ResourceLength are ignored, and may be NULL.
   @param[in, out]  ResourceBase    A pointer to the base address of the resource range to be modified
                                    by the attributes specified by Attributes.
   @param[in, out]  ResourceLength  A pointer to the length of the resource range to be modified by the
                                    attributes specified by Attributes.
   
   @retval  EFI_SUCCESS     The current configuration of this PCI root bridge was returned in Resources.
   @retval  EFI_UNSUPPORTED The current configuration of this PCI root bridge could not be retrieved.
   @retval  EFI_INVALID_PARAMETER Invalid pointer of EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL

**/
EFI_STATUS
EFIAPI
RootBridgeIoSetAttributes (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN     UINT64                           Attributes,
  IN OUT UINT64                           *ResourceBase,
  IN OUT UINT64                           *ResourceLength 
  ); 

/**
   Retrieves the current resource settings of this PCI root bridge in the form of a set of ACPI 2.0
   resource descriptors.

   There are only two resource descriptor types from the ACPI Specification that may be used to
   describe the current resources allocated to a PCI root bridge. These are the QWORD Address
   Space Descriptor (ACPI 2.0 Section 6.4.3.5.1), and the End Tag (ACPI 2.0 Section 6.4.2.8). The
   QWORD Address Space Descriptor can describe memory, I/O, and bus number ranges for dynamic
   or fixed resources. The configuration of a PCI root bridge is described with one or more QWORD
   Address Space Descriptors followed by an End Tag.

   @param[in]   This        A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[out]  Resources   A pointer to the ACPI 2.0 resource descriptors that describe the
                            current configuration of this PCI root bridge. The storage for the
                            ACPI 2.0 resource descriptors is allocated by this function. The
                            caller must treat the return buffer as read-only data, and the buffer
                            must not be freed by the caller.
   
   @retval  EFI_SUCCESS     The current configuration of this PCI root bridge was returned in Resources.
   @retval  EFI_UNSUPPORTED The current configuration of this PCI root bridge could not be retrieved.
   @retval  EFI_INVALID_PARAMETER Invalid pointer of EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL

**/
EFI_STATUS
EFIAPI
RootBridgeIoConfiguration (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  OUT    VOID                             **Resources
  );

/**

  Initialize and create Pci Root Bridges for Board

  @param PrvateData       Driver instance of this Root Bridge IO protocol
  @param Protocol         Point to protocol instance
  @param Info 	          Point to Info field of this driver instance
  @param HostBridgeHandle Handle of host bridge
  @param Attri            Attribute of host bridge
  @param HostNo      	  HostNo for this Host Bridge
  @param Busno      	  Bus Number for the Host Bridge

  @retval EFI_SUCCESS Success to initialize the Pci Root Bridge.

**/
EFI_STATUS
PciRbInitialize (
  IN PCI_ROOT_BRIDGE_INSTANCE          *PrivateData,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL    *Protocol,
  IN struct LsPcieInfo     		*Info,
  IN EFI_HANDLE                         HostBridgeHandle,
  IN UINT64                             Attri,
  IN INTN				HostNo,
  IN INTN				Busno
  )
{
  EFI_STATUS                        Status;
  struct LsPcie *Pcie;
  PciDevT Pdev = ((Busno) << 16 | (0) << 11 | (0) << 8);
  INTN LinkUp, EpMode;
  UINT8 HeaderType;
  UINT16 Temp16;
  UINT16 VendorID16;
  UINT16 DeviceID16;

  Pcie = PrivateData->Pcie;

  PrivateData->FirstBusno = Busno;
  PrivateData->Pcie->Dbi = (VOID *)Info->Regs;
  PrivateData->Pcie->VaCfg0 = (VOID *)Info->Cfg0Phys;
  PrivateData->Pcie->VaCfg1 = (VOID *)Info->Cfg1Phys;

  /* outbound memory */
  PciSetRegion(&PrivateData->Regions[0],
       (PciSizeT)Info->MemBus,
       (PhysSizeT)Info->MemPhys,
       (PciSizeT)Info->MemSize,
       LS_PCI_REGION_MEM);

  /* outbound io */
  PciSetRegion(&PrivateData->Regions[1],
       (PciSizeT)Info->IoBus,
       (PhysSizeT)Info->IoPhys,
       (PciSizeT)Info->IoSize,
       LS_PCI_REGION_IO);

  /* System memory space */
  PciSetRegion(&PrivateData->Regions[2],
       CONFIG_SYS_LS_PCI_MEMORY_BUS,
       CONFIG_SYS_LS_PCI_MEMORY_PHYS,
       CONFIG_SYS_LS_PCI_MEMORY_SIZE,
       LS_PCI_REGION_SYS_MEMORY);

  PrivateData->RegionCnt = 3;

  PcieReadConfigWord(PrivateData, Pdev, LS_PCI_VENDOR_ID, (UINT16 *)&VendorID16);
  DEBUG((EFI_D_RELEASE,"PCIe:VendorID: %04lx\n", VendorID16));
  PcieReadConfigWord(PrivateData, Pdev, LS_PCI_DEVICE_ID, (UINT16 *)&DeviceID16);
  DEBUG((EFI_D_RELEASE,"PCIe Device ID: %04lx\n", DeviceID16));
  PcieReadConfigByte(PrivateData, Pdev, LS_PCI_HEADER_TYPE, (UINT8 *)&HeaderType);
  DEBUG((EFI_D_RELEASE,"PCIe Header Type: %02lx\n", HeaderType));
  EpMode = (HeaderType & 0x7f) == LS_PCI_HEADER_TYPE_NORMAL;
  DEBUG((EFI_D_INFO,"EpMode: %d\n", EpMode));
  DEBUG((EFI_D_RELEASE,"PCIe%d: %a\n", (UINT64)Info->PciNum, EpMode ? "Endpoint" : "Root Complex"));

  LinkUp = PcieLinkUp(Pcie);

  if (!LinkUp) {
    /* Let the user know there's no PCIe link */
    DEBUG((EFI_D_ERROR,"no link, regs @ 0x%lx\n", Info->Regs));
	   PrivateData->LastBusno = PrivateData->FirstBusno;
    return EFI_ABORTED;
  }

  DEBUG((EFI_D_INFO, "Passed Linkup Phase\n"));
  /* Print the negotiated PCIe link width */
  PcieReadConfigWord(PrivateData, Pdev, LS_PCIE_LINK_STA, (UINT16 *)&Temp16);
  DEBUG((EFI_D_RELEASE,"PCIe Link Status: %04lx\n", Temp16));
  DEBUG((EFI_D_RELEASE,"x%d gen%d, regs @ 0x%lx\n", (Temp16 & 0x3f0) >> 4,
	 (Temp16 & 0xf), Info->Regs));

  if (EpMode) {
    DEBUG((EFI_D_RELEASE, "EpMode: %d\n", Busno));
    return Busno;
  }

  if (HostNo == 1) {
    PrivateData->PciBaseAddress64 = PcdGet64 (PcdPci1ExpressBaseAddress); 
    if (!PrivateData->PciBaseAddress64) {
      DEBUG ((EFI_D_ERROR, "%a: PCI 1 host bridge not present\n", __FUNCTION__));
      return EFI_ABORTED;
    }

    PcieSetupCntrl(PrivateData, PrivateData->Pcie, PrivateData->Info);
  } else if (HostNo == 2 ) { 
    PrivateData->PciBaseAddress64 = PcdGet64 (PcdPci2ExpressBaseAddress); 
    if (!PrivateData->PciBaseAddress64) {
      DEBUG ((EFI_D_ERROR, "%a: PCI 2 host bridge not present\n", __FUNCTION__));
      return EFI_ABORTED;
    }

    PcieSetupCntrl(PrivateData, PrivateData->Pcie, PrivateData->Info);
  } else if (HostNo == 3 ) { 
    PrivateData->PciBaseAddress64 = PcdGet64 (PcdPci3ExpressBaseAddress); 
    if (!PrivateData->PciBaseAddress64) {
      DEBUG ((EFI_D_ERROR, "%a: PCI 3 host bridge not present\n", __FUNCTION__));
      return EFI_ABORTED;
    }

    PcieSetupCntrl(PrivateData, PrivateData->Pcie, PrivateData->Info);
  }

  PrivateData->BusStart  = FixedPcdGet32 (PcdPciBusMin);
  PrivateData->BusLength = FixedPcdGet32 (PcdPciBusMax) - FixedPcdGet32 (PcdPciBusMin) + 1;
  
  PrivateData->RootBridgeAttrib = Attri;

  PrivateData->Supports    = EFI_PCI_ATTRIBUTE_IDE_PRIMARY_IO | EFI_PCI_ATTRIBUTE_IDE_SECONDARY_IO | \
                             EFI_PCI_ATTRIBUTE_ISA_IO_16 | EFI_PCI_ATTRIBUTE_ISA_MOTHERBOARD_IO | \
                             EFI_PCI_ATTRIBUTE_VGA_MEMORY | \
                             EFI_PCI_ATTRIBUTE_VGA_IO_16  | EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO_16;
  PrivateData->Attributes  = PrivateData->Supports;

  Protocol->ParentHandle   = HostBridgeHandle;

  Protocol->PollMem        = RootBridgeIoPollMem;
  Protocol->PollIo         = RootBridgeIoPollIo;

  Protocol->Mem.Read       = RootBridgeIoMemRead;
  Protocol->Mem.Write      = RootBridgeIoMemWrite;

  Protocol->Io.Read        = RootBridgeIoIoRead;
  Protocol->Io.Write       = RootBridgeIoIoWrite;

  Protocol->CopyMem        = RootBridgeIoCopyMem;

  Protocol->Pci.Read       = RootBridgeIoPciRead;
  Protocol->Pci.Write      = RootBridgeIoPciWrite;

  Protocol->Map            = RootBridgeIoMap;
  Protocol->Unmap          = RootBridgeIoUnmap;

  Protocol->AllocateBuffer = RootBridgeIoAllocateBuffer;
  Protocol->FreeBuffer     = RootBridgeIoFreeBuffer;

  Protocol->Flush          = RootBridgeIoFlush;

  Protocol->GetAttributes  = RootBridgeIoGetAttributes;
  Protocol->SetAttributes  = RootBridgeIoSetAttributes;

  Protocol->Configuration  = RootBridgeIoConfiguration;

  Protocol->SegmentNumber  = HostNo;

  Status = gBS->LocateProtocol (&gEfiMetronomeArchProtocolGuid, NULL, (VOID **)&mMetronome);
  DEBUG ((EFI_D_INFO, "PCI : Locate MetronomeArch protocol\n"));
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}


/**
   Polls an address in memory mapped I/O space until an exit condition is met, or 
   a timeout occurs. 

   This function provides a standard way to poll a PCI memory location. A PCI memory read
   operation is performed at the PCI memory address specified by Address for the width specified
   by Width. The result of this PCI memory read operation is stored in Result. This PCI memory
   read operation is repeated until either a timeout of Delay 100 ns units has expired, or (Result &
   Mask) is equal to Value.

   @param[in]   This      A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width     Signifies the width of the memory operations.
   @param[in]   Address   The base address of the memory operations. The caller is
                          responsible for aligning Address if required.
   @param[in]   Mask      Mask used for the polling criteria. Bytes above Width in Mask
                          are ignored. The bits in the bytes below Width which are zero in
                          Mask are ignored when polling the memory address.
   @param[in]   Value     The comparison value used for the polling exit criteria.
   @param[in]   Delay     The number of 100 ns units to poll. Note that timer available may
                          be of poorer granularity.
   @param[out]  Result    Pointer to the last value read from the memory location.
   
   @retval EFI_SUCCESS            The last data returned from the access matched the poll exit criteria.
   @retval EFI_INVALID_PARAMETER  Width is invalid.
   @retval EFI_INVALID_PARAMETER  Result is NULL.
   @retval EFI_TIMEOUT            Delay expired before a match occurred.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoPollMem ( 
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN  UINT64                                 Address,
  IN  UINT64                                 Mask,
  IN  UINT64                                 Value,
  IN  UINT64                                 Delay,
  OUT UINT64                                 *Result
  )
{
  EFI_STATUS                      Status;
  UINT64                          NumberOfTicks;
  UINT32                          Remainder;

  if (Result == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Width > EfiPciWidthUint64) {
    return EFI_INVALID_PARAMETER;
  }

  // No matter what, always do a single poll.
  Status = This->Mem.Read (This, Width, Address, 1, Result);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if ((*Result & Mask) == Value) {
    return EFI_SUCCESS;
  }

  if (Delay == 0) {
    return EFI_SUCCESS;
  }

  NumberOfTicks = DivU64x32Remainder (Delay, (UINT32) mMetronome->TickPeriod, &Remainder);
  if (Remainder != 0) {
    NumberOfTicks += 1;
  }
  NumberOfTicks += 1;

  while (NumberOfTicks) {
    mMetronome->WaitForTick (mMetronome, 1);

    Status = This->Mem.Read (This, Width, Address, 1, Result);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if ((*Result & Mask) == Value) {
      return EFI_SUCCESS;
    }

    NumberOfTicks -= 1;
  }

  return EFI_TIMEOUT;
}
  
/**
   Reads from the I/O space of a PCI Root Bridge. Returns when either the polling exit criteria is
   satisfied or after a defined duration.

   This function provides a standard way to poll a PCI I/O location. A PCI I/O read operation is
   performed at the PCI I/O address specified by Address for the width specified by Width.
   The result of this PCI I/O read operation is stored in Result. This PCI I/O read operation is
   repeated until either a timeout of Delay 100 ns units has expired, or (Result & Mask) is equal
   to Value.

   @param[in] This      A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in] Width     Signifies the width of the I/O operations.
   @param[in] Address   The base address of the I/O operations. The caller is responsible
                        for aligning Address if required.
   @param[in] Mask      Mask used for the polling criteria. Bytes above Width in Mask
                        are ignored. The bits in the bytes below Width which are zero in
                        Mask are ignored when polling the I/O address.
   @param[in] Value     The comparison value used for the polling exit criteria.
   @param[in] Delay     The number of 100 ns units to poll. Note that timer available may
                        be of poorer granularity.
   @param[out] Result   Pointer to the last value read from the memory location.
   
   @retval EFI_SUCCESS            The last data returned from the access matched the poll exit criteria.
   @retval EFI_INVALID_PARAMETER  Width is invalid.
   @retval EFI_INVALID_PARAMETER  Result is NULL.
   @retval EFI_TIMEOUT            Delay expired before a match occurred.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoPollIo ( 
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN  UINT64                                 Address,
  IN  UINT64                                 Mask,
  IN  UINT64                                 Value,
  IN  UINT64                                 Delay,
  OUT UINT64                                 *Result
  )
{
  EFI_STATUS                      Status;
  UINT64                          NumberOfTicks;
  UINT32                          Remainder;

  if (Result == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Width > EfiPciWidthUint64) {
    return EFI_INVALID_PARAMETER;
  }

  // No matter what, always do a single poll.
  Status = This->Io.Read (This, Width, Address, 1, Result);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if ((*Result & Mask) == Value) {
    return EFI_SUCCESS;
  }

  if (Delay == 0) {
    return EFI_SUCCESS;
  }

  NumberOfTicks = DivU64x32Remainder (Delay, (UINT32) mMetronome->TickPeriod, &Remainder);
  if (Remainder != 0) {
    NumberOfTicks += 1;
  }
  NumberOfTicks += 1;

  while (NumberOfTicks) {
    mMetronome->WaitForTick (mMetronome, 1);

    Status = This->Io.Read (This, Width, Address, 1, Result);
    if (EFI_ERROR (Status)) {
        return Status;
    }

    if ((*Result & Mask) == Value) {
        return EFI_SUCCESS;
    }

    NumberOfTicks -= 1;
  }

  return EFI_TIMEOUT;
}


/**
   Enables a PCI driver to access PCI controller registers in the PCI root bridge memory space.

   The Mem.Read(), and Mem.Write() functions enable a driver to access PCI controller
   registers in the PCI root bridge memory space.
   The memory operations are carried out exactly as requested. The caller is responsible for satisfying
   any alignment and memory width restrictions that a PCI Root Bridge on a platform might require.

   @param[in]   This      A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width     Signifies the width of the memory operation.
   @param[in]   Address   The base address of the memory operation. The caller is
                          responsible for aligning the Address if required.
   @param[in]   Count     The number of memory operations to perform. Bytes moved is
                          Width size * Count, starting at Address.
   @param[out]  Buffer    For read operations, the destination buffer to store the results. For
                          write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS            The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoMemRead (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  OUT    VOID                                   *Buffer
  )
{
  return RootBridgeIoMemRW (This, FALSE, Width, Address, Count, Buffer);
}

/**
   Enables a PCI driver to access PCI controller registers in the PCI root bridge memory space.

   The Mem.Read(), and Mem.Write() functions enable a driver to access PCI controller
   registers in the PCI root bridge memory space.
   The memory operations are carried out exactly as requested. The caller is responsible for satisfying
   any alignment and memory width restrictions that a PCI Root Bridge on a platform might require.

   @param[in]   This      A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width     Signifies the width of the memory operation.
   @param[in]   Address   The base address of the memory operation. The caller is
                          responsible for aligning the Address if required.
   @param[in]   Count     The number of memory operations to perform. Bytes moved is
                          Width size * Count, starting at Address.
   @param[in]   Buffer    For read operations, the destination buffer to store the results. For
                          write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS            The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.
**/
EFI_STATUS
EFIAPI
RootBridgeIoMemWrite (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN     VOID                                   *Buffer
  )
{
  return RootBridgeIoMemRW (This, TRUE, Width, Address, Count, Buffer);  
}

/**
   Enables a PCI driver to access PCI controller registers in the PCI root bridge I/O space.

   @param[in]   This        A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width       Signifies the width of the memory operations.
   @param[in]   Address     The base address of the I/O operation. The caller is responsible for
                            aligning the Address if required.
   @param[in]   Count       The number of I/O operations to perform. Bytes moved is Width
                            size * Count, starting at Address.
   @param[out]  Buffer      For read operations, the destination buffer to store the results. For
                            write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS              The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER    Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER    Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoIoRead (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  OUT    VOID                                   *Buffer
  )
{
  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Width >= EfiPciWidthMaximum) {
    return EFI_INVALID_PARAMETER;
  }

  return RootBridgeIoIoRW (This, FALSE, Width, Address, Count, Buffer);  
}

/**
   Enables a PCI driver to access PCI controller registers in the PCI root bridge I/O space.

   @param[in]   This        A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width       Signifies the width of the memory operations.
   @param[in]   Address     The base address of the I/O operation. The caller is responsible for
                            aligning the Address if required.
   @param[in]   Count       The number of I/O operations to perform. Bytes moved is Width
                            size * Count, starting at Address.
   @param[in]   Buffer       For read operations, the destination buffer to store the results. For
                            write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS              The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER    Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER    Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoIoWrite (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL         *This,
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH   Width,
  IN       UINT64                                  Address,
  IN       UINTN                                   Count,
  IN       VOID                                    *Buffer
  )
{
  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Width >= EfiPciWidthMaximum) {
    return EFI_INVALID_PARAMETER;
  }

  return RootBridgeIoIoRW (This, TRUE, Width, Address, Count, Buffer);  
}

/**
   Enables a PCI driver to copy one region of PCI root bridge memory space to another region of PCI
   root bridge memory space.

   The CopyMem() function enables a PCI driver to copy one region of PCI root bridge memory
   space to another region of PCI root bridge memory space. This is especially useful for video scroll
   operation on a memory mapped video buffer.
   The memory operations are carried out exactly as requested. The caller is responsible for satisfying
   any alignment and memory width restrictions that a PCI root bridge on a platform might require.

   @param[in] This        A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL instance.
   @param[in] Width       Signifies the width of the memory operations.
   @param[in] DestAddress The destination address of the memory operation. The caller is
                          responsible for aligning the DestAddress if required.
   @param[in] SrcAddress  The source address of the memory operation. The caller is
                          responsible for aligning the SrcAddress if required.
   @param[in] Count       The number of memory operations to perform. Bytes moved is
                          Width size * Count, starting at DestAddress and SrcAddress.
   
   @retval  EFI_SUCCESS             The data was copied from one memory region to another memory region.
   @retval  EFI_INVALID_PARAMETER   Width is invalid for this PCI root bridge.
   @retval  EFI_OUT_OF_RESOURCES    The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoCopyMem (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL              *This,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH        Width,
  IN UINT64                                       DestAddress,
  IN UINT64                                       SrcAddress,
  IN UINTN                                        Count
  )
{
  EFI_STATUS  Status;
  BOOLEAN     Direction;
  UINTN       Stride;
  UINTN       Index;
  UINT64      Result;

  if (Width > EfiPciWidthUint64) {
    return EFI_INVALID_PARAMETER;
  }

  if (DestAddress == SrcAddress) {
    return EFI_SUCCESS;
  }

  Stride = (UINTN)(1 << Width);

  Direction = TRUE;
  if ((DestAddress > SrcAddress) && (DestAddress < (SrcAddress + Count * Stride))) {
    Direction   = FALSE;
    SrcAddress  = SrcAddress  + (Count-1) * Stride;
    DestAddress = DestAddress + (Count-1) * Stride;
  }

  for (Index = 0; Index < Count; Index++) {
    Status = RootBridgeIoMemRead (
               This,
               Width,
               SrcAddress,
               1,
               &Result
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = RootBridgeIoMemWrite (
               This,
               Width,
               DestAddress,
               1,
               &Result
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (Direction) {
      SrcAddress  += Stride;
      DestAddress += Stride;
    } else {
      SrcAddress  -= Stride;
      DestAddress -= Stride;
    }
  }
  return EFI_SUCCESS;
}

/**
   Enables a PCI driver to access PCI controller registers in a PCI root bridge's configuration space.

   The Pci.Read() and Pci.Write() functions enable a driver to access PCI configuration
   registers for a PCI controller.
   The PCI Configuration operations are carried out exactly as requested. The caller is responsible for
   any alignment and PCI configuration width issues that a PCI Root Bridge on a platform might
   require.

   @param[in]   This      A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width     Signifies the width of the memory operations.
   @param[in]   Address   The address within the PCI configuration space for the PCI controller.
   @param[in]   Count     The number of PCI configuration operations to perform. Bytes
                          moved is Width size * Count, starting at Address.
   @param[out]  Buffer    For read operations, the destination buffer to store the results. For
                          write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS            The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoPciRead (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN       UINT64                                 Address,
  IN       UINTN                                  Count,
  OUT      VOID                                   *Buffer
  )
{
  return RootBridgeIoPciRW (This, FALSE, Width, Address, Count, Buffer);
}

/**
   Enables a PCI driver to access PCI controller registers in a PCI root bridge's configuration space.

   The Pci.Read() and Pci.Write() functions enable a driver to access PCI configuration
   registers for a PCI controller.
   The PCI Configuration operations are carried out exactly as requested. The caller is responsible for
   any alignment and PCI configuration width issues that a PCI Root Bridge on a platform might
   require.

   @param[in]   This      A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width     Signifies the width of the memory operations.
   @param[in]   Address   The address within the PCI configuration space for the PCI controller.
   @param[in]   Count     The number of PCI configuration operations to perform. Bytes
                          moved is Width size * Count, starting at Address.
   @param[in]   Buffer    For read operations, the destination buffer to store the results. For
                          write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS            The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoPciWrite (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN       UINT64                                 Address,
  IN       UINTN                                  Count,
  IN       VOID                                   *Buffer
  )
{
  return RootBridgeIoPciRW (This, TRUE, Width, Address, Count, Buffer);
}

/**
   Provides the PCI controller-specific addresses required to access system memory from a
   DMA bus master.

   The Map() function provides the PCI controller specific addresses needed to access system
   memory. This function is used to map system memory for PCI bus master DMA accesses.

   @param[in]       This            A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]       Operation       Indicates if the bus master is going to read or write to system memory.
   @param[in]       HostAddress     The system memory address to map to the PCI controller.
   @param[in, out]  NumberOfBytes   On input the number of bytes to map. On output the number of bytes that were mapped.
   @param[out]      DeviceAddress   The resulting map address for the bus master PCI controller to use
                                    to access the system memory's HostAddress.
   @param[out]      Mapping         The value to pass to Unmap() when the bus master DMA operation is complete.
   
   @retval EFI_SUCCESS            The range was mapped for the returned NumberOfBytes.
   @retval EFI_INVALID_PARAMETER  Operation is invalid.
   @retval EFI_INVALID_PARAMETER  HostAddress is NULL.
   @retval EFI_INVALID_PARAMETER  NumberOfBytes is NULL.
   @retval EFI_INVALID_PARAMETER  DeviceAddress is NULL.
   @retval EFI_INVALID_PARAMETER  Mapping is NULL.
   @retval EFI_UNSUPPORTED        The HostAddress cannot be mapped as a common buffer.
   @retval EFI_DEVICE_ERROR       The system hardware could not map the requested address.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoMap (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL            *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION  Operation,
  IN     VOID                                       *HostAddress,
  IN OUT UINTN                                      *NumberOfBytes,
  OUT    EFI_PHYSICAL_ADDRESS                       *DeviceAddress,
  OUT    VOID                                       **Mapping
  )
{
  EFI_STATUS            Status;
  EFI_PHYSICAL_ADDRESS  PhysicalAddress;
  MAP_INFO              *MapInfo;

  if (HostAddress == NULL || NumberOfBytes == NULL || DeviceAddress == NULL || Mapping == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Initialize the return values to their defaults
  //
  *Mapping = NULL;

  //
  // Make sure that Operation is valid
  //
  if ((UINT32)Operation >= EfiPciOperationMaximum) {
    return EFI_INVALID_PARAMETER;
  }

  if ((*NumberOfBytes) > 0x10000000ULL) {
    DEBUG((EFI_D_INFO, "Cannot map more than 256MB at one transaction\n"));
    return EFI_INVALID_PARAMETER;
  }

  //
  // If any part of the DMA transfer being mapped is above 4GB, then
  // map the DMA transfer to a buffer below 4GB
  //
  PhysicalAddress = (EFI_PHYSICAL_ADDRESS) (UINTN) HostAddress;
  if ((PhysicalAddress + *NumberOfBytes) > 0x100000000ULL) {

    //
    // Common Buffer operations can not be remapped.  If the common buffer
    // if above 4GB, then it is not possible to generate a mapping, so return
    // an error.
    //
    if (Operation == EfiPciOperationBusMasterCommonBuffer || Operation == EfiPciOperationBusMasterCommonBuffer64) {
      return EFI_UNSUPPORTED;
    }

    //
    // Allocate a MAP_INFO structure to remember the mapping when Unmap() is
    // called later.
    //
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    sizeof(MAP_INFO),
                    (VOID **)&MapInfo
                    );
    if (EFI_ERROR (Status)) {
      *NumberOfBytes = 0;
      return Status;
    }

    //
    // Return a pointer to the MAP_INFO structure in Mapping
    //
    *Mapping = MapInfo;

    //
    // Initialize the MAP_INFO structure
    //
    MapInfo->Operation         = Operation;
    MapInfo->NumberOfBytes     = *NumberOfBytes;
    MapInfo->NumberOfPages     = EFI_SIZE_TO_PAGES(*NumberOfBytes);
    MapInfo->HostAddress       = PhysicalAddress;
    MapInfo->MappedHostAddress = 0x00000000ffffffff;

    //
    // Allocate a buffer below 4GB to map the transfer to.
    //
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiBootServicesData,
                    MapInfo->NumberOfPages,
                    &MapInfo->MappedHostAddress
                    );
    if (EFI_ERROR (Status)) {
      gBS->FreePool (MapInfo);
      *NumberOfBytes = 0;
      return Status;
    }

    //
    // If this is a read operation from the Bus Master's point of view,
    // then copy the contents of the real buffer into the mapped buffer
    // so the Bus Master can read the contents of the real buffer.
    //
    if (Operation == EfiPciOperationBusMasterRead || Operation == EfiPciOperationBusMasterRead64) {
      CopyMem (
        (VOID *)(UINTN)MapInfo->MappedHostAddress,
        (VOID *)(UINTN)MapInfo->HostAddress,
        MapInfo->NumberOfBytes
        );
    }

    //
    // The DeviceAddress is the address of the maped buffer below 4GB
    //
    *DeviceAddress = MapInfo->MappedHostAddress;
  } else {
    //
    // The transfer is below 4GB, so the DeviceAddress is simply the HostAddress
    //
    *DeviceAddress = PhysicalAddress;
  }

  return EFI_SUCCESS;
}

/**
   Completes the Map() operation and releases any corresponding resources.

   The Unmap() function completes the Map() operation and releases any corresponding resources.
   If the operation was an EfiPciOperationBusMasterWrite or
   EfiPciOperationBusMasterWrite64, the data is committed to the target system memory.
   Any resources used for the mapping are freed.  

   @param[in] This      A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in] Mapping   The mapping value returned from Map().
   
   @retval EFI_SUCCESS            The range was unmapped.
   @retval EFI_INVALID_PARAMETER  Mapping is not a value that was returned by Map().
   @retval EFI_DEVICE_ERROR       The data was not committed to the target system memory.

**/
EFI_STATUS
EFIAPI
RootBridgeIoUnmap (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN VOID                             *Mapping
  )
{
  MAP_INFO    *MapInfo;

  //
  // See if the Map() operation associated with this Unmap() required a mapping buffer.
  // If a mapping buffer was not required, then this function simply returns EFI_SUCCESS.
  //
  if (Mapping != NULL) {
    //
    // Get the MAP_INFO structure from Mapping
    //
    MapInfo = (MAP_INFO *)Mapping;

    //
    // If this is a write operation from the Bus Master's point of view,
    // then copy the contents of the mapped buffer into the real buffer
    // so the processor can read the contents of the real buffer.
    //
    if (MapInfo->Operation == EfiPciOperationBusMasterWrite || MapInfo->Operation == EfiPciOperationBusMasterWrite64) {
      CopyMem (
        (VOID *)(UINTN)MapInfo->HostAddress,
        (VOID *)(UINTN)MapInfo->MappedHostAddress,
        MapInfo->NumberOfBytes
        );
    }

    //
    // Free the mapped buffer and the MAP_INFO structure.
    //
    gBS->FreePages (MapInfo->MappedHostAddress, MapInfo->NumberOfPages);
    gBS->FreePool (Mapping);
  }
  return EFI_SUCCESS;
}


/**
   Allocates pages that are suitable for an EfiPciOperationBusMasterCommonBuffer or
   EfiPciOperationBusMasterCommonBuffer64 mapping.
  
   @param This        A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param Type        This parameter is not used and must be ignored.
   @param MemoryType  The type of memory to allocate, EfiBootServicesData or EfiRuntimeServicesData.
   @param Pages       The number of pages to allocate.
   @param HostAddress A pointer to store the base system memory address of the allocated range.
   @param Attributes  The requested bit mask of attributes for the allocated range. Only
                      the attributes EFI_PCI_ATTRIBUTE_MEMORY_WRITE_COMBINE, EFI_PCI_ATTRIBUTE_MEMORY_CACHED, 
                      and EFI_PCI_ATTRIBUTE_DUAL_ADDRESS_CYCLE may be used with this function.
   
   @retval EFI_SUCCESS            The requested memory pages were allocated.
   @retval EFI_INVALID_PARAMETER  MemoryType is invalid.
   @retval EFI_INVALID_PARAMETER  HostAddress is NULL.
   @retval EFI_UNSUPPORTED        Attributes is unsupported. The only legal attribute bits are
                                  MEMORY_WRITE_COMBINE, MEMORY_CACHED, and DUAL_ADDRESS_CYCLE.
   @retval EFI_OUT_OF_RESOURCES   The memory pages could not be allocated.

**/
EFI_STATUS
EFIAPI
RootBridgeIoAllocateBuffer (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  EFI_ALLOCATE_TYPE                Type,
  IN  EFI_MEMORY_TYPE                  MemoryType,
  IN  UINTN                            Pages,
  OUT VOID                             **HostAddress,
  IN  UINT64                           Attributes
  )
{
  EFI_STATUS            Status;
  EFI_PHYSICAL_ADDRESS  PhysicalAddress;

  //
  // Validate Attributes
  //
  if ((Attributes & EFI_PCI_ATTRIBUTE_INVALID_FOR_ALLOCATE_BUFFER) != 0) {
    return EFI_UNSUPPORTED;
  }

  //
  // Check for invalid inputs
  //
  if (HostAddress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // The only valid memory types are EfiBootServicesData and EfiRuntimeServicesData
  //
  if (MemoryType != EfiBootServicesData && MemoryType != EfiRuntimeServicesData) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Limit allocations to memory below 4GB
  //
  PhysicalAddress = (EFI_PHYSICAL_ADDRESS)(0xffffffff);

  Status = gBS->AllocatePages (AllocateMaxAddress, MemoryType, Pages, &PhysicalAddress);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *HostAddress = (VOID *)(UINTN)PhysicalAddress;

  return EFI_SUCCESS;
}

/**
   Frees memory that was allocated with AllocateBuffer().

   The FreeBuffer() function frees memory that was allocated with AllocateBuffer().

   @param This        A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param Pages       The number of pages to free.
   @param HostAddress The base system memory address of the allocated range.
   
   @retval EFI_SUCCESS            The requested memory pages were freed.
   @retval EFI_INVALID_PARAMETER  The memory range specified by HostAddress and Pages
                                  was not allocated with AllocateBuffer().

**/
EFI_STATUS
EFIAPI
RootBridgeIoFreeBuffer (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  UINTN                            Pages,
  OUT VOID                             *HostAddress
  )
{

  return gBS->FreePages ((EFI_PHYSICAL_ADDRESS) (UINTN) HostAddress, Pages);
}

/**
   Flushes all PCI posted write transactions from a PCI host bridge to system memory.

   The Flush() function flushes any PCI posted write transactions from a PCI host bridge to system
   memory. Posted write transactions are generated by PCI bus masters when they perform write
   transactions to target addresses in system memory.
   This function does not flush posted write transactions from any PCI bridges. A PCI controller
   specific action must be taken to guarantee that the posted write transactions have been flushed from
   the PCI controller and from all the PCI bridges into the PCI host bridge. This is typically done with
   a PCI read transaction from the PCI controller prior to calling Flush().

   @param This        A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   
   @retval EFI_SUCCESS        The PCI posted write transactions were flushed from the PCI host
                              bridge to system memory.
   @retval EFI_DEVICE_ERROR   The PCI posted write transactions were not flushed from the PCI
                              host bridge due to a hardware error.

**/
EFI_STATUS
EFIAPI
RootBridgeIoFlush (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL           *This
  )
{
  //
  // not supported yet
  //
  return EFI_SUCCESS;
}

/**
   Gets the attributes that a PCI root bridge supports setting with SetAttributes(), and the
   attributes that a PCI root bridge is currently using.  

   The GetAttributes() function returns the mask of attributes that this PCI root bridge supports
   and the mask of attributes that the PCI root bridge is currently using.

   @param This        A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param Supported   A pointer to the mask of attributes that this PCI root bridge
                      supports setting with SetAttributes().
   @param Attributes  A pointer to the mask of attributes that this PCI root bridge is
                      currently using.
   
   @retval  EFI_SUCCESS           If Supports is not NULL, then the attributes that the PCI root
                                  bridge supports is returned in Supports. If Attributes is
                                  not NULL, then the attributes that the PCI root bridge is currently
                                  using is returned in Attributes.
   @retval  EFI_INVALID_PARAMETER Both Supports and Attributes are NULL.

**/
EFI_STATUS
EFIAPI
RootBridgeIoGetAttributes (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  OUT UINT64                           *Supported,
  OUT UINT64                           *Attributes
  )
{
  PCI_ROOT_BRIDGE_INSTANCE *PrivateData;

  PrivateData = DRIVER_INSTANCE_FROM_PCI_ROOT_BRIDGE_IO_THIS(This);

  if (Attributes == NULL && Supported == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Set the return value for Supported and Attributes
  //
  if (Supported != NULL) {
    *Supported  = PrivateData->Supports;
  }

  if (Attributes != NULL) {
    *Attributes = PrivateData->Attributes;
  }

  return EFI_SUCCESS;
}

/**
   Sets attributes for a resource range on a PCI root bridge.

   The SetAttributes() function sets the attributes specified in Attributes for the PCI root
   bridge on the resource range specified by ResourceBase and ResourceLength. Since the
   granularity of setting these attributes may vary from resource type to resource type, and from
   platform to platform, the actual resource range and the one passed in by the caller may differ. As a
   result, this function may set the attributes specified by Attributes on a larger resource range
   than the caller requested. The actual range is returned in ResourceBase and
   ResourceLength. The caller is responsible for verifying that the actual range for which the
   attributes were set is acceptable.

   @param[in]       This            A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]       Attributes      The mask of attributes to set. If the attribute bit
                                    MEMORY_WRITE_COMBINE, MEMORY_CACHED, or
                                    MEMORY_DISABLE is set, then the resource range is specified by
                                    ResourceBase and ResourceLength. If
                                    MEMORY_WRITE_COMBINE, MEMORY_CACHED, and
                                    MEMORY_DISABLE are not set, then ResourceBase and
                                    ResourceLength are ignored, and may be NULL.
   @param[in, out]  ResourceBase    A pointer to the base address of the resource range to be modified
                                    by the attributes specified by Attributes.
   @param[in, out]  ResourceLength  A pointer to the length of the resource range to be modified by the
                                    attributes specified by Attributes.
   
   @retval  EFI_SUCCESS     The current configuration of this PCI root bridge was returned in Resources.
   @retval  EFI_UNSUPPORTED The current configuration of this PCI root bridge could not be retrieved.
   @retval  EFI_INVALID_PARAMETER Invalid pointer of EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL

**/
EFI_STATUS
EFIAPI
RootBridgeIoSetAttributes (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN     UINT64                           Attributes,
  IN OUT UINT64                           *ResourceBase,
  IN OUT UINT64                           *ResourceLength 
  )
{
  PCI_ROOT_BRIDGE_INSTANCE            *PrivateData;

  PrivateData = DRIVER_INSTANCE_FROM_PCI_ROOT_BRIDGE_IO_THIS(This);

  if (Attributes != 0) {
    if ((Attributes & (~(PrivateData->Supports))) != 0) {
      return EFI_UNSUPPORTED;
    }
  }

  //
  // This is a generic driver for a PC-AT class system.  It does not have any
  // chipset specific knowlegde, so none of the attributes can be set or
  // cleared.  Any attempt to set attribute that are already set will succeed,
  // and any attempt to set an attribute that is not supported will fail.
  //
  if (Attributes & (~PrivateData->Attributes)) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
   Retrieves the current resource settings of this PCI root bridge in the form of a set of ACPI 2.0
   resource descriptors.

   There are only two resource descriptor types from the ACPI Specification that may be used to
   describe the current resources allocated to a PCI root bridge. These are the QWORD Address
   Space Descriptor (ACPI 2.0 Section 6.4.3.5.1), and the End Tag (ACPI 2.0 Section 6.4.2.8). The
   QWORD Address Space Descriptor can describe memory, I/O, and bus number ranges for dynamic
   or fixed resources. The configuration of a PCI root bridge is described with one or more QWORD
   Address Space Descriptors followed by an End Tag.

   @param[in]   This        A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[out]  Resources   A pointer to the ACPI 2.0 resource descriptors that describe the
                            current configuration of this PCI root bridge. The storage for the
                            ACPI 2.0 resource descriptors is allocated by this function. The
                            caller must treat the return buffer as read-only data, and the buffer
                            must not be freed by the caller.
   
   @retval  EFI_SUCCESS     The current configuration of this PCI root bridge was returned in Resources.
   @retval  EFI_UNSUPPORTED The current configuration of this PCI root bridge could not be retrieved.
   @retval  EFI_INVALID_PARAMETER Invalid pointer of EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL

**/
EFI_STATUS
EFIAPI
RootBridgeIoConfiguration (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *This,
  OUT VOID                                **Resources
)
{
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridge;
  UINTN                                 Index;

  RootBridge = DRIVER_INSTANCE_FROM_PCI_ROOT_BRIDGE_IO_THIS(This);

  for (Index = 0; Index < ResTypeMax; Index++) {
    //if (ResAlloc[Index].Length != 0) => Resource allocated
    if (RootBridge->ResAlloc[Index].Length != 0) {
      Configuration.SpaceDesp[Index].AddrRangeMin = RootBridge->ResAlloc[Index].Base;
      Configuration.SpaceDesp[Index].AddrRangeMax = RootBridge->ResAlloc[Index].Base + RootBridge->ResAlloc[Index].Length - 1;
      Configuration.SpaceDesp[Index].AddrLen      = RootBridge->ResAlloc[Index].Length;
    }
  }

  // Set up Configuration for the bus
  Configuration.SpaceDesp[Index].AddrRangeMin = RootBridge->BusStart;
  Configuration.SpaceDesp[Index].AddrLen      = RootBridge->BusLength;

  *Resources = &Configuration;
  return EFI_SUCCESS;
}
