/** Dpaa2BoardSpecificLib.h
  DPAA2 Board Specific library interface

  Copyright (c) 2016, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __DPAA2_BOARD_SPECIFIC_LIB_H__
#define __DPAA2_BOARD_SPECIFIC_LIB_H__

#include <Uefi.h>
#include <Ls2088aSerDes.h>
#include <Library/Dpaa2EthernetPhyLib.h>

/**
 * Number of Ethernet PHY MDIO buses
 */
#define DPAA2_MDIO_BUSES_COUNT   2

VOID
Dpaa2DiscoverWriopDpmac(SERDES_INSTANCE SerDesInstance,
                        SERDES_LANE_PROTOCOL LaneProtocol,
                        VOID *Arg);

extern DPAA2_PHY_MDIO_BUS gDpaa2MdioBuses[];

#endif /* __DPAA2_BOARD_SPECIFIC_LIB_H__ */
