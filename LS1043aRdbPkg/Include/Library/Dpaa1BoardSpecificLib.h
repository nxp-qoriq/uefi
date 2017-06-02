/** Dpaa1BoardSpecificLib.h
  DPAA1 Board Specific library interface

  Copyright (c) 2016, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __DPAA1_BOARD_SPECIFIC_LIB_H__
#define __DPAA1_BOARD_SPECIFIC_LIB_H__

#include <Library/Dpaa1EthernetMacLib.h>
#include <Library/Dpaa1EthernetPhyLib.h>
#include <Ls1043aSerDes.h>
#include <Uefi.h>

/**
 * Number of Ethernet PHY MDIO buses
 */
#define DPAA1_MDIO_BUSES_COUNT   2

#define RGMII_PHY1_ADDR			0x1
#define RGMII_PHY2_ADDR			0x2

#define QSGMII_PORT1_PHY_ADDR		0x4
#define QSGMII_PORT2_PHY_ADDR		0x5
#define QSGMII_PORT3_PHY_ADDR		0x6
#define QSGMII_PORT4_PHY_ADDR		0x7

#define FM1_10GEC1_PHY_ADDR			0x1

#define FMAN_FW_LENGTH			0x10000

typedef struct FdtPort {
    FMAN_MEMAC_ID  Id;
    PHY_INTERFACE_TYPE  PhyInterfaceType;
    UINTN  CompatAddress;
} FDT_PORT;

VOID
Dpaa1ParseSerDes(SERDES_LANE_PROTOCOL LaneProtocol,
                 VOID *Arg);
VOID
Dpaa1DiscoverFmanMemac(FMAN_MEMAC_ID MemacId,
                       PHY_INTERFACE_TYPE PhyInterfaceType,
                       VOID *Arg);

VOID
GetMemacIdAndPhyType(IN     SERDES_LANE_PROTOCOL LaneProtocol,
                     OUT    FMAN_MEMAC_ID        *MemacId,
                     IN OUT UINT8                *MemacIdCount,
                     OUT    PHY_INTERFACE_TYPE   *PhyInterfaceType);

BOOLEAN
IsMemacEnabled (
  IN  FMAN_MEMAC_ID MemacId
  );

extern DPAA1_PHY_MDIO_BUS gDpaa1MdioBuses[];

#endif /* __DPAA1_BOARD_SPECIFIC_LIB_H__ */
