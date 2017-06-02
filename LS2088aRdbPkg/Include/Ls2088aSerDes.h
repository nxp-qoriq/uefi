/** Ls2088aSerDes.h
 The Header file of SerDes Module

 Copyright (c) 2016, Freescale Semiconductor, Inc. All rights reserved.

 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution. The full text of the license may be found
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __LS2088A_SERDES_H
#define __LS2088A_SERDES_H

#include <LS2088aRdb.h>
#include <Common.h>

#define	SERDES_MAX_LANES	8

/**
 * SerDes lane protocols/devices
 */
typedef enum _SERDES_LANE_PROTOCOL {
  NONE = 0,
  PCIE1,
  PCIE2,
  PCIE3,
  PCIE4,
  SATA1,
  SATA2,
  XAUI1,
  XAUI2,
  XFI1,
  XFI2,
  XFI3,
  XFI4,
  XFI5,
  XFI6,
  XFI7,
  XFI8,
  SGMII1,
  SGMII2,
  SGMII3,
  SGMII4,
  SGMII5,
  SGMII6,
  SGMII7,
  SGMII8,
  SGMII9,
  SGMII10,
  SGMII11,
  SGMII12,
  SGMII13,
  SGMII14,
  SGMII15,
  SGMII16,
  QSGMII_A,
  QSGMII_B,
  QSGMII_C,
  QSGMII_D,
  /*
   * Number of entries in this enum
   */
  SERDES_LANE_PROTOCOLS_COUNT
} SERDES_LANE_PROTOCOL;

C_ASSERT(SERDES_LANE_PROTOCOLS_COUNT <= sizeof(UINT64) * 8);

/**
 * SerDes module instances
 */
typedef enum _SERDES_INSTANCES {
  SERDES_INSTANCE_1  = 0,
  SERDES_INSTANCE_2  = 1,

  /*
   * Number of entries in this enum
   */
  SERDES_INSTANCES_COUNT
} SERDES_INSTANCE;


VOID
GetSerdesProtocolMaps(
  OUT UINT64 *SerDes1ProtocolMapPtr,
  OUT UINT64 *SerDes2ProtocolMapPtr
);

BOOLEAN
IsSerDesLaneProtocolConfigured(
  IN UINT64 SerDes1ProtocolMap,
  IN UINT64 SerDes2ProtocolMap,
  IN SERDES_LANE_PROTOCOL Device
);

typedef VOID
SERDES_LANE_PROBE_CALLBACK(
  IN SERDES_INSTANCE SerDesInstance,
  IN SERDES_LANE_PROTOCOL LaneProtocol,
  IN VOID *Arg
);

VOID
SerDesProbeLanes(
  IN SERDES_LANE_PROBE_CALLBACK *SerDesLaneProbeCallback,
  IN VOID *Arg
);

#endif /* __LS2088A_SERDES_H */
