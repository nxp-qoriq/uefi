/** @file  Dpaa2EthernetDxe.h

  DPAA2 Ethernet DXE driver private declarations

  Copyright (c) 2016, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __DPAA2_ETHERTNET_DXE_H__
#define __DPAA2_ETHERTNET_DXE_H__

#include <Library/UefiLib.h>
#include <Protocol/SimpleNetwork.h>
#include <Library/Dpaa2EthernetMacLib.h>
#include <Library/Dpaa2ManagementComplexLib.h>

#define DPAA2_ETHERNET_DRIVER_VERSION   0x1

/**
 * DPAA2 Ethernet Device Path
 */
typedef struct _DPAA2_DEVICE_PATH {
  MAC_ADDR_DEVICE_PATH      MacAddrDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  EndMarker;
} DPAA2_DEVICE_PATH;

/**
 * DPAA2 Ethernet Device Instance
 */
typedef struct _DPAA2_ETHERNET_DEVICE {
  /**
   * Signature for run-time type checking
   */
  UINT32 Signature;
# define DPAA2_ETHERNET_DEVICE_SIGNATURE SIGNATURE_32('D', 'P', 'A', 'A')

  /**
   * UEFI driver model handle associated with this device instance
   */
  EFI_HANDLE ControllerHandle;

  /**
   * SNP protocol instance
   */
  EFI_SIMPLE_NETWORK_PROTOCOL Snp;

  /**
   * SNP mode instance
   */
  EFI_SIMPLE_NETWORK_MODE SnpMode;

  /**
   * SNP statistics instance
   */
  EFI_NETWORK_STATISTICS Stats;

  /**
   * Device path protocol instance
   */
  DPAA2_DEVICE_PATH DevicePath;

  /**
   * Pointer to corresponding WRIOP DPMAC
   */
  WRIOP_DPMAC *WriopDpmac;

  /**
   * Flag indicating if the PHY for this Ethernet device has
   * already been in initialized.
   */
  BOOLEAN PhyInitialized;

  /**
   * DPAA2 network interface object made of
   * MC objects
   */
  DPAA2_NETWORK_INTERFACE Dpaa2NetInterface;

  /**
   * Node in the global list of DPAA2 Ethernet devices
   */
  LIST_ENTRY ListNode;

  /**
   * Head of the linked list of Tx frames in flight
   */
  LIST_ENTRY TxFramesInFlightList;

  /**
   * TPL-based lock to serialize access to TxFramesInFlightList
   */
  EFI_LOCK TxFramesInFlightLock;

} DPAA2_ETHERNET_DEVICE;

/**
 * Information kept for a Tx frame that is in flight
 */
typedef struct _DPAA2_TX_FRAME_IN_FLIGHT {
  /**
   * Signature for run-time type checking
   */
  UINT32 Signature;
# define DPAA2_TX_FRAME_IN_FLIGHT_SIGNATURE SIGNATURE_32('D', 'P', 'T', 'X')

  /**
   * Pointer to the UEFI networking stack's Tx buffer associated with the frame
   */
  VOID *UefiTxBuffer;

  /**
   * Address of QBMAN buffer ssociated with the frame
   */
  UINT64 QBmanTxBufferAddr;

  /**
   * Node in the list of Tx frames in flight for the corresponding Ethernet device
   */
  LIST_ENTRY ListNode;
} DPAA2_TX_FRAME_IN_FLIGHT;

#define SNP_TO_DPAA2_DEV(_SnpPtr) \
        CR(_SnpPtr, DPAA2_ETHERNET_DEVICE, Snp, DPAA2_ETHERNET_DEVICE_SIGNATURE)

#endif /* __DPAA2_ETHERTNET_DXE_H__ */

